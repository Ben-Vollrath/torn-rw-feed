#include "Room.hpp"

bool Room::isClosed() const
{
	return closed_.load(std::memory_order_acquire);
}

std::int64_t Room::factionId() const
{
	return m_factionId;
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
		closed_.store(true, std::memory_order_release);
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
	if (membersState.empty()) { return; }

	oatpp::String currentStateJson = objectMapper->writeToString(
		FactionMemberInfoResponseDto::fromMembersState(membersState));
	peer->sendMessage(currentStateJson);
}

void Room::updateMembers(const std::vector<FactionMemberInfo> memberInfos)
{
	std::vector<FactionMemberInfo> updates;
	for (FactionMemberInfo member : memberInfos)
	{
		auto it = membersState.find(member.id);

		if (it == membersState.end())
		{
			membersState[member.id] = member;
			updates.push_back(member);
		}
		else
		{
			const auto& old = it->second;
			if (old.actionTimestamp != member.actionTimestamp ||
				old.status != member.status ||
				old.statusState != member.statusState)
			{
				it->second = member;
				updates.push_back(member);
			}
		}
	}

	if (!updates.empty())
	{
		oatpp::String updateJson = objectMapper->writeToString(FactionMemberInfoResponseDto::fromInfoList(updates));
		sendMessage(updateJson->c_str());
	}
}
