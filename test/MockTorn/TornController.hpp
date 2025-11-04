#pragma once
#include "MockResponseLoader.hpp"
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
            OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
            return _return(controller->createResponse(Status::CODE_200, mockResponseLoader->getNextResponse()));
        }
    };


    ENDPOINT_ASYNC("GET", "/user/basic", GetUserBasic) {
        ENDPOINT_ASYNC_INIT(GetUserBasic)

            Action act() override {
            OATPP_COMPONENT(std::shared_ptr<MockResponseLoader>, mockResponseLoader);
            return _return(controller->createResponse(Status::CODE_200, mockResponseLoader->getNextResponse()));
        }
    };
};

#include OATPP_CODEGEN_END(ApiController)
