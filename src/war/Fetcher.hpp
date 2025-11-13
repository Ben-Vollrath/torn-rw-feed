#pragma once

#include "Room.hpp"
#include "oatpp/core/async/Coroutine.hpp"
#include "service/FactionService.hpp"
#include "service/FFScouterApiService.hpp"
#include "service/MemberStatsService.hpp"
#include "service/TornApiService.hpp"
#include "service/TornApiServiceKeyManaged.hpp"
#include "service/WarFactionStatsFetchesService.hpp"
#include "service/WarService.hpp"

class Fetcher : public oatpp::async::Coroutine<Fetcher>
{
	std::int64_t m_factionId;
	TornApiServiceKeyManaged m_tornApiService;
	std::weak_ptr<Room> m_room;
	std::chrono::microseconds m_interval;

	WarService m_warService;
	WarFactionStatsService m_warFactionStatsService;
	MemberStatsService m_memberStatsService;
	FFScouterApiService m_ffScouterApiService;

	std::int64_t m_count = 100;
	bool m_warNeedsStats = true;
	std::optional<std::int64_t> m_enemyFactionId;
	std::optional<std::int64_t> m_warId;


	const std::string TAG = "FETCHER";

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

		if (m_count >= 100)
		{
			return m_tornApiService.getFactionWar().callbackTo(&Fetcher::onFactionIdUpdate);
		}

		if (m_enemyFactionId)
		{
			OATPP_LOGD(TAG, "Fetching Faction Members.")
			return m_tornApiService.getFactionMembers(m_enemyFactionId.value()).callbackTo(
				&Fetcher::onMemberUpdate);
		}
		OATPP_LOGD(TAG, "Currently no war running.")


		m_count++;
		return scheduleNextTick();
	}

	Action onFactionIdUpdate(const oatpp::Object<TornFactionWarResponseDto>& factionWarResponse)
	{
		auto enemyFactionId = factionWarResponse->getEnemyFactionId(m_factionId);
		if (m_enemyFactionId != enemyFactionId)
		{
			//New war is detected
			auto dto = WarDto::fromWarResponse(factionWarResponse->wars->ranked);
			auto newWar = m_warService.upsertById(dto);
			m_warId = newWar->id;
			m_warNeedsStats = true;
			m_enemyFactionId = enemyFactionId;
			
		}
		m_count = 0;
		return scheduleNextTick();
	}

	Action onMemberUpdate(const oatpp::Object<TornFactionMembersResponse>& memberInfo)
	{

		auto room = m_room.lock();
		if (!room || room->isClosed())
		{
			OATPP_LOGD(TAG, "Room is closed, finishing fetcher.")
			return finish();
		}

		room->updateMembers(memberInfo);

		if (m_warNeedsStats) {
			auto stats = m_warFactionStatsService.getByFactionAndWarId(m_enemyFactionId.value(), m_warId.value());

			if (!stats) {
				return m_ffScouterApiService.getScout(memberInfo).callbackTo(&Fetcher::onScouts);
			}
			m_warNeedsStats = false;
		}

		return scheduleNextTick();
	}

	Action onScouts(const FFScouterResponseDto& scouts)
	{
	m_memberStatsService.createMany(MemberStatsDto::fromFFScouterResponse(m_warId.value(), m_enemyFactionId.value(), scouts));
		m_warNeedsStats = false;
		m_warFactionStatsService.createWithIds(m_enemyFactionId.value(), m_warId.value());

		return scheduleNextTick();
	}

	Action scheduleNextTick() {
		using namespace std::chrono;
		auto now = duration_cast<microseconds>(seconds(std::time(nullptr)));
		++m_count;
		return oatpp::async::Action::createWaitRepeatAction(now.count() + m_interval.count());
	}

	Action handleError(Error* e) override {
		OATPP_LOGE(TAG, "Fetcher error: %s", e ? e->what() : "unknown");

		return scheduleNextTick();
	}
};
