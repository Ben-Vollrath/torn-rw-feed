#pragma once

#include "dto/TargetsDto.hpp"
#include "AppConfig.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class TargetsDb : public oatpp::orm::DbClient
{
  OATPP_COMPONENT(std::shared_ptr<AppConfig>, appConfig);

public:
	TargetsDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
	: DbClient(executor)
	{
		oatpp::orm::SchemaMigration m(executor, "targets");
		m.addFile(1, appConfig->sqlFilePath + "targets/init.sql");
		m.migrate();
	}

	QUERY(create,
		"INSERT INTO targets (enemy_faction_id, user_id, targets) "
		"SELECT :t.enemy_faction_id, :t.user_id, :t.targets "
		"WHERE EXISTS (SELECT 1 FROM users WHERE id = :t.user_id) "
		"ON CONFLICT (enemy_faction_id, user_id) DO UPDATE SET "
		"  targets = EXCLUDED.targets "
		"RETURNING *;",
		PARAM(oatpp::Object<TargetsDbDto>, t)
	)


	QUERY(getAllByEnemyFactionAndUser,
	  "SELECT * FROM targets "
	  "WHERE enemy_faction_id = :enemy_faction_id "
	  "AND user_id = :user_id;",
	PARAM(oatpp::Int64, enemy_faction_id),
	PARAM(oatpp::Int64, user_id))
};

#include OATPP_CODEGEN_END(DbClient)
