#pragma once

#include <cassert>
#include <optional>

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class TornFactionRankedWarFactionDto : public oatpp::DTO
{
	DTO_INIT(TornFactionRankedWarFactionDto, DTO)

	DTO_FIELD(Int64, id);
	DTO_FIELD(String, name);
	DTO_FIELD(Int32, score);
	DTO_FIELD(Int32, chain);
};

class TornFactionRankedWarDto : public oatpp::DTO
{
	DTO_INIT(TornFactionRankedWarDto, DTO)

	DTO_FIELD(Int64, war_id);
	DTO_FIELD(Int64, start);
	DTO_FIELD(Int64, end);
	DTO_FIELD(Int64, target);
	DTO_FIELD(Int64, winner);
	DTO_FIELD(List<Object<TornFactionRankedWarFactionDto>>, factions);
};


class TornFactionWarsDto : public oatpp::DTO
{
	DTO_INIT(TornFactionWarsDto, DTO)

	DTO_FIELD(Object<TornFactionRankedWarDto>, ranked);
};


class TornFactionWarResponseDto : public oatpp::DTO
{
	DTO_INIT(TornFactionWarResponseDto, DTO)

	DTO_FIELD(Object<TornFactionWarsDto>, wars);


	std::optional<std::int64_t> getEnemyFactionId(std::int64_t factionId)
	{
		if (!this->wars->ranked) {
			return std::nullopt;
		}

		const auto& factions = this->wars->ranked->factions;

		OATPP_ASSERT_HTTP(factions->size() == 2, oatpp::web::protocol::http::Status::CODE_502, "502")

		if (factions[0]->id == factionId)
			return factions[1]->id;

		assert(factions[1]->id == factionId);
		return factions[0]->id;
	}

	std::optional<std::int64_t> getWarId()
	{
		if (!this->wars->ranked->war_id)
		{
			return std::nullopt;
		}
		return this->wars->ranked->war_id;
	}
};

#include OATPP_CODEGEN_END(DTO)
