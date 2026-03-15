# TASK_QUEUE

## Prioritaet jetzt
1. `bat_sen`-Restpunkt sauber halten: der reale Spannungspfad ueber `GPIO4` gilt als belegt; offen bleiben die boardkonkrete Profilwahl fuer `battery_pct` und spaetere Deep-Sleep-Rueckkehr erst nach bewusstem Sleep-Pfad
2. Naechste integrierte Hardware-Welle fuer die modular vorbereiteten `net_sen`-/`bat_sen`-Optionen planen und nur mit realer Hardware belegen:
   `BMP280`/`BME280`, `ENS160`, `PIR`, `reed`, `button`, `rain`
3. Weitere offizielle Nachweise jenseits der belegten Einzelpfade `net_erl_01`, `net_zrl_01`, `net_sen_01`, `bat_sen_01` und der getrennten `sim_*`-Servervalidierung bevorzugt als groessere zusammenhaengende Realtests statt weiterer Mikrozerlegung aufbauen

## Danach
4. Test- und Nachweisdoku nur gegen neue reale Belege oder klar getrennte `build`-/`simulation`-Laeufe nachziehen
5. Erst danach wieder weiterer Feature-Ausbau oder breitere Serverarbeit

## Arbeitsregel fuer die Queue
- Nur reale Luecken aufnehmen.
- Punkte erst streichen, wenn das zugehoerige Dokument oder der Nachweis wirklich im Repo liegt.
- PROTOKOLL-Dateien nicht rueckwirkend ueberschreiben.
