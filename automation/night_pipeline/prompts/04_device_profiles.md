Arbeite nur im Worktree dieser Stufe.

Lies zuerst:
- AGENTS.md
- docs/PROJECT_CONTEXT.md
- docs/DECISIONS.md
- migration/01_reuse_matrix/reuse_matrix.md
- migration/extracted_helpers/index.md

Aufgabe:
Erstelle eine Migrationssicht für alle Basistypen und Sondergeräte.

Erstelle:
- migration/device_profiles/master.md
- migration/device_profiles/net_erl.md
- migration/device_profiles/net_zrl.md
- migration/device_profiles/net_sen.md
- migration/device_profiles/bat_sen.md
- migration/special_devices/index.md

Für jedes Profil:
- relevante Altbausteine
- direkt nutzbare Helper
- nicht übernehmbare Altlasten
- sinnvolle nächste Integrationsschritte

Für Sondergeräte:
- pro Sondergerät kurzer Eintrag:
  - was ist es
  - was funktioniert daran bereits
  - was ist für neue Architektur brauchbar
  - ist es eigener Typ, Modus, Featurepaket oder Altlast