#pragma once

#include "oatpp-websocket/AsyncWebSocket.hpp"
#include "oatpp/core/async/Executor.hpp"
#include "oatpp/core/async/Lock.hpp"
#include "oatpp/core/macro/component.hpp"

class Room;  // FWD

class Peer : public oatpp::websocket::AsyncWebSocket::Listener {
  /**
   * Buffer for messages. Needed for multi-frame messages.
   */
  oatpp::data::stream::BufferOutputStream m_messageBuffer;

  /**
   * Lock for synchronization of writes to the web socket.
   */
  oatpp::async::Lock m_writeLock;

  std::shared_ptr<AsyncWebSocket> m_socket;
  std::weak_ptr<Room> m_room;
  std::int64_t m_peerId;
  std::int64_t m_userId;

  /**
   * Inject async executor object.
   */
  OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, m_asyncExecutor);

 public:
  Peer(const std::shared_ptr<AsyncWebSocket>& socket,
       const std::shared_ptr<Room>& room, const std::int64_t peerId,
       const std::int64_t userId)
      : m_socket(socket), m_room(room), m_peerId(peerId), m_userId(userId) {}

  /**
   * Get the peer Id
   */
  std::int64_t getPeerId() { return m_peerId; }

  /**
   * Get the peer Id
   */
  std::int64_t getUserId() { return m_userId; }

  /**
   * Send message to peer (to user).
   * @param message
   */
  void sendMessage(const oatpp::String& message);

  /**
   * Get room of the peer.
   * @return
   */
  std::shared_ptr<Room> getRoom();

  // WebSocket Listener methods

  CoroutineStarter onPing(const std::shared_ptr<AsyncWebSocket>& socket,
                          const oatpp::String& message) override;
  CoroutineStarter onPong(const std::shared_ptr<AsyncWebSocket>& socket,
                          const oatpp::String& message) override;
  CoroutineStarter onClose(const std::shared_ptr<AsyncWebSocket>& socket,
                           v_uint16 code,
                           const oatpp::String& message) override;
  CoroutineStarter readMessage(const std::shared_ptr<AsyncWebSocket>& socket,
                               v_uint8 opcode, p_char8 data,
                               oatpp::v_io_size size) override;
};
