#pragma once
#include "TornUserBasicResponseDto.hpp"
#include "dto/FactionMemberInfoDto.hpp"
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
	DTO_FIELD(Object<TornUserLastAction>, last_action);
	DTO_FIELD(Object<TornUserStatusDto>, status);
};


class TornFactionMembersResponse : public oatpp::DTO
{
	DTO_INIT(TornFactionMembersResponse, DTO)

	DTO_FIELD(List<Object<TornFactionMember>>, members);

	std::vector<FactionMemberInfo> toFactionMemberInfoStruct()
	{
		std::vector<FactionMemberInfo> out;
		if (!members) return out;
		out.reserve(members->size());

		for (const oatpp::Object<TornFactionMember>& m : *members)
		{
			FactionMemberInfo info{
				m->id,
				m->name,
				m->level,
				m->last_action->status,
				m->last_action->timestamp,
				m->status->state
			};
			out.push_back(std::move(info));
		}
		return out;
	}
};

#include OATPP_CODEGEN_END(DTO)
