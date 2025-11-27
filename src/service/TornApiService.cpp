#include "TornApiService.hpp"

#include <cassert>
#include <iostream>

#include "dto/responses/WarStateResponseDto.hpp"
#include "dto/clients/TornFactionResponseDto.hpp"
#include "dto/clients/TornUserBasicResponseDto.hpp"
#include "dto/clients/TornErrorResponseDto.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/client/ApiClient.hpp"


void TornApiService::handleErrorCodes(const oatpp::Object<TornErrorResponseDto>& parsedError)
{
	if (parsedError && parsedError->error && parsedError->error->code)
	{
		switch (parsedError->error->code)
		{
		case 0: OATPP_ASSERT_HTTP(false, Status::CODE_502, "502");
		case 1: OATPP_ASSERT_HTTP(false, Status::CODE_401, "401");
		case 2: OATPP_ASSERT_HTTP(false, Status::CODE_401, "401");
		case 3: OATPP_ASSERT_HTTP(false, Status::CODE_400, "400");
		case 4: OATPP_ASSERT_HTTP(false, Status::CODE_400, "400");
		case 5: OATPP_ASSERT_HTTP(false, Status::CODE_429, "429");
		case 6: OATPP_ASSERT_HTTP(false, Status::CODE_400, "400");
		case 7: OATPP_ASSERT_HTTP(false, Status::CODE_403, "403");
		case 8: OATPP_ASSERT_HTTP(false, Status::CODE_429, "429");
		case 9: OATPP_ASSERT_HTTP(false, Status::CODE_503, "503");
		case 10: OATPP_ASSERT_HTTP(false, Status::CODE_403, "403");
		case 11: OATPP_ASSERT_HTTP(false, Status::CODE_429, "429");
		case 12: OATPP_ASSERT_HTTP(false, Status::CODE_502, "502");
		case 13: OATPP_ASSERT_HTTP(false, Status::CODE_403, "403");
		case 14: OATPP_ASSERT_HTTP(false, Status::CODE_429, "429");
		case 15: OATPP_ASSERT_HTTP(false, Status::CODE_500, "500");
		case 16: OATPP_ASSERT_HTTP(false, Status::CODE_403, "403");
		case 17: OATPP_ASSERT_HTTP(false, Status::CODE_502, "502");
		case 18: OATPP_ASSERT_HTTP(false, Status::CODE_403, "403");
		case 19: OATPP_ASSERT_HTTP(false, Status::CODE_400, "400");
		case 20: OATPP_ASSERT_HTTP(false, Status::CODE_400, "400");
		case 21: OATPP_ASSERT_HTTP(false, Status::CODE_400, "400");
		case 22: OATPP_ASSERT_HTTP(false, Status::CODE_410, "410");
		case 23: OATPP_ASSERT_HTTP(false, Status::CODE_410, "410");
		case 24: OATPP_ASSERT_HTTP(false, Status::CODE_503, "503");
		default: OATPP_ASSERT_HTTP(false, Status::CODE_502, "502");
		}
	}

	OATPP_ASSERT_HTTP(false, Status::CODE_502, "502");
}


oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionResponseDto>&> TornApiService::getFactionBasic(
	const std::string& key)
{
	class GetFactionCoroutine : public ApiActionBase<GetFactionCoroutine, const oatpp::Object<TornFactionResponseDto>&>
	{
		using Base = ApiActionBase;

	public:
		using Base::Base;

	private:
		oatpp::async::Action act() override
		{
			return this->m_client->getFactionBasic(m_key, m_apiService->COMMENT).callbackTo(
				&GetFactionCoroutine::parseResponse);
		}

		oatpp::async::Action parseResponse(const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>& rsp)
		{
			oatpp::String body = rsp->readBodyToString();
			return _return(m_apiService->parseSafely<TornFactionResponseDto>(body));
		}
	};

	return GetFactionCoroutine::startForResult(this, tornApiClient, objectMapper, key);
}


oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornUserBasicResponseDto>&> TornApiService::getUserBasic(
	const std::string& key)
{
	class GetUserCoroutine : public ApiActionBase<GetUserCoroutine, const oatpp::Object<TornUserBasicResponseDto>&>
	{
		using Base = ApiActionBase;

	public:
		using Base::Base;

	private:
		oatpp::async::Action act() override
		{
			return this->m_client->getUserBasic(m_key, m_apiService->COMMENT).callbackTo(
				&GetUserCoroutine::parseResponse);
		}

		oatpp::async::Action parseResponse(const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>& rsp)
		{
			oatpp::String body = rsp->readBodyToString();
			return _return(m_apiService->parseSafely<TornUserBasicResponseDto>(body));
		}
	};

	return GetUserCoroutine::startForResult(this, tornApiClient, objectMapper, key);
}


oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionMembersResponse>&>
TornApiService::getFactionMembers(
	const std::string& key, const std::int64_t factionId)
{
	class GetFactionMembersCoroutine : public FactionApiActionBase<
			GetFactionMembersCoroutine, const oatpp::Object<TornFactionMembersResponse>&>
	{
		using Base = FactionApiActionBase;

	public:
		using Base::Base;

	private:
		oatpp::async::Action act() override
		{
			const auto now = std::time(nullptr);
			return this->m_client->getFactionMembers(m_key, now, m_factionId, m_apiService->COMMENT).callbackTo(
				&GetFactionMembersCoroutine::parseResponse);
		}

		oatpp::async::Action parseResponse(const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>& rsp)
		{
			oatpp::String body = rsp->readBodyToString();
			return _return(m_apiService->parseSafely<TornFactionMembersResponse>(body));
		}
	};

	return GetFactionMembersCoroutine::startForResult(this, tornApiClient, objectMapper, key, factionId);
}

oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionWarResponseDto>&> TornApiService::getFactionWar(
	const std::string& key)
{
	class GetFactionWarCoroutine : public ApiActionBase<
			GetFactionWarCoroutine, const oatpp::Object<TornFactionWarResponseDto>&>
	{
		using Base = ApiActionBase;

	public:
		using Base::Base;

	private:
		oatpp::async::Action act() override
		{
			return this->m_client->getFactionWars(m_key, m_apiService->COMMENT).callbackTo(
				&GetFactionWarCoroutine::parseResponse);
		}

		oatpp::async::Action parseResponse(const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>& rsp)
		{
			oatpp::String body = rsp->readBodyToString();
			return _return(m_apiService->parseSafely<TornFactionWarResponseDto>(body));
		}
	};

	return GetFactionWarCoroutine::startForResult(this, tornApiClient, objectMapper, key);
}

oatpp::async::CoroutineStarterForResult<const std::optional<std::int64_t>&> TornApiService::getEnemyWarFaction(
	const std::string& key, const std::int64_t factionId)
{
	class GetEnemyWarFactionCoroutine : public FactionApiActionBase<
			GetEnemyWarFactionCoroutine, const std::optional<std::int64_t>&>
	{
		using Base = FactionApiActionBase;

	public:
		using Base::Base;

	private:
		oatpp::async::Action act() override
		{
			return m_apiService->getFactionWar(m_key).callbackTo(&GetEnemyWarFactionCoroutine::parseResponse);
		}

		oatpp::async::Action parseResponse(const oatpp::Object<TornFactionWarResponseDto>& warInfo)
		{
			if (!warInfo->wars->ranked)
			{
				return _return(std::nullopt);
			}

			const auto& factions = warInfo->wars->ranked->factions;

			OATPP_ASSERT_HTTP(factions->size() == 2, Status::CODE_502, "502");

			if (factions[0]->id == m_factionId)
				return _return(factions[1]->id);

			assert(factions[1]->id == m_factionId);
			return _return(factions[0]->id);
		}
	};

	return GetEnemyWarFactionCoroutine::startForResult(this, tornApiClient, objectMapper, key, factionId);
}

oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionWarAndMembersResponseDto>&> TornApiService::getFactionWarAndMembers(
	const std::string& key)
{
	class GetWarAndMembersCoroutine : public ApiActionBase<
		GetWarAndMembersCoroutine, const oatpp::Object<TornFactionWarAndMembersResponseDto>&>
	{
		using Base = ApiActionBase;

	public:
		using Base::Base;

	private:
		oatpp::async::Action act() override
		{
			return this->m_client->getFactionSelection(m_key, "wars,members", m_apiService->COMMENT).callbackTo(
				&GetWarAndMembersCoroutine::parseResponse);
		}

		oatpp::async::Action parseResponse(const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>& rsp)
		{
			oatpp::String body = rsp->readBodyToString();
			return _return(m_apiService->parseSafely<TornFactionWarAndMembersResponseDto>(body));
		}
	};

	return GetWarAndMembersCoroutine::startForResult(this, tornApiClient, objectMapper, key);
}
