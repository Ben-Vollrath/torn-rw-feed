#include "Room.hpp"

#include "dto/responses/WarStateResponseDto.hpp"
#include "util/DtoUtils.hpp"

void Room::saveTargets()
{
    std::lock_guard<std::mutex> guard(m_userTargetsLock);
    for (auto& pair : m_userTargets)
    {
        auto dbDto = pair.second->getDbDto();
        m_targetService.create(dbDto);
    }
}

void Room::updateTarget(std::int64_t userId, const std::string& updateString)
{
	oatpp::Object<UpdateTargetDto> target;
	try {
		target = objectMapper->readFromString<oatpp::Object<UpdateTargetDto>>(updateString);
		if (!target)
		{
			OATPP_LOGD(TAG, "Websocket sent non target update message");
			return;
		}
	} catch (oatpp::parser::ParsingError e)
	{
		OATPP_LOGD(TAG, "Websocket sent non target update message");
		return;
	}


	std::lock_guard<std::mutex> guard(m_userTargetsLock);
	oatpp::Object<TargetsDto> targets = m_userTargets[userId];

	if (target->updateType == TargetUpdateType::ADD)
	{
		targets->targets_set->insert(target->targetId);
	}
	else //TargetUpdateType::REMOVE
	{
		targets->targets_set->erase(target->targetId);
	}

	auto updates = oatpp::Vector<oatpp::Object<UpdateTargetDto>>::createShared();
	updates->push_back(target);
	auto dto = WarStateResponseDto::fromTargets(updates);
	oatpp::String targetUpdate = objectMapper->writeToString(dto);
	sendMessage(targetUpdate, userId);
}

oatpp::Vector<oatpp::Object<UpdateTargetDto>> Room::loadTargetsForUser(std::int64_t userId)
{
	if (!getEnemyFactionId())
	{
		return oatpp::Vector<oatpp::Object<UpdateTargetDto>>::createShared();
	}

	auto it = m_userTargets.find(userId);
	if (it == m_userTargets.end())
	{
		auto targets = m_targetService.getAllForUser(getEnemyFactionId().value(), userId);
		m_userTargets[userId] = targets;
		return UpdateTargetDto::fromTargetsDto(targets);
	}

	return UpdateTargetDto::fromTargetsDto(it->second);
}

bool Room::isClosed() const
{
	return m_closed.load(std::memory_order_acquire);
}

std::int64_t Room::factionId() const
{
	return m_factionId;
}

std::optional<std::int64_t> Room::getWarId()
{
	return m_factionWar->getWarId();
}

std::optional<std::int64_t> Room::getEnemyFactionId()
{
	return m_factionWar->getEnemyFactionId(m_factionId);
}

void Room::addPeer(const std::shared_ptr<Peer>& peer)
{
	std::lock_guard<std::mutex> guard(m_peerByIdLock);
	m_peerById[peer->getPeerId()] = peer;
	m_peersByUserId[peer->getUserId()][peer->getPeerId()] = peer;
	sendCurrentState(peer);
}

void Room::removePeerByPeerId(v_int32 peerId)
{
	std::lock_guard<std::mutex> guard(m_peerByIdLock);
	auto it = m_peerById.find(peerId);
	if (it != m_peerById.end())
	{
		auto userId = it->second->getUserId();
		m_peerById.erase(it);
		// remove from user index
		auto uit = m_peersByUserId.find(userId);
		if (uit != m_peersByUserId.end())
		{
			uit->second.erase(peerId);
			if (uit->second.empty())
			{
				m_peersByUserId.erase(uit);
			}
		}
	}
	if (m_peerById.empty())
	{
		OATPP_LOGD(TAG, "All peers disconnected, closing room: %s", std::to_string(m_factionId).c_str());
		m_closed.store(true, std::memory_order_release);
	}
}

void Room::sendMessage(const oatpp::String& message)
{
	std::lock_guard<std::mutex> guard(m_peerByIdLock);
	for (auto& pair : m_peerById)
	{
		pair.second->sendMessage(message);
	}
}

void Room::sendMessage(const oatpp::String& message, std::int64_t userId)
{
	{
		std::lock_guard<std::mutex> guard(m_peerByIdLock);
		auto it = m_peersByUserId.find(userId);
		if (it == m_peersByUserId.end())
		{
			return;
		}
		for (auto pIt : it->second)
		{
			auto peer = pIt.second;
			peer->sendMessage(message);
		}
	}
}

void Room::sendCurrentState(const std::shared_ptr<Peer>& peer)
{
	if (m_enemiesState.empty()) { return; }

	auto rsp = WarStateResponseDto::fromMembersInfo(m_enemiesState);
	rsp->addMemberStats(m_memberStats);
	rsp->addWar(m_factionWar);
	rsp->addUser(m_alliesState[peer->getUserId()]);
	rsp->addTargets(loadTargetsForUser(peer->getUserId()));
	oatpp::String currentStateJson = objectMapper->writeToString(rsp);

	peer->sendMessage(currentStateJson);
}

void Room::updateEnemies(const oatpp::Object<TornFactionMembersResponse>& memberInfos)
{
	const auto& members = memberInfos->members;
	auto updates = oatpp::Vector<oatpp::Object<TornFactionMember>>::createShared();
	for (const oatpp::Object<TornFactionMember>& member : *members)
	{
		auto it = m_enemiesState.find(member->id);

		if (it == m_enemiesState.end())
		{
			m_enemiesState[member->id] = member;
			updates->push_back(member);
		}
		else
		{
			const auto& old = it->second;
			if (old->last_action->timestamp != member->last_action->timestamp ||
				old->status->state != member->status->state ||
				old->status->description != member->status->description ||
				old->status->until != member->status->until ||
				old->last_action->status != member->last_action->status)
			{
				it->second = member;
				updates->push_back(member);
			}
		}
	}

	if (!updates->empty())
	{
		oatpp::String updateJson = objectMapper->writeToString(WarStateResponseDto::fromMembersInfo(updates));
		sendMessage(updateJson->c_str());
	}
}

void Room::updateStats(const oatpp::Vector<oatpp::Object<MemberStatsDto>>& stats)
{
	for (const oatpp::Object<MemberStatsDto>& stat : *stats)
	{
		m_memberStats[stat->member_id][stat->type] = stat;
	}
	auto out = WarStateResponseDto::fromMembersStats(stats);
	oatpp::String updateJson = objectMapper->writeToString(out);
	sendMessage(updateJson->c_str());
}

void Room::updateWarAndAllies(const oatpp::Object<TornFactionWarAndMembersResponseDto>& warAndAllies)
{
    // Update in-memory state first
	bool isNewWar = !m_factionWar || m_factionWar->getWarId() != warAndAllies->wars->getWarId();
	bool warHasUpdates = !dtoFieldsEqual(m_factionWar, warAndAllies->wars, objectMapper);
    m_factionWar = warAndAllies->wars;
    const auto& alliesMembers = warAndAllies->members;
	std::unordered_set<std::int64_t> updatedAllies;
    for (const oatpp::Object<TornFactionMember>& member : *alliesMembers) {
		auto it = m_alliesState.find(member->id);

		if (it == m_alliesState.end())
		{
			m_alliesState[member->id] = member;
			updatedAllies.insert(member->id);
		}
		else
		{
			const auto& old = it->second;
			if (old->status->state != member->status->state ||
				old->status->description != member->status->description ||
				old->status->until != member->status->until ||
				old->last_action->status != member->last_action->status)
			{
				it->second = member;
				updatedAllies.insert(member->id);
			}
		}
    }

	if (!warHasUpdates && updatedAllies.empty())
	{
		return;
	}

	std::lock_guard<std::mutex> guard(m_peerByIdLock);
    for (auto& pair : m_peerById) {
        const auto userId = pair.second->getUserId();
        auto rsp = WarStateResponseDto::createShared();

		if (warHasUpdates) {
			rsp->addWar(warAndAllies->wars);
		}

		if (isNewWar)
		{
			rsp->addTargets(loadTargetsForUser(userId));
		}

        // Attach the ally info for the specific user/peer
        auto it = updatedAllies.find(userId);
        if (it != updatedAllies.end()) {
            rsp->addUser(m_alliesState[userId]);
			rsp->user->status->parseLocation();
        }

        // Serialize once per peer with its specific user 
        oatpp::String json = objectMapper->writeToString(rsp);
		pair.second->sendMessage(json);
    }
}

void Room::updateWar(const oatpp::Object<TornFactionWarsDto>& factionWarResponses)
{
	bool isNewData = !dtoFieldsEqual(m_factionWar, factionWarResponses, objectMapper);

	if (isNewData)
	{
		m_factionWar = factionWarResponses;
		auto out = WarStateResponseDto::fromWar(factionWarResponses);
		oatpp::String updateJson = objectMapper->writeToString(out);
		sendMessage(updateJson->c_str());
	}
}

bool Room::needStats()
{
	return m_memberStats.empty();
}


void Room::resetState()
{
	m_enemiesState.clear();
	m_alliesState.clear();
	m_memberStats.clear();
	m_factionWar = nullptr;
}
