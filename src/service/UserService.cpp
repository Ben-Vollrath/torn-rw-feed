#include "UserService.hpp"

oatpp::Vector<oatpp::Object<TornKeyRow>> UserService::getTornKeys(
    const std::int64_t factionId) {
  auto qr = db_->getTornKeys(factionId);
  OATPP_ASSERT_HTTP(qr->isSuccess(), Status::CODE_500, qr->getErrorMessage());

  return qr->fetch<oatpp::Vector<oatpp::Object<TornKeyRow>>>();
}

void UserService::updateFaction(const std::string& tornKey,
                                std::int64_t factionId) {
  auto qr = db_->updateFaction(tornKey, factionId);
  OATPP_ASSERT_HTTP(qr->isSuccess(), Status::CODE_500, qr->getErrorMessage());
}

void UserService::removeTornKey(const std::string& tornKey) {
  db_->removeTornKey(tornKey);
}
