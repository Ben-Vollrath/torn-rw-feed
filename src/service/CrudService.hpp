#pragma once

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/Types.hpp"

#include <memory>

template <typename DbT, typename DtoT>
class CrudService
{
protected:
	using Status = oatpp::web::protocol::http::Status;
	OATPP_COMPONENT(std::shared_ptr<DbT>, db_); // injected Db client

	// Helper: fetch exactly one row or throw
	static oatpp::Object<DtoT> fetchOne(const std::shared_ptr<oatpp::orm::QueryResult>& qr)
	{
		OATPP_ASSERT_HTTP(qr->isSuccess(), Status::CODE_500, qr->getErrorMessage());
		auto rows = qr->fetch<oatpp::Vector<oatpp::Object<DtoT>>>();
		OATPP_ASSERT_HTTP(rows && rows->size() == 1, Status::CODE_500, "Unknown Error");
		return rows[0];
	}

public:
	// CREATE
	oatpp::Object<DtoT> create(const oatpp::Object<DtoT>& dto) const
	{
		auto qr = db_->create(dto);
		return fetchOne(qr);
	}

	oatpp::Object<DtoT> upsertById(const oatpp::Object<DtoT>& dto) const
	{
		auto qr = db_->upsertById(dto);
		return fetchOne(qr);
	}

	// UPDATE
	oatpp::Object<DtoT> update(const oatpp::Object<DtoT>& dto) const
	{
		auto qr = db_->update(dto);
		return fetchOne(qr);
	}

	// READ (by id)
	oatpp::Object<DtoT> getById(const std::int64_t& id) const
	{
		auto qr = db_->getById(id);
		OATPP_ASSERT_HTTP(qr->isSuccess(), Status::CODE_500, qr->getErrorMessage());
		OATPP_ASSERT_HTTP(qr->hasMoreToFetch(), Status::CODE_404, "Not found");
		auto rows = qr->template fetch<oatpp::Vector<oatpp::Object<DtoT>>>();
		OATPP_ASSERT_HTTP(rows && rows->size() == 1, Status::CODE_500, "Unknown Error");
		return rows[0];
	}


	// READ (by id) nullable
	oatpp::Object<DtoT> getByIdNullable(const std::int64_t& id) const
	{
		auto qr = db_->getById(id);
		OATPP_ASSERT_HTTP(qr->isSuccess(), Status::CODE_500, qr->getErrorMessage());
		if (!qr->hasMoreToFetch())
		{
			return nullptr;
		}
		auto rows = qr->template fetch<oatpp::Vector<oatpp::Object<DtoT>>>();
		if (!rows || rows->empty())
		{
			return nullptr;
		}
		return rows[0];
	}

	// DELETE (throws on error)
	void removeById(const std::int64_t& id) const
	{
		auto qr = db_->deleteById(id);
		OATPP_ASSERT_HTTP(qr->isSuccess(), Status::CODE_500, qr->getErrorMessage());
	}
};
