#pragma once

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "dto/Enums.hpp"
#include <regex>

#include OATPP_CODEGEN_BEGIN(DTO)

class TornUserStatusDto : public oatpp::DTO
{
	DTO_INIT(TornUserStatusDto, DTO)

	DTO_FIELD(String, description);
	DTO_FIELD(Enum<TornUserStatusState>::AsString, state);
	DTO_FIELD(String, color);
	DTO_FIELD(Enum<TornLocation>::AsString, location);

public:
    void parseLocation() {
        location = TornLocation::TORN;

        if (!description || description->empty()) {
            return;
        }

        const std::string desc = description->c_str();
        std::smatch match;

        if (state == TornUserStatusState::HOSPITAL) {
            static const std::regex re(
                R"(In an? ([A-Za-z ]+?) hospital)",
                std::regex::icase
            );

            if (std::regex_search(desc, match, re) && match.size() > 1) {
                std::string nat = match[1].str();
                trimInPlace(nat);
                location = locationFromHospString(nat);
                return;
            }
        }
        else if (state == TornUserStatusState::ABROAD) {
            static const std::regex re(
                R"(In ([A-Za-z ]+))",
                std::regex::icase
            );

            if (std::regex_search(desc, match, re) && match.size() > 1) {
                std::string place = match[1].str();
                trimInPlace(place);
                location = locationFromString(place);
                return;
            }
        }
        else if (state == TornUserStatusState::TRAVELING) {
            static const std::regex re(
                R"(to ([A-Za-z ].*?)(?=from|$))",
                std::regex::icase
            );

            if (std::regex_search(desc, match, re) && match.size() > 1) {
                std::string dest = match[1].str();
                trimInPlace(dest);
                location = locationFromString(dest);
                return;
            }
        }

    }

private:
    static void trimInPlace(std::string& s) {
        auto notSpace = [](int ch) { return !std::isspace(ch); };

        // left trim
        s.erase(s.begin(),
            std::find_if(s.begin(), s.end(), notSpace));

        // right trim
        s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(),
            s.end());
    }

	TornLocation locationFromString(std::string& str) {
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);

		static const std::unordered_map<std::string, TornLocation> names = {
			{"mexico",                 TornLocation::MEXICO},
			{"cayman islands",         TornLocation::CAYMAN_ISLANDS},
			{"canada",                 TornLocation::CANADA},
			{"hawaii",                 TornLocation::HAWAII},
			{"united kingdom",         TornLocation::UNITED_KINGDOM},
			{"argentina",              TornLocation::ARGENTINA},
			{"switzerland",            TornLocation::SWITZERLAND},
			{"japan",                  TornLocation::JAPAN},
			{"china",                  TornLocation::CHINA},
			{"uae",					   TornLocation::UNITED_ARAB_EMIRATES},
			{"south africa",           TornLocation::SOUTH_AFRICA},
			{"torn",                   TornLocation::TORN}
		};

		auto it = names.find(str);
		if (it != names.end()) {
			return it->second;
		}

		return TornLocation::TORN;
	}


	TornLocation locationFromHospString(std::string str) {
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);

		static const std::unordered_map<std::string, TornLocation> names = {
			{"mexican",                TornLocation::MEXICO},
			{"caymanian",              TornLocation::CAYMAN_ISLANDS},
			{"canadian",               TornLocation::CANADA},
			{"hawaiian",               TornLocation::HAWAII},
			{"british",                TornLocation::UNITED_KINGDOM},
			{"argentinian",            TornLocation::ARGENTINA},
			{"swiss",                  TornLocation::SWITZERLAND},
			{"japanese",               TornLocation::JAPAN},
			{"chinese",                TornLocation::CHINA},
			{"emirati",                TornLocation::UNITED_ARAB_EMIRATES},
			{"south african",          TornLocation::SOUTH_AFRICA}
		};

		auto it = names.find(str);
		if (it != names.end()) {
			return it->second;
		}

		return TornLocation::TORN;
	}
};


class TornUserProfileDto : public oatpp::DTO
{
	DTO_INIT(TornUserProfileDto, DTO)

	DTO_FIELD(Int32, id);
	DTO_FIELD(Int8, level);
	DTO_FIELD(String, gender);
	DTO_FIELD(Object<TornUserStatusDto>, status);
};


class TornUserBasicResponseDto : public oatpp::DTO
{
	DTO_INIT(TornUserBasicResponseDto, DTO)

	DTO_FIELD(Object<TornUserProfileDto>, profile);
};

#include OATPP_CODEGEN_END(DTO)
