CREATE TABLE factions
(
    id                BIGSERIAL PRIMARY KEY,
    name              TEXT NOT NULL UNIQUE,
    leader_torn_id    BIGINT,
    co_leader_torn_id BIGINT
);