# Server Runbook Lokalbetrieb

## 1. Zweck und Geltungsbereich

Dieses Runbook regelt den lokalen Betrieb des versionierten `server/`-Stacks in diesem Repo.
Es deckt Start, Neustart, pruefbare Grundkontrollen und den bewussten Umgang mit persistierten Laufzeitdaten ab.

Nicht dieses Dokument:
- Produktions-Runbook
- vollstaendige Admin-Doku
- Deployment-Doku fuer Fremdserver

Arbeitsverzeichnis:
- echtes Git-Repo unter `C:\Users\mries\Documents\Playground\smarthome-esp32`
- alle Compose-Befehle aus `C:\Users\mries\Documents\Playground\smarthome-esp32\server`

## 2. Voraussetzungen

Technische Basis:
- echtes Git-Repo, nicht OneDrive-Kopie
- Docker Desktop oder gleichwertige Docker-Engine mit `docker compose`
- lokale `server/.env` fuer reproduzierbare Host-Ports und lokale Secrets
- lokale, nicht versionierte Mosquitto-Auth-Dateien nur falls Passwort-Auth bewusst genutzt wird

Versioniert und damit offizielle Repo-Basis:
- `server/docker-compose.yml`
- `server/.env.example`
- `server/config/mosquitto/mosquitto.conf`
- `server/db/schema.sql`
- `server/nodered/build-flows.js`

Lokal und nicht zu committen:
- `server/.env`
- `server/config/.env`
- `server/config/mosquitto/config/`
- `server/config/mosquitto/config/10-auth.conf`
- `server/config/mosquitto/config/passwd`
- `server/docker-compose.override*.yml`
- `server/docker-compose.local*.yml`
- `server/nodered/_generated_v1_flows.tmp.json`
- Docker-Volumes wie `server_nodered_data`, `server_influxdb_data`, `server_mosquitto_data`

Empfohlener lokaler Start vor dem ersten Lauf:

```powershell
Copy-Item .env.example .env
```

Wenn lokale Broker-Auth genutzt wird, bleiben `10-auth.conf` und `passwd` ausschliesslich lokal.

## 3. Standard-Startpfad

Standardpfad fuer normalen lokalen Betrieb ohne bewussten Laufzeitreset:

```powershell
docker compose config
docker compose up --build -d
docker compose ps
```

Grundpruefung nach dem Start:

```powershell
docker compose exec nodered sh -lc "test -s /data/flows.json && test -s /data/sqlite/smarthome.sqlite"
```

Start gilt als erfolgreich, wenn:
- `docker compose config` ohne Fehler laeuft
- `docker compose ps` die Dienste `mosquitto`, `influxdb` und `nodered` ohne Restart-Schleife zeigt
- die Grundpruefung in `nodered` erfolgreich ist
- Node-RED auf dem konfigurierten Host-Port erreichbar ist

Wenn ein Dienst nicht sauber hochkommt:

```powershell
docker compose logs --tail 100 mosquitto influxdb nodered
```

Wichtig:
- Ein erfolgreiches `up --build -d` beweist nur, dass der Stack startet.
- Es beweist nicht, dass Node-RED bereits den aktuellen versionierten Flow-Stand aus dem Repo faehrt. Das ist ein eigenes Betriebsthema und kein Detailfehler.

## 4. Portbelegung und lokale Overrides

Repo-Defaults:
- MQTT: `0.0.0.0:1883 -> 1883`
- Node-RED: `0.0.0.0:1880 -> 1880`
- InfluxDB: `127.0.0.1:8086 -> 8086`

Bei lokalen Host-Port-Kollisionen:
- nur `server/.env` anpassen
- keine zweite offizielle Compose-Datei einfuehren
- keine neuen offiziellen Compose-Varianten bauen

Relevante `.env`-Schalter:
- `MQTT_BIND_ADDRESS`
- `MQTT_HOST_PORT`
- `NODERED_BIND_ADDRESS`
- `NODERED_HOST_PORT`
- `INFLUX_BIND_ADDRESS`
- `INFLUX_HOST_PORT`

Beispiel fuer isolierten lokalen Testlauf:

```dotenv
MQTT_BIND_ADDRESS=127.0.0.1
MQTT_HOST_PORT=18883
NODERED_BIND_ADDRESS=127.0.0.1
NODERED_HOST_PORT=18880
INFLUX_BIND_ADDRESS=127.0.0.1
INFLUX_HOST_PORT=18086
```

Wenn ein abweichender Compose-Projektname per `-p` genutzt wird, aendern sich auch die realen Docker-Volume-Namen. Das ist gewollt und muss im Testprotokoll benannt werden.

## 5. Node-RED-Laufzeitrealitaet

Node-RED arbeitet unter dem persistierten Pfad `/data`.
Im Repo-Default wird daraus beim Start aus `server/` ohne `-p` das Docker-Volume `server_nodered_data`.

Reale Betriebsfolge:
- ein vorhandenes `server_nodered_data` kann ein altes `/data/flows.json` weiterfahren
- dadurch kann `docker compose up --build -d` einen technisch "gesunden" Stack liefern, obwohl Node-RED nicht den aktuellen Generatorstand aus `server/nodered/build-flows.js` benutzt
- das ist kein Zufallsfall, sondern ein reproduzierbarer Betriebsfaktor

Ein einfaches `docker compose up --build -d` reicht nur, wenn:
- du bewusst denselben persistierten Node-RED-Laufzeitstand weiterverwenden willst
- keine Flow-Generator-Aenderung gegen den aktuellen Repo-Stand verifiziert werden soll

Ein bewusster Node-RED-Reset ist noetig, wenn:
- `server/nodered/build-flows.js` geaendert wurde
- ein Test explizit gegen den aktuellen versionierten Flow-Stand laufen soll
- ein Nachweis fachlich wertlos waere, wenn noch alte `/data/flows.json`-Inhalte aktiv sind

Bewusster Node-RED-Reset:

```powershell
docker compose down --remove-orphans
docker volume rm server_nodered_data
docker compose up --build -d
```

Bei abweichendem Compose-Projektnamen:
- statt `server_nodered_data` das reale Volume `<projektname>_nodered_data` entfernen

Vollreset des gesamten Stacks nur wenn wirklich gewollt:

```powershell
docker compose down -v --remove-orphans
docker compose up --build -d
```

Ehrliche Begriffe:
- normaler Neustart = Container neu starten, Persistenz bleibt
- bewusster Node-RED-Reset = nur Node-RED-Laufzeitvolumen entfernen
- Vollreset = alle Stack-Volumes entfernen

Wer einen normalen Neustart als "frischen Repo-Stand" verkauft, testet gegen eine bequeme Luege.

## 6. Sichere Pruefpfade nach Aenderungen

Vor jedem Test zuerst den Reset-Level festlegen:
- nur Container-Neustart
- Node-RED-Reset
- Vollreset

Flow-Generator-Aenderungen verifizieren:

```powershell
docker compose down --remove-orphans
docker volume rm server_nodered_data
docker compose up --build -d
docker compose exec nodered sh -lc "node /opt/smarthome/build-flows.js > /tmp/expected_flows.json && node -e \"const fs=require('fs'); process.exit(fs.readFileSync('/tmp/expected_flows.json','utf8')===fs.readFileSync('/data/flows.json','utf8') ? 0 : 1)\" && rm -f /tmp/expected_flows.json"
```

Erst danach funktional pruefen. Fuer den aktuell versionierten Rueckweg von `net_erl_01` existieren im Repo bereits diese Server-Trigger. Dabei den realen `NODERED_HOST_PORT` aus der lokalen `.env` einsetzen:

```powershell
Invoke-WebRequest -Method Post http://127.0.0.1:<NODERED_HOST_PORT>/inject/inject_net_erl_on
Invoke-WebRequest -Method Post http://127.0.0.1:<NODERED_HOST_PORT>/inject/inject_net_erl_off
```

Broker/MQTT pruefen:
- Basis: `docker compose ps`
- bei Bedarf: `docker compose logs --tail 100 mosquitto`
- funktional: passiv gegen den Broker pruefen, nicht blind mit Altclient-Resten

Beispiel fuer den Broker-Selbsttest im Container:

```powershell
docker compose exec mosquitto sh -lc 'if [ -n "$MQTT_USERNAME" ] && [ -n "$MQTT_PASSWORD" ]; then mosquitto_sub -h 127.0.0.1 -u "$MQTT_USERNAME" -P "$MQTT_PASSWORD" -t "\$SYS/broker/version" -C 1 -W 5 > /dev/null; else mosquitto_sub -h 127.0.0.1 -t "\$SYS/broker/version" -C 1 -W 5 > /dev/null; fi'
```

SQLite pruefen:

```powershell
docker compose exec nodered sh -lc "sqlite3 /data/sqlite/smarthome.sqlite '.tables'"
docker compose exec nodered sh -lc "sqlite3 /data/sqlite/smarthome.sqlite \"SELECT COUNT(*) FROM audit_log;\""
docker compose exec nodered sh -lc "sqlite3 /data/sqlite/smarthome.sqlite \"SELECT device_id, state_json FROM device_last_state;\""
```

Gegen Altzustaende testest du nicht, wenn du vor dem Lauf klar festhaeltst:
- welcher Reset-Level benutzt wurde
- ob `server_nodered_data` bewusst behalten oder entfernt wurde
- ob SQLite-Befunde auf alter Persistenz oder auf frischer Baseline beruhen
- ob ein Port-Override nur lokal war oder fachlich Teil des Nachweises wurde

## 7. Dokumentationspflicht

In `PROTOKOLL/` und bei repo-weitem Nachweiswert zusaetzlich in `docs/14_test_und_nachweisstand.md` gehoeren:
- reale Betriebsabweichungen, die die Aussagekraft eines Tests veraendern
- reproduzierbare Persistenz- oder Startprobleme
- bewusst verwendete Reset-Pfade, wenn sie fuer den Nachweis fachlich relevant waren
- isolierte Host-Port-Abweichungen, wenn sie die Testgueltigkeit oder Reichweite beeinflusst haben

Nicht jede lokale Bequemlichkeit ist projektweit relevant:
- ein normaler lokaler Port-Workaround gehoert primaer in die lokale `.env`
- eine reproduzierbare Betriebsgrenze wie stale `server_nodered_data` gehoert in die offizielle Doku, weil sie Testaussagen kippen kann

Historische PROTOKOLL-Dateien werden nicht rueckwirkend geglaettet.

## 8. Offene Grenzen

Bewusst nicht Teil dieses Runbooks:
- Produktionsbetrieb
- TLS- und ACL-Vollhaertung
- Backup- und Restore-Doku
- vollstaendige Deployment-Doku
- generische Admin-Prozesse fuer Fremdumgebungen
