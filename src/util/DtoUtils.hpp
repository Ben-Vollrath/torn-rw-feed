#pragma once

template <typename DtoOneT, typename DtoTwoT>
inline bool dtoFieldsEqual(DtoOneT DtoOne, DtoTwoT DtoTwo, std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper) {
	auto x = objectMapper->writeToString(DtoOne);
	auto y = objectMapper->writeToString(DtoTwo);
	return x == y;
}

template <typename DtoOneT, typename DtoTwoT>
inline bool dtoFieldsEqualLogger(DtoOneT DtoOne, DtoTwoT DtoTwo, std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper) {
	auto x = objectMapper->writeToString(DtoOne);
	auto y = objectMapper->writeToString(DtoTwo);
	if (x != y) {
		OATPP_LOGE("ASSERTION", "%s \n doesnt equal \n %s", x->c_str(), y->c_str())
			return false;
	}
	return true;
}