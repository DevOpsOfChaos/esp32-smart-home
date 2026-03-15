# ShSensors

Kleine sensornahe Bausteine und Provider-Scaffolds fuer `net_sen` und `bat_sen`.

Aktueller Zweck:
- compile-time Auswahl zwischen belegten Referenzpfaden und klar markierten Stub-Providern
- gemeinsame Sentinel-Werte, Cap-Berechnung und State-Helfer fuer optionale Module
- gemeinsame Presence-Provider-Grundlage fuer PIR und spaetere LD2410-Varianten
- gemeinsame I2C-Bus-Helfer fuer Scan, Registerzugriff und Recovery bei vorbereiteten Sensorpfaden
- additive Vorbereitung fuer spaetere Hardwaretests ohne neue Geraetearchitektur

Provider-Familien in diesem Stand:
- Umgebung: `dht22` als belegter Basispfad sowie vorbereitete `bmp280`, `bme280`, `sht41` plus Stubs
- Licht: vorbereiteter `veml7700` plus Stub
- Luftqualitaet: vorbereiteter `ens160` plus Stub

Wichtig:
- Stub-Provider sind nur Entwicklungs- und Build-Scaffolds
- sie sind kein Hardware-Nachweis und kein stiller Ersatz fuer reale Messpfade
- die realen I2C-Pfade sind Architekturvorbereitung mit Fehler-/Warmup-Handling, nicht der Beleg fuer reale Feldhardware
- der LD2410-UART-Pfad ist in dieser Schicht nur konzeptionell vorbereitet; ein Parser ist damit noch nicht behauptet
