#pragma once

#include "oatpp/web/server/HttpConnectionHandler.hpp"

#include "oatpp/network/virtual_/client/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/server/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/Interface.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"

#include "DatabaseComponent.hpp"
#include "clients/TornApiClient.hpp"
#include "../TestingFixtures.hpp"
#include "../MockTorn/MockResponseLoader.hpp"
#include "clients/FFScouterApiClient.hpp"
#include "clients/TornStatsApiClient.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "war/Lobby.hpp"

/**
 * Test Components config
 */
class TestComponent
{
public:

	static std::string getenv_or(const char* key, const char* def = "") {
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

	DatabaseComponent databaseComponent;

	/**
	 * Create oatpp virtual network interface for test networking
	 */
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, virtualInterface)([]
	{
		return oatpp::network::virtual_::Interface::obtainShared("virtualhost");
	}());

	/**
	 * Create server ConnectionProvider of oatpp virtual connections for test
	 */
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([]
	{
		OATPP_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, _interface);
		return oatpp::network::virtual_::server::ConnectionProvider::createShared(_interface);
	}());

	/**
	 * Create client ConnectionProvider of oatpp virtual connections for test
	 */
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider)([]
	{
		OATPP_COMPONENT(std::shared_ptr<oatpp::network::virtual_::Interface>, _interface);
		return oatpp::network::virtual_::client::ConnectionProvider::createShared(_interface);
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
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)([]
	{
		return oatpp::parser::json::mapping::ObjectMapper::createShared();
	}());


	OATPP_CREATE_COMPONENT(std::shared_ptr<TornApiClient>, tornApiClient)([]
	{
		/* Get client connection provider for Api Client */
		OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);

		/* Get object mapper component */
		OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

		/* Create http request executor for Api Client */
		auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(clientConnectionProvider);

		return TornApiClient::createShared(requestExecutor, objectMapper);
	}());


	OATPP_CREATE_COMPONENT(std::shared_ptr<FFScouterApiClient>, ffScouterApiClient)([] {
		/* Get client connection provider for Api Client */
		OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);

		/* Get object mapper component */
		OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

		/* Create http request executor for Api Client */
		auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(clientConnectionProvider);

		return FFScouterApiClient::createShared(requestExecutor, objectMapper);
		}());


	OATPP_CREATE_COMPONENT(std::shared_ptr<TornStatsApiClient>, tornStatsApiClient)([] {
		/* Get client connection provider for Api Client */
		OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);

		/* Get object mapper component */
		OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

		/* Create http request executor for Api Client */
		auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(clientConnectionProvider);

		return TornStatsApiClient::createShared(requestExecutor, objectMapper);
		}());


	OATPP_CREATE_COMPONENT(std::shared_ptr<Lobby>, lobby)([] {
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


	OATPP_CREATE_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader)([]
	{
		return std::make_shared<MockResponseLoader>();
	}());


	OATPP_CREATE_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures)([]
	{
		return std::make_shared<TestingFixtures>();
	}());
};
