# CURRENT_SPRINT

## Sprintziel
Die Firmware-Basis fuer die fuenf Basisgeraete additiv so vorbereiten, dass optionale `net_sen`-/`bat_sen`-Module sauber anschliessbar sind, ohne die belegten Referenzpfade oder die Architekturgrenzen zu verwischen. Stub- und Scaffold-Pfade bleiben ausdruecklich Build-/Dev-Hilfen; der naechste groessere Realnachweis folgt spaeter als integrierte Hardware-Welle.

## Im Fokus
- additive Provider-/Feature-Struktur in `firmware/` fuer `net_sen` und `bat_sen` statt weiterer Monolith-Hacks
- compile-time Auswahl fuer optionale Module und Sonderpfade:
  `BMP280`/`BME280`, `ENS160`, `PIR`, `reed`, `button`, `rain`
- reale Referenzpfade stabil halten:
  `master`, `net_erl`, `net_zrl`, `net_sen` mit DHT22 auf `GPIO6`, `bat_sen`-Batteriemessung auf `GPIO4`
- Build-Validierung fuer Basis-Targets und ausgewaehlte Stub-Kombinationen
- Doku sauber halten:
  scaffolding und Stub-Support sind kein Hardware-Nachweis

## Aktuell nicht im Fokus
- Dashboard-Politur
- breiter Server-/Schema-Umbau
- neue per-device/per-sensor Sonderarchitekturen
- zerlegte Mikrotests auf halbfertiger Firmware
- grosse kombinierte Realhardware-Wellen vor sauber vorbereiteter Codebasis

## Belegter Projektstand, auf dem dieser Sprint aufsetzt
- `master <-> net_erl_01` und `master <-> net_zrl_01` sind mit offiziellem Node-RED-Rueckweg, ACK/Retry, Offline-Timeout und Langzeitstabilitaet im Repo belegt
- `net_sen_01` ist mit realem DHT22 auf `GPIO6` bis MQTT `meta/status/state`, SQLite und Influx offiziell belegt
- die Server-V1 hat reale lokale Nachweise fuer MQTT-Ingest, SQLite-Ablage, Influx-Schreibpfad und `cmd/set`-Egress-Audit
- der Fake-Master-Harness und die getrennte `[SIM]`-Dashboard-Seite sind fuer `sim_*` lokal belegt, bleiben aber ausdruecklich `simulation` und kein Hardware-Nachweis
- `bat_sen_01` ist auf realem Board jetzt lokal bis ADC-/Handshake-Basis, offiziell bis Broker/SQLite/Influx und zusaetzlich in einem fokussierten Langlauf belegt; offen bleiben die bewusste Profildeutung von `battery_pct`, spaeterer Deep-Sleep und Mehrgeraetebetrieb
- optionale `net_sen`-/`bat_sen`-Module jenseits dieser Referenzpfade koennen code- und build-seitig vorbereitet sein, sind ohne separate Realbelege aber weiterhin offen

## Sprint-Abnahme
1. `net_sen`- und `bat_sen`-Erweiterungen bauen ueber klare Provider-/Featurepfade, ohne die belegten Defaults zu brechen.
2. Shared Docs und `docs/14_test_und_nachweisstand.md` trennen weiter sauber zwischen `build`, `simulation` und `real hardware`.
3. Die naechste Realprioritaet ist klar: integrierte Hardware-Wellen fuer optionale Module und Mehrgeraetebetrieb, nicht neuer Serverballast oder weitere Monolith-Hacks.
