#ifndef DatabaseComponent_hpp
#define DatabaseComponent_hpp

#include "db/DbClient.hpp"
#include "oatpp-postgresql/orm.hpp"

class DatabaseComponent {
public:

    /**
     * Create DbClient component.
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<DbClient>, m_database)([] {
        /* Create database-specific ConnectionProvider */
        auto connectionProvider = std::make_shared<oatpp::postgresql::ConnectionProvider>("postgresql://torn:tornpass@192.168.0.117:5432/torn_rw_feed");

        /* Create database-specific ConnectionPool */
        auto connectionPool = oatpp::postgresql::ConnectionPool::createShared(connectionProvider,
            10 /* max-connections */,
            std::chrono::seconds(5) /* connection TTL */);

        /* Create database-specific Executor */
        auto executor = std::make_shared<oatpp::postgresql::Executor>(connectionPool);

        /* Create DbClient database client */
        return std::make_shared<DbClient>(executor);
        }());

};
#endif /* DatabaseComponent_hpp */