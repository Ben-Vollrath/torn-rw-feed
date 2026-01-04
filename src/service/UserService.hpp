#pragma once

#include "CrudService.hpp"
#include "db/UserDb.hpp"
#include "dto/UserDto.hpp"

class UserService : public CrudService<UserDb, UserDto> {
 public:
  using CrudService::create;
  using CrudService::getById;
  using CrudService::getByIdNullable;
  using CrudService::removeById;
  using CrudService::update;
  using CrudService::upsertById;
  oatpp::Vector<oatpp::Object<TornKeyRow>> getTornKeys(std::int64_t factionId);
  void updateFaction(const std::string& tornKey, std::int64_t factionId);
  void removeTornKey(const std::string& tornKey);
};
