CREATE TABLE targets
(
	enemy_faction_id BIGINT NOT NULL,
	user_id BIGINT NOT NULL REFERENCES users (id) ON DELETE CASCADE,
	targets TEXT NOT NULL,
	PRIMARY KEY (enemy_faction_id, user_id)
);

CREATE UNIQUE INDEX idx_user_faction ON targets(enemy_faction_id, user_id);