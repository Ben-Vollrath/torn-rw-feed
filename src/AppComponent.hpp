#pragma once

#include "SwaggerComponent.hpp"
#include "DatabaseComponent.hpp"
#include "TornApiComponent.hpp"

#include "oatpp-websocket/ConnectionHandler.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"
#include "war/Lobby.hpp"


/**
 *  Class which creates and holds Application components and registers components in oatpp::base::Environment
 *  Order of components initialization is from top to bottom
 */
class AppComponent
{
public:
	/**
	*  Create ConnectionProvider component which listens on the port
	*/
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([]
	{
		return oatpp::network::tcp::server::ConnectionProvider::createShared({
			"0.0.0.0", 8000, oatpp::network::Address::IP_4
		});
	}());

	/**
	*  Create Router component
	*/
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([]
	{
		return oatpp::web::server::HttpRouter::createShared();
	}());


	/**
	 * Create Async Executor to execute Coroutines
	 */
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, asyncExecutor)([]
	{
		return std::make_shared<oatpp::async::Executor>(4, 1, 1);
	}());


	/**
	*  Create ConnectionHandler component which uses Router component to route requests
	*/
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([]
	{
		OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
		OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, asyncExecutor);
		return oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, asyncExecutor);
	}());


	/**
	*  Create ObjectMapper component to serialize/deserialize DTOs in Contoller's API
	*/
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper)([]
	{
		return oatpp::parser::json::mapping::ObjectMapper::createShared();
	}());


	/**
	 *  Create websocket connection handler
	 */
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::websocket::AsyncConnectionHandler>, websocketConnectionHandler)(
		"websocket", []
		{
			OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, asyncExecutor);
			auto connectionHandler = oatpp::websocket::AsyncConnectionHandler::createShared(asyncExecutor);
			connectionHandler->setSocketInstanceListener(std::make_shared<Lobby>());
			return connectionHandler;
		}());

	/**
	*  Swagger component
	*/
	SwaggerComponent swaggerComponent;

	/**
	 * Database Component
	 */
	DatabaseComponent databaseComponent;

	/**
	 * TornApi Component
	 */
	TornApiComponent tornApiComponent;
};
