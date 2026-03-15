#pragma once

#include <Arduino.h>
#include <DHT.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

#include "ProviderCommon.h"
#include "ProviderIds.h"
#include "PresenceProviders.h"
#include "../../ShProtocol/src/DeviceTypes.h"

namespace SmartHome {
namespace ShSensors {

constexpr int16_t NET_SEN_TEMP_UNGUELTIG = INT16_MIN;
constexpr uint16_t NET_SEN_HUM_UNGUELTIG = 0xFFFFU;
constexpr uint16_t NET_SEN_LUX_UNGUELTIG = 0xFFFFU;
constexpr uint32_t NET_SEN_PRESSURE_UNGUELTIG = 0xFFFFFFFFUL;
constexpr uint16_t NET_SEN_AQI_UNGUELTIG = 0xFFFFU;
constexpr uint16_t NET_SEN_TVOC_UNGUELTIG = 0xFFFFU;
constexpr uint16_t NET_SEN_ECO2_UNGUELTIG = 0xFFFFU;

struct NetSenState {
    bool motion;
    bool fault;
    PresenceState presence;
    int16_t temp_01c;
    uint16_t hum_01pct;
    uint16_t lux;
    uint32_t pressure_pa;
    uint16_t aqi;
    uint16_t tvoc_ppb;
    uint16_t eco2_ppm;
};

struct EnvironmentProviderConfig {
    int data_pin;
    unsigned long warmup_ms;
    unsigned long read_interval_ms;
    unsigned long diag_hint_interval_ms;
    int16_t temp_delta_01c;
    uint16_t hum_delta_01pct;
    uint32_t pressure_delta_pa;
};

struct AirQualityProviderConfig {
    unsigned long read_interval_ms;
    uint16_t aqi_delta;
    uint16_t tvoc_delta_ppb;
    uint16_t eco2_delta_ppm;
};

inline void resetNetSenState(NetSenState& state) {
    state.motion = false;
    state.fault = false;
    resetPresenceState(state.presence);
    state.temp_01c = NET_SEN_TEMP_UNGUELTIG;
    state.hum_01pct = NET_SEN_HUM_UNGUELTIG;
    state.lux = NET_SEN_LUX_UNGUELTIG;
    state.pressure_pa = NET_SEN_PRESSURE_UNGUELTIG;
    state.aqi = NET_SEN_AQI_UNGUELTIG;
    state.tvoc_ppb = NET_SEN_TVOC_UNGUELTIG;
    state.eco2_ppm = NET_SEN_ECO2_UNGUELTIG;
}

inline bool hasExtendedNetSenMetrics(const NetSenState& state) {
    return state.pressure_pa != NET_SEN_PRESSURE_UNGUELTIG ||
           state.aqi != NET_SEN_AQI_UNGUELTIG ||
           state.tvoc_ppb != NET_SEN_TVOC_UNGUELTIG ||
           state.eco2_ppm != NET_SEN_ECO2_UNGUELTIG;
}

inline bool updateTemperature(NetSenState& state, int16_t newValue, int16_t delta01c) {
    bool changed = false;
    if (state.temp_01c != newValue) {
        changed =
            state.temp_01c == NET_SEN_TEMP_UNGUELTIG ||
            newValue == NET_SEN_TEMP_UNGUELTIG ||
            abs((int)newValue - (int)state.temp_01c) >= delta01c;
    }
    state.temp_01c = newValue;
    return changed;
}

inline bool updateHumidity(NetSenState& state, uint16_t newValue, uint16_t delta01pct) {
    bool changed = false;
    if (state.hum_01pct != newValue) {
        changed =
            state.hum_01pct == NET_SEN_HUM_UNGUELTIG ||
            newValue == NET_SEN_HUM_UNGUELTIG ||
            abs((int)newValue - (int)state.hum_01pct) >= delta01pct;
    }
    state.hum_01pct = newValue;
    return changed;
}

inline bool updatePressure(NetSenState& state, uint32_t newValue, uint32_t deltaPa) {
    bool changed = false;
    if (state.pressure_pa != newValue) {
        changed =
            state.pressure_pa == NET_SEN_PRESSURE_UNGUELTIG ||
            newValue == NET_SEN_PRESSURE_UNGUELTIG ||
            labs((long)newValue - (long)state.pressure_pa) >= (long)deltaPa;
    }
    state.pressure_pa = newValue;
    return changed;
}

inline bool updateMetricU16(uint16_t& currentValue, uint16_t newValue, uint16_t invalidValue, uint16_t delta) {
    bool changed = false;
    if (currentValue != newValue) {
        changed =
            currentValue == invalidValue ||
            newValue == invalidValue ||
            abs((int)newValue - (int)currentValue) >= delta;
    }
    currentValue = newValue;
    return changed;
}

inline int16_t roundTempTo01c(float temperaturC) {
    return static_cast<int16_t>(lroundf(temperaturC * 10.0f));
}

inline uint16_t roundHumTo01pct(float feuchtePct) {
    long gerundet = lroundf(feuchtePct * 10.0f);
    if (gerundet < 0L) gerundet = 0L;
    if (gerundet > 1000L) gerundet = 1000L;
    return static_cast<uint16_t>(gerundet);
}

constexpr bool netSenEnvProvidesTemp(uint8_t providerKind) {
    return providerKind == SH_NET_SEN_ENV_PROVIDER_DHT22 ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_BMP280_STUB ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_BME280_STUB;
}

constexpr bool netSenEnvProvidesHum(uint8_t providerKind) {
    return providerKind == SH_NET_SEN_ENV_PROVIDER_DHT22 ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_BME280_STUB;
}

constexpr bool netSenEnvProvidesPressure(uint8_t providerKind) {
    return providerKind == SH_NET_SEN_ENV_PROVIDER_BMP280_STUB ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_BME280_STUB;
}

constexpr bool netSenAirProvidesMetrics(uint8_t providerKind) {
    return providerKind == SH_NET_SEN_AIR_PROVIDER_ENS160_STUB;
}

constexpr bool netSenMotionProvidesSignal(uint8_t providerKind) {
    return presenceProviderProvidesSignal(providerKind);
}

inline bool syncNetSenMotionFromPresence(NetSenState& state) {
    const bool nextMotion = presenceIsActive(state.presence);
    const bool changed = (state.motion != nextMotion);
    state.motion = nextMotion;
    return changed;
}

template<uint8_t ENV_PROVIDER_KIND, uint8_t AIR_PROVIDER_KIND, uint8_t MOTION_PROVIDER_KIND>
constexpr uint16_t buildNetSenCaps() {
    return (netSenEnvProvidesTemp(ENV_PROVIDER_KIND) ? SH_CAP_TEMP : 0U) |
           (netSenEnvProvidesHum(ENV_PROVIDER_KIND) ? SH_CAP_HUM : 0U) |
           (netSenEnvProvidesPressure(ENV_PROVIDER_KIND) ? SH_CAP_PRESSURE : 0U) |
           (netSenAirProvidesMetrics(AIR_PROVIDER_KIND) ? SH_CAP_AQI : 0U) |
           (netSenMotionProvidesSignal(MOTION_PROVIDER_KIND) ? SH_CAP_MOTION : 0U);
}

template<uint8_t ENV_PROVIDER_KIND, uint8_t AIR_PROVIDER_KIND>
constexpr bool usesExtendedNetSenState() {
    return netSenEnvProvidesPressure(ENV_PROVIDER_KIND) ||
           netSenAirProvidesMetrics(AIR_PROVIDER_KIND);
}

template<uint8_t ProviderKind>
class EnvironmentProvider;

template<>
class EnvironmentProvider<SH_NET_SEN_ENV_PROVIDER_NONE> {
public:
    explicit EnvironmentProvider(const EnvironmentProviderConfig&) {}

    void begin(unsigned long, LogFn) {
        ready_ = true;
    }

    const char* name() const { return "none"; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(unsigned long, NetSenState&, LogFn) {
        return {false, false};
    }

private:
    bool ready_ = false;
};

template<>
class EnvironmentProvider<SH_NET_SEN_ENV_PROVIDER_DHT22> {
public:
    explicit EnvironmentProvider(const EnvironmentProviderConfig& config)
        : dht_(config.data_pin, DHT22), config_(config) {}

    void begin(unsigned long bootMs, LogFn) {
        boot_ms_ = bootMs;
        last_read_ms_ = 0UL;
        last_good_ms_ = 0UL;
        last_diag_ms_ = 0UL;
        read_attempts_ = 0UL;
        consecutive_errors_ = 0UL;
        ready_ = false;
        fault_ = false;
        dht_.begin();
    }

    const char* name() const { return "dht22"; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(unsigned long nowMs, NetSenState& state, LogFn log) {
        if ((nowMs - boot_ms_) < config_.warmup_ms) {
            return {false, fault_};
        }

        if (last_read_ms_ != 0UL && (nowMs - last_read_ms_) < config_.read_interval_ms) {
            return {false, fault_};
        }

        last_read_ms_ = nowMs;
        read_attempts_++;
        if (log) {
            log("INFO", "DHT22 READ GPIO%d versuch=%lu", config_.data_pin, read_attempts_);
        }

        if (!dht_.read()) {
            return setReadError(state, nowMs, log, "Lib-Read fehlgeschlagen (Timeout/Checksum)");
        }

        const float feuchtePct = dht_.readHumidity();
        const float temperaturC = dht_.readTemperature();
        const char* fehlergrund = validateMeasurement(temperaturC, feuchtePct);
        if (fehlergrund != nullptr) {
            return setReadError(state, nowMs, log, fehlergrund);
        }

        ready_ = true;
        fault_ = false;
        consecutive_errors_ = 0UL;
        last_good_ms_ = nowMs;

        bool changed = false;
        changed = updateTemperature(state, roundTempTo01c(temperaturC), config_.temp_delta_01c) || changed;
        changed = updateHumidity(state, roundHumTo01pct(feuchtePct), config_.hum_delta_01pct) || changed;
        changed = updatePressure(state, NET_SEN_PRESSURE_UNGUELTIG, config_.pressure_delta_pa) || changed;

        if (log) {
            log(
                "INFO",
                "DHT22 OK temp=%d.%dC hum=%u.%u%%",
                state.temp_01c / 10,
                abs((int)(state.temp_01c % 10)),
                state.hum_01pct / 10,
                state.hum_01pct % 10);
        }

        return {changed, false};
    }

private:
    const char* validateMeasurement(float temperaturC, float feuchtePct) const {
        if (isnan(feuchtePct) || isnan(temperaturC)) {
            return "NaN";
        }

        if (feuchtePct < 0.0f || feuchtePct > 100.0f || temperaturC < -40.0f || temperaturC > 80.0f) {
            return "ausserhalb DHT22-Bereich";
        }

        const int16_t gerundeteTemp = roundTempTo01c(temperaturC);
        const uint16_t gerundeteHum = roundHumTo01pct(feuchtePct);
        if (gerundeteTemp == 0 && gerundeteHum == 0U) {
            return "0.0C/0.0% unplausibel (typisches Nullframe-/Verdrahtungsbild)";
        }

        return nullptr;
    }

    void logWiringHint(unsigned long nowMs, LogFn log) {
        if (!log) return;

        const bool hintDue =
            consecutive_errors_ == 1UL ||
            consecutive_errors_ == 3UL ||
            (nowMs - last_diag_ms_) >= config_.diag_hint_interval_ms;

        if (!hintDue) {
            return;
        }

        last_diag_ms_ = nowMs;
        if (last_good_ms_ == 0UL) {
            log(
                "WARN",
                "DHT22 HINWEIS: seit Boot keine gueltige Messung; GPIO%d/VCC/GND/Pull-up pruefen",
                config_.data_pin);
            return;
        }

        log(
            "WARN",
            "DHT22 HINWEIS: letzte gueltige Messung vor %lus; Verdrahtung/Sensor pruefen",
            (nowMs - last_good_ms_) / 1000UL);
    }

    ProviderUpdate setReadError(NetSenState& state, unsigned long nowMs, LogFn log, const char* reason) {
        const bool hadFault = fault_;
        ready_ = true;
        fault_ = true;
        consecutive_errors_++;

        bool changed = false;
        changed = updateTemperature(state, NET_SEN_TEMP_UNGUELTIG, config_.temp_delta_01c) || changed;
        changed = updateHumidity(state, NET_SEN_HUM_UNGUELTIG, config_.hum_delta_01pct) || changed;
        changed = updatePressure(state, NET_SEN_PRESSURE_UNGUELTIG, config_.pressure_delta_pa) || changed;
        changed = (!hadFault) || changed;

        if (log) {
            log(
                "WARN",
                "DHT22 FEHLER GPIO%d: %s (versuch=%lu, fehler_in_folge=%lu)",
                config_.data_pin,
                reason,
                read_attempts_,
                consecutive_errors_);
        }
        logWiringHint(nowMs, log);
        return {changed, true};
    }

    DHT dht_;
    EnvironmentProviderConfig config_;
    unsigned long boot_ms_ = 0UL;
    unsigned long last_read_ms_ = 0UL;
    unsigned long last_good_ms_ = 0UL;
    unsigned long last_diag_ms_ = 0UL;
    unsigned long read_attempts_ = 0UL;
    unsigned long consecutive_errors_ = 0UL;
    bool ready_ = false;
    bool fault_ = false;
};

template<>
class EnvironmentProvider<SH_NET_SEN_ENV_PROVIDER_BMP280_STUB> {
public:
    explicit EnvironmentProvider(const EnvironmentProviderConfig& config) : config_(config) {}

    void begin(unsigned long, LogFn log) {
        ready_ = true;
        if (log) {
            log("WARN", "NET-SEN stub provider aktiv: bmp280_stub (kein Hardware-Nachweis)");
        }
    }

    const char* name() const { return "bmp280_stub"; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(unsigned long, NetSenState& state, LogFn) {
        bool changed = false;
        changed = updateTemperature(state, 214, config_.temp_delta_01c) || changed;
        changed = updateHumidity(state, NET_SEN_HUM_UNGUELTIG, config_.hum_delta_01pct) || changed;
        changed = updatePressure(state, 100820UL, config_.pressure_delta_pa) || changed;
        return {changed, false};
    }

private:
    EnvironmentProviderConfig config_;
    bool ready_ = false;
};

template<>
class EnvironmentProvider<SH_NET_SEN_ENV_PROVIDER_BME280_STUB> {
public:
    explicit EnvironmentProvider(const EnvironmentProviderConfig& config) : config_(config) {}

    void begin(unsigned long, LogFn log) {
        ready_ = true;
        if (log) {
            log("WARN", "NET-SEN stub provider aktiv: bme280_stub (kein Hardware-Nachweis)");
        }
    }

    const char* name() const { return "bme280_stub"; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(unsigned long, NetSenState& state, LogFn) {
        bool changed = false;
        changed = updateTemperature(state, 214, config_.temp_delta_01c) || changed;
        changed = updateHumidity(state, 503, config_.hum_delta_01pct) || changed;
        changed = updatePressure(state, 100820UL, config_.pressure_delta_pa) || changed;
        return {changed, false};
    }

private:
    EnvironmentProviderConfig config_;
    bool ready_ = false;
};

template<uint8_t ProviderKind>
class AirQualityProvider;

template<>
class AirQualityProvider<SH_NET_SEN_AIR_PROVIDER_NONE> {
public:
    explicit AirQualityProvider(const AirQualityProviderConfig&) {}

    void begin(unsigned long, LogFn) {
        ready_ = true;
    }

    const char* name() const { return "none"; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(unsigned long, NetSenState&, LogFn) {
        return {false, false};
    }

private:
    bool ready_ = false;
};

template<>
class AirQualityProvider<SH_NET_SEN_AIR_PROVIDER_ENS160_STUB> {
public:
    explicit AirQualityProvider(const AirQualityProviderConfig& config) : config_(config) {}

    void begin(unsigned long, LogFn log) {
        ready_ = true;
        if (log) {
            log("WARN", "NET-SEN stub provider aktiv: ens160_stub (kein Hardware-Nachweis)");
        }
    }

    const char* name() const { return "ens160_stub"; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(unsigned long, NetSenState& state, LogFn) {
        bool changed = false;
        changed = updateMetricU16(state.aqi, 1U, NET_SEN_AQI_UNGUELTIG, config_.aqi_delta) || changed;
        changed = updateMetricU16(state.tvoc_ppb, 65U, NET_SEN_TVOC_UNGUELTIG, config_.tvoc_delta_ppb) || changed;
        changed = updateMetricU16(state.eco2_ppm, 450U, NET_SEN_ECO2_UNGUELTIG, config_.eco2_delta_ppm) || changed;
        return {changed, false};
    }

private:
    AirQualityProviderConfig config_;
    bool ready_ = false;
};

}  // namespace ShSensors
}  // namespace SmartHome
