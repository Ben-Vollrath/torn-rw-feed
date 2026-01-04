#include "FFScouterApiService.hpp"

inline std::string joinIds(
    const oatpp::Object<TornFactionMembersResponse>& memberInfo) {
  const auto& members = memberInfo->members;
  if (members->empty()) return {};

  std::vector<std::string> idStrings;
  size_t total = 0;
  for (const auto& m : *members) {
    idStrings.emplace_back(std::to_string(m->id));
    total += idStrings.back().size() + 1;  // +1 for commas
  }

  std::string out;
  out.reserve(total);
  out += idStrings[0];
  for (size_t i = 1; i < idStrings.size(); ++i) {
    out.push_back(',');
    out += idStrings[i];
  }
  return out;
}

oatpp::async::CoroutineStarterForResult<const FFScouterResponseDto&>
FFScouterApiService::getScout(
    const oatpp::Object<TornFactionMembersResponse>& memberInfo) {
  class GetScoutsCoroutine
      : public oatpp::async::CoroutineWithResult<GetScoutsCoroutine,
                                                 const FFScouterResponseDto&> {
    const FFScouterApiService* m_apiService;
    const std::shared_ptr<FFScouterApiClient> m_client;
    const std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_om;
    const oatpp::Object<TornFactionMembersResponse> m_memberInfo;

   public:
    GetScoutsCoroutine(
        const FFScouterApiService* apiService,
        const std::shared_ptr<FFScouterApiClient>& client,
        const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& om,
        const oatpp::Object<TornFactionMembersResponse>& memberInfo)
        : m_apiService(apiService),
          m_client(client),
          m_om(om),
          m_memberInfo(memberInfo) {}

   private:
    oatpp::async::Action act() override {
      auto key = m_apiService->appConfig->ffscouterApiKey;
      auto targets = joinIds(m_memberInfo);
      return this->m_client->getScouts(key, targets)
          .callbackTo(&GetScoutsCoroutine::parseResponse);
    }

    oatpp::async::Action parseResponse(
        const std::shared_ptr<oatpp::web::protocol::http::incoming::Response>&
            rsp) {
      return _return(rsp->readBodyToDto<FFScouterResponseDto>(m_om));
    }
  };

  return GetScoutsCoroutine::startForResult(this, ffScouterApiClient,
                                            objectMapper, memberInfo);
}
