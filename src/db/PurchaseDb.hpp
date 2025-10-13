#pragma once

#include "dto/PurchaseDto.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class PurchaseDb : public oatpp::orm::DbClient {
public:
    PurchaseDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
        : oatpp::orm::DbClient(executor) {
        oatpp::orm::SchemaMigration m(executor);
        m.addFile(1, "purchases/init.sql");
        m.migrate();
    }

    QUERY(createPurchase,
        "INSERT INTO purchases (faction_id, purchaser_id, amount, full_log, start_date, end_date) "
        "VALUES (:p.factionId, :p.purchaserId, :p.amount, :p.fullLog, "
        "COALESCE(:p.startDate, (EXTRACT(EPOCH FROM now()))::BIGINT), :p.endDate) "
        "RETURNING *;",
        PREPARE(true),
        PARAM(oatpp::Object<PurchaseDto>, p))

    QUERY(updatePurchase,
        "UPDATE purchases SET faction_id=:p.factionId, purchaser_id=:p.purchaserId, amount=:p.amount, "
        "full_log=:p.fullLog, start_date=:p.startDate, end_date=:p.endDate "
        "WHERE id=:p.id RETURNING *;",
        PREPARE(true),
        PARAM(oatpp::Object<PurchaseDto>, p))

    QUERY(getPurchaseById,
        "SELECT * FROM purchases WHERE id=:id;",
        PREPARE(true),
        PARAM(oatpp::Int64, id))

    QUERY(listPurchasesForFaction,
        "SELECT * FROM purchases WHERE faction_id=:factionId "
        "ORDER BY id DESC LIMIT :limit OFFSET :offset;",
        PREPARE(true),
        PARAM(oatpp::Int64, factionId),
        PARAM(oatpp::UInt32, limit),
        PARAM(oatpp::UInt32, offset))

    QUERY(deletePurchaseById,
        "DELETE FROM purchases WHERE id=:id;",
        PREPARE(true),
        PARAM(oatpp::Int64, id))
};

#include OATPP_CODEGEN_END(DbClient)
