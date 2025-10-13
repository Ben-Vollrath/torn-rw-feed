#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class WarDto : public oatpp::DTO {
	DTO_INIT(WarDto, DTO)

		DTO_FIELD(Int64, id);
	DTO_FIELD(Int64, factionId, "faction_id");
	DTO_FIELD(Int64, enemyFactionId, "enemy_faction_id");
	DTO_FIELD(Int64, startAt, "start_at");
	DTO_FIELD(Int64, endAt, "end_at");
	DTO_FIELD(Int64, winnerId, "winner_id");
	DTO_FIELD(Int32, factionScore, "faction_score");
	DTO_FIELD(Int32, factionChain, "faction_chain");
	DTO_FIELD(Int32, enemyFactionScore, "enemy_faction_score");
	DTO_FIELD(Int32, enemyFactionChain, "enemy_faction_chain");
};

#include OATPP_CODEGEN_END(DTO)
