#pragma once

#include <sstream>
#include <string>
#include <vector>

inline std::vector<std::string> split(const std::string& target, char c)
{
	std::string temp;
	std::stringstream stringstream{target};
	std::vector<std::string> result;

	while (std::getline(stringstream, temp, c))
	{
		result.push_back(temp);
	}

	return result;
}
