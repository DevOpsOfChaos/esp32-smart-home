#pragma once

#include <Arduino.h>
#include <DHT.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

#if __has_include(<Adafruit_BMP280.h>)
  #include <Adafruit_BMP280.h>
  #define SHSENSORS_HAS_ADAFRUIT_BMP280 1
#else
  #define SHSENSORS_HAS_ADAFRUIT_BMP280 0
#endif

#if __has_include(<Adafruit_BME280.h>)
  #include <Adafruit_BME280.h>
  #define SHSENSORS_HAS_ADAFRUIT_BME280 1
#else
  #define SHSENSORS_HAS_ADAFRUIT_BME280 0
#endif

#if __has_include(<Adafruit_SHT4x.h>)
  #include <Adafruit_SHT4x.h>
  #define SHSENSORS_HAS_ADAFRUIT_SHT4X 1
#else
  #define SHSENSORS_HAS_ADAFRUIT_SHT4X 0
#endif

#if __has_include(<Adafruit_VEML7700.h>)
  #include <Adafruit_VEML7700.h>
  #define SHSENSORS_HAS_ADAFRUIT_VEML7700 1
#else
  #define SHSENSORS_HAS_ADAFRUIT_VEML7700 0
#endif

#if __has_include(<ScioSense_ENS160.h>)
  #include <ScioSense_ENS160.h>
  #define SHSENSORS_HAS_SCIOSENSE_ENS160 1
#elif __has_include("ScioSense_ENS160.h")
  #include "ScioSense_ENS160.h"
  #define SHSENSORS_HAS_SCIOSENSE_ENS160 1
#else
  #define SHSENSORS_HAS_SCIOSENSE_ENS160 0
#endif

#if SHSENSORS_HAS_SCIOSENSE_ENS160
  #ifndef ENS160_I2CADDR_0
    #define ENS160_I2CADDR_0 0x52
  #endif
  #ifndef ENS160_I2CADDR_1
    #define ENS160_I2CADDR_1 0x53
  #endif
#endif

#include "I2cSensorSupport.h"
#include "PresenceProviders.h"
#include "ProviderCommon.h"
#include "ProviderIds.h"
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
    I2cBusConfig i2c_bus;
    unsigned long warmup_ms;
    unsigned long read_interval_ms;
    unsigned long diag_hint_interval_ms;
    int16_t temp_delta_01c;
    uint16_t hum_delta_01pct;
    uint32_t pressure_delta_pa;
};

struct LightProviderConfig {
    I2cBusConfig i2c_bus;
    unsigned long read_interval_ms;
    unsigned long diag_hint_interval_ms;
    uint16_t lux_delta;
};

struct AirQualityProviderConfig {
    I2cBusConfig i2c_bus;
    unsigned long warmup_ms;
    unsigned long read_interval_ms;
    unsigned long diag_hint_interval_ms;
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

inline bool updateLux(NetSenState& state, uint16_t newValue, uint16_t deltaLux) {
    bool changed = false;
    if (state.lux != newValue) {
        changed =
            state.lux == NET_SEN_LUX_UNGUELTIG ||
            newValue == NET_SEN_LUX_UNGUELTIG ||
            abs((int)newValue - (int)state.lux) >= deltaLux;
    }
    state.lux = newValue;
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

inline uint16_t clampLuxToU16(float lux) {
    long gerundet = lroundf(lux);
    if (gerundet < 0L) gerundet = 0L;
    if (gerundet > 65535L) gerundet = 65535L;
    return static_cast<uint16_t>(gerundet);
}

inline uint32_t roundPressureToPa(float pressurePa) {
    long gerundet = lroundf(pressurePa);
    if (gerundet < 0L) gerundet = 0L;
    return static_cast<uint32_t>(gerundet);
}

inline uint16_t mapEns160AqiTo0_500(uint16_t raw) {
    if (raw <= 5U) return (uint16_t)(raw * 100U);
    if (raw <= 500U) return raw;
    return 0U;
}

inline const char* validateTempHum(float temperaturC, float feuchtePct, float maxTempC) {
    if (isnan(temperaturC) || isnan(feuchtePct)) return "NaN";
    if (temperaturC < -40.0f || temperaturC > maxTempC) return "Temperatur ausserhalb Bereich";
    if (feuchtePct < 0.0f || feuchtePct > 100.0f) return "Feuchte ausserhalb Bereich";
    return nullptr;
}

inline const char* validateTempPressure(float temperaturC, float pressurePa, float maxTempC) {
    if (isnan(temperaturC) || isnan(pressurePa)) return "NaN";
    if (temperaturC < -40.0f || temperaturC > maxTempC) return "Temperatur ausserhalb Bereich";
    if (pressurePa < 30000.0f || pressurePa > 110000.0f) return "Druck ausserhalb Bereich";
    return nullptr;
}

inline const char* validateTempHumPressure(float temperaturC, float feuchtePct, float pressurePa, float maxTempC) {
    const char* tempHumReason = validateTempHum(temperaturC, feuchtePct, maxTempC);
    if (tempHumReason != nullptr) return tempHumReason;
    if (isnan(pressurePa) || pressurePa < 30000.0f || pressurePa > 110000.0f) return "Druck ausserhalb Bereich";
    return nullptr;
}

inline bool shouldLogProviderHint(
    unsigned long nowMs,
    unsigned long lastDiagMs,
    unsigned long consecutiveErrors,
    unsigned long diagIntervalMs)
{
    return consecutiveErrors == 1UL ||
           consecutiveErrors == 3UL ||
           (diagIntervalMs > 0UL && (nowMs - lastDiagMs) >= diagIntervalMs);
}

constexpr bool netSenEnvProvidesTemp(uint8_t providerKind) {
    return providerKind == SH_NET_SEN_ENV_PROVIDER_DHT22 ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_BMP280 ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_BME280 ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_SHT41 ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_BMP280_STUB ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_BME280_STUB ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_SHT41_STUB;
}

constexpr bool netSenEnvProvidesHum(uint8_t providerKind) {
    return providerKind == SH_NET_SEN_ENV_PROVIDER_DHT22 ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_BME280 ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_SHT41 ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_BME280_STUB ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_SHT41_STUB;
}

constexpr bool netSenEnvProvidesPressure(uint8_t providerKind) {
    return providerKind == SH_NET_SEN_ENV_PROVIDER_BMP280 ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_BME280 ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_BMP280_STUB ||
           providerKind == SH_NET_SEN_ENV_PROVIDER_BME280_STUB;
}

constexpr bool netSenLightProvidesLux(uint8_t providerKind) {
    return providerKind == SH_NET_SEN_LIGHT_PROVIDER_VEML7700 ||
           providerKind == SH_NET_SEN_LIGHT_PROVIDER_VEML7700_STUB;
}

constexpr bool netSenAirProvidesMetrics(uint8_t providerKind) {
    return providerKind == SH_NET_SEN_AIR_PROVIDER_ENS160 ||
           providerKind == SH_NET_SEN_AIR_PROVIDER_ENS160_STUB;
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

template<uint8_t ENV_PROVIDER_KIND, uint8_t AIR_PROVIDER_KIND, uint8_t LIGHT_PROVIDER_KIND, uint8_t MOTION_PROVIDER_KIND>
constexpr uint16_t buildNetSenCaps() {
    return (netSenEnvProvidesTemp(ENV_PROVIDER_KIND) ? SH_CAP_TEMP : 0U) |
           (netSenEnvProvidesHum(ENV_PROVIDER_KIND) ? SH_CAP_HUM : 0U) |
           (netSenLightProvidesLux(LIGHT_PROVIDER_KIND) ? SH_CAP_LUX : 0U) |
           (netSenAirProvidesMetrics(AIR_PROVIDER_KIND) ? SH_CAP_AQI : 0U) |
           (netSenMotionProvidesSignal(MOTION_PROVIDER_KIND) ? SH_CAP_MOTION : 0U) |
           (netSenEnvProvidesPressure(ENV_PROVIDER_KIND) ? SH_CAP_PRESSURE : 0U);
}

template<uint8_t ENV_PROVIDER_KIND, uint8_t AIR_PROVIDER_KIND>
constexpr bool usesExtendedNetSenState() {
    return netSenEnvProvidesPressure(ENV_PROVIDER_KIND) ||
           netSenAirProvidesMetrics(AIR_PROVIDER_KIND);
}

template<uint8_t ProviderKind>
class EnvironmentProvider;

template<uint8_t ProviderKind>
class LightProvider;

template<uint8_t ProviderKind>
class AirQualityProvider;

template<>
class EnvironmentProvider<SH_NET_SEN_ENV_PROVIDER_NONE> {
public:
    explicit EnvironmentProvider(const EnvironmentProviderConfig&) {}

    void begin(unsigned long, LogFn) { ready_ = true; }
    const char* name() const { return "none"; }
    bool isReady() const { return ready_; }
    ProviderUpdate poll(unsigned long, NetSenState&, LogFn) { return {false, false}; }

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
        if ((nowMs - boot_ms_) < config_.warmup_ms) return {false, fault_};
        if (last_read_ms_ != 0UL && (nowMs - last_read_ms_) < config_.read_interval_ms) return {false, fault_};

        last_read_ms_ = nowMs;
        read_attempts_++;
        if (log) log("INFO", "DHT22 READ GPIO%d versuch=%lu", config_.data_pin, read_attempts_);

        if (!dht_.read()) return setReadError(state, nowMs, log, "Lib-Read fehlgeschlagen (Timeout/Checksum)");

        const float feuchtePct = dht_.readHumidity();
        const float temperaturC = dht_.readTemperature();
        const char* fehlergrund = validateMeasurement(temperaturC, feuchtePct);
        if (fehlergrund != nullptr) return setReadError(state, nowMs, log, fehlergrund);

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
        if (isnan(feuchtePct) || isnan(temperaturC)) return "NaN";
        if (feuchtePct < 0.0f || feuchtePct > 100.0f || temperaturC < -40.0f || temperaturC > 80.0f) return "ausserhalb DHT22-Bereich";

        const int16_t gerundeteTemp = roundTempTo01c(temperaturC);
        const uint16_t gerundeteHum = roundHumTo01pct(feuchtePct);
        if (gerundeteTemp == 0 && gerundeteHum == 0U) return "0.0C/0.0% unplausibel (typisches Nullframe-/Verdrahtungsbild)";
        return nullptr;
    }

    void logWiringHint(unsigned long nowMs, LogFn log) {
        if (!log) return;
        if (!shouldLogProviderHint(nowMs, last_diag_ms_, consecutive_errors_, config_.diag_hint_interval_ms)) return;

        last_diag_ms_ = nowMs;
        if (last_good_ms_ == 0UL) {
            log("WARN", "DHT22 HINWEIS: seit Boot keine gueltige Messung; GPIO%d/VCC/GND/Pull-up pruefen", config_.data_pin);
            return;
        }

        log("WARN", "DHT22 HINWEIS: letzte gueltige Messung vor %lus; Verdrahtung/Sensor pruefen", (nowMs - last_good_ms_) / 1000UL);
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

// REAL_ENV_PROVIDERS

#if SHSENSORS_HAS_ADAFRUIT_BMP280
template<>
class EnvironmentProvider<SH_NET_SEN_ENV_PROVIDER_BMP280> {
public:
    explicit EnvironmentProvider(const EnvironmentProviderConfig& config) : config_(config) {}

    void begin(unsigned long bootMs, LogFn log) {
        boot_ms_ = bootMs;
        last_read_ms_ = 0UL;
        last_good_ms_ = 0UL;
        last_diag_ms_ = 0UL;
        consecutive_errors_ = 0UL;
        ready_ = true;
        fault_ = !beginSensor(log);
    }

    const char* name() const { return "bmp280"; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(unsigned long nowMs, NetSenState& state, LogFn log) {
        if ((nowMs - boot_ms_) < config_.warmup_ms) return {false, fault_};
        if (last_read_ms_ != 0UL && (nowMs - last_read_ms_) < config_.read_interval_ms) return {false, fault_};

        last_read_ms_ = nowMs;
        if (!sensor_initialized_ && !beginSensor(log)) {
            return setReadError(state, nowMs, log, "kein Sensor auf 0x76/0x77");
        }

        const float temperaturC = sensor_.readTemperature();
        const float pressurePa = sensor_.readPressure();
        const char* reason = validateTempPressure(temperaturC, pressurePa, 85.0f);
        if (reason != nullptr) return setReadError(state, nowMs, log, reason);

        fault_ = false;
        consecutive_errors_ = 0UL;
        last_good_ms_ = nowMs;

        bool changed = false;
        changed = updateTemperature(state, roundTempTo01c(temperaturC), config_.temp_delta_01c) || changed;
        changed = updateHumidity(state, NET_SEN_HUM_UNGUELTIG, config_.hum_delta_01pct) || changed;
        changed = updatePressure(state, roundPressureToPa(pressurePa), config_.pressure_delta_pa) || changed;

        if (log) {
            log(
                "INFO",
                "BMP280 OK addr=0x%02X temp=%d.%dC pressure=%luPa",
                address_,
                state.temp_01c / 10,
                abs((int)(state.temp_01c % 10)),
                (unsigned long)state.pressure_pa);
        }
        return {changed, false};
    }

private:
    bool beginSensor(LogFn log) {
        if (!sharedI2cBus().begin(config_.i2c_bus, log)) return false;

        const uint8_t addresses[] = {0x76U, 0x77U};
        for (uint8_t address : addresses) {
            if (sensor_.begin(address)) {
                address_ = address;
                sensor_initialized_ = true;
                if (log) log("INFO", "BMP280 init OK auf 0x%02X", address_);
                return true;
            }
        }

        sensor_initialized_ = false;
        return false;
    }

    void logHint(unsigned long nowMs, LogFn log) {
        if (!log) return;
        if (!shouldLogProviderHint(nowMs, last_diag_ms_, consecutive_errors_, config_.diag_hint_interval_ms)) return;

        last_diag_ms_ = nowMs;
        if (last_good_ms_ == 0UL) {
            log("WARN", "BMP280 HINWEIS: noch keine gueltige Messung; Bus, Adresse 0x76/0x77 und Versorgung pruefen");
            return;
        }

        log("WARN", "BMP280 HINWEIS: letzte gueltige Messung vor %lus; Bus/Verdrahtung pruefen", (nowMs - last_good_ms_) / 1000UL);
    }

    ProviderUpdate setReadError(NetSenState& state, unsigned long nowMs, LogFn log, const char* reason) {
        const bool hadFault = fault_;
        fault_ = true;
        consecutive_errors_++;
        if (consecutive_errors_ >= 3UL) {
            sharedI2cBus().recover(nowMs, log, "bmp280");
        }

        bool changed = false;
        changed = updateTemperature(state, NET_SEN_TEMP_UNGUELTIG, config_.temp_delta_01c) || changed;
        changed = updateHumidity(state, NET_SEN_HUM_UNGUELTIG, config_.hum_delta_01pct) || changed;
        changed = updatePressure(state, NET_SEN_PRESSURE_UNGUELTIG, config_.pressure_delta_pa) || changed;
        changed = (!hadFault) || changed;

        if (log) {
            log("WARN", "BMP280 FEHLER addr=0x%02X: %s (fehler_in_folge=%lu)", address_, reason, consecutive_errors_);
        }
        logHint(nowMs, log);
        return {changed, true};
    }

    Adafruit_BMP280 sensor_;
    EnvironmentProviderConfig config_;
    unsigned long boot_ms_ = 0UL;
    unsigned long last_read_ms_ = 0UL;
    unsigned long last_good_ms_ = 0UL;
    unsigned long last_diag_ms_ = 0UL;
    unsigned long consecutive_errors_ = 0UL;
    uint8_t address_ = 0U;
    bool ready_ = false;
    bool fault_ = false;
    bool sensor_initialized_ = false;
};
#endif

#if SHSENSORS_HAS_ADAFRUIT_BME280
template<>
class EnvironmentProvider<SH_NET_SEN_ENV_PROVIDER_BME280> {
public:
    explicit EnvironmentProvider(const EnvironmentProviderConfig& config) : config_(config) {}

    void begin(unsigned long bootMs, LogFn log) {
        boot_ms_ = bootMs;
        last_read_ms_ = 0UL;
        last_good_ms_ = 0UL;
        last_diag_ms_ = 0UL;
        consecutive_errors_ = 0UL;
        ready_ = true;
        fault_ = !beginSensor(log);
    }

    const char* name() const { return "bme280"; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(unsigned long nowMs, NetSenState& state, LogFn log) {
        if ((nowMs - boot_ms_) < config_.warmup_ms) return {false, fault_};
        if (last_read_ms_ != 0UL && (nowMs - last_read_ms_) < config_.read_interval_ms) return {false, fault_};

        last_read_ms_ = nowMs;
        if (!sensor_initialized_ && !beginSensor(log)) {
            return setReadError(state, nowMs, log, "kein Sensor auf 0x76/0x77");
        }

        const float temperaturC = sensor_.readTemperature();
        const float feuchtePct = sensor_.readHumidity();
        const float pressurePa = sensor_.readPressure();
        const char* reason = validateTempHumPressure(temperaturC, feuchtePct, pressurePa, 85.0f);
        if (reason != nullptr) return setReadError(state, nowMs, log, reason);

        fault_ = false;
        consecutive_errors_ = 0UL;
        last_good_ms_ = nowMs;

        bool changed = false;
        changed = updateTemperature(state, roundTempTo01c(temperaturC), config_.temp_delta_01c) || changed;
        changed = updateHumidity(state, roundHumTo01pct(feuchtePct), config_.hum_delta_01pct) || changed;
        changed = updatePressure(state, roundPressureToPa(pressurePa), config_.pressure_delta_pa) || changed;

        if (log) {
            log(
                "INFO",
                "BME280 OK addr=0x%02X temp=%d.%dC hum=%u.%u%% pressure=%luPa",
                address_,
                state.temp_01c / 10,
                abs((int)(state.temp_01c % 10)),
                state.hum_01pct / 10,
                state.hum_01pct % 10,
                (unsigned long)state.pressure_pa);
        }
        return {changed, false};
    }

private:
    bool beginSensor(LogFn log) {
        if (!sharedI2cBus().begin(config_.i2c_bus, log)) return false;

        const uint8_t addresses[] = {0x76U, 0x77U};
        for (uint8_t address : addresses) {
            if (sensor_.begin(address)) {
                address_ = address;
                sensor_initialized_ = true;
                if (log) log("INFO", "BME280 init OK auf 0x%02X", address_);
                return true;
            }
        }

        sensor_initialized_ = false;
        return false;
    }

    void logHint(unsigned long nowMs, LogFn log) {
        if (!log) return;
        if (!shouldLogProviderHint(nowMs, last_diag_ms_, consecutive_errors_, config_.diag_hint_interval_ms)) return;

        last_diag_ms_ = nowMs;
        if (last_good_ms_ == 0UL) {
            log("WARN", "BME280 HINWEIS: noch keine gueltige Messung; Bus, Adresse 0x76/0x77 und Versorgung pruefen");
            return;
        }

        log("WARN", "BME280 HINWEIS: letzte gueltige Messung vor %lus; Bus/Verdrahtung pruefen", (nowMs - last_good_ms_) / 1000UL);
    }

    ProviderUpdate setReadError(NetSenState& state, unsigned long nowMs, LogFn log, const char* reason) {
        const bool hadFault = fault_;
        fault_ = true;
        consecutive_errors_++;
        if (consecutive_errors_ >= 3UL) {
            sharedI2cBus().recover(nowMs, log, "bme280");
        }

        bool changed = false;
        changed = updateTemperature(state, NET_SEN_TEMP_UNGUELTIG, config_.temp_delta_01c) || changed;
        changed = updateHumidity(state, NET_SEN_HUM_UNGUELTIG, config_.hum_delta_01pct) || changed;
        changed = updatePressure(state, NET_SEN_PRESSURE_UNGUELTIG, config_.pressure_delta_pa) || changed;
        changed = (!hadFault) || changed;

        if (log) {
            log("WARN", "BME280 FEHLER addr=0x%02X: %s (fehler_in_folge=%lu)", address_, reason, consecutive_errors_);
        }
        logHint(nowMs, log);
        return {changed, true};
    }

    Adafruit_BME280 sensor_;
    EnvironmentProviderConfig config_;
    unsigned long boot_ms_ = 0UL;
    unsigned long last_read_ms_ = 0UL;
    unsigned long last_good_ms_ = 0UL;
    unsigned long last_diag_ms_ = 0UL;
    unsigned long consecutive_errors_ = 0UL;
    uint8_t address_ = 0U;
    bool ready_ = false;
    bool fault_ = false;
    bool sensor_initialized_ = false;
};
#endif

#if SHSENSORS_HAS_ADAFRUIT_SHT4X
template<>
class EnvironmentProvider<SH_NET_SEN_ENV_PROVIDER_SHT41> {
public:
    explicit EnvironmentProvider(const EnvironmentProviderConfig& config) : config_(config) {}

    void begin(unsigned long bootMs, LogFn log) {
        boot_ms_ = bootMs;
        last_read_ms_ = 0UL;
        last_good_ms_ = 0UL;
        last_diag_ms_ = 0UL;
        consecutive_errors_ = 0UL;
        ready_ = true;
        fault_ = !beginSensor(log);
    }

    const char* name() const { return "sht41"; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(unsigned long nowMs, NetSenState& state, LogFn log) {
        if ((nowMs - boot_ms_) < config_.warmup_ms) return {false, fault_};
        if (last_read_ms_ != 0UL && (nowMs - last_read_ms_) < config_.read_interval_ms) return {false, fault_};

        last_read_ms_ = nowMs;
        if (!sensor_initialized_ && !beginSensor(log)) {
            return setReadError(state, nowMs, log, "kein Sensor auf 0x44");
        }

        sensors_event_t humidity;
        sensors_event_t temperature;
        sensor_.getEvent(&humidity, &temperature);
        const float temperaturC = temperature.temperature;
        const float feuchtePct = humidity.relative_humidity;
        const char* reason = validateTempHum(temperaturC, feuchtePct, 125.0f);
        if (reason != nullptr) return setReadError(state, nowMs, log, reason);

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
                "SHT41 OK temp=%d.%dC hum=%u.%u%%",
                state.temp_01c / 10,
                abs((int)(state.temp_01c % 10)),
                state.hum_01pct / 10,
                state.hum_01pct % 10);
        }
        return {changed, false};
    }

private:
    bool beginSensor(LogFn log) {
        if (!sharedI2cBus().begin(config_.i2c_bus, log)) return false;
        if (!sensor_.begin()) {
            sensor_initialized_ = false;
            return false;
        }

        sensor_.setPrecision(SHT4X_HIGH_PRECISION);
        sensor_.setHeater(SHT4X_NO_HEATER);
        sensor_initialized_ = true;
        if (log) log("INFO", "SHT41 init OK auf 0x44");
        return true;
    }

    void logHint(unsigned long nowMs, LogFn log) {
        if (!log) return;
        if (!shouldLogProviderHint(nowMs, last_diag_ms_, consecutive_errors_, config_.diag_hint_interval_ms)) return;

        last_diag_ms_ = nowMs;
        if (last_good_ms_ == 0UL) {
            log("WARN", "SHT41 HINWEIS: noch keine gueltige Messung; Bus, Adresse 0x44 und Versorgung pruefen");
            return;
        }

        log("WARN", "SHT41 HINWEIS: letzte gueltige Messung vor %lus; Bus/Verdrahtung pruefen", (nowMs - last_good_ms_) / 1000UL);
    }

    ProviderUpdate setReadError(NetSenState& state, unsigned long nowMs, LogFn log, const char* reason) {
        const bool hadFault = fault_;
        fault_ = true;
        consecutive_errors_++;
        if (consecutive_errors_ >= 3UL) {
            sharedI2cBus().recover(nowMs, log, "sht41");
        }

        bool changed = false;
        changed = updateTemperature(state, NET_SEN_TEMP_UNGUELTIG, config_.temp_delta_01c) || changed;
        changed = updateHumidity(state, NET_SEN_HUM_UNGUELTIG, config_.hum_delta_01pct) || changed;
        changed = updatePressure(state, NET_SEN_PRESSURE_UNGUELTIG, config_.pressure_delta_pa) || changed;
        changed = (!hadFault) || changed;

        if (log) {
            log("WARN", "SHT41 FEHLER: %s (fehler_in_folge=%lu)", reason, consecutive_errors_);
        }
        logHint(nowMs, log);
        return {changed, true};
    }

    Adafruit_SHT4x sensor_;
    EnvironmentProviderConfig config_;
    unsigned long boot_ms_ = 0UL;
    unsigned long last_read_ms_ = 0UL;
    unsigned long last_good_ms_ = 0UL;
    unsigned long last_diag_ms_ = 0UL;
    unsigned long consecutive_errors_ = 0UL;
    bool ready_ = false;
    bool fault_ = false;
    bool sensor_initialized_ = false;
};
#endif

template<>
class EnvironmentProvider<SH_NET_SEN_ENV_PROVIDER_BMP280_STUB> {
public:
    explicit EnvironmentProvider(const EnvironmentProviderConfig& config) : config_(config) {}
    void begin(unsigned long, LogFn log) {
        ready_ = true;
        if (log) log("WARN", "NET-SEN stub provider aktiv: bmp280_stub (kein Hardware-Nachweis)");
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
        if (log) log("WARN", "NET-SEN stub provider aktiv: bme280_stub (kein Hardware-Nachweis)");
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

template<>
class EnvironmentProvider<SH_NET_SEN_ENV_PROVIDER_SHT41_STUB> {
public:
    explicit EnvironmentProvider(const EnvironmentProviderConfig& config) : config_(config) {}
    void begin(unsigned long, LogFn log) {
        ready_ = true;
        if (log) log("WARN", "NET-SEN stub provider aktiv: sht41_stub (kein Hardware-Nachweis)");
    }
    const char* name() const { return "sht41_stub"; }
    bool isReady() const { return ready_; }
    ProviderUpdate poll(unsigned long, NetSenState& state, LogFn) {
        bool changed = false;
        changed = updateTemperature(state, 219, config_.temp_delta_01c) || changed;
        changed = updateHumidity(state, 486, config_.hum_delta_01pct) || changed;
        changed = updatePressure(state, NET_SEN_PRESSURE_UNGUELTIG, config_.pressure_delta_pa) || changed;
        return {changed, false};
    }

private:
    EnvironmentProviderConfig config_;
    bool ready_ = false;
};

template<>
class LightProvider<SH_NET_SEN_LIGHT_PROVIDER_NONE> {
public:
    explicit LightProvider(const LightProviderConfig&) {}
    void begin(unsigned long, LogFn) { ready_ = true; }
    const char* name() const { return "none"; }
    bool isReady() const { return ready_; }
    ProviderUpdate poll(unsigned long, NetSenState&, LogFn) { return {false, false}; }

private:
    bool ready_ = false;
};

// REAL_LIGHT_PROVIDERS

#if SHSENSORS_HAS_ADAFRUIT_VEML7700
template<>
class LightProvider<SH_NET_SEN_LIGHT_PROVIDER_VEML7700> {
public:
    explicit LightProvider(const LightProviderConfig& config) : config_(config) {}

    void begin(unsigned long, LogFn log) {
        last_read_ms_ = 0UL;
        last_good_ms_ = 0UL;
        last_diag_ms_ = 0UL;
        consecutive_errors_ = 0UL;
        ready_ = true;
        fault_ = !beginSensor(log);
    }

    const char* name() const { return "veml7700"; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(unsigned long nowMs, NetSenState& state, LogFn log) {
        if (last_read_ms_ != 0UL && (nowMs - last_read_ms_) < config_.read_interval_ms) return {false, fault_};

        last_read_ms_ = nowMs;
        if (!sensor_initialized_ && !beginSensor(log)) {
            return setReadError(state, nowMs, log, "kein Sensor auf 0x10");
        }

        uint16_t rawAls = 0U;
        if (!sharedI2cBus().readRegisterU16(0x10U, 0x04U, rawAls)) {
            return setReadError(state, nowMs, log, "ALS Register-Read fehlgeschlagen");
        }

        const float luxF = (float)rawAls * 0.4608f;
        if (isnan(luxF) || luxF < 0.0f) {
            return setReadError(state, nowMs, log, "Lux unplausibel");
        }

        fault_ = false;
        consecutive_errors_ = 0UL;
        last_good_ms_ = nowMs;

        const uint16_t lux = clampLuxToU16(luxF);
        const bool changed = updateLux(state, lux, config_.lux_delta);
        if (log) log("INFO", "VEML7700 OK lux=%u", lux);
        return {changed, false};
    }

private:
    bool beginSensor(LogFn log) {
        if (!sharedI2cBus().begin(config_.i2c_bus, log)) return false;
        if (!sensor_.begin()) {
            sensor_initialized_ = false;
            return false;
        }

        sensor_.setIntegrationTime(VEML7700_IT_100MS);
        sensor_.setGain(VEML7700_GAIN_1_8);
        sensor_initialized_ = true;
        if (log) log("INFO", "VEML7700 init OK auf 0x10");
        return true;
    }

    void logHint(unsigned long nowMs, LogFn log) {
        if (!log) return;
        if (!shouldLogProviderHint(nowMs, last_diag_ms_, consecutive_errors_, config_.diag_hint_interval_ms)) return;

        last_diag_ms_ = nowMs;
        if (last_good_ms_ == 0UL) {
            log("WARN", "VEML7700 HINWEIS: noch keine gueltige Messung; Bus, Adresse 0x10 und Versorgung pruefen");
            return;
        }

        log("WARN", "VEML7700 HINWEIS: letzte gueltige Messung vor %lus; Bus/Verdrahtung pruefen", (nowMs - last_good_ms_) / 1000UL);
    }

    ProviderUpdate setReadError(NetSenState& state, unsigned long nowMs, LogFn log, const char* reason) {
        const bool hadFault = fault_;
        fault_ = true;
        consecutive_errors_++;
        if (consecutive_errors_ >= 3UL) {
            sharedI2cBus().recover(nowMs, log, "veml7700");
        }

        const bool changed = updateLux(state, NET_SEN_LUX_UNGUELTIG, config_.lux_delta) || (!hadFault);
        if (log) {
            log("WARN", "VEML7700 FEHLER: %s (fehler_in_folge=%lu)", reason, consecutive_errors_);
        }
        logHint(nowMs, log);
        return {changed, true};
    }

    Adafruit_VEML7700 sensor_;
    LightProviderConfig config_;
    unsigned long last_read_ms_ = 0UL;
    unsigned long last_good_ms_ = 0UL;
    unsigned long last_diag_ms_ = 0UL;
    unsigned long consecutive_errors_ = 0UL;
    bool ready_ = false;
    bool fault_ = false;
    bool sensor_initialized_ = false;
};
#endif

template<>
class LightProvider<SH_NET_SEN_LIGHT_PROVIDER_VEML7700_STUB> {
public:
    explicit LightProvider(const LightProviderConfig& config) : config_(config) {}
    void begin(unsigned long, LogFn log) {
        ready_ = true;
        if (log) log("WARN", "NET-SEN stub provider aktiv: veml7700_stub (kein Hardware-Nachweis)");
    }
    const char* name() const { return "veml7700_stub"; }
    bool isReady() const { return ready_; }
    ProviderUpdate poll(unsigned long, NetSenState& state, LogFn) {
        return {updateLux(state, 320U, config_.lux_delta), false};
    }

private:
    LightProviderConfig config_;
    bool ready_ = false;
};

template<>
class AirQualityProvider<SH_NET_SEN_AIR_PROVIDER_NONE> {
public:
    explicit AirQualityProvider(const AirQualityProviderConfig&) {}
    void begin(unsigned long, LogFn) { ready_ = true; }
    const char* name() const { return "none"; }
    bool isReady() const { return ready_; }
    ProviderUpdate poll(unsigned long, NetSenState&, LogFn) { return {false, false}; }

private:
    bool ready_ = false;
};

// REAL_AIR_PROVIDERS

#if SHSENSORS_HAS_SCIOSENSE_ENS160
template<>
class AirQualityProvider<SH_NET_SEN_AIR_PROVIDER_ENS160> {
public:
    explicit AirQualityProvider(const AirQualityProviderConfig& config)
        : sensor_0_(ENS160_I2CADDR_0), sensor_1_(ENS160_I2CADDR_1), config_(config) {}

    void begin(unsigned long bootMs, LogFn log) {
        boot_ms_ = bootMs;
        last_cycle_ms_ = 0UL;
        last_good_ms_ = 0UL;
        last_diag_ms_ = 0UL;
        wait_started_ms_ = 0UL;
        consecutive_errors_ = 0UL;
        phase_ = 0U;
        ready_ = true;
        fault_ = !beginSensor(log);
    }

    const char* name() const { return "ens160"; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(unsigned long nowMs, NetSenState& state, LogFn log) {
        if (!sensor_initialized_) {
            if (last_cycle_ms_ != 0UL && (nowMs - last_cycle_ms_) < config_.read_interval_ms) {
                return {false, fault_};
            }

            last_cycle_ms_ = nowMs;
            if (!beginSensor(log)) {
                if ((nowMs - boot_ms_) < config_.warmup_ms) {
                    return {false, false};
                }
                return setReadError(state, nowMs, log, "kein Sensor auf 0x52/0x53");
            }

            fault_ = false;
            consecutive_errors_ = 0UL;
            return {true, false};
        }

        if (phase_ == 0U) {
            if (last_cycle_ms_ != 0UL && (nowMs - last_cycle_ms_) < config_.read_interval_ms) {
                return {false, fault_};
            }

            if (state.temp_01c != NET_SEN_TEMP_UNGUELTIG &&
                state.hum_01pct != NET_SEN_HUM_UNGUELTIG) {
                const float temperaturC = (float)state.temp_01c / 10.0f;
                const float feuchtePct = (float)state.hum_01pct / 10.0f;
                (void)sensor_->set_envdata(temperaturC, feuchtePct);
            }

            (void)sensor_->measure(false);
            last_cycle_ms_ = nowMs;
            wait_started_ms_ = nowMs;
            phase_ = 1U;
            return {false, fault_};
        }

        const bool newData = sensor_->measure(false);
        if (newData) {
            phase_ = 0U;

            const uint16_t rawAqi500 = sensor_->getAQI500();
            const uint16_t rawAqi = sensor_->getAQI();
            const uint16_t mapped =
                (rawAqi500 > 0U && rawAqi500 <= 500U) ? rawAqi500 : mapEns160AqiTo0_500(rawAqi);

            if (mapped == 0U) {
                if ((nowMs - boot_ms_) < config_.warmup_ms) {
                    return {false, false};
                }
                return setReadError(state, nowMs, log, "AQI noch ungueltig/warmup");
            }

            fault_ = false;
            consecutive_errors_ = 0UL;
            last_good_ms_ = nowMs;

            bool changed = false;
            changed = updateMetricU16(state.aqi, mapped, NET_SEN_AQI_UNGUELTIG, config_.aqi_delta) || changed;
            changed = updateMetricU16(state.tvoc_ppb, sensor_->getTVOC(), NET_SEN_TVOC_UNGUELTIG, config_.tvoc_delta_ppb) || changed;
            changed = updateMetricU16(state.eco2_ppm, sensor_->geteCO2(), NET_SEN_ECO2_UNGUELTIG, config_.eco2_delta_ppm) || changed;

            if (log) {
                log(
                    "INFO",
                    "ENS160 OK addr=0x%02X aqi=%u tvoc=%u eco2=%u",
                    address_,
                    state.aqi,
                    state.tvoc_ppb,
                    state.eco2_ppm);
            }
            return {changed, false};
        }

        if ((nowMs - wait_started_ms_) <= 5000UL) {
            return {false, fault_};
        }

        phase_ = 0U;
        if ((nowMs - boot_ms_) < config_.warmup_ms) {
            return {false, false};
        }
        return setReadError(state, nowMs, log, "Messung lieferte kein Ergebnis");
    }

private:
    bool beginSensor(LogFn log) {
        if (!sharedI2cBus().begin(config_.i2c_bus, log)) return false;

        sensor_ = &sensor_1_;
        if (!sensor_->begin()) {
            sensor_ = &sensor_0_;
            if (!sensor_->begin()) {
                sensor_ = nullptr;
                sensor_initialized_ = false;
                return false;
            }
            address_ = ENS160_I2CADDR_0;
        } else {
            address_ = ENS160_I2CADDR_1;
        }

        sensor_->setMode(ENS160_OPMODE_STD);
        sensor_initialized_ = true;
        phase_ = 0U;
        if (log) log("INFO", "ENS160 init OK auf 0x%02X", address_);
        return true;
    }

    void logHint(unsigned long nowMs, LogFn log) {
        if (!log) return;
        if (!shouldLogProviderHint(nowMs, last_diag_ms_, consecutive_errors_, config_.diag_hint_interval_ms)) return;

        last_diag_ms_ = nowMs;
        if (last_good_ms_ == 0UL) {
            log("WARN", "ENS160 HINWEIS: noch keine gueltigen Daten; Bus, Adresse 0x52/0x53 und Warmup pruefen");
            return;
        }

        log("WARN", "ENS160 HINWEIS: letzte gueltige Messung vor %lus; Bus/Verdrahtung oder Warmup pruefen", (nowMs - last_good_ms_) / 1000UL);
    }

    ProviderUpdate setReadError(NetSenState& state, unsigned long nowMs, LogFn log, const char* reason) {
        const bool hadFault = fault_;
        fault_ = true;
        consecutive_errors_++;
        if (consecutive_errors_ >= 3UL) {
            sharedI2cBus().recover(nowMs, log, "ens160");
        }
        if (consecutive_errors_ >= 10UL) {
            sensor_initialized_ = false;
        }

        bool changed = false;
        if (last_good_ms_ == 0UL || (nowMs - last_good_ms_) > 120000UL) {
            changed = updateMetricU16(state.aqi, NET_SEN_AQI_UNGUELTIG, NET_SEN_AQI_UNGUELTIG, config_.aqi_delta) || changed;
            changed = updateMetricU16(state.tvoc_ppb, NET_SEN_TVOC_UNGUELTIG, NET_SEN_TVOC_UNGUELTIG, config_.tvoc_delta_ppb) || changed;
            changed = updateMetricU16(state.eco2_ppm, NET_SEN_ECO2_UNGUELTIG, NET_SEN_ECO2_UNGUELTIG, config_.eco2_delta_ppm) || changed;
        }
        changed = (!hadFault) || changed;

        if (log) {
            log("WARN", "ENS160 FEHLER addr=0x%02X: %s (fehler_in_folge=%lu)", address_, reason, consecutive_errors_);
        }
        logHint(nowMs, log);
        return {changed, true};
    }

    ScioSense_ENS160 sensor_0_;
    ScioSense_ENS160 sensor_1_;
    ScioSense_ENS160* sensor_ = nullptr;
    AirQualityProviderConfig config_;
    unsigned long boot_ms_ = 0UL;
    unsigned long last_cycle_ms_ = 0UL;
    unsigned long last_good_ms_ = 0UL;
    unsigned long last_diag_ms_ = 0UL;
    unsigned long wait_started_ms_ = 0UL;
    unsigned long consecutive_errors_ = 0UL;
    uint8_t address_ = 0U;
    uint8_t phase_ = 0U;
    bool ready_ = false;
    bool fault_ = false;
    bool sensor_initialized_ = false;
};
#endif

template<>
class AirQualityProvider<SH_NET_SEN_AIR_PROVIDER_ENS160_STUB> {
public:
    explicit AirQualityProvider(const AirQualityProviderConfig& config) : config_(config) {}
    void begin(unsigned long, LogFn log) {
        ready_ = true;
        if (log) log("WARN", "NET-SEN stub provider aktiv: ens160_stub (kein Hardware-Nachweis)");
    }
    const char* name() const { return "ens160_stub"; }
    bool isReady() const { return ready_; }
    ProviderUpdate poll(unsigned long, NetSenState& state, LogFn) {
        bool changed = false;
        changed = updateMetricU16(state.aqi, 150U, NET_SEN_AQI_UNGUELTIG, config_.aqi_delta) || changed;
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
