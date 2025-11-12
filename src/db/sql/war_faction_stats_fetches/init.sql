CREATE TABLE war_faction_stats_fetches (
	war_id BIGINT NOT NULL REFERENCES wars(id) ON DELETE CASCADE,
	faction_id BIGINT NOT NULL,
	PRIMARY KEY(war_id, faction_id)
);

