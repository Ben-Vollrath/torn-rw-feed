#pragma once

#include "db/WarDb.hpp"
#include "CrudService.hpp"

class WarService : public CrudService<WarDb, WarDTO>
{
	using CrudService::create;
	using CrudService::update;
	using CrudService::getById;
	using CrudService::removeById;
};