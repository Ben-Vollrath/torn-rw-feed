#pragma once

#include <string>

#include "db/ApiKeyDb.hpp"
#include "db/FactionDb.hpp"
#include "db/UserDb.hpp"
#include "AppConfig.hpp"
#include "db/MemberStatsDb.hpp"
#include "db/WarDb.hpp"
#include "db/WarFactionStatsFetchesDb.hpp"

#include "oatpp/core/macro/component.hpp"
#include "oatpp-postgresql/orm.hpp"

class DatabaseComponent
{
public:

	// Provider (backend-specific) toggles between sqlite and postgre depending on environment
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionProvider>, connectionProvider)([]
	{
			OATPP_COMPONENT(std::shared_ptr<AppConfig>, appConfig);
		return std::make_shared<oatpp::postgresql::ConnectionProvider>(appConfig->databaseUrl);
	}());


	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionPool>, connectionPool)
	([]
	{
		OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionProvider>, connectionProvider);
		return oatpp::postgresql::ConnectionPool::createShared(
			connectionProvider,
			10,
			std::chrono::seconds(5)
		);
	}());

	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::postgresql::Executor>, dbExecutor)
	([]
	{
		OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionPool>, connectionPool);
		return std::make_shared<oatpp::postgresql::Executor>(connectionPool);
	}());

	OATPP_CREATE_COMPONENT(std::shared_ptr<FactionDb>, factionDb)
	([]
	{
		OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::Executor>, dbExecutor);
		return std::make_shared<FactionDb>(dbExecutor);
	}());

	OATPP_CREATE_COMPONENT(std::shared_ptr<UserDb>, userDb)
	([]
	{
		OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::Executor>, dbExecutor);
		return std::make_shared<UserDb>(dbExecutor);
	}());

	OATPP_CREATE_COMPONENT(std::shared_ptr<ApiKeyDb>, apiKeyDb)
	([]
	{
		OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::Executor>, dbExecutor);
		return std::make_shared<ApiKeyDb>(dbExecutor);
	}());

	OATPP_CREATE_COMPONENT(std::shared_ptr<WarDb>, warDb)
		([] {
		OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::Executor>, dbExecutor);
		return std::make_shared<WarDb>(dbExecutor);
	}());

	OATPP_CREATE_COMPONENT(std::shared_ptr<WarFactionStatsFetchesDb>, warFactionStatFetchesDb)
		([] {
		OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::Executor>, dbExecutor);
		return std::make_shared<WarFactionStatsFetchesDb>(dbExecutor);
	}());

	OATPP_CREATE_COMPONENT(std::shared_ptr<MemberStatsDb>, memberStatsDb)
		([] {
		OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::Executor>, dbExecutor);
		return std::make_shared<MemberStatsDb>(dbExecutor);
	}());
};
