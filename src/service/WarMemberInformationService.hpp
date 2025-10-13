#pragma once

#include "db/WarMemberInformationDb.hpp"
#include "CrudService.hpp"

class WarMemberInformationService : public CrudService<WarMemberInformationDb, WarMemberInformationDto>
{
	using CrudService::create;
	using CrudService::update;
	using CrudService::getById;
	using CrudService::removeById;
};