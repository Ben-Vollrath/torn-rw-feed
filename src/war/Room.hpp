#pragma once

#include "Peer.hpp"

#include <unordered_map>


#include "dto/MemberStatsDto.hpp"
#include "dto/clients/TornFactionMembersResponseDto.hpp"
#include "oatpp/core/data/mapping/ObjectMapper.hpp"
#include "service/TornApiServiceKeyManaged.hpp"


class Room
{
	/* Kept in order to inform new Peers about memberStats in [sendCurrentState].
	 * All updates on [updateStats] are sent directly through messages and not checked for changes against [memberStats]
	 */
	std::unordered_map<std::int64_t, oatpp::Object<MemberStatsDto>> m_memberStats;

	/* Kept in order to only update on new enemy Information */
	std::unordered_map<std::int64_t, oatpp::Object<TornFactionMember>> m_enemiesState;

	/* Kept in order to only update on new ally Information */
	std::unordered_map<std::int64_t, oatpp::Object<TornFactionMember>> m_alliesState;

	/* General information about the ongoing war */
	oatpp::Object<TornFactionWarResponseDto> m_factionWar;


	std::unordered_map<v_int64, std::shared_ptr<Peer>> m_peerById;
	std::unordered_map<std::int64_t,
	                   std::unordered_map<std::int64_t, std::shared_ptr<Peer>>> m_peersByUserId;

	std::mutex m_peerByIdLock;

	std::atomic<bool> m_closed{false};
	std::int64_t m_factionId;

	OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

public:
	explicit Room(std::int64_t factionId) : m_factionId(factionId)
	{
	}

	bool isClosed() const;

	std::int64_t factionId() const;

	std::optional<std::int64_t> getWarId();

	std::optional<std::int64_t> getEnemyFactionId();

	/**
	 * Add peer to the room.
	 * @param peer
	 */
	void addPeer(const std::shared_ptr<Peer>& peer);

	/**
	 * Remove peer from the room.
	 * @param peerId
	 */
	void removePeerByPeerId(v_int32 peerId);


	/** Update enemies information.
	 * @param memberInfos
	 */
	void updateEnemies(const oatpp::Object<TornFactionMembersResponse>& memberInfos);

	/** Update allies information.
	 * @param memberInfos
	 */
	void updateAllies(const oatpp::Object<TornFactionMembersResponse>& memberInfos);

	/** Update member stats information.
	 * Always updates all included fields.
	 * @param memberStats
	 */
	void updateStats(const oatpp::Vector<oatpp::Object<MemberStatsDto>>& memberStats);

	/** Updates current war
	 *
	 */
	void updateWar(const oatpp::Object<TornFactionWarResponseDto>& factionWarResponses);

	/** Return true if room has no stat information.
	 */
	bool needStats();

	/** Reset member state.
	 */
	void resetState();

private:
	/**
	* Send message to all peers in the room.
	* @param message
	*/
	void sendMessage(const oatpp::String& message);
	void sendMessage(const oatpp::String& message, std::int64_t peerId);

	/**
	* Sends the current state of the room to the given Peer.
	*/
	void sendCurrentState(const std::shared_ptr<Peer>& peer);
};
