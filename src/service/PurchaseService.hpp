#pragma once

#include "db/PurchaseDb.hpp"
#include "CrudService.hpp"

class PurchaseService : public CrudService<PurchaseDb, PurchaseDTO>
{
	using CrudService::create;
	using CrudService::update;
	using CrudService::getById;
	using CrudService::removeById;
};