# Server Datenbasis V1

## Speichertrennung

- SQLite: `devices`, `rooms`, `device_capabilities`, `device_config`, `device_last_state`, `automations`, `audit_log`, `weather_settings`
- InfluxDB: Sensor-Zeitreihen aus `state`

SQLite wird beim Containerstart ueber `db/schema.sql` initialisiert.

## SQLite-Regeln

- unbekannte Geraete aus `master/.../status`, `master/.../event`, `node/.../meta`, `node/.../status`, `node/.../state`, `node/.../event`, `node/.../ack` oder `node/.../cfg/report` werden sofort angelegt
- `display_name` startet immer mit `device_id` und wird erst durch echte Metadaten ersetzt
- `status` und `state` bleiben getrennt
- `event` und `ack` landen nicht im `state_json`
- retained `event` und retained `ack` werden nur auditiert und nicht in Registry oder `device_last_state` uebernommen
- `smarthome/server/status` landet als JSON plus Zeitstempel in `server_settings`
- `cfg/report` landet schluesselweise in `device_config`
- `audit_log` wird per Housekeeping-Flow auf 7 Tage begrenzt

## InfluxDB-Regeln

- Bucket: `smarthome`
- Retention: `365d`
- Measurement: `smarthome_sensor`
- Quelle: nur numerische Messwerte aus `node/.../state`, bevorzugt aus `state.sensors`
- reale Root-Felder wie `temp_01c`, `hum_01pct`, `battery_pct`, `battery_mv` oder `rain_raw` werden ebenfalls akzeptiert
- Tags: `node_id`, `device_type`, `room` (falls vorhanden), `metric`, `unit` (falls ableitbar)
- Feld: `value_num`
- boolesche, textuelle oder offensichtliche Aktor-/Statusfelder werden nicht nach Influx geschrieben
