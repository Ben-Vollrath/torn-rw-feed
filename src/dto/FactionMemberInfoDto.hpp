#pragma once


#include "Enums.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

struct FactionMemberInfo
{
	std::int64_t id;
	std::string name;
	std::int8_t level;
	TornActionStatus status;
	std::int64_t actionTimestamp;
	TornUserStatusState statusState;
};


#include OATPP_CODEGEN_BEGIN(DTO)

class FactionMemberInfoDto : public oatpp::DTO
{
	DTO_INIT(FactionMemberInfoDto, DTO)

	DTO_FIELD(Int64, id);
	DTO_FIELD(String, name);
	DTO_FIELD(Int8, level);
	DTO_FIELD(Enum<TornActionStatus>::AsString, status);
	DTO_FIELD(Int64, actionTimestamp);
	DTO_FIELD(Enum<TornUserStatusState>::AsString, statusState);

	static oatpp::Object<FactionMemberInfoDto> fromInfoStruct(const FactionMemberInfo& memberInfo)
	{
		auto dto = createShared();
		dto->id = memberInfo.id;
		dto->name = memberInfo.name;
		dto->level = memberInfo.level;
		dto->status = memberInfo.status;
		dto->actionTimestamp = memberInfo.actionTimestamp;
		dto->statusState = memberInfo.statusState;
		return dto;
	}
};

#include OATPP_CODEGEN_END(DTO)


#include OATPP_CODEGEN_BEGIN(DTO)

class FactionMemberInfoResponseDto : public oatpp::DTO
{
	DTO_INIT(FactionMemberInfoResponseDto, DTO)

	DTO_FIELD(Vector<Object<FactionMemberInfoDto>>, members);

	static oatpp::Object<FactionMemberInfoResponseDto> fromInfoList(std::vector<FactionMemberInfo>& memberInfos)
	{
		auto dto = createShared();
		dto->members = oatpp::Vector<oatpp::Object<FactionMemberInfoDto>>::createShared();
		dto->members->reserve(memberInfos.size());

		for (const auto& memberInfo : memberInfos)
		{
			dto->members->emplace_back(FactionMemberInfoDto::fromInfoStruct(memberInfo));
		}
		return dto;
	}

	static oatpp::Object<FactionMemberInfoResponseDto> fromMembersState(
		std::unordered_map<std::int64_t, FactionMemberInfo> membersState)
	{
		auto dto = createShared();
		dto->members = oatpp::Vector<oatpp::Object<FactionMemberInfoDto>>::createShared();
		dto->members->reserve(membersState.size());

		for (const auto& statePair : membersState)
		{
			dto->members->emplace_back(FactionMemberInfoDto::fromInfoStruct(statePair.second));
		}

		return dto;
	}
};

#include OATPP_CODEGEN_END(DTO)
