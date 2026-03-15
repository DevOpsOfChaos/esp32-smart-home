# Sondermodule und ihre Basistyp-Ableitung

## Rolle dieser Datei
Diese Datei beschreibt die aktuell gewollten Sondermodule und ordnet sie den vorhandenen Basisfamilien zu.

Nicht diese Datei:
- kein Nachweisdokument
- kein Versprechen, dass alle genannten Module bereits Firmware oder Hardware haben

## Harte Ableitungsregel
- Sondermodule wachsen aus vorhandenen Basisfamilien.
- Ableitung geschieht ueber Featurewahl, Pin-Mapping und Profile.
- Es gibt keine neue Basisklasse pro Sondermodul.

## Aktuelle Sondermodule

| Sondermodul | Basistyp | Geplanter Kern | Bewusst offen / nicht vortaeuschen | Stand jetzt |
|---|---|---|---|---|
| Kuechenlicht | `net_erl` | Relais-Lichtsteuerung plus Praesenz per bevorzugtem `HLK-LD2410C`, `HLK-LD2410B` nur als Alt-/Testvariante; Feuchte/Temperatur ueber `BME280` oder `SHT41`; Luftqualitaet ueber `ENS160`; Lichtwert ueber `VEML7700`, der Auto-Ein bei hellem Raum unterdruecken darf; 17er NeoPixel-Ring fuer spaetere Luftqualitaets-/Wertanzeige und einfache Animation | kein fertiges Sensor-Set entschieden, kein Radar-/Licht-/AQ-Hardwarebeleg, kein LED-Ring-Beweis; der gemeinsame Presence-Unterbau ersetzt noch keinen `net_erl`-Integrationspfad | `net_erl`-Basis real belegt; eigenes Kuechenlicht-Profil und Ring-/Pinpfad jetzt compile-vorbereitet, aber noch ohne Sensor-/Praesenzintegration oder Hardwarebeleg |
| Rolladensteuerung | `net_zrl` | zwei Relais, 3 Touch-Buttons, 2 Zusatz-LEDs fuer Auf/Ab, spaetere Temperatur-/Feuchtesensorik auf einzelnen Modulen | Cover-/Positionslogik, Kalibrierung und Reifegrad nicht vortaeuschen | `net_zrl`-Basis real belegt; eigenes Rolladen-Profil mit aktivem Cover-Modus jetzt compile-vorbereitet, Touch/LED/Sensorik weiter offen |
| Flurlicht | `net_erl` | Relais-Lichtsteuerung, Temperatur/Feuchte, optionaler Lichtwert, PIR-basierte Logik als pragmatische Radar-Annaeherung | PIR erkennt Stillstand schlechter als Radar; diese Grenze bleibt offen benannt | `net_erl`-Basis real belegt; eigenes Flurlicht-Profil jetzt compile-vorbereitet, PIR-/Licht-/Umwelterweiterung weiter ohne Hardwarebeleg |
| Fensterkontakt | `bat_sen` | Deep-Sleep, Reed-Kontakt, sonst bewusst schlank | keine unnoetige Zusatzsensorik | `bat_sen`-Batteriepfad real belegt; eigenes Fensterkontakt-Profil jetzt compile/stub vorbereitet, Deep-Sleep-Rueckkehr offen |
| Tuerkontakt | `bat_sen` | wie Fensterkontakt: Deep-Sleep plus Reed | keine unnoetige Zusatzsensorik | `bat_sen`-Batteriepfad real belegt; eigenes Tuerkontakt-Profil jetzt compile/stub vorbereitet, Deep-Sleep-Rueckkehr offen |
| Regensensor-Node | `bat_sen` | Deep-Sleep plus analoger Regensensor | keine breite Dauersensorik darueber hinaus | `bat_sen`-Batteriepfad real belegt; eigenes Regen-Profil jetzt compile/stub vorbereitet |
| Wandschalter | `bat_sen` | Deep-Sleep, zwei Varianten: 2 mechanische Buttons oder 2 Touch-Buttons; spaeter optional mehr Buttons fuer Dashboard-/Server-Automationen | Touch-Zuverlaessigkeit nicht vortaeuschen; Wakeup-/Debounce-Konzept bleibt eigene Restarbeit | eigenes Wandschalter-Profil jetzt compile/stub vorbereitet; mechanische Button-Grundlage bleibt ohne Hardwarebeleg, Touch weiter nur architekturvorbereitet |
| Zweiter Master | `master` | weiterer Bridge-Knoten derselben Familie, kein neuer Node-/Serverpfad | Mehrgeraetebetrieb, Provisionierung und Rollenabgrenzung offen | einzelner `master` real belegt; zweiter `master` jetzt als eigenes Profil compile-vorbereitet, aber fachlich weiter ohne Mehrgeraete-Nachweis |
| `net_sen`-Umweltknoten | `net_sen` | breitere Umwelt- und Luftqualitaetssensorik; spaeter optional Display, aber nicht zwingend | Display ist kein Muss; Sensorwahl bleibt diszipliniert statt Alles-auf-ein-Board; der LD2410-UART-Pfad bleibt trotz gemeinsamer Presence-Schicht fachlich offen | `net_sen` mit DHT22 real belegt; eigener Umweltknoten als Profil mit `BME280`-/`ENS160`-Stubpfad jetzt compile-vorbereitet, weitere Sensorik weiter offen |

## Leitplanken fuer die Umsetzung
- Nicht jedes Sondermodul braucht jeden Sensor aus der Gesamtlandschaft.
- `net_erl` und `net_zrl` bekommen Zusatzsensorik nur dann, wenn ein konkretes Modul sie wirklich braucht.
- `bat_sen` bleibt fuer tiefe Schlaf- und Eventpfade knapp und stromsparend.
- `master` bleibt Bridge und wird nicht zur Sensor- oder UI-Spielwiese.
- Eigene Sondergeraete sollen zuerst als benannte Profile und Pin-/Provider-Pfade sichtbar werden, bevor reale Pins oder Produktreife behauptet werden.
- Stub- und Scaffold-Pfade sind Vorbereitung, keine Hardwareaussage.
