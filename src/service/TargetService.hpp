#pragma once

#include "db/TargetsDb.hpp"
#include "dto/TargetsDto.hpp"
#include "CrudService.hpp"

class TargetService : public CrudService<TargetsDb, TargetsDbDto>
{
public:
	using CrudService::create;
	oatpp::Object<TargetsDto> getAllForUser(std::int64_t enemyFactionId, std::int64_t userId);
};
