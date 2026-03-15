# CURRENT_SPRINT

## Sprintziel
Die Firmware-Basis und die Projektdoku so diszipliniert halten, dass kommende Sondermodule aus den bestehenden Basisfamilien wachsen koennen, ohne die belegten Referenzpfade oder die Architekturgrenzen zu verwischen. Jetzt zaehlt code-first Vorbereitung mit ehrlichen Reifestufen; groessere reale Hardwarewellen folgen spaeter gebuendelt.

## Im Fokus
- additive Provider-/Feature-Struktur in `firmware/` fuer `net_sen` und `bat_sen` statt weiterer Monolith-Hacks
- klare Basistyp-Disziplin fuer kommende Sondermodule:
  `net_sen` fuer Umwelt-/Praesenz-/Lichtsensorik, `bat_sen` fuer Batterie-/Deep-Sleep-/Eventpfade, `net_erl` und `net_zrl` nur fuer begruendete Relais-Sonderrollen
- compile-time Auswahl und dokumentierte Architekturvorbereitung fuer optionale Module:
  `BMP280`/`BME280`, `ENS160`, `PIR`, `reed`, `button`, `rain`, spaetere `SHT41`, `VEML7700` und `LD2410`
- reale Referenzpfade stabil halten:
  `master`, `net_erl`, `net_zrl`, `net_sen` mit DHT22 auf `GPIO6`, `bat_sen`-Batteriemessung auf `GPIO4`
- Setup-/Provisionierungs-, Reset- und Server-Authority-Regeln jetzt dokumentarisch festziehen, ohne ihre Firmware-Implementierung vorzutaeuschen
- Doku sauber halten:
  `nicht vorgesehen`, `architekturvorbereitet`, `compile/stub vorbereitet` und `real hardware` bleiben strikt getrennt

## Aktuell nicht im Fokus
- Vollimplementierung aller kuenftigen Sondermodule
- Dashboard-Politur
- breiter Server-/Schema-Umbau
- neue per-device/per-sensor Sonderarchitekturen oder neue Basisklassen
- zerlegte Mikrotests auf halbfertiger Firmware
- grosse kombinierte Realhardware-Wellen vor sauber vorbereiteter Codebasis
- vorgetaeuschte Reife bei Rolladen-Positionslogik, Touch-Zuverlaessigkeit oder Radar-/Lichtsensorpfaden

## Belegter Projektstand, auf dem dieser Sprint aufsetzt
- `master <-> net_erl_01` und `master <-> net_zrl_01` sind mit offiziellem Node-RED-Rueckweg, ACK/Retry, Offline-Timeout und Langzeitstabilitaet im Repo belegt
- `net_sen_01` ist mit realem DHT22 auf `GPIO6` bis MQTT `meta/status/state`, SQLite und Influx offiziell belegt
- die Server-V1 hat reale lokale Nachweise fuer MQTT-Ingest, SQLite-Ablage, Influx-Schreibpfad und `cmd/set`-Egress-Audit
- der Fake-Master-Harness und die getrennte `[SIM]`-Dashboard-Seite sind fuer `sim_*` lokal belegt, bleiben aber ausdruecklich `simulation` und kein Hardware-Nachweis
- `bat_sen_01` ist auf realem Board jetzt lokal bis ADC-/Handshake-Basis, offiziell bis Broker/SQLite/Influx und zusaetzlich in einem fokussierten Langlauf belegt; offen bleiben die bewusste Profildeutung von `battery_pct`, spaeterer Deep-Sleep und Mehrgeraetebetrieb
- optionale `net_sen`-/`bat_sen`-Module jenseits dieser Referenzpfade koennen code- und build-seitig vorbereitet sein, sind ohne separate Realbelege aber weiterhin offen
- kuenftige `net_erl`-/`net_zrl`-Sondermodule sind als Ableitungen der Basistypen gewollt, aber aktuell grossenteils nur architektonisch beschrieben und nicht hardwarebelegt

## Sprint-Abnahme
1. Die Basisfamilien und ihre zulaessigen Erweiterungen sind im Repo klar dokumentiert, ohne neue Basisklassen oder Sensor-Spielwiesen zu erfinden.
2. Shared Docs trennen sauber zwischen `nicht vorgesehen`, `architekturvorbereitet` und `compile/stub vorbereitet`; `docs/14_test_und_nachweisstand.md` bleibt fuer `build`, `simulation` und `real hardware`.
3. Setup-/Provisionierungs- und Reset-Regeln sowie Server-Authority sind dokumentiert, ohne ihre Firmware-Implementierung als erledigt zu behaupten.
4. Die naechste Realprioritaet ist klar: integrierte Hardware-Wellen fuer begruendete Modulgruppen und Mehrgeraetebetrieb, nicht neuer Serverballast oder weitere Monolith-Hacks.
