# TASK_QUEUE

## Prioritaet jetzt
1. `bat_sen`-Restpunkt sauber halten: der reale Spannungspfad ueber `GPIO4` gilt als belegt; offen ist nur die boardkonkrete Profilwahl fuer `battery_pct` sowie spaetere Deep-Sleep-Rueckkehr nach bewusst implementiertem Sleep-Pfad
2. Weitere offizielle Nachweise jenseits der belegten Einzelpfade `net_erl_01`, `net_zrl_01`, `net_sen_01`, `bat_sen_01` und der getrennten `sim_*`-Servervalidierung real aufbauen

## Danach
3. Test- und Nachweisdoku nur gegen neue reale Belege oder klar getrennte `simulation`-Laeufe nachziehen
4. Erst danach wieder Feature-Ausbau

## Arbeitsregel fuer die Queue
- Nur reale Luecken aufnehmen.
- Punkte erst streichen, wenn das zugehoerige Dokument oder der Nachweis wirklich im Repo liegt.
- PROTOKOLL-Dateien nicht rueckwirkend ueberschreiben.
