#pragma once

#include "CrudService.hpp"
#include "db/MemberStatsDb.hpp"


class MemberStatsService : public CrudService<MemberStatsDb, MemberStatsDto> {
public:
	oatpp::Vector < oatpp::Object<MemberStatsDto>> createMany(const oatpp::Vector<oatpp::Object<MemberStatsDto>>& memberStats);
	oatpp::Vector<oatpp::Object<MemberStatsDto>> getAllForWar(std::int64_t warId, std::int64_t factionId);

};
