#pragma once
#include "TornUserProfileDto.hpp"
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class TornUserBasicResponseDto : public oatpp::DTO {
	DTO_INIT(TornUserBasicResponseDto, DTO)

		DTO_FIELD(Object<TornUserProfileDto>, profile);
};

#include OATPP_CODEGEN_END(DTO)
