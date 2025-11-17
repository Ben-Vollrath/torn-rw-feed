#pragma once

#include "Room.hpp"

#include "oatpp-websocket/AsyncConnectionHandler.hpp"
#include "service/TornApiService.hpp"

#include <unordered_map>
#include <mutex>


class Lobby : public oatpp::websocket::AsyncConnectionHandler::SocketInstanceListener
{
	OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, asyncExecutor);

public:
	std::atomic<v_int32> m_userIdCounter;
	std::unordered_map<std::int64_t, std::shared_ptr<Room>> m_rooms;
	std::mutex m_roomsMutex;

	Lobby()
		: m_userIdCounter(0)
	{
	}

	/**
	 * Generate id for new user
	 * @return
	 */
	v_int32 obtainNewUserId();

	/**
	 *Get room if one is available else return nullopt
	 */
	std::optional<std::shared_ptr<Room>> getRoomNullable(std::int64_t factionId);

	/**
	 * Get room by name or create new one if not exists.
	 * @param roomName
	 * @return
	 */
	std::shared_ptr<Room> getOrCreateRoom(std::int64_t factionId);

	/**
	 *  Called when socket is created
	 */
	void onAfterCreate_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket,
	                               const std::shared_ptr<const ParameterMap>& params) override;

	/**
	 *  Called before socket instance is destroyed.
	 */
	void onBeforeDestroy_NonBlocking(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket) override;

private:
	std::string TAG = "LOBBY";
};
