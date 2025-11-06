#pragma once
#include "service/ApiKeyService.hpp"
#include "service/FactionService.hpp"
#include "service/UserService.hpp"

#include <unordered_set>

class TestingFixtures
{
	UserService m_userService;
	FactionService m_factionService;
	ApiKeyService m_apiKeyService;

	std::int64_t idx = 0;

	std::unordered_set<std::int64_t> m_userIds;
	std::unordered_set<std::int64_t> m_factionIds;

public:
	~TestingFixtures()
	{
		reset();
	}

	void reset()
	{
		idx = 0;
		for (const auto userId : m_userIds)
		{
			try { m_userService.removeById(userId); }
			catch (...)
			{
			}
		}
		for (const auto factionId : m_factionIds)
		{
			try { m_factionService.removeById(factionId); }
			catch (...)
			{
			}
		}
	}

	oatpp::Object<FactionDto> createTestFaction(std::int64_t factionId)
	{
		if (m_factionIds.find(factionId) != m_factionIds.end())
		{
			return m_factionService.getById(factionId);
		}
		auto dto = FactionDto::createShared();
		dto->id = factionId;
		dto->name = std::to_string(idx);
		auto newFaction = m_factionService.upsertById(dto);
		m_factionIds.insert(newFaction->id);
		return newFaction;
	}

	oatpp::Object<UserDto> createTestUser(std::optional<std::int64_t> factionId = std::nullopt)
	{
		if (factionId)
		{
			createTestFaction(*factionId);
		}

		auto dto = UserDto::createShared();
		dto->id = idx;
		dto->tornKey = std::to_string(idx);
		if (factionId) dto->factionId = *factionId;

		++idx;

		auto newUser = m_userService.upsertById(dto);
		m_userIds.insert(newUser->id);
		return newUser;
	}

	ApiKeyService::IssueResult getUserApiKey(std::int64_t userId)
	{
		return m_apiKeyService.issueKey(userId);
	}

	oatpp::web::protocol::http::Headers getUserAuthHeader(std::int64_t userId)
	{
		auto issueResult = getUserApiKey(userId);
		oatpp::web::protocol::http::Headers headers;
		headers.put(oatpp::web::protocol::http::Header::AUTHORIZATION, oatpp::String(issueResult.fullKey));
		return headers;
	}
};
