#include "Room.hpp"

#include "dto/FactionMemberInfoDto.hpp"

bool Room::isClosed() const
{
	return m_closed.load(std::memory_order_acquire);
}

std::int64_t Room::factionId() const
{
	return m_factionId;
}

std::optional<std::int64_t> Room::getWarId()
{
	return m_factionWar->getWarId();
}

std::optional<std::int64_t> Room::getEnemyFactionId()
{
	return m_factionWar->getEnemyFactionId(m_factionId);
}

void Room::addPeer(const std::shared_ptr<Peer>& peer)
{
	std::lock_guard<std::mutex> guard(m_peerByIdLock);
	m_peerById[peer->getPeerId()] = peer;
	sendCurrentState(peer);
}

void Room::removePeerByUserId(v_int32 userId)
{
	std::lock_guard<std::mutex> guard(m_peerByIdLock);
	m_peerById.erase(userId);
	if (m_peerById.empty())
	{
		m_closed.store(true, std::memory_order_release);
	}
}

void Room::sendMessage(const oatpp::String& message)
{
	std::lock_guard<std::mutex> guard(m_peerByIdLock);
	for (auto& pair : m_peerById)
	{
		pair.second->sendMessage(message);
	}
}

void Room::sendCurrentState(const std::shared_ptr<Peer>& peer)
{
	if (m_memberState.empty()) { return; }

	auto rsp = WarStateResponseDto::fromMembersInfo(m_memberState);
	rsp->addMemberStats(m_memberStats);
	oatpp::String currentStateJson = objectMapper->writeToString(
		WarStateResponseDto::fromMembersInfo(m_memberState));

	peer->sendMessage(currentStateJson);
}

void Room::updateMembers(const oatpp::Object<TornFactionMembersResponse>& memberInfos)
{
	const auto& members = memberInfos->members;
	auto updates = oatpp::Vector<oatpp::Object<TornFactionMember>>::createShared();
	for (const oatpp::Object<TornFactionMember>& member : *members)
	{
		auto it = m_memberState.find(member->id);

		if (it == m_memberState.end())
		{
			m_memberState[member->id] = member;
			updates->push_back(member);
		}
		else
		{
			const auto& old = it->second;
			if (old->last_action->timestamp != member->last_action->timestamp ||
				old->status->state != member->status->state ||
				old->last_action->status != member->last_action->status)
			{
				it->second = member;
				updates->push_back(member);
			}
		}
	}

	if (!updates->empty())
	{
		oatpp::String updateJson = objectMapper->writeToString(WarStateResponseDto::fromMembersInfo(updates));
		sendMessage(updateJson->c_str());
	}
}


void Room::updateStats(const oatpp::Vector<oatpp::Object<MemberStatsDto>>& stats)
{
	for (const oatpp::Object<MemberStatsDto>& stat : *stats)
	{
		m_memberStats[stat->member_id] = stat;
	}
	auto out = WarStateResponseDto::fromMembersStats(stats);
	oatpp::String updateJson = objectMapper->writeToString(out);
	sendMessage(updateJson->c_str());
}

void Room::updateWar(const oatpp::Object<TornFactionWarResponseDto>& factionWarResponses)
{
	m_factionWar = factionWarResponses;
}

bool Room::needStats()
{
	return m_memberStats.empty();
}


void Room::resetState()
{
	m_memberState.clear();
	m_memberStats.clear();
	m_factionWar = nullptr;
}
