#pragma once

#include <iostream>


#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "AuthHandler.hpp"
#include "service/UserService.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class UserController : public oatpp::web::server::api::ApiController
{
	using Action = oatpp::async::Action;
	using __ControllerType = UserController;

public:
	UserController(const std::shared_ptr<ObjectMapper>& objectMapper)
		: ApiController(objectMapper)
	{
		setDefaultAuthorizationHandler(std::make_shared<AuthHandler>());
	}

private:
	UserService m_userService;

	std::shared_ptr<AuthHandler> m_authHandler = std::make_shared<AuthHandler>();

public:
	static std::shared_ptr<UserController> createShared(
		OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)
	)
	{
		return std::make_shared<UserController>(objectMapper);
	}

	const std::shared_ptr<AuthHandler>& authHandler() const { return m_authHandler; }

	ENDPOINT_INFO(deleteUser)
	{
		info->summary = "Delete User";

		auto& authHeader = info->headers.add<oatpp::String>(oatpp::web::protocol::http::Header::AUTHORIZATION);
		authHeader.description = "Bearer token for authentication";
		authHeader.required = true;


		info->addTag("User");
	}

	ENDPOINT_ASYNC("GET", "/delete", deleteUser)
	{
		ENDPOINT_ASYNC_INIT(deleteUser);

		Action act() override
		{
			const oatpp::String authHeader = request->getHeader(oatpp::web::protocol::http::Header::AUTHORIZATION);

			auto baseObj = controller->authHandler()->authorize(authHeader);
			auto authObj = std::dynamic_pointer_cast<AuthObject>(baseObj);

			controller->m_userService.removeById(authObj->userId);
			return _return(controller->createResponse(Status::CODE_200));
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
