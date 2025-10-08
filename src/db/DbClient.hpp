#ifndef DbClient_hpp
#define DbClient_hpp

#include "oatpp/orm/SchemaMigration.hpp"
#include "oatpp/orm/DbClient.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient) ///< Begin code-gen section

class DbClient : public oatpp::orm::DbClient {
public:

    DbClient(const std::shared_ptr<oatpp::orm::Executor>& executor)
        : oatpp::orm::DbClient(executor)
    {
    }

    QUERY(getUserById,
        "SELECT * FROM users WHERE id=:id;",
        PARAM(oatpp::Int32, id))

};

#include OATPP_CODEGEN_END(DbClient) ///< End code-gen section

#endif /* DbClient_hpp */