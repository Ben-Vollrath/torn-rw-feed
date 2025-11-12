#include "MemberStatsService.hpp"


void MemberStatsService::createMany(
	const std::vector<oatpp::Object<MemberStatsDto>>& memberStats)
{
	for (const oatpp::Object<MemberStatsDto>& memberStat : memberStats)
	{
		create(memberStat);
	}
}

oatpp::Vector<oatpp::Object<MemberStatsDto>> MemberStatsService::getAllForWar(std::int64_t warId,
	std::int64_t factionId)
{
	auto qr = db_->findByWarAndFaction(warId, factionId);
	OATPP_ASSERT_HTTP(qr->isSuccess(), Status::CODE_500, qr->getErrorMessage());
	auto rows = qr->fetch<oatpp::Vector<oatpp::Object<MemberStatsDto>>>();
	return rows;
}
