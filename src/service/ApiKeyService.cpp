#include "ApiKeyService.hpp"
#include "dto/ApiKeyDto.hpp"


oatpp::Object<ApiKeyDto> ApiKeyService::getByprefix(const std::string& prefix) const
{
	auto qr = db_->getKeyByPrefix(prefix);
	return fetchOne(qr);
}

oatpp::Object<ApiKeyDto> ApiKeyService::touchLastUsedAt(const std::int64_t id) const
{
	auto qr = db_->touchLastUsedAt(id);
	return fetchOne(qr);
}
