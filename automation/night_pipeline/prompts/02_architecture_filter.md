Arbeite nur im Worktree dieser Stufe.

Lies zuerst:
- AGENTS.md
- docs/PROJECT_CONTEXT.md
- docs/CURRENT_SPRINT.md
- docs/TASK_QUEUE.md
- docs/DECISIONS.md
- docs/04_mqtt_topics.md
- migration/00_inventory/index.md
- migration/00_inventory/device_candidates.md

Altbestand:
__LEGACY_ROOT__

Zielrepo:
__TARGET_REPO__

Aufgabe:
Klassifiziere den Altbestand nach Wiederverwendbarkeit für die neue Architektur.

Erstelle:
- migration/01_reuse_matrix/reuse_matrix.md
- migration/01_reuse_matrix/reuse_matrix.csv

Kategorien:
- direkt wiederverwendbar
- teilweise wiederverwendbar
- nur als Referenz
- verwerfen

Bewerte besonders:
- allgemeine Helper
- Sensorpfade
- Config-/Persistenzlogik
- Serialisierung / Parsing
- Retry-/Duplikatlogik
- Sondergeräte
- Altarchitektur-Reste

Harte Regeln:
- Noch nichts direkt integrieren
- Nicht weichspülen
- Altmonolithen klar als Altlast markieren
- Sondergeräte nicht blind übernehmen; stattdessen einordnen als:
  - eigener Gerätetyp verwerfen
  - Betriebsmodus
  - Featurepaket
  - reine Altlast

Am Ende:
- migration/reports/02_architecture_filter_summary.md
- committe nur die Ergebnisse dieser Stufe