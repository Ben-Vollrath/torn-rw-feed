cmake_minimum_required(VERSION 3.15)
project(TornRwFeed VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON) 
set(CMAKE_CXX_EXTENSIONS ON)  

find_package(OpenSSL REQUIRED)
find_package(oatpp CONFIG REQUIRED)
find_package(oatpp-swagger CONFIG REQUIRED)
find_package(oatpp-postgresql CONFIG REQUIRED)
find_package(oatpp-curl CONFIG REQUIRED)
find_package(oatpp-websocket CONFIG REQUIRED)
find_package(cryptopp CONFIG REQUIRED)

add_executable(torn_rw_feed
  src/App.cpp
 "src/service/ApiKeyService.cpp" "src/service/TornApiService.cpp" "src/war/Room.cpp" "src/war/Lobby.cpp" "src/war/Peer.cpp" "src/service/TornApiServiceKeyManaged.cpp" "src/service/UserService.cpp"   "src/service/FFScouterApiService.cpp" "src/service/MemberStatsService.cpp" "src/service/TornStatsApiService.cpp" )

target_include_directories(torn_rw_feed PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(torn_rw_feed PRIVATE
  oatpp::oatpp
  oatpp::oatpp-swagger
  oatpp::oatpp-postgresql
  oatpp::oatpp-curl
  oatpp::oatpp-websocket
  cryptopp::cryptopp
)


enable_testing()
add_executable(torn_rw_feed_test
	test/tests.cpp
 "src/service/ApiKeyService.cpp" "test/AuthControllerTest.cpp" "src/service/TornApiService.cpp" "src/war/Room.cpp" "src/war/Lobby.cpp" "src/war/Peer.cpp" "src/service/TornApiServiceKeyManaged.cpp" "src/service/UserService.cpp"  "test/WarSocketTest.cpp" "test/app/WSListener.cpp"  "src/service/FFScouterApiService.cpp" "src/service/MemberStatsService.cpp" "src/service/TornStatsApiService.cpp" "test/LocationParserTest.cpp" )

add_definitions(-DMOCK_RESPONSE_BASE="${CMAKE_SOURCE_DIR}/test/MockTorn/mock_responses/")

target_include_directories(torn_rw_feed_test PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(torn_rw_feed_test PRIVATE
  oatpp::oatpp
  oatpp::oatpp-swagger
  oatpp::oatpp-postgresql
  oatpp::oatpp-curl
  oatpp::oatpp-websocket
  cryptopp::cryptopp
)

enable_testing()
add_test(NAME TornTests COMMAND torn_rw_feed_test)