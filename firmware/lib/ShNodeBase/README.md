# ShNodeBase

Gemeinsame Node-Basislogik fuer Setup, Provisionierung und Boot-Taster-Reset.

Aktueller Stand:
- lokaler Setup-AP mit kleiner Webform
- gemeinsamer Boot-Taster-Pfad fuer Setup-Reentry und Factory Reset
- Nutzung der persistierten Shared-Settings aus `ShStorage`

Bewusst noch nicht enthalten:
- grosse Captive-Portal-Komfortlogik
- per-Geraet-Sonderseiten
- vollstaendige Runtime-Nutzung aller gespeicherten Basiswerte
