#pragma once

#include <oatpp/core/macro/component.hpp>

#include "app/MyApiTestClient.hpp"
#include "oatpp-test/UnitTest.hpp"

class AuthControllerTest : public oatpp::test::UnitTest
{
	void testAuthOk(std::shared_ptr<ApiTestClient> client,
	                std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper);
	void testAuthError(std::shared_ptr<ApiTestClient> client,
	                   std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper);

public:
	AuthControllerTest() : UnitTest("TEST[AuthControllerTest]")
	{
	}

	void onRun() override;
};
