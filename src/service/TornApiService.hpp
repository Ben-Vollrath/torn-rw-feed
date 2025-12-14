#pragma once

#include <optional>

#include "clients/TornApiClient.hpp"
#include "dto/clients/TornErrorResponseDto.hpp"
#include "dto/clients/TornFactionMembersResponseDto.hpp"
#include "dto/clients/TornFactionResponseDto.hpp"
#include "dto/clients/TornFactionWarResponseDto.hpp"
#include "dto/clients/TornUserBasicResponseDto.hpp"
#include "dto/clients/TornFactionWarAndMembersResponseDto.hpp"
#include "dto/clients/TornKeyResponseDto.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/client/ApiClient.hpp"


class TornApiService
{
	OATPP_COMPONENT(std::shared_ptr<TornApiClient>, tornApiClient);
	OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

	using Status = oatpp::web::protocol::http::Status;
	std::string COMMENT = "Torn-RW-Feed";

	template <typename T, typename TDto>
	class ApiActionBase : public oatpp::async::CoroutineWithResult<T, TDto>
	{
	protected:
		TornApiService* m_apiService;
		std::shared_ptr<TornApiClient> m_client;
		std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_om;
		std::string m_key;
		std::string m_lastResponse;
		v_int32 m_lastStatus = 0;

	public:
		ApiActionBase(TornApiService* apiService, const std::shared_ptr<TornApiClient>& client,
		              const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& om, std::string key)
			: m_apiService(apiService), m_client(client), m_om(om), m_key(std::move(key))
		{
		}

	protected:
		void setLastResponse(const oatpp::String& body, v_int32 status = 0) {
			m_lastStatus = status;
			if (body) {
				m_lastResponse.assign(body->data(), body->size());
			}
			else {
				m_lastResponse.clear();
			}
		}

		oatpp::async::Action handleError(oatpp::async::Error* e) override {
			const char* cls = typeid(T).name();
			const char* msg = (e && e->what()) ? e->what() : "unknown error";

			if (!m_lastResponse.empty()) {
				OATPP_LOGE("ApiService",
					"Coroutine '%s' failed: %s (http=%d). Last response: %s",
					cls, msg, m_lastStatus, m_lastResponse.c_str());
			}
			else {
				OATPP_LOGE("ApiService",
					"Coroutine '%s' failed: %s (http=%d). No response captured.",
					cls, msg, m_lastStatus);
			}

			// propagate error
			return oatpp::async::Action(e);
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
		                     const std::shared_ptr<TornApiClient>& client,
		                     const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& om,
		                     std::string key,
		                     std::int64_t factionId)
			: ApiActionBase<T, TDto>(apiService, client, om, std::move(key))
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
	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornKeyResponseDto>&> getKeyInfo(
		const std::string& key);
	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionMembersResponse>&> getFactionMembers(
		const std::string& key, std::int64_t factionId);
	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionWarResponseDto>&> getFactionWar(
		const std::string& key);
	oatpp::async::CoroutineStarterForResult<const std::optional<std::int64_t>&> getEnemyWarFaction(
		const std::string& key, std::int64_t factionId);
	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionWarAndMembersResponseDto>&> getFactionWarAndMembers(
		const std::string& key);
};
