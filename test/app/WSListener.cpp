#include "WSListener.hpp"


void WSListener::onPing(const WebSocket& socket, const oatpp::String& message)
{
}

void WSListener::onPong(const WebSocket& socket, const oatpp::String& message)
{
}

void WSListener::onClose(const WebSocket& socket, v_uint16 code, const oatpp::String& message)
{
	{
		std::lock_guard<std::mutex> lk(m_mtx);
		m_closed = true;
	}
	m_cv.notify_all();
}

void WSListener::readMessage(const WebSocket& socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size)
{
	if (size == 0)
	{
		// message transfer finished

		auto wholeMessage = m_messageBuffer.toString();
		m_messageBuffer.setCurrentPosition(0);

		if (wholeMessage)
		{
			{
				std::lock_guard<std::mutex> lk(m_mtx);
				auto messageDto = apiObjectMapper->readFromString<oatpp::Object<WarStateResponseDto>>(
					wholeMessage);
				m_inbox.push(messageDto);
			}
			m_cv.notify_all();
		}
		OATPP_LOGD(TAG, "on message received '%s'", wholeMessage->c_str());
	}
	else if (size > 0)
	{
		// message frame received
		m_messageBuffer.writeSimple(data, size);
	}
}

bool WSListener::waitForNext(oatpp::Object<WarStateResponseDto>& out, std::chrono::milliseconds timeout)
{
	std::unique_lock<std::mutex> lk(m_mtx);
	if (!m_cv.wait_for(lk, timeout, [&] { return !m_inbox.empty() || m_closed; }))
	{
		return false; // timeout
	}
	if (m_inbox.empty()) return false; // closed without messages
	out = std::move(m_inbox.front());
	m_inbox.pop();
	return true;
}
