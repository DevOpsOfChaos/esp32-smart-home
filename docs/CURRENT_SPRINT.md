# CURRENT_SPRINT

## Aktueller Fokus
Erste vertikale Minimalstrecke fuer einen netzbetriebenen Relais-Node:

- `net_erl -> master -> MQTT -> master -> net_erl`
- wenige, klar pruefbare Nachrichten statt breiter Featureflaeche
- Fokus auf belastbare Grundkommunikation vor Komfortfunktionen

## Aktuell nicht im Fokus
- `net_zrl`
- Cover-Modus
- grosse Dashboard- oder Server-Erweiterungen

## Aktueller Abnahmetest
Die Minimalstrecke gilt erst dann als belastbar, wenn diese Punkte sauber nachgewiesen sind:

1. `HELLO`
2. `HEARTBEAT`
3. `STATE_REPORT`
4. `set_relay`
5. Offline-Timeout

## Technischer Hinweis
Build- und Testbasis ist noch nicht stabil. Neue Arbeit soll zuerst die Minimalstrecke und ihre Nachweisbarkeit absichern, nicht den Funktionsumfang aufblasen.
