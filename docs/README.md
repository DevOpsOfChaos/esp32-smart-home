# Docs-Index

## Rolle dieser Datei
Zentraler Index fuer die Projektdoku. Diese Datei ordnet die Dokumente ein, ersetzt aber nicht das Projektgedaechtnis.

## Zuerst lesen
1. `../AGENTS.md`
2. `../README.md`
3. `PROJECT_CONTEXT.md`
4. `CURRENT_SPRINT.md`
5. `TASK_QUEUE.md`
6. `DECISIONS.md`
7. `14_test_und_nachweisstand.md`, wenn Status oder Testaussagen relevant sind

## Verbindliches Projektgedaechtnis
- `PROJECT_CONTEXT.md`: stabiler Projektrahmen, Repo-Grenzen, Dokumentrollen
- `CURRENT_SPRINT.md`: aktueller Fokus
- `TASK_QUEUE.md`: naechste reale Arbeitspakete
- `DECISIONS.md`: dauerhafte Entscheidungen mit Grund

## Fachdoku
- `00_projektueberblick.md`: kurzer Projektabriss
- `01_architektur.md`: Zielarchitektur und harte Grenzen
- `03_protokoll.md`: ESP-NOW-Protokoll und Paketregeln
- `04_mqtt_topics.md`: MQTT-Vertrag
- `05_hardware_geraete.md`: Basisgeraete und Sondergeraete
- `BAT_SEN_ADC_FACT_GAP.md`: aktueller `bat_sen`-ADC-Stand, erledigte Realnachweise und ehrliche Restpunkte
- `15_hardware_pinstandard.md`: verbindlicher Pinstandard

## Test und Nachweis
- `06_testplan.md`: Soll-Pruefplan, keine Ist-Aussagen
- `14_test_und_nachweisstand.md`: belegter Ist-Stand; `simulation` bleibt dort explizit von `real hardware` getrennt
- `14_minimalstrecke_net_erl_master.md`: technischer Ablauf und Detailblatt der Minimalstrecke, kein globales Nachweisdokument
- `16_build_flash_bringup_master_net_erl.md`: kontrolliertes Runbook fuer Build, Portwahl, Flash und seriellen Bring-up von `master` und `net_erl`

## Historie
- `../PROTOKOLL/`: historische Einzelstaende, inklusive damaliger Testaussagen
- `../CHANGELOG.md`: verdichtete Versionshistorie des Repos

## Bereichsdoku
- `../server/README.md` und `../server/docs/`: Server-Bereichsdoku
- `../server/docs/03_server_runbook_lokalbetrieb.md`: lokales Betriebs-Runbook fuer den `server/`-Stack
- `../server/docs/04_server_simulation_runbook.md`: isolierter Fake-Master-Simulationslauf; ausdruecklich kein Hardware-Nachweis
- `../server/db/README.md`: Datenbasis des Servers
- `../server/flows/README.md`: Flow-Rolle und Ingest-Semantik
- `../hardware/*/README.md`: geraetespezifische Hardwareunterlagen
- `../tools/*/README.md`: Werkzeugrollen
- `../firmware/test/*/README.md`: Testordner-Rollen

## Offene Doku-Luecken
- endgueltige reale Batterieprofilwahl fuer `bat_sen_01` und spaetere Deep-Sleep-Rueckkehrvalidierung erst bei implementiertem Sleep-Pfad
- weitere offizielle Nachweise jenseits der belegten Einzelpfade `net_erl_01`, `net_zrl_01`, `net_sen_01`, `bat_sen_01` und der getrennten `sim_*`-Servervalidierung
- vollstaendiger Gesamtprojektnachweis ueber die bisherigen realen Pilotpfade und die getrennte Simulation hinaus
- Hardware-README-Dateien enthalten bisher nur Platzhalter
