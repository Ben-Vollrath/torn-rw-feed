#include <condition_variable>
#include <queue>
#include <oatpp/core/macro/component.hpp>

#include "dto/responses/WarStateResponseDto.hpp"
#include "oatpp/core/data/mapping/ObjectMapper.hpp"
#include "oatpp-websocket/ConnectionHandler.hpp"
#include "oatpp-websocket/WebSocket.hpp"


class WSListener : public oatpp::websocket::WebSocket::Listener
{
	OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper);
	static constexpr auto TAG = "Client_WSListener";
	/**
	 * Buffer for messages. Needed for multi-frame messages.
	 */
	oatpp::data::stream::BufferOutputStream m_messageBuffer;
	std::mutex m_mtx;
	std::condition_variable m_cv;
	std::queue<oatpp::Object<WarStateResponseDto>> m_inbox;
	bool m_closed = false;

public:
	/**
	 * Called on "ping" frame.
	 */
	void onPing(const WebSocket& socket, const oatpp::String& message) override;

	/**
	 * Called on "pong" frame
	 */
	void onPong(const WebSocket& socket, const oatpp::String& message) override;

	/**
	 * Called on "close" frame
	 */
	void onClose(const WebSocket& socket, v_uint16 code, const oatpp::String& message) override;

	/**
	 * Called on each message frame. After the last message will be called once-again with size == 0 to designate end of the message.
	 */
	void readMessage(const WebSocket& socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) override;


	bool waitForNext(oatpp::Object<WarStateResponseDto>& out, std::chrono::milliseconds timeout);
};
