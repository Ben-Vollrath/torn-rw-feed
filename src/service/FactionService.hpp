#pragma once

#include "db/FactionDb.hpp"
#include "CrudService.hpp"

class FactionService : public CrudService<FactionDb, FactionDTO>
{
	using CrudService::create;
	using CrudService::update;
	using CrudService::getById;
	using CrudService::removeById;
};