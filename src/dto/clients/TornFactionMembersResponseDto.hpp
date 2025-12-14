#pragma once
#include "TornUserBasicResponseDto.hpp"
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class TornUserLastAction : public oatpp::DTO
{
	DTO_INIT(TornUserLastAction, DTO)

	DTO_FIELD(Enum<TornActionStatus>::AsString, status);
	DTO_FIELD(Int64, timestamp);
	DTO_FIELD(String, relative);
};


class TornFactionMember : public oatpp::DTO
{
	DTO_INIT(TornFactionMember, DTO)

	DTO_FIELD(Int64, id);
	DTO_FIELD(String, name);
	DTO_FIELD(Int8, level);
	DTO_FIELD(Boolean, is_revivable);
	DTO_FIELD(Boolean, has_early_discharge);
	DTO_FIELD(Object<TornUserLastAction>, last_action);
	DTO_FIELD(Object<TornUserStatusDto>, status);
};


class TornFactionMembersResponse : public oatpp::DTO
{
	DTO_INIT(TornFactionMembersResponse, DTO)

	DTO_FIELD(List<Object<TornFactionMember>>, members);
};

#include OATPP_CODEGEN_END(DTO)
