#include "TornApiServiceKeyManaged.hpp"

#include <cassert>


std::string TornApiServiceKeyManaged::getKey()
{
	assert(!m_keys.empty()); // Should never happen

	const std::string& key = m_keys[index];
	index = index + 1 % m_keys.size(); //Wrap around
	return key;
}

void TornApiServiceKeyManaged::removeCurrentKey()
{
	m_keys.erase(m_keys.begin() + index);
}

std::chrono::microseconds TornApiServiceKeyManaged::getCooldown()
{
	using namespace std::chrono;

	constexpr auto keyRefillCooldown = 60s; // cooldown window for each key
	constexpr int usesPerKey = 20;

	if (m_keys.empty())
	{
		return 1s;
	}

	// Calculate per-request cooldown
	microseconds cooldown = duration_cast<microseconds>(keyRefillCooldown) / (m_keys.size() * usesPerKey);

	// Ensure minimum cooldown of 1 second
	return std::max(cooldown, duration_cast<microseconds>(1s));
}
