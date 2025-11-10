CREATE TABLE wars (
  id                    BIGSERIAL PRIMARY KEY,
  faction_one_id            BIGINT NOT NULL REFERENCES factions(id) ON DELETE CASCADE,
  faction_two_id      BIGINT NOT NULL REFERENCES factions(id) ON DELETE CASCADE,
  start_at              BIGINT NOT NULL DEFAULT (EXTRACT(EPOCH FROM now()))::BIGINT,
  end_at                BIGINT,
  winner_id             BIGINT REFERENCES factions(id) ON DELETE SET NULL,
  faction_one_score         INT    NOT NULL DEFAULT 0,
  faction_one_chain         INT    NOT NULL DEFAULT 0,
  faction_two_score   INT    NOT NULL DEFAULT 0,
  faction_two_chain   INT    NOT NULL DEFAULT 0
);
