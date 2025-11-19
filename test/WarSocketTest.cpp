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
#include "AssertionHelper.hpp"

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


void WarSocketTest::testSocketOk(std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
	testingFixtures->reset();

	OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
	mockResponseLoader->setResponsePaths({
		factionWarOKPath_, factionMembersOfflineOKPath_, ffscouterScoutOkPath_, factionMembersOneOnlineOKPath_, factionMembersOfflineOKPath_
	});
	auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarResponseDto>>(factionWarOKPath_);
	auto factionMembersOffline = mockResponseLoader->loadDtoFromFile< oatpp::Object<TornFactionMembersResponse>>(factionMembersOfflineOKPath_);
	factionMembersOffline->members[0]->status->parseLocation();
	factionMembersOffline->members[1]->status->parseLocation();
	auto memberOneId = factionMembersOffline->members[0]->id;
	auto memberTwoId = factionMembersOffline->members[1]->id;
	auto factionMembersOnline = mockResponseLoader->loadDtoFromFile< oatpp::Object<TornFactionMembersResponse>>(factionMembersOneOnlineOKPath_);
	factionMembersOnline->members[0]->status->parseLocation();
	auto ffScouter = mockResponseLoader->loadDtoFromFile<FFScouterResponseDto>(ffscouterScoutOkPath_);
	auto memberOneStats = ffScouter[0]->bs_estimate;
	auto memberTwoStats = ffScouter[1]->bs_estimate;

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
	OATPP_ASSERT(dtoFieldsEqual(msg->members[0], factionMembersOffline->members[0], objectMapper))
	OATPP_ASSERT(dtoFieldsEqual(msg->members[1], factionMembersOffline->members[1], objectMapper))

	//ffscouterScoutOkPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(!msg->members);
	OATPP_ASSERT(msg->memberStats[std::to_string(memberOneId)]->total == memberOneStats);
	OATPP_ASSERT(!msg->memberStats[std::to_string(memberTwoId)]->total);

	//factionMembersOneOnlineOKPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->members->size() == 1);
	OATPP_ASSERT(dtoFieldsEqual(msg->members[0], factionMembersOnline->members[0], objectMapper))

	//factionMembersOfflineOKPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->members->size() == 1);
	OATPP_ASSERT(dtoFieldsEqual(msg->members[0], factionMembersOffline->members[0], objectMapper))

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
	auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarResponseDto>>(factionWarOKPath_);
	auto factionMembersOffline = mockResponseLoader->loadDtoFromFile< oatpp::Object<TornFactionMembersResponse>>(factionMembersOfflineOKPath_);
	factionMembersOffline->members[0]->status->parseLocation();
	factionMembersOffline->members[1]->status->parseLocation();
	auto memberOneId = factionMembersOffline->members[0]->id;
	auto memberTwoId = factionMembersOffline->members[1]->id;
	auto factionMembersOnline = mockResponseLoader->loadDtoFromFile< oatpp::Object<TornFactionMembersResponse>>(factionMembersOneOnlineOKPath_);
	factionMembersOnline->members[0]->status->parseLocation();
	auto ffScouter = mockResponseLoader->loadDtoFromFile<FFScouterResponseDto>(ffscouterScoutOkPath_);
	auto memberOneStats = ffScouter[0]->bs_estimate;
	auto memberTwoStats = ffScouter[1]->bs_estimate;
	auto tornStatsSpy = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornStatsSpyResponseDto>>(tornStatsSpyOkPath_);
	auto memberOneSpy = tornStatsSpy->faction->members[std::to_string(memberOneId)]->spy;

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
	OATPP_ASSERT(dtoFieldsEqual(msg->members[0], factionMembersOffline->members[0], objectMapper))

	//ffscouterScoutOkPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(!msg->members);
	OATPP_ASSERT(msg->memberStats[std::to_string(memberOneId)]->total == memberOneStats);
	OATPP_ASSERT(!msg->memberStats[std::to_string(memberTwoId)]->total);

	auto rsp = client->warSpy("testKey", issueResult.fullKey);
	OATPP_ASSERT(rsp->getStatusCode() == 200);
	auto body = rsp->readBodyToDto<oatpp::Object<SpyResponseDto>>(objectMapper);
	OATPP_ASSERT(body->importSize == 1)

	//tornStatsSpyOkPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->memberStats[std::to_string(memberOneId)]->total == memberOneSpy->total);
	OATPP_ASSERT(msg->memberStats->find(std::to_string(memberTwoId)) == msg->memberStats->end());

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
	auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarResponseDto>>(factionWarOKPath_);
	auto factionMembersOffline = mockResponseLoader->loadDtoFromFile< oatpp::Object<TornFactionMembersResponse>>(factionMembersOfflineOKPath_);
	auto tornStatsSpy = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornStatsSpyResponseDto>>(tornStatsSpyOkPath_);
	auto memberOneSpy = tornStatsSpy->faction->members[std::to_string(1)]->spy;

	auto user = testingFixtures->createTestUser(1);
	auto issueResult = testingFixtures->getUserApiKey(user->id);
	auto authHeader = testingFixtures->getUserAuthHeader(issueResult);

	auto rsp = client->warSpy("testKey", issueResult.fullKey);
	OATPP_ASSERT(rsp->getStatusCode() == 200);
	auto body = rsp->readBodyToDto<oatpp::Object<SpyResponseDto>>(objectMapper);
	OATPP_ASSERT(body->importSize == 1)

	auto statsService = MemberStatsService();
	auto stats = statsService.getAllForWar(factionWar->getWarId().value(), factionWar->getEnemyFactionId(1).value());
	OATPP_ASSERT(stats->size() == 1)
	auto stat = stats[0];
	OATPP_ASSERT(stat->total == memberOneSpy->total)
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


		testSocketOk(objectMapper);
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
