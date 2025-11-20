#ifndef MyApiTestClient_hpp
#define MyApiTestClient_hpp

#include "oatpp/web/client/ApiClient.hpp"
#include "oatpp/core/macro/codegen.hpp"

/* Begin Api Client code generation */
#include OATPP_CODEGEN_BEGIN(ApiClient)

/**
 * Test API client.
 * Use this client to call application APIs.
 */
class ApiTestClient : public oatpp::web::client::ApiClient
{
	API_CLIENT_INIT(ApiTestClient)

	API_CALL("GET", "/auth", auth, QUERY(String, torn_key))
	API_CALL("GET", "/delete", deleteUser, HEADER(String, key, "Authorization"))
	API_CALL("POST", "/wars/spy", warSpy, QUERY(String, torn_stats_key), HEADER(String, key, "Authorization"))
};

/* End Api Client code generation */
#include OATPP_CODEGEN_END(ApiClient)

#endif // MyApiTestClient_hpp
