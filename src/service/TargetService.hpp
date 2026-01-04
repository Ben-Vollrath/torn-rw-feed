#pragma once

#include "CrudService.hpp"
#include "db/TargetsDb.hpp"
#include "dto/TargetsDto.hpp"

class TargetService : public CrudService<TargetsDb, TargetsDbDto> {
 public:
  using CrudService::create;
  oatpp::Object<TargetsDto> getAllForUser(std::int64_t enemyFactionId,
                                          std::int64_t userId);
};
