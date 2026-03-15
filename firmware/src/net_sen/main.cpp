/*
====================================================================
 Projekt   : SmartHome ESP32
 Geraet    : NET-SEN (Netzbetrieben, Sensor-Basis)
 Datei     : main.cpp
 Version   : 0.3.0
 Stand     : 2026-03-15

 Funktion:
 Allgemeine netzbetriebene Sensor-Basis in derselben ESP-NOW-Linie
 wie NET-ERL.
 - feste node_id net_sen_01
 - HELLO beim Start
 - HEARTBEAT zyklisch
 - STATE_REPORT mit modularen Sensor-Providern
 - DHT22 bleibt der real belegte Standardpfad
 - optionale Stub-Provider fuer kommende Module bleiben explizit
   getrennt von echten Hardwarepfaden
====================================================================
*/

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <stdarg.h>
#include <string.h>

#if __has_include(<esp_arduino_version.h>)
  #include <esp_arduino_version.h>
#endif

#ifndef ESP_ARDUINO_VERSION_MAJOR
  #define ESP_ARDUINO_VERSION_MAJOR 2
#endif

#include "AppConfig.h"
#include "PinConfig.h"
#include "../../include/ProjectVersion.h"
#include "../../include/DebugConfig.h"
#include "../../lib/ShProtocol/src/Protocol.h"
#include "../../lib/ShProtocol/src/DeviceTypes.h"
#include "../../lib/ShSensors/src/NetSenModules.h"
#include <ShNodeProvisioning.h>

#if NET_SEN_ENV_PROVIDER == SH_NET_SEN_ENV_PROVIDER_BMP280 && !SHSENSORS_HAS_ADAFRUIT_BMP280
  #error "NET_SEN_ENV_PROVIDER=BMP280 braucht Adafruit_BMP280."
#endif

#if NET_SEN_ENV_PROVIDER == SH_NET_SEN_ENV_PROVIDER_BME280 && !SHSENSORS_HAS_ADAFRUIT_BME280
  #error "NET_SEN_ENV_PROVIDER=BME280 braucht Adafruit_BME280."
#endif

#if NET_SEN_ENV_PROVIDER == SH_NET_SEN_ENV_PROVIDER_SHT41 && !SHSENSORS_HAS_ADAFRUIT_SHT4X
  #error "NET_SEN_ENV_PROVIDER=SHT41 braucht Adafruit_SHT4x."
#endif

#if NET_SEN_AIR_PROVIDER == SH_NET_SEN_AIR_PROVIDER_ENS160 && !SHSENSORS_HAS_SCIOSENSE_ENS160
  #error "NET_SEN_AIR_PROVIDER=ENS160 braucht ScioSense_ENS160."
#endif

#if NET_SEN_LIGHT_PROVIDER == SH_NET_SEN_LIGHT_PROVIDER_VEML7700 && !SHSENSORS_HAS_ADAFRUIT_VEML7700
  #error "NET_SEN_LIGHT_PROVIDER=VEML7700 braucht Adafruit_VEML7700."
#endif

constexpr bool DEBUG_LOKAL_AKTIV = DEVICE_DEBUG_AKTIV && DEBUG_AKTIV;
constexpr char DATEI_GERAET[] = "NET-SEN";
constexpr char DATEI_VERSION[] = "0.3.0";
const uint8_t BROADCAST_MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
SmartHome::ShNodeBase::NodeProvisioning provisioning;

using EnvironmentProvider = SmartHome::ShSensors::EnvironmentProvider<NET_SEN_ENV_PROVIDER_KIND>;
using AirQualityProvider = SmartHome::ShSensors::AirQualityProvider<NET_SEN_AIR_PROVIDER_KIND>;
using LightProvider = SmartHome::ShSensors::LightProvider<NET_SEN_LIGHT_PROVIDER_KIND>;
using PresenceProvider = SmartHome::ShSensors::PresenceProvider<NET_SEN_MOTION_PROVIDER_KIND>;

constexpr bool NET_SEN_ERWEITERTER_STATE =
    SmartHome::ShSensors::usesExtendedNetSenState<NET_SEN_ENV_PROVIDER_KIND, NET_SEN_AIR_PROVIDER_KIND>();
constexpr uint16_t NET_SEN_CAPS =
    SmartHome::ShSensors::buildNetSenCaps<
        NET_SEN_ENV_PROVIDER_KIND,
        NET_SEN_AIR_PROVIDER_KIND,
        NET_SEN_LIGHT_PROVIDER_KIND,
        NET_SEN_MOTION_PROVIDER_KIND>();

constexpr bool NET_SEN_ENV_IS_DHT22 =
    NET_SEN_ENV_PROVIDER_KIND == SH_NET_SEN_ENV_PROVIDER_DHT22;
constexpr bool NET_SEN_REQUIRES_I2C_PINS =
    NET_SEN_ENV_PROVIDER_KIND == SH_NET_SEN_ENV_PROVIDER_BMP280 ||
    NET_SEN_ENV_PROVIDER_KIND == SH_NET_SEN_ENV_PROVIDER_BME280 ||
    NET_SEN_ENV_PROVIDER_KIND == SH_NET_SEN_ENV_PROVIDER_SHT41 ||
    NET_SEN_AIR_PROVIDER_KIND == SH_NET_SEN_AIR_PROVIDER_ENS160 ||
    NET_SEN_LIGHT_PROVIDER_KIND == SH_NET_SEN_LIGHT_PROVIDER_VEML7700;
constexpr unsigned long NET_SEN_ENV_WARMUP_MS =
    NET_SEN_ENV_IS_DHT22 ? DHT_WARMUP_MS : I2C_ENV_WARMUP_MS;
constexpr unsigned long NET_SEN_ENV_READ_INTERVAL_MS =
    NET_SEN_ENV_IS_DHT22 ? DHT_READ_INTERVAL_MS : I2C_ENV_READ_INTERVAL_MS;
constexpr unsigned long NET_SEN_ENV_DIAG_HINT_INTERVAL_MS =
    NET_SEN_ENV_IS_DHT22 ? DHT_DIAG_HINT_INTERVAL_MS : I2C_DIAG_HINT_INTERVAL_MS;

static_assert(
    NET_SEN_ENV_PROVIDER_KIND == SH_NET_SEN_ENV_PROVIDER_DHT22 ||
    NET_SEN_ENV_PROVIDER_KIND == SH_NET_SEN_ENV_PROVIDER_NONE ||
    NET_SEN_ENV_PROVIDER_KIND == SH_NET_SEN_ENV_PROVIDER_BMP280 ||
    NET_SEN_ENV_PROVIDER_KIND == SH_NET_SEN_ENV_PROVIDER_BME280 ||
    NET_SEN_ENV_PROVIDER_KIND == SH_NET_SEN_ENV_PROVIDER_SHT41 ||
    NET_SEN_ENV_PROVIDER_KIND == SH_NET_SEN_ENV_PROVIDER_BMP280_STUB ||
    NET_SEN_ENV_PROVIDER_KIND == SH_NET_SEN_ENV_PROVIDER_BME280_STUB ||
    NET_SEN_ENV_PROVIDER_KIND == SH_NET_SEN_ENV_PROVIDER_SHT41_STUB,
    "NET-SEN env provider unbekannt.");

static_assert(
    NET_SEN_AIR_PROVIDER_KIND == SH_NET_SEN_AIR_PROVIDER_NONE ||
    NET_SEN_AIR_PROVIDER_KIND == SH_NET_SEN_AIR_PROVIDER_ENS160 ||
    NET_SEN_AIR_PROVIDER_KIND == SH_NET_SEN_AIR_PROVIDER_ENS160_STUB,
    "NET-SEN air provider unbekannt.");

static_assert(
    NET_SEN_LIGHT_PROVIDER_KIND == SH_NET_SEN_LIGHT_PROVIDER_NONE ||
    NET_SEN_LIGHT_PROVIDER_KIND == SH_NET_SEN_LIGHT_PROVIDER_VEML7700 ||
    NET_SEN_LIGHT_PROVIDER_KIND == SH_NET_SEN_LIGHT_PROVIDER_VEML7700_STUB,
    "NET-SEN light provider unbekannt.");

static_assert(
    NET_SEN_MOTION_PROVIDER_KIND == SH_NET_SEN_MOTION_PROVIDER_NONE ||
    NET_SEN_MOTION_PROVIDER_KIND == SH_NET_SEN_MOTION_PROVIDER_PIR_PIN ||
    NET_SEN_MOTION_PROVIDER_KIND == SH_NET_SEN_MOTION_PROVIDER_LD2410C ||
    NET_SEN_MOTION_PROVIDER_KIND == SH_NET_SEN_MOTION_PROVIDER_LD2410B ||
    NET_SEN_MOTION_PROVIDER_KIND == SH_NET_SEN_MOTION_PROVIDER_PIR_STUB,
    "NET-SEN motion provider unbekannt.");

static_assert(
    NET_SEN_ENV_PROVIDER_KIND != SH_NET_SEN_ENV_PROVIDER_DHT22 || PIN_DHT22_DATA >= 0,
    "NET-SEN DHT22 provider braucht PIN_DHT22_DATA.");

static_assert(
    !NET_SEN_REQUIRES_I2C_PINS || (PIN_SENSOR_SDA >= 0 && PIN_SENSOR_SCL >= 0),
    "NET-SEN I2C-Sensorpfade brauchen gueltige SDA/SCL-Pins.");

static_assert(
    NET_SEN_MOTION_PROVIDER_KIND != SH_NET_SEN_MOTION_PROVIDER_PIR_PIN || PIN_PRESENCE_OUT >= 0,
    "NET-SEN PIR provider braucht einen gueltigen PIR-Pin.");

struct NodeState {
    bool master_bekannt;
    bool master_mac_gueltig;
    bool state_report_offen;
    unsigned long letztes_hello_ms;
    unsigned long letzter_heartbeat_ms;
    unsigned long letzter_state_ms;
    unsigned long letzter_state_wait_log_ms;
    uint8_t master_mac[6];
    uint8_t naechste_seq;
    SmartHome::ShSensors::NetSenState sensor;
};

NodeState nodeStatus = {};

constexpr SmartHome::ShSensors::I2cBusConfig NET_SEN_I2C_CONFIG = {
    PIN_SENSOR_SDA,
    PIN_SENSOR_SCL,
    SENSOR_I2C_CLOCK_HZ,
    SENSOR_I2C_TIMEOUT_MS,
    SENSOR_I2C_SCAN_ON_BEGIN,
    SENSOR_I2C_RECOVER_COOLDOWN_MS};

const SmartHome::ShSensors::EnvironmentProviderConfig ENVIRONMENT_CONFIG = {
    PIN_DHT22_DATA,
    NET_SEN_I2C_CONFIG,
    NET_SEN_ENV_WARMUP_MS,
    NET_SEN_ENV_READ_INTERVAL_MS,
    NET_SEN_ENV_DIAG_HINT_INTERVAL_MS,
    TEMP_DELTA_01C,
    HUM_DELTA_01PCT,
    PRESSURE_DELTA_PA};

const SmartHome::ShSensors::LightProviderConfig LIGHT_CONFIG = {
    NET_SEN_I2C_CONFIG,
    LIGHT_READ_INTERVAL_MS,
    I2C_DIAG_HINT_INTERVAL_MS,
    LUX_DELTA};

const SmartHome::ShSensors::AirQualityProviderConfig AIR_CONFIG = {
    NET_SEN_I2C_CONFIG,
    AIR_WARMUP_MS,
    AIR_READ_INTERVAL_MS,
    I2C_DIAG_HINT_INTERVAL_MS,
    AQI_DELTA,
    TVOC_DELTA_PPB,
    ECO2_DELTA_PPM};

const SmartHome::ShSensors::PresenceProviderConfig PRESENCE_CONFIG = {
    PIN_PRESENCE_OUT,
    PIN_PRESENCE_UART_RX,
    PIN_PRESENCE_UART_TX,
    PRESENCE_UART_BAUD};

EnvironmentProvider environmentProvider(ENVIRONMENT_CONFIG);
AirQualityProvider airQualityProvider(AIR_CONFIG);
LightProvider lightProvider(LIGHT_CONFIG);
PresenceProvider presenceProvider(PRESENCE_CONFIG);

bool istBroadcastMac(const uint8_t* mac) {
    return mac != nullptr && memcmp(mac, BROADCAST_MAC, sizeof(BROADCAST_MAC)) == 0;
}

const uint8_t* holeHelloZielMac() {
    return nodeStatus.master_mac_gueltig ? nodeStatus.master_mac : BROADCAST_MAC;
}

void logf(const char* level, const char* format, ...) {
    if (!DEBUG_LOKAL_AKTIV) return;

    char message[192];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    Serial.print("[");
    Serial.print(level);
    Serial.print("] ");
    Serial.println(message);
}

void logProvisioning(const char* level, const char* message) {
    logf(level, "%s", message);
}

void copyText(char* target, size_t targetSize, const char* source) {
    if (!target || targetSize == 0U) return;
    if (!source) {
        target[0] = '\0';
        return;
    }

    strncpy(target, source, targetSize - 1U);
    target[targetSize - 1U] = '\0';
}

void logMac(const char* prefix, const uint8_t* mac) {
    char macText[18] = {0};
    SmartHome::macToString(mac, macText);
    logf("INFO", "%s%s", prefix, macText);
}

void uebernehmeProvisionierteMasterMac() {
    if (!provisioning.hasStoredMasterMac()) {
        nodeStatus.master_bekannt = false;
        nodeStatus.master_mac_gueltig = false;
        memset(nodeStatus.master_mac, 0, sizeof(nodeStatus.master_mac));
        return;
    }

    memcpy(nodeStatus.master_mac, provisioning.masterMac(), sizeof(nodeStatus.master_mac));
    nodeStatus.master_bekannt = false;
    nodeStatus.master_mac_gueltig = true;
    nodeStatus.state_report_offen = true;
    logMac("Persistierte Master-MAC: ", nodeStatus.master_mac);
}

bool stellePeerSicher(const uint8_t* mac) {
    if (mac == nullptr) return false;
    if (!istBroadcastMac(mac) && !SmartHome::isValidMac(mac)) return false;
    if (esp_now_is_peer_exist(mac)) return true;

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = (uint8_t)WLAN_KANAL;
    peerInfo.encrypt = false;

    esp_err_t err = esp_now_add_peer(&peerInfo);
    if (err != ESP_OK) {
        logf("WARN", "Peer konnte nicht angelegt werden (err=%d)", (int)err);
        return false;
    }

    logMac("Peer angelegt: ", mac);
    return true;
}

bool sendePaket(const uint8_t* zielMac, uint8_t msgType, const void* payload, size_t payloadLen, const char* label) {
    if (zielMac == nullptr || payloadLen > SH_MAX_PAYLOAD_BYTES) return false;
    if (!stellePeerSicher(zielMac)) return false;

    uint8_t buffer[SH_ESPNOW_MAX_BYTES] = {0};
    SmartHome::MsgHeader header = {};
    SmartHome::fillHeader(header, msgType, nodeStatus.naechste_seq++, 0, (uint16_t)payloadLen);

    uint8_t* payloadBuffer = buffer + SH_HEADER_SIZE;
    if (payloadLen > 0U && payload != nullptr) {
        memcpy(payloadBuffer, payload, payloadLen);
    }

    SmartHome::finalizePacketCrc(header, payloadBuffer);
    memcpy(buffer, &header, sizeof(header));

    esp_err_t err = esp_now_send(zielMac, buffer, SH_HEADER_SIZE + payloadLen);
    if (err != ESP_OK) {
        logf("WARN", "%s konnte nicht gesendet werden (err=%d)", label, (int)err);
        return false;
    }

    logf("INFO", "%s gesendet", label);
    return true;
}

bool sendeHello() {
    SmartHome::HelloPayload payload = {};
    copyText(payload.device_id, sizeof(payload.device_id), DEVICE_ID);
    copyText(payload.device_name, sizeof(payload.device_name), provisioning.deviceName());
    payload.device_class = SH_CLASS_NET_SEN;
    payload.caps_hi = (uint8_t)((NET_SEN_CAPS >> 8) & 0xFFU);
    payload.caps_lo = (uint8_t)(NET_SEN_CAPS & 0xFFU);
    payload.power_type = SH_POWER_MAINS;
    payload.fw_version = 0;
    payload.boot_counter = 1U;

    nodeStatus.letztes_hello_ms = millis();
    return sendePaket(holeHelloZielMac(), SH_MSG_HELLO, &payload, sizeof(payload), "HELLO");
}

bool sendeHeartbeat() {
    if (!nodeStatus.master_mac_gueltig) {
        logf("WARN", "HEARTBEAT verschoben: Master-MAC unbekannt");
        return false;
    }

    SmartHome::HeartbeatPayload payload = {};
    copyText(payload.node_id, sizeof(payload.node_id), DEVICE_ID);
    payload.uptime_s = millis() / 1000UL;

    if (sendePaket(nodeStatus.master_mac, SH_MSG_HEARTBEAT, &payload, sizeof(payload), "HEARTBEAT")) {
        nodeStatus.letzter_heartbeat_ms = millis();
        return true;
    }

    return false;
}

bool sendeStateReport() {
    if (!nodeStatus.master_mac_gueltig) {
        logf("WARN", "STATE_REPORT wartet auf HELLO_ACK");
        return false;
    }

    if (NET_SEN_ERWEITERTER_STATE) {
        SmartHome::ExtendedSensorStateReportPayload payload = {};
        copyText(payload.node_id, sizeof(payload.node_id), DEVICE_ID);
        payload.temp_01c = nodeStatus.sensor.temp_01c;
        payload.hum_01pct = nodeStatus.sensor.hum_01pct;
        payload.lux = nodeStatus.sensor.lux;
        payload.pressure_pa = nodeStatus.sensor.pressure_pa;
        payload.aqi = nodeStatus.sensor.aqi;
        payload.tvoc_ppb = nodeStatus.sensor.tvoc_ppb;
        payload.eco2_ppm = nodeStatus.sensor.eco2_ppm;
        payload.motion = nodeStatus.sensor.motion ? 1U : 0U;
        payload.fault = nodeStatus.sensor.fault ? 1U : 0U;

        if (sendePaket(nodeStatus.master_mac, SH_MSG_STATE, &payload, sizeof(payload), "STATE_REPORT")) {
            nodeStatus.state_report_offen = false;
            nodeStatus.letzter_state_ms = millis();
            return true;
        }
        return false;
    }

    SmartHome::SensorStateReportPayload payload = {};
    copyText(payload.node_id, sizeof(payload.node_id), DEVICE_ID);
    payload.temp_01c = nodeStatus.sensor.temp_01c;
    payload.hum_01pct = nodeStatus.sensor.hum_01pct;
    payload.lux = nodeStatus.sensor.lux;
    payload.motion = nodeStatus.sensor.motion ? 1U : 0U;
    payload.fault = nodeStatus.sensor.fault ? 1U : 0U;

    if (sendePaket(nodeStatus.master_mac, SH_MSG_STATE, &payload, sizeof(payload), "STATE_REPORT")) {
        nodeStatus.state_report_offen = false;
        nodeStatus.letzter_state_ms = millis();
        return true;
    }

    return false;
}

bool sendeEvent(uint8_t eventType, uint8_t trigger, uint8_t param1, uint16_t param2) {
    if (!nodeStatus.master_mac_gueltig) {
        return false;
    }

    SmartHome::EventReportPayload payload = {};
    copyText(payload.node_id, sizeof(payload.node_id), DEVICE_ID);
    payload.event_type = eventType;
    payload.trigger = trigger;
    payload.param1 = param1;
    payload.param2 = param2;

    return sendePaket(nodeStatus.master_mac, SH_MSG_EVENT, &payload, sizeof(payload), "EVENT");
}

bool sensorStateBereit() {
    return environmentProvider.isReady() &&
           airQualityProvider.isReady() &&
           lightProvider.isReady() &&
           presenceProvider.isReady();
}

void initialisiereHardware() {
    if (PIN_STATUS_LED >= 0) {
        pinMode(PIN_STATUS_LED, OUTPUT);
        digitalWrite(PIN_STATUS_LED, LOW);
    }

    SmartHome::ShSensors::resetNetSenState(nodeStatus.sensor);
    nodeStatus.state_report_offen = false;

    const unsigned long bootMs = millis();
    environmentProvider.begin(bootMs, logf);
    airQualityProvider.begin(bootMs, logf);
    lightProvider.begin(bootMs, logf);
    presenceProvider.begin(logf);
}

void initialisiereFunk() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.setSleep(false);

    esp_err_t kanalErr = esp_wifi_set_channel((uint8_t)WLAN_KANAL, WIFI_SECOND_CHAN_NONE);
    if (kanalErr != ESP_OK) {
        logf("WARN", "WLAN-Kanal %d konnte nicht gesetzt werden (err=%d)", WLAN_KANAL, (int)kanalErr);
    }

    if (esp_now_init() != ESP_OK) {
        logf("WARN", "ESP-NOW Initialisierung fehlgeschlagen");
        return;
    }

    if (!stellePeerSicher(BROADCAST_MAC)) {
        logf("WARN", "Broadcast-Peer konnte nicht vorbereitet werden");
    }
    logf("INFO", "ESP-NOW bereit auf Kanal %d", WLAN_KANAL);
}

void gibStartmeldungAus() {
    if (!DEBUG_LOKAL_AKTIV) return;

    Serial.println("================================");
    Serial.println(PROJECT_NAME);
    Serial.print(DATEI_GERAET);
    Serial.print(" v");
    Serial.println(DATEI_VERSION);
    Serial.print("Node-ID: ");
    Serial.println(DEVICE_ID);
    Serial.print("Name: ");
    Serial.println(provisioning.deviceName());
    Serial.print("FW: ");
    Serial.println(PROJECT_VERSION);
    Serial.print("Umgebung: ");
    Serial.println(environmentProvider.name());
    Serial.print("Luftqualitaet: ");
    Serial.println(airQualityProvider.name());
    Serial.print("Lux: ");
    Serial.println(lightProvider.name());
    Serial.print("Presence: ");
    Serial.println(presenceProvider.name());
    Serial.print("State-Format: ");
    Serial.println(NET_SEN_ERWEITERTER_STATE ? "extended" : "baseline");
    Serial.print("Setup-Report-Intervall: ");
    Serial.print(provisioning.reportIntervalSeconds());
    Serial.println(" s (persistiert)");
    Serial.println("================================");
}

void aktualisiereSensoren() {
    const unsigned long jetzt = millis();
    const bool motionVorher = nodeStatus.sensor.motion;
    const bool faultVorher = nodeStatus.sensor.fault;

    const SmartHome::ShSensors::ProviderUpdate envUpdate =
        environmentProvider.poll(jetzt, nodeStatus.sensor, logf);
    const SmartHome::ShSensors::ProviderUpdate lightUpdate =
        lightProvider.poll(jetzt, nodeStatus.sensor, logf);
    const SmartHome::ShSensors::ProviderUpdate airUpdate =
        airQualityProvider.poll(jetzt, nodeStatus.sensor, logf);
    const SmartHome::ShSensors::ProviderUpdate presenceUpdate =
        presenceProvider.poll(nodeStatus.sensor.presence, logf);
    const bool motionChanged =
        SmartHome::ShSensors::syncNetSenMotionFromPresence(nodeStatus.sensor);

    const bool faultJetzt = envUpdate.fault || lightUpdate.fault || airUpdate.fault;
    nodeStatus.sensor.fault = faultJetzt;

    if (envUpdate.changed ||
        lightUpdate.changed ||
        airUpdate.changed ||
        presenceUpdate.changed ||
        motionChanged ||
        faultVorher != faultJetzt) {
        nodeStatus.state_report_offen = true;
    }

    if (!motionVorher && nodeStatus.sensor.motion) {
        sendeEvent(SH_EVENT_MOTION_DETECTED, SH_TRIGGER_AUTO, 0U, 0U);
    }
}

void verarbeiteHelloAck(const uint8_t* senderMac, const SmartHome::HelloAckPayload& payload) {
    if (payload.ack_status != SH_ACK_OK) {
        logf("WARN", "HELLO_ACK abgelehnt");
        return;
    }

    memcpy(nodeStatus.master_mac, senderMac, 6);
    nodeStatus.master_mac_gueltig = true;
    nodeStatus.master_bekannt = true;
    provisioning.persistMasterMac(senderMac);
    stellePeerSicher(nodeStatus.master_mac);
    logf("INFO", "HELLO_ACK empfangen, Master-Kanal=%u", payload.channel);
    logMac("Master-MAC: ", senderMac);
    nodeStatus.state_report_offen = sensorStateBereit();
}

void verarbeiteCmd(const SmartHome::CmdPayload& cmd) {
    if (cmd.cmd_type == SH_CMD_STATE_REQUEST) {
        nodeStatus.state_report_offen = true;
        return;
    }

    logf("WARN", "CMD ignoriert (cmd=%u)", (unsigned)cmd.cmd_type);
}

void verarbeiteEspNowPaket(const uint8_t* senderMac, const uint8_t* daten, int laenge) {
    if (senderMac == nullptr || daten == nullptr || laenge < (int)sizeof(SmartHome::MsgHeader)) {
        logf("WARN", "ESP-NOW Paket verworfen: ungueltige Eingabe");
        return;
    }

    if (!SmartHome::hasValidPacketCrc(daten, (size_t)laenge)) {
        logf("WARN", "ESP-NOW Paket verworfen: CRC/Header ungueltig");
        return;
    }

    const SmartHome::MsgHeader* header = reinterpret_cast<const SmartHome::MsgHeader*>(daten);
    const uint8_t* payload = daten + SH_HEADER_SIZE;

    switch (header->msg_type) {
        case SH_MSG_HELLO_ACK:
            if (header->payload_len == sizeof(SmartHome::HelloAckPayload)) {
                verarbeiteHelloAck(senderMac, *reinterpret_cast<const SmartHome::HelloAckPayload*>(payload));
            }
            break;

        case SH_MSG_CMD:
            if (header->payload_len == sizeof(SmartHome::CmdPayload)) {
                verarbeiteCmd(*reinterpret_cast<const SmartHome::CmdPayload*>(payload));
            }
            break;

        default:
            logf("WARN", "ESP-NOW Nachricht ignoriert (msg_type=%u)", (unsigned)header->msg_type);
            break;
    }
}

#if ESP_ARDUINO_VERSION_MAJOR >= 3
void onEspNowReceive(const esp_now_recv_info_t* info, const uint8_t* daten, int laenge) {
    if (info == nullptr) return;
    verarbeiteEspNowPaket(info->src_addr, daten, laenge);
}
#else
void onEspNowReceive(const uint8_t* senderMac, const uint8_t* daten, int laenge) {
    verarbeiteEspNowPaket(senderMac, daten, laenge);
}
#endif

void onEspNowSent(const uint8_t* mac, esp_now_send_status_t status) {
    char macText[18] = {0};
    if (mac != nullptr) {
        SmartHome::macToString(mac, macText);
    } else {
        copyText(macText, sizeof(macText), "unbekannt");
    }

    logf(
        status == ESP_NOW_SEND_SUCCESS ? "INFO" : "WARN",
        "ESP-NOW Sendestatus an %s: %s",
        macText,
        status == ESP_NOW_SEND_SUCCESS ? "OK" : "FEHLER");
}

void setup() {
    if (DEBUG_LOKAL_AKTIV) {
        Serial.begin(115200);
        delay(150);
    }

    nodeStatus = {};
    provisioning.begin(
        {
            DEVICE_ID,
            DEVICE_NAME,
            STATE_INTERVAL_MS / 1000UL,
            STATE_INTERVAL_MS / 1000UL,
            true,
            false,
            WLAN_KANAL,
            PIN_BOOT_BUTTON,
            true,
            SETUP_REENTRY_HOLD_MS,
            FACTORY_RESET_HOLD_MS,
        },
        logProvisioning);
    uebernehmeProvisionierteMasterMac();
    logf("INFO", "setup() gestartet");
    initialisiereHardware();
    gibStartmeldungAus();

    if (provisioning.isSetupModeActive()) {
        logf("WARN", "Setup-Modus aktiv. Funkpfad wartet auf lokale Provisionierung.");
        return;
    }

    logf("INFO", "Initialisiere Funk");
    initialisiereFunk();

    esp_now_register_send_cb(onEspNowSent);
    esp_now_register_recv_cb(onEspNowReceive);

    if (!sendeHello()) {
        logf("WARN", "Erstes HELLO konnte lokal nicht gesendet werden");
    }
}

void loop() {
    provisioning.update();
    if (provisioning.isSetupModeActive()) {
        delay(LOOP_INTERVAL_MS);
        return;
    }

    const unsigned long jetzt = millis();

    aktualisiereSensoren();

    if (!nodeStatus.master_bekannt) {
        if ((jetzt - nodeStatus.letztes_hello_ms) >= HELLO_RETRY_INTERVAL_MS) {
            sendeHello();
        }
    } else if ((jetzt - nodeStatus.letzter_heartbeat_ms) >= HEARTBEAT_INTERVAL_MS) {
        sendeHeartbeat();
    }

    if (sensorStateBereit() &&
        !nodeStatus.master_mac_gueltig &&
        nodeStatus.state_report_offen &&
        (jetzt - nodeStatus.letzter_state_wait_log_ms) >= HELLO_RETRY_INTERVAL_MS) {
        nodeStatus.letzter_state_wait_log_ms = jetzt;
        logf("WARN", "STATE_REPORT geparkt: Master-MAC unbekannt");
    }

    if (nodeStatus.master_mac_gueltig &&
        sensorStateBereit() &&
        (nodeStatus.state_report_offen || (jetzt - nodeStatus.letzter_state_ms) >= STATE_INTERVAL_MS)) {
        sendeStateReport();
    }

    delay(LOOP_INTERVAL_MS);
}
