# NET_ERL

## Zweck
- Netzbetriebener Ein-Relais-Node fuer einfache Schaltpfade.
- Aktueller Repo-Pilot: `net_erl_01`

## Repo-Basis
- Firmware-Environment: `net_erl`
- Buildbasis im Repo: `esp32-c3-devkitm-1` in `firmware/platformio.ini`
- Aktuelle Firmware-Variante im Repo: `net_erl_pilot`

## Feste relevante Pins
- `GPIO10` = Relais 1
- `GPIO0` = I2C SDA
- `GPIO1` = I2C SCL
- `GPIO8` = interner NeoPixel
- Kein zweites Relais, kein lokaler Taster, kein PIR und kein LED-Ring im aktuellen `PinConfig.h`

## Kommunikationspfad
- Node spricht nur ESP-NOW zum `master`, aktuell auf Kanal `6`
- MQTT existiert fuer `net_erl` nur indirekt ueber den `master`

## Reale Statuslage
- Reales Bring-up `master <-> net_erl_01` ist belegt.
- MQTT-Minimalpfad inkl. realem Relais-Schalten ist belegt.
- Offizieller serverseitiger Rueckweg ueber Node-RED ist belegt.
- ACK/Retry, Offline-Timeout und Langzeitlauf sind fuer den offiziellen `cmd/set`-Pfad belegt.
- Details: `docs/14_test_und_nachweisstand.md`

## Offen / nicht verifiziert
- Optionale Varianten mit lokalem Taster, PIR, I2C-Zusatzsensorik oder LED-Ring sind im aktuellen Repo-Stand nicht real nachgewiesen.
- Keine separate Hardwaredoku mit Fotos oder Schaltplan im Repo.

## Build / Flash
- Build: `platformio.exe run -e net_erl`
- Flash: `platformio.exe run -e net_erl -t upload --upload-port <COM_PORT>`
- Monitor: `115200`
- Detail-Runbook: `docs/16_build_flash_bringup_master_net_erl.md`
