#pragma once

#include "dto/WarDto.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class WarDb : public oatpp::orm::DbClient {
public:
    WarDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
        : oatpp::orm::DbClient(executor) {
        oatpp::orm::SchemaMigration m(executor, "wars");
        m.addFile(1, SQL_FILE_PATH"wars/init.sql");
        m.migrate();
    }

    QUERY(create,
        "INSERT INTO wars (faction_id, enemy_faction_id, start_at, end_at, winner_id, "
        "faction_score, faction_chain, enemy_faction_score, enemy_faction_chain) "
        "VALUES (:w.faction_id, :w.enemy_faction_id, :w.start_at, :w.end_at, :w.winner_id, "
        "COALESCE(:w.faction_score,0), COALESCE(:w.faction_chain,0), "
        "COALESCE(:w.enemy_faction_score,0), COALESCE(:w.enemy_faction_chain,0)) "
        "RETURNING *;",
        PARAM(oatpp::Object<WarDto>, w))

    QUERY(update,
        "UPDATE wars SET faction_id=:w.faction_id, enemy_faction_id=:w.enemy_faction_id, start_at=:w.start_at, "
        "end_at=:w.end_at, winner_id=:w.winner_id, faction_score=:w.faction_score, faction_chain=:w.faction_chain, "
        "enemy_faction_score=:w.enemy_faction_score, enemy_faction_chain=:w.enemy_faction_chain "
        "WHERE id=:w.id RETURNING *;",
        PARAM(oatpp::Object<WarDto>, w))

    QUERY(getById,
        "SELECT * FROM wars WHERE id=:id;",
        PARAM(oatpp::Int64, id))

    QUERY(listWarsForFaction,
        "SELECT * FROM wars "
        "WHERE faction_id=:faction_id OR enemy_faction_id=:faction_id "
        "ORDER BY start_at DESC LIMIT :limit OFFSET :offset;",
        PARAM(oatpp::Int64, faction_id),
        PARAM(oatpp::UInt32, limit),
        PARAM(oatpp::UInt32, offset))

    QUERY(deleteById,
        "DELETE FROM wars WHERE id=:id;",
        PARAM(oatpp::Int64, id))
};

#include OATPP_CODEGEN_END(DbClient)