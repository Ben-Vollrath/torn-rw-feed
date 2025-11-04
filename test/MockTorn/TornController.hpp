#pragma once
#include "FixtureLoader.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class TornController : public oatpp::web::server::api::ApiController {
public:
    TornController(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper) {}

    static std::shared_ptr<TornController> createShared(
        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) {
        return std::make_shared<TornController>(objectMapper);
    }

    ENDPOINT_ASYNC("GET", "/faction/basic", GetFactionBasic) {
        ENDPOINT_ASYNC_INIT(GetFactionBasic)

            Action act() override {
            auto body = loadFile(factionBasicOKFixturePath_);
            return _return(controller->createResponse(Status::CODE_200, body));
        }
    };


    ENDPOINT_ASYNC("GET", "/user/basic", GetUserBasic) {
        ENDPOINT_ASYNC_INIT(GetUserBasic)

            Action act() override {
            auto keyOpt = request->getQueryParameter("key");
            OATPP_ASSERT_HTTP(keyOpt, Status::CODE_400, "Missing 'key' query parameter");

            const std::string key = keyOpt->data();

            static const std::unordered_map<std::string, std::string> fixtures = {
              {"ok",       userBasicOkFixturePath_},
              {"keyError", errorInactiveKey_}
            };

            const auto it = fixtures.find(key);
            OATPP_ASSERT_HTTP(it != fixtures.end(), Status::CODE_400, "Unknown test key");

            const auto body = loadFile(it->second);
            return _return(controller->createResponse(Status::CODE_200, body));
        }
    };
};

#include OATPP_CODEGEN_END(ApiController)
