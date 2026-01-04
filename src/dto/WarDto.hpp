#pragma once

#include "clients/TornFactionWarResponseDto.hpp"
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class WarDto : public oatpp::DTO {
  DTO_INIT(WarDto, DTO)

  DTO_FIELD(Int64, id);
  DTO_FIELD(Int64, factionOneId, "faction_one_id");
  DTO_FIELD(Int64, factionTwoId, "faction_two_id");
  DTO_FIELD(Int64, startAt, "start_at");
  DTO_FIELD(Int64, endAt, "end_at");
  DTO_FIELD(Int64, winnerId, "winner_id");
  DTO_FIELD(Int32, factionOneScore, "faction_one_score");
  DTO_FIELD(Int32, factionOneChain, "faction_one_chain");
  DTO_FIELD(Int32, factionTwoScore, "faction_two_score");
  DTO_FIELD(Int32, factionTwoChain, "faction_two_chain");

  static oatpp::Object<WarDto> fromWarResponse(
      const oatpp::Object<TornFactionRankedWarDto>& warInfo) {
    auto war = createShared();
    war->id = warInfo->war_id;
    war->startAt = warInfo->start;
    war->endAt = warInfo->end;
    auto factionOne = warInfo->factions[0];
    auto factionTwo = warInfo->factions[1];

    war->factionOneId = factionOne->id;
    war->factionOneScore = factionOne->score;
    war->factionOneChain = factionOne->chain;

    war->factionTwoId = factionTwo->id;
    war->factionTwoScore = factionTwo->score;
    war->factionTwoChain = factionTwo->chain;

    return war;
  }
};

#include OATPP_CODEGEN_END(DTO)
