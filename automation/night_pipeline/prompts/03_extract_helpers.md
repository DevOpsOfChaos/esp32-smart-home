Arbeite nur im Worktree dieser Stufe.

Lies zuerst:
- AGENTS.md
- docs/PROJECT_CONTEXT.md
- docs/DECISIONS.md
- migration/01_reuse_matrix/reuse_matrix.md

Altbestand:
__LEGACY_ROOT__

Zielrepo:
__TARGET_REPO__

Aufgabe:
Extrahiere nur gemeinsame Low-Risk-Helper aus dem Altbestand in einen Migrationsordner.

Erstelle:
- migration/extracted_helpers/README.md
- migration/extracted_helpers/index.md

Wenn sinnvoll, lege unter migration/extracted_helpers/ Unterordner an für:
- config
- validation
- parsing
- timing
- retry_dedup
- sensor_helpers
- serialization

Harte Regeln:
- Noch keine direkte Produktionsintegration
- Keine Architekturverbiegung
- Kein Copy-Paste ohne Herkunftsdokumentation

Jeder extrahierte Baustein braucht:
- Herkunftspfad
- kurze Funktionsbeschreibung
- Architekturverträglichkeit
- Empfehlung:
  - direkt integrierbar
  - später manuell adaptieren
  - nur Referenz