#pragma once

#include "dto/WarDto.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class WarDb : public oatpp::orm::DbClient
{
public:
	WarDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
		: DbClient(executor)
	{
		oatpp::orm::SchemaMigration m(executor, "wars");
		m.addFile(1, SQL_FILE_PATH"wars/init.sql");
		m.migrate();
	}

	QUERY(create,
	      "INSERT INTO wars (faction_one_id, faction_two_id, start_at, end_at, winner_id, "
	      "                  faction_one_score, faction_one_chain, faction_two_score, faction_two_chain) "
	      "VALUES (:w.faction_one_id, :w.faction_two_id, :w.start_at, :w.end_at, :w.winner_id, "
	      "        COALESCE(:w.faction_one_score,0), COALESCE(:w.faction_one_chain,0), "
	      "        COALESCE(:w.faction_two_score,0), COALESCE(:w.faction_two_chain,0)) "
	      "RETURNING *;",
	      PARAM(oatpp::Object<WarDto>, w)
	)

	QUERY(update,
	      "UPDATE wars SET "
	      "  faction_one_id      = :w.faction_one_id, "
	      "  faction_two_id      = :w.faction_two_id, "
	      "  start_at            = :w.start_at, "
	      "  end_at              = :w.end_at, "
	      "  winner_id           = :w.winner_id, "
	      "  faction_one_score   = :w.faction_one_score, "
	      "  faction_one_chain   = :w.faction_one_chain, "
	      "  faction_two_score   = :w.faction_two_score, "
	      "  faction_two_chain   = :w.faction_two_chain "
	      "WHERE id = :w.id "
	      "RETURNING *;",
	      PARAM(oatpp::Object<WarDto>, w)
	)

	QUERY(getById,
	      "SELECT * FROM wars WHERE id = :id;",
	      PARAM(oatpp::Int64, id)
	)

	QUERY(getLatestWarForFaction,
	      "SELECT * FROM wars "
	      "WHERE faction_one_id = :faction_id OR faction_two_id = :faction_id "
	      "ORDER BY start_at DESC, id DESC "
	      "LIMIT 1;",
	      PARAM(oatpp::Int64, faction_id)
	)

	QUERY(deleteById,
	      "DELETE FROM wars WHERE id = :id;",
	      PARAM(oatpp::Int64, id)
	)

	QUERY(upsertById,
	      "INSERT INTO wars (id, faction_one_id, faction_two_id, start_at, end_at, winner_id, "
	      "                  faction_one_score, faction_one_chain, faction_two_score, faction_two_chain) "
	      "VALUES (:w.id, :w.faction_one_id, :w.faction_two_id, :w.start_at, :w.end_at, :w.winner_id, "
	      "        COALESCE(:w.faction_one_score,0), COALESCE(:w.faction_one_chain,0), "
	      "        COALESCE(:w.faction_two_score,0), COALESCE(:w.faction_two_chain,0)) "
	      "ON CONFLICT (id) DO UPDATE SET "
	      "  end_at              = EXCLUDED.end_at, "
	      "  winner_id           = EXCLUDED.winner_id, "
	      "  faction_one_score   = COALESCE(EXCLUDED.faction_one_score, wars.faction_one_score), "
	      "  faction_one_chain   = COALESCE(EXCLUDED.faction_one_chain, wars.faction_one_chain), "
	      "  faction_two_score   = COALESCE(EXCLUDED.faction_two_score, wars.faction_two_score), "
	      "  faction_two_chain   = COALESCE(EXCLUDED.faction_two_chain, wars.faction_two_chain) "
	      "RETURNING *;",
	      PARAM(oatpp::Object<WarDto>, w)
	)
};

#include OATPP_CODEGEN_END(DbClient)
