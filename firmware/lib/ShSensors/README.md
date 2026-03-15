# ShSensors

Kleine sensornahe Bausteine und Provider-Scaffolds fuer `net_sen` und `bat_sen`.

Aktueller Zweck:
- compile-time Auswahl zwischen belegten Referenzpfaden und klar markierten Stub-Providern
- gemeinsame Sentinel-Werte, Cap-Berechnung und State-Helfer fuer optionale Module
- gemeinsame Presence-Provider-Grundlage fuer PIR und spaetere LD2410-Varianten
- additive Vorbereitung fuer spaetere Hardwaretests ohne neue Geraetearchitektur

Wichtig:
- Stub-Provider sind nur Entwicklungs- und Build-Scaffolds
- sie sind kein Hardware-Nachweis und kein stiller Ersatz fuer reale Messpfade
- der LD2410-UART-Pfad ist in dieser Schicht nur konzeptionell vorbereitet; ein Parser ist damit noch nicht behauptet
