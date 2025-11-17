#pragma once

#include <optional>

#include "AppConfig.hpp"
#include "TornApiClient.hpp"
#include "clients/FFScouterApiClient.hpp"
#include "dto/clients/FFScouterResponseDto.hpp"
#include "dto/clients/TornFactionMembersResponseDto.hpp"
#include "dto/clients/TornFactionWarResponseDto.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/client/ApiClient.hpp"


class FFScouterApiService
{
	OATPP_COMPONENT(std::shared_ptr<FFScouterApiClient>, ffScouterApiClient);
	OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);
	OATPP_COMPONENT(std::shared_ptr<AppConfig>, appConfig);

	using Status = oatpp::web::protocol::http::Status;

public:
	oatpp::async::CoroutineStarterForResult<const FFScouterResponseDto&> getScout(
		const oatpp::Object<TornFactionMembersResponse>& memberInfo);
};
