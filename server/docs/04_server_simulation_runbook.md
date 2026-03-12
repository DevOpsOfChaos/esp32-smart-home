# Server-Simulations-Runbook

## Zweck

Dieses Runbook beschreibt den kleinsten test-only Lauf fuer den Fake-Master-Harness aus `server/tools/sim_master_contract_runner.py`.

Der Lauf validiert nur die serverseitige MQTT-Grenze oberhalb des Masters:

- serverseitige HELLO-Folge als Registrierung ueber `master/status` plus `node/meta|status|state`
- `meta`-, `status`- und `state`-Ingest
- Offline / Recovery ueber `node/status`
- SQLite- und Influx-Schreibpfad

Nicht validiert werden:

- echtes ESP-NOW-`HELLO`
- `HELLO_ACK` auf Funkebene
- echte Hardwarewirkung
- reale Sensorplausibilitaet
- echter Master-Timeout an der Funkkante
- `cmd/set`-Egress

## Harte Regeln

- Nur isoliert laufen lassen.
- Compose-Projekt muss `smarthome_sim` heissen oder damit beginnen.
- Ports bleiben getrennt vom normalen lokalen Stack.
- Nur `sim_*`-IDs verwenden.
- Nie reale Pilot-IDs wie `net_erl_01`, `net_zrl_01` oder `net_sen_01` fuer diesen Lauf missbrauchen.

## Versionierte Simulations-Defaults

Die versionierte Beispielumgebung liegt in `server/.env.sim.example`.

Sie erzwingt fuer den Simulationsstack:

- Broker auf `127.0.0.1:18883`
- Node-RED auf `127.0.0.1:18880`
- InfluxDB auf `127.0.0.1:18086`

Wenn du lokal andere Testports brauchst:

1. `Copy-Item .env.sim.example .env.sim.local`
2. Werte nur in `.env.sim.local` anpassen
3. alle folgenden Befehle mit `--env-file .env.sim.local` fahren

## Start des isolierten Stacks

Im Ordner `server/`:

```powershell
docker compose --project-name smarthome_sim --env-file .env.sim.example down -v
docker compose --project-name smarthome_sim --env-file .env.sim.example up --build -d
docker compose --project-name smarthome_sim --env-file .env.sim.example ps
```

Der `down -v` davor ist Absicht.
Wenn du alte Volumes stehen laesst, vermischst du Altlasten mit neuem Simulationslauf und faelschst dir die Aussage.

## Harness ausfuehren

Im Ordner `server/`:

```powershell
python .\tools\sim_master_contract_runner.py --env-file .\.env.sim.example --compose-project smarthome_sim
```

Optional mit Markdown-Report:

```powershell
python .\tools\sim_master_contract_runner.py --env-file .\.env.sim.example --compose-project smarthome_sim --report-file ..\local\server_sim\latest.md
```

Der Runner:

- publiziert nur ueber einen Fake-Master mit `sim_master_01`
- benutzt nur `sim_net_erl_01`, `sim_net_zrl_01`, `sim_net_sen_01`, `sim_bat_sen_01`
- raeumt retained Simulations-Topics vor und nach dem Lauf wieder leer
- prueft SQLite und Influx aktiv gegen den aktuellen Serverzustand

## Was der Lauf konkret prueft

### Registrierung / serverseitige HELLO-Folge

- `smarthome/master/sim_master_01/status`
- `smarthome/node/sim_*/meta`
- `smarthome/node/sim_*/status`
- `smarthome/node/sim_*/state`

Erwartung:

- `devices` enthaelt Master und alle vier Simulationsnodes
- `display_name` startet mit `[SIM]`
- `origin_master_id = sim_master_01`
- `device_capabilities` wird aus `meta` aufgebaut

### Offline / Recovery

Nur fuer `sim_net_sen_01`:

- zuerst `status online:true`
- dann `status online:false`
- danach `status online:true` plus neuer `state`

Erwartung:

- `status_json.online` kippt auf `false`
- alter `state_json` bleibt waehrend offline erhalten
- Recovery schreibt `online:true` und einen neuen `state`

### Influx

Erwartung:

- `sim_net_sen_01` liefert `temp_01c`, `hum_01pct`, `lux`
- `sim_bat_sen_01` liefert `battery_mv`, `battery_pct`
- `sim_net_erl_01` und `sim_net_zrl_01` schreiben keine reinen Bool-Aktorzustande nach Influx

## Lauf beenden

Wenn der Stack nur fuer die Simulation lief:

```powershell
docker compose --project-name smarthome_sim --env-file .env.sim.example down -v
```

## Trennung von Simulations- und Hardwareevidenz

Diese Trennung ist nicht verhandelbar.

Simulationsevidenz erkennst du an allen folgenden Merkmalen:

- Compose-Projekt `smarthome_sim`
- Loopback-Ports `18883`, `18880`, `18086`
- IDs `sim_master_01`, `sim_net_erl_01`, `sim_net_zrl_01`, `sim_net_sen_01`, `sim_bat_sen_01`
- `display_name` mit Prefix `[SIM]`
- Run-Report oder Protokolldatei mit klarer Simulationsmarkierung

Nicht behaupten:

- "HELLO nachgewiesen", wenn nur der MQTT-Folgeburst simuliert wurde
- "Hardwarepfad bewiesen", wenn kein reales Geraet lief
- "Node sendete direkt", wenn alles aus dem Fake-Master kam

Wenn du diese Grenzen aufweichst, ruinierst du die Nachweisdoku selbst.
