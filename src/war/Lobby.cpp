#include "Lobby.hpp"

#include "Fetcher.hpp"

v_int32 Lobby::obtainNewPeerId()
{
	return m_userIdCounter++;
}

std::optional<std::shared_ptr<Room>> Lobby::getRoomNullable(std::int64_t factionId)
{
	std::lock_guard<std::mutex> lock(m_roomsMutex);
	std::shared_ptr<Room>& room = m_rooms[factionId];
	if (!room)
		return std::nullopt;

	return room;
}


std::shared_ptr<Room> Lobby::getOrCreateRoom(const std::int64_t factionId)
{
	std::lock_guard<std::mutex> lock(m_roomsMutex);
	std::shared_ptr<Room>& room = m_rooms[factionId];
	if (!room)
	{
		try {
			room = std::make_shared<Room>(factionId);
		} catch (const std::runtime_error& e)
		{
			OATPP_LOGD(TAG, "Error occured %s", e.what());
		}
		asyncExecutor->execute<Fetcher>(factionId, room);
	}
	return room;
}

void Lobby::onAfterCreate_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket,
                                      const std::shared_ptr<const ParameterMap>& params)
{
	oatpp::String factionIdText = params->find("faction_id")->second;
	oatpp::String userIdText = params->find("user_id")->second;
	OATPP_LOGD(TAG, "onAfterCreate_NonBlocking called, userId: %s", userIdText->c_str());

	std::int64_t factionId = std::stoll(factionIdText);
	std::int64_t userId = std::stoll(userIdText);
	auto room = getOrCreateRoom(factionId);

	auto peer = std::make_shared<Peer>(socket, room, obtainNewPeerId(), userId);
	socket->setListener(peer);

	room->addPeer(peer);
}

void Lobby::onBeforeDestroy_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket)
{
	OATPP_LOGD(TAG, "onBeforeDestroy_NonBlocking called");
	auto peer = std::static_pointer_cast<Peer>(socket->getListener());
	auto room = peer->getRoom();

	room->removePeerByPeerId(peer->getPeerId());

	/* Remove circle `std::shared_ptr` dependencies */
	socket->setListener(nullptr);

	if (room->isClosed())
	{
		std::lock_guard<std::mutex> lock(m_roomsMutex);
		m_rooms.erase(room->factionId());
	}
}
