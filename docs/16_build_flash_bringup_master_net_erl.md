# Build-, Flash- und Bring-up-Runbook fuer master und net_erl

## 1. Zweck und Geltungsbereich
- Dieses Dokument beschreibt den kontrollierten Ablauf fuer Build, Portwahl, Flash und seriellen Bring-up von `master` und `net_erl` im aktiven Repo `C:\Users\mries\Documents\Playground\smarthome-esp32`.
- Es ist bewusst kein vollstaendiger End-to-End-Systemnachweis und ersetzt nicht `docs/14_test_und_nachweisstand.md`.
- Bereits im Repo belegt:
  - reale Bring-up-Minimalstrecke `master <-> net_erl`: `docs/14_test_und_nachweisstand.md`, `PROTOKOLL/beta06_firmware_bringup_master_net_erl..txt`
  - Buildbasis fuer `master` und `net_erl`: `docs/14_test_und_nachweisstand.md`
- Getrennt vom Runbook inzwischen ebenfalls belegt:
  - offizieller kombinierter Live-Nachweis `Node-RED -> MQTT cmd/set -> SQLite audit_log egress -> Master -> net_erl -> MQTT state -> SQLite state`: `docs/14_test_und_nachweisstand.md`, `PROTOKOLL/beta14_kombinierter_live_nachweis_offizieller_rueckweg_net_erl_01_aktueller_repo_stand.txt`
- Weiterhin offen:
  - ACK-/Retry-Nachweis, vollstaendige Servervalidierung, weitere Basisgeraete

## 2. Voraussetzungen
- Aktives Repo: `C:\Users\mries\Documents\Playground\smarthome-esp32`
- Massgebliche PlatformIO-Konfiguration: `firmware/platformio.ini`
- Im Repo-Root liegt kein eigenes `platformio.ini`.
- PlatformIO in VS Code oder PlatformIO-CLI
- Auf diesem Rechner ist `platformio.exe` unter `%USERPROFILE%\.platformio\penv\Scripts\platformio.exe` vorhanden; `pio` war beim Bearbeitungsstand nicht im `PATH`.
- USB-Datenverbindung zu den realen Zielgeraeten
- Reale Zielgeraete:
  - `master`, Device-ID `MASTER-001`, Environment `master`
  - `net_erl`, Device-ID `net_erl_01`, Environment `net_erl`
- Beide Firmware-Staende bauen fuer `esp32-c3-devkitm-1`.
- Fuer realen `master`-Bring-up mit WLAN und MQTT ist eine lokale `firmware/include/Secrets.h` mit gueltigen Werten noetig. Vorlage: `firmware/include/Secrets.example.h`.
- MQTT/Broker ist nur fuer den `master`-Bring-up und die Minimal-Funktionspruefung relevant, nicht fuer den reinen Build von `net_erl`.
- Wenn fuer den MQTT-Teil der versionierte Server-Stack unter `server/` genutzt wird, muss Docker Desktop real laufen. Fuer den Repo-Default muss der lokal ignorierte Ordner `server/config/mosquitto/config/` als Verzeichnis vorhanden sein, auch wenn er leer bleibt; sonst scheitert Mosquitto an `include_dir /mosquitto/config/local`.
- Wenn fuer den serverseitigen Node-RED-Bedienpfad ein geaenderter versionierter Flow-Generatorstand geprueft werden soll, reicht ein blosses `docker compose up --build -d` nicht immer. Ein persistiertes `server_nodered_data` kann ein altes `/data/flows.json` weiterfahren; fuer einen echten Test des aktuellen Generatorstands die Node-RED-Datenbasis vorher gezielt erneuern, z. B. per `docker compose down` und `docker volume rm server_nodered_data`.

## 3. Relevante Build-Umgebungen

| Bereich | PlatformIO-Environment | Relevante Dateien |
|---|---|---|
| `master` | `master` | `firmware/platformio.ini`, `firmware/src/master/AppConfig.h`, `firmware/src/master/PinConfig.h`, `firmware/include/Secrets.h`, `firmware/lib/ShProtocol/src/Protocol.h` |
| `net_erl` | `net_erl` | `firmware/platformio.ini`, `firmware/src/net_erl/AppConfig.h`, `firmware/src/net_erl/PinConfig.h`, `firmware/lib/ShProtocol/src/Protocol.h` |

- `firmware/platformio.ini` setzt fuer beide Environments:
  - Board `esp32-c3-devkitm-1`
  - `monitor_speed = 115200`
  - USB-CDC-Build-Flags `ARDUINO_USB_MODE=1` und `ARDUINO_USB_CDC_ON_BOOT=1`
- `master` nutzt `build_src_filter = -<*> +<master/>`.
- `net_erl` nutzt `build_src_filter = -<*> +<net_erl/>`.

## 4. Kontrollierter Build
- Erst beide Builds erfolgreich abschliessen, dann erst flashen.
- Empfohlene Reihenfolge:
  1. `master`
  2. `net_erl`
- Gepruefte CLI-Form mit lokal vorhandenem Tool:

```powershell
cd C:\Users\mries\Documents\Playground\smarthome-esp32\firmware
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e master
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e net_erl
```

- Wenn `pio` im `PATH` liegt, ist die kuerzere Form moeglich:

```powershell
cd C:\Users\mries\Documents\Playground\smarthome-esp32\firmware
pio run -e master
pio run -e net_erl
```

- Erfolgreicher Build:
  - PlatformIO endet mit `SUCCESS`
  - das richtige Environment steht auf `SUCCESS`
  - Artefakte liegen unter `.pio\build\<environment>\`
- Bei Buildfehlern festhalten:
  - Datum/Uhrzeit
  - exakter Befehl
  - betroffenes Environment
  - erste fachlich relevante Fehlermeldung oder die letzten relevanten Konsolenzeilen
  - ob lokale `Secrets.h` vorhanden war
- Nicht sauber ist:
  - nach einem fehlgeschlagenen Build trotzdem ein altes Binary zu flashen
  - mehrere Fehlerquellen gleichzeitig zu aendern und danach nicht mehr zu wissen, was geholfen hat

## 5. Portwahl ohne Blindflug
- Vor dem Flashen Ports immer aktiv bestimmen, nicht raten.
- Bewaehrte lokale Abfragen:

```powershell
Get-CimInstance Win32_SerialPort | Select-Object DeviceID, Name, Description, PNPDeviceID
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" device list
```

- Vorgehen:
  1. Ausgangsbestand der Ports erfassen.
  2. Genau ein Zielgeraet anstecken.
  3. Portliste erneut erfassen.
  4. Neu erschienenen Port mit `DeviceID` und Hardware-ID/SER notieren.
  5. Erst danach `--upload-port` und Monitor-Port setzen.
  6. Fuer das zweite Geraet denselben Ablauf getrennt wiederholen.
- Bei mehreren USB-Seriell-Geraeten ist die COM-Nummer allein zu schwach. Auf dem lokalen Bearbeitungsstand lieferten `COM3` und `COM10` beide `USB VID:PID=303A:1001`; unterscheidbar waren sie erst ueber die Hardware-ID/SER.
- Nicht erlauben:
  - Upload auf einen Port, dessen Geraet nicht sauber zugeordnet ist
  - "wahrscheinlich ist es COMx" als Entscheidungsgrundlage
- Vor jedem Upload den gewaehlten Port kurz festhalten, z. B.:
  - `master -> COM3, Hardware-ID/SER ...`
  - `net_erl -> COM10, Hardware-ID/SER ...`

## 6. Kontrollierter Flash-Ablauf
- Sinnvolle Reihenfolge:
  1. `master` flashen
  2. `master` seriell pruefen
  3. `net_erl` flashen
  4. `net_erl` seriell pruefen
  5. danach erst die MQTT-Minimalpruefung abschliessen
- Grund fuer diese Reihenfolge: `net_erl` sendet sofort `HELLO`; ohne aktiven `master` ist der erste Bring-up-Befund unnoetig verrauscht.
- Gepruefte Upload-Syntax:

```powershell
cd C:\Users\mries\Documents\Playground\smarthome-esp32\firmware
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e master -t upload --upload-port <COM_MASTER>
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e net_erl -t upload --upload-port <COM_NET_ERL>
```

- Direkt nach jedem Upload pruefen:
  - meldet PlatformIO den Upload ohne Fehler
  - startet die serielle Ausgabe auf `115200`
  - erscheint das erwartete Startbanner
  - haengt das Geraet nicht in einem Boot-Loop
- Beim `master` unmittelbar danach zusaetzlich pruefen:
  - `ESP-NOW bereit`
  - bei gueltiger `Secrets.h` und laufendem Broker auch `WLAN verbunden` und `MQTT verbunden`
- Beim `net_erl` unmittelbar danach zusaetzlich pruefen:
  - `ESP-NOW bereit auf Kanal 6`
  - `HELLO gesendet`
  - nach erreichbarem `master`: `HELLO_ACK empfangen`

## 7. Serieller Bring-up
- Monitor-Geschwindigkeit: `115200`
- Sauberer Monitor-Aufruf:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" device monitor -d C:\Users\mries\Documents\Playground\smarthome-esp32\firmware -e master -p <COM_MASTER>
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" device monitor -d C:\Users\mries\Documents\Playground\smarthome-esp32\firmware -e net_erl -p <COM_NET_ERL>
```

- Wenn Logs mitgeschrieben werden sollen, den Monitor in eine Datei spiegeln. Ein Repo-eigenes Auswertewerkzeug existiert dafuer noch nicht; `tools/serial_log_parser/README.md` ist aktuell nur ein Platzhalter.

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" device monitor -d C:\Users\mries\Documents\Playground\smarthome-esp32\firmware -e master -p <COM_MASTER> | Tee-Object -FilePath .\master_serial.log
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" device monitor -d C:\Users\mries\Documents\Playground\smarthome-esp32\firmware -e net_erl -p <COM_NET_ERL> | Tee-Object -FilePath .\net_erl_serial.log
```

### Relevante Beobachtungen am `master`
- Startbanner mit `MASTER`, Firmware-Version und bekannter Node-Liste
- `WLAN-Verbindung gestartet: SSID=...`
- `WLAN verbunden: IP=..., Kanal=6`
- `ESP-NOW bereit`
- `MQTT konfiguriert: <host>:<port>`
- `MQTT verbunden`
- nach `net_erl`-Start:
  - `HELLO von net_erl_01`
  - `Node net_erl_01 ist online`
  - `STATE_REPORT von net_erl_01 verarbeitet`
  - spaeter periodisch `HEARTBEAT von net_erl_01`

### Relevante Beobachtungen am `net_erl`
- Startbanner mit `NET-ERL`, `Node-ID: net_erl_01`, `Relay1 GPIO: 10`, `I2C: SDA=GPIO0 SCL=GPIO1`, `NeoPixel GPIO: 8`
- `ESP-NOW bereit auf Kanal 6`
- `HELLO gesendet`
- `HELLO_ACK empfangen, Master-Kanal=6`
- `Master-MAC: ...`
- `STATE_REPORT gesendet`
- spaeter periodisch `HEARTBEAT gesendet`
- bei MQTT-Schaltbefehl:
  - `COMMAND_SET_RELAY empfangen: relay_1=true/false`
  - `Relais physisch EIN/AUS auf GPIO10` oder logisch entsprechender Hinweis

### Beobachtungen, die auf Fehler hindeuten
- `ESP-NOW Initialisierung fehlgeschlagen`
- `WLAN getrennt`
- `MQTT connect fehlgeschlagen`
- `HELLO ignoriert` oder `HELLO_ACK abgelehnt`
- `STATE_REPORT wartet auf HELLO_ACK`
- `ESP-NOW Paket verworfen: CRC/Header ungueltig`
- `Node net_erl_01 nach Timeout offline`
- Wiederholte `HELLO`-Versuche ohne `HELLO_ACK` deuten praktisch auf fehlenden `master`, Kanalproblem oder falschen Port/Flashstand hin.
- Logs nicht mit "sieht gut aus" bewerten. Relevante Zeilen und Zeitpunkt mitschreiben oder als Rohlog ablegen.

## 8. MQTT-/Funktionspruefung im Minimalumfang
- Das Repo enthaelt derzeit keinen belastbaren Topic-Tester; `tools/topic_tester/README.md` ist nur ein Platzhalter. Das MQTT-Client-Werkzeug muss lokal vorhanden sein und im Testnachweis benannt werden.
- Minimal relevante Topics gemaess `docs/04_mqtt_topics.md` und aktuellem Firmware-Stand:
  - `smarthome/master/MASTER-001/status`
  - `smarthome/master/MASTER-001/event`
  - `smarthome/node/net_erl_01/meta`
  - `smarthome/node/net_erl_01/status`
  - `smarthome/node/net_erl_01/state`
  - `smarthome/node/net_erl_01/cmd/set`
  - `smarthome/node/net_erl_01/cmd/get`
- Minimale Erwartung nach erfolgreichem Bring-up:
  - `master/status` meldet `online=true`
  - `net_erl_01/meta` erscheint nach `HELLO`
  - `net_erl_01/status` geht auf `online=true`
  - `net_erl_01/state` erscheint nach `STATE_REPORT`
- `smarthome/master/MASTER-001/event` ist bewusst transient und nicht retained. Wenn dieser Kanal real beobachtet werden soll, muss das Topic bereits vor einem Master-Neustart oder MQTT-Reconnect abonniert sein.
- Minimaler Schreibtest:

```json
{"cmd":"set_relay","relay_1":true,"request_id":"cmd_0001"}
```

- Minimaler Erfolg:
  - `master` loggt den MQTT-Empfang
  - `master` sendet `COMMAND_SET_RELAY`
  - `net_erl` loggt den Empfang und schaltet `relay_1`
  - ein aktualisierter `state`-Payload fuer `net_erl_01` erscheint wieder auf MQTT
- Optionaler Lesetest:
  - `cmd/get` mit `{"cmd":"state"}` oder `{"cmd":"get_state"}`
- Nicht automatisch als vollstaendiger End-to-End-Nachweis werten:
  - Nachweis von Node-RED, SQLite oder Influx
  - ACK-/Retry-Funktionalitaet
  - komfortbezogene Logik, weitere Geraete oder dauerhafte Stabilitaet
  - offizieller Gesamtbeleg gemaess `docs/14_test_und_nachweisstand.md`

## 9. Dokumentationspflicht
- Nach einem real durchgefuehrten Lauf sind Beobachtungen sauber nachzuziehen, nicht nur das Ergebnisetikett.
- In `PROTOKOLL/` gehoert ein neuer Beta-Stand, wenn der Lauf einen neuen echten Repo-Stand belegt.
- `docs/14_test_und_nachweisstand.md` wird nur angepasst, wenn der neue Lauf ueber den bisher offiziell belegten Stand hinausgeht.
- Mindestens festhalten:
  - Datum/Uhrzeit
  - verwendete Befehle fuer Build, Upload und Monitor
  - zugeordnete Ports inklusive Hardware-ID/SER
  - ob `Secrets.h`, WLAN und MQTT real aktiv waren
  - serielle Kernbeobachtungen
  - beobachtete MQTT-Topics und Beispielpayloads
  - was fehlgeschlagen ist oder offen blieb
- Annahmen und Beobachtungen nicht vermischen. Klare Schreibweise:
  - `Annahme:` erwarteter Ablauf oder vorbereitete Voraussetzung
  - `Beobachtung:` real gesehene Ausgabe, Topic oder Hardware-Reaktion

## 10. Offene Grenzen
- Kein vollstaendiges Server-Runbook
- Dieses Dokument selbst ist nicht der offizielle Gesamtbeleg ueber die komplette Strecke bis Server und zurueck; der aktuelle Beleg fuer `net_erl_01` liegt getrennt in `docs/14_test_und_nachweisstand.md` und `PROTOKOLL/beta14_kombinierter_live_nachweis_offizieller_rueckweg_net_erl_01_aktueller_repo_stand.txt`
- Kein belastbarer ACK-/Retry-Nachweis
- Kein Nachweis fuer `net_zrl`, `net_sen` oder `bat_sen`
- Kein vollstaendiger Offline-/Langzeittest
- Keine Komfortfunktionen, lokale Tasterlogik oder Cover-Sonderfaelle
- Hardware-README-Dateien fuer `master` und `net_erl` sind weiterhin Platzhalter und ersetzen keine reale Hardware-Dokumentation.
