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

ENUM(TornLocation, v_int32,
     VALUE(MEXICO, 0, "Mexico"),
     VALUE(CAYMAN_ISLANDS, 1, "Cayman Islands"),
     VALUE(CANADA, 2, "Canada"),
     VALUE(HAWAII, 3, "Hawaii"),
     VALUE(UNITED_KINGDOM, 4, "United Kingdom"),
     VALUE(ARGENTINA, 5, "Argentina"),
     VALUE(SWITZERLAND, 6, "Switzerland"),
     VALUE(JAPAN, 7, "Japan"),
     VALUE(CHINA, 8, "China"),
     VALUE(UNITED_ARAB_EMIRATES, 9, "United Arab Emirates"),
     VALUE(SOUTH_AFRICA, 10, "South Africa"),
     VALUE(TORN, 11, "Torn")
)

ENUM(MemberStatsType, v_int32,
     VALUE(FFSCOUTER, 0, "ffscouter"),
     VALUE(TORNSTATSSPIES, 1, "tornstatsspies")
)

ENUM(TargetUpdateType, v_int32,
    VALUE(ADD, 0),
    VALUE(REMOVE, 1)
)

ENUM(TornKeyAccessType, v_int32,
    VALUE(CUSTOM, 0, "Custom"),
    VALUE(PUBLIC, 1, "Public Only"),
    VALUE(MINIMAL, 2, "Minimal Access"),
    VALUE(LIMITED, 3, "Limited Access"),
    VALUE(FULL, 4, "Full Access")
)

ENUM(ErrorMessage, v_int32,
    VALUE(KeyLimit, 0, "KeyLimit")
)

#include OATPP_CODEGEN_END(DTO)
