# ShStorage

Persistenz, NVS und gemeinsamer Konfigurationsspeicher fuer Nodes.

Aktueller Stand:
- gemeinsamer Blob fuer Master-MAC, Anzeigename und einfache Basiswerte
- Factory-Reset ueber gemeinsames Loeschen des Setup-Blob
- Hilfsfunktionen fuer MAC-Parsing und kontrollierte Defaults

Bewusst noch nicht enthalten:
- breite Migrationshistorie
- per-Geraet-Sonderdatenbanken
- Server- oder MQTT-Konfiguration in Nodes
