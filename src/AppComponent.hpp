#pragma once

#include "AppConfig.hpp"
#include "SwaggerComponent.hpp"
#include "DatabaseComponent.hpp"
#include "clients/ClientComponent.hpp"

#include "oatpp-websocket/ConnectionHandler.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp-openssl/server/ConnectionProvider.hpp"
#include "oatpp-openssl/Config.hpp"
#include "oatpp/web/server/interceptor/AllowCorsGlobal.hpp"

#include "war/Lobby.hpp"


/**
 *  Class which creates and holds Application components and registers components in oatpp::base::Environment
 *  Order of components initialization is from top to bottom
 */
class AppComponent
{
public:
	static std::string getenv_or(const char* key, const char* def = "")
	{
		if (const char* v = std::getenv(key); v && *v) return std::string(v);
		return std::string(def);
	}

	OATPP_CREATE_COMPONENT(std::shared_ptr<AppConfig>, appConfig)([]
	{
		auto cfg = std::make_shared<AppConfig>();
		cfg->ffscouterApiKey = getenv_or("FFSCOUTER_API_KEY", "");
		cfg->databaseUrl = getenv_or("DATABASE_URL", "postgresql://torn:tornpass@localhost:5432/torn_rw_feed");
		cfg->oatppSwaggerResPath = getenv_or("OATPP_SWAGGER_RES_PATH", OATPP_SWAGGER_RES_PATH);
		cfg->sqlFilePath = getenv_or("SQL_FILE_PATH", SQL_FILE_PATH);
		cfg->certPath = getenv_or("CERT_PATH", CERT_PATH);
		cfg->keyPath = getenv_or("KEY_PATH", KEY_PATH);
		return cfg;
	}());

	/**
	*  Create ConnectionProvider component which listens on the port
	*/
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([]
	{
		OATPP_COMPONENT(std::shared_ptr<AppConfig>, appConfig);
		auto config = oatpp::openssl::Config::createDefaultServerConfigShared(appConfig->certPath, appConfig->keyPath);
		return oatpp::openssl::server::ConnectionProvider::createShared(config, {
			                                                                "0.0.0.0", 8000,
			                                                                oatpp::network::Address::IP_4
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
		auto connectionHandler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, asyncExecutor);
		connectionHandler->addRequestInterceptor(std::make_shared<oatpp::web::server::interceptor::AllowOptionsGlobal>());
		connectionHandler->addResponseInterceptor(std::make_shared<oatpp::web::server::interceptor::AllowCorsGlobal>());
		return connectionHandler;
	}());


	/**
	*  Create ObjectMapper component to serialize/deserialize DTOs in Contoller's API
	*/
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper)([]
	{
		return oatpp::parser::json::mapping::ObjectMapper::createShared();
	}());


	OATPP_CREATE_COMPONENT(std::shared_ptr<Lobby>, lobby)([]
	{
		return std::make_shared<Lobby>();
	}());

	/**
	 *  Create websocket connection handler
	 */
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::websocket::AsyncConnectionHandler>, websocketConnectionHandler)(
		"websocket", []
		{
			OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, asyncExecutor);
			auto connectionHandler = oatpp::websocket::AsyncConnectionHandler::createShared(asyncExecutor);
			OATPP_COMPONENT(std::shared_ptr<Lobby>, lobby);
			connectionHandler->setSocketInstanceListener(lobby);
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
	ClientComponent clientComponent;
};
