# BAT_SEN

## Zweck
- Batteriebetriebener Sensor-/Event-Node fuer einfache Zustands- und Batteriepfade.
- Aktueller Repo-Pilot: `bat_sen_01`

## Repo-Basis
- Firmware-Environment: `bat_sen`
- Buildbasis im Repo: `esp32-c3-devkitm-1` in `firmware/platformio.ini`
- Aktuelle Firmware-Variante im Repo: `bat_sen_base`

## Feste relevante Pins
- `GPIO4` = Batterie-ADC
- `GPIO0` = I2C SDA
- `GPIO1` = I2C SCL
- `GPIO8` = interner NeoPixel
- Keine Relais, keine Buttons, kein Reed und kein Rain-ADC im aktuellen `PinConfig.h`

## Kommunikationspfad
- Node spricht nur ESP-NOW zum `master`, aktuell auf Kanal `6`
- MQTT, SQLite und Influx werden nur ueber den `master` erreicht

## Reale Statuslage
- Lokaler Bring-up bis `HELLO_ACK` und erster `STATE_REPORT` ist belegt.
- Der reale Spannungspfad ueber `GPIO4` ist lokal gegen Batteriespannung und Teilerknoten plausibilisiert.
- Der offizielle Serverpfad `bat_sen_01 -> master -> MQTT meta/status/state -> SQLite -> Influx` ist belegt.
- Ein fokussierter laengerer Realbetrieb ueber mehrere 5-Minuten-Zyklen ist belegt.
- Details: `docs/14_test_und_nachweisstand.md`

## Batterieprofil im aktuellen Stand
- `battery_mv` kommt aus dem real gemessenen ADC-Pfad auf `GPIO4`.
- `battery_pct` ist eine profilabhaengige Interpretation dieses Spannungswerts.
- Das aktive Profil wird bewusst in `firmware/src/bat_sen/AppConfig.h` gesetzt.
- Im aktuellen Repo-Stand ist `BATTERY_PROFILE = coin_3v_primary`.

## Offen / nicht verifiziert
- Die endgueltige boardkonkrete Profilwahl fuer `battery_pct` bleibt offen, falls `coin_3v_primary` die reale Chemie nicht sauber abbildet.
- Deep-Sleep-Rueckkehr ist nicht belegt, weil der Sleep-Pfad im aktuellen Stand nicht bewusst aktiviert ist.
- Optionaler Reed-, Button- oder Rain-Pfad ist nicht real nachgewiesen.
- Mehrgeraetebetrieb ist nicht belegt.
- Keine separate Hardwaredoku mit Fotos oder Schaltplan im Repo.

## Build / Flash
- Build: `platformio.exe run -e bat_sen`
- Flash: `platformio.exe run -e bat_sen -t upload --upload-port <COM_PORT>`
- Monitor: `115200`
