#pragma once

#include <optional>

#include "TornApiClient.hpp"
#include "dto/torn_client/TornErrorResponseDto.hpp"
#include "dto/torn_client/TornFactionMembersResponseDto.hpp"
#include "dto/torn_client/TornFactionResponseDto.hpp"
#include "dto/torn_client/TornFactionWarResponseDto.hpp"
#include "dto/torn_client/TornUserBasicResponseDto.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/client/ApiClient.hpp"


class TornApiService
{
	OATPP_COMPONENT(std::shared_ptr<TornApiClient>, tornApiClient);
	OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

	using Status = oatpp::web::protocol::http::Status;

	template <typename T, typename TDto>
	class ApiActionBase : public oatpp::async::CoroutineWithResult<T, TDto>
	{
	protected:
		TornApiService* m_apiService;
		std::shared_ptr<TornApiClient> m_client;
		std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_om;
		std::string m_key;

	public:
		ApiActionBase(TornApiService* apiService, std::shared_ptr<TornApiClient> client,
		              std::shared_ptr<oatpp::data::mapping::ObjectMapper> om, std::string key)
			: m_apiService(apiService), m_client(std::move(client)), m_om(std::move(om)), m_key(std::move(key))
		{
		}

	private:
		oatpp::async::Action act() override = 0;
	};

	template <typename T, typename TDto>
	class FactionApiActionBase : public ApiActionBase<T, TDto>
	{
	protected:
		std::int64_t m_factionId;

	public:
		FactionApiActionBase(TornApiService* apiService,
		                     std::shared_ptr<TornApiClient> client,
		                     std::shared_ptr<oatpp::data::mapping::ObjectMapper> om,
		                     std::string key,
		                     std::int64_t factionId)
			: ApiActionBase<T, TDto>(apiService, std::move(client), std::move(om), std::move(key))
			  , m_factionId(factionId)
		{
		}
	};

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
	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionResponseDto>&> getFactionBasic(
		const std::string& key);
	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornUserBasicResponseDto>&> getUserBasic(
		const std::string& key);
	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionMembersResponse>&> getFactionMembers(
		const std::string& key, std::int64_t factionId);
	oatpp::async::CoroutineStarterForResult<const std::vector<FactionMemberInfo>&> getFactionMembersCompact(
		const std::string& key, std::int64_t factionId);
	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionWarResponseDto>&> getFactionWar(
		const std::string& key);
	oatpp::async::CoroutineStarterForResult<const std::optional<std::int64_t>&> getEnemyWarFaction(
		const std::string& key, std::int64_t factionId);
};
