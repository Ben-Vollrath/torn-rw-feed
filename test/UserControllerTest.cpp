#include "UserControllerTest.hpp"

#include "app/MyApiTestClient.hpp"

#include "app/TestComponent.hpp"
#include "controller/UserController.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"

#include "oatpp-test/web/ClientServerTestRunner.hpp"

#include "MockTorn/MockController.hpp"

void UserControllerTest::testDelete(const std::shared_ptr<ApiTestClient> client,
	std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
	testingFixtures->reset();

	auto user = testingFixtures->createTestUser(1);
	auto issueResult = testingFixtures->getUserApiKey(user->id);
	auto userService = UserService();
	auto userdb = userService.getByIdNullable(user->id);
	OATPP_ASSERT(userdb);

	auto rsp = client->deleteUser(issueResult.fullKey);
	OATPP_ASSERT(rsp->getStatusCode() == 200);
	userdb = userService.getByIdNullable(user->id);
	OATPP_ASSERT(!userdb);
}


void UserControllerTest::onRun()
{
	/* Register test components */
	TestComponent component;

	/* Create client-server test runner */
	oatpp::test::web::ClientServerTestRunner runner;

	/* Add Controller endpoints to the router of the test server */
	runner.addController(UserController::createShared());


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

			testDelete(client, objectMapper);
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
