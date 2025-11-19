

template <typename DtoOneT, typename DtoTwoT>
inline bool dtoFieldsEqual(DtoOneT DtoOne, DtoTwoT DtoTwo, std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper) {
	auto x = objectMapper->writeToString(DtoOne);
	auto y = objectMapper->writeToString(DtoTwo);
	return x == y;
}