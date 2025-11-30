#include "TornApiServiceKeyManaged.hpp"

#include <cassert>


std::string TornApiServiceKeyManaged::getKey()
{
	assert(!m_keys->empty()); // Should never happen

	const std::string& key = m_keys[index]->torn_key;
	index = (index + 1) % m_keys->size(); //Wrap around
	return key;
}

oatpp::Object<TornKeyRow> TornApiServiceKeyManaged::getLastKey()
{
	int lastIndex = (index - 1 + m_keys->size()) % m_keys->size();
	return m_keys[lastIndex];
}

void TornApiServiceKeyManaged::removeLastKey()
{
	int lastIndex = (index - 1 + m_keys->size()) % m_keys->size();
	auto key = m_keys[lastIndex];
	m_keys->erase(m_keys->begin() + lastIndex);
	index = 0;
}

void TornApiServiceKeyManaged::removeLastKeyWithDb()
{
	int lastIndex = (index - 1 + m_keys->size()) % m_keys->size();
	auto key = m_keys[lastIndex];
	m_UserService.removeTornKey(key->torn_key);
	m_keys->erase(m_keys->begin() + lastIndex);
	index = 0;
}

std::chrono::microseconds TornApiServiceKeyManaged::getCooldown()
{
	using namespace std::chrono;

	constexpr auto keyRefillCooldown = 60s; // cooldown window for each key
	constexpr int usesPerKey = 10;

	if (m_keys->empty())
	{
		return 1s;
	}

	// Calculate per-request cooldown
	microseconds cooldown = duration_cast<microseconds>(keyRefillCooldown) / (m_keys->size() * usesPerKey);

	// Ensure minimum cooldown of 1 second
	return std::max(cooldown, duration_cast<microseconds>(1s));
}
