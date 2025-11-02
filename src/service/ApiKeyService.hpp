#pragma once

#include "db/ApiKeyDb.hpp"
#include "dto/ApiKeyDto.hpp"
#include "CrudService.hpp"
#include <optional>
#include <ctime>
#include <chrono>

#include "util/Crypto.hpp"


class ApiKeyService : public CrudService<ApiKeyDb, ApiKeyDto>
{
public:
	using CrudService::create;
	using CrudService::update;
	using CrudService::getById;
	using CrudService::removeById;
	oatpp::Object<ApiKeyDto> getByprefix(const std::string& prefix) const;
	oatpp::Object<ApiKeyDto> touchLastUsedAt(std::int64_t id) const;

	struct IssueResult
	{
		oatpp::Int64 id;
		oatpp::String prefix;
		std::string fullKey;
		oatpp::Int64 expiresAt{nullptr};
	};


	IssueResult issueKey(const oatpp::Int64& userId,
	                     const std::optional<int64_t>& ttlSeconds = std::chrono::duration_cast<std::chrono::seconds>(
		                     std::chrono::hours(24)).count(),
	                     const oatpp::String& alg = "SHA256") const
	{
		auto prefixBytes = CryptoUtils::randomBytes(8);
		auto secretBytes = CryptoUtils::randomBytes(32);

		std::string prefixHex = CryptoUtils::hexEncode(prefixBytes.data(), prefixBytes.size());
		std::string secretB64 = CryptoUtils::base64URLEncode(secretBytes.data(), secretBytes.size());

		//hash secret (store only hash)
		std::string secretHashHex = CryptoUtils::sha256Hex(secretBytes.data(), secretBytes.size());
		// bytea literal as hex needs a leading \x
		std::string secretHashBytea = "\\x" + secretHashHex;

		// build DTO for insert
		auto now = std::time(nullptr);
		oatpp::Int64 expiresAt = nullptr;
		if (ttlSeconds.has_value())
		{
			expiresAt = now + *ttlSeconds;
		}

		auto dto = ApiKeyDto::createShared();
		dto->userId = userId;
		dto->prefix = prefixHex.c_str();
		dto->secretHash = secretHashBytea.c_str();
		dto->alg = alg;
		dto->expiresAt = expiresAt;
		dto->revoked = false;
		dto->createdAt = now;
		dto->lastUsedAt = now;

		auto created = this->create(dto);

		IssueResult result;
		result.id = created->id;
		result.prefix = created->prefix;
		result.expiresAt = created->expiresAt;

		// full key returned ONCE to caller
		result.fullKey = "ak." + std::string(result.prefix) + "." + secretB64;
		return result;
	}
};
