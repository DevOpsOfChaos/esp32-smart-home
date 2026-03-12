# CURRENT_SPRINT

## Sprintziel
Gemeinsame Doku nach dem Fake-Master-Serverlauf, der getrennten `[SIM]`-Dashboard-Sicht und dem neuen `bat_sen`-ADC-Stand repo-weit widerspruchsfrei halten, ohne Simulation zu Hardware umzudeuten.

## Im Fokus
- offizielles Projektgedaechtnis fuer neue Chats eindeutig machen
- `simulation` fuer `sim_*` oberhalb der MQTT-Grenze explizit von `real hardware` trennen
- den `bat_sen`-Stand ehrlich halten: ADC-Messpfad und explizite Batterieprofile sind im Code da, reale Validierung am echten Geraet aber nicht
- den naechsten echten Blocker als `bat_sen`-Hardwarevalidierung sichtbar halten

## Aktuell nicht im Fokus
- Ausbau der `sim_*`-Serverpfade ueber die bestehende Ingest-/Dashboard-Validierung hinaus
- neue Firmware-Features ausserhalb des bereits implementierten `bat_sen`-Pfads
- neue Server-Features
- MQTT-Vertragsaenderungen
- Architekturumbauten

## Belegter Projektstand, auf dem dieser Sprint aufsetzt
- `master <-> net_erl_01` und `master <-> net_zrl_01` sind mit offiziellem Node-RED-Rueckweg, ACK/Retry, Offline-Timeout und Langzeitstabilitaet im Repo belegt
- `net_sen_01` ist mit realem DHT22 auf `GPIO6` bis MQTT `meta/status/state`, SQLite und Influx offiziell belegt
- die Server-V1 hat reale lokale Nachweise fuer MQTT-Ingest, SQLite-Ablage, Influx-Schreibpfad und `cmd/set`-Egress-Audit
- der Fake-Master-Harness und die getrennte `[SIM]`-Dashboard-Seite sind fuer `sim_*` lokal belegt, bleiben aber ausdruecklich `simulation` und kein Hardware-Nachweis
- `bat_sen` hat im aktuellen Firmware-Stand einen ADC-basierten Batteriepfad auf `GPIO4` und explizite Batterieprofile, aber noch keinen Realnachweis am eigentlichen Geraet
- offen bleiben vor allem `bat_sen`-Hardwarevalidierung, weitere offizielle Pfade jenseits der belegten Pilotpfade, Mehrgeraetebetrieb und ein vollstaendiger Gesamtprojektnachweis

## Sprint-Abnahme
1. Ein neuer Chat findet Einstieg und Projektgedaechtnis ohne Rueckfragen.
2. Shared Docs und `docs/14_test_und_nachweisstand.md` trennen `simulation`, Implementierungsstand und `real hardware` ohne Widerspruch.
3. Der naechste echte Blocker `bat_sen` auf realer Hardware ist explizit benannt statt weichformuliert.
