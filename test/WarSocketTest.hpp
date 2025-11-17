#pragma once


#include "app/MyApiTestClient.hpp"
#include "oatpp-test/UnitTest.hpp"

class WarSocketTest : public oatpp::test::UnitTest
{
	void testSocketOk();
	void testPostSpyWithRoom(const std::shared_ptr<ApiTestClient> client);
	void testPostSpyWithoutRoom(const std::shared_ptr<ApiTestClient> client);

public:
	WarSocketTest() : UnitTest("TEST[WarSocketTest]")
	{
	}

	void onRun() override;
};
