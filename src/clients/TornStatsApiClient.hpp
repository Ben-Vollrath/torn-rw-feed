#pragma once

#include "oatpp/web/client/ApiClient.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/web/client/HttpRequestExecutor.hpp"

class TornStatsApiClient : public oatpp::web::client::ApiClient
{
#include OATPP_CODEGEN_BEGIN(ApiClient)
	API_CLIENT_INIT(TornStatsApiClient)

	API_CALL_ASYNC("GET", "/api/v2/{key}/spy/faction/{faction_id}", getSpies, PATH(String, key),
	               PATH(String, faction_id))
};
