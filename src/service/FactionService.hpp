#pragma once

#include "CrudService.hpp"
#include "db/FactionDb.hpp"

class FactionService : public CrudService<FactionDb, FactionDto> {
 public:
  using CrudService::create;
  using CrudService::getById;
  using CrudService::removeById;
  using CrudService::update;
  using CrudService::upsertById;
};
