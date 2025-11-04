#pragma once

#include <iostream>

#include "dto/FactionDto.hpp"
#include "dto/responses/ApiKeyIssueResponseDto.hpp"
#include "service/UserService.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "service/ApiKeyService.hpp"
#include "service/FactionService.hpp"
#include "service/TornApiService.hpp"
#include "AuthHandler.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class AuthController : public oatpp::web::server::api::ApiController
{
	using __ControllerType = AuthController;

public:
	AuthController(const std::shared_ptr<ObjectMapper>& objectMapper)
		: ApiController(objectMapper)
	{
		setDefaultAuthorizationHandler(std::make_shared<AuthHandler>());
	}

private:
	UserService m_userService;
	ApiKeyService m_apiKeyService;
	FactionService m_factionService;
	TornApiService m_tornApiService;

public:
	static std::shared_ptr<AuthController> createShared(
		OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)
	)
	{
		return std::make_shared<AuthController>(objectMapper);
	}

	ENDPOINT_ASYNC("GET", "/auth", loginUser)
	{
		ENDPOINT_ASYNC_INIT(loginUser);

		oatpp::async::Action act() override
		{
			try {
				// TODO use async calls here
				auto tornKey = request->getQueryParameter("torn_key");
				auto userBasics = controller->m_tornApiService.getUserBasic(tornKey);
				auto factionBasics = controller->m_tornApiService.getFactionBasic(tornKey);

				// Update faction information
				oatpp::Object<FactionDto> faction = FactionDto::fromFactionResponse(factionBasics->basic);
				auto createdFaction = controller->m_factionService.upsertById(faction);
				OATPP_ASSERT_HTTP(createdFaction, Status::CODE_500, "Couldn't upsert faction");

				// Update User information
				oatpp::Object<UserDto> newUser = UserDto::createShared();
				newUser->tornKey = tornKey;
				newUser->id = userBasics->profile->id;
				newUser->factionId = factionBasics->basic->id;
				auto createdUser = controller->m_userService.upsertById(newUser);
				OATPP_ASSERT_HTTP(createdUser, Status::CODE_500, "Couldn't upsert user");

				// Issue api key
				auto key = controller->m_apiKeyService.issueKey(createdUser->id);
				auto keyResponse = ApiKeyIssueResponseDto::fromIssueResult(key);
				return _return(controller->createDtoResponse(Status::CODE_200, keyResponse));
			} catch (oatpp::web::protocol::http::HttpError& e) {
				return _return(controller->createResponse(e.getInfo().status, e.getMessage())
				);
			}
		}
	};
};

#include OATPP_CODEGEN_END(ApiController)
