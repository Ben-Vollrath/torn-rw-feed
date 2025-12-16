#pragma once

#include "Room.hpp"
#include "dto/WarDto.hpp"
#include "oatpp/core/async/Coroutine.hpp"
#include "service/FactionService.hpp"
#include "service/FFScouterApiService.hpp"
#include "service/MemberStatsService.hpp"
#include "service/TornApiServiceKeyManaged.hpp"
#include "oatpp/core/base/Environment.hpp"

class Fetcher : public oatpp::async::Coroutine<Fetcher>
{
	std::int64_t m_factionId;
	TornApiServiceKeyManaged m_tornApiService;
	std::weak_ptr<Room> m_room;
	std::chrono::microseconds m_interval;

	MemberStatsService m_memberStatsService;
	UserService m_userService;
	FFScouterApiService m_ffScouterApiService;

	std::optional<std::int64_t> m_enemyFactionId;
	std::optional<std::int64_t> m_warId;

	const std::string TAG = "FETCHER";

	std::chrono::microseconds now = std::chrono::microseconds(oatpp::base::Environment::getMicroTickCount());
	std::chrono::microseconds m_nextExecWarWithAllies{ now };
	std::chrono::microseconds m_nextExecEnemies{ now };

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
			OATPP_LOGD(TAG, "Act Room is closed, finishing fetcher.")
			return finish();
		}
		if (m_nextExecEnemies <= m_nextExecWarWithAllies)
		{
			setNextExecTimer(m_nextExecEnemies);
			if (m_enemyFactionId) {
					return m_tornApiService.getFactionMembers(m_enemyFactionId.value())
					.callbackTo(&Fetcher::onEnemiesUpdate);
			}

		}

		setNextExecTimer(m_nextExecWarWithAllies);
		return m_tornApiService.getFactionWarAndMembers().callbackTo(&Fetcher::onFactionWarAndMembersResponse);
	
	}

	Action onFactionWarAndMembersResponse(const oatpp::Object<TornFactionWarAndMembersResponseDto>& factionWarResponse)
	{
		if (factionWarResponse->basic->id != m_factionId) // Key is outdated (member moved to another faction)
		{
			m_userService.updateFaction(m_tornApiService.getLastKey()->torn_key, factionWarResponse->basic->id);
			m_tornApiService.removeLastKey();
			return scheduleNextIteration();
		}

		auto room = m_room.lock();
		if (!room || room->isClosed())
		{
			OATPP_LOGD(TAG, "Act Room is closed, finishing fetcher.")
				return finish();
		}

		bool isNewWar = factionWarResponse->getWarId() != m_warId;
		if (isNewWar)
		{
			//New war means new members -> reset old member state
			room->resetState();
		}

		m_warId = factionWarResponse->getWarId();
		m_enemyFactionId = factionWarResponse->getEnemyFactionId(m_factionId);
		room->updateWarAndAllies(factionWarResponse);

		if (!factionWarResponse->isWarActive())
		{
			auto room = m_room.lock();
			if (room)
			{
				room->updateWarAndAllies(factionWarResponse);
			}
			return scheduleNextIteration();
		}

		return scheduleNextIteration();
	}

	Action onEnemiesUpdate(const oatpp::Object<TornFactionMembersResponse>& memberInfo)
	{
		auto room = m_room.lock();
		if (!room || room->isClosed())
		{
			OATPP_LOGD(TAG, "Enemies, Room is closed, finishing fetcher.")
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

		return scheduleNextIteration();
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
		return scheduleNextIteration();
	}

	Action scheduleNextIteration()
	{
		return oatpp::async::Action::createWaitRepeatAction(
			std::min(m_nextExecEnemies.count(), m_nextExecWarWithAllies.count())
		);
	}

	Action handleError(Error* e) override
	{
		using clock = std::chrono::microseconds;

		const auto now = clock{ oatpp::base::Environment::getMicroTickCount() };
		const auto next = clock{
		  std::min(m_nextExecEnemies.count(), m_nextExecWarWithAllies.count())
		};

		const auto waitTime = std::max(
			clock::zero(),
			next - now
		);

		try {
			OATPP_LOGE(TAG, "Fetcher error: %s", e && e->what() ? e->what() : "unknown");
			const int code = std::atoi(e && e->what() ? e->what() : "0");
			if (code == 429)
			{
				auto room = m_room.lock();
				if (room)
				{
					room->sendError(ErrorMessage::KeyLimit, m_tornApiService.getLastKey()->userId);
				}
			}

			return waitFor(waitTime).next(yieldTo(&Fetcher::act));	
		}
		catch (...) {
			waitFor(waitTime).next(yieldTo(&Fetcher::act));
		}
	}
private:
	void setNextExecTimer(std::chrono::microseconds& timer)
	{
		timer = std::chrono::microseconds(std::max(
			(timer + m_interval).count(),
			oatpp::base::Environment::getMicroTickCount())
		);
	}
};
