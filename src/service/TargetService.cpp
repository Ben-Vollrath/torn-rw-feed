#include "TargetService.hpp"


oatpp::Object<TargetsDto> TargetService::getAllForUser(std::int64_t enemyFactionId, std::int64_t userId)
{
	auto qr = db_->getAllByEnemyFactionAndUser(enemyFactionId, userId);
	auto row = fetchOneOrNone(qr);
	if (!row)
	{
		row = TargetsDbDto::createFromFactionAndUser(enemyFactionId, userId);
	}
	return TargetsDto::fromDbDto(row);
}
