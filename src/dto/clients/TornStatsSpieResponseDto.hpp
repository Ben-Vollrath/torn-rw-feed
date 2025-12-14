#pragma once
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class SpyDto : public oatpp::DTO
{
	DTO_INIT(SpyDto, DTO)
	DTO_FIELD(Int64, strength);
	DTO_FIELD(Int64, defense);
	DTO_FIELD(Int64, speed);
	DTO_FIELD(Int64, dexterity);
	DTO_FIELD(Int64, total);
};

class MemberDto : public oatpp::DTO
{
	DTO_INIT(MemberDto, DTO)
	DTO_FIELD(Object<SpyDto>, spy);
};

class TornStatsFactionDto : public oatpp::DTO
{
	DTO_INIT(TornStatsFactionDto, DTO)
	DTO_FIELD(UnorderedFields<Object<MemberDto>>, members);
};

class TornStatsSpyResponseDto : public oatpp::DTO
{
	DTO_INIT(TornStatsSpyResponseDto, DTO)

	DTO_FIELD(Object<TornStatsFactionDto>, faction);
};

#include OATPP_CODEGEN_END(DTO)
