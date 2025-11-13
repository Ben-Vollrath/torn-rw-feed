#include "Room.hpp"

#include "dto/FactionMemberInfoDto.hpp"

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

void Room::updateMembers(const oatpp::Object<TornFactionMembersResponse>& memberInfos)
{
	const auto& members = memberInfos->members;
	auto updates = oatpp::Vector<oatpp::Object<TornFactionMember>>::createShared();
	for (const oatpp::Object<TornFactionMember>& member : *members)
	{
		auto it = membersState.find(member->id);

		if (it == membersState.end())
		{
			membersState[member->id] = member;
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
		oatpp::String updateJson = objectMapper->writeToString(FactionMemberInfoResponseDto::fromInfoList(updates));
		sendMessage(updateJson->c_str());
	}
}

void Room::resetMemberState()
{
	membersState.clear();
}
