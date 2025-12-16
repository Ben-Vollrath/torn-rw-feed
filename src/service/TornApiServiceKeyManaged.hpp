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

	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionResponseDto>&>
		getFactionBasic()
	{
		using Result = oatpp::Object<TornFactionResponseDto>;
		return callWithKeyManagementAsync<Result>(&TornApiService::getFactionBasic);
	}

	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionMembersResponse>&>
		getFactionMembers(const std::int64_t factionId)
	{
		using Result = oatpp::Object<TornFactionMembersResponse>;
		return callWithKeyManagementAsync<Result>(
			&TornApiService::getFactionMembers,
			factionId
		);
	}

	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionWarResponseDto>&>
		getFactionWar()
	{
		using Result = oatpp::Object<TornFactionWarResponseDto>;
		return callWithKeyManagementAsync<Result>(&TornApiService::getFactionWar);
	}

	oatpp::async::CoroutineStarterForResult<const oatpp::Object<TornFactionWarAndMembersResponseDto>&>
		getFactionWarAndMembers()
	{
		using Result = oatpp::Object<TornFactionWarAndMembersResponseDto>;
		return callWithKeyManagementAsync<Result>(&TornApiService::getFactionWarAndMembers);
	}

	oatpp::async::CoroutineStarterForResult<const std::optional<std::int64_t>&>
		getEnemyWarFaction(const std::int64_t factionId)
	{
		using Result = std::optional<std::int64_t>;
		return callWithKeyManagementAsync<Result>(
			&TornApiService::getEnemyWarFaction,
			factionId
		);
	}

	oatpp::Object<TornKeyRow> getLastKey();
	void removeLastKey();
	void removeLastKeyWithDb();

private:
	oatpp::Vector<oatpp::Object<TornKeyRow>> m_keys;
	std::size_t index = 0;
	UserService m_UserService;
	TornApiService m_tornApiService;


	std::string getKey();

	template<typename Result, typename MemFn, typename... Args>
	oatpp::async::CoroutineStarterForResult<const Result&>
		callWithKeyManagementAsync(MemFn mf, Args&&... args)
	{
		using Action = oatpp::async::Action;

		// Local coroutine type that closes over template args
		class Coroutine
			: public oatpp::async::CoroutineWithResult<Coroutine, const Result&>
		{
			TornApiServiceKeyManaged* m_self;
			MemFn m_mf;
			std::tuple<std::decay_t<Args>...> m_args;

		public:
			Coroutine(TornApiServiceKeyManaged* self, MemFn mf, Args&&... args)
				: m_self(self)
				, m_mf(mf)
				, m_args(std::forward<Args>(args)...)
			{
			}

			Action act() override
			{
				auto key = m_self->getKey();

				// Call underlying TornApiService method with key + stored args
				return std::apply(
					[&](auto&&... unpackedArgs)
					{
						return (m_self->m_tornApiService.*m_mf)(
							key,
							std::forward<decltype(unpackedArgs)>(unpackedArgs)...
							).template callbackTo<Coroutine>(&Coroutine::returnResponse);
					},
					m_args
				);
			}

			Action returnResponse(const Result& r)
			{
				return this->_return(r);
			}

			Action handleError(oatpp::async::Error* e) override
			{
				OATPP_LOGE("KeyManaged", "Fetcher error: %s", e ? e->what() : "unknown");

				const int code = std::atoi(e ? e->what() : "0");
				switch (code)
				{
				case 401:
				case 406:
				case 403:
					// Key invalid -> drop and retry with next key
					m_self->removeLastKeyWithDb();
					return this->yieldTo(&Coroutine::act);
				default:
					return Action(e);
				}
			}
		};

		return Coroutine::startForResult(this, mf, std::forward<Args>(args)...);
	}
};
