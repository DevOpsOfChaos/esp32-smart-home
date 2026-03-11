# NET_SEN

## Lokaler DHT22-Minimalcheck
- DHT22-Datenleitung an `GPIO6`
- gemeinsame Masse zwischen `net_sen_01` und DHT22
- Versorgung des DHT22 passend zum verwendeten Sensor/Modul; die Datenleitung darf den ESP32-C3 nicht mit 5V treiben
- bei einem nackten DHT22/AM2302 ohne Modul-Pull-up einen Pull-up zwischen `DATA` und `VCC` vorsehen

## Serielle Marker fuer den Hardware-Nachweis
- gueltig: `HELLO_ACK empfangen` und danach `DHT22 OK temp=... hum=...`
- Sensorfehler: `DHT22 FEHLER ...`
- Verdrahtungshinweis: `DHT22 HINWEIS: seit Boot keine gueltige Messung; GPIO6/VCC/GND/Pull-up pruefen`

## Bewusste Grenze
- Diese Datei belegt keinen Servernachweis. Sie beschreibt nur den lokalen Minimalpfad fuer den echten DHT22-Hardwarecheck.
