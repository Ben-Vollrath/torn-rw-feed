#include "WarSocketTest.hpp"

#include "oatpp-websocket/WebSocket.hpp"
#include "oatpp-websocket/Connector.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"

#include <thread>

#include "app/TestComponent.hpp"
#include "app/WSListener.hpp"
#include "controller/WarController.hpp"

#include "oatpp-test/web/ClientServerTestRunner.hpp"

#include "MockTorn/MockController.hpp"

namespace
{
	auto TAG = "websocket-client";

	bool finished = false;

	void socketTask(const std::shared_ptr<oatpp::websocket::WebSocket>& websocket)
	{
		websocket->listen();
		OATPP_LOGD(TAG, "SOCKET CLOSED!!!");
		finished = true;
	}
}


void WarSocketTest::testSocketOk()
{
	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
	testingFixtures->reset();

	OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
	mockResponseLoader->setResponsePaths({
		factionWarOKPath_, factionMembersOfflineOKPath_, ffscouterScoutOkPath_, factionMembersOneOnlineOKPath_, factionMembersOfflineOKPath_
	});

	auto user = testingFixtures->createTestUser(1);
	auto authHeader = testingFixtures->getUserAuthHeader(user->id);

	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
	auto connector = oatpp::websocket::Connector::createShared(clientConnectionProvider);
	auto connection = connector->connect("/wars/socket", authHeader);

	OATPP_LOGI(TAG, "Connected");

	auto socket = oatpp::websocket::WebSocket::createShared(connection, true);

	auto listener = std::make_shared<WSListener>();
	socket->setListener(listener);

	std::thread pump([&] { socket->listen(); });

	oatpp::Object<WarStateResponseDto> msg;
	//factionMembersOfflineOKPath_
	bool got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->members->size() == 2);
	OATPP_ASSERT(msg->members[0]->last_action->status == TornActionStatus::OFFLINE);

	//ffscouterScoutOkPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(!msg->members);
	OATPP_ASSERT(msg->memberStats["1"]->total == 2989885521);
	OATPP_ASSERT(!msg->memberStats["2"]->total);

	//factionMembersOneOnlineOKPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->members->size() == 1);
	OATPP_ASSERT(msg->members[0]->last_action->status == TornActionStatus::ONLINE);

	//factionMembersOfflineOKPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->members->size() == 1);
	OATPP_ASSERT(msg->members[0]->last_action->status == TornActionStatus::OFFLINE);

	socket->sendClose(1000, "test done");

	if (pump.joinable()) pump.join();
}

void WarSocketTest::testPostSpyWithRoom(const std::shared_ptr<ApiTestClient> client, std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
	testingFixtures->reset();

	OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
	mockResponseLoader->setResponsePaths({
		factionWarOKPath_, factionMembersOfflineOKPath_, ffscouterScoutOkPath_, tornStatsSpyOkPath_, factionMembersOfflineOKPath_
		});

	auto user = testingFixtures->createTestUser(1);
	auto issueResult = testingFixtures->getUserApiKey(user->id);
	auto authHeader = testingFixtures->getUserAuthHeader(user->id);

	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
	auto connector = oatpp::websocket::Connector::createShared(clientConnectionProvider);
	auto connection = connector->connect("/wars/socket", authHeader);

	OATPP_LOGI(TAG, "Connected");

	auto socket = oatpp::websocket::WebSocket::createShared(connection, true);

	auto listener = std::make_shared<WSListener>();
	socket->setListener(listener);

	std::thread pump([&] { socket->listen(); });

	oatpp::Object<WarStateResponseDto> msg;
	//factionMembersOfflineOKPath_
	bool got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->members->size() == 2);
	OATPP_ASSERT(msg->members[0]->last_action->status == TornActionStatus::OFFLINE);
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(!msg->members);
	OATPP_ASSERT(msg->memberStats["1"]->total == 2989885521);
	OATPP_ASSERT(!msg->memberStats["2"]->total);

	auto rsp = client->warSpy("testKey", issueResult.fullKey);
	OATPP_ASSERT(rsp->getStatusCode() == 200);
	auto body = rsp->readBodyToDto<oatpp::Object<SpyResponseDto>>(objectMapper);
	OATPP_ASSERT(body->importSize == 1)

	//ffscouterScoutOkPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->memberStats["1"]->total == 100);
	OATPP_ASSERT(msg->memberStats->find("2") == msg->memberStats->end());

	socket->sendClose(1000, "test done");

	if (pump.joinable()) pump.join();
}

void WarSocketTest::testPostSpyWithoutRoom(const std::shared_ptr<ApiTestClient> client, std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
	testingFixtures->reset();

	OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
	mockResponseLoader->setResponsePaths({
		factionWarOKPath_, tornStatsSpyOkPath_,
		});


	auto user = testingFixtures->createTestUser(1);
	auto issueResult = testingFixtures->getUserApiKey(user->id);
	auto authHeader = testingFixtures->getUserAuthHeader(issueResult);

	auto rsp = client->warSpy("testKey", issueResult.fullKey);
	OATPP_ASSERT(rsp->getStatusCode() == 200);
	auto body = rsp->readBodyToDto<oatpp::Object<SpyResponseDto>>(objectMapper);
	OATPP_ASSERT(body->importSize == 1)

	auto statsService = MemberStatsService();
	auto stats = statsService.getAllForWar(1073741824, 2);
	OATPP_ASSERT(stats->size() == 1)
	auto stat = stats[0];
	OATPP_ASSERT(stat->total == 100)
}

void WarSocketTest::onRun()
{
	/* Register test components */
	TestComponent component;

	/* Create client-server test runner */
	oatpp::test::web::ClientServerTestRunner runner;

	/* Add Controller endpoints to the router of the test server */
	runner.addController(MockController::createShared());
	runner.addController(WarController::createShared());


	/* Run test */
	runner.run([this, &runner]
	{
		OATPP_LOGI(TAG, "Application Started");
		/* Get client connection provider for Api Client */
		OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);

		OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

		/* Create http request executor for Api Client */
		auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(clientConnectionProvider);

		/* Create Test API client */
		auto client = ApiTestClient::createShared(requestExecutor, objectMapper);


		testSocketOk();
		testPostSpyWithRoom(client, objectMapper);
		testPostSpyWithoutRoom(client, objectMapper);
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
