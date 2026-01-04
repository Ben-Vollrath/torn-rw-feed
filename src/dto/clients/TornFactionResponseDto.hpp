#pragma once
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class TornFactionRankDto : public oatpp::DTO {
  DTO_INIT(TornFactionRankDto, DTO)

  DTO_FIELD(Int32, level);
  DTO_FIELD(String, name);
  DTO_FIELD(Int32, division);
  DTO_FIELD(Int32, position);
  DTO_FIELD(Int32, wins);
};

class TornFactionBasicDto : public oatpp::DTO {
  DTO_INIT(TornFactionBasicDto, DTO)

  DTO_FIELD(Int32, id);
  DTO_FIELD(String, name);
  DTO_FIELD(String, tag);
  DTO_FIELD(String, tag_image);
  DTO_FIELD(Int32, leader_id);
  DTO_FIELD(Int32, co_leader_id);
  DTO_FIELD(Int64, respect);
  DTO_FIELD(Int32, days_old);
  DTO_FIELD(Int32, capacity);
  DTO_FIELD(Int32, members);
  DTO_FIELD(Any, is_enlisted);  // or Bool if it's always true/false/null
  DTO_FIELD(Object<TornFactionRankDto>, rank);
  DTO_FIELD(Int32, best_chain);
};

class TornFactionResponseDto : public oatpp::DTO {
  DTO_INIT(TornFactionResponseDto, DTO)

  DTO_FIELD(Object<TornFactionBasicDto>, basic);
};

#include OATPP_CODEGEN_END(DTO)
