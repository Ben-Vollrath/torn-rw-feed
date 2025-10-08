
#ifndef CRUD_USERSERVICE_HPP
#define CRUD_USERSERVICE_HPP

#include "../db/DbClient.hpp"
#include "../dto/UserDTO.hpp"

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/macro/component.hpp"

class UserService {
private:
	typedef oatpp::web::protocol::http::Status Status;
	OATPP_COMPONENT(std::shared_ptr<DbClient>, m_database);
public:

	oatpp::Object<UserDto> getUserById(const oatpp::Int32& id, const oatpp::provider::ResourceHandle<oatpp::orm::Connection>& connection = nullptr);

};

#endif //CRUD_USERSERVICE_HPP