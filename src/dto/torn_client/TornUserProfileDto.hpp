#pragma once
#include "TornUserStatusDto.hpp"
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class TornUserProfileDto : public oatpp::DTO {
	DTO_INIT(TornUserProfileDto, DTO)

		DTO_FIELD(Int32, id);
	DTO_FIELD(Int8, level);
	DTO_FIELD(String, gender);
	DTO_FIELD(Object<TornUserStatusDto>, status);
};

#include OATPP_CODEGEN_END(DTO)
