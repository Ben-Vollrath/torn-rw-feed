#pragma once

#include <string>

#include "db/ApiKeyDb.hpp"
#include "db/FactionDb.hpp"
#include "db/UserDb.hpp"

#include "oatpp/core/macro/component.hpp"
#include "oatpp-postgresql/orm.hpp"

class DatabaseComponent
{
public:

	static std::string getenv_or(const char* key, const char* defv) {
		if (const char* v = std::getenv(key); v && *v) return std::string(v);
		return std::string(defv);
	}

	// Provider (backend-specific) toggles between sqlite and postgre depending on environment
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionProvider>, connectionProvider)([] {
		const char* envUrl = std::getenv("DATABASE_URL");

		const std::string dbUrl = envUrl && *envUrl
			? envUrl
			: "postgresql://torn:tornpass@192.168.0.117:5432/torn_rw_feed"; // local fallback

		return std::make_shared<oatpp::postgresql::ConnectionProvider>(dbUrl.c_str());
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

};
