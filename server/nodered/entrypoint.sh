#!/bin/sh
set -eu

# Ensure the SQLite schema exists before Node-RED starts.
SQLITE_TARGET="${SQLITE_PATH:-/data/sqlite/smarthome.sqlite}"
SQLITE_DIR="$(dirname "$SQLITE_TARGET")"

mkdir -p "$SQLITE_DIR"
sqlite3 "$SQLITE_TARGET" < /opt/smarthome/schema.sql

exec /usr/src/node-red/entrypoint.sh "$@"
