#pragma once

#include "db/UserDb.hpp"
#include "dto/UserDto.hpp"
#include "CrudService.hpp"

class UserService : public CrudService<UserDb, UserDto>
{
public:
	using CrudService::create;
	using CrudService::upsertById;
	using CrudService::update;
	using CrudService::getById;
	using CrudService::getByIdNullable;
	using CrudService::removeById;
	oatpp::Vector<oatpp::Object<TornKeyRow>> getTornKeys(std::int64_t factionId);
	void removeTornKey(const std::string& tornKey);
};
