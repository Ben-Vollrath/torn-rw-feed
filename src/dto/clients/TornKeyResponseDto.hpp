#pragma once
#include "dto/Enums.hpp"
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class TornKeyAccess : public oatpp::DTO
{
	DTO_INIT(TornKeyAccess, DTO);
	DTO_FIELD(Enum<TornKeyAccessType>::AsString, type);
};

class TornUserInfo : public oatpp::DTO
{
	DTO_INIT(TornUserInfo, DTO);
	DTO_FIELD(Int64, faction_id);
	DTO_FIELD(Int64, id);
};

class TornKeyInfo : public oatpp::DTO
{
	DTO_INIT(TornKeyInfo, DTO);
	DTO_FIELD(Object<TornUserInfo>, user);
	DTO_FIELD(Object<TornKeyAccess>, access);
};

class TornKeyResponseDto : public oatpp::DTO
{
	DTO_INIT(TornKeyResponseDto, DTO);
	DTO_FIELD(Object<TornKeyInfo>, info);
};

#include OATPP_CODEGEN_END(DTO)