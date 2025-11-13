#pragma once


#include "Enums.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include "clients/TornFactionMembersResponseDto.hpp"


#include OATPP_CODEGEN_BEGIN(DTO)

class FactionMemberInfoResponseDto : public oatpp::DTO
{
	DTO_INIT(FactionMemberInfoResponseDto, DTO)

	DTO_FIELD(Vector<Object<TornFactionMember>>, members);

	static oatpp::Object<FactionMemberInfoResponseDto> fromInfoList(const Vector<Object<TornFactionMember>>& memberInfos)
	{
		auto dto = createShared();
		dto->members = memberInfos;
		return dto;
	}

	static oatpp::Object<FactionMemberInfoResponseDto> fromMembersState(
		const std::unordered_map<std::int64_t, oatpp::Object<TornFactionMember>>& membersState)
	{
		auto dto = createShared();
		dto->members = oatpp::Vector<oatpp::Object<TornFactionMember>>::createShared();
		dto->members->reserve(membersState.size());

		for (const auto& statePair : membersState)
		{
			dto->members->emplace_back(statePair.second);
		}

		return dto;
	}
};

#include OATPP_CODEGEN_END(DTO)
