#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class PurchaseDto : public oatpp::DTO
{
	DTO_INIT(PurchaseDto, DTO)

	DTO_FIELD(Int64, id);
	DTO_FIELD(Int64, factionId, "faction_id");
	DTO_FIELD(Int64, purchaserId, "purchaser_id");
	DTO_FIELD(Int64, amount);
	DTO_FIELD(String, fullLog, "full_log");
	DTO_FIELD(Int64, startDate, "start_date");
	DTO_FIELD(Int64, endDate, "end_date");
};

#include OATPP_CODEGEN_END(DTO)
