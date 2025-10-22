#pragma once
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

inline std::string loadFile(const std::string& path) {
	std::ifstream f(path);
	if (!f) throw std::runtime_error("Cannot open fixture: " + path);
	std::ostringstream ss; ss << f.rdbuf();
	return ss.str();
}


const std::string factionBasicOKFixturePath_ = FIXTURE_BASE "/torn_faction_basic_ok.json";
const std::string userBasicOkFixturePath_ = FIXTURE_BASE "/torn_user_basic_ok.json";

// Errors
const std::string errorInactiveKey_ = FIXTURE_BASE "/torn_error_incorrect_api_key.json";