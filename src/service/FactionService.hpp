#pragma once

#include "db/FactionDb.hpp"
#include "CrudService.hpp"

class FactionService : public CrudService<FactionDb, FactionDto>
{
public:
	using CrudService::create;
	using CrudService::upsertById;
	using CrudService::update;
	using CrudService::getById;
	using CrudService::removeById;
};
