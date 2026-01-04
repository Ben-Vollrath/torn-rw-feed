#pragma once

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class SpyResponseDto : public oatpp::DTO {
  DTO_INIT(SpyResponseDto, DTO)

  DTO_FIELD(oatpp::Int64, importSize, "import_size");

  static oatpp::Object<SpyResponseDto> fromSize(const oatpp::Int64& size) {
    auto dto = createShared();
    dto->importSize = size;
    return dto;
  }
};

#include OATPP_CODEGEN_END(DTO)
