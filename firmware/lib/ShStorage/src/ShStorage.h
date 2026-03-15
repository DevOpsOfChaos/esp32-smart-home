/*
====================================================================
 Projekt   : SmartHome ESP32
 Datei     : ShStorage.h
 Modul     : ShStorage
 Version   : 0.1.0
 Stand     : 2026-03-15

 Funktion:
 Gemeinsamer persistenter Speicher fuer Node-Setup und
 Basis-Provisionierung.

 Dieser Stand speichert bewusst nur das gemeinsame Fundament:
 - Master-MAC-Bindung
 - lokaler Anzeigename
 - einfache Grundwerte fuer spaetere Runtime-Konfiguration

 Nicht Teil dieses Moduls:
 - MQTT- oder Server-Konfiguration in Nodes
 - grosse per-Geraet-Sonderstrukturen
 - ausufernde Versionierungs- oder Migrationslogik
====================================================================
*/

#pragma once

#include <Arduino.h>
#include <Preferences.h>

#include <Protocol.h>

namespace SmartHome {
namespace ShStorage {

constexpr uint32_t SH_NODE_SETTINGS_MAGIC = 0x53484E50UL;
constexpr uint16_t SH_NODE_SETTINGS_VERSION = 1U;
constexpr uint32_t SH_NODE_SETTINGS_FLAG_MASTER_BOUND = 0x00000001UL;

struct SharedNodeSettings {
    uint32_t magic;
    uint16_t version;
    uint16_t reserved;
    uint32_t flags;
    uint8_t master_mac[6];
    uint8_t reserved_mac[2];
    char device_name[SH_DEVICE_NAME_LEN];
    uint32_t report_interval_s;
    uint32_t wake_interval_s;
};

void makeDefaultSettings(
    SharedNodeSettings& settings,
    const char* defaultDeviceName,
    uint32_t defaultReportIntervalS,
    uint32_t defaultWakeIntervalS);

bool isSettingsStructValid(const SharedNodeSettings& settings);
bool hasStoredMasterMac(const SharedNodeSettings& settings);
bool setStoredMasterMac(SharedNodeSettings& settings, const uint8_t* masterMac);
void clearStoredMasterMac(SharedNodeSettings& settings);
void setStoredDeviceName(SharedNodeSettings& settings, const char* deviceName);
bool parseMacText(const char* text, uint8_t outMac[6]);
void formatMacText(const uint8_t* mac, char* buffer, size_t bufferSize);

class SharedNodeStorage {
  public:
    SharedNodeStorage() = default;

    bool load(
        SharedNodeSettings& settings,
        const char* defaultDeviceName,
        uint32_t defaultReportIntervalS,
        uint32_t defaultWakeIntervalS);

    bool save(const SharedNodeSettings& settings);
    bool factoryReset();
};

}  // namespace ShStorage
}  // namespace SmartHome
