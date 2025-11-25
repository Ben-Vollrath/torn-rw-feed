#pragma once

#include "Room.hpp"
#include "dto/WarDto.hpp"
#include "oatpp/core/async/Coroutine.hpp"
#include "service/FactionService.hpp"
#include "service/FFScouterApiService.hpp"
#include "service/MemberStatsService.hpp"
#include "service/TornApiServiceKeyManaged.hpp"

class Fetcher : public oatpp::async::Coroutine<Fetcher>
{
	enum class Phase : std::uint8_t
	{
		WAR,
		ENEMIES,
		ALLIES
	};

	std::int64_t m_factionId;
	TornApiServiceKeyManaged m_tornApiService;
	std::weak_ptr<Room> m_room;
	std::chrono::microseconds m_interval;

	MemberStatsService m_memberStatsService;
	FFScouterApiService m_ffScouterApiService;

	std::int64_t m_count = 100;
	std::optional<std::int64_t> m_enemyFactionId;
	std::optional<std::int64_t> m_warId;


	const std::string TAG = "FETCHER";

	std::vector<Phase> m_cycle{Phase::WAR, Phase::ENEMIES, Phase::ALLIES};
	std::size_t m_phaseIndex = 0;

public:
	Fetcher(const std::int64_t factionId, const std::weak_ptr<Room>& room)
		: m_factionId(factionId)
		  , m_tornApiService(factionId)
		  , m_room(room)
		  , m_interval(m_tornApiService.getCooldown())
	{
	}

	Action act() override
	{
		auto room = m_room.lock();
		if (!room || room->isClosed())
		{
			OATPP_LOGD(TAG, "Room is closed, finishing fetcher.")
			return finish();
		}

		for (std::size_t attempts = 0; attempts < m_cycle.size(); ++attempts)
		{
			Phase phase = m_cycle[m_phaseIndex];

			switch (phase)
			{
			case Phase::WAR:
				advancePhase();
				return m_tornApiService.getFactionWar().callbackTo(&Fetcher::onFactionWarResponse);

			case Phase::ENEMIES:
				if (m_enemyFactionId)
				{
					OATPP_LOGD(TAG, "Fetching Faction Members.")
					advancePhase();
					return m_tornApiService.getFactionMembers(m_enemyFactionId.value())
					                       .callbackTo(&Fetcher::onEnemiesUpdate);
				}

				advancePhase();
				break;
			case Phase::ALLIES:
				advancePhase();
				return m_tornApiService.getFactionMembers(m_factionId).callbackTo(&Fetcher::onAlliesUpdate);
			}
		}

		return scheduleNextTick();
	}

	Action onFactionWarResponse(const oatpp::Object<TornFactionWarResponseDto>& factionWarResponse)
	{
		if (!factionWarResponse->isWarActive())
		{
			auto room = m_room.lock();
			if (room)
			{
				room->updateWar(factionWarResponse);
			}
			return scheduleNextTick();
		}

		if (factionWarResponse->isValidResponse(m_factionId))
		{
			auto enemyFactionId = factionWarResponse->getEnemyFactionId(m_factionId);
			bool isNewWar = factionWarResponse->getWarId() != m_warId;
			m_warId = factionWarResponse->getWarId();
			m_enemyFactionId = enemyFactionId;

			auto room = m_room.lock();
			if (room)
			{
				if (isNewWar)
				{
					//New war means new members -> reset old member state
					room->resetState();
				}

				room->updateWar(factionWarResponse);
			}
			m_count = 0;
		}
		else
		{
			//Key is outdated
			m_tornApiService.removeLastKey();
			m_count = 100;
		}
		return scheduleNextTick();
	}

	Action onEnemiesUpdate(const oatpp::Object<TornFactionMembersResponse>& memberInfo)
	{
		auto room = m_room.lock();
		if (!room || room->isClosed())
		{
			OATPP_LOGD(TAG, "Room is closed, finishing fetcher.")
			return finish();
		}

		room->updateEnemies(memberInfo);

		if (room->needStats())
		{
			auto stats = m_memberStatsService.getAllForWar(m_warId.value(), m_enemyFactionId.value());

			if (stats->empty())
			{
				return m_ffScouterApiService.getScout(memberInfo).callbackTo(&Fetcher::onScouts);
			}
			room->updateStats(stats);
		}

		return scheduleNextTick();
	}

	Action onAlliesUpdate(const oatpp::Object<TornFactionMembersResponse>& memberInfo)
	{
		auto room = m_room.lock();
		if (!room || room->isClosed())
		{
			OATPP_LOGD(TAG, "Room is closed, finishing fetcher.")
			return finish();
		}

		room->updateAllies(memberInfo);

		return scheduleNextTick();
	}

	Action onScouts(const FFScouterResponseDto& scouts)
	{
		auto stats = m_memberStatsService.createMany(
			MemberStatsDto::fromFFScouterResponse(m_warId.value(), m_enemyFactionId.value(), scouts));

		auto room = m_room.lock();
		if (room)
		{
			room->updateStats(stats);
		}
		return scheduleNextTick();
	}

	Action scheduleNextTick()
	{
		using namespace std::chrono;
		auto now = duration_cast<microseconds>(seconds(std::time(nullptr)));
		++m_count;
		return oatpp::async::Action::createWaitRepeatAction(now.count() + m_interval.count());
	}

	Action handleError(Error* e) override
	{
		OATPP_LOGE(TAG, "Fetcher error: %s", e ? e->what() : "unknown");

		return scheduleNextTick();
	}

private:
	void advancePhase()
	{
		m_phaseIndex = (m_phaseIndex + 1) % m_cycle.size();
	}
};
