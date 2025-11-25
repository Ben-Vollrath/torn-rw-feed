#pragma once

#include "Enums.hpp"
#include "clients/FFScouterResponseDto.hpp"
#include "clients/TornStatsSpieResponseDto.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class MemberStatsDto : public oatpp::DTO
{
	DTO_INIT(MemberStatsDto, DTO)

	DTO_FIELD(Int64, war_id);
	DTO_FIELD(Int64, faction_id);
	DTO_FIELD(Int64, member_id);
	DTO_FIELD(Int64, str);
	DTO_FIELD(Int64, def);
	DTO_FIELD(Int64, spd);
	DTO_FIELD(Int64, dex);
	DTO_FIELD(Int64, total);

	DTO_FIELD(Enum<MemberStatsType>::AsString, type);

	static oatpp::Object<MemberStatsDto> fromFFScouterItem(std::int64_t warId, std::int64_t factionId,
	                                                       const oatpp::Object<FFScouterItemDto>& rsp)
	{
		auto dto = createShared();
		dto->total = rsp->bs_estimate;
		dto->member_id = rsp->player_id;
		dto->war_id = warId;
		dto->faction_id = factionId;
		dto->type = MemberStatsType::FFSCOUTER;
		return dto;
	}

	static oatpp::Vector<oatpp::Object<MemberStatsDto>> fromFFScouterResponse(
		std::int64_t warId, std::int64_t factionId, const FFScouterResponseDto& rsp)
	{
		auto out = oatpp::Vector<oatpp::Object<MemberStatsDto>>::createShared();
		for (const auto& ffScout : *rsp)
		{
			out->emplace_back(fromFFScouterItem(warId, factionId, ffScout));
		}
		return out;
	}

	static oatpp::Object<MemberStatsDto> fromTornStatsSpyItem(std::int64_t warId, std::int64_t factionId,
	                                                          std::int64_t memberId, const oatpp::Object<SpyDto>& spy)
	{
		auto dto = createShared();
		dto->total = spy->total;
		dto->str = spy->strength;
		dto->def = spy->defense;
		dto->spd = spy->speed;
		dto->dex = spy->dexterity;
		dto->member_id = memberId;
		dto->war_id = warId;
		dto->faction_id = factionId;
		dto->type = MemberStatsType::TORNSTATSSPIES;
		return dto;
	}

	static oatpp::Vector<oatpp::Object<MemberStatsDto>> fromTornStatsSpyResponse(
		std::int64_t warId, std::int64_t factionId, const oatpp::Object<TornStatsSpyResponseDto>& spy)
	{
		auto out = oatpp::Vector<oatpp::Object<MemberStatsDto>>::createShared();
		for (const auto& pair : *spy->faction->members)
		{
			if (!pair.second->spy)
			{
				continue;
			}

			out->emplace_back(fromTornStatsSpyItem(warId, factionId, std::stoll(pair.first), pair.second->spy));
		}
		return out;
	}
};

#include OATPP_CODEGEN_END(DTO)
