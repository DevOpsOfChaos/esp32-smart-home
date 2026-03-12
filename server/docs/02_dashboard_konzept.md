# Dashboard-Konzept V1

## Seiten

- Übersicht
- Räume
- Geräte
- Sensoren
- Aktoren
- Rollläden
- Wetter
- Diagramme
- Automationen
- Konfiguration
- Ereignisse/Logs
- `[SIM]`-Validierung als technischer Sonderpfad

## V1-Ziel

- neue Geräte sofort sichtbar
- responsive Grundstruktur für Handy und Desktop
- Seitenwechsel primär über das FlowFuse-Seitenmenü
- Gerätedetail nur als kontextuelle Unterseite, nicht als Hauptnavigation
- manuelle Raum- und Wetterpflege direkt im Dashboard
- gerätetypische Bedienung statt generischer Sammel-Controls
- `sim_*`-Serverdaten separat sichtbar machen, ohne reale Pilot-IDs in dieselbe Sicht zu ziehen

## Bedienregeln

- `net_zrl` zeigt Rollladen-Bedienung nur im echten Rollladenmodus
- Geräte mit Relaisausgängen zeigen sonst Relais-/Schalt-Controls
- die Konfigurationsseite ist ein ehrlicher Hub
- serverseitig schreibbar ist aktuell das Wetter-Setup
- geräteseitig schreibbar sind aktuell Anzeigename und Raumzuordnung
- gemeldete `cfg/report`-Werte bleiben sichtbar, aber read-only

## Bewusst offen

- Feinschliff der UI
- historische Diagramme aus Influx-Abfragen
- echte Raumauswahl-Widgets statt technischer Textfelder
- weitergehende Server-Settings mit verdrahteter Live-Wirkung
