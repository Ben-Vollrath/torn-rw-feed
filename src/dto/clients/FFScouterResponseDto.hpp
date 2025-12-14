#pragma once
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"


#include OATPP_CODEGEN_BEGIN(DTO)

class FFScouterItemDto : public oatpp::DTO
{
	DTO_INIT(FFScouterItemDto, DTO)

	DTO_FIELD(Int64, player_id);
	DTO_FIELD(Int64, bs_estimate);
};

#include OATPP_CODEGEN_END(DTO)

using FFScouterResponseDto = oatpp::List<oatpp::Object<FFScouterItemDto>>;
