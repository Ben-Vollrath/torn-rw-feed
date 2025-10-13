#pragma once

#include "oatpp/web/client/ApiClient.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/web/client/HttpRequestExecutor.hpp"

class TornApiClient : public oatpp::web::client::ApiClient
{
#include OATPP_CODEGEN_BEGIN(ApiClient)
	API_CLIENT_INIT(TornApiClient)

	API_CALL("GET", "/faction/basic", getFactionBasic, QUERY(String, key))

	API_CALL("GET", "/user/basic", getUserBasic, QUERY(String, key))
};
