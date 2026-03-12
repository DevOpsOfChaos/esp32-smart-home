# BAT_SEN ADC Fact Gap

Stand: `2026-03-12`

## Scope

Geprueft wurden nur diese Pfade:

- `firmware/src/bat_sen/`
- `docs/`
- `PROTOKOLL/`
- `migration/device_profiles/`
- `migration/special_devices/`
- `worktrees/night_03_extract_helpers/`
- `worktrees/night_04_device_profiles/`
- `worktrees/night_06_morning_report/`

Nicht geprueft wurden bewusst:

- `hardware/bat_sen/`
- andere aktive Firmware-Dateien ausserhalb des freigegebenen Scopes

Wichtig fuer die Einordnung:

- `migration/device_profiles/` und `migration/special_devices/` existieren im aktiven Repo, sind dort aber aktuell leer.
- Die einzigen lesbaren `bat_sen`-Migrationsnotizen liegen damit nur in den genannten Worktrees.
- Massgeblich fuer den aktuellen Produktstand sind jetzt aber die aktiven Dateien unter `firmware/src/bat_sen/`, nicht die Worktree-Notizen.

## 1. Was im aktuellen Repo jetzt wirklich implementiert ist

- `bat_sen` bleibt eine batteriegetriebene ESP-NOW-Node-Basis mit `HELLO`, `HEARTBEAT`, `STATE_REPORT` und `EVENT`.
- `STATE_REPORT` transportiert weiter `battery_pct` und `battery_mv`.
- Der Batteriepfad laeuft jetzt lokal ueber `PIN_BATTERY_ADC` auf `GPIO4`.
- `main.cpp` liest den ADC ueber `analogReadMilliVolts()`, bildet aus `BATTERY_ADC_SAMPLE_COUNT = 4` Samples einen Mittelwert und rechnet diesen ueber den festen Teiler `100k / 100k` wieder auf Batteriespannung hoch.
- `main.cpp` setzt fuer den Batteriepin bewusst `ADC_11db` als konservative V1-Vorgabe.
- `AppConfig.h` definiert jetzt drei explizite Profile:
  - `coin_3v_primary`: `2000..3000 mV`
  - `alkaline_2x`: `2000..3200 mV`
  - `liion_1s`: `3200..4200 mV`
- Die Prozentberechnung ist damit nicht mehr der alte Platzhalter `100% / 3700 mV`, sondern eine lineare Umrechnung innerhalb des aktuell gewaehlten Profils.
- Default im aktiven Repo ist `BAT_PROFILE_COIN_3V_PRIMARY`.
- Deep-Sleep bleibt weiterhin bewusst aus; `SLEEP_INTERVAL_S`, `RX_WINDOW_MS` und `WACH_NACH_EVENT_MS` sind also noch kein real validierter Sleep-Betrieb.
- `PIN_BUTTON_1..4`, `PIN_REED` und `PIN_RAIN_ADC` stehen weiter auf `-1`. Der einzige feste aktive Hardwarepfad ist damit heute die Batteriemessung.

## 2. Was dadurch geklaert ist und was weiter offen bleibt

| Thema | Im aktiven Repo jetzt geklaert | Weiter offen |
|---|---|---|
| Anschlussprinzip | `GPIO4` ist der feste Batterie-ADC-Pin. | Kein Realnachweis am echten `bat_sen`-Board. |
| Spannungsteiler | Im Code ist jetzt `100k / 100k` hinterlegt. | Ob das dem realen Board exakt entspricht, ist im Repo nicht am echten Geraet validiert. |
| ADC-Vorgabe | `ADC_11db` ist in `main.cpp` gesetzt. | Ob diese Vorgabe fuer das reale Board und seine Genauigkeit passt, ist nicht hardwarebelegt. |
| Batterieprofile | `coin_3v_primary`, `alkaline_2x`, `liion_1s` sind explizit definiert. | Welches Profil das echte `bat_sen`-Geraet tatsaechlich braucht, ist noch nicht als Hardwarefakt festgezurrt. |
| Prozentlogik | Prozent wird jetzt aus dem jeweils gewaehlten Profil abgeleitet. | Ob die Werte fuer den realen Einsatzfall und die reale Lastsituation passen, ist offen. |
| Serversicht | `battery_mv` und `battery_pct` sind serverseitig ingestierbar und in der Simulation sichtbar. | Das beweist keine reale `bat_sen`-Hardware und keine echte ADC-Genauigkeit. |

Die grosse Restluecke ist also nicht mehr "es gibt gar keinen ADC-Pfad", sondern:

- passt dieser Codepfad zur echten `bat_sen`-Hardware?
- welches Profil ist fuer das reale Geraet das richtige?
- stimmen `battery_mv` und daraus abgeleitete `battery_pct` gegen reale Messung?

## 3. Was bewusst nicht als Hardware-Nachweis zaehlt

- `PROTOKOLL/beta31_server_sim_fake_master_ingest_harness.txt` ist nur ein serverseitiger Simulationslauf mit `sim_bat_sen_01`.
- `PROTOKOLL/beta32_server_sim_dashboard_validierung.txt` zeigt nur, dass dieselben `sim_*`-Daten getrennt im Dashboard reviewbar sind.
- Beide Dateien pruefen den Server oberhalb der MQTT-Grenze. Sie beweisen weder ein reales `bat_sen`-Board noch einen echten ADC-Pfad auf Hardware.
- Auch der jetzt vorhandene Code in `firmware/src/bat_sen/` ist fuer sich allein noch kein Hardware-Nachweis. Implementiert ist nicht validiert.

## 4. Was die gemeinsame Projektdoku jetzt ehrlich sagen darf

- `bat_sen` hat jetzt im aktiven Firmware-Stand einen implementierten ADC-basierten Batteriepfad.
- `bat_sen` hat jetzt explizite Batterieprofile im Repo und nicht mehr nur einen stumpfen Platzhalterwert.
- Die Serverseite kann Batterie-Felder getrennt ingestieren; die `sim_*`-Validierung zeigt das zusaetzlich als `simulation`.
- Die reale `bat_sen`-Hardwarevalidierung auf dem eigentlichen Geraet fehlt weiter. Genau das bleibt der naechste Blocker.

## 5. Kleinster naechster technischer Schritt

1. Das echte `bat_sen`-Geraet an den aktuellen Firmware-Stand bringen.
2. Das reale Batterieprofil fuer genau diese Hardware bewusst waehlen statt den Default still zu uebernehmen.
3. Batteriespannung und ADC-Knoten `GPIO4` parallel mit Multimeter messen, waehrend die Firmware `battery_mv` sendet.
4. Den ersten offiziellen Lauf schmal halten:
   - Boot
   - `HELLO` / `HELLO_ACK`
   - `STATE_REPORT` mit plausiblen `battery_mv` und `battery_pct`
   - Sichtbarkeit im bestehenden Serverpfad
5. Erst danach die Aussage in `docs/14_test_und_nachweisstand.md` von "implementiert, aber offen" auf echten Hardware-Nachweis anheben.

## 6. Vorbereitete Realvalidierung im Repo

- `firmware/src/bat_sen/main.cpp` gibt fuer den Realtest jetzt zusaetzlich stabile serielle Zeilen aus:
  - `BATTERY_CONFIG ...`
  - `BATTERY_PROFILE ...`
  - `BATTERY_VALIDATION phase=boot ...`
  - `BATTERY_VALIDATION phase=state_report ...`
- `PROTOKOLL/beta33_bat_sen_realhardware_validierung_checkliste.txt` ist die knappe Operator-Checkliste fuer Portwahl, Flash, Serial und Multimetervergleich.
- `PROTOKOLL/beta35_bat_sen_realhardware_validierungsprotokoll_vorlage.txt` ist die ausfuellbare Vorlage fuer den echten Lauf.
- Diese Artefakte bereiten nur den Nachweis vor. Sie sind selbst noch kein realer Hardwarebeleg.

## Kurzfazit

Der Repo-Stand ist besser als vorher und schlechter als man sich leicht einreden koennte.

- Besser: der `bat_sen`-Batteriepfad ist jetzt wirklich im Code implementiert, inklusive expliziter Profile.
- Schlechter: der entscheidende Realnachweis am echten Geraet fehlt weiter.

Die naechste Arbeit ist deshalb nicht noch mehr Doku oder Shared-Refactor, sondern die reale `bat_sen`-Validierung auf Hardware.
