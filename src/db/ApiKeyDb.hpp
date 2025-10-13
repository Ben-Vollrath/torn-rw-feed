#pragma once

#include "dto/ApiKeyDto.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class ApiKeyDb : public oatpp::orm::DbClient {
public:
    ApiKeyDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
        : oatpp::orm::DbClient(executor) {
        oatpp::orm::SchemaMigration m(executor);
        m.addFile(1, "api_keys/init.sql");
        m.migrate();
    }

    QUERY(createKey,
        "INSERT INTO api_keys "
        "(user_id, prefix, secret_hash, alg, created_at, expires_at, revoked, last_used_at) "
        "VALUES (:key.userId, :key.prefix, :key.secretHash::bytea, "
        "COALESCE(:key.alg, 'SHA256'), "
        "COALESCE(:key.createdAt, (EXTRACT(EPOCH FROM now()))::BIGINT), "
        ":key.expiresAt, COALESCE(:key.revoked, FALSE), :key.lastUsedAt) "
        "RETURNING *;",
        PREPARE(true),
        PARAM(oatpp::Object<ApiKeyDto>, key))

    QUERY(updateKey,
        "UPDATE api_keys SET "
        "expires_at=:key.expiresAt, revoked=:key.revoked, last_used_at=:key.lastUsedAt "
        "WHERE id=:key.id RETURNING *;",
        PREPARE(true),
        PARAM(oatpp::Object<ApiKeyDto>, key))

    QUERY(getKeyById,
        "SELECT * FROM api_keys WHERE id=:id;",
        PREPARE(true),
        PARAM(oatpp::Int64, id))

    QUERY(getKeyByPrefix,
        "SELECT * FROM api_keys WHERE prefix=:prefix;",
        PREPARE(true),
        PARAM(oatpp::String, prefix))

    QUERY(listKeysForUser,
        "SELECT * FROM api_keys WHERE user_id=:userId ORDER BY id DESC;",
        PREPARE(true),
        PARAM(oatpp::Int64, userId))

    QUERY(deleteKeyById,
        "DELETE FROM api_keys WHERE id=:id;",
        PREPARE(true),
        PARAM(oatpp::Int64, id))
};

#include OATPP_CODEGEN_END(DbClient)
