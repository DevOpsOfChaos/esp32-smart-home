#pragma once

#include <Arduino.h>
#include <stdlib.h>

#include "ProviderIds.h"
#include "../../ShProtocol/src/DeviceTypes.h"

namespace SmartHome {
namespace ShSensors {

constexpr uint8_t BAT_SEN_WINDOW_UNGUELTIG = 0xFFU;
constexpr uint16_t BAT_SEN_RAIN_UNGUELTIG = 0xFFFFU;

struct BatSenInputState {
    bool window_open;
    uint8_t button_flags;
    uint16_t rain_raw;
};

struct ReedProviderConfig {
    int pin;
};

struct ButtonProviderConfig {
    int pin_1;
    int pin_2;
    int pin_3;
    int pin_4;
};

struct RainProviderConfig {
    int pin;
    uint16_t delta_raw;
};

struct ReedUpdate {
    bool changed;
    bool window_open;
};

struct ButtonUpdate {
    bool changed;
    uint8_t button_flags;
    uint8_t press_mask;
};

struct RainUpdate {
    bool changed;
    uint16_t rain_raw;
};

inline void resetBatSenInputs(BatSenInputState& state) {
    state.window_open = false;
    state.button_flags = 0U;
    state.rain_raw = BAT_SEN_RAIN_UNGUELTIG;
}

template<uint8_t REED_PROVIDER_KIND, uint8_t BUTTON_PROVIDER_KIND, uint8_t RAIN_PROVIDER_KIND, uint8_t BUTTON_CHANNEL_COUNT>
constexpr uint16_t buildBatSenCaps() {
    return SH_CAP_BATTERY |
           ((REED_PROVIDER_KIND != SH_BAT_SEN_REED_PROVIDER_NONE) ? SH_CAP_WINDOW : 0U) |
           ((BUTTON_PROVIDER_KIND != SH_BAT_SEN_BUTTON_PROVIDER_NONE)
                ? ((BUTTON_CHANNEL_COUNT <= 1U) ? SH_CAP_BUTTON : SH_CAP_MULTIBUTTON)
                : 0U) |
           ((RAIN_PROVIDER_KIND != SH_BAT_SEN_RAIN_PROVIDER_NONE) ? SH_CAP_RAIN : 0U);
}

template<uint8_t ProviderKind>
class ReedProvider;

template<>
class ReedProvider<SH_BAT_SEN_REED_PROVIDER_NONE> {
public:
    explicit ReedProvider(const ReedProviderConfig&) {}

    void begin() {}
    const char* name() const { return "none"; }

    ReedUpdate poll() {
        return {false, false};
    }
};

template<>
class ReedProvider<SH_BAT_SEN_REED_PROVIDER_PIN> {
public:
    explicit ReedProvider(const ReedProviderConfig& config) : pin_(config.pin) {}

    void begin() {
        if (pin_ >= 0) {
            pinMode(pin_, INPUT_PULLUP);
            last_window_open_ = (digitalRead(pin_) == LOW);
        }
    }

    const char* name() const { return "reed_pin"; }

    ReedUpdate poll() {
        if (pin_ < 0) {
            return {false, false};
        }

        const bool jetzt = (digitalRead(pin_) == LOW);
        const bool changed = (jetzt != last_window_open_);
        last_window_open_ = jetzt;
        return {changed, jetzt};
    }

private:
    int pin_ = -1;
    bool last_window_open_ = false;
};

template<>
class ReedProvider<SH_BAT_SEN_REED_PROVIDER_STUB> {
public:
    explicit ReedProvider(const ReedProviderConfig&) {}

    void begin() {}
    const char* name() const { return "reed_stub"; }

    ReedUpdate poll() {
        return {false, false};
    }
};

template<uint8_t ProviderKind>
class ButtonProvider;

template<>
class ButtonProvider<SH_BAT_SEN_BUTTON_PROVIDER_NONE> {
public:
    explicit ButtonProvider(const ButtonProviderConfig&) {}

    void begin() {}
    const char* name() const { return "none"; }

    ButtonUpdate poll() {
        return {false, 0U, 0U};
    }
};

template<>
class ButtonProvider<SH_BAT_SEN_BUTTON_PROVIDER_PIN> {
public:
    explicit ButtonProvider(const ButtonProviderConfig& config) {
        pins_[0] = config.pin_1;
        pins_[1] = config.pin_2;
        pins_[2] = config.pin_3;
        pins_[3] = config.pin_4;
    }

    void begin() {
        for (uint8_t i = 0U; i < 4U; ++i) {
            if (pins_[i] >= 0) {
                pinMode(pins_[i], INPUT_PULLUP);
            }
        }
        last_flags_ = readFlags();
    }

    const char* name() const { return "button_pin"; }

    ButtonUpdate poll() {
        const uint8_t flags = readFlags();
        const uint8_t pressMask = (uint8_t)((~last_flags_) & flags);
        const bool changed = (flags != last_flags_);
        last_flags_ = flags;
        return {changed, flags, pressMask};
    }

private:
    uint8_t readFlags() const {
        uint8_t flags = 0U;
        if (pins_[0] >= 0 && digitalRead(pins_[0]) == LOW) flags |= 0x01U;
        if (pins_[1] >= 0 && digitalRead(pins_[1]) == LOW) flags |= 0x02U;
        if (pins_[2] >= 0 && digitalRead(pins_[2]) == LOW) flags |= 0x04U;
        if (pins_[3] >= 0 && digitalRead(pins_[3]) == LOW) flags |= 0x08U;
        return flags;
    }

    int pins_[4] = {-1, -1, -1, -1};
    uint8_t last_flags_ = 0U;
};

template<>
class ButtonProvider<SH_BAT_SEN_BUTTON_PROVIDER_STUB> {
public:
    explicit ButtonProvider(const ButtonProviderConfig&) {}

    void begin() {}
    const char* name() const { return "button_stub"; }

    ButtonUpdate poll() {
        return {false, 0U, 0U};
    }
};

template<uint8_t ProviderKind>
class RainProvider;

template<>
class RainProvider<SH_BAT_SEN_RAIN_PROVIDER_NONE> {
public:
    explicit RainProvider(const RainProviderConfig&) {}

    void begin() {}
    const char* name() const { return "none"; }

    RainUpdate poll() {
        return {false, BAT_SEN_RAIN_UNGUELTIG};
    }
};

template<>
class RainProvider<SH_BAT_SEN_RAIN_PROVIDER_ADC> {
public:
    explicit RainProvider(const RainProviderConfig& config) : pin_(config.pin), delta_raw_(config.delta_raw) {}

    void begin() {
        if (pin_ >= 0) {
            adcAttachPin(pin_);
        }
    }

    const char* name() const { return "rain_adc"; }

    RainUpdate poll() {
        if (pin_ < 0) {
            return {false, BAT_SEN_RAIN_UNGUELTIG};
        }

        int raw = analogRead(pin_);
        if (raw < 0) raw = 0;
        if (raw > 4095) raw = 4095;

        const uint16_t currentRaw = static_cast<uint16_t>(raw);
        const bool changed =
            last_raw_ == BAT_SEN_RAIN_UNGUELTIG ||
            abs((int)currentRaw - (int)last_raw_) >= delta_raw_;
        last_raw_ = currentRaw;
        return {changed, currentRaw};
    }

private:
    int pin_ = -1;
    uint16_t delta_raw_ = 0U;
    uint16_t last_raw_ = BAT_SEN_RAIN_UNGUELTIG;
};

template<>
class RainProvider<SH_BAT_SEN_RAIN_PROVIDER_STUB> {
public:
    explicit RainProvider(const RainProviderConfig&) {}

    void begin() {}
    const char* name() const { return "rain_stub"; }

    RainUpdate poll() {
        return {false, 2048U};
    }
};

}  // namespace ShSensors
}  // namespace SmartHome
