cmake_minimum_required(VERSION 3.15)
project(TornRwFeed VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON) 

find_package(OpenSSL REQUIRED)
find_package(oatpp CONFIG REQUIRED)
find_package(oatpp-swagger CONFIG REQUIRED)
find_package(oatpp-postgresql CONFIG REQUIRED)


add_executable(torn_rw_feed
  src/App.cpp
  src/service/UserService.cpp
)

set(OATPP_SWAGGER_RES_PATH "${CMAKE_SOURCE_DIR}/libs/oatpp-swagger/res")

target_compile_definitions(torn_rw_feed PRIVATE
  OATPP_SWAGGER_RES_PATH="${OATPP_SWAGGER_RES_PATH}"
)

target_link_libraries(torn_rw_feed PRIVATE
  oatpp::oatpp
  oatpp::oatpp-swagger
  oatpp::oatpp-postgresql
)
