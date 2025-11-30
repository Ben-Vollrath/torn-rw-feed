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
#include "util/DtoUtils.hpp"

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
	OATPP_LOGD(TAG, "TestSocketOk Started"); 
	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
	testingFixtures->reset();

	OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
	mockResponseLoader->setResponsePaths({
		factionWarAndMembersOKPath_, factionMembersOfflineOKPath_, ffscouterScoutOkPath_, 
		factionWarAndMembersChangedOKPath_, factionMembersOneOnlineOKPath_, factionWarAndMembersChangedOKPath_, factionMembersOfflineOKPath_});

	auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersOKPath_);
	factionWar->members[0]->status->parseLocation();
	auto factionWarChangedScore = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersChangedOKPath_);
	factionWarChangedScore->members[0]->status->parseLocation();
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

	auto user = testingFixtures->createTestUser(1, memberOneId);
	auto issueResult = testingFixtures->getUserApiKey(user->id);

	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
	auto connector = oatpp::websocket::Connector::createShared(clientConnectionProvider);
	auto connection = connector->connect("/wars/socket?token=" + issueResult.fullKey);

	OATPP_LOGI(TAG, "Connected");

	auto socket = oatpp::websocket::WebSocket::createShared(connection, true);

	auto listener = std::make_shared<WSListener>();
	socket->setListener(listener);

	std::thread pump([&] { socket->listen(); });

	oatpp::Object<WarStateResponseDto> msg;
	//factionWarAndMembersOKPath_
	bool got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(!msg->members);
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->user, factionWar->members[0], objectMapper))
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->war, factionWar->wars, objectMapper))

	//factionMembersOfflineOKPath_ - members
	listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->members->size() == 2);
	OATPP_ASSERT(!msg->war);
	OATPP_ASSERT(!msg->user);
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->members[0], factionMembersOffline->members[0], objectMapper))
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->members[1], factionMembersOffline->members[1], objectMapper))

	//ffscouterScoutOkPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(!msg->members);
	OATPP_ASSERT(msg->memberStats[std::to_string(memberOneId)]->total == memberOneStats);
	OATPP_ASSERT(!msg->memberStats[std::to_string(memberTwoId)]->total);

	//factionWarAndMembersChangedScoreOKPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(!msg->members);
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->user, factionWarChangedScore->members[0], objectMapper))
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->war, factionWarChangedScore->wars, objectMapper))

	//factionMembersOneOnlineOKPath_ - members
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->members->size() == 1);
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->members[0], factionMembersOnline->members[0], objectMapper))

	//factionMembersOfflineOKPath_ - members
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->members->size() == 1);
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->members[0], factionMembersOffline->members[0], objectMapper))

	socket->sendClose(1000, "test done");
	if (pump.joinable()) pump.join();
	OATPP_LOGD(TAG, "TestSocketOk Completed");
	std::this_thread::sleep_for(std::chrono::seconds(3)); //Ensure fetcher closes
}

void WarSocketTest::testSocketTooManyRequests(std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
	OATPP_LOGD(TAG, "TestSocketTooManyRequests Started");
	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
	testingFixtures->reset();

	OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
	mockResponseLoader->setResponsePaths({errorTooManyRequests_, factionWarAndMembersOKPath_, factionMembersOfflineOKPath_});

	auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersOKPath_);
	factionWar->members[0]->status->parseLocation();
	auto user = testingFixtures->createTestUser(1);
	auto issueResult = testingFixtures->getUserApiKey(user->id);

	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
	auto connector = oatpp::websocket::Connector::createShared(clientConnectionProvider);
	auto connection = connector->connect("/wars/socket?token=" + issueResult.fullKey);


	auto socket = oatpp::websocket::WebSocket::createShared(connection, true);

	auto listener = std::make_shared<WSListener>();
	socket->setListener(listener);

	std::thread pump([&] { socket->listen(); });

	oatpp::Object<WarStateResponseDto> msg;

	//factionWarAndMembersOKPath_
	bool got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(!msg->members);
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->user, factionWar->members[0], objectMapper))
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->war, factionWar->wars, objectMapper))

	socket->sendClose(1000, "test done");
	if (pump.joinable()) pump.join();
	OATPP_LOGD(TAG, "TestSocketTooManyRequests Completed");
	std::this_thread::sleep_for(std::chrono::seconds(3)); //Ensure fetcher closes
}

void WarSocketTest::testSocketNoWar(std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
	OATPP_LOGD(TAG, "TestSocketNoWar Started");
	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
	testingFixtures->reset();

	OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
	mockResponseLoader->setResponsePaths({
		factionWarAndMembersNoWarOKPath_, factionWarAndMembersNoWarChangedOKPath_ });

	auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersNoWarOKPath_);
	factionWar->members[0]->status->parseLocation();
	auto factionWarUpdated = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersNoWarChangedOKPath_);
	factionWarUpdated->members[0]->status->parseLocation();
	auto user = testingFixtures->createTestUser(1, factionWar->members[0]->id);
	auto issueResult = testingFixtures->getUserApiKey(user->id);

	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
	auto connector = oatpp::websocket::Connector::createShared(clientConnectionProvider);
	auto connection = connector->connect("/wars/socket?token=" + issueResult.fullKey);

	OATPP_LOGI(TAG, "Connected");

	auto socket = oatpp::websocket::WebSocket::createShared(connection, true);

	auto listener = std::make_shared<WSListener>();
	socket->setListener(listener);

	std::thread pump([&] { socket->listen(); });

	oatpp::Object<WarStateResponseDto> msg;
	//factionWarAndMembersOKPath_
	bool got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(!msg->members);
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->user, factionWar->members[0], objectMapper))
		OATPP_ASSERT(dtoFieldsEqualLogger(msg->war, factionWar->wars, objectMapper))

	//factionWarAndMembersOKPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(!msg->members);
	OATPP_ASSERT(!msg->war);
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->user, factionWarUpdated->members[0], objectMapper))

	socket->sendClose(1000, "test done");
	if (pump.joinable()) pump.join();
	OATPP_LOGD(TAG, "TestSocketNoWar Completed");
	std::this_thread::sleep_for(std::chrono::seconds(3)); //Ensure fetcher closes
}

void WarSocketTest::testSocketTargetsUpdate(std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
	OATPP_LOGD(TAG, "testSocketTargetsUpdate Started");
	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
	testingFixtures->reset();

	OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
	mockResponseLoader->setResponsePaths({
		factionWarAndMembersOKPath_, factionMembersOfflineOKPath_, ffscouterScoutOkPath_, });

	auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersOKPath_);
	auto user = testingFixtures->createTestUser(1);
	auto issueResult = testingFixtures->getUserApiKey(user->id);

	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
	auto connector = oatpp::websocket::Connector::createShared(clientConnectionProvider);
	auto connection = connector->connect("/wars/socket?token=" + issueResult.fullKey);

	OATPP_LOGI(TAG, "Connected");

	auto socket = oatpp::websocket::WebSocket::createShared(connection, true);

	auto listener = std::make_shared<WSListener>();
	socket->setListener(listener);

	std::thread pump([&] { socket->listen(); });

	oatpp::Object<WarStateResponseDto> msg;
	//factionWarAndMembersOKPath_
	bool got = listener->waitForNext(msg, std::chrono::seconds(500));
	//factionMembersOfflineOKPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	//ffscouterScoutOkPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));

	auto dto = UpdateTargetDto::createShared();
	dto->targetId = 4;
	dto->updateType = TargetUpdateType::ADD;
	auto text = objectMapper->writeToString(dto);
	socket->sendOneFrameText(text);


	got = listener->waitForNext(msg, std::chrono::seconds(5));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->targets)
	OATPP_ASSERT(msg->targets->size() == 1);
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->targets[0], dto, objectMapper));

	socket->sendClose(1000, "test done");
	if (pump.joinable()) pump.join();
	
	std::this_thread::sleep_for(std::chrono::seconds(3)); //Ensure fetcher closes

	auto targetService = TargetService();

	auto db = targetService.getAllForUser(factionWar->getEnemyFactionId(user->factionId).value(), user->id);
	OATPP_ASSERT(db->targets_set->size() == 1)
	OATPP_LOGD(TAG, "testSocketTargetsUpdate Completed");
}

void WarSocketTest::testSocketTargetsLoad(std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
	OATPP_LOGD(TAG, "testSocketTargetsLoad Started");
	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
	testingFixtures->reset();

	OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
	mockResponseLoader->setResponsePaths({
		factionWarAndMembersOKPath_, factionMembersOfflineOKPath_, ffscouterScoutOkPath_, });

	auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersOKPath_);
	auto user = testingFixtures->createTestUser(1);
	auto issueResult = testingFixtures->getUserApiKey(user->id);
	auto targetDbDto = TargetsDbDto::createFromFactionAndUser(factionWar->getEnemyFactionId(user->factionId).value(), user->id);
	auto target = TargetsDto::fromDbDto(targetDbDto);
	target->targets_set->insert(4);
	auto targets = testingFixtures->createTargets(target->getDbDto());
	auto targetUpdate = UpdateTargetDto::fromTargetsDto(target);

	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
	auto connector = oatpp::websocket::Connector::createShared(clientConnectionProvider);
	auto connection = connector->connect("/wars/socket?token=" + issueResult.fullKey);

	OATPP_LOGI(TAG, "Connected");

	auto socket = oatpp::websocket::WebSocket::createShared(connection, true);

	auto listener = std::make_shared<WSListener>();
	socket->setListener(listener);

	std::thread pump([&] { socket->listen(); });

	oatpp::Object<WarStateResponseDto> msg;
	//factionWarAndMembersOKPath_
	bool got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->targets, targetUpdate, objectMapper))

	socket->sendClose(1000, "test done");
	if (pump.joinable()) pump.join();

	std::this_thread::sleep_for(std::chrono::seconds(3)); //Ensure fetcher closes
	OATPP_LOGD(TAG, "testSocketTargetsLoad Completed");
}

void WarSocketTest::testPostSpyWithRoom(const std::shared_ptr<ApiTestClient> client, std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
	OATPP_LOGD(TAG, "TestPostSpyWithRoom Started");
	OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
	testingFixtures->reset();

	OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
	mockResponseLoader->setResponsePaths({
		factionWarAndMembersOKPath_, factionMembersOfflineOKPath_, ffscouterScoutOkPath_, tornStatsSpyOkPath_, factionWarAndMembersOKPath_, factionWarAndMembersOKPath_
		});
	auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersOKPath_);
	factionWar->members[0]->status->parseLocation();
	auto factionWarChangedScore = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersChangedOKPath_);
	factionWarChangedScore->members[0]->status->parseLocation();

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

	auto user = testingFixtures->createTestUser(1, factionWar->members[0]->id);
	auto issueResult = testingFixtures->getUserApiKey(user->id);
	auto authHeader = testingFixtures->getUserAuthHeader(user->id);

	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
	auto connector = oatpp::websocket::Connector::createShared(clientConnectionProvider);
	auto connection = connector->connect("/wars/socket?token=" + issueResult.fullKey);

	OATPP_LOGI(TAG, "Connected");

	auto socket = oatpp::websocket::WebSocket::createShared(connection, true);

	auto listener = std::make_shared<WSListener>();
	socket->setListener(listener);

	std::thread pump([&] { socket->listen(); });

	oatpp::Object<WarStateResponseDto> msg;
	//factionWarAndMembersOKPath_
	bool got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(!msg->members);
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->user, factionWar->members[0], objectMapper))
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->war, factionWar->wars, objectMapper))

	//factionMembersOfflineOKPath_
	got = listener->waitForNext(msg, std::chrono::seconds(500));
	OATPP_ASSERT(got);
	OATPP_ASSERT(msg->members->size() == 2);
	OATPP_ASSERT(dtoFieldsEqualLogger(msg->members[0], factionMembersOffline->members[0], objectMapper))

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
	OATPP_ASSERT(!msg->memberStats[std::to_string(memberTwoId)]);

	socket->sendClose(1000, "test done");
	if (pump.joinable()) pump.join();
	OATPP_LOGD(TAG, "TestPostSpyWithRoom Completed");
	std::this_thread::sleep_for(std::chrono::seconds(3)); //Ensure fetcher closes
}

void WarSocketTest::testPostSpyWithoutRoom(const std::shared_ptr<ApiTestClient> client, std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
	OATPP_LOGD(TAG, "TestPostSpyWithoutRoom Started");
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
	OATPP_LOGD(TAG, "TestPostSpyWithoutRoom Completed");
	std::this_thread::sleep_for(std::chrono::seconds(3)); //Ensure fetcher closes
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
		testSocketTooManyRequests(objectMapper);
		testSocketTargetsUpdate(objectMapper);
		testSocketTargetsLoad(objectMapper);
		testSocketNoWar(objectMapper);
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
