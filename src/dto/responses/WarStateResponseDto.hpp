#pragma once


#include "dto/Enums.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include "dto/clients/TornFactionMembersResponseDto.hpp"
#include "dto/clients/TornFactionWarResponseDto.hpp"
#include "dto/MemberStatsDto.hpp"
#include "dto/TargetsDto.hpp"


#include OATPP_CODEGEN_BEGIN(DTO)

class WarStateResponseDto : public oatpp::DTO
{
	DTO_INIT(WarStateResponseDto, DTO)

	DTO_FIELD(Fields<Object<MemberStatsDto>>, memberStats);
	DTO_FIELD(Vector<Object<TornFactionMember>>, members);
	DTO_FIELD(Object<TornFactionWarsDto>, war);
	DTO_FIELD(Object<TornFactionMember>, user);
	DTO_FIELD(Vector<Object<UpdateTargetDto>>, targets);

	static oatpp::Object<WarStateResponseDto> fromUser(const Object<TornFactionMember>& user)
	{
		auto dto = createShared();
		dto->user = user;
		dto->user->status->parseLocation();
		return dto;
	}

	static oatpp::Object<WarStateResponseDto> fromWar(const Object<TornFactionWarsDto>& war)
	{
		auto dto = createShared();
		dto->war = war;
		return dto;
	}

	static oatpp::Object<WarStateResponseDto> fromMembersInfo(const Vector<Object<TornFactionMember>>& memberInfos)
	{
		auto dto = createShared();
		dto->members = memberInfos;
		dto->parseMemberLocation();
		return dto;
	}

	static oatpp::Object<WarStateResponseDto> fromMembersInfo(
		const std::unordered_map<std::int64_t, oatpp::Object<TornFactionMember>>& membersState)
	{
		auto dto = createShared();
		dto->members = oatpp::Vector<oatpp::Object<TornFactionMember>>::createShared();
		dto->members->reserve(membersState.size());

		for (const auto& statePair : membersState)
		{
			dto->members->emplace_back(statePair.second);
		}

		dto->parseMemberLocation();
		return dto;
	}

	static oatpp::Object<WarStateResponseDto> fromMembersStats(
		const std::unordered_map<std::int64_t, oatpp::Object<MemberStatsDto>>& memberStats)
	{
		auto dto = createShared();
		dto->addMemberStats(memberStats);
		return dto;
	}

	static oatpp::Object<WarStateResponseDto> fromMembersStats(
		const oatpp::Vector<oatpp::Object<MemberStatsDto>>& memberStats)
	{
		auto dto = createShared();
		dto->addMemberStats(memberStats);
		return dto;
	}

	static oatpp::Object<WarStateResponseDto> fromTargets(const Vector<Object<UpdateTargetDto>>& targets)
	{
		auto dto = createShared();
		dto->addTargets(targets);
		return dto;
	}

	void addUser(const Object<TornFactionMember>& user)
	{
		this->user = user;
	}

	void addWar(const Object<TornFactionWarsDto>& war)
	{
		this->war = war;
	}


	void addMemberStats(const std::unordered_map<std::int64_t, oatpp::Object<MemberStatsDto>>& memberStats)
	{
		this->memberStats = Fields<Object<MemberStatsDto>>::createShared();
		for (const auto& statsPair : memberStats)
		{
			this->memberStats[std::to_string(statsPair.first)] = statsPair.second;
		}
	}

	void addMemberStats(const oatpp::Vector<oatpp::Object<MemberStatsDto>>& memberStats)
	{
		this->memberStats = Fields<Object<MemberStatsDto>>::createShared();
		for (const oatpp::Object<MemberStatsDto>& stats : *memberStats)
		{
			this->memberStats[std::to_string(stats->member_id)] = stats;
		}
	}

	void addTargets(const Vector<Object<UpdateTargetDto>>& targets)
	{
		this->targets = targets;
	}

	void parseMemberLocation()
	{
		for (const Object<TornFactionMember> member : *members)
		{
			member->status->parseLocation();
		}
	}
};

#include OATPP_CODEGEN_END(DTO)
