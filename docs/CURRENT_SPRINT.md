# CURRENT_SPRINT

## Sprintziel
Gemeinsame Doku nach dem Fake-Master-Serverlauf, der getrennten `[SIM]`-Dashboard-Sicht und dem jetzt belegten `bat_sen`-Server-/Langlaufstand repo-weit widerspruchsfrei halten, ohne Restluecken zu verschweigen oder funktionierende Pfade wieder kuenstlich offen zu reden.

## Im Fokus
- offizielles Projektgedaechtnis fuer neue Chats eindeutig machen
- Hardware-README-Dateien der fuenf Basisgeraete als kompakte Mindestdoku konsistent halten
- `simulation` fuer `sim_*` oberhalb der MQTT-Grenze explizit von `real hardware` trennen
- den `bat_sen`-Stand sauber trennen: realer `battery_mv`-Pfad ueber `GPIO4`, offizieller Serverpfad und fokussierter Langlauf sind belegt; offen bleiben die boardkonkrete Profilwahl fuer `battery_pct`, spaeterer Deep-Sleep und Mehrgeraetebetrieb
- die naechsten echten Blocker nach dem `bat_sen`-Grundnachweis sichtbar halten

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
- `bat_sen_01` ist auf realem Board jetzt lokal bis ADC-/Handshake-Basis, offiziell bis Broker/SQLite/Influx und zusaetzlich in einem fokussierten Langlauf belegt; der offene Restpunkt liegt bei der Profildeutung von `battery_pct`, nicht beim Spannungsmesspfad
- offen bleiben vor allem bewusste `bat_sen`-Profilwahl fuer das konkrete Board, Deep-Sleep-/Mehrgeraetebetrieb, weitere offizielle Pfade jenseits der belegten Einzelgeraete und ein vollstaendiger Gesamtprojektnachweis

## Sprint-Abnahme
1. Ein neuer Chat findet Einstieg und Projektgedaechtnis ohne Rueckfragen.
2. Shared Docs und `docs/14_test_und_nachweisstand.md` trennen `simulation`, Implementierungsstand und `real hardware` ohne Widerspruch.
3. Die echten Restpunkte nach dem belegten `bat_sen`-Pfad sind explizit benannt: Profilwahl fuer `battery_pct`, spaeterer Deep-Sleep und Mehrgeraetebetrieb statt einer faelschlich offenen Messgrundlage.
