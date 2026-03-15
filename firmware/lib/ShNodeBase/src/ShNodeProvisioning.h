/*
====================================================================
 Projekt   : SmartHome ESP32
 Datei     : ShNodeProvisioning.h
 Modul     : ShNodeBase
 Version   : 0.1.0
 Stand     : 2026-03-15

 Funktion:
 Gemeinsame Setup-, Provisionierungs- und Reset-Basis fuer Nodes.

 Dieser Stand liefert bewusst das Grundgeruest:
 - persistente Master-Bindung ueber ShStorage
 - Boot-Taster-Haltezeiten fuer Setup-Reentry und Factory Reset
 - kleinen lokalen Setup-AP mit minimaler Webform

 Nicht Teil dieses Standes:
 - Captive-Portal-Komfortfunktionen
 - grosse per-Geraet-Konfigurationsseiten
 - vollstaendige Runtime-Nutzung aller gespeicherten Basiswerte
====================================================================
*/

#pragma once

#include <Arduino.h>
#include <WebServer.h>

#include <ShStorage.h>

namespace SmartHome {
namespace ShNodeBase {

using SetupLogFn = void (*)(const char* level, const char* message);

enum SetupStartReason : uint8_t {
    SETUP_REASON_NONE = 0U,
    SETUP_REASON_MISSING_MASTER = 1U,
    SETUP_REASON_BUTTON_REENTRY = 2U,
    SETUP_REASON_FACTORY_RESET = 3U,
};

struct NodeProvisioningConfig {
    const char* device_id;
    const char* default_device_name;
    uint32_t default_report_interval_s;
    uint32_t default_wake_interval_s;
    bool allow_report_interval_edit;
    bool allow_wake_interval_edit;
    int wifi_channel;
    int boot_button_pin;
    bool boot_button_active_low;
    unsigned long setup_hold_ms;
    unsigned long factory_reset_hold_ms;
};

class NodeProvisioning {
  public:
    NodeProvisioning();

    bool begin(const NodeProvisioningConfig& config, SetupLogFn logFn = nullptr);
    void update();

    bool isSetupModeActive() const;
    SetupStartReason startReason() const;
    const ShStorage::SharedNodeSettings& settings() const;
    bool hasStoredMasterMac() const;
    const uint8_t* masterMac() const;
    const char* deviceName() const;
    uint32_t reportIntervalSeconds() const;
    uint32_t wakeIntervalSeconds() const;
    bool persistMasterMac(const uint8_t* masterMac);

  private:
    void configureRoutes();
    void updateBootButton(unsigned long nowMs);
    bool readBootButton() const;
    void enterSetupMode(SetupStartReason reason);
    String buildSetupPage(
        const String& masterMacText,
        const String& deviceNameText,
        uint32_t reportIntervalS,
        uint32_t wakeIntervalValue,
        const char* wakeIntervalUnit,
        const String& infoText,
        const String& errorText) const;
    void handleRoot();
    void handleSave();
    void sendForm(
        const String& masterMacText,
        const String& deviceNameText,
        uint32_t reportIntervalS,
        uint32_t wakeIntervalValue,
        const char* wakeIntervalUnit,
        const String& infoText,
        const String& errorText,
        int statusCode = 200);
    void log(const char* level, const char* format, ...) const;

    NodeProvisioningConfig config_;
    SetupLogFn log_fn_;
    ShStorage::SharedNodeStorage storage_;
    ShStorage::SharedNodeSettings settings_;
    WebServer server_;
    bool initialized_;
    bool setup_mode_active_;
    bool routes_configured_;
    bool button_last_pressed_;
    bool setup_hold_triggered_;
    bool factory_reset_triggered_;
    bool restart_pending_;
    unsigned long button_pressed_since_ms_;
    unsigned long restart_requested_at_ms_;
    SetupStartReason start_reason_;
};

}  // namespace ShNodeBase
}  // namespace SmartHome
