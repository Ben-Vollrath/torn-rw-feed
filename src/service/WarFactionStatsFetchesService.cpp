#include "WarFactionStatsFetchesService.hpp"

oatpp::Object<WarFactionStatsFetchesDto> WarFactionStatsService::getByFactionAndWarId(std::int64_t factionId, std::int64_t warId)
{
	auto qr = db_->getByFactionAndWarId(factionId, warId);
	return fetchOneOrNone(qr);
}

oatpp::Object<WarFactionStatsFetchesDto> WarFactionStatsService::createWithIds(std::int64_t factionId,
	std::int64_t warId)
{
	auto qr = db_->create(warId, factionId);
	return fetchOne(qr);
}
