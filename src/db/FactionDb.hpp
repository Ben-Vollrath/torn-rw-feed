#pragma once
#pragma once

#include "AppConfig.hpp"
#include "dto/FactionDto.hpp"
#include "oatpp-postgresql/orm.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class FactionDb : public oatpp::orm::DbClient {
  OATPP_COMPONENT(std::shared_ptr<AppConfig>, appConfig);

 public:
  FactionDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
      : DbClient(executor) {
    oatpp::orm::SchemaMigration m(executor, "factions");
    m.addFile(1, appConfig->sqlFilePath + "factions/init.sql");
    m.addFile(2, appConfig->sqlFilePath + "factions/name_not_unique.sql");
    m.migrate();
  }

  QUERY(
      create,
      "INSERT INTO factions (name, leader_torn_id, co_leader_torn_id) "
      "VALUES (:f.name, :f.leader_torn_id, :f.co_leader_torn_id) RETURNING *;",
      PARAM(oatpp::Object<FactionDto>, f))

  QUERY(upsertById,
        "INSERT INTO factions (id, name, leader_torn_id, co_leader_torn_id) "
        "VALUES (:f.id, :f.name, :f.leader_torn_id, :f.co_leader_torn_id) "
        "ON CONFLICT (id) DO UPDATE SET "
        "  name = COALESCE(EXCLUDED.name, factions.name), "
        "  leader_torn_id = COALESCE(EXCLUDED.leader_torn_id, "
        "factions.leader_torn_id), "
        "  co_leader_torn_id = COALESCE(EXCLUDED.co_leader_torn_id, "
        "factions.co_leader_torn_id) "
        "RETURNING *;",
        PARAM(oatpp::Object<FactionDto>, f))

  QUERY(update,
        "UPDATE factions SET name=:f.name, leader_torn_id=:f.leader_torn_id, "
        "co_leader_torn_id=:f.co_leader_torn_id WHERE id=:f.id RETURNING *;",
        PARAM(oatpp::Object<FactionDto>, f))

  QUERY(getById, "SELECT * FROM factions WHERE id=:id;",
        PARAM(oatpp::Int64, id))

  QUERY(listFactions,
        "SELECT * FROM factions ORDER BY id DESC LIMIT :limit OFFSET :offset;",
        PARAM(oatpp::UInt32, limit), PARAM(oatpp::UInt32, offset))

  QUERY(deleteById, "DELETE FROM factions WHERE id=:id;",
        PARAM(oatpp::Int64, id))

  QUERY(deleteAll, "DELETE FROM factions;")
};

#include OATPP_CODEGEN_END(DbClient)
