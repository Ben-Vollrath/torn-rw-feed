#pragma once

#include <memory>

#include "oatpp-postgresql/orm.hpp"
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

template <typename DbT, typename DtoT>
class CrudService {
 protected:
  using Status = oatpp::web::protocol::http::Status;
  OATPP_COMPONENT(std::shared_ptr<DbT>, db_);  // injected Db client

  // Helper: fetch exactly one row or
  static void checkSuccess(const std::shared_ptr<oatpp::orm::QueryResult>& qr) {
    OATPP_ASSERT_HTTP(qr->isSuccess(), Status::CODE_500, qr->getErrorMessage());
  }

  // Helper: fetch exactly one row or
  static oatpp::Object<DtoT> fetchOne(
      const std::shared_ptr<oatpp::orm::QueryResult>& qr) {
    checkSuccess(qr);
    auto rows = qr->fetch<oatpp::Vector<oatpp::Object<DtoT>>>();
    OATPP_ASSERT_HTTP(rows && rows->size() == 1, Status::CODE_500,
                      "Unknown Error");
    return rows[0];
  }

  // Helper: fetch zero or one row (returns nullptr if no rows)
  static oatpp::Object<DtoT> fetchOneOrNone(
      const std::shared_ptr<oatpp::orm::QueryResult>& qr) {
    checkSuccess(qr);
    if (!qr->hasMoreToFetch()) {
      return nullptr;
    }
    auto rows = qr->fetch<oatpp::Vector<oatpp::Object<DtoT>>>();
    if (!rows || rows->empty()) {
      return nullptr;
    }
    OATPP_ASSERT_HTTP(rows->size() == 1, Status::CODE_500, "Unknown Error");
    return rows[0];
  }

 public:
  // CREATE
  oatpp::Object<DtoT> create(const oatpp::Object<DtoT>& dto) const {
    auto qr = db_->create(dto);
    return fetchOne(qr);
  }

  void createNoFetch(const oatpp::Object<DtoT>& dto) const {
    auto qr = db_->create(dto);
    checkSuccess(qr);
  }

  oatpp::Object<DtoT> upsertById(const oatpp::Object<DtoT>& dto) const {
    auto qr = db_->upsertById(dto);
    return fetchOne(qr);
  }

  // UPDATE
  oatpp::Object<DtoT> update(const oatpp::Object<DtoT>& dto) const {
    auto qr = db_->update(dto);
    return fetchOne(qr);
  }

  // READ (by id)
  oatpp::Object<DtoT> getById(const std::int64_t& id) const {
    auto qr = db_->getById(id);
    OATPP_ASSERT_HTTP(qr->isSuccess(), Status::CODE_500, qr->getErrorMessage());
    OATPP_ASSERT_HTTP(qr->hasMoreToFetch(), Status::CODE_404, "Not found");
    auto rows = qr->template fetch<oatpp::Vector<oatpp::Object<DtoT>>>();
    OATPP_ASSERT_HTTP(rows && rows->size() == 1, Status::CODE_500,
                      "Unknown Error");
    return rows[0];
  }

  // READ (by id) nullable
  oatpp::Object<DtoT> getByIdNullable(const std::int64_t& id) const {
    auto qr = db_->getById(id);
    return fetchOneOrNone(qr);
  }

  // DELETE (throws on error)
  void removeById(const std::int64_t& id) const {
    auto qr = db_->deleteById(id);
    OATPP_ASSERT_HTTP(qr->isSuccess(), Status::CODE_500, qr->getErrorMessage());
  }

  // DELETE (throws on error)
  void removeAll() const {
    auto qr = db_->deleteAll();
    OATPP_ASSERT_HTTP(qr->isSuccess(), Status::CODE_500, qr->getErrorMessage());
  }
};
