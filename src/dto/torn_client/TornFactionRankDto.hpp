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

#include OATPP_CODEGEN_END(DTO)
