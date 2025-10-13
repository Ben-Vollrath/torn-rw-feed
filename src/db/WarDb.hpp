#pragma once

#include "dto/WarDto.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class WarDb : public oatpp::orm::DbClient {
public:
    WarDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
        : oatpp::orm::DbClient(executor) {
        oatpp::orm::SchemaMigration m(executor);
        m.addFile(1, "wars/init.sql");
        m.migrate();
    }

    QUERY(createWar,
        "INSERT INTO wars (faction_id, enemy_faction_id, start_at, end_at, winner_id, "
        "faction_score, faction_chain, enemy_faction_score, enemy_faction_chain) "
        "VALUES (:w.factionId, :w.enemyFactionId, "
        "COALESCE(:w.startAt, (EXTRACT(EPOCH FROM now()))::BIGINT), :w.endAt, :w.winnerId, "
        "COALESCE(:w.factionScore,0), COALESCE(:w.factionChain,0), "
        "COALESCE(:w.enemyFactionScore,0), COALESCE(:w.enemyFactionChain,0)) "
        "RETURNING *;",
        PREPARE(true),
        PARAM(oatpp::Object<WarDto>, w))

    QUERY(updateWar,
        "UPDATE wars SET faction_id=:w.factionId, enemy_faction_id=:w.enemyFactionId, start_at=:w.startAt, "
        "end_at=:w.endAt, winner_id=:w.winnerId, faction_score=:w.factionScore, faction_chain=:w.factionChain, "
        "enemy_faction_score=:w.enemyFactionScore, enemy_faction_chain=:w.enemyFactionChain "
        "WHERE id=:w.id RETURNING *;",
        PREPARE(true),
        PARAM(oatpp::Object<WarDto>, w))

    QUERY(getWarById,
        "SELECT * FROM wars WHERE id=:id;",
        PREPARE(true),
        PARAM(oatpp::Int64, id))

    QUERY(listWarsForFaction,
        "SELECT * FROM wars "
        "WHERE faction_id=:factionId OR enemy_faction_id=:factionId "
        "ORDER BY start_at DESC LIMIT :limit OFFSET :offset;",
        PREPARE(true),
        PARAM(oatpp::Int64, factionId),
        PARAM(oatpp::UInt32, limit),
        PARAM(oatpp::UInt32, offset))

    QUERY(deleteWarById,
        "DELETE FROM wars WHERE id=:id;",
        PREPARE(true),
        PARAM(oatpp::Int64, id))
};

#include OATPP_CODEGEN_END(DbClient)