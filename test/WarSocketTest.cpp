#include "WarSocketTest.hpp"

#include "oatpp-websocket/WebSocket.hpp"
#include "oatpp-websocket/Connector.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"

#include <thread>
#include "app/TestComponent.hpp"
#include "app/WSListener.hpp"
#include "controller/WarController.hpp"

#include "oatpp-test/web/ClientServerTestRunner.hpp"

#include "MockTorn/TornController.hpp"

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
	OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
	mockResponseLoader->setResponsePaths({
		factionWarOKPath_, factionMembersOfflineOKPath_, factionMembersOneOnlineOKPath_, factionMembersOfflineOKPath_
	});

	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
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

	oatpp::Object<FactionMemberInfoResponseDto> msg;
	bool got = listener->waitForNext(msg, std::chrono::seconds(5));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->members->size() == 2);
	OATPP_ASSERT(msg->members[0]->status == TornActionStatus::OFFLINE);

	got = listener->waitForNext(msg, std::chrono::seconds(5));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->members->size() == 1);
	OATPP_ASSERT(msg->members[0]->status == TornActionStatus::ONLINE);

	got = listener->waitForNext(msg, std::chrono::seconds(5));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->members->size() == 1);
	OATPP_ASSERT(msg->members[0]->status == TornActionStatus::OFFLINE);

	socket->sendClose(1000, "test done");

	if (pump.joinable()) pump.join();
}


void WarSocketTest::onRun()
{
	/* Register test components */
	TestComponent component;

	/* Create client-server test runner */
	oatpp::test::web::ClientServerTestRunner runner;

	/* Add Controller endpoints to the router of the test server */
	runner.addController(TornController::createShared());
	runner.addController(WarController::createShared());


	/* Run test */
	runner.run([this, &runner]
	{
		OATPP_LOGI(TAG, "Application Started");


		testSocketOk();
	}, std::chrono::minutes(10) /* test timeout */);

	OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionPool>, connectionPool);
	connectionPool->stop();

	OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, asyncExecutor);
	asyncExecutor->waitTasksFinished();
	asyncExecutor->stop();
	asyncExecutor->join();

	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
	testingFixtures->reset();

	/* wait all server threads finished */
	std::this_thread::sleep_for(std::chrono::seconds(1));
}
