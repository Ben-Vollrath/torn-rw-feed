#pragma once

#include "TornApiClient.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp-curl/RequestExecutor.hpp"



class TornApiComponent {
public:

	OATPP_CREATE_COMPONENT(std::shared_ptr<TornApiClient>, tornApiClient)([] {
		using namespace oatpp::network;
		using namespace oatpp::web;
		using namespace oatpp::parser;

		auto requestExecutor = oatpp::curl::RequestExecutor::createShared("https://api.torn.com/v2");

		/* get Object Mapper */
		OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

		return TornApiClient::createShared(requestExecutor, objectMapper);
		}());

};