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


#include OATPP_CODEGEN_BEGIN(DTO)

class TornUserProfileDto : public oatpp::DTO {
	DTO_INIT(TornUserProfileDto, DTO)

		DTO_FIELD(Int32, id);
	DTO_FIELD(Int8, level);
	DTO_FIELD(String, gender);
	DTO_FIELD(Object<TornUserStatusDto>, status);
};

#include OATPP_CODEGEN_END(DTO)


#include OATPP_CODEGEN_BEGIN(DTO)

class TornUserBasicResponseDto : public oatpp::DTO {
	DTO_INIT(TornUserBasicResponseDto, DTO)

		DTO_FIELD(Object<TornUserProfileDto>, profile);
};

#include OATPP_CODEGEN_END(DTO)
