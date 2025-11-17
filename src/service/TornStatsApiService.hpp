#pragma once

#include <optional>

#include "AppConfig.hpp"
#include "TornApiClient.hpp"
#include "clients/TornStatsApiClient.hpp"
#include "dto/clients/FFScouterResponseDto.hpp"
#include "dto/clients/TornFactionMembersResponseDto.hpp"
#include "dto/clients/TornFactionWarResponseDto.hpp"
#include "dto/clients/TornStatsSpieResponseDto.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/client/ApiClient.hpp"


class TornStatsApiService
{
	OATPP_COMPONENT(std::shared_ptr<TornStatsApiClient>, tornStatsApiClient);
	OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

	using Status = oatpp::web::protocol::http::Status;

public:
	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornStatsSpyResponseDto>&> getSpies(
		std::string key, std::string factionId);
};
