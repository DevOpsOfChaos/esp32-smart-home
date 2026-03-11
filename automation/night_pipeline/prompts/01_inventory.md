Arbeite nur im Worktree dieser Stufe.

Lies zuerst:
- AGENTS.md
- docs/PROJECT_CONTEXT.md
- docs/CURRENT_SPRINT.md
- docs/TASK_QUEUE.md
- docs/DECISIONS.md
- README.md

Altbestand:
__LEGACY_ROOT__

Zielrepo:
__TARGET_REPO__

Aufgabe:
Inventarisiere den gesamten Altbestand. Bearbeite in dieser Stufe keine Produktions-Firmware und keine Serverlogik des Zielrepos.

Ziele:
1. Erstelle unter migration/00_inventory/ eine vollständige Altbestands-Landkarte.
2. Erfasse:
   - relevante Ordner
   - Firmware-/Server-/Tool-/Doku-Bereiche
   - Basistyp-Kandidaten
   - Sondergeräte
   - alte Doku/Konzepte
3. Erzeuge:
   - migration/00_inventory/index.md
   - migration/00_inventory/file_map.csv
   - migration/00_inventory/device_candidates.md
4. Kennzeichne klar:
   - Basistyp-nah
   - Sondergerät-nah
   - allgemeine Helper
   - Doku/Konzepte
   - wahrscheinlich veraltet oder unbrauchbar

Harte Regeln:
- Nur Inventar
- Nichts integrieren
- Keine Architekturentscheidungen neu erfinden
- Keine Fake-Behauptungen
- Wenn Pfade/Dateien fehlen oder unlesbar sind, sauber markieren und weitermachen

Am Ende:
- schreibe einen kurzen belastbaren Stand in migration/reports/01_inventory_summary.md
- committe nur die Ergebnisse dieser Stufe