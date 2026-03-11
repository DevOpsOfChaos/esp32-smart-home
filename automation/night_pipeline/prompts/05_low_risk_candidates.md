Arbeite nur im Worktree dieser Stufe.

Lies zuerst:
- AGENTS.md
- docs/PROJECT_CONTEXT.md
- docs/CURRENT_SPRINT.md
- docs/TASK_QUEUE.md
- docs/DECISIONS.md
- migration/extracted_helpers/index.md
- migration/device_profiles/*.md
- migration/special_devices/index.md

Aufgabe:
Bereite nur Low-Risk-Integrationskandidaten vor.

Erstelle:
- migration/reports/05_low_risk_candidates.md

Liste nur Bausteine, die:
- architekturverträglich sind
- morgen direkt Zeit sparen
- kein großes Refactoring erzwingen
- nicht auf alter Monolithlogik beruhen

Optional:
- wenn ein Kandidat wirklich harmlos und klar ist, darfst du einen vorbereitenden Patch im Worktree anlegen
- aber nur mit Build-Check und klarer Herkunftsnotiz