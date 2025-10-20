#pragma once

#include "dto/UserDto.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class UserDb : public oatpp::orm::DbClient {
public:
    UserDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
        : oatpp::orm::DbClient(executor) {
        oatpp::orm::SchemaMigration m(executor, "users");
        m.addFile(1, SQL_FILE_PATH"users/init.sql");
        m.migrate();
    }

    QUERY(create,
        "INSERT INTO users (torn_key, faction_id) "
        "VALUES (:user.torn_key, :user.faction_id) "
        "RETURNING *;",
        PARAM(oatpp::Object<UserDto>, user))

    QUERY(upsertById,
        "INSERT INTO users (id, torn_key, faction_id) "
        "VALUES (:user.id, :user.torn_key, :user.faction_id) "
		"ON CONFLICT (id) DO UPDATE "
		" SET torn_key = COALESCE(EXCLUDED.torn_key, users.torn_key), "
		"     faction_id = COALESCE(EXCLUDED.faction_id, users.faction_id)"
        "RETURNING *;",
        PARAM(oatpp::Object<UserDto>, user))

    QUERY(update,
        "UPDATE users SET torn_key=:user.torn_key, faction_id=:user.faction_id "
        "WHERE id=:user.id RETURNING *;",
        PARAM(oatpp::Object<UserDto>, user))

    QUERY(getById,
        "SELECT * FROM users WHERE id=:id;",
        PARAM(oatpp::Int64, id))

    QUERY(deleteById,
        "DELETE FROM users WHERE id=:id;",
        PARAM(oatpp::Int64, id))
};

#include OATPP_CODEGEN_END(DbClient)
