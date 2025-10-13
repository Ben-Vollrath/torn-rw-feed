CREATE TABLE wars (
  id                    BIGSERIAL PRIMARY KEY,
  faction_id            BIGINT NOT NULL REFERENCES factions(id) ON DELETE CASCADE,
  enemy_faction_id      BIGINT NOT NULL REFERENCES factions(id) ON DELETE CASCADE,
  start_at              BIGINT NOT NULL DEFAULT (EXTRACT(EPOCH FROM now()))::BIGINT,
  end_at                BIGINT,
  winner_id             BIGINT REFERENCES factions(id) ON DELETE SET NULL,
  faction_score         INT    NOT NULL DEFAULT 0,
  faction_chain         INT    NOT NULL DEFAULT 0,
  enemy_faction_score   INT    NOT NULL DEFAULT 0,
  enemy_faction_chain   INT    NOT NULL DEFAULT 0
);

CREATE INDEX idx_wars_factions ON wars (faction_id, enemy_faction_id);