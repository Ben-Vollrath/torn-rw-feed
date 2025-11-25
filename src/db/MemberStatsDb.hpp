#pragma once

#include "dto/MemberStatsDto.hpp"
#include "AppConfig.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class MemberStatsDb : public oatpp::orm::DbClient
{
	OATPP_COMPONENT(std::shared_ptr<AppConfig>, appConfig);

public:
	MemberStatsDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
		: DbClient(executor)
	{
		oatpp::orm::SchemaMigration m(executor, "member_stats");
		m.addFile(1, appConfig->sqlFilePath + "member_stats/init.sql");
		m.migrate();
	}

	QUERY(create,
	      "INSERT INTO member_stats (war_id, faction_id, member_id, type, str, def, spd, dex, total) "
	      "VALUES (:memberStats.war_id, :memberStats.faction_id, :memberStats.member_id, :memberStats.type, :memberStats.str, :memberStats.def, :memberStats.spd, :memberStats.dex, :memberStats.total) "
	      "RETURNING *;",
	      PARAM(oatpp::Object<MemberStatsDto>, memberStats))

	QUERY(upsert,
	      "INSERT INTO member_stats (war_id, faction_id, member_id, type, str, def, spd, dex, total) "
	      "VALUES (:memberStats.war_id, :memberStats.faction_id, :memberStats.member_id, :memberStats.type, :memberStats.str, :memberStats.def, :memberStats.spd, :memberStats.dex, :memberStats.total) "
	      "ON CONFLICT (war_id, faction_id, member_id, type) DO UPDATE SET "
	      "  str = EXCLUDED.str, "
	      "  def = EXCLUDED.def, "
	      "  spd = EXCLUDED.spd, "
	      "  dex = EXCLUDED.dex, "
	      "  total = EXCLUDED.total "
	      "RETURNING *;",
	      PARAM(oatpp::Object<MemberStatsDto>, memberStats))


	QUERY(findByWarAndFaction,
	      "SELECT DISTINCT ON(member_id) * "
	      "FROM member_stats "
	      "WHERE war_id = :war_id "
	      "AND faction_id = :faction_id "
	      "ORDER BY member_id, "
	      "CASE WHEN type = 'tornstatsspies' THEN 0 ELSE 1 END; ",
	      PARAM(oatpp::Int64, war_id), PARAM(oatpp::Int64, faction_id))

	QUERY(deleteAll,
	      "DELETE FROM member_stats;")
};

#include OATPP_CODEGEN_END(DbClient)
