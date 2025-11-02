#pragma once

#include "dto/WarMemberInformationDto.hpp"
#include "oatpp-postgresql/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class WarMemberInformationDb : public oatpp::orm::DbClient
{
public:
	WarMemberInformationDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
		: DbClient(executor)
	{
		oatpp::orm::SchemaMigration m(executor, "war_member_information");
		m.addFile(1, SQL_FILE_PATH"war_member_information/init.sql");
		m.migrate();
	}

	QUERY(create,
	      "INSERT INTO war_member_information (war_id, name, level, str, def, spd, dex, total) "
	      "VALUES (:mi.war_id, :mi.name, :mi.level, :mi.str, :mi.def, :mi.spd, :mi.dex, :mi.total) "
	      "RETURNING *;",
	      PARAM(oatpp::Object<WarMemberInformationDto>, mi))

	QUERY(update,
	      "UPDATE war_member_information SET war_id=:mi.war_id, name=:mi.name, level=:mi.level, "
	      "str=:mi.str, def=:mi.def, spd=:mi.spd, dex=:mi.dex, total=:mi.total "
	      "WHERE id=:mi.id RETURNING *;",
	      PARAM(oatpp::Object<WarMemberInformationDto>, mi))

	QUERY(getById,
	      "SELECT * FROM war_member_information WHERE id=:id;",
	      PARAM(oatpp::Int64, id))

	QUERY(listMembersForWar,
	      "SELECT * FROM war_member_information WHERE war_id=:war_id "
	      "ORDER BY total DESC, level DESC LIMIT :limit OFFSET :offset;",
	      PARAM(oatpp::Int64, war_id),
	      PARAM(oatpp::UInt32, limit),
	      PARAM(oatpp::UInt32, offset))

	QUERY(deleteById,
	      "DELETE FROM war_member_information WHERE id=:id;",
	      PARAM(oatpp::Int64, id))
};

#include OATPP_CODEGEN_END(DbClient)
