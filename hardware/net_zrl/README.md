# NET_ZRL

## Zweck
- Netzbetriebener Zwei-Relais-Node als allgemeine Basis fuer Doppelschaltaktoren.
- `net_zrl` bleibt allgemeine Zwei-Relais-Basis; Cover-Logik ist nur im Cover-Modus zulaessig.
- Aktueller Repo-Pilot: `net_zrl_01`

## Repo-Basis
- Firmware-Environment: `net_zrl`
- Buildbasis im Repo: `esp32-c3-devkitm-1` in `firmware/platformio.ini`
- Aktuelle Firmware-Variante im Repo: `net_zrl_base`
- `COVER_MODUS_AKTIV` steht im aktuellen Repo-Stand auf `false`

## Feste relevante Pins
- `GPIO10` = Relais 1
- `GPIO5` = Relais 2
- `GPIO0` = I2C SDA
- `GPIO1` = I2C SCL
- `GPIO8` = interner NeoPixel
- Keine lokalen Taster, kein PIR und keine Zusatz-LED im aktuellen `PinConfig.h`

## Kommunikationspfad
- Node spricht nur ESP-NOW zum `master`, aktuell auf Kanal `6`
- MQTT laeuft ausschliesslich ueber den `master`

## Reale Statuslage
- Reales Bring-up `master <-> net_zrl_01` inkl. HEARTBEAT und `STATE_REPORT` ist belegt.
- Offizieller serverseitiger Rueckweg fuer `relay_1` und `relay_2` ist belegt.
- ACK/Retry, Offline-Timeout und Langzeitlauf sind fuer den offiziellen `cmd/set`-Pfad belegt.
- Details: `docs/14_test_und_nachweisstand.md`

## Offen / nicht verifiziert
- Cover-Modus und zugehoerige Rolladen-Schutzlogik sind im aktuellen Repo-Stand nicht als Realnachweis belegt.
- Optionale Taster- oder Sensorsonderbestaeckung ist nicht real nachgewiesen.
- Keine separate Hardwaredoku mit Fotos oder Schaltplan im Repo.

## Build / Flash
- Build: `platformio.exe run -e net_zrl`
- Flash: `platformio.exe run -e net_zrl -t upload --upload-port <COM_PORT>`
- Monitor: `115200`
