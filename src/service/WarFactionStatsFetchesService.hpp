#pragma once

#include "db/WarFactionStatsFetchesDb.hpp"
#include "CrudService.hpp"
#include "dto/clients/TornFactionWarResponseDto.hpp"

class WarFactionStatsService : public CrudService<WarFactionStatsFetchesDb, WarFactionStatsFetchesDto> {
public:
	using CrudService::create;
	oatpp::Object<WarFactionStatsFetchesDto> getByFactionAndWarId(std::int64_t factionId, std::int64_t warId);
	oatpp::Object<WarFactionStatsFetchesDto> createWithIds(std::int64_t factionId, std::int64_t warId);
};
