#include "TornStatsApiService.hpp"


oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornStatsSpyResponseDto>&> TornStatsApiService::getSpies(
	std::string key,
	std::string factionId)
{
	class GetSpiesCoroutine : public oatpp::async::CoroutineWithResult<
			GetSpiesCoroutine, const oatpp::Object<TornStatsSpyResponseDto>&>
	{
		const std::shared_ptr<TornStatsApiClient> m_client;
		const std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_om;
		const std::string m_key;
		const std::string m_factionId;

	public:
		GetSpiesCoroutine(
			const std::shared_ptr<TornStatsApiClient>& client,
			const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& om,
			std::string key,
			std::string factionId) :
			m_client(client), m_om(om), m_key(std::move(key)), m_factionId(std::move(factionId))
		{
		}

	private:
		oatpp::async::Action act() override
		{
			return m_client->getSpies(m_key, m_factionId).callbackTo(&GetSpiesCoroutine::parseResponse);
		}

		oatpp::async::Action parseResponse(const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>& rsp)
		{
			return _return(rsp->readBodyToDto<oatpp::Object<TornStatsSpyResponseDto>>(m_om));
		}
	};

	return GetSpiesCoroutine::startForResult(tornStatsApiClient, objectMapper, std::move(key), std::move(factionId));
}
