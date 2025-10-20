CREATE TABLE purchases (
  id             BIGSERIAL PRIMARY KEY,
  faction_id     BIGINT NOT NULL REFERENCES factions(id) ON DELETE CASCADE,
  purchaser_id   BIGINT REFERENCES users(id) ON DELETE SET NULL,
  amount         BIGINT NOT NULL,
  full_log       TEXT,
  start_date     BIGINT NOT NULL DEFAULT (EXTRACT(EPOCH FROM now()))::BIGINT,
  end_date       BIGINT NOT NULL
);

CREATE INDEX idx_purchases_faction_id ON purchases (faction_id);
CREATE INDEX idx_purchases_purchaser_id ON purchases (purchaser_id);
CREATE INDEX idx_purchases_start_end ON purchases (start_date, end_date);