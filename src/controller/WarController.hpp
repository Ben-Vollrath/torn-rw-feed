#pragma once

#include "AuthHandler.hpp"
#include "dto/responses/SpyResponseDto.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp-websocket/AsyncConnectionHandler.hpp"
#include "oatpp-websocket/Handshaker.hpp"
#include "service/MemberStatsService.hpp"
#include "service/TornStatsApiService.hpp"
#include "war/Room.hpp"
#include "war/Lobby.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class WarController : public oatpp::web::server::api::ApiController
{
	using Action = oatpp::async::Action;
	using __ControllerType = WarController;
	OATPP_COMPONENT(std::shared_ptr<oatpp::websocket::AsyncConnectionHandler>, websocketConnectionHandler, "websocket");

	TornApiService m_tornApiService;
	TornStatsApiService m_tornStatsService;
	MemberStatsService m_memberStatsService;

	std::shared_ptr<AuthHandler> m_authHandler = std::make_shared<AuthHandler>();

public:
	WarController(const std::shared_ptr<ObjectMapper>& objectMapper)
		: ApiController(objectMapper)
	{
	}

	static std::shared_ptr<WarController> createShared(
		OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)
	)
	{
		return std::make_shared<WarController>(objectMapper);
	}

	const std::shared_ptr<AuthHandler>& authHandler() const { return m_authHandler; }

	ENDPOINT_INFO(WS)
	{
		info->summary = "Connects to the War WebSocket";
		info->description = "This endpoint upgrades the connection to a WebSocket. "
			"The client must provide a valid `Authorization` header (Bearer token) "
			"to authenticate before the upgrade.";

		info->addResponse<String>(Status::CODE_101, "text/plain", "WebSocket upgrade");

		auto& authHeader = info->headers.add<oatpp::String>(oatpp::web::protocol::http::Header::AUTHORIZATION);
		authHeader.description = "Bearer token for authentication";
		authHeader.required = true;

		info->addTag("War");
	}

	ENDPOINT_ASYNC("GET", "/wars/socket", WS)
	{
		const std::string TAG = "WARSOCKET";

		ENDPOINT_ASYNC_INIT(WS)

		Action act() override
		{
			const oatpp::String authHeader = request->getHeader(oatpp::web::protocol::http::Header::AUTHORIZATION);

			auto baseObj = controller->authHandler()->authorize(authHeader);
			auto authObj = std::dynamic_pointer_cast<AuthObject>(baseObj);

			/* Websocket handshake */
			auto response = oatpp::websocket::Handshaker::serversideHandshake(request->getHeaders(),
			                                                                  controller->websocketConnectionHandler);

			auto parameters = std::make_shared<oatpp::network::ConnectionHandler::ParameterMap>();

			(*parameters)["faction_id"] = std::to_string(authObj->factionId);

			/* Set connection upgrade params */
			response->setConnectionUpgradeParameters(parameters);

			return _return(response);
		}

		Action handleError(Error* e) override
		{
			OATPP_LOGE(TAG, " Error: %s", e ? e->what() : "unknown");
			return _return(controller->createResponse(Status::CODE_500, "Internal Server Error"));
		}
	};

	ENDPOINT_INFO(addWarSpy)
	{
		info->summary = "Add War Spy Information";
		info->queryParams.add<oatpp::String>("torn_stats_key");

		auto& authHeader = info->headers.add<oatpp::String>(oatpp::web::protocol::http::Header::AUTHORIZATION);
		authHeader.description = "Bearer token for authentication";
		authHeader.required = true;

		info->addResponse<Object<SpyResponseDto>>(Status::CODE_200, "application/json");

		info->addTag("War");
	}

	ENDPOINT_ASYNC("POST", "/wars/spy", addWarSpy)
	{
		const std::string TAG = "ADDWARSPY";

		std::int64_t m_factionId;
		std::string m_tornStatsKey;

		std::int64_t m_warId;
		std::int64_t m_enemyFactionId;
		std::shared_ptr<Room> m_room;

		ENDPOINT_ASYNC_INIT(addWarSpy)

		Action act() override
		{
			const oatpp::String authHeader = request->getHeader(oatpp::web::protocol::http::Header::AUTHORIZATION);
			auto baseObj = controller->authHandler()->authorize(authHeader);
			auto authObj = std::dynamic_pointer_cast<AuthObject>(baseObj);
			m_factionId = authObj->factionId;

			m_tornStatsKey = request->getQueryParameter("torn_stats_key");

			OATPP_COMPONENT(std::shared_ptr<Lobby>, lobby);
			auto room = lobby->getRoomNullable(authObj->factionId);

			if (room && room.value()->getEnemyFactionId())
			{
				m_room = room.value();
				m_enemyFactionId = room.value()->getEnemyFactionId().value();
				m_warId = room.value()->getWarId().value();

				return sendSpyRequest();
			}

			return controller->m_tornApiService.getFactionWar(m_tornStatsKey).callbackTo(&addWarSpy::parseFactionWar);
		}

		Action parseFactionWar(const oatpp::Object<TornFactionWarResponseDto>& factionWar)
		{
			if (factionWar->getWarId() && factionWar->getEnemyFactionId(m_factionId))
			{
				m_warId = factionWar->getWarId().value();
				m_enemyFactionId = factionWar->getEnemyFactionId(m_factionId).value();
				return sendSpyRequest();
			}
			return _return(controller->createResponse(Status::CODE_400, "No war currently running"));
		}

		Action sendSpyRequest()
		{
			return controller->m_tornStatsService.getSpies(
				                 m_tornStatsKey, std::to_string(m_enemyFactionId))
			                 .callbackTo(&addWarSpy::parseTornStatsSpy);
		}

		Action parseTornStatsSpy(const oatpp::Object<TornStatsSpyResponseDto>& spyResponse)
		{
			auto dto = MemberStatsDto::fromTornStatsSpyResponse(m_warId, m_enemyFactionId, spyResponse);
			auto stats = controller->m_memberStatsService.createMany(dto);

			if (m_room)
			{
				m_room->updateStats(stats);
			}

			return _return(controller->createDtoResponse(Status::CODE_200, SpyResponseDto::fromSize(stats->size())));
		}

		Action handleError(Error* e) override
		{
			OATPP_LOGE(TAG, " Error: %s", e ? e->what() : "unknown");
			return _return(controller->createResponse(Status::CODE_500, "Internal Server Error"));
		}
	};
};

#include OATPP_CODEGEN_END(ApiController)
