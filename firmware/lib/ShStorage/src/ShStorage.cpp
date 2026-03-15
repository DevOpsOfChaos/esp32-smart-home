/*
====================================================================
 Projekt   : SmartHome ESP32
 Datei     : ShStorage.cpp
 Modul     : ShStorage
 Version   : 0.1.0
 Stand     : 2026-03-15

 Funktion:
 NVS-Implementierung fuer die gemeinsame Node-Setup-Struktur.
====================================================================
*/

#include "ShStorage.h"

#include <ctype.h>
#include <string.h>

namespace SmartHome {
namespace ShStorage {

namespace {

constexpr char NVS_NAMESPACE[] = "sh_node";
constexpr char NVS_KEY_SETTINGS[] = "settings";

void copyText(char* target, size_t targetSize, const char* source) {
    if (!target || targetSize == 0U) return;
    if (!source) {
        target[0] = '\0';
        return;
    }

    strncpy(target, source, targetSize - 1U);
    target[targetSize - 1U] = '\0';
}

uint8_t decodeHexNibble(char value) {
    if (value >= '0' && value <= '9') return (uint8_t)(value - '0');
    if (value >= 'a' && value <= 'f') return (uint8_t)(10 + (value - 'a'));
    if (value >= 'A' && value <= 'F') return (uint8_t)(10 + (value - 'A'));
    return 0xFFU;
}

}  // namespace

void makeDefaultSettings(
    SharedNodeSettings& settings,
    const char* defaultDeviceName,
    uint32_t defaultReportIntervalS,
    uint32_t defaultWakeIntervalS) {
    settings = {};
    settings.magic = SH_NODE_SETTINGS_MAGIC;
    settings.version = SH_NODE_SETTINGS_VERSION;
    settings.reserved = 0U;
    settings.flags = 0U;
    memset(settings.master_mac, 0, sizeof(settings.master_mac));
    memset(settings.reserved_mac, 0, sizeof(settings.reserved_mac));
    copyText(settings.device_name, sizeof(settings.device_name), defaultDeviceName);
    settings.report_interval_s = defaultReportIntervalS;
    settings.wake_interval_s = defaultWakeIntervalS;
}

bool isSettingsStructValid(const SharedNodeSettings& settings) {
    return settings.magic == SH_NODE_SETTINGS_MAGIC &&
           settings.version == SH_NODE_SETTINGS_VERSION;
}

bool hasStoredMasterMac(const SharedNodeSettings& settings) {
    return (settings.flags & SH_NODE_SETTINGS_FLAG_MASTER_BOUND) != 0U &&
           SmartHome::isValidMac(settings.master_mac);
}

bool setStoredMasterMac(SharedNodeSettings& settings, const uint8_t* masterMac) {
    if (masterMac == nullptr || !SmartHome::isValidMac(masterMac)) {
        return false;
    }

    memcpy(settings.master_mac, masterMac, sizeof(settings.master_mac));
    settings.flags |= SH_NODE_SETTINGS_FLAG_MASTER_BOUND;
    return true;
}

void clearStoredMasterMac(SharedNodeSettings& settings) {
    memset(settings.master_mac, 0, sizeof(settings.master_mac));
    settings.flags &= ~SH_NODE_SETTINGS_FLAG_MASTER_BOUND;
}

void setStoredDeviceName(SharedNodeSettings& settings, const char* deviceName) {
    copyText(settings.device_name, sizeof(settings.device_name), deviceName);
}

bool parseMacText(const char* text, uint8_t outMac[6]) {
    if (text == nullptr || outMac == nullptr) return false;

    char normalized[13] = {0};
    size_t normalizedLen = 0U;

    for (size_t i = 0U; text[i] != '\0'; ++i) {
        const unsigned char current = (unsigned char)text[i];
        if (isxdigit(current)) {
            if (normalizedLen >= 12U) {
                return false;
            }
            normalized[normalizedLen++] = (char)current;
            continue;
        }

        if (current == ':' || current == '-' || isspace(current)) {
            continue;
        }

        return false;
    }

    if (normalizedLen != 12U) {
        return false;
    }

    for (size_t i = 0U; i < 6U; ++i) {
        const uint8_t hi = decodeHexNibble(normalized[i * 2U]);
        const uint8_t lo = decodeHexNibble(normalized[(i * 2U) + 1U]);
        if (hi == 0xFFU || lo == 0xFFU) {
            return false;
        }
        outMac[i] = (uint8_t)((hi << 4U) | lo);
    }

    return SmartHome::isValidMac(outMac);
}

void formatMacText(const uint8_t* mac, char* buffer, size_t bufferSize) {
    if (!buffer || bufferSize == 0U) return;

    if (mac == nullptr || !SmartHome::isValidMac(mac)) {
        buffer[0] = '\0';
        return;
    }

    char text[18] = {0};
    SmartHome::macToString(mac, text);
    copyText(buffer, bufferSize, text);
}

bool SharedNodeStorage::load(
    SharedNodeSettings& settings,
    const char* defaultDeviceName,
    uint32_t defaultReportIntervalS,
    uint32_t defaultWakeIntervalS) {
    makeDefaultSettings(settings, defaultDeviceName, defaultReportIntervalS, defaultWakeIntervalS);

    Preferences preferences;
    if (!preferences.begin(NVS_NAMESPACE, true)) {
        return false;
    }

    const size_t storedLength = preferences.getBytesLength(NVS_KEY_SETTINGS);
    if (storedLength != sizeof(SharedNodeSettings)) {
        preferences.end();
        return false;
    }

    SharedNodeSettings loaded = {};
    const size_t readLength = preferences.getBytes(NVS_KEY_SETTINGS, &loaded, sizeof(loaded));
    preferences.end();

    if (readLength != sizeof(SharedNodeSettings) || !isSettingsStructValid(loaded)) {
        return false;
    }

    loaded.device_name[SH_DEVICE_NAME_LEN - 1U] = '\0';
    if (!hasStoredMasterMac(loaded)) {
        clearStoredMasterMac(loaded);
    }

    settings = loaded;
    return true;
}

bool SharedNodeStorage::save(const SharedNodeSettings& settings) {
    SharedNodeSettings normalized = settings;
    normalized.magic = SH_NODE_SETTINGS_MAGIC;
    normalized.version = SH_NODE_SETTINGS_VERSION;
    normalized.reserved = 0U;
    memset(normalized.reserved_mac, 0, sizeof(normalized.reserved_mac));
    normalized.device_name[SH_DEVICE_NAME_LEN - 1U] = '\0';

    if (!hasStoredMasterMac(normalized)) {
        clearStoredMasterMac(normalized);
    }

    Preferences preferences;
    if (!preferences.begin(NVS_NAMESPACE, false)) {
        return false;
    }

    const size_t written = preferences.putBytes(NVS_KEY_SETTINGS, &normalized, sizeof(normalized));
    preferences.end();
    return written == sizeof(normalized);
}

bool SharedNodeStorage::factoryReset() {
    Preferences preferences;
    if (!preferences.begin(NVS_NAMESPACE, false)) {
        return false;
    }

    preferences.remove(NVS_KEY_SETTINGS);
    preferences.end();
    return true;
}

}  // namespace ShStorage
}  // namespace SmartHome
