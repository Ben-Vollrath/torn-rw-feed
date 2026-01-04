#include "WarSocketTest.hpp"

#include "oatpp-websocket/WebSocket.hpp"
#include "oatpp-websocket/Connector.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"

#include <thread>

#include "TestHelper.hpp"
#include "app/TestComponent.hpp"
#include "app/WSListener.hpp"
#include "controller/WarController.hpp"

#include "oatpp-test/web/ClientServerTestRunner.hpp"

#include "MockTorn/MockController.hpp"
#include "util/DtoUtils.hpp"


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
  auto w1 = makeTestWebSocket("/wars/socket?token=" + issueResult.fullKey, clientConnectionProvider);

  oatpp::Object<WarStateResponseDto> msg;
  //factionWarAndMembersOKPath_
  bool got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  OATPP_ASSERT(!msg->members);
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->user, factionWar->members[0], objectMapper))
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->war, factionWar->wars, objectMapper))

  //factionMembersOfflineOKPath_ - members
  w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  OATPP_ASSERT(msg->members->size() == 2);
  OATPP_ASSERT(!msg->war);
  OATPP_ASSERT(!msg->user);
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->members[0], factionMembersOffline->members[0], objectMapper))
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->members[1], factionMembersOffline->members[1], objectMapper))

  //ffscouterScoutOkPath_
  got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  OATPP_ASSERT(!msg->members);
  OATPP_ASSERT(msg->memberStats[std::to_string(memberOneId)]["ffscouter"]->total == memberOneStats);
  OATPP_ASSERT(!msg->memberStats[std::to_string(memberTwoId)]["ffscouter"]->total);

  //factionWarAndMembersChangedScoreOKPath_
  got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  OATPP_ASSERT(!msg->members);
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->user, factionWarChangedScore->members[0], objectMapper))
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->war, factionWarChangedScore->wars, objectMapper))

  //factionMembersOneOnlineOKPath_ - members
  got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  OATPP_ASSERT(msg->members->size() == 1);
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->members[0], factionMembersOnline->members[0], objectMapper))

  //factionMembersOfflineOKPath_ - members
  got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  OATPP_ASSERT(msg->members->size() == 1);
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->members[0], factionMembersOffline->members[0], objectMapper))

  w1.close();
  OATPP_LOGD(TAG, "TestSocketOk Completed");
  std::this_thread::sleep_for(std::chrono::seconds(3)); //Ensure fetcher closes
}

void WarSocketTest::testSocketSendCurrentState(std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
  OATPP_LOGD(TAG, "TestSocketInitialState Started");
  OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
  testingFixtures->reset();

  OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
  mockResponseLoader->setResponsePaths({
    factionWarAndMembersOKPath_, factionMembersOfflineOKPath_, ffscouterScoutOkPath_,
    factionWarAndMembersChangedOKPath_, factionMembersOneOnlineOKPath_, factionWarAndMembersChangedOKPath_, factionMembersOfflineOKPath_ });

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
  auto w1 = makeTestWebSocket("/wars/socket?token=" + issueResult.fullKey, clientConnectionProvider);

  oatpp::Object<WarStateResponseDto> msg;
  //factionWarAndMembersOKPath_
  bool got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  //factionMembersOfflineOKPath_ - members
  w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  //ffscouterScoutOkPath_
  got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);

  auto user2 = testingFixtures->createTestUser(1, memberTwoId);
  auto issueResult2 = testingFixtures->getUserApiKey(user2->id);

  auto w2 = makeTestWebSocket("/wars/socket?token=" + issueResult2.fullKey, clientConnectionProvider);

  got = w2.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->user, factionWar->members[1], objectMapper))
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->war, factionWar->wars, objectMapper))
  OATPP_ASSERT(msg->members->size() == 2);
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->members[0], factionMembersOffline->members[1], objectMapper))
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->members[1], factionMembersOffline->members[0], objectMapper))

  w1.close();
  w2.close();
  OATPP_LOGD(TAG, "TestSocketInitialState Completed");
  std::this_thread::sleep_for(std::chrono::seconds(3)); //Ensure fetcher closes
}

void WarSocketTest::testSocketTooManyRequests(std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
  OATPP_LOGD(TAG, "TestSocketTooManyRequests Started");
  OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
  testingFixtures->reset();

  OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
  mockResponseLoader->setResponsePaths({errorTooManyRequests_, errorTooManyRequests_, factionWarAndMembersOKPath_, factionMembersOfflineOKPath_});

  auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersOKPath_);
  factionWar->members[0]->status->parseLocation();
  auto user = testingFixtures->createTestUser(1, factionWar->members[0]->id);
  auto issueResult = testingFixtures->getUserApiKey(user->id);

  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
  auto w1 = makeTestWebSocket("/wars/socket?token=" + issueResult.fullKey, clientConnectionProvider);

  oatpp::Object<WarStateResponseDto> msg;

  //errorTooManyRequests_
  bool got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  OATPP_ASSERT(msg->error == ErrorMessage::KeyLimit);

  //errorTooManyRequests_
  got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  OATPP_ASSERT(msg->error == ErrorMessage::KeyLimit);

  //factionWarAndMembersOKPath_
  got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  OATPP_ASSERT(!msg->members);
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->user, factionWar->members[0], objectMapper))
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->war, factionWar->wars, objectMapper))

  w1.close();
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
    factionWarAndMembersNoWarOKPath_, factionWarAndMembersNoWarChangedOKPath_, factionWarAndMembersNoWarChangedOKPath_ });

  auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersNoWarOKPath_);
  factionWar->members[0]->status->parseLocation();
  auto factionWarUpdated = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersNoWarChangedOKPath_);
  factionWarUpdated->members[0]->status->parseLocation();
  auto user = testingFixtures->createTestUser(1, factionWar->members[0]->id);
  auto issueResult = testingFixtures->getUserApiKey(user->id);

  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
  auto w1 = makeTestWebSocket("/wars/socket?token=" + issueResult.fullKey, clientConnectionProvider);

  oatpp::Object<WarStateResponseDto> msg;
  //factionWarAndMembersOKPath_
  bool got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  OATPP_ASSERT(!msg->members);
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->user, factionWar->members[0], objectMapper))
    OATPP_ASSERT(dtoFieldsEqualLogger(msg->war, factionWar->wars, objectMapper))

  //factionWarAndMembersOKPath_
  got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  OATPP_ASSERT(!msg->members);
  OATPP_ASSERT(!msg->war);
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->user, factionWarUpdated->members[0], objectMapper))

  w1.close();
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
    factionWarAndMembersOKPath_, factionMembersOfflineOKPath_, ffscouterScoutOkPath_, factionWarAndMembersOKPath_});

  auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersOKPath_);
  auto user = testingFixtures->createTestUser(1);
  auto issueResult = testingFixtures->getUserApiKey(user->id);

  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
  auto w1 = makeTestWebSocket("/wars/socket?token=" + issueResult.fullKey, clientConnectionProvider);

  oatpp::Object<WarStateResponseDto> msg;
  //factionWarAndMembersOKPath_
  bool got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  //factionMembersOfflineOKPath_
  got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  //ffscouterScoutOkPath_
  got = w1.listener->waitForNext(msg, std::chrono::seconds(500));

  auto dto = UpdateTargetDto::createShared();
  dto->targetId = 4;
  dto->updateType = TargetUpdateType::ADD;
  auto text = objectMapper->writeToString(dto);
  w1.socket->sendOneFrameText(text);

  got = w1.listener->waitForNext(msg, std::chrono::seconds(5));
  OATPP_ASSERT(got);
  OATPP_ASSERT(msg->targets)
  OATPP_ASSERT(msg->targets->size() == 1);
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->targets[0], dto, objectMapper));

  w1.close();
  
  std::this_thread::sleep_for(std::chrono::seconds(3)); //Ensure fetcher closes

  auto targetService = TargetService();

  auto db = targetService.getAllForUser(factionWar->getEnemyFactionId(user->factionId).value(), user->id);
  OATPP_ASSERT(db->targets_set->size() == 1)
  OATPP_LOGD(TAG, "testSocketTargetsUpdate Completed");
}

void WarSocketTest::testSocketTargetsUpdateUserDeleted(std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
  OATPP_LOGD(TAG, "testSocketTargetsUpdateUserDeleted Started");
  OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
  testingFixtures->reset();

  OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
  mockResponseLoader->setResponsePaths({
    factionWarAndMembersOKPath_, factionMembersOfflineOKPath_, ffscouterScoutOkPath_, factionWarAndMembersOKPath_ });

  auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersOKPath_);
  auto user = testingFixtures->createTestUser(1);
  auto issueResult = testingFixtures->getUserApiKey(user->id);

  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
  auto w1 = makeTestWebSocket("/wars/socket?token=" + issueResult.fullKey, clientConnectionProvider);

  oatpp::Object<WarStateResponseDto> msg;
  //factionWarAndMembersOKPath_
  bool got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  //factionMembersOfflineOKPath_
  got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  //ffscouterScoutOkPath_
  got = w1.listener->waitForNext(msg, std::chrono::seconds(500));

  auto dto = UpdateTargetDto::createShared();
  dto->targetId = 4;
  dto->updateType = TargetUpdateType::ADD;
  auto text = objectMapper->writeToString(dto);
  w1.socket->sendOneFrameText(text);

  got = w1.listener->waitForNext(msg, std::chrono::seconds(5));
  OATPP_ASSERT(got);
  OATPP_ASSERT(msg->targets)
    OATPP_ASSERT(msg->targets->size() == 1);
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->targets[0], dto, objectMapper));

  testingFixtures->deleteTestUser(user->id);

  w1.close();

  std::this_thread::sleep_for(std::chrono::seconds(3)); //Ensure fetcher closes

  OATPP_LOGD(TAG, "testSocketTargetsUpdateUserDeleted Completed");
}

void WarSocketTest::testSocketTargetsLoad(std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
  OATPP_LOGD(TAG, "testSocketTargetsLoad Started");
  OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
  testingFixtures->reset();

  OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
  mockResponseLoader->setResponsePaths({
    factionWarAndMembersOKPath_, factionMembersOfflineOKPath_, ffscouterScoutOkPath_, factionWarAndMembersOKPath_ });

  auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersOKPath_);
  auto user = testingFixtures->createTestUser(1);
  auto issueResult = testingFixtures->getUserApiKey(user->id);
  auto targetDbDto = TargetsDbDto::createFromFactionAndUser(factionWar->getEnemyFactionId(user->factionId).value(), user->id);
  auto target = TargetsDto::fromDbDto(targetDbDto);
  target->targets_set->insert(4);
  auto targets = testingFixtures->createTargets(target->getDbDto());
  auto targetUpdate = UpdateTargetDto::fromTargetsDto(target);

  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
  auto w1 = makeTestWebSocket("/wars/socket?token=" + issueResult.fullKey, clientConnectionProvider);

  oatpp::Object<WarStateResponseDto> msg;
  //factionWarAndMembersOKPath_
  bool got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->targets, targetUpdate, objectMapper))

  w1.close();

  std::this_thread::sleep_for(std::chrono::seconds(3)); //Ensure fetcher closes
  OATPP_LOGD(TAG, "testSocketTargetsLoad Completed");
}

void WarSocketTest::testSocketUserSwitchedFaction(std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
  OATPP_LOGD(TAG, "TestSocketOk Started");
  OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
  testingFixtures->reset();

  OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
  mockResponseLoader->setResponsePaths({
    factionWarAndMembersOKPath_, factionMembersOfflineOKPath_,});

  auto factionWar = mockResponseLoader->loadDtoFromFile<oatpp::Object<TornFactionWarAndMembersResponseDto>>(factionWarAndMembersOKPath_);
  factionWar->members[0]->status->parseLocation();

  auto user = testingFixtures->createTestUser(factionWar->basic->id+5);
  auto issueResult = testingFixtures->getUserApiKey(user->id);

  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, clientConnectionProvider);
  auto w1 = makeTestWebSocket("/wars/socket?token=" + issueResult.fullKey, clientConnectionProvider);

  std::this_thread::sleep_for(std::chrono::seconds(1));
  auto userService = UserService();
  auto dbUser = userService.getById(user->id);
  OATPP_ASSERT(dbUser->factionId == factionWar->basic->id);

  w1.close();
  OATPP_LOGD(TAG, "TestSocketOk Completed");
  std::this_thread::sleep_for(std::chrono::seconds(3)); //Ensure fetcher closes
}

void WarSocketTest::testPostSpyWithRoom(const std::shared_ptr<ApiTestClient> client, std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
{
  OATPP_LOGD(TAG, "TestPostSpyWithRoom Started");
  OATPP_COMPONENT(std::shared_ptr<TestingFixtures>, testingFixtures);
  testingFixtures->reset();

  OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
  mockResponseLoader->setResponsePaths({
    factionWarAndMembersOKPath_, tornStatsSpyOkPath_, factionMembersOfflineOKPath_, ffscouterScoutOkPath_,factionWarAndMembersOKPath_,
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
  auto w1 = makeTestWebSocket("/wars/socket?token=" + issueResult.fullKey, clientConnectionProvider);

  oatpp::Object<WarStateResponseDto> msg;
  //factionWarAndMembersOKPath_
  bool got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  OATPP_ASSERT(!msg->members);
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->user, factionWar->members[0], objectMapper))
  OATPP_ASSERT(dtoFieldsEqualLogger(msg->war, factionWar->wars, objectMapper))

  auto rsp = client->warSpy("testKey", issueResult.fullKey);
  OATPP_ASSERT(rsp->getStatusCode() == 200);
  auto body = rsp->readBodyToDto<oatpp::Object<SpyResponseDto>>(objectMapper);
  OATPP_ASSERT(body->importSize == 1)

  //tornStatsSpyOkPath_
  got = w1.listener->waitForNext(msg, std::chrono::seconds(500));
  OATPP_ASSERT(got);
  OATPP_ASSERT(msg->memberStats[std::to_string(memberOneId)]["tornstatsspies"]->total == memberOneSpy->total);
  OATPP_ASSERT(!msg->memberStats[std::to_string(memberTwoId)]);

  w1.close();
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
    testSocketSendCurrentState(objectMapper);
    testSocketTooManyRequests(objectMapper);
    testSocketTargetsUpdate(objectMapper);
    testSocketTargetsUpdateUserDeleted(objectMapper);
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
