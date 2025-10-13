CREATE TABLE war_member_information (
  id      BIGSERIAL PRIMARY KEY,
  war_id  BIGINT NOT NULL REFERENCES wars(id) ON DELETE CASCADE,
  name    TEXT NOT NULL,
  level   INT NOT NULL,
  str     BIGINT,
  def     BIGINT,
  spd     BIGINT,
  dex     BIGINT,
  total   BIGINT
);

CREATE INDEX idx_wmi_war_id ON war_member_information (war_id);
CREATE INDEX idx_wmi_name ON war_member_information (name);