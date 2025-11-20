#pragma once

#include <oatpp/core/macro/component.hpp>

#include "app/MyApiTestClient.hpp"
#include "oatpp-test/UnitTest.hpp"

class UserControllerTest : public oatpp::test::UnitTest
{
	void testDelete(std::shared_ptr<ApiTestClient> client,
		std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper);

public:
	UserControllerTest() : UnitTest("TEST[UserControllerTest]")
	{
	}

	void onRun() override;
};
