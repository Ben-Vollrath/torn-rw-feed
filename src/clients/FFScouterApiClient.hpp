#pragma once

#include "oatpp/web/client/ApiClient.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/web/client/HttpRequestExecutor.hpp"

class FFScouterApiClient : public oatpp::web::client::ApiClient
{
#include OATPP_CODEGEN_BEGIN(ApiClient)
	API_CLIENT_INIT(FFScouterApiClient)

	API_CALL_ASYNC("GET", "/api/v1/get-stats", getScouts, QUERY(String, key), QUERY(String, targets))
};
