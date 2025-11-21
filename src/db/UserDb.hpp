#pragma once

#include "dto/UserDto.hpp"
#include "AppConfig.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class UserDb : public oatpp::orm::DbClient
{
	OATPP_COMPONENT(std::shared_ptr<AppConfig>, appConfig);

public:
	UserDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
		: DbClient(executor)
	{
		oatpp::orm::SchemaMigration m(executor, "users");
		m.addFile(1, appConfig->sqlFilePath + "users/init.sql");
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

	QUERY(getTornKeys,
	      "SELECT torn_key as torn_key "
	      "FROM users "
	      "WHERE faction_id = :faction_id "
	      "AND torn_key IS NOT NULL;",
	      PARAM(oatpp::Int64, faction_id))

	QUERY(removeTornKey,
	      "UPDATE users "
	      "SET torn_key = NULL "
	      "WHERE torn_key = :torn_key;",
	      PARAM(oatpp::String, torn_key))

	QUERY(deleteAll,
	      "DELETE FROM users;")
};

#include OATPP_CODEGEN_END(DbClient)
