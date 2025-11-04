#pragma once
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

class MockResponseLoader
{
private:
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

private:

    static std::string loadFile(const std::string& path) {
        std::ifstream f(path);
        if (!f) throw std::runtime_error("Cannot open fixture: " + path);
        std::ostringstream ss; ss << f.rdbuf();
        return ss.str();
    }
};

inline const char* factionBasicOKFixturePath_ = FIXTURE_BASE "/torn_faction_basic_ok.json";
inline const char* userBasicOkFixturePath_ = FIXTURE_BASE "/torn_user_basic_ok.json";
inline const char* errorInactiveKey_ = FIXTURE_BASE "/torn_error_incorrect_api_key.json";
