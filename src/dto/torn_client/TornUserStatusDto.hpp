#pragma once
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class TornUserStatusDto : public oatpp::DTO {
	DTO_INIT(TornUserStatusDto, DTO)

		DTO_FIELD(String, description);
	DTO_FIELD(Any, details);
	DTO_FIELD(String, state);
	DTO_FIELD(String, color);
	DTO_FIELD(Any, until);
};

#include OATPP_CODEGEN_END(DTO)
