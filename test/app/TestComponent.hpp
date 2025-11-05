#pragma once

#include "oatpp/web/server/HttpConnectionHandler.hpp"

#include "oatpp/network/virtual_/client/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/server/ConnectionProvider.hpp"
#include "oatpp/network/virtual_/Interface.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"

#include "DatabaseComponent.hpp"
#include "TornApiClient.hpp"
#include "../TestingFixtures.hpp"
#include "../MockTorn/MockResponseLoader.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"

/**
 * Test Components config
 */
class TestComponent
{
public:
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
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, asyncExecutor)([] {
		return std::make_shared<oatpp::async::Executor>(4, 1, 1);
	}());

	/**
	 *  Create ConnectionHandler component which uses Router component to route requests
	 */
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([] {
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


	OATPP_CREATE_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader)([]
	{
		return std::make_shared<MockResponseLoader>();
	}());


	OATPP_CREATE_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures)([]
	{
			return std::make_shared<TestingFixtures>();
	}());
};
