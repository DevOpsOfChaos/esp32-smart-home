# BAT_SEN ADC Fact Gap

Stand: `2026-03-12`

## Scope

Geprueft wurden in diesem engen Nachzug nur diese aktiven Bereiche:

- `firmware/src/bat_sen/`
- `firmware/src/master/`
- `firmware/include/`
- `docs/BAT_SEN_ADC_FACT_GAP.md`
- `docs/14_test_und_nachweisstand.md`
- `PROTOKOLL/` mit den `bat_sen`-relevanten Staenden `beta33`, `beta35`, `beta36`, `beta38`, `beta39`

## 1. Was lokal fuer `bat_sen_01` jetzt real erledigt ist

- `COM7` ist der reale `bat_sen`, `COM3` der reale `master`.
- `PROTOKOLL/beta38_bat_sen_01_realer_handshake_com3_com7_hello_ack_state_report.txt` belegt real:
  - `HELLO`
  - `HELLO_ACK`
  - erster `STATE_REPORT`
  - Verarbeitung dieses ersten `STATE_REPORT` im `master`
- `PROTOKOLL/beta36_bat_sen_2v9_realer_profilvergleich_und_mastercheck_com3_com7.txt` hat den lokalen Batteriepfad bereits gegen die reale Batteriespannung plausibel gemacht.
- `PROTOKOLL/beta39_bat_sen_01_realer_gpio4_teilerknotencheck_und_v1_reststand.txt` zieht den direkten GPIO4-Teilerknotenvergleich als fuer V1 erledigt nach.
- Der aktive Firmwarepfad ist in den geprueften Dateien konsistent:
  - `GPIO4` als fester Batterie-ADC-Pin
  - `100k / 100k`-Teiler
  - `analogReadMilliVolts()` mit `BATTERY_ADC_SAMPLE_COUNT = 4`
  - `ADC_11db`
  - explizite Profile `coin_3v_primary`, `alkaline_2x`, `liion_1s`

## 2. Was nicht mehr als offene Grundsatzluecke bezeichnet werden darf

- der lokale Handshake `HELLO -> HELLO_ACK`
- der erste lokale `STATE_REPORT`
- die Frage, ob `GPIO4` ueberhaupt der reale Batteriepfad ist
- der lokale V1-Plausibilitaetscheck des GPIO4-Teilerknotens gegen den seriellen ADC-Wert

## 3. Was ehrlich weiter offen bleibt

- die endgueltige reale Batterieprofilwahl kann weiterhin offen sein
- der offizielle `bat_sen_01`-Servernachweis ueber Broker, SQLite und Influx fehlt weiter
- die Langzeitvalidierung fuer `bat_sen_01` fehlt weiter
- Deep-Sleep-Rueckkehr und Mehrgeraetebetrieb sind in diesem engen V1-Finish bewusst nicht Teil der Aussage

## 4. Ergebnis der Firmware-Pruefung

- Im inspizierten `bat_sen`-/`master`-Pfad ist vor dem offiziellen Servernachweis kein weiterer klarer V1-Grundbaustein als "fehlend" erkennbar.
- Die explizite Profilbehandlung ist bereits vorhanden. Offen ist nicht die Implementierung, sondern die bewusste Wahl des richtigen Profils fuer das reale Board.
- `LOW_BATTERY_PCT` ist im aktuellen Pfad nur eine Konfigurationskonstante; ein eigener `battery_low`-Vertrag ist dort nicht vorhanden. So etwas jetzt einzuziehen waere kein kleiner V1-Finish, sondern ein Nachrichten-/Schema-Eingriff.
- Die verbleibenden Restpunkte sind deshalb vor allem Evidenz und bewusste Profilentscheidung, nicht ein noch fehlender lokaler Handshake- oder ADC-Codepfad.

## 5. Kleinste ehrliche Naechstschritte

1. Falls die reale Board-Chemie inzwischen feststeht, das Profil in `firmware/src/bat_sen/AppConfig.h` bewusst festziehen. Falls nicht, diese Frage offen lassen statt einen Default als Wahrheit zu verkaufen.
2. Den offiziellen realen `bat_sen_01`-Serverpfad schmal nachweisen: `meta/status/state` ueber Broker bis SQLite und Influx.
3. Danach den laengeren Realbetrieb fuer `bat_sen_01` nachziehen.
