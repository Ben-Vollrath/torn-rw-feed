#pragma once

#include <sstream>
#include <unordered_set>

#include "Enums.hpp"
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class TargetsDbDto : public oatpp::DTO {
  DTO_INIT(TargetsDbDto, DTO)

  DTO_FIELD(Int64, enemyFactionId, "enemy_faction_id");
  DTO_FIELD(Int64, userId, "user_id");
  DTO_FIELD(String, targets, "targets");

  static Object<TargetsDbDto> createFromFactionAndUser(
      std::int64_t enemyFactionId, std::int64_t userId) {
    auto dto = createShared();
    dto->enemyFactionId = enemyFactionId;
    dto->userId = userId;
    return dto;
  }
};

class TargetsDto : public oatpp::DTO {
  DTO_INIT(TargetsDto, DTO)

  DTO_FIELD(Object<TargetsDbDto>, dbDto);
  DTO_FIELD(UnorderedSet<Int64>, targets_set);

 public:
  static Object<TargetsDto> fromDbDto(const Object<TargetsDbDto>& db) {
    auto dto = createShared();
    dto->dbDto = db;
    dto->targets_set = UnorderedSet<Int64>::createShared();

    if (db && db->targets && !db->targets->empty()) {
      std::istringstream ss(db->targets->c_str());
      std::string tok;
      while (std::getline(ss, tok, ',')) {
        if (tok.empty()) continue;
        try {
          const auto id = std::stoll(tok);
          dto->targets_set->insert(Int64(id));
        } catch (...) {
          // ignore malformed tokens
        }
      }
    }
    return dto;
  }

  oatpp::Object<TargetsDbDto> getDbDto() {
    if (!targets_set || targets_set->empty()) {
      dbDto->targets = "";
      return dbDto;
    }

    std::ostringstream ss;
    bool first = true;
    for (const auto& id : *targets_set) {
      if (!first) ss << ',';
      first = false;
      ss << id;
    }
    dbDto->targets = oatpp::String(ss.str().c_str());
    return dbDto;
  }
};

class UpdateTargetDto : public oatpp::DTO {
  DTO_INIT(UpdateTargetDto, DTO)

  DTO_FIELD(Int64, targetId, "target_id");
  DTO_FIELD(Enum<TargetUpdateType>::AsString, updateType, "updateType");

  static Vector<Object<UpdateTargetDto>> fromTargetsDto(
      const Object<TargetsDto>& targetsDto) {
    auto vec = Vector<Object<UpdateTargetDto>>::createShared();
    for (auto target : *targetsDto->targets_set) {
      auto dto = UpdateTargetDto::createShared();
      dto->targetId = target;
      dto->updateType = TargetUpdateType::ADD;
      vec->emplace_back(dto);
    }
    return vec;
  }
};

#include OATPP_CODEGEN_END(DTO)
