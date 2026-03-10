# AGENTS.md

## Projekt
Neubasis fuer ein modulares Smart-Home-System auf ESP32-Basis fuer das Technikerprojekt. Dieses Repository ist die veroeffentlichbare Arbeitsbasis und nicht die Fortsetzung des Altprojekts.

## Vor dem Arbeiten lesen
1. `README.md`
2. `AGENTS.md`
3. `docs/PROJECT_CONTEXT.md`
4. `docs/CURRENT_SPRINT.md`
5. `docs/TASK_QUEUE.md`
6. `docs/DECISIONS.md`

## Architekturregeln
- Nodes sprechen nur ESP-NOW.
- Master ist die Bridge zwischen ESP-NOW und MQTT.
- Server ist getrennt und spricht nicht direkt mit Nodes.
- Kein MQTT in Nodes.
- Keine Sonderarchitektur pro Geraet, sondern Basisklassen mit klaren Erweiterungen.

## Arbeitsregeln fuer Codex
- Erst lesen, dann aendern.
- Keine unnoetigen Refactorings.
- Keine Altlasten blind uebernehmen.
- Keine Secrets ins Repo.
- Jede relevante Aenderung dokumentieren.
- Neue Beta-Staende im `PROTOKOLL/` pflegen.
- Tests, Builds und Checks nur dokumentieren, wenn sie wirklich ausgefuehrt wurden.

## Bevorzugte Arbeitsweise
- Kleine, klar abgegrenzte Aufgaben.
- Keine Parallelaenderungen an unnoetig vielen Modulen.
- Bestehende sinnvolle Inhalte konsolidieren statt neu schreiben.
- Wenn unklar: konservativ aendern statt Architektur neu erfinden.

## Bauen und Pruefen
Repo-Check:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\check_public_repo.ps1
```

Firmware-Build mit PlatformIO im PATH:

```powershell
cd .\firmware
pio run -e master
```

Firmware-Build ohne `pio` im PATH:

```powershell
cd .\firmware
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e master
```

Weitere Environments:
- `net_erl`
- `net_zrl`
- `net_sen`
- `bat_sen`

## Dokumentationspflicht
- Architektur- oder Arbeitsregeln in `docs/DECISIONS.md` festhalten.
- Aktiven Fokus in `docs/CURRENT_SPRINT.md` pflegen.
- Naechste Arbeitspakete in `docs/TASK_QUEUE.md` pflegen.
- Beta-Staende und relevante technische Schritte in `PROTOKOLL/` dokumentieren.
