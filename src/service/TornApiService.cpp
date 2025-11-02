#include "TornApiService.hpp"

#include <cassert>
#include <iostream>

#include "TornApiClient.hpp"
#include "dto/FactionMemberInfoDto.hpp"
#include "dto/torn_client/TornFactionResponseDto.hpp"
#include "dto/torn_client/TornUserBasicResponseDto.hpp"
#include "dto/torn_client/TornErrorResponseDto.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/client/ApiClient.hpp"


void TornApiService::handleErrorCodes(const oatpp::Object<TornErrorResponseDto>& parsedError)
{
	if (parsedError && parsedError->error && parsedError->error->code)
	{
		const auto code = *parsedError->error->code;
		const auto msg = parsedError->error->error ? *parsedError->error->error : "Unknown Torn API error";

		switch (code)
		{
		case 0: OATPP_ASSERT_HTTP(false, Status::CODE_502, "Unknown error: " + msg);
		case 1: OATPP_ASSERT_HTTP(false, Status::CODE_401, "Key is empty");
		case 2: OATPP_ASSERT_HTTP(false, Status::CODE_401, "Incorrect API key");
		case 3: OATPP_ASSERT_HTTP(false, Status::CODE_400, "Wrong type in request");
		case 4: OATPP_ASSERT_HTTP(false, Status::CODE_400, "Wrong fields in request");
		case 5: OATPP_ASSERT_HTTP(false, Status::CODE_429, "Too many requests");
		case 6: OATPP_ASSERT_HTTP(false, Status::CODE_400, "Incorrect ID");
		case 7: OATPP_ASSERT_HTTP(false, Status::CODE_403, "Incorrect ID-entity relation (private data)");
		case 8: OATPP_ASSERT_HTTP(false, Status::CODE_429, "IP temporarily blocked due to abuse");
		case 9: OATPP_ASSERT_HTTP(false, Status::CODE_503, "API system disabled");
		case 10: OATPP_ASSERT_HTTP(false, Status::CODE_403, "Key owner is in federal jail");
		case 11: OATPP_ASSERT_HTTP(false, Status::CODE_429, "Key change too frequent");
		case 12: OATPP_ASSERT_HTTP(false, Status::CODE_502, "Key read error (database)");
		case 13: OATPP_ASSERT_HTTP(false, Status::CODE_403, "Key temporarily disabled (owner inactive)");
		case 14: OATPP_ASSERT_HTTP(false, Status::CODE_429, "Daily read limit reached");
		case 15: OATPP_ASSERT_HTTP(false, Status::CODE_500, "Temporary test error");
		case 16: OATPP_ASSERT_HTTP(false, Status::CODE_403, "Access level too low for selection");
		case 17: OATPP_ASSERT_HTTP(false, Status::CODE_502, "Backend error occurred");
		case 18: OATPP_ASSERT_HTTP(false, Status::CODE_403, "API key paused by owner");
		case 19: OATPP_ASSERT_HTTP(false, Status::CODE_400, "Must be migrated to crimes 2.0");
		case 20: OATPP_ASSERT_HTTP(false, Status::CODE_400, "Race not yet finished");
		case 21: OATPP_ASSERT_HTTP(false, Status::CODE_400, "Incorrect category");
		case 22: OATPP_ASSERT_HTTP(false, Status::CODE_410, "Selection only available in API v1");
		case 23: OATPP_ASSERT_HTTP(false, Status::CODE_410, "Selection only available in API v2");
		case 24: OATPP_ASSERT_HTTP(false, Status::CODE_503, "Endpoint closed temporarily");
		default: OATPP_ASSERT_HTTP(false, Status::CODE_502, "Unhandled Torn API error code: " + std::to_string(code));
		}
	}

	OATPP_ASSERT_HTTP(false, Status::CODE_502, "Unknown error from upstream Torn API");
}


oatpp::Object<TornFactionResponseDto> TornApiService::getFactionBasic(const std::string& key)
{
	auto rsp = tornApiClient->getFactionBasic(key);
	OATPP_ASSERT_HTTP(rsp && rsp->getStatusCode() == 200, Status::CODE_502, "Upstream call failed");

	oatpp::String body = rsp->readBodyToString();

	return parseSafely<TornFactionResponseDto>(body);
}


oatpp::Object<TornUserBasicResponseDto> TornApiService::getUserBasic(const std::string& key)
{
	auto rsp = tornApiClient->getUserBasic(key);
	OATPP_ASSERT_HTTP(rsp && rsp->getStatusCode() == 200, Status::CODE_502, "Upstream call failed");

	oatpp::String body = rsp->readBodyToString();

	return parseSafely<TornUserBasicResponseDto>(body);
}

oatpp::Object<TornFactionMembersResponse> TornApiService::getFactionMembers(
	const std::string& key, const std::int64_t factionId)
{
	const auto now = std::time(nullptr);

	auto rsp = tornApiClient->getFactionMembers(key, now, factionId);
	OATPP_ASSERT_HTTP(rsp && rsp->getStatusCode() == 200, Status::CODE_502, "Upstream call failed");

	oatpp::String body = rsp->readBodyToString();

	return parseSafely<TornFactionMembersResponse>(body);
}


std::vector<FactionMemberInfo> TornApiService::getFactionMembersCompact(
	const std::string& key, const std::int64_t factionId)
{
	const auto now = std::time(nullptr);

	auto rsp = tornApiClient->getFactionMembers(key, now, factionId);
	OATPP_ASSERT_HTTP(rsp && rsp->getStatusCode() == 200, Status::CODE_502, "Upstream call failed");

	oatpp::String body = rsp->readBodyToString();

	return (parseSafely<TornFactionMembersResponse>(body))->toFactionMemberInfoStruct();
}

oatpp::Object<TornFactionWarResponseDto> TornApiService::getFactionWar(const std::string& key)
{
	auto rsp = tornApiClient->getFactionWars(key);
	OATPP_ASSERT_HTTP(rsp && rsp->getStatusCode() == 200, Status::CODE_502, "Upstream call failed");

	oatpp::String body = rsp->readBodyToString();

	return parseSafely<TornFactionWarResponseDto>(body);
}

std::optional<std::int64_t> TornApiService::getEnemyWarFaction(const std::string& key, const std::int64_t factionId)
{
	auto warInfo = getFactionWar(key);

	if (!warInfo->wars->ranked)
	{
		return std::nullopt;
	}

	const auto& factions = warInfo->wars->ranked->factions;

	OATPP_ASSERT_HTTP(factions->size() == 2, Status::CODE_502, "Torn API returned more than 2 war participants.");

	if (factions[0]->id == factionId)
		return factions[1]->id;

	assert(factions[1]->id == factionId);
	return factions[0]->id;
}
