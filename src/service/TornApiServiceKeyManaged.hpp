#pragma once
#include <functional>

#include "TornApiService.hpp"
#include "UserService.hpp"


class TornApiServiceKeyManaged : public TornApiService
{
public:
	TornApiServiceKeyManaged(std::int64_t factionId)
	{
		m_keys = m_UserService.getTornKeys(factionId);
	}

	std::chrono::microseconds getCooldown();


	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionResponseDto>&> getFactionBasic()
	{
		return callWithKeyManagement(&TornApiService::getFactionBasic);
	}

	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornUserBasicResponseDto>&> getUserBasic()
	{
		return callWithKeyManagement(&TornApiService::getUserBasic);
	}

	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionMembersResponse>&> getFactionMembers(
		const std::int64_t factionId)
	{
		return callWithKeyManagement(&TornApiService::getFactionMembers, factionId);
	}

	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionWarResponseDto>&> getFactionWar()
	{
		return callWithKeyManagement(&TornApiService::getFactionWar);
	}

	oatpp::async::CoroutineStarterForResult<const std::optional<std::int64_t>&> getEnemyWarFaction(
		const std::int64_t factionId)
	{
		return callWithKeyManagement(&TornApiService::getEnemyWarFaction, factionId);
	}

private:
	std::vector<std::string> m_keys;
	std::size_t index = 0;
	UserService m_UserService;


	std::string getKey();
	void removeCurrentKey();

	template <typename MemFn, typename... Args>
	std::invoke_result_t<MemFn, TornApiService*, const std::string&, Args...> callWithKeyManagement(
		MemFn mf, Args&&... args)
	{
		using Return = std::invoke_result_t<MemFn, TornApiService*, const std::string&, Args...>;

		auto key = getKey();
		try
		{
			return std::invoke(mf,
			                   static_cast<TornApiService*>(this),
			                   key,
			                   std::forward<Args>(args)...);
		}
		catch (oatpp::web::protocol::http::HttpError& e)
		{
			auto status = e.getInfo().status;
			switch (status.code)
			{
			case 429:
				// Retry with same args
				return callWithKeyManagement<MemFn, Args...>(mf, std::forward<Args>(args)...);
			case 401:
				m_UserService.removeTornKey(key);
				removeCurrentKey();
				return callWithKeyManagement<MemFn, Args...>(mf, std::forward<Args>(args)...);
			default:
				throw; // rethrow
			}
		}
	}
};
