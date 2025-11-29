#include "Peer.hpp"
#include "Room.hpp"

void Peer::sendMessage(const oatpp::String& message)
{
	class SendMessageCoroutine : public oatpp::async::Coroutine<SendMessageCoroutine>
	{
		oatpp::async::Lock* m_lock;
		std::shared_ptr<AsyncWebSocket> m_websocket;
		oatpp::String m_message;

	public:
		SendMessageCoroutine(oatpp::async::Lock* lock,
		                     const std::shared_ptr<AsyncWebSocket>& websocket,
		                     const oatpp::String& message)
			: m_lock(lock)
			  , m_websocket(websocket)
			  , m_message(message)
		{
		}

		Action act() override
		{
			return oatpp::async::synchronize(m_lock, m_websocket->sendOneFrameTextAsync(m_message)).next(finish());
		}
	};

	m_asyncExecutor->execute<SendMessageCoroutine>(&m_writeLock, m_socket, message);
}

std::shared_ptr<Room> Peer::getRoom()
{
	return m_room.lock();
}

oatpp::async::CoroutineStarter Peer::onPing(const std::shared_ptr<AsyncWebSocket>& socket, const oatpp::String& message)
{
	return oatpp::async::synchronize(&m_writeLock, socket->sendPongAsync(message));
}

oatpp::async::CoroutineStarter Peer::onPong(const std::shared_ptr<AsyncWebSocket>& socket, const oatpp::String& message)
{
	return nullptr;
}

oatpp::async::CoroutineStarter Peer::onClose(const std::shared_ptr<AsyncWebSocket>& socket, v_uint16 code,
                                             const oatpp::String& message)
{
	return nullptr;
}


oatpp::async::CoroutineStarter Peer::readMessage(const std::shared_ptr<AsyncWebSocket>& socket, v_uint8 opcode,
                                                 p_char8 data, oatpp::v_io_size size)
{
	if (size == 0) { // message transfer finished

		auto wholeMessage = m_messageBuffer.toString();
		m_messageBuffer.setCurrentPosition(0);

		auto room = m_room.lock();
		if (room)
		{

			room->updateTarget(getUserId(), wholeMessage);
		}

	}
	else if (size > 0) { // message frame received
		m_messageBuffer.writeSimple(data, size);
	}

	return nullptr; // do nothing

}
