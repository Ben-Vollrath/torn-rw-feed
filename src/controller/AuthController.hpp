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

	ENDPOINT_INFO(authorizeUser)
	{
		info->summary = "Authorizes the user and returns API Key";
		info->queryParams.add<oatpp::String>("torn_key");
		info->addResponse<Object<ApiKeyIssueResponseDto>>(Status::CODE_200, "application/json");
		info->addTag("Auth");
	}

	ENDPOINT_ASYNC("GET", "/auth", authorizeUser)
	{
		std::string m_tornKey;
		oatpp::Int64 m_factionId;
		ENDPOINT_ASYNC_INIT(authorizeUser);

		oatpp::async::Action act() override
		{
			m_tornKey = request->getQueryParameter("torn_key");
			return controller->m_tornApiService.getFactionBasic(m_tornKey).
			                   callbackTo(&authorizeUser::upsertFaction);
		}

		oatpp::async::Action upsertFaction(const oatpp::Object<TornFactionResponseDto>& factionBasics)
		{
			// Update faction information
			oatpp::Object<FactionDto> faction = FactionDto::fromFactionResponse(factionBasics->basic);
			auto createdFaction = controller->m_factionService.upsertById(faction);
			OATPP_ASSERT_HTTP(createdFaction, Status::CODE_500, "502");

			m_factionId = factionBasics->basic->id;
			return controller->m_tornApiService.getUserBasic(m_tornKey).
			                   callbackTo(&authorizeUser::createUserAndRespond);
		}

		oatpp::async::Action createUserAndRespond(const oatpp::Object<TornUserBasicResponseDto>& userBasics)
		{
			// Update User information
			oatpp::Object<UserDto> newUser = UserDto::createShared();
			newUser->tornKey = m_tornKey;
			newUser->id = userBasics->profile->id;
			newUser->factionId = m_factionId;
			auto createdUser = controller->m_userService.upsertById(newUser);
			OATPP_ASSERT_HTTP(createdUser, Status::CODE_500, "500");

			// Issue api key
			auto key = controller->m_apiKeyService.issueKey(createdUser->id);
			auto keyResponse = ApiKeyIssueResponseDto::fromIssueResult(key);
			return _return(controller->createDtoResponse(Status::CODE_200, keyResponse));
		}


		oatpp::async::Action handleError(Error* e) override
		{
			// Workaround until fix oatpp-1.4.0
			auto status = Status(std::atoi(e->what()), e->what());
			return _return(controller->createResponse(status, "Error occured."));
		}
	};
};

#include OATPP_CODEGEN_END(ApiController)
