CREATE TABLE wars (
  id                  BIGSERIAL PRIMARY KEY,
  faction_one_id      BIGINT,
  faction_two_id      BIGINT,
  start_at              BIGINT NOT NULL DEFAULT (EXTRACT(EPOCH FROM now()))::BIGINT,
  end_at                BIGINT,
  winner_id             BIGINT REFERENCES factions(id) ON DELETE SET NULL,
  faction_one_score         INT    NOT NULL DEFAULT 0,
  faction_one_chain         INT    NOT NULL DEFAULT 0,
  faction_two_score   INT    NOT NULL DEFAULT 0,
  faction_two_chain   INT    NOT NULL DEFAULT 0
);
