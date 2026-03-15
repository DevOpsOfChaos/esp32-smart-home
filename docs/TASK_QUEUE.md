# TASK_QUEUE

## Prioritaet jetzt
1. Minimale Firmware-Umsetzung des dokumentierten Node-Setup-/Provisionierungsvertrags vorbereiten, ohne die belegten Referenzpfade zu destabilisieren:
   lokale Setup-AP plus Webserver, Master-MAC-Speicherung, Re-Entry ueber Boot-Haltezeit, Factory-Reset-Regel
2. Erste integrierte Umwelt-/Praesenz-/Licht-Hardwarewelle an den passenden Basisfamilien planen und nur dort real pruefen, wo die Matrix es vorsieht:
   `BME280`/`BMP280`, `ENS160`, `SHT41`, `VEML7700`, `PIR`, `HLK-LD2410C`, `HLK-LD2410B`
3. Naechste `bat_sen`-Welle gebuendelt und ehrlich angehen:
   reed fuer Fenster/Tuer, analoger Regensensor, mechanische und spaetere Touch-Wandschalter-Varianten, Deep-Sleep-Rueckkehr
4. `net_zrl` nur kontrolliert zum Rolladen-Sondermodul ausbauen:
   3 Touch-Buttons, 2 Richtungs-LEDs, spaetere Temperatur-/Feuchtesensorik; Cover-/Positionslogik erst mit bewusstem Realnachweis
5. Weitere offizielle Nachweise jenseits der belegten Einzelpfade `net_erl_01`, `net_zrl_01`, `net_sen_01`, `bat_sen_01` und der getrennten `sim_*`-Servervalidierung bevorzugt als groessere zusammenhaengende Realtests statt weiterer Mikrozerlegung aufbauen

## Danach
6. `bat_sen`-Restpunkt sauber halten: der reale Spannungspfad ueber `GPIO4` gilt als belegt; offen bleiben die boardkonkrete Profilwahl fuer `battery_pct` und spaetere Deep-Sleep-Rueckkehr erst nach bewusstem Sleep-Pfad
7. Zweiten `master` erst als reale Erweiterung angehen, wenn Provisionierung, Rollenabgrenzung und Mehrgeraetebetrieb sauberer sind
8. Test- und Nachweisdoku nur gegen neue reale Belege oder klar getrennte `build`-/`simulation`-Laeufe nachziehen
9. Erst danach wieder weiterer Feature-Ausbau oder breitere Serverarbeit

## Arbeitsregel fuer die Queue
- Nur reale Luecken aufnehmen.
- Punkte erst streichen, wenn das zugehoerige Dokument oder der Nachweis wirklich im Repo liegt.
- PROTOKOLL-Dateien nicht rueckwirkend ueberschreiben.
