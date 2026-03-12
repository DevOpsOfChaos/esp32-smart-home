#!/usr/bin/env python3
from __future__ import annotations

import argparse
import csv
import io
import json
import subprocess
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Callable


ROOT_DIR = Path(__file__).resolve().parents[2]
SERVER_DIR = ROOT_DIR / "server"

SIM_CASE_ID = "simcase_server_ingest_minimal"
SIM_MASTER_ID = "sim_master_01"
SIM_NET_ERL_ID = "sim_net_erl_01"
SIM_NET_ZRL_ID = "sim_net_zrl_01"
SIM_NET_SEN_ID = "sim_net_sen_01"
SIM_BAT_SEN_ID = "sim_bat_sen_01"

EXPECTED_AUDIT_COUNTS = {
    f"smarthome/master/{SIM_MASTER_ID}/status": 1,
    f"smarthome/node/{SIM_NET_ERL_ID}/meta": 1,
    f"smarthome/node/{SIM_NET_ERL_ID}/status": 1,
    f"smarthome/node/{SIM_NET_ERL_ID}/state": 1,
    f"smarthome/node/{SIM_NET_ZRL_ID}/meta": 1,
    f"smarthome/node/{SIM_NET_ZRL_ID}/status": 1,
    f"smarthome/node/{SIM_NET_ZRL_ID}/state": 1,
    f"smarthome/node/{SIM_NET_SEN_ID}/meta": 1,
    f"smarthome/node/{SIM_NET_SEN_ID}/status": 3,
    f"smarthome/node/{SIM_NET_SEN_ID}/state": 2,
    f"smarthome/node/{SIM_BAT_SEN_ID}/meta": 1,
    f"smarthome/node/{SIM_BAT_SEN_ID}/status": 1,
    f"smarthome/node/{SIM_BAT_SEN_ID}/state": 1,
}

EXPECTED_CAPABILITIES = {
    SIM_NET_ERL_ID: {"relay_1"},
    SIM_NET_ZRL_ID: {"relay_1", "relay_2"},
    SIM_NET_SEN_ID: {"temp_01c", "hum_01pct", "lux", "motion"},
    SIM_BAT_SEN_ID: {"battery_mv", "battery_pct"},
}

EXPECTED_INFLUX_METRICS = {
    SIM_NET_SEN_ID: {"temp_01c", "hum_01pct", "lux"},
    SIM_BAT_SEN_ID: {"battery_mv", "battery_pct"},
}

NON_INFLUX_NODES = {SIM_NET_ERL_ID, SIM_NET_ZRL_ID}


class HarnessError(RuntimeError):
    pass


@dataclass(frozen=True)
class PublishMessage:
    topic: str
    payload: dict[str, Any] | None
    retain: bool


def utc_now_iso() -> str:
    return datetime.now(timezone.utc).isoformat(timespec="milliseconds").replace("+00:00", "Z")


def load_env_file(path: Path) -> dict[str, str]:
    if not path.is_file():
        raise HarnessError(f"Env-Datei nicht gefunden: {path}")
    values: dict[str, str] = {}
    for raw_line in path.read_text(encoding="utf-8").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue
        key, separator, value = line.partition("=")
        if not separator:
            continue
        values[key.strip()] = value.strip()
    return values


def csv_rows(text: str) -> list[dict[str, str]]:
    stripped = text.strip()
    if not stripped:
        return []
    return list(csv.DictReader(io.StringIO(stripped)))


def sql_quote(value: str) -> str:
    return value.replace("'", "''")


def bool_from_json(value: str | None, key: str) -> bool | None:
    if not value:
        return None
    parsed = json.loads(value)
    current = parsed.get(key)
    return current if isinstance(current, bool) else None


def int_from_json(value: str | None, key: str) -> int | None:
    if not value:
        return None
    parsed = json.loads(value)
    current = parsed.get(key)
    return int(current) if isinstance(current, int) else None


class SimulationHarness:
    def __init__(self, args: argparse.Namespace, env_values: dict[str, str]) -> None:
        self.args = args
        self.env_values = env_values
        self.compose_cmd = [
            "docker",
            "compose",
            "--project-name",
            args.compose_project,
            "--env-file",
            str(args.env_file),
        ]
        self.run_started_at = ""
        self.assertions: list[tuple[bool, str]] = []
        self.published_topics: list[str] = []
        self.nodered_url = f"http://127.0.0.1:{env_values.get('NODERED_HOST_PORT', '18880')}/"
        self.influx_url = f"http://127.0.0.1:{env_values.get('INFLUX_HOST_PORT', '18086')}"
        self.influx_org = env_values.get("INFLUXDB_ORG", "smarthome")
        self.influx_bucket = env_values.get("INFLUXDB_BUCKET", "smarthome")
        self.influx_token = env_values.get("INFLUXDB_ADMIN_TOKEN", "")

    def run(self) -> str:
        self._validate_isolation()
        self._wait_for_http("Node-RED", self.nodered_url, expect_status=200)
        self._wait_for_http("InfluxDB", f"{self.influx_url}/health", expect_status=200)
        self._clear_retained_topics()

        self.run_started_at = utc_now_iso()
        self._publish_batch("bootstrap", self._bootstrap_messages())

        devices = self._wait_for(
            "simulierte Geraete in SQLite",
            lambda: self._fetch_devices() if len(self._fetch_devices()) == 5 else None,
        )
        capabilities = self._wait_for(
            "Capability-Ingest aus Meta",
            lambda: self._fetch_capabilities() if self._capabilities_ready(self._fetch_capabilities()) else None,
        )
        initial_sensor_state = self._wait_for(
            "initialer Sensorzustand",
            lambda: self._fetch_last_state(SIM_NET_SEN_ID)
            if self._sensor_initial_state_ready(self._fetch_last_state(SIM_NET_SEN_ID))
            else None,
        )

        self._publish_batch("offline", self._offline_messages())
        offline_sensor_state = self._wait_for(
            "Offline-Sicht fuer sim_net_sen_01",
            lambda: self._fetch_last_state(SIM_NET_SEN_ID)
            if self._sensor_offline_state_ready(self._fetch_last_state(SIM_NET_SEN_ID))
            else None,
        )

        self._publish_batch("recovery", self._recovery_messages())
        recovered_sensor_state = self._wait_for(
            "Recovery-Sicht fuer sim_net_sen_01",
            lambda: self._fetch_last_state(SIM_NET_SEN_ID)
            if self._sensor_recovery_state_ready(self._fetch_last_state(SIM_NET_SEN_ID))
            else None,
        )

        audit_counts = self._wait_for(
            "Audit-Log fuer alle Simulationsnachrichten",
            lambda: self._fetch_audit_counts()
            if self._audit_counts_ready(self._fetch_audit_counts())
            else None,
        )
        influx_rows = self._wait_for(
            "Influx-Punkte fuer numerische Simulationen",
            lambda: self._fetch_influx_rows()
            if self._influx_rows_ready(self._fetch_influx_rows())
            else None,
        )

        self._clear_retained_topics()

        self._check_device_rows(devices)
        self._check_capabilities(capabilities)
        self._check_offline_recovery(initial_sensor_state, offline_sensor_state, recovered_sensor_state)
        self._check_audit_counts(audit_counts)
        self._check_influx_rows(influx_rows)

        report = self._build_report(devices, capabilities, audit_counts, influx_rows)
        if self.args.report_file:
            report_path = self.args.report_file.resolve()
            report_path.parent.mkdir(parents=True, exist_ok=True)
            report_path.write_text(report, encoding="utf-8")
        return report

    def _validate_isolation(self) -> None:
        if not self.args.compose_project.startswith("smarthome_sim"):
            raise HarnessError(
                "Compose-Projekt muss mit 'smarthome_sim' beginnen. Alles andere ist ein unnoetiges Risiko."
            )
        for env_key in ("MQTT_BIND_ADDRESS", "NODERED_BIND_ADDRESS", "INFLUX_BIND_ADDRESS"):
            bind_value = self.env_values.get(env_key, "").strip()
            if bind_value not in {"127.0.0.1", "localhost"}:
                raise HarnessError(
                    f"{env_key} muss fuer die Simulation auf Loopback zeigen. Gefunden: {bind_value or '<leer>'}"
                )

    def _run_command(self, command: list[str], description: str) -> str:
        completed = subprocess.run(
            command,
            cwd=SERVER_DIR,
            text=True,
            capture_output=True,
            check=False,
        )
        if completed.returncode != 0:
            message = completed.stderr.strip() or completed.stdout.strip() or "unbekannter Fehler"
            raise HarnessError(f"{description} fehlgeschlagen: {message}")
        return completed.stdout

    def _wait_for_http(self, label: str, url: str, expect_status: int) -> None:
        deadline = time.time() + self.args.timeout_seconds
        last_error = "keine Antwort"
        while time.time() < deadline:
            try:
                request = urllib.request.Request(url, method="GET")
                with urllib.request.urlopen(request, timeout=5) as response:
                    if response.status == expect_status:
                        return
                    last_error = f"HTTP {response.status}"
            except (urllib.error.URLError, TimeoutError) as exc:
                last_error = str(exc)
            time.sleep(self.args.poll_interval)
        raise HarnessError(f"{label} nicht bereit: {last_error}")

    def _wait_for(self, label: str, probe: Callable[[], Any]) -> Any:
        deadline = time.time() + self.args.timeout_seconds
        last_error: str | None = None
        while time.time() < deadline:
            try:
                value = probe()
                if value:
                    return value
            except HarnessError as exc:
                last_error = str(exc)
            time.sleep(self.args.poll_interval)
        if last_error:
            raise HarnessError(f"{label} nicht erreicht: {last_error}")
        raise HarnessError(f"{label} nicht erreicht.")

    def _compose_exec(self, service: str, command: list[str], description: str) -> str:
        return self._run_command(self.compose_cmd + ["exec", "-T", service] + command, description)

    def _mqtt_command(self, message: PublishMessage) -> list[str]:
        command = [
            "mosquitto_pub",
            "-h",
            "127.0.0.1",
            "-p",
            "1883",
            "-q",
            "1",
            "-i",
            SIM_MASTER_ID,
            "-t",
            message.topic,
        ]
        mqtt_username = self.env_values.get("MQTT_USERNAME", "").strip()
        mqtt_password = self.env_values.get("MQTT_PASSWORD", "").strip()
        if mqtt_username and mqtt_password:
            command.extend(["-u", mqtt_username, "-P", mqtt_password])
        if message.retain:
            command.append("-r")
        if message.payload is None:
            command.append("-n")
        else:
            command.extend(["-m", json.dumps(message.payload, separators=(",", ":"), ensure_ascii=True)])
        return command

    def _publish_batch(self, label: str, messages: list[PublishMessage]) -> None:
        for message in messages:
            self._compose_exec("mosquitto", self._mqtt_command(message), f"MQTT-Publish {label} {message.topic}")
            self.published_topics.append(message.topic)
            time.sleep(self.args.message_delay)

    def _clear_retained_topics(self) -> None:
        for topic in self._retained_topics():
            self._compose_exec(
                "mosquitto",
                self._mqtt_command(PublishMessage(topic=topic, payload=None, retain=True)),
                f"Retained-Cleanup {topic}",
            )

    def _sqlite_query(self, sql: str) -> list[dict[str, str]]:
        output = self._compose_exec(
            "nodered",
            ["sqlite3", "-readonly", "-header", "-csv", "/data/sqlite/smarthome.sqlite", sql],
            "SQLite-Abfrage",
        )
        return csv_rows(output)

    def _fetch_devices(self) -> list[dict[str, str]]:
        device_ids = ", ".join(
            f"'{sql_quote(device_id)}'"
            for device_id in (SIM_MASTER_ID, SIM_NET_ERL_ID, SIM_NET_ZRL_ID, SIM_NET_SEN_ID, SIM_BAT_SEN_ID)
        )
        sql = (
            "SELECT device_id, device_role, display_name, origin_master_id, device_class, power_source, status, "
            "last_meta_at, last_status_at, last_state_at "
            f"FROM devices WHERE device_id IN ({device_ids}) ORDER BY device_id;"
        )
        return self._sqlite_query(sql)

    def _fetch_capabilities(self) -> list[dict[str, str]]:
        sql = (
            "SELECT device_id, capability_key FROM device_capabilities "
            "WHERE device_id GLOB 'sim_*' ORDER BY device_id, capability_key;"
        )
        return self._sqlite_query(sql)

    def _fetch_last_state(self, device_id: str) -> dict[str, str] | None:
        sql = (
            "SELECT device_id, status_json, status_at, state_json, state_at, ack_json, ack_at "
            f"FROM device_last_state WHERE device_id = '{sql_quote(device_id)}';"
        )
        rows = self._sqlite_query(sql)
        return rows[0] if rows else None

    def _fetch_audit_counts(self) -> dict[str, int]:
        sql = (
            "SELECT topic, COUNT(*) AS row_count "
            "FROM audit_log "
            f"WHERE direction = 'ingest' AND occurred_at >= '{sql_quote(self.run_started_at)}' "
            "AND device_id GLOB 'sim_*' "
            "GROUP BY topic ORDER BY topic;"
        )
        rows = self._sqlite_query(sql)
        return {row["topic"]: int(row["row_count"]) for row in rows}

    def _fetch_influx_rows(self) -> list[dict[str, str]]:
        flux = (
            f'from(bucket: "{self.influx_bucket}")\n'
            f'  |> range(start: time(v: "{self.run_started_at}"))\n'
            '  |> filter(fn: (r) => r._measurement == "smarthome_sensor")\n'
            '  |> filter(fn: (r) => r.node_id =~ /^sim_/)\n'
            '  |> keep(columns: ["_time", "_value", "metric", "node_id", "unit"])\n'
            '  |> sort(columns: ["node_id", "metric", "_time"])'
        )
        url = f"{self.influx_url}/api/v2/query?org={urllib.parse.quote(self.influx_org, safe='')}"
        request = urllib.request.Request(
            url,
            data=flux.encode("utf-8"),
            method="POST",
            headers={
                "Authorization": f"Token {self.influx_token}",
                "Accept": "application/csv",
                "Content-Type": "application/vnd.flux",
            },
        )
        try:
            with urllib.request.urlopen(request, timeout=10) as response:
                body = response.read().decode("utf-8")
        except urllib.error.URLError as exc:
            raise HarnessError(f"Influx-Query fehlgeschlagen: {exc}") from exc
        data_lines = [line for line in body.splitlines() if line and not line.startswith("#")]
        if not data_lines:
            return []
        return list(csv.DictReader(io.StringIO("\n".join(data_lines))))

    def _base_fields(self, entity_id: str, device_type: str, power_source: str) -> dict[str, Any]:
        return {
            "source": "simulation",
            "sim_case": SIM_CASE_ID,
            "master_id": SIM_MASTER_ID,
            "node_id": entity_id,
            "device_id": entity_id,
            "device_type": device_type,
            "device_class": device_type,
            "power_source": power_source,
        }

    def _bootstrap_messages(self) -> list[PublishMessage]:
        return [
            PublishMessage(
                topic=f"smarthome/master/{SIM_MASTER_ID}/status",
                payload={
                    "source": "simulation",
                    "sim_case": SIM_CASE_ID,
                    "master_id": SIM_MASTER_ID,
                    "device_id": SIM_MASTER_ID,
                    "display_name": f"[SIM] {SIM_MASTER_ID}",
                    "device_type": "master",
                    "device_class": "master",
                    "power_source": "mains",
                    "online": True,
                    "fw": "sim-0.1.0",
                    "protocol_version": "sim-v1",
                },
                retain=True,
            ),
            PublishMessage(
                topic=f"smarthome/node/{SIM_NET_ERL_ID}/meta",
                payload={
                    **self._base_fields(SIM_NET_ERL_ID, "net_erl", "mains"),
                    "display_name": f"[SIM] {SIM_NET_ERL_ID}",
                    "room": "sim_lab",
                    "capabilities": {
                        "relay_1": {"role": "actuator", "type": "boolean", "writable": True, "readable": True}
                    },
                },
                retain=True,
            ),
            PublishMessage(
                topic=f"smarthome/node/{SIM_NET_ERL_ID}/status",
                payload={**self._base_fields(SIM_NET_ERL_ID, "net_erl", "mains"), "online": True},
                retain=True,
            ),
            PublishMessage(
                topic=f"smarthome/node/{SIM_NET_ERL_ID}/state",
                payload={
                    **self._base_fields(SIM_NET_ERL_ID, "net_erl", "mains"),
                    "room": "sim_lab",
                    "relay_1": True,
                    "fault": False,
                },
                retain=True,
            ),
            PublishMessage(
                topic=f"smarthome/node/{SIM_NET_ZRL_ID}/meta",
                payload={
                    **self._base_fields(SIM_NET_ZRL_ID, "net_zrl", "mains"),
                    "display_name": f"[SIM] {SIM_NET_ZRL_ID}",
                    "room": "sim_lab",
                    "capabilities": {
                        "relay_1": {"role": "actuator", "type": "boolean", "writable": True, "readable": True},
                        "relay_2": {"role": "actuator", "type": "boolean", "writable": True, "readable": True},
                    },
                },
                retain=True,
            ),
            PublishMessage(
                topic=f"smarthome/node/{SIM_NET_ZRL_ID}/status",
                payload={**self._base_fields(SIM_NET_ZRL_ID, "net_zrl", "mains"), "online": True},
                retain=True,
            ),
            PublishMessage(
                topic=f"smarthome/node/{SIM_NET_ZRL_ID}/state",
                payload={
                    **self._base_fields(SIM_NET_ZRL_ID, "net_zrl", "mains"),
                    "room": "sim_lab",
                    "relay_1": False,
                    "relay_2": True,
                    "cover_mode": False,
                    "fault": False,
                },
                retain=True,
            ),
            PublishMessage(
                topic=f"smarthome/node/{SIM_NET_SEN_ID}/meta",
                payload={
                    **self._base_fields(SIM_NET_SEN_ID, "net_sen", "mains"),
                    "display_name": f"[SIM] {SIM_NET_SEN_ID}",
                    "room": "sim_lab",
                    "capabilities": {
                        "temp_01c": {"role": "sensor", "type": "number", "unit": "0.1c"},
                        "hum_01pct": {"role": "sensor", "type": "number", "unit": "0.1pct"},
                        "lux": {"role": "sensor", "type": "number", "unit": "lux"},
                        "motion": {"role": "sensor", "type": "boolean"},
                    },
                },
                retain=True,
            ),
            PublishMessage(
                topic=f"smarthome/node/{SIM_NET_SEN_ID}/status",
                payload={**self._base_fields(SIM_NET_SEN_ID, "net_sen", "mains"), "online": True},
                retain=True,
            ),
            PublishMessage(
                topic=f"smarthome/node/{SIM_NET_SEN_ID}/state",
                payload={
                    **self._base_fields(SIM_NET_SEN_ID, "net_sen", "mains"),
                    "room": "sim_lab",
                    "temp_01c": 214,
                    "hum_01pct": 487,
                    "lux": 120,
                    "motion": False,
                    "fault": False,
                },
                retain=True,
            ),
            PublishMessage(
                topic=f"smarthome/node/{SIM_BAT_SEN_ID}/meta",
                payload={
                    **self._base_fields(SIM_BAT_SEN_ID, "bat_sen", "battery"),
                    "display_name": f"[SIM] {SIM_BAT_SEN_ID}",
                    "room": "sim_lab",
                    "capabilities": {
                        "battery_mv": {"role": "sensor", "type": "number", "unit": "mv"},
                        "battery_pct": {"role": "sensor", "type": "number", "unit": "pct"},
                    },
                },
                retain=True,
            ),
            PublishMessage(
                topic=f"smarthome/node/{SIM_BAT_SEN_ID}/status",
                payload={**self._base_fields(SIM_BAT_SEN_ID, "bat_sen", "battery"), "online": True},
                retain=True,
            ),
            PublishMessage(
                topic=f"smarthome/node/{SIM_BAT_SEN_ID}/state",
                payload={
                    **self._base_fields(SIM_BAT_SEN_ID, "bat_sen", "battery"),
                    "room": "sim_lab",
                    "battery_mv": 3740,
                    "battery_pct": 87,
                    "fault": False,
                },
                retain=True,
            ),
        ]

    def _offline_messages(self) -> list[PublishMessage]:
        return [
            PublishMessage(
                topic=f"smarthome/node/{SIM_NET_SEN_ID}/status",
                payload={**self._base_fields(SIM_NET_SEN_ID, "net_sen", "mains"), "online": False},
                retain=True,
            )
        ]

    def _recovery_messages(self) -> list[PublishMessage]:
        return [
            PublishMessage(
                topic=f"smarthome/node/{SIM_NET_SEN_ID}/status",
                payload={**self._base_fields(SIM_NET_SEN_ID, "net_sen", "mains"), "online": True},
                retain=True,
            ),
            PublishMessage(
                topic=f"smarthome/node/{SIM_NET_SEN_ID}/state",
                payload={
                    **self._base_fields(SIM_NET_SEN_ID, "net_sen", "mains"),
                    "room": "sim_lab",
                    "temp_01c": 218,
                    "hum_01pct": 502,
                    "lux": 133,
                    "motion": False,
                    "fault": False,
                },
                retain=True,
            ),
        ]

    def _retained_topics(self) -> list[str]:
        return [
            f"smarthome/master/{SIM_MASTER_ID}/status",
            f"smarthome/node/{SIM_NET_ERL_ID}/meta",
            f"smarthome/node/{SIM_NET_ERL_ID}/status",
            f"smarthome/node/{SIM_NET_ERL_ID}/state",
            f"smarthome/node/{SIM_NET_ZRL_ID}/meta",
            f"smarthome/node/{SIM_NET_ZRL_ID}/status",
            f"smarthome/node/{SIM_NET_ZRL_ID}/state",
            f"smarthome/node/{SIM_NET_SEN_ID}/meta",
            f"smarthome/node/{SIM_NET_SEN_ID}/status",
            f"smarthome/node/{SIM_NET_SEN_ID}/state",
            f"smarthome/node/{SIM_BAT_SEN_ID}/meta",
            f"smarthome/node/{SIM_BAT_SEN_ID}/status",
            f"smarthome/node/{SIM_BAT_SEN_ID}/state",
        ]

    def _capabilities_ready(self, rows: list[dict[str, str]]) -> bool:
        current: dict[str, set[str]] = {}
        for row in rows:
            current.setdefault(row["device_id"], set()).add(row["capability_key"])
        return all(expected <= current.get(device_id, set()) for device_id, expected in EXPECTED_CAPABILITIES.items())

    def _sensor_initial_state_ready(self, row: dict[str, str] | None) -> bool:
        if not row:
            return False
        return (
            bool_from_json(row.get("status_json"), "online") is True
            and int_from_json(row.get("state_json"), "temp_01c") == 214
            and not row.get("ack_json")
        )

    def _sensor_offline_state_ready(self, row: dict[str, str] | None) -> bool:
        if not row:
            return False
        return (
            bool_from_json(row.get("status_json"), "online") is False
            and int_from_json(row.get("state_json"), "temp_01c") == 214
        )

    def _sensor_recovery_state_ready(self, row: dict[str, str] | None) -> bool:
        if not row:
            return False
        return (
            bool_from_json(row.get("status_json"), "online") is True
            and int_from_json(row.get("state_json"), "temp_01c") == 218
            and not row.get("ack_json")
        )

    def _audit_counts_ready(self, counts: dict[str, int]) -> bool:
        return counts == EXPECTED_AUDIT_COUNTS

    def _influx_rows_ready(self, rows: list[dict[str, str]]) -> bool:
        if len(rows) != 8:
            return False
        metrics_by_node: dict[str, set[str]] = {}
        for row in rows:
            metrics_by_node.setdefault(row["node_id"], set()).add(row["metric"])
        if any(node_id in metrics_by_node for node_id in NON_INFLUX_NODES):
            return False
        return all(expected <= metrics_by_node.get(node_id, set()) for node_id, expected in EXPECTED_INFLUX_METRICS.items())

    def _check(self, condition: bool, message: str) -> None:
        self.assertions.append((condition, message))

    def _check_device_rows(self, rows: list[dict[str, str]]) -> None:
        by_id = {row["device_id"]: row for row in rows}
        self._check(len(rows) == 5, "SQLite hat genau 5 Simulationsgeraete registriert.")
        self._check(by_id[SIM_MASTER_ID]["device_role"] == "master", "sim_master_01 bleibt als Master registriert.")
        self._check(
            by_id[SIM_MASTER_ID]["display_name"].startswith("[SIM] "),
            "Master-Display-Name traegt die [SIM]-Markierung.",
        )
        for node_id, expected_class, expected_power in (
            (SIM_NET_ERL_ID, "net_erl", "mains"),
            (SIM_NET_ZRL_ID, "net_zrl", "mains"),
            (SIM_NET_SEN_ID, "net_sen", "mains"),
            (SIM_BAT_SEN_ID, "bat_sen", "battery"),
        ):
            row = by_id[node_id]
            self._check(row["origin_master_id"] == SIM_MASTER_ID, f"{node_id} bleibt sauber am Fake-Master aufgehangen.")
            self._check(row["display_name"].startswith("[SIM] "), f"{node_id} bleibt ueber den Namen als Simulation erkennbar.")
            self._check(row["device_class"] == expected_class, f"{node_id} wurde als {expected_class} klassifiziert.")
            self._check(row["power_source"] == expected_power, f"{node_id} behaelt power_source={expected_power}.")
            self._check(bool(row["last_meta_at"]), f"{node_id} hat meta ingestiert.")
            self._check(bool(row["last_status_at"]), f"{node_id} hat status ingestiert.")
            self._check(bool(row["last_state_at"]), f"{node_id} hat state ingestiert.")

    def _check_capabilities(self, rows: list[dict[str, str]]) -> None:
        current: dict[str, set[str]] = {}
        for row in rows:
            current.setdefault(row["device_id"], set()).add(row["capability_key"])
        for device_id, expected in EXPECTED_CAPABILITIES.items():
            self._check(expected <= current.get(device_id, set()), f"{device_id} hat die erwarteten Capability-Schluessel aus meta.")

    def _check_offline_recovery(
        self,
        initial_state: dict[str, str],
        offline_state: dict[str, str],
        recovered_state: dict[str, str],
    ) -> None:
        self._check(
            bool_from_json(initial_state.get("status_json"), "online") is True,
            "sim_net_sen_01 startet online in device_last_state.status_json.",
        )
        self._check(
            int_from_json(initial_state.get("state_json"), "temp_01c") == 214,
            "sim_net_sen_01 schreibt den ersten Sensorzustand nach device_last_state.state_json.",
        )
        self._check(
            bool_from_json(offline_state.get("status_json"), "online") is False,
            "Offline-Status kippt fuer sim_net_sen_01 auf false.",
        )
        self._check(
            int_from_json(offline_state.get("state_json"), "temp_01c") == 214,
            "Der letzte state bleibt waehrend offline unveraendert bestehen.",
        )
        self._check(
            bool_from_json(recovered_state.get("status_json"), "online") is True,
            "Recovery kippt status_json wieder auf online:true.",
        )
        self._check(
            int_from_json(recovered_state.get("state_json"), "temp_01c") == 218,
            "Recovery schreibt einen neuen state statt den Offline-Zustand umzudeuten.",
        )
        self._check(not recovered_state.get("ack_json"), "ack_json bleibt leer; status/state werden nicht vermischt.")

    def _check_audit_counts(self, counts: dict[str, int]) -> None:
        self._check(counts == EXPECTED_AUDIT_COUNTS, "audit_log enthaelt jede Simulationsnachricht genau einmal als ingest.")

    def _check_influx_rows(self, rows: list[dict[str, str]]) -> None:
        metrics_by_node: dict[str, set[str]] = {}
        for row in rows:
            metrics_by_node.setdefault(row["node_id"], set()).add(row["metric"])
        for node_id, expected in EXPECTED_INFLUX_METRICS.items():
            self._check(expected <= metrics_by_node.get(node_id, set()), f"{node_id} schreibt nur die erwarteten numerischen Metriken nach Influx.")
        for node_id in NON_INFLUX_NODES:
            self._check(node_id not in metrics_by_node, f"{node_id} schreibt keine booleschen Aktor-States nach Influx.")

    def _build_report(
        self,
        devices: list[dict[str, str]],
        capabilities: list[dict[str, str]],
        audit_counts: dict[str, int],
        influx_rows: list[dict[str, str]],
    ) -> str:
        success = all(status for status, _ in self.assertions)
        device_ids = ", ".join(row["device_id"] for row in devices)
        lines = [
            "# Server-Simulationslauf",
            "",
            f"- Ergebnis: {'PASS' if success else 'FAIL'}",
            f"- Simulationsfall: `{SIM_CASE_ID}`",
            f"- Compose-Projekt: `{self.args.compose_project}`",
            f"- Env-Datei: `{self.args.env_file}`",
            f"- Startzeitpunkt: `{self.run_started_at}`",
            f"- Simulationsgeraete: `{device_ids}`",
            f"- Publizierte MQTT-Nachrichten: `{len(self.published_topics)}`",
            f"- Persistierte Capabilities: `{len(capabilities)}`",
            f"- Influx-Zeilen im Lauf: `{len(influx_rows)}`",
            "",
            "## Grenzen",
            "",
            "- Dieser Lauf simuliert nur die serverseitige HELLO-Folge nach erfolgreichem HELLO/HELLO_ACK.",
            "- Kein ESP-NOW, keine Funk-ACK-Mechanik, keine Hardwarewirkung, kein Hardware-Nachweis.",
            "- Die Nodes waren keine direkten MQTT-Peers; alle Nachrichten kamen ueber den einen Fake-Master-Boundary-Runner.",
            "",
            "## Assertions",
            "",
        ]
        for status, message in self.assertions:
            lines.append(f"- {'OK' if status else 'FAIL'}: {message}")
        lines.extend(["", "## Audit-Zaehlung", ""])
        for topic, count in sorted(audit_counts.items()):
            lines.append(f"- `{topic}` -> `{count}`")
        if self.args.report_file:
            lines.extend(["", "## Report-Datei", "", f"- `{self.args.report_file.resolve()}`"])
        return "\n".join(lines) + "\n"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Test-only Fake-Master-Harness fuer den isolierten SmartHome-Server-Stack."
    )
    parser.add_argument(
        "--compose-project",
        default="smarthome_sim",
        help="Docker-Compose-Projektname. Muss mit smarthome_sim beginnen.",
    )
    parser.add_argument(
        "--env-file",
        type=Path,
        default=SERVER_DIR / ".env.sim.example",
        help="Versionierte oder lokale Simulations-Env-Datei fuer den isolierten Stack.",
    )
    parser.add_argument(
        "--report-file",
        type=Path,
        default=None,
        help="Optionale Markdown-Ausgabe fuer den Laufbericht.",
    )
    parser.add_argument(
        "--timeout-seconds",
        type=float,
        default=30.0,
        help="Maximale Wartezeit pro Check.",
    )
    parser.add_argument(
        "--poll-interval",
        type=float,
        default=0.5,
        help="Polling-Intervall fuer SQLite/Influx-Checks.",
    )
    parser.add_argument(
        "--message-delay",
        type=float,
        default=0.25,
        help="Kurze Pause zwischen den MQTT-Publishes.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        env_values = load_env_file(args.env_file.resolve())
        harness = SimulationHarness(args, env_values)
        report = harness.run()
        print(report, end="")
        if not all(status for status, _ in harness.assertions):
            return 1
        return 0
    except HarnessError as exc:
        print(f"FEHLER: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
