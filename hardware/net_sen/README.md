# NET_SEN

## Zweck
- Netzbetriebener Sensor-Node ohne Relais.
- Aktueller real belegter Sensorpfad ist ein DHT22-Minimalpfad.
- Aktueller Repo-Pilot: `net_sen_01`

## Repo-Basis
- Firmware-Environment: `net_sen`
- Buildbasis im Repo: `esp32-c3-devkitm-1` in `firmware/platformio.ini`
- Zusaetzliche Library im Repo-Build: `adafruit/DHT sensor library`

## Feste relevante Pins
- `GPIO6` = DHT22-Datenleitung im aktuell belegten Minimalpfad
- `GPIO0` = I2C SDA
- `GPIO1` = I2C SCL
- `GPIO8` = interner NeoPixel
- Keine Relais, kein PIR und kein LED-Ring im aktuellen `PinConfig.h`

## Kommunikationspfad
- Node spricht nur ESP-NOW zum `master`, aktuell auf Kanal `6`
- MQTT, SQLite und Influx werden nur ueber den `master` erreicht

## Reale Statuslage
- Lokaler Bring-up bis `HELLO_ACK`, `STATE_REPORT` und `HEARTBEAT` ist belegt.
- Realer DHT22-Pfad auf `GPIO6` mit plausiblen Temperatur-/Feuchtewerten ist belegt.
- Offizieller Serverpfad `net_sen_01 -> master -> MQTT meta/status/state -> SQLite -> Influx` ist belegt.
- Details: `docs/14_test_und_nachweisstand.md`

## Offen / nicht verifiziert
- Andere Sensorvarianten ueber I2C, PIR/Radar oder LED-Ring sind im aktuellen Repo-Stand nicht real nachgewiesen.
- Keine separate Hardwaredoku mit Fotos oder Schaltplan im Repo.

## Lokaler DHT22-Minimalcheck
- DHT22-Datenleitung an `GPIO6`
- gemeinsame Masse zwischen `net_sen_01` und DHT22
- Versorgung passend zum verwendeten Sensor/Modul; die Datenleitung darf den ESP32-C3 nicht mit `5V` treiben
- bei einem nackten DHT22/AM2302 ohne Modul-Pull-up einen Pull-up zwischen `DATA` und `VCC` vorsehen

## Serielle Marker
- gueltig: `HELLO_ACK empfangen` und danach `DHT22 OK temp=... hum=...`
- Sensorfehler: `DHT22 FEHLER ...`
- Verdrahtungshinweis: `DHT22 HINWEIS: seit Boot keine gueltige Messung; GPIO6/VCC/GND/Pull-up pruefen`

## Build / Flash
- Build: `platformio.exe run -e net_sen`
- Flash: `platformio.exe run -e net_sen -t upload --upload-port <COM_PORT>`
- Monitor: `115200`
