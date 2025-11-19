#pragma once
#include "MockResponseLoader.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class MockController : public oatpp::web::server::api::ApiController
{
public:
	MockController(const std::shared_ptr<ObjectMapper>& objectMapper)
		: ApiController(objectMapper)
	{
	}

	static std::shared_ptr<MockController> createShared(
		OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
	{
		return std::make_shared<MockController>(objectMapper);
	}

	ENDPOINT_ASYNC("GET", "/user/basic", GetUserBasic)
	{
		ENDPOINT_ASYNC_INIT(GetUserBasic)

		Action act() override
		{
			OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
			auto rsp = mockResponseLoader->getNextResponse();
			OATPP_LOGD("/user/basic", "Response: %s", rsp.c_str());
			return _return(controller->createResponse(Status::CODE_200, rsp));
		}
	};

	ENDPOINT_ASYNC("GET", "/faction/basic", GetFactionBasic)
	{
		ENDPOINT_ASYNC_INIT(GetFactionBasic)

		Action act() override
		{
			OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
			auto rsp = mockResponseLoader->getNextResponse();
			OATPP_LOGD("/faction/basic", "Response: %s", rsp.c_str());
			return _return(controller->createResponse(Status::CODE_200, rsp));
		}
	};

	ENDPOINT_ASYNC("GET", "/faction/wars", GetFactionWars)
	{
		ENDPOINT_ASYNC_INIT(GetFactionWars)

		Action act() override
		{
			OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
			auto rsp = mockResponseLoader->getNextResponse();
			OATPP_LOGD("/faction/wars", "Response: %s", rsp.c_str());
			return _return(controller->createResponse(Status::CODE_200, rsp));
		}
	};

	ENDPOINT_ASYNC("GET", "/faction/{factionId}/members", GetFactionMembers)
	{
		ENDPOINT_ASYNC_INIT(GetFactionMembers)

		Action act() override
		{
			OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
			auto rsp = mockResponseLoader->getNextResponse();
			OATPP_LOGD("/faction/{factionId}/members", "Response: %s", rsp.c_str());
			return _return(controller->createResponse(Status::CODE_200, rsp));
		}
	};

	ENDPOINT_ASYNC("GET", "/api/v1/get-stats", FFScouts) {
		ENDPOINT_ASYNC_INIT(FFScouts)

			Action act() override {
			OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
			auto rsp = mockResponseLoader->getNextResponse();
			OATPP_LOGD("/api/v1/get-stats", "Response: %s", rsp.c_str());
			return _return(controller->createResponse(Status::CODE_200, rsp));
		}
	};


	ENDPOINT_ASYNC("GET", "/api/v2/{key}/spy/faction/{faction_id}", GetSpies) {
		ENDPOINT_ASYNC_INIT(GetSpies)

			Action act() override {
			OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
			auto rsp = mockResponseLoader->getNextResponse();
			OATPP_LOGD("/api/v2/{key}/spy/faction/{faction_id}", "Response: %s", rsp.c_str());
			return _return(controller->createResponse(Status::CODE_200, rsp));
		}
	};

};

#include OATPP_CODEGEN_END(ApiController)
