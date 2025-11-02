#pragma once

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class TornFactionRankedWarFactionDto : public oatpp::DTO
{
	DTO_INIT(TornFactionRankedWarFactionDto, DTO)

	DTO_FIELD(Int64, id);
	DTO_FIELD(String, name);
	DTO_FIELD(Int32, score);
	DTO_FIELD(Int32, chain);
};

class TornFactionRankedWarDto : public oatpp::DTO
{
	DTO_INIT(TornFactionRankedWarDto, DTO)

	DTO_FIELD(Int64, war_id);
	DTO_FIELD(Int64, start);
	DTO_FIELD(Int64, end);
	DTO_FIELD(Int64, target);
	DTO_FIELD(Int64, winner);
	DTO_FIELD(List<Object<TornFactionRankedWarFactionDto>>, factions);
};


class TornFactionWarsDto : public oatpp::DTO
{
	DTO_INIT(TornFactionWarsDto, DTO)

	DTO_FIELD(Object<TornFactionRankedWarDto>, ranked);
};


class TornFactionWarResponseDto : public oatpp::DTO
{
	DTO_INIT(TornFactionWarResponseDto, DTO)

	DTO_FIELD(Object<TornFactionWarsDto>, wars);
};

#include OATPP_CODEGEN_END(DTO)
