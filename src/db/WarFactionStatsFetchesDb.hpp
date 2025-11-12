#pragma once

#include "dto/WarFactionStatsFetchesDto.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class WarFactionStatsFetchesDb : public oatpp::orm::DbClient {
public:
	WarFactionStatsFetchesDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
		: DbClient(executor) {
		oatpp::orm::SchemaMigration m(executor, "war_faction_stats_fetches");
		m.addFile(1, SQL_FILE_PATH"war_faction_stats_fetches/init.sql");
		m.migrate();
	}

	QUERY(create,
		"INSERT INTO war_faction_stats_fetches (war_id, faction_id) "
		"VALUES (:war_id, :faction_id) "
		"RETURNING *;",
		PARAM(oatpp::Int64, war_id), PARAM(oatpp::Int64, faction_id))

	QUERY(getByFactionAndWarId,
		"SELECT * FROM war_faction_stats_fetches "
		"WHERE faction_id = :faction_id "
		"AND war_id = :war_id",
		PARAM(oatpp::Int64, faction_id), PARAM(oatpp::Int64, war_id))


	QUERY(deleteAll,
		"DELETE FROM war_faction_stats_fetches;")
};

#include OATPP_CODEGEN_END(DbClient)