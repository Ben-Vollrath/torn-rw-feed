#pragma once
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class TornErrorDto : public oatpp::DTO {
  DTO_INIT(TornErrorDto, DTO)

  DTO_FIELD(Int8, code);
  DTO_FIELD(String, error);
};

class TornErrorResponseDto : public oatpp::DTO {
  DTO_INIT(TornErrorResponseDto, DTO)

  DTO_FIELD(Object<TornErrorDto>, error);
};

#include OATPP_CODEGEN_END(DTO)
