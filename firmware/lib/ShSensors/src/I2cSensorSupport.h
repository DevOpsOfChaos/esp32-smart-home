#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "ProviderCommon.h"

namespace SmartHome {
namespace ShSensors {

struct I2cBusConfig {
    int sda_pin;
    int scl_pin;
    uint32_t clock_hz;
    uint16_t timeout_ms;
    bool scan_on_begin;
    unsigned long recover_cooldown_ms;
};

class SharedI2cBus {
public:
    bool begin(const I2cBusConfig& config, LogFn log) {
        if (ready_) {
            return true;
        }

        config_ = config;
        if (config.sda_pin < 0 || config.scl_pin < 0) {
            if (log) {
                log("WARN", "I2C Bus ungueltig: SDA=%d SCL=%d", config.sda_pin, config.scl_pin);
            }
            ready_ = false;
            return false;
        }

        Wire.begin(config.sda_pin, config.scl_pin);
        Wire.setClock(config.clock_hz);
        Wire.setTimeOut(config.timeout_ms);
        ready_ = true;
        scanned_ = false;

        if (config.scan_on_begin) {
            scanOnce(log);
        }
        return true;
    }

    bool isReady() const { return ready_; }

    void scanOnce(LogFn log) {
        if (!ready_ || scanned_) {
            return;
        }

        scanned_ = true;
        uint8_t found = 0U;
        if (log) {
            log("INFO", "I2C scan gestartet");
        }

        for (uint8_t address = 0x08U; address <= 0x77U; ++address) {
            Wire.beginTransmission(address);
            const uint8_t err = Wire.endTransmission();
            if (err == 0U) {
                found++;
                if (log) {
                    log("INFO", "I2C device 0x%02X gefunden", address);
                }
            }
            delay(0);
        }

        if (found == 0U && log) {
            log("WARN", "I2C scan fand kein Geraet; Verdrahtung, Power und Pull-ups pruefen");
        }
    }

    bool ping(uint8_t address) const {
        if (!ready_) {
            return false;
        }

        Wire.beginTransmission(address);
        return Wire.endTransmission() == 0U;
    }

    bool readRegisterU16(uint8_t address, uint8_t reg, uint16_t& value) const {
        if (!ready_) {
            return false;
        }

        Wire.beginTransmission(address);
        Wire.write(reg);
        if (Wire.endTransmission(true) != 0U) {
            return false;
        }

        const int got = Wire.requestFrom(address, (uint8_t)2U, (uint8_t)true);
        if (got != 2 || Wire.available() < 2) {
            return false;
        }

        const uint8_t lo = (uint8_t)Wire.read();
        const uint8_t hi = (uint8_t)Wire.read();
        value = (uint16_t)(((uint16_t)hi << 8) | lo);
        return true;
    }

    bool recover(unsigned long nowMs, LogFn log, const char* reason) {
        if (!ready_) {
            return false;
        }

        if (last_recover_ms_ != 0UL &&
            (nowMs - last_recover_ms_) < config_.recover_cooldown_ms) {
            return false;
        }

        last_recover_ms_ = nowMs;
        if (log) {
            log(
                "WARN",
                "I2C recover gestartet (%s)",
                reason != nullptr ? reason : "unspezifiziert");
        }

        Wire.end();
        pinMode(config_.sda_pin, OUTPUT_OPEN_DRAIN);
        pinMode(config_.scl_pin, OUTPUT_OPEN_DRAIN);
        digitalWrite(config_.sda_pin, HIGH);
        digitalWrite(config_.scl_pin, HIGH);
        delayMicroseconds(5);

        for (uint8_t i = 0U; i < 9U; ++i) {
            digitalWrite(config_.scl_pin, LOW);
            delayMicroseconds(5);
            digitalWrite(config_.scl_pin, HIGH);
            delayMicroseconds(5);
        }

        digitalWrite(config_.sda_pin, LOW);
        delayMicroseconds(5);
        digitalWrite(config_.scl_pin, HIGH);
        delayMicroseconds(5);
        digitalWrite(config_.sda_pin, HIGH);
        delayMicroseconds(5);

        Wire.begin(config_.sda_pin, config_.scl_pin);
        Wire.setClock(config_.clock_hz);
        Wire.setTimeOut(config_.timeout_ms);
        scanned_ = false;
        return true;
    }

private:
    I2cBusConfig config_ = {-1, -1, 100000UL, 50U, true, 5000UL};
    bool ready_ = false;
    bool scanned_ = false;
    unsigned long last_recover_ms_ = 0UL;
};

inline SharedI2cBus& sharedI2cBus() {
    static SharedI2cBus bus;
    return bus;
}

template<size_t N>
inline bool firstPresentI2cAddress(const uint8_t (&addresses)[N], uint8_t& foundAddress) {
    for (size_t i = 0U; i < N; ++i) {
        if (sharedI2cBus().ping(addresses[i])) {
            foundAddress = addresses[i];
            return true;
        }
    }

    foundAddress = 0U;
    return false;
}

}  // namespace ShSensors
}  // namespace SmartHome
