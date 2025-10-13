#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class FactionDto : public oatpp::DTO {
	DTO_INIT(FactionDto, DTO)

		DTO_FIELD(Int64, id);
	DTO_FIELD(String, name);
	DTO_FIELD(Int64, leaderTornId, "leader_torn_id");
	DTO_FIELD(Int64, coLeaderTornId, "co_leader_torn_id");
};

#include OATPP_CODEGEN_END(DTO)
