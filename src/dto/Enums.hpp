#pragma once
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

ENUM(TornActionStatus, v_int32,
     VALUE(ONLINE, 0, "Online"),
     VALUE(IDLE, 1, "Idle"),
     VALUE(OFFLINE, 2, "Offline")
)

ENUM(TornUserStatusState, v_int32,
     VALUE(ABROAD, 0, "Abroad"),
     VALUE(AWOKEN, 1, "Awoken"),
     VALUE(DORMANT, 2, "Dormant"),
     VALUE(FALLEN, 3, "Fallen"),
     VALUE(FEDERAL, 4, "Federal"),
     VALUE(HOSPITAL, 5, "Hospital"),
     VALUE(JAIL, 6, "Jail"),
     VALUE(OKAY, 7, "Okay"),
     VALUE(TRAVELING, 8, "Traveling")
)

#include OATPP_CODEGEN_END(DTO)
