#include "Lobby.hpp"

#include "Fetcher.hpp"

v_int32 Lobby::obtainNewUserId()
{
	return m_userIdCounter++;
}

std::shared_ptr<Room> Lobby::getOrCreateRoom(const std::int64_t factionId)
{
	std::lock_guard<std::mutex> lock(m_roomsMutex);
	std::shared_ptr<Room>& room = m_rooms[factionId];
	if (!room)
	{
		room = std::make_shared<Room>(factionId);
		asyncExecutor->execute<Fetcher>(factionId, room);
	}
	return room;
}

void Lobby::onAfterCreate_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket,
                                      const std::shared_ptr<const ParameterMap>& params)
{
	OATPP_LOGD(TAG, "onAfterCreate_NonBlocking called");

	std::int64_t factionId = std::stoll(params->find("faction_id")->second);
	auto room = getOrCreateRoom(factionId);

	auto peer = std::make_shared<Peer>(socket, room, obtainNewUserId());
	socket->setListener(peer);

	room->addPeer(peer);
}

void Lobby::onBeforeDestroy_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket)
{
	OATPP_LOGD(TAG, "onBeforeDestroy_NonBlocking called");
	auto peer = std::static_pointer_cast<Peer>(socket->getListener());
	auto room = peer->getRoom();

	room->removePeerByUserId(peer->getPeerId());

	/* Remove circle `std::shared_ptr` dependencies */
	socket->setListener(nullptr);

	if (room->isClosed())
	{
		std::lock_guard<std::mutex> lock(m_roomsMutex);
		m_rooms.erase(room->factionId());
	}
}
