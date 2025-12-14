#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class UserDto : public oatpp::DTO
{
	DTO_INIT(UserDto, DTO)

	DTO_FIELD(Int64, id);
	DTO_FIELD(String, tornKey, "torn_key");
	DTO_FIELD(Int64, factionId, "faction_id");
	DTO_FIELD(Int64, createdAt, "created_at");
};

class TornKeyRow : public oatpp::DTO
{
	DTO_INIT(TornKeyRow, DTO)
	DTO_FIELD(oatpp::String, torn_key);
};

#include OATPP_CODEGEN_END(DTO)
