#pragma once

#include "Peer.hpp"

#include <unordered_map>


#include "dto/clients/TornFactionMembersResponseDto.hpp"
#include "oatpp/core/data/mapping/ObjectMapper.hpp"
#include "service/TornApiServiceKeyManaged.hpp"


class Room
{
	std::unordered_map<std::int64_t, FactionMemberInfo> membersState;
	std::unordered_map<v_int32, std::shared_ptr<Peer>> m_peerById;
	std::mutex m_peerByIdLock;

	std::atomic<bool> closed_{false};
	std::int64_t m_factionId;
	OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

public:
	explicit Room(std::int64_t factionId) : m_factionId(factionId)
	{
	}

	bool isClosed() const;

	std::int64_t factionId() const;

	/**
	 * Add peer to the room.
	 * @param peer
	 */
	void addPeer(const std::shared_ptr<Peer>& peer);

	/**
	 * Remove peer from the room.
	 * @param userId
	 */
	void removePeerByUserId(v_int32 userId);


	/** Update member information.
	 * @param memberInfos
	 */
	void updateMembers(std::vector<FactionMemberInfo> memberInfos);

private:
	/**
	* Send message to all peers in the room.
	* @param message
	*/
	void sendMessage(const oatpp::String& message);

	/**
	* Sends the current state of the room to the given Peer.
	*/
	void sendCurrentState(const std::shared_ptr<Peer>& peer);
};
