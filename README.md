# SmartHome ESP32

Dieses Repository enthaelt die aktive Firmware- und Serverbasis fuer ein lokal betriebenes Smart-Home-System auf ESP32. Im Repo liegen der ESP32-C3-Master, die aktuellen Basis-Nodes, der lokale Server-Stack und die Nachweis- und Protokolldoku.

## System in einem Satz

ESP32-Nodes sprechen nur ESP-NOW, der `master` bridged diese Funkebene nach MQTT, und der `server` arbeitet nur oberhalb dieses Masters mit Mosquitto, Node-RED, InfluxDB und SQLite.

## Architektur

```text
net_erl / net_zrl / net_sen / bat_sen
              -- ESP-NOW -->
        master (ESP32-C3 bridge)
              -- MQTT -->
server (Mosquitto + Node-RED + InfluxDB + SQLite)
```

- Nodes nutzen ausschliesslich ESP-NOW.
- MQTT existiert nur zwischen `master` und `server`.
- Der `master` ist die einzige Bridge zwischen Funkebene und Serverebene.
- Der `server` spricht nie direkt mit Nodes.
- Dashboard, Automationen und Datenhaltung laufen serverseitig.
- `status` und `state` bleiben im MQTT-Vertrag getrennt.

## Aktuelle Geraetebasis

- `master`: ESP32-C3-Bridge fuer ESP-NOW <-> MQTT, Registry, ACK/Retry-Weiterleitung und Online/Offline-Erkennung.
- `net_erl`: netzbetriebener Ein-Relais-Node fuer einfache Schaltpfade.
- `net_zrl`: netzbetriebener Zwei-Relais-Node; allgemeine Zwei-Relais-Basis, Cover-Logik nur im Cover-Modus.
- `net_sen`: netzbetriebener Sensor-Node; der aktuell belegte reale Sensorpfad nutzt DHT22 auf `GPIO6`.
- `bat_sen`: batteriebetriebener Sensor-/Event-Node; lokaler ADC-Pfad auf `GPIO4`, offizieller realer Serverpfad bis MQTT `meta/status/state`, SQLite und Influx sowie ein fokussierter Real-Langlauf sind belegt. Offen bleiben die endgueltige Batterieprofilwahl und spaetere Deep-Sleep-/Mehrgeraeteaussagen.

Alle fuenf Geraete liegen als PlatformIO-Environments unter `firmware/platformio.ini`.

## Fester Hardware-Pinstandard

Der verbindliche Referenzpunkt ist `firmware/include/HardwarePinStandard.h`.

- I2C: `GPIO0` = SDA, `GPIO1` = SCL
- interner NeoPixel: `GPIO8`
- Relais-Boards: `Relais 1 = GPIO10`, `Relais 2 = GPIO5`
- Batterie-ADC fuer `bat_sen`: `GPIO4`
- Abweichungen sind nur mit expliziter Doku zulaessig

## Belegter Stand

Die belastbare Statusquelle ist `docs/14_test_und_nachweisstand.md`. Dort zaehlen nur reale Hardware- und reale lokale Servernachweise; Simulationen sind kein Hardware-Beleg.

- `master <-> net_erl_01`: realer Bring-up, MQTT-Minimalpfad, offizieller Node-RED-Rueckweg, ACK/Retry, Offline-Timeout und Langzeitlauf sind belegt.
- `master <-> net_zrl_01`: realer Bring-up, offizieller Node-RED-Rueckweg fuer beide Relais, ACK/Retry, Offline-Timeout und Langzeitlauf sind belegt.
- `net_sen_01`: lokaler Bring-up und offizieller Serverpfad mit realem DHT22 auf `GPIO6` bis MQTT `meta/status/state`, SQLite und Influx sind belegt.
- Server-V1: MQTT-Ingest, SQLite-Ablage, Influx-Schreibpfad und SQLite-Audit fuer ausgehende `cmd/set`-Publishes sind belegt.
- Test-only Servervalidierung: Fake-Master-Harness und getrennte `[SIM]`-Dashboard-Sicht fuer `sim_*` sind lokal belegt, bleiben aber ausdruecklich `simulation` und kein Hardware-Nachweis.
- `bat_sen`: lokaler ADC-/Handshake-Basispfad, offizieller Serverpfad bis MQTT `meta/status/state`, SQLite und Influx sowie ein fokussierter Real-Langlauf sind belegt; offen bleiben endgueltige Batterieprofilwahl, Deep-Sleep-Rueckkehr und Mehrgeraetebetrieb.

Offen bleiben vor allem:

- endgueltige reale Batterieprofilwahl fuer das konkrete `bat_sen_01`-Board und spaetere Deep-Sleep-Rueckkehr erst bei implementiertem Sleep-Pfad
- weitere offizielle Nachweise jenseits der bereits belegten Einzelpfade und der getrennten `sim_*`-Servervalidierung
- Mehrgeraetebetrieb und ein vollstaendiger Gesamtprojektnachweis

## Server-Stack

Der produktive Serverpfad im Repo ist bewusst klein gehalten:

- Mosquitto als MQTT-Broker
- Node-RED als offizieller Bedien-, Automations- und `cmd/set`-Pfad
- InfluxDB fuer numerische Sensorzeitreihen
- SQLite fuer Geraetestatus, Konfiguration und Auditdaten

Der Stack ist unter `server/` als Docker-Compose-Basis aufgebaut und fuer lokalen Betrieb sowie spaetere Uebernahme auf Raspberry Pi ausgelegt.

## Rolle von `migration/` und `automation/night_pipeline/`

Diese Bereiche sind Hilfsstruktur fuer kontrollierte Arbeit am Altbestand, nicht Teil des eigentlichen Produktpfads.

- `migration/` haelt im Haupt-Branch vor allem Stage-Ordner und Status- bzw. Reportdateien fuer die Legacy-Auswertung.
- `automation/night_pipeline/` enthaelt Runner, Prompts, Konfiguration und Logs fuer die gestufte Nacht-Pipeline.
- inhaltlichere Stage-Ergebnisse liegen aktuell in den zugehoerigen `worktrees/night_*`.
- Die Pipeline soll brauchbare Kleinteile aus Altmaterial sichtbar machen und filtern. Sie ersetzt weder die aktive Firmware noch den aktiven Server-Stack.

## Repo-Struktur

- `docs/`: Architektur, MQTT-Vertrag, Pinstandard, Projektgedaechtnis und belegter Ist-Stand
- `PROTOKOLL/`: historische Beta-Staende und einzelne Nachweisdateien
- `firmware/`: PlatformIO-Firmware fuer `master`, `net_erl`, `net_zrl`, `net_sen`, `bat_sen`
- `server/`: Docker-Compose-Serverbasis mit Mosquitto, Node-RED, InfluxDB und SQLite
- `migration/`: Status- und Reportspur der Legacy-Auswertung
- `automation/night_pipeline/`: automatisierte Pipeline fuer gestufte Legacy-Auswertung
- `worktrees/`: stage-spezifische Night-Pipeline-Arbeitsstaende

## Naechste reale Luecke

Die naechste harte Luecke ist nicht mehr der nackte `bat_sen`-Grundpfad, sondern die ehrliche Restarbeit nach dem jetzt belegten Einzelgeraet: reale Batteriechemie bzw. Profilwahl fuer `bat_sen_01`, Deep-Sleep-Rueckkehr erst bei spaeter implementiertem Sleep-Pfad, weitere offizielle Nachweise jenseits der bisherigen Einzelpfade und belastbarer Mehrgeraetebetrieb.

## Einstieg

- `docs/14_test_und_nachweisstand.md`: aktueller belegter Ist-Stand
- `docs/15_hardware_pinstandard.md`: verbindlicher Pinstandard
- `docs/04_mqtt_topics.md`: MQTT-Vertrag
- `server/README.md`: Server-Basis und lokaler Startpfad
- `PROTOKOLL/`: historische Einzelstaende
