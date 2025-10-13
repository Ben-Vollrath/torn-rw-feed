#pragma once

#include "db/ApiKeyDb.hpp"
#include "CrudService.hpp"

class ApiKeyService : public CrudService<ApiKeyDb, ApiKeyDto>
{
	using CrudService::create;
	using CrudService::update;
	using CrudService::getById;
	using CrudService::removeById;
};