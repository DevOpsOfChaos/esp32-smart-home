# Dashboard Konzept V1

## Seiten

- Uebersicht
- Raeume
- Geraete
- Sensoren
- Aktoren
- Rollaeden
- Wetter
- Diagramme
- Automationen
- Konfiguration
- Ereignisse/Logs
- [SIM] Validierung

## V1-Ziel

- neue Geraete sofort sichtbar
- responsive Grundstruktur fuer Handy und Desktop
- manuelle Raum- und Wetterpflege direkt im Dashboard
- technische MQTT-Aktionen sichtbar, aber bewusst generisch
- `sim_*`-Serverdaten fuer Testlaeufe separat sichtbar machen, ohne reale Pilot-IDs in dieselbe Sicht zu ziehen

## Bewusst offen

- Feinschliff der UI
- historische Diagramme aus Influx-Abfragen
- echte Raumauswahl-Widgets statt technischer Textfelder
- semantische Geraete-Controls pro Geraetetyp
