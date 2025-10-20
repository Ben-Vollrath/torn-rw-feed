#pragma once

#include "dto/PurchaseDto.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class PurchaseDb : public oatpp::orm::DbClient {
public:
    PurchaseDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
        : oatpp::orm::DbClient(executor) {
        oatpp::orm::SchemaMigration m(executor, "purchases");
        m.addFile(1, SQL_FILE_PATH"purchases/init.sql");
        m.migrate();
    }

    QUERY(create,
        "INSERT INTO purchases (faction_id, purchaser_id, amount, full_log, end_date) "
        "VALUES (:p.faction_id, :p.purchaser_id, :p.amount, :p.full_log, :p.end_date) "
        "RETURNING *;",
        PARAM(oatpp::Object<PurchaseDto>, p))

    QUERY(update,
        "UPDATE purchases SET faction_id=:p.faction_id, purchaser_id=:p.purchaser_id, amount=:p.amount, "
        "full_log=:p.fullLog, start_date=:p.startDate, end_date=:p.end_date "
        "WHERE id=:p.id RETURNING *;",
        PARAM(oatpp::Object<PurchaseDto>, p))

    QUERY(getById,
        "SELECT * FROM purchases WHERE id=:id;",
        PARAM(oatpp::Int64, id))

    QUERY(listPurchasesForFaction,
        "SELECT * FROM purchases WHERE faction_id=:faction_id "
        "ORDER BY id DESC LIMIT :limit OFFSET :offset;",
        PARAM(oatpp::Int64, faction_id),
        PARAM(oatpp::UInt32, limit),
        PARAM(oatpp::UInt32, offset))

    QUERY(deleteById,
        "DELETE FROM purchases WHERE id=:id;",
        PARAM(oatpp::Int64, id))
};

#include OATPP_CODEGEN_END(DbClient)
