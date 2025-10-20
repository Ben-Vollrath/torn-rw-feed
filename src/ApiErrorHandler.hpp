#pragma once
#include "oatpp/web/protocol/http/outgoing/ResponseFactory.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"

class ApiErrorHandler : public oatpp::web::server::handler::ErrorHandler
{
	virtual
		std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>
		handleError(const oatpp::web::protocol::http::Status& status, const oatpp::String& message, const Headers& headers)
	{
		auto id = oatpp::String(std::to_string(std::time(nullptr)).c_str());
		OATPP_LOGE("HttpError", "status=%d id=%s msg=%s", status.code, id->c_str(), message ? message->c_str() : "");

		return oatpp::web::protocol::http::outgoing::ResponseFactory::createResponse(status, message);
	}

};