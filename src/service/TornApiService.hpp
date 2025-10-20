#pragma once

#include "TornApiClient.hpp"
#include "dto/torn_client/TornFactionResponseDto.hpp"
#include "dto/torn_client/TornUserBasicResponseDto.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/client/ApiClient.hpp"

class TornApiService
{
private:
	OATPP_COMPONENT(std::shared_ptr<TornApiClient>, tornApiClient);
	OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

	using Status = oatpp::web::protocol::http::Status;
public:
	oatpp::Object<TornFactionResponseDto> getFactionBasic(const std::string& key)
	{
		auto rsp = tornApiClient->getFactionBasic(key);
		OATPP_ASSERT_HTTP(rsp && rsp->getStatusCode() == 200, Status::CODE_502, "Upstream call failed");

		oatpp::String body = rsp->readBodyToString();

		return objectMapper->readFromString<oatpp::Object<TornFactionResponseDto>>(body);
	}


	oatpp::Object<TornUserBasicResponseDto> getUserBasic(const std::string& key)
	{
		auto rsp = tornApiClient->getUserBasic(key);
		OATPP_ASSERT_HTTP(rsp && rsp->getStatusCode() == 200, Status::CODE_502, "Upstream call failed");

		oatpp::String body = rsp->readBodyToString();

		return objectMapper->readFromString<oatpp::Object<TornUserBasicResponseDto>>(body);
	}
};
