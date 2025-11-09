#pragma once

#include "Room.hpp"
#include "oatpp/core/async/Coroutine.hpp"
#include "service/TornApiService.hpp"
#include "service/TornApiServiceKeyManaged.hpp"

class Fetcher : public oatpp::async::Coroutine<Fetcher>
{
	std::int64_t m_factionId;
	std::optional<std::int64_t> m_enemyFactionId;
	TornApiServiceKeyManaged m_tornApiService;
	std::weak_ptr<Room> m_room;
	std::chrono::microseconds m_interval;

	std::int64_t count = 100;

	std::string TAG = "FETCHER";

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

		if (count >= 100)
		{
			return m_tornApiService.getEnemyWarFaction(m_factionId).callbackTo(&Fetcher::onFactionIdUpdate);
		}

		if (m_enemyFactionId)
		{
			OATPP_LOGD(TAG, "Fetching Faction Members.")
			return m_tornApiService.getFactionMembersCompact(m_enemyFactionId.value()).callbackTo(
				&Fetcher::onMemberUpdate);
		}
		OATPP_LOGD(TAG, "Currently no war running.")


		count++;

		using namespace std::chrono;
		auto now = duration_cast<microseconds>(seconds(std::time(nullptr)));
		return oatpp::async::Action::createWaitRepeatAction(now.count() + m_interval.count());
	}

	Action onFactionIdUpdate(const std::optional<std::int64_t>& enemyFactionId)
	{
		m_enemyFactionId = enemyFactionId;
		count = 0;
		return yieldTo(&Fetcher::act);
	}

	Action onMemberUpdate(const std::vector<FactionMemberInfo>& memberInfo)
	{
		auto room = m_room.lock();
		if (!room || room->isClosed())
		{
			OATPP_LOGD(TAG, "Room is closed, finishing fetcher.")
			return finish();
		}

		room->updateMembers(memberInfo);
		return yieldTo(&Fetcher::act);
	}
};
