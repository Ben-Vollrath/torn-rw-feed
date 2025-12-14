#pragma once

#include <optional>
#include "TornFactionMembersResponseDto.hpp"
#include "TornFactionWarResponseDto.hpp"
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class TornFactionWarAndMembersResponseDto : public oatpp::DTO
{
	DTO_INIT(TornFactionWarAndMembersResponseDto, DTO)

	DTO_FIELD(Object<TornFactionBasicDto>, basic);
	DTO_FIELD(Object<TornFactionWarsDto>, wars);
	DTO_FIELD(List<Object<TornFactionMember>>, members);

public:
	std::optional<std::int64_t> getEnemyFactionId(std::int64_t factionId) const
	{
		return wars ? wars->getEnemyFactionId(factionId) : std::nullopt;
	}

	std::optional<std::int64_t> getWarId() const
	{
		return wars ? wars->getWarId() : std::nullopt;
	}

	bool isWarActive() const
	{
		return wars ? wars->isWarActive() : false;
	}
};


#include OATPP_CODEGEN_END(DTO)
