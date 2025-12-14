#pragma once

#include "oatpp/web/server/handler/AuthorizationHandler.hpp"
#include "service/ApiKeyService.hpp"
#include "util/String.hpp"

class AuthObject : public oatpp::web::server::handler::AuthorizationObject
{
public:
	oatpp::Int64 userId;
	oatpp::Int64 factionId;
};

class AuthHandler : public oatpp::web::server::handler::BearerAuthorizationHandler
{
	ApiKeyService m_apiKeyService;
	UserService m_userService;
	using Status = oatpp::web::protocol::http::Status;

public:
	AuthHandler()
		: BearerAuthorizationHandler("Authorization")
	{
	}

	static bool ct_equal(const std::string& a, const std::string& b)
	{
		if (a.size() != b.size()) return false;
		unsigned char diff = 0;
		for (size_t i = 0; i < a.size(); ++i) diff |= static_cast<unsigned char>(a[i] ^ b[i]);
		return diff == 0;
	}

	std::shared_ptr<AuthorizationObject>
	authorize(const oatpp::String& token) override
	{
		OATPP_ASSERT_HTTP(token, Status::CODE_401, "401");

		// Token format: "ak_<prefix>_<secretB64>"
		std::vector<std::string> parts = split(token, '.');
		OATPP_ASSERT_HTTP(parts.size() == 3 && parts[0] == "ak",
		                  Status::CODE_401, "401");

		const std::string& prefix = parts[1];
		const std::string& secretB64 = parts[2];

		// Lookup by prefix
		auto key = m_apiKeyService.getByprefix(prefix);
		OATPP_ASSERT_HTTP(key, Status::CODE_401, "401");

		// State checks
		const auto now = std::time(nullptr);
		OATPP_ASSERT_HTTP(!(key->revoked && *key->revoked), Status::CODE_401, "401");
		if (key->expiresAt && *key->expiresAt > 0)
		{
			OATPP_ASSERT_HTTP(now <= *key->expiresAt, Status::CODE_401, "401");
		}

		// Decode secret and hash
		const std::string secretBytes = CryptoUtils::base64URLDecode(secretB64);
		const std::string presentedHashHex =
			CryptoUtils::sha256Hex(reinterpret_cast<const CryptoPP::byte*>(secretBytes.data()),
			                       secretBytes.size());

		// Normalize stored hex (strip Postgres bytea "\x" prefix if present)
		std::string storedHex = key->secretHash ? std::string(key->secretHash->c_str()) : "";
		if (storedHex.size() >= 2 && storedHex[0] == '\\' && storedHex[1] == 'x')
		{
			storedHex.erase(0, 2);
		}

		// Constant-time compare
		OATPP_ASSERT_HTTP(ct_equal(storedHex, presentedHashHex),
		                  Status::CODE_401, "401");


		m_apiKeyService.touchLastUsedAt(key->id);

		auto user = m_userService.getById(key->userId);

		// Build and return auth object
		auto auth = std::make_shared<AuthObject>();
		auth->userId = key->userId;
		auth->factionId = user->factionId;

		return auth;
	}
};
