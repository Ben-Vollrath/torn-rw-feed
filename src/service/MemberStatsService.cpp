#include "MemberStatsService.hpp"


oatpp::Vector<oatpp::Object<MemberStatsDto>> MemberStatsService::createMany(
	const oatpp::Vector<oatpp::Object<MemberStatsDto>>& memberStats)
{
	auto out = oatpp::Vector<oatpp::Object<MemberStatsDto>>::createShared();
	for (const oatpp::Object<MemberStatsDto>& memberStat : *memberStats)
	{
		auto qr = db_->upsert(memberStat);

		out->emplace_back(fetchOne(qr));
	}
	return out;
}

oatpp::Vector<oatpp::Object<MemberStatsDto>> MemberStatsService::getAllForWar(std::int64_t warId,
                                                                              std::int64_t factionId)
{
	auto qr = db_->findByWarAndFaction(warId, factionId);
	OATPP_ASSERT_HTTP(qr->isSuccess(), Status::CODE_500, qr->getErrorMessage());
	auto rows = qr->fetch<oatpp::Vector<oatpp::Object<MemberStatsDto>>>();
	return rows;
}
