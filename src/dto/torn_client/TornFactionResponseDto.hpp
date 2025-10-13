#pragma once
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "TornFactionBasicDto.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class TornFactionResponseDto : public oatpp::DTO {
	DTO_INIT(TornFactionResponseDto, DTO)

		DTO_FIELD(Object<TornFactionBasicDto>, basic);
};

#include OATPP_CODEGEN_END(DTO)
