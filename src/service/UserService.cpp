#include "UserService.hpp"


std::vector<std::string> UserService::getTornKeys(const std::int64_t factionId)
{
	auto qr = db_->getTornKeys(factionId);
	OATPP_ASSERT_HTTP(qr->isSuccess(), Status::CODE_500, qr->getErrorMessage());

	auto rows = qr->fetch<oatpp::Vector<oatpp::Object<TornKeyRow>>>();
	std::vector<std::string> out;
	if (!rows) return out;
	out.reserve(rows->size());
	for (const auto& r : *rows)
	{
		if (r && r->torn_key) out.emplace_back(r->torn_key->c_str());
	}
	return out;
}

void UserService::removeTornKey(const std::string& tornKey)
{
	db_->removeTornKey(tornKey);
}
