#pragma once


#include "app/MyApiTestClient.hpp"
#include "oatpp-test/UnitTest.hpp"

class WarSocketTest : public oatpp::test::UnitTest
{
	void testSocketOk(std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper);
	void testPostSpyWithRoom(const std::shared_ptr<ApiTestClient> client, std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper);
	void testPostSpyWithoutRoom(const std::shared_ptr<ApiTestClient> client, std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper);

public:
	WarSocketTest() : UnitTest("TEST[WarSocketTest]")
	{
	}

	void onRun() override;
};
