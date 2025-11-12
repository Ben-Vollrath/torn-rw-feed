#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class WarFactionStatsFetchesDto : public oatpp::DTO {
	DTO_INIT(WarFactionStatsFetchesDto, DTO)

		DTO_FIELD(Int64, warId, "war_id");
		DTO_FIELD(Int64, factionId, "faction_id");
};

#include OATPP_CODEGEN_END(DTO)
