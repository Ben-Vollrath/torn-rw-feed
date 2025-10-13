#pragma once

#include "dto/FactionDto.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class FactionDb : public oatpp::orm::DbClient {
public:
    FactionDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
        : oatpp::orm::DbClient(executor) {
        oatpp::orm::SchemaMigration m(executor);
        m.addFile(1, "factions/init.sql");
        m.migrate();
    }

    QUERY(createFaction,
        "INSERT INTO factions (name, leader_torn_id, co_leader_torn_id) "
        "VALUES (:f.name, :f.leaderTornId, :f.coLeaderTornId) RETURNING *;",
        PREPARE(true),
        PARAM(oatpp::Object<FactionDto>, f))

        QUERY(updateFaction,
            "UPDATE factions SET name=:f.name, leader_torn_id=:f.leaderTornId, "
            "co_leader_torn_id=:f.coLeaderTornId WHERE id=:f.id RETURNING *;",
            PREPARE(true),
            PARAM(oatpp::Object<FactionDto>, f))

        QUERY(getFactionById,
            "SELECT * FROM factions WHERE id=:id;",
            PREPARE(true),
            PARAM(oatpp::Int64, id))

        QUERY(listFactions,
            "SELECT * FROM factions ORDER BY id DESC LIMIT :limit OFFSET :offset;",
            PREPARE(true),
            PARAM(oatpp::UInt32, limit),
            PARAM(oatpp::UInt32, offset))

        QUERY(deleteFactionById,
            "DELETE FROM factions WHERE id=:id;",
            PREPARE(true),
            PARAM(oatpp::Int64, id))
};

#include OATPP_CODEGEN_END(DbClient)
