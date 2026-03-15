# MASTER

## Zweck
- ESP32-C3-Master als einzige Bridge zwischen ESP-NOW-Funkebene und MQTT-Server.
- Aktueller Repo-Stand nutzt die feste Pilot-ID `MASTER-001`.

## Repo-Basis
- Firmware-Environment: `master`
- Buildbasis im Repo: `esp32-c3-devkitm-1` in `firmware/platformio.ini`
- Fuer realen WLAN-/MQTT-Betrieb ist eine lokale `firmware/include/Secrets.h` noetig

## Feste relevante Pins
- `GPIO0` = I2C SDA
- `GPIO1` = I2C SCL
- `GPIO8` = interner NeoPixel
- Keine Relais, keine separate Status-LED und kein lokaler Taster im aktuellen `PinConfig.h`

## Kommunikationspfad
- Zu Nodes: nur ESP-NOW, aktuell auf Kanal `6`
- Zum Server: nur WLAN/MQTT
- Der `server` spricht nie direkt mit Nodes

## Reale Statuslage
- Mit `net_erl_01` sind Bring-up, MQTT-Minimalpfad, offizieller Node-RED-Rueckweg, ACK/Retry, Offline-Timeout und Langzeitlauf belegt.
- Mit `net_zrl_01` sind offizieller Rueckweg fuer beide Relais, ACK/Retry, Offline-Timeout und Langzeitlauf belegt.
- Mit `net_sen_01` ist der offizielle DHT22-Serverpfad belegt.
- Mit `bat_sen_01` sind lokaler Handshake, offizieller Serverpfad und fokussierter Langlauf belegt.
- Details: `docs/14_test_und_nachweisstand.md`

## Offen / nicht verifiziert
- Kein gemeinsamer Real-Langlauf mit mehreren aktiven Basisnodes im selben Versuch.
- Keine separate Master-Hardwaredoku mit Fotos oder Schaltplan im Repo.

## Build / Flash
- Build: `platformio.exe run -e master`
- Flash: `platformio.exe run -e master -t upload --upload-port <COM_PORT>`
- Monitor: `115200`
- Detail-Runbook: `docs/16_build_flash_bringup_master_net_erl.md`
