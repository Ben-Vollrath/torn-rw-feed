#include "WarService.hpp"


oatpp::Object<WarDto> WarService::upsertFromTornResponse(oatpp::Object<TornFactionRankedWarDto> warInfo)
{
	auto qr = db_->upsertById(WarDto::fromWarResponse(warInfo));
	return fetchOne(qr);
}
