CREATE TABLE api_keys
(
    id           BIGSERIAL PRIMARY KEY,
    user_id      BIGINT  NOT NULL REFERENCES users (id) ON DELETE CASCADE,
    prefix       TEXT    NOT NULL UNIQUE,
    secret_hash  BYTEA   NOT NULL,
    alg          TEXT    NOT NULL DEFAULT 'SHA256',
    created_at   BIGINT  NOT NULL DEFAULT (EXTRACT(EPOCH FROM now()))::BIGINT,
    expires_at   BIGINT,
    revoked      BOOLEAN NOT NULL DEFAULT FALSE,
    last_used_at BIGINT
);

CREATE INDEX IF NOT EXISTS idx_api_keys_user_id ON api_keys (user_id);
CREATE UNIQUE INDEX IF NOT EXISTS uq_api_keys_prefix ON api_keys (prefix);