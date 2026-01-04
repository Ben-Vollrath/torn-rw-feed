#pragma once
#include "app/WSListener.hpp"
#include "oatpp-websocket/Connector.hpp"
#include "oatpp-websocket/WebSocket.hpp"

struct TestWebSocket {
  std::shared_ptr<oatpp::websocket::WebSocket> socket;
  std::shared_ptr<WSListener> listener;
  std::thread pump;

  void close(int code = 1000, const char* reason = "test done") {
    if (socket) socket->sendClose(code, reason);
    if (pump.joinable()) pump.join();
  }
};

inline TestWebSocket makeTestWebSocket(
    const std::string& pathWithQuery,
    const std::shared_ptr<oatpp::network::ClientConnectionProvider>& provider) {
  auto connector = oatpp::websocket::Connector::createShared(provider);
  auto connection = connector->connect(pathWithQuery);
  auto ws = oatpp::websocket::WebSocket::createShared(connection, true);
  auto listener = std::make_shared<WSListener>();
  ws->setListener(listener);
  std::thread pump([ws] { ws->listen(); });
  return TestWebSocket{ws, listener, std::move(pump)};
}