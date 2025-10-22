#pragma once
#include "FixtureLoader.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)
class TornController : public oatpp::web::server::api::ApiController
{
public:
    TornController(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper)
    {}

public:
    static std::shared_ptr<TornController> createShared(
        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)
    ) {
        return std::make_shared<TornController>(objectMapper);
    }

    ENDPOINT("GET", "/faction/basic", getFactionBasic, QUERY(String, key)) {
        auto body = loadFile(factionBasicOKFixturePath_);
        return createResponse(Status::CODE_200, body);
    }

    ENDPOINT("GET", "/user/basic", getUserBasic, QUERY(String, key)) {
        static const std::unordered_map<std::string, std::string> fixtures = {
            {"ok", userBasicOkFixturePath_},
            {"keyError", errorInactiveKey_}
        };

        auto it = fixtures.find(key);
        OATPP_ASSERT_HTTP(it != fixtures.end(), Status::CODE_400, "Unknown test key");

        const auto body = loadFile(it->second);
        return createResponse(Status::CODE_200, body);
    }


};
#include OATPP_CODEGEN_BEGIN(ApiController)