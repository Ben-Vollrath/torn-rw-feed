#pragma once

#include "db/WarDb.hpp"
#include "CrudService.hpp"
#include "dto/torn_client/TornFactionWarResponseDto.hpp"

class WarService : public CrudService<WarDb, WarDto>
{
	using CrudService::create;
	using CrudService::update;
	using CrudService::getById;
	using CrudService::removeById;
	oatpp::Object<WarDto> upsertFromTornResponse(oatpp::Object<TornFactionRankedWarDto> warInfo);
};
