# Hardware und Geräte

## Basisgeräte
- `MASTER` – einzige ESP-NOW-zu-MQTT-Bridge
- `NET-ERL` – netzbetrieben, ein Relais
- `NET-ZRL` – netzbetrieben, zwei Relais
- `NET-SEN` – netzbetriebener Sensor-Node
- `BAT-SEN` – batteriebetriebener Sensor-/Event-Node

## Arbeitsregel
- Sondermodule wachsen aus den vorhandenen Basistypen ueber Featurewahl, Pin-Mapping und Profile.
- Es gibt keine neue Basisklasse pro Sondergeraet.
- Nicht jedes Feature gehoert auf jeden Basistyp; die Zuordnung wird bewusst begrenzt.
- Konkrete Pinouts und Sensorvarianten werden erst nach sauberer Hardwareentscheidung finalisiert.

## Verbindliche Detaildoku
- `17_firmware_featurematrix.md` haelt fest, welcher Basistyp welches Feature primaer tragen soll und welcher Reifestand aktuell gilt.
- `18_setup_provisioning_reset.md` definiert den gemeinsamen Setup-, Provisionierungs- und Resetvertrag fuer Nodes.
- `19_sondermodule_roadmap.md` beschreibt die aktuell gewollten Sondermodule und ihre Ableitung aus den Basisfamilien.
