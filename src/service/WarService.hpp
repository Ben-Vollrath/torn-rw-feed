#pragma once

#include "db/WarDb.hpp"
#include "CrudService.hpp"
#include "dto/clients/TornFactionWarResponseDto.hpp"

class WarService : public CrudService<WarDb, WarDto>
{
public:
	using CrudService::create;
	using CrudService::update;
	using CrudService::getById;
	using CrudService::removeById;
	oatpp::Object<WarDto> upsertFromTornResponse(oatpp::Object<TornFactionRankedWarDto> warInfo);
};
