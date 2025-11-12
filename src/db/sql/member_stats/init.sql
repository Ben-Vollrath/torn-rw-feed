CREATE TABLE member_stats (
	war_id BIGINT NOT NULL REFERENCES wars(id) ON DELETE CASCADE,
	faction_id BIGINT NOT NULL,
	member_id BIGINT NOT NULL,
	str BIGINT,
	def BIGINT,
	spd BIGINT,
	dex BIGINT,
	total BIGINT,
	type TEXT NOT NULL CHECK (type IN ('ffscouter','tornstatsspies')),
	PRIMARY KEY(war_id, faction_id, member_id, type)
);

CREATE INDEX idx_member_stats_war ON member_stats(war_id, faction_id);