CREATE TABLE users
(
    id         BIGSERIAL PRIMARY KEY,
    torn_key   TEXT,
    faction_id BIGINT REFERENCES factions (id) ON DELETE SET NULL,
    created_at BIGINT NOT NULL DEFAULT (EXTRACT(EPOCH FROM now()))::BIGINT
);

CREATE INDEX idx_users_faction_id ON users (faction_id);
CREATE UNIQUE INDEX uq_users_torn_key ON users (torn_key);