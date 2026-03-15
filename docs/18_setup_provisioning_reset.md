# Setup, Provisionierung und Reset fuer Nodes

## Rolle dieser Datei
Diese Datei definiert den gemeinsamen Setup-, Provisionierungs- und Resetvertrag fuer `net_erl`, `net_zrl`, `net_sen` und `bat_sen`.

Nicht diese Datei:
- `14_test_und_nachweisstand.md` = belegt nicht automatisch, dass dieser Vertrag schon implementiert ist
- `19_sondermodule_roadmap.md` = beschreibt konkrete kuenftige Module

## Geltungsbereich und Ehrlichkeit
- Der Vertrag gilt fuer Nodes, nicht fuer den `master`.
- Der Vertrag ist eine verbindliche Architekturvorgabe.
- Im aktuellen Repo-Stand ist dieser Pfad noch nicht als realer Firmware-Nachweis zu behandeln.

## Verbindliche Trigger

| Ausloeser | Pflichtverhalten | Datenwirkung | Reifestand jetzt |
|---|---|---|---|
| keine gespeicherte Master-MAC | automatischer Setup-Modus | noch nichts gespeichert | build-validiertes Grundgeruest |
| Boot-Taster 5 Sekunden halten | Setup-Modus erneut oeffnen | gespeicherte Daten bleiben erhalten und werden in der kleinen Setup-Form vorbefuellt angezeigt | build-validiertes Grundgeruest |
| Boot-Taster 10 Sekunden halten | Factory Reset und danach Setup-Modus | gespeicherte Master-Bindung und relevante lokale Einstellungen werden geloescht | build-validiertes Grundgeruest |

## Aktueller Repo-Stand
- Das gemeinsame Grundgeruest liegt jetzt in `firmware/lib/ShNodeBase/` und `firmware/lib/ShStorage/`.
- Persistiert werden aktuell:
  - Master-MAC-Bindung
  - Anzeigename des Nodes
  - einfacher Report-Intervall-Basiswert
  - einfacher Wake-/Sleep-Intervall-Basiswert
- Der Setup-Pfad startet einen kleinen lokalen AP mit schlichter Webform.
- `master_mac` kann per GET-Parameter vorbefuellt werden, wird aber erst nach sichtbarer Pruefung und Speichern uebernommen.
- Die Boot-Taster-Haltezeiten sind in den betroffenen `AppConfig.h`-Dateien sichtbar auf `5 s` bzw. `10 s` festgelegt.
- Der Stand ist bewusst nur `build`-/Groundwork-Niveau:
  - kein Captive-Portal oder DNS-Hijack
  - keine ausgebauten per-Geraet-Sonderseiten
  - keine reale Hardwarevalidierung dieses Setup-Pfads im Repo
  - Report-/Wake-Werte werden schon gespeichert, aber noch nicht vollstaendig als Laufzeitkonfiguration genutzt

## Setup-Modus
- Setup-Modus bedeutet: gehostetes lokales WLAN plus einfacher lokaler Webserver.
- Die Setup-Seite darf per QR-Code-Link bzw. GET-Parametern geoeffnet werden.
- Eine uebergebene Master-MAC muss geparst, sichtbar angezeigt und vor dem Speichern bestaetigt werden.
- Relevante Basisfelder duerfen mit Defaults oder bekannten Werten vorbefuellt sein.
- Nach dem Speichern wird die Master-MAC persistent abgelegt und das Geraet startet in den Normalmodus.

## Bewusst nicht Ziel des Setup-Modus
- keine MQTT-Konfiguration in Nodes
- kein zweites Dashboard
- keine grosse Logik- oder Regel-UI
- keine per-Sondergeraet auswuchernde Expertenoberflaeche

Der Setup-Pfad dient der Inbetriebnahme und Grundwartung, nicht dem Nachbauen der Serveroberflaeche.

## Re-Entry und Reset
- `5 s` Boot-Haltezeit bedeutet Rueckkehr in den Setup-Modus ohne automatisches Loeschen aller Werte.
- `10 s` Boot-Haltezeit bedeutet echter Factory Reset.
- Nach Factory Reset muss das Geraet wieder wie ein ungebundener Node starten und deshalb direkt in den Setup-Modus gehen.
- Der Setup-Modus nach Re-Entry soll die bisherigen Felder vorbefuellt zeigen, damit Wartung nicht zur Neubestueckung aller Werte wird.

## `bat_sen`-Spezifika
- Die Setup-Seite muss mindestens das Deep-Sleep-Intervall editierbar machen.
- Falls spaeter Zwischen-Wakeups nur fuer `HELLO`/Keepalive zulaessig sind, duerfen sie als optionale Einstellung erscheinen.
- Benutzerseitige Einheiten sind Minuten und Stunden, nicht Millisekunden.
- Die aktuelle Grundform speichert den Wake-/Sleep-Wert bereits in Minuten/Stunden-Eingabe, nutzt ihn aber noch nicht als fertigen Deep-Sleep-Runtime-Pfad.
- Deep-Sleep selbst ist damit nicht automatisch als implementiert oder belegt zu betrachten.

## Lokale Bedienung und Server-Autoritaet
- Jedes Relaisgeraet braucht einen lokalen manuellen Taster zum Toggeln des Relais.
- Lokale PIR-/Radar-/Lichtsensor-Automatik darf Komfort liefern, aber keine Hoheit ueber den Zielzustand beanspruchen.
- Der Server bzw. das Dashboard muss lokale Automatik jederzeit uebersteuern koennen.
- Ein lokal automatisch eingeschaltetes Licht muss serverseitig wieder ausschaltbar bleiben.

## Architekturgrenzen bleiben trotzdem hart
- Nodes sprechen weiterhin nur ESP-NOW.
- MQTT bleibt ausschliesslich zwischen `master` und `server`.
- Der Setup-Modus ist ein lokaler Wartungspfad, kein Seiteneinstieg fuer direkte Server-Node-Kommunikation.
