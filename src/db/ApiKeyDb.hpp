#pragma once

#include "AppConfig.hpp"
#include "dto/ApiKeyDto.hpp"
#include "oatpp-postgresql/orm.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class ApiKeyDb : public oatpp::orm::DbClient {
  OATPP_COMPONENT(std::shared_ptr<AppConfig>, appConfig);

 public:
  ApiKeyDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
      : DbClient(executor) {
    oatpp::orm::SchemaMigration m(executor, "api_keys");
    m.addFile(1, appConfig->sqlFilePath + "api_keys/init.sql");
    m.migrate();
  }

  QUERY(create,
        "INSERT INTO api_keys "
        "(user_id, prefix, secret_hash, alg, created_at, expires_at, revoked, "
        "last_used_at) "
        "VALUES (:key.user_id, :key.prefix, CAST(:key.secret_hash AS bytea),"
        "COALESCE(:key.alg, 'SHA256'), "
        ":key.created_at, :key.expires_at, COALESCE(:key.revoked, FALSE), "
        ":key.last_used_at) "
        "RETURNING id, user_id, prefix, alg, created_at, expires_at, revoked, "
        "last_used_at;",
        PARAM(oatpp::Object<ApiKeyDto>, key))

  QUERY(update,
        "UPDATE api_keys SET "
        "expires_at=:key.expires_at, revoked=:key.revoked, "
        "last_used_at=:key.last_used_at "
        "WHERE id=:key.id "
        "RETURNING id, user_id, prefix, encode(secret_hash, 'hex') AS "
        "secret_hash, alg, created_at, expires_at, revoked, last_used_at;",
        PARAM(oatpp::Object<ApiKeyDto>, key))

  QUERY(getById,
        "SELECT id, user_id, prefix, encode(secret_hash, 'hex') AS "
        "secret_hash, alg, created_at, expires_at, revoked, last_used_at "
        "FROM api_keys WHERE id=:id;",
        PARAM(oatpp::Int64, id))

  QUERY(getKeyByPrefix,
        "SELECT id, user_id, prefix, encode(secret_hash, 'hex') AS "
        "secret_hash, alg, created_at, expires_at, revoked, last_used_at "
        "FROM api_keys WHERE prefix=:prefix;",
        PARAM(oatpp::String, prefix))

  QUERY(listKeysForUser,
        "SELECT id, user_id, prefix, encode(secret_hash, 'hex') AS "
        "secret_hash, alg, created_at, expires_at, revoked, last_used_at "
        "FROM api_keys WHERE user_id=:userId ORDER BY id DESC;",
        PARAM(oatpp::Int64, userId))

  QUERY(deleteById, "DELETE FROM api_keys WHERE id=:id;",
        PARAM(oatpp::Int64, id))

  QUERY(touchLastUsedAt,
        "UPDATE api_keys "
        "SET last_used_at = CAST(EXTRACT(EPOCH FROM now()) AS BIGINT), "
        "expires_at = CAST(EXTRACT(EPOCH FROM (now() + INTERVAL '30 days')) AS "
        "BIGINT) "
        "WHERE id = :id "
        "RETURNING id, user_id, prefix, encode(secret_hash, 'hex') AS "
        "secret_hash, alg, created_at, expires_at, revoked, last_used_at;",
        PARAM(oatpp::Int64, id))

  QUERY(deleteAll, "DELETE FROM api_keys;")
};

#include OATPP_CODEGEN_END(DbClient)
