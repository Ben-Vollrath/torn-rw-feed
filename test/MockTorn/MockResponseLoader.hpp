#pragma once
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "oatpp/core/macro/component.hpp"

class MockResponseLoader
{
	OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper);

	std::vector<std::string> responsePaths;
	std::int8_t index = 0;

public:
	void setResponsePaths(const std::vector<std::string>& paths)
	{
		responsePaths = paths;
		index = 0;
	}

	std::string getNextResponse()
	{
		if (index >= responsePaths.size())
		{
			throw std::runtime_error("Not enough responses configured.");
		}
		auto response = loadFile(responsePaths[index]);
		index++;

		return response;
	}

	template <typename DtoT>
	DtoT loadDtoFromFile(const std::string& path)
	{
		return apiObjectMapper->readFromString<DtoT>(loadFile(path));
	}

private:
	static std::string loadFile(const std::string& path)
	{
		std::ifstream f(path);
		if (!f) throw std::runtime_error("Cannot open fixture: " + path);
		std::ostringstream ss;
		ss << f.rdbuf();
		return ss.str();
	}
};

inline auto factionBasicOKPath_ = MOCK_RESPONSE_BASE "/torn_faction_basic_ok.json";
inline auto factionMembersOfflineOKPath_ = MOCK_RESPONSE_BASE "/torn_faction_members_offline_ok.json";
inline auto factionMembersOneOnlineOKPath_ = MOCK_RESPONSE_BASE "/torn_faction_members_one_online_ok.json";
inline auto factionWarOKPath_ = MOCK_RESPONSE_BASE "/torn_faction_war_ok.json";
inline auto factionWarChangedScoreOKPath_ = MOCK_RESPONSE_BASE "/torn_faction_war_changed_score_ok.json";
inline auto factionWarNoWarOKPath_ = MOCK_RESPONSE_BASE "/torn_faction_war_no_war_ok.json";
inline auto userBasicOkPath_ = MOCK_RESPONSE_BASE "/torn_user_basic_ok.json";
inline auto errorInactiveKey_ = MOCK_RESPONSE_BASE "/torn_error_incorrect_api_key.json";
inline auto errorTooManyRequests_ = MOCK_RESPONSE_BASE "/torn_error_too_many_requests.json";

inline auto ffscouterScoutOkPath_ = MOCK_RESPONSE_BASE "/ffscouter_scout_ok.json";
inline auto tornStatsSpyOkPath_ = MOCK_RESPONSE_BASE "/tornstats_spy_ok.json";
