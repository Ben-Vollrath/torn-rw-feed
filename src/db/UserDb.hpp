#pragma once

#include "dto/UserDto.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class UserDb : public oatpp::orm::DbClient {
public:
    UserDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
        : oatpp::orm::DbClient(executor) {
        oatpp::orm::SchemaMigration m(executor);
        m.addFile(1, "users/init.sql");
        m.migrate();
    }

    QUERY(createUser,
        "INSERT INTO users (torn_key, faction_id, created_at) "
        "VALUES (:user.tornKey, :user.factionId, "
        "COALESCE(:user.createdAt, (EXTRACT(EPOCH FROM now()))::BIGINT)) "
        "RETURNING *;",
        PREPARE(true),
        PARAM(oatpp::Object<UserDto>, user))

    QUERY(updateUser,
        "UPDATE users SET torn_key=:user.tornKey, faction_id=:user.factionId "
        "WHERE id=:user.id RETURNING *;",
        PREPARE(true),
        PARAM(oatpp::Object<UserDto>, user))

    QUERY(getUserById,
        "SELECT * FROM users WHERE id=:id;",
        PREPARE(true),
        PARAM(oatpp::Int64, id))

    QUERY(deleteUserById,
        "DELETE FROM users WHERE id=:id;",
        PREPARE(true),
        PARAM(oatpp::Int64, id))
};

#include OATPP_CODEGEN_END(DbClient)
