#include "AuthControllerTest.hpp"
#include "app/MyApiTestClient.hpp"

#include "app/TestComponent.hpp"
#include "controller/AuthController.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"

#include "oatpp-test/web/ClientServerTestRunner.hpp"

#include "MockTorn/MockController.hpp"

void AuthControllerTest::testAuthOk(const std::shared_ptr<ApiTestClient> client,
                                    std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
	testingFixtures->reset();

	OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
	mockResponseLoader->setResponsePaths({ factionBasicOKPath_, userBasicOkPath_});
	auto factionInfo = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionResponseDto>>(factionBasicOKPath_);
	auto userInfo = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornUserBasicResponseDto>>(userBasicOkPath_);

	/* Call server API */
	/* Call root endpoint of MyController */
	auto response = client->auth("ok");

	/* Assert that server responds with 200 */
	OATPP_ASSERT(response->getStatusCode() == 200)

	/* Read response body as MessageDto */
	auto message = response->readBodyToDto<oatpp::Object<ApiKeyIssueResponseDto>>(objectMapper.get());

	/* Assert that received message is as expected */
	OATPP_ASSERT(message)
	OATPP_ASSERT(message->apiKey)

	auto now = std::time(nullptr);
	constexpr auto seconds_in_month = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::hours(24*30)).count();
	OATPP_ASSERT(message->expiresAt - now >= (seconds_in_month - 5))

	auto m_userService = UserService();
	auto user = m_userService.getById(userInfo->profile->id);
	OATPP_ASSERT(user && user->factionId == factionInfo->basic->id && user->tornKey == "ok")
}

void AuthControllerTest::testAuthError(const std::shared_ptr<ApiTestClient> client,
                                       std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
	testingFixtures->reset();

	OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
	mockResponseLoader->setResponsePaths({errorInactiveKey_});
	auto response = client->auth("keyError");

	OATPP_ASSERT(response->getStatusCode() == 401)
}

void AuthControllerTest::onRun()
{
	/* Register test components */
	TestComponent component;

	/* Create client-server test runner */
	oatpp::test::web::ClientServerTestRunner runner;

	/* Add Controller endpoints to the router of the test server */
	runner.addController(MockController::createShared());
	runner.addController(AuthController::createShared());


	/* Run test */
	runner.run([this, &runner]
	{
		/* Get client connection provider for Api Client */
		OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);

		OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

		/* Create http request executor for Api Client */
		auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(clientConnectionProvider);

		/* Create Test API client */
		auto client = ApiTestClient::createShared(requestExecutor, objectMapper);

		testAuthOk(client, objectMapper);
		testAuthError(client, objectMapper);
	}, std::chrono::minutes(10) /* test timeout */);

	OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionPool>, connectionPool);
	connectionPool->stop();

	OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, asyncExecutor);
	asyncExecutor->waitTasksFinished();
	asyncExecutor->stop();
	asyncExecutor->join();


	/* wait all server threads finished */
	std::this_thread::sleep_for(std::chrono::seconds(1));
}
