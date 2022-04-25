#!/bin/bash

set -e
export PGPASSWORD=$POSTGRES_PASSWORD;
psql -v ON_ERROR_STOP=1 --username "$POSTGRES_USER" --dbname "$POSTGRES_DB" <<-EOSQL
    CREATE ROLE web_anon NOLOGIN;
    CREATE ROLE authenticator NOINHERIT LOGIN PASSWORD '$AUTHENTICATOR_PASS';
    GRANT web_anon TO authenticator;
    CREATE SCHEMA IF NOT EXISTS api;
    CREATE TABLE api.gokarts (
        id SERIAL PRIMARY KEY,
        gokart VARCHAR(80) NOT NULL,
        time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        signal VARCHAR(80),
        value REAL,
        unit VARCHAR(80),
        power_state VARCHAR(80),
        gps VARCHAR(80)
    );
    GRANT usage ON SCHEMA api TO web_anon;
    GRANT SELECT ON api.gokarts TO web_anon;
EOSQL


