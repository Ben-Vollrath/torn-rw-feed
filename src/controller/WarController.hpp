#pragma once

#include "AuthHandler.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp-websocket/AsyncConnectionHandler.hpp"
#include "oatpp-websocket/Handshaker.hpp"


#include OATPP_CODEGEN_BEGIN(ApiController)

class WarController : public oatpp::web::server::api::ApiController
{
	using __ControllerType = WarController;
	OATPP_COMPONENT(std::shared_ptr<oatpp::websocket::AsyncConnectionHandler>, websocketConnectionHandler, "websocket");
	std::shared_ptr<AuthHandler> m_authHandler = std::make_shared<AuthHandler>();

public:
	WarController(const std::shared_ptr<ObjectMapper>& objectMapper)
		: ApiController(objectMapper)
	{
	}

	static std::shared_ptr<WarController> createShared(
		OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)
	)
	{
		return std::make_shared<WarController>(objectMapper);
	}

	const std::shared_ptr<AuthHandler>& authHandler() const { return m_authHandler; }

	ENDPOINT_INFO(WS) {
		info->summary = "Connects to the War WebSocket";
		info->description = "This endpoint upgrades the connection to a WebSocket. "
			"The client must provide a valid `Authorization` header (Bearer token) "
			"to authenticate before the upgrade.";

		info->addResponse<String>(Status::CODE_101, "text/plain", "WebSocket upgrade");

		auto& authHeader = info->headers.add<oatpp::String>(oatpp::web::protocol::http::Header::AUTHORIZATION);
		authHeader.description = "Bearer token for authentication";
		authHeader.required = true;

		info->addTag("WebSocket");
	}

	ENDPOINT_ASYNC("GET", "/wars/socket", WS)
	{
		ENDPOINT_ASYNC_INIT(WS)

		oatpp::async::Action act() override
		{
			const oatpp::String authHeader = request->getHeader(oatpp::web::protocol::http::Header::AUTHORIZATION);

			auto baseObj = controller->authHandler()->authorize(authHeader);
			auto authObj = std::dynamic_pointer_cast<AuthObject>(baseObj);

			/* Websocket handshake */
			auto response = oatpp::websocket::Handshaker::serversideHandshake(request->getHeaders(),
			                                                                  controller->websocketConnectionHandler);

			auto parameters = std::make_shared<oatpp::network::ConnectionHandler::ParameterMap>();

			(*parameters)["faction_id"] = std::to_string(authObj->factionId);

			/* Set connection upgrade params */
			response->setConnectionUpgradeParameters(parameters);

			return _return(response);
		}
	};
};

#include OATPP_CODEGEN_END(ApiController)
