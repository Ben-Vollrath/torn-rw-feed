#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include "clients/TornFactionResponseDto.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class FactionDto : public oatpp::DTO
{
	DTO_INIT(FactionDto, DTO)

	DTO_FIELD(Int64, id);
	DTO_FIELD(String, name);
	DTO_FIELD(Int64, leaderTornId, "leader_torn_id");
	DTO_FIELD(Int64, coLeaderTornId, "co_leader_torn_id");

	static oatpp::Object<FactionDto> fromFactionResponse(const oatpp::Object<TornFactionBasicDto>& factionBasics)
	{
		auto faction = createShared();
		faction->id = factionBasics->id;
		faction->leaderTornId = factionBasics->leader_id;
		faction->coLeaderTornId = factionBasics->co_leader_id;
		faction->name = factionBasics->name;

		return faction;
	}
};

#include OATPP_CODEGEN_END(DTO)
