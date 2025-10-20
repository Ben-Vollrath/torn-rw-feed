#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include "service/ApiKeyService.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class ApiKeyIssueResponseDto : public oatpp::DTO {
	DTO_INIT(ApiKeyIssueResponseDto, DTO)

		DTO_FIELD(oatpp::Int64, id);
	DTO_FIELD(oatpp::String, prefix);

	// Show the full API key ONCE (never store this)
	DTO_FIELD(oatpp::String, apiKey, "api_key");

	DTO_FIELD(oatpp::Int64, expiresAt, "expires_at");

public:
	static oatpp::Object<ApiKeyIssueResponseDto>
		fromIssueResult(const ApiKeyService::IssueResult& res) {
		auto dto = createShared();
		dto->id = res.id;
		dto->prefix = res.prefix;
		dto->apiKey = res.fullKey.c_str();
		dto->expiresAt = res.expiresAt;
		return dto;
	}
};

#include OATPP_CODEGEN_BEGIN(DTO)