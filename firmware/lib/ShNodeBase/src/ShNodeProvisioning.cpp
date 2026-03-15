/*
====================================================================
 Projekt   : SmartHome ESP32
 Datei     : ShNodeProvisioning.cpp
 Modul     : ShNodeBase
 Version   : 0.1.0
 Stand     : 2026-03-15

 Funktion:
 Gemeinsame Setup-/Provisionierungslogik fuer alle Nodes.
====================================================================
*/

#include "ShNodeProvisioning.h"

#include <WiFi.h>
#include <esp_now.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

namespace SmartHome {
namespace ShNodeBase {

namespace {

constexpr unsigned long RESTART_DELAY_MS = 1500UL;
constexpr uint32_t MIN_INTERVAL_S = 1UL;
constexpr uint32_t MAX_INTERVAL_S = 604800UL;

String htmlEscape(const String& text) {
    String escaped;
    escaped.reserve(text.length() + 16U);

    for (size_t i = 0U; i < text.length(); ++i) {
        const char current = text[i];
        switch (current) {
            case '&': escaped += F("&amp;"); break;
            case '<': escaped += F("&lt;"); break;
            case '>': escaped += F("&gt;"); break;
            case '"': escaped += F("&quot;"); break;
            case '\'': escaped += F("&#39;"); break;
            default: escaped += current; break;
        }
    }

    return escaped;
}

String reasonText(SetupStartReason reason) {
    switch (reason) {
        case SETUP_REASON_MISSING_MASTER:
            return F("Keine gespeicherte Master-MAC gefunden.");
        case SETUP_REASON_BUTTON_REENTRY:
            return F("Setup-Modus ueber Boot-Taster erneut geoeffnet.");
        case SETUP_REASON_FACTORY_RESET:
            return F("Factory Reset ausgefuehrt. Gemeinsame Basiswerte wurden auf Defaults gesetzt.");
        default:
            return F("Setup-Modus aktiv.");
    }
}

void splitWakeInterval(uint32_t wakeIntervalS, uint32_t& valueOut, const char*& unitOut) {
    if (wakeIntervalS >= 3600UL && (wakeIntervalS % 3600UL) == 0UL) {
        valueOut = wakeIntervalS / 3600UL;
        unitOut = "hours";
        return;
    }

    valueOut = (wakeIntervalS >= 60UL) ? (wakeIntervalS / 60UL) : 1UL;
    if (valueOut == 0UL) {
        valueOut = 1UL;
    }
    unitOut = "minutes";
}

bool parsePositiveUInt32(const String& text, uint32_t minimum, uint32_t maximum, uint32_t& outValue) {
    String trimmed = text;
    trimmed.trim();

    if (trimmed.length() == 0U) {
        return false;
    }

    for (size_t i = 0U; i < trimmed.length(); ++i) {
        if (!isDigit(trimmed[i])) {
            return false;
        }
    }

    const unsigned long parsed = strtoul(trimmed.c_str(), nullptr, 10);
    if (parsed < minimum || parsed > maximum) {
        return false;
    }

    outValue = (uint32_t)parsed;
    return true;
}

}  // namespace

NodeProvisioning::NodeProvisioning()
    : config_{},
      log_fn_(nullptr),
      settings_{},
      server_(80),
      initialized_(false),
      setup_mode_active_(false),
      routes_configured_(false),
      button_last_pressed_(false),
      setup_hold_triggered_(false),
      factory_reset_triggered_(false),
      restart_pending_(false),
      button_pressed_since_ms_(0UL),
      restart_requested_at_ms_(0UL),
      start_reason_(SETUP_REASON_NONE) {}

bool NodeProvisioning::begin(const NodeProvisioningConfig& config, SetupLogFn logFn) {
    config_ = config;
    log_fn_ = logFn;
    initialized_ = true;
    setup_mode_active_ = false;
    start_reason_ = SETUP_REASON_NONE;
    button_last_pressed_ = false;
    setup_hold_triggered_ = false;
    factory_reset_triggered_ = false;
    restart_pending_ = false;
    button_pressed_since_ms_ = 0UL;
    restart_requested_at_ms_ = 0UL;

    if (config_.boot_button_pin >= 0) {
        pinMode(config_.boot_button_pin, INPUT_PULLUP);
    }

    const bool loaded = storage_.load(
        settings_,
        config_.default_device_name,
        config_.default_report_interval_s,
        config_.default_wake_interval_s);

    if (loaded) {
        log("INFO", "Gemeinsame Settings aus NVS geladen.");
    } else {
        log("INFO", "Keine validen gemeinsamen Settings gefunden. Defaults aktiv.");
    }

    if (!ShStorage::hasStoredMasterMac(settings_)) {
        enterSetupMode(SETUP_REASON_MISSING_MASTER);
    }

    return true;
}

void NodeProvisioning::configureRoutes() {
    if (routes_configured_) return;

    server_.on("/", HTTP_GET, [this]() { handleRoot(); });
    server_.on("/save", HTTP_POST, [this]() { handleSave(); });
    server_.onNotFound([this]() { handleRoot(); });

    routes_configured_ = true;
}

void NodeProvisioning::log(const char* level, const char* format, ...) const {
    if (log_fn_ == nullptr || format == nullptr) return;

    char message[224];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    log_fn_(level, message);
}

bool NodeProvisioning::readBootButton() const {
    if (config_.boot_button_pin < 0) {
        return false;
    }

    const int rawLevel = digitalRead(config_.boot_button_pin);
    if (config_.boot_button_active_low) {
        return rawLevel == LOW;
    }
    return rawLevel == HIGH;
}

void NodeProvisioning::updateBootButton(unsigned long nowMs) {
    const bool pressed = readBootButton();

    if (!pressed) {
        button_last_pressed_ = false;
        setup_hold_triggered_ = false;
        factory_reset_triggered_ = false;
        button_pressed_since_ms_ = 0UL;
        return;
    }

    if (!button_last_pressed_) {
        button_last_pressed_ = true;
        button_pressed_since_ms_ = nowMs;
        log("INFO", "Boot-Taster erkannt. Haltezeit gestartet.");
        return;
    }

    const unsigned long holdTimeMs = nowMs - button_pressed_since_ms_;

    if (!setup_hold_triggered_ && holdTimeMs >= config_.setup_hold_ms) {
        setup_hold_triggered_ = true;
        log("WARN", "Boot-Taster %lu ms gehalten. Setup-Reentry wird geoeffnet.", config_.setup_hold_ms);
        enterSetupMode(SETUP_REASON_BUTTON_REENTRY);
    }

    if (!factory_reset_triggered_ && holdTimeMs >= config_.factory_reset_hold_ms) {
        factory_reset_triggered_ = true;
        log("WARN", "Boot-Taster %lu ms gehalten. Factory Reset wird ausgefuehrt.", config_.factory_reset_hold_ms);
        enterSetupMode(SETUP_REASON_FACTORY_RESET);
    }
}

void NodeProvisioning::enterSetupMode(SetupStartReason reason) {
    if (!initialized_) return;

    if (reason == SETUP_REASON_FACTORY_RESET) {
        if (!storage_.factoryReset()) {
            log("WARN", "Factory Reset konnte den NVS-Blob nicht sauber entfernen.");
        }
        ShStorage::makeDefaultSettings(
            settings_,
            config_.default_device_name,
            config_.default_report_interval_s,
            config_.default_wake_interval_s);
    }

    start_reason_ = reason;
    restart_pending_ = false;

    if (!setup_mode_active_) {
        esp_now_deinit();
        WiFi.persistent(false);
        WiFi.disconnect(true, true);
        WiFi.mode(WIFI_MODE_NULL);
        delay(25);

        configureRoutes();
        const String ssid = String(F("SH-SETUP-")) + String(config_.device_id ? config_.device_id : "node");
        setup_mode_active_ = true;

        WiFi.mode(WIFI_AP);
        if (!WiFi.softAP(ssid.c_str(), nullptr, config_.wifi_channel)) {
            log("WARN", "Setup-AP konnte nicht gestartet werden.");
        } else {
            server_.begin();
            log(
                "INFO",
                "Setup-AP aktiv: ssid=%s ip=%s",
                ssid.c_str(),
                WiFi.softAPIP().toString().c_str());
        }
    } else {
        log("INFO", "Setup-Modus bleibt aktiv: %s", reasonText(reason).c_str());
    }
}

String NodeProvisioning::buildSetupPage(
    const String& masterMacText,
    const String& deviceNameText,
    uint32_t reportIntervalS,
    uint32_t wakeIntervalValue,
    const char* wakeIntervalUnit,
    const String& infoText,
    const String& errorText) const {
    String page;
    page.reserve(2600U);

    const String escapedMasterMac = htmlEscape(masterMacText);
    const String escapedDeviceName = htmlEscape(deviceNameText);
    const String reason = htmlEscape(reasonText(start_reason_));
    const String deviceId = htmlEscape(String(config_.device_id ? config_.device_id : "node"));
    const String ipText = htmlEscape(WiFi.softAPIP().toString());
    const bool wakeHours = wakeIntervalUnit != nullptr && strcmp(wakeIntervalUnit, "hours") == 0;

    page += F("<!doctype html><html><head><meta charset=\"utf-8\">");
    page += F("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
    page += F("<title>Node Setup</title>");
    page += F("<style>body{font-family:Arial,sans-serif;background:#f4f1ea;color:#1f2328;margin:0;padding:24px;}main{max-width:720px;margin:0 auto;background:#fff;padding:24px;border-radius:16px;box-shadow:0 10px 30px rgba(0,0,0,.08);}h1{margin-top:0;}label{display:block;font-weight:bold;margin-top:16px;}input,select{width:100%;padding:10px;margin-top:6px;border:1px solid #c7c2b8;border-radius:10px;box-sizing:border-box;}button{margin-top:20px;padding:12px 16px;border:0;border-radius:999px;background:#31493c;color:#fff;font-weight:bold;cursor:pointer;}small,p.meta{color:#5b6168;}div.info{background:#eaf5ee;border:1px solid #b5d7c2;padding:12px;border-radius:10px;margin:16px 0;}div.error{background:#fbeaea;border:1px solid #e5b4b4;padding:12px;border-radius:10px;margin:16px 0;}code{background:#f0ece4;padding:2px 5px;border-radius:6px;}</style></head><body><main>");
    page += F("<h1>Node Setup</h1>");
    page += F("<p class=\"meta\">Geraet: <code>");
    page += deviceId;
    page += F("</code><br>Setup-IP: <code>");
    page += ipText;
    page += F("</code></p>");
    page += F("<p>");
    page += reason;
    page += F("</p>");
    page += F("<p class=\"meta\">Dieser Stand ist bewusst nur das Grundgeruest. Die Seite speichert bereits die Master-Bindung und einfache Basiswerte. Ein grosses Captive-Portal oder per-Geraet-Sonderseiten sind hier absichtlich noch nicht gebaut.</p>");

    if (infoText.length() > 0U) {
        page += F("<div class=\"info\">");
        page += htmlEscape(infoText);
        page += F("</div>");
    }

    if (errorText.length() > 0U) {
        page += F("<div class=\"error\">");
        page += htmlEscape(errorText);
        page += F("</div>");
    }

    page += F("<form method=\"post\" action=\"/save\">");
    page += F("<label for=\"master_mac\">Master-MAC</label>");
    page += F("<input id=\"master_mac\" name=\"master_mac\" value=\"");
    page += escapedMasterMac;
    page += F("\" placeholder=\"AA:BB:CC:DD:EE:FF\" required>");
    page += F("<small>GET-Parameter wie <code>?master_mac=AA:BB:CC:DD:EE:FF</code> werden nur vorbefuellt und erst nach Speichern uebernommen.</small>");

    page += F("<label for=\"device_name\">Geraetename</label>");
    page += F("<input id=\"device_name\" name=\"device_name\" value=\"");
    page += escapedDeviceName;
    page += F("\" maxlength=\"31\">");
    page += F("<small>Wird sofort als Anzeigename im HELLO-Payload genutzt.</small>");

    if (config_.allow_report_interval_edit) {
        page += F("<label for=\"report_interval_s\">Report-Intervall (Sekunden)</label>");
        page += F("<input id=\"report_interval_s\" name=\"report_interval_s\" type=\"number\" min=\"1\" max=\"604800\" value=\"");
        page += String(reportIntervalS);
        page += F("\">");
        page += F("<small>Wird in diesem Stand persistent gespeichert. Der volle Runtime-Hook bleibt bewusst ein spaeterer Schritt.</small>");
    }

    if (config_.allow_wake_interval_edit) {
        page += F("<label for=\"wake_interval_value\">Sleep-/Wake-Intervall</label>");
        page += F("<input id=\"wake_interval_value\" name=\"wake_interval_value\" type=\"number\" min=\"1\" max=\"10080\" value=\"");
        page += String(wakeIntervalValue);
        page += F("\">");
        page += F("<label for=\"wake_interval_unit\">Einheit</label>");
        page += F("<select id=\"wake_interval_unit\" name=\"wake_interval_unit\">");
        page += F("<option value=\"minutes\"");
        if (!wakeHours) page += F(" selected");
        page += F(">Minuten</option>");
        page += F("<option value=\"hours\"");
        if (wakeHours) page += F(" selected");
        page += F(">Stunden</option></select>");
        page += F("<small>Die Speicherung erfolgt intern in Sekunden. Die spaetere `bat_sen`-Runtime bleibt separat offen.</small>");
    }

    page += F("<button type=\"submit\">Speichern und neu starten</button></form>");
    page += F("</main></body></html>");
    return page;
}

void NodeProvisioning::sendForm(
    const String& masterMacText,
    const String& deviceNameText,
    uint32_t reportIntervalS,
    uint32_t wakeIntervalValue,
    const char* wakeIntervalUnit,
    const String& infoText,
    const String& errorText,
    int statusCode) {
    server_.send(
        statusCode,
        "text/html; charset=utf-8",
        buildSetupPage(
            masterMacText,
            deviceNameText,
            reportIntervalS,
            wakeIntervalValue,
            wakeIntervalUnit,
            infoText,
            errorText));
}

void NodeProvisioning::handleRoot() {
    char storedMacText[18] = {0};
    ShStorage::formatMacText(settings_.master_mac, storedMacText, sizeof(storedMacText));

    String masterMacText = String(storedMacText);
    String infoText;
    String errorText;

    if (server_.hasArg("master_mac")) {
        masterMacText = server_.arg("master_mac");
        uint8_t parsedMac[6] = {0};
        if (ShStorage::parseMacText(masterMacText.c_str(), parsedMac)) {
            infoText = F("Master-MAC aus Link uebernommen. Bitte pruefen und erst dann speichern.");
        } else {
            errorText = F("Die uebergebene Master-MAC ist ungueltig und wurde nicht uebernommen.");
        }
    }

    uint32_t wakeIntervalValue = 0UL;
    const char* wakeIntervalUnit = "minutes";
    splitWakeInterval(settings_.wake_interval_s, wakeIntervalValue, wakeIntervalUnit);

    sendForm(
        masterMacText,
        String(settings_.device_name),
        settings_.report_interval_s,
        wakeIntervalValue,
        wakeIntervalUnit,
        infoText,
        errorText);
}

void NodeProvisioning::handleSave() {
    String masterMacText = server_.arg("master_mac");
    String deviceNameText = server_.arg("device_name");
    deviceNameText.trim();

    if (deviceNameText.length() == 0U) {
        deviceNameText = String(config_.default_device_name ? config_.default_device_name : "Node");
    }

    uint8_t parsedMasterMac[6] = {0};
    if (!ShStorage::parseMacText(masterMacText.c_str(), parsedMasterMac)) {
        uint32_t wakeValue = 0UL;
        const char* wakeUnit = "minutes";
        splitWakeInterval(settings_.wake_interval_s, wakeValue, wakeUnit);
        sendForm(
            masterMacText,
            deviceNameText,
            settings_.report_interval_s,
            wakeValue,
            wakeUnit,
            String(),
            F("Master-MAC ist ungueltig. Erwartet wird z. B. AA:BB:CC:DD:EE:FF."),
            400);
        return;
    }

    uint32_t reportIntervalS = settings_.report_interval_s;
    if (config_.allow_report_interval_edit) {
        if (!parsePositiveUInt32(server_.arg("report_interval_s"), MIN_INTERVAL_S, MAX_INTERVAL_S, reportIntervalS)) {
            uint32_t wakeValue = 0UL;
            const char* wakeUnit = "minutes";
            splitWakeInterval(settings_.wake_interval_s, wakeValue, wakeUnit);
            sendForm(
                masterMacText,
                deviceNameText,
                settings_.report_interval_s,
                wakeValue,
                wakeUnit,
                String(),
                F("Report-Intervall muss zwischen 1 und 604800 Sekunden liegen."),
                400);
            return;
        }
    }

    uint32_t wakeIntervalS = settings_.wake_interval_s;
    uint32_t wakeIntervalValue = 0UL;
    const char* wakeIntervalUnit = "minutes";

    if (config_.allow_wake_interval_edit) {
        if (!parsePositiveUInt32(server_.arg("wake_interval_value"), 1UL, 10080UL, wakeIntervalValue)) {
            sendForm(
                masterMacText,
                deviceNameText,
                reportIntervalS,
                1UL,
                "minutes",
                String(),
                F("Sleep-/Wake-Intervall muss als positiver Minuten- oder Stundenwert angegeben werden."),
                400);
            return;
        }

        const String requestedUnit = server_.arg("wake_interval_unit");
        if (requestedUnit == "hours") {
            wakeIntervalUnit = "hours";
            wakeIntervalS = wakeIntervalValue * 3600UL;
        } else {
            wakeIntervalUnit = "minutes";
            wakeIntervalS = wakeIntervalValue * 60UL;
        }

        if (wakeIntervalS < 60UL || wakeIntervalS > MAX_INTERVAL_S) {
            sendForm(
                masterMacText,
                deviceNameText,
                reportIntervalS,
                wakeIntervalValue,
                wakeIntervalUnit,
                String(),
                F("Sleep-/Wake-Intervall liegt ausserhalb des erlaubten Bereichs."),
                400);
            return;
        }
    } else {
        splitWakeInterval(settings_.wake_interval_s, wakeIntervalValue, wakeIntervalUnit);
    }

    ShStorage::setStoredMasterMac(settings_, parsedMasterMac);
    ShStorage::setStoredDeviceName(settings_, deviceNameText.c_str());
    settings_.report_interval_s = reportIntervalS;
    settings_.wake_interval_s = wakeIntervalS;

    if (!storage_.save(settings_)) {
        sendForm(
            masterMacText,
            deviceNameText,
            reportIntervalS,
            wakeIntervalValue,
            wakeIntervalUnit,
            String(),
            F("Speichern in NVS fehlgeschlagen."),
            500);
        return;
    }

    restart_pending_ = true;
    restart_requested_at_ms_ = millis();

    sendForm(
        masterMacText,
        deviceNameText,
        reportIntervalS,
        wakeIntervalValue,
        wakeIntervalUnit,
        F("Setup-Daten gespeichert. Der Node startet gleich neu und nutzt danach die persistierte Master-Bindung."),
        String(),
        200);

    log("INFO", "Setup-Daten gespeichert. Neustart angefordert.");
}

void NodeProvisioning::update() {
    if (!initialized_) return;

    const unsigned long nowMs = millis();
    updateBootButton(nowMs);

    if (setup_mode_active_) {
        server_.handleClient();

        if (restart_pending_ && (nowMs - restart_requested_at_ms_) >= RESTART_DELAY_MS) {
            log("INFO", "Setup-Neustart wird ausgefuehrt.");
            ESP.restart();
        }
    }
}

bool NodeProvisioning::isSetupModeActive() const {
    return setup_mode_active_;
}

SetupStartReason NodeProvisioning::startReason() const {
    return start_reason_;
}

const ShStorage::SharedNodeSettings& NodeProvisioning::settings() const {
    return settings_;
}

bool NodeProvisioning::hasStoredMasterMac() const {
    return ShStorage::hasStoredMasterMac(settings_);
}

const uint8_t* NodeProvisioning::masterMac() const {
    return hasStoredMasterMac() ? settings_.master_mac : nullptr;
}

const char* NodeProvisioning::deviceName() const {
    return settings_.device_name;
}

uint32_t NodeProvisioning::reportIntervalSeconds() const {
    return settings_.report_interval_s;
}

uint32_t NodeProvisioning::wakeIntervalSeconds() const {
    return settings_.wake_interval_s;
}

bool NodeProvisioning::persistMasterMac(const uint8_t* masterMac) {
    if (masterMac == nullptr || !SmartHome::isValidMac(masterMac)) {
        return false;
    }

    if (ShStorage::hasStoredMasterMac(settings_) &&
        memcmp(settings_.master_mac, masterMac, sizeof(settings_.master_mac)) == 0) {
        return true;
    }

    if (!ShStorage::setStoredMasterMac(settings_, masterMac)) {
        return false;
    }

    const bool saved = storage_.save(settings_);
    if (saved) {
        log("INFO", "Master-MAC aus bestaetigter Laufzeitkommunikation persistiert.");
    } else {
        log("WARN", "Persistieren der bestaetigten Master-MAC fehlgeschlagen.");
    }
    return saved;
}

}  // namespace ShNodeBase
}  // namespace SmartHome
