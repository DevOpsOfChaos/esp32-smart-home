# DECISIONS

## D-001 Neubasis statt Altprojekt-Fortsetzung
Entscheidung:
Das Projekt wird als Neubasis aufgebaut und nicht als direkte Fortsetzung des Altprojekts.

Grund:
Der Altbestand enthaelt zu viele vermischte Altannahmen, Projektreste und technische Altlasten. Eine saubere, veroeffentlichbare Basis ist wichtiger als scheinbar schneller Uebernahmegewinn.

## D-002 Lokales Repo ausserhalb OneDrive
Entscheidung:
Das aktive Git-Repository liegt lokal unter `Documents\Playground`, nicht in OneDrive.

Grund:
OneDrive ist fuer Begleitdateien brauchbar, aber als aktives Git-Arbeitsverzeichnis stoeranfaellig. Das zentrale Versionssystem ist GitHub.

## D-003 Nodes ohne MQTT
Entscheidung:
Nodes sprechen kein MQTT.

Grund:
Die Trennung zwischen Funkebene und Serverebene bleibt nur dann stabil, wenn MQTT ausschliesslich am Master terminiert. Sonst wird jede Node zum Server-Sonderfall.

## D-004 net_zrl als allgemeine Basis, Cover-Logik nur im Cover-Modus
Entscheidung:
`net_zrl` bleibt ein allgemeiner Zwei-Relais-Node. Rolladen- oder Cover-Logik wird nur ueber einen klaren Cover-Modus aktiviert.

Grund:
Die Basisklasse muss wiederverwendbar bleiben. Sonderlogik in der Basis wuerde den Lampenmodus und andere Einsaetze unnoetig verkomplizieren.

## D-005 Kleine Arbeitspakete fuer Codex statt Grossauftraege
Entscheidung:
Codex bekommt kleine, klar abgegrenzte Arbeitspakete statt breiter Sammelauftraege.

Grund:
Kleine Pakete halten Architektur, Review und Protokollierung kontrollierbar. Grosse Pakete erzeugen Kontextverlust, halbfertige Umbauten und schlechte Rueckverfolgbarkeit.

## Pflegehinweis
Neue Architektur- oder Arbeitsentscheidungen hier kurz mit Entscheidung und Grund ergaenzen. Keine langen Rechtfertigungen, nur belastbare Regeln.
