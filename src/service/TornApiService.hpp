#pragma once

#include "TornApiClient.hpp"
#include "dto/torn_client/TornErrorResponseDto.hpp"
#include "dto/torn_client/TornFactionResponseDto.hpp"
#include "dto/torn_client/TornUserBasicResponseDto.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/client/ApiClient.hpp"

class TornApiService
{
	OATPP_COMPONENT(std::shared_ptr<TornApiClient>, tornApiClient);
	OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

	using Status = oatpp::web::protocol::http::Status;

	template <typename DtoT>
	oatpp::Object<DtoT> parseSafely(const oatpp::String& body)
	{
		auto rsp = objectMapper->readFromString<oatpp::Object<TornErrorResponseDto>>(body);

		if (rsp && rsp->error)
		{
			handleErrorCodes(rsp); // Will throw
		}

		return objectMapper->readFromString<oatpp::Object<DtoT>>(body);
	}

	void handleErrorCodes(const oatpp::Object<TornErrorResponseDto>& parsedError);
public:
	oatpp::Object<TornFactionResponseDto> getFactionBasic(const std::string& key);
	oatpp::Object<TornUserBasicResponseDto> getUserBasic(const std::string& key);
};

