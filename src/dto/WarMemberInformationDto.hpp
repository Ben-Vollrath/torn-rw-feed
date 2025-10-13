#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class WarMemberInformationDto : public oatpp::DTO {
	DTO_INIT(WarMemberInformationDto, DTO)

		DTO_FIELD(Int64, id);
	DTO_FIELD(Int64, warId, "war_id");
	DTO_FIELD(String, name);
	DTO_FIELD(Int32, level);
	DTO_FIELD(Int64, str);
	DTO_FIELD(Int64, def);
	DTO_FIELD(Int64, spd);
	DTO_FIELD(Int64, dex);
	DTO_FIELD(Int64, total);
};

#include OATPP_CODEGEN_END(DTO)
