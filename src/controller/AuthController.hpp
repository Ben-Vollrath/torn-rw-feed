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
public:
    AuthController(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper)
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
    ) {
        return std::make_shared<AuthController>(objectMapper);
    }


    ENDPOINT("GET", "auth/{tornKey}", loginUser,
        PATH(String, tornKey)) {

        auto userBasics = m_tornApiService.getUserBasic(tornKey);
        auto factionBasics = m_tornApiService.getFactionBasic(tornKey);

		// Update faction information
        oatpp::Object<FactionDto> faction = FactionDto::fromFactionResponse(factionBasics->basic);
        auto createdFaction = m_factionService.upsertById(faction);
        OATPP_ASSERT_HTTP(createdFaction, Status::CODE_500, "Couldn't upsert faction");

        // Update User information
        oatpp::Object<UserDto> newUser = UserDto::createShared();
        newUser->tornKey = tornKey;
        newUser->id = userBasics->profile->id;
        newUser->factionId = factionBasics->basic->id;
        auto createdUser = m_userService.upsertById(newUser);
        OATPP_ASSERT_HTTP(createdUser, Status::CODE_500, "Couldn't upsert user");

        // Issue api key
        auto key = m_apiKeyService.issueKey(createdUser->id);
        auto keyResponse = ApiKeyIssueResponseDto::fromIssueResult(key);
        return createDtoResponse(Status::CODE_200, keyResponse);
    }

};
#include OATPP_CODEGEN_BEGIN(ApiController)