#pragma once

#include "dto/WarMemberInformationDto.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class WarMemberInformationDb : public oatpp::orm::DbClient {
public:
    WarMemberInformationDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
        : oatpp::orm::DbClient(executor) {
        oatpp::orm::SchemaMigration m(executor);
        m.addFile(1, "war_member_information/init.sql");
        m.migrate();
    }

    QUERY(createMember,
        "INSERT INTO war_member_information (war_id, name, level, str, def, spd, dex, total) "
        "VALUES (:mi.warId, :mi.name, :mi.level, :mi.str, :mi.def, :mi.spd, :mi.dex, :mi.total) "
        "RETURNING *;",
        PREPARE(true),
        PARAM(oatpp::Object<WarMemberInformationDto>, mi))

    QUERY(updateMember,
        "UPDATE war_member_information SET war_id=:mi.warId, name=:mi.name, level=:mi.level, "
        "str=:mi.str, def=:mi.def, spd=:mi.spd, dex=:mi.dex, total=:mi.total "
        "WHERE id=:mi.id RETURNING *;",
        PREPARE(true),
        PARAM(oatpp::Object<WarMemberInformationDto>, mi))

    QUERY(getMemberById,
        "SELECT * FROM war_member_information WHERE id=:id;",
        PREPARE(true),
        PARAM(oatpp::Int64, id))

    QUERY(listMembersForWar,
        "SELECT * FROM war_member_information WHERE war_id=:warId "
        "ORDER BY total DESC, level DESC LIMIT :limit OFFSET :offset;",
        PREPARE(true),
        PARAM(oatpp::Int64, warId),
        PARAM(oatpp::UInt32, limit),
        PARAM(oatpp::UInt32, offset))

    QUERY(deleteMemberById,
        "DELETE FROM war_member_information WHERE id=:id;",
        PREPARE(true),
        PARAM(oatpp::Int64, id))
};

#include OATPP_CODEGEN_END(DbClient)
