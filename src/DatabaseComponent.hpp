#pragma once

#include "db/ApiKeyDb.hpp"
#include "db/FactionDb.hpp"
#include "db/PurchaseDb.hpp"
#include "db/UserDb.hpp"
#include "db/WarDb.hpp"
#include "db/WarMemberInformationDb.hpp"

#include "oatpp/core/macro/component.hpp"
#include "oatpp-postgresql/orm.hpp"

class DatabaseComponent {
public:

    // Provider (backend-specific) toggles between sqlite and postgre depending on environment
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionProvider>, connectionProvider)([] {
        return std::make_shared<oatpp::postgresql::ConnectionProvider>("postgresql://torn:tornpass@192.168.0.117:5432/torn_rw_feed");
	}());


    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionPool>, connectionPool)
        ([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionProvider>, connectionProvider);
        return oatpp::postgresql::ConnectionPool::createShared(
            connectionProvider,
            10,
            std::chrono::seconds(5)
        );
            }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::postgresql::Executor>, dbExecutor)
        ([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionPool>, connectionPool);
        return std::make_shared<oatpp::postgresql::Executor>(connectionPool);
            }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<FactionDb>, factionDb)
        ([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::Executor>, dbExecutor);
        return std::make_shared<FactionDb>(dbExecutor);
            }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<UserDb>, userDb)
        ([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::Executor>, dbExecutor);
        return std::make_shared<UserDb>(dbExecutor);
            }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<ApiKeyDb>, apiKeyDb)
        ([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::Executor>, dbExecutor);
        return std::make_shared<ApiKeyDb>(dbExecutor);
            }());


    OATPP_CREATE_COMPONENT(std::shared_ptr<PurchaseDb>, purchaseDb)
        ([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::Executor>, dbExecutor);
        return std::make_shared<PurchaseDb>(dbExecutor);
            }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<WarDb>, warDb)
        ([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::Executor>, dbExecutor);
        return std::make_shared<WarDb>(dbExecutor);
            }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<WarMemberInformationDb>, warMemberInformationDb)
        ([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::Executor>, dbExecutor);
        return std::make_shared<WarMemberInformationDb>(dbExecutor);
            }());
};
