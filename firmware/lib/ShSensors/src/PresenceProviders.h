#pragma once

#include <Arduino.h>

#include "ProviderCommon.h"
#include "ProviderIds.h"

namespace SmartHome {
namespace ShSensors {

struct PresenceState {
    bool motion;
    bool presence;
    bool out_pin_active;
    bool uart_motion_active;
    bool uart_presence_active;
};

struct PresenceProviderConfig {
    int out_pin;
    int uart_rx_pin;
    int uart_tx_pin;
    unsigned long uart_baud;
};

inline void resetPresenceState(PresenceState& state) {
    state.motion = false;
    state.presence = false;
    state.out_pin_active = false;
    state.uart_motion_active = false;
    state.uart_presence_active = false;
}

inline bool updatePresenceState(PresenceState& current, const PresenceState& next) {
    const bool changed =
        current.motion != next.motion ||
        current.presence != next.presence ||
        current.out_pin_active != next.out_pin_active ||
        current.uart_motion_active != next.uart_motion_active ||
        current.uart_presence_active != next.uart_presence_active;
    current = next;
    return changed;
}

inline bool presenceIsActive(const PresenceState& state) {
    return state.presence;
}

inline PresenceState buildPirPresenceState(bool signalHigh) {
    PresenceState state = {};
    state.motion = signalHigh;
    state.presence = signalHigh;
    state.out_pin_active = signalHigh;
    return state;
}

inline PresenceState buildRadarPresenceState(bool outPinActive, bool uartMotionActive, bool uartPresenceActive) {
    PresenceState state = {};
    state.out_pin_active = outPinActive;
    state.uart_motion_active = uartMotionActive;
    state.uart_presence_active = uartPresenceActive;
    state.motion = outPinActive || uartMotionActive;
    state.presence = state.motion || uartPresenceActive;
    return state;
}

constexpr bool presenceProviderProvidesSignal(uint8_t providerKind) {
    return providerKind == SH_PRESENCE_PROVIDER_PIR_PIN ||
           providerKind == SH_PRESENCE_PROVIDER_PIR_STUB ||
           providerKind == SH_PRESENCE_PROVIDER_LD2410C ||
           providerKind == SH_PRESENCE_PROVIDER_LD2410B;
}

template<uint8_t ProviderKind>
class PresenceProvider;

template<>
class PresenceProvider<SH_PRESENCE_PROVIDER_NONE> {
public:
    explicit PresenceProvider(const PresenceProviderConfig&) {}

    void begin(LogFn) {
        ready_ = true;
    }

    const char* name() const { return "none"; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(PresenceState&, LogFn) {
        return {false, false};
    }

private:
    bool ready_ = false;
};

template<>
class PresenceProvider<SH_PRESENCE_PROVIDER_PIR_PIN> {
public:
    explicit PresenceProvider(const PresenceProviderConfig& config) : out_pin_(config.out_pin) {}

    void begin(LogFn) {
        if (out_pin_ >= 0) {
            pinMode(out_pin_, INPUT);
        }
        ready_ = true;
    }

    const char* name() const { return "pir_pin"; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(PresenceState& state, LogFn) {
        if (out_pin_ < 0) {
            const PresenceState next = buildPirPresenceState(false);
            return {updatePresenceState(state, next), false};
        }

        const PresenceState next = buildPirPresenceState(digitalRead(out_pin_) == HIGH);
        return {updatePresenceState(state, next), false};
    }

private:
    int out_pin_ = -1;
    bool ready_ = false;
};

template<>
class PresenceProvider<SH_PRESENCE_PROVIDER_PIR_STUB> {
public:
    explicit PresenceProvider(const PresenceProviderConfig&) {}

    void begin(LogFn log) {
        ready_ = true;
        if (log) {
            log("WARN", "Presence stub provider aktiv: pir_stub (kein Hardware-Nachweis)");
        }
    }

    const char* name() const { return "pir_stub"; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(PresenceState& state, LogFn) {
        const PresenceState next = buildPirPresenceState(false);
        return {updatePresenceState(state, next), false};
    }

private:
    bool ready_ = false;
};

class Ld2410PresenceProviderCommon {
public:
    Ld2410PresenceProviderCommon(const PresenceProviderConfig& config, const char* providerName)
        : config_(config), provider_name_(providerName) {}

    void begin(LogFn log) {
        if (config_.out_pin >= 0) {
            pinMode(config_.out_pin, INPUT);
        }
        ready_ = true;

        if (log && uartConfigured()) {
            log(
                "WARN",
                "%s UART-State nur vorbereitet; Parser in diesem Stand absichtlich noch nicht implementiert",
                provider_name_);
        }
    }

    const char* name() const { return provider_name_; }
    bool isReady() const { return ready_; }

    ProviderUpdate poll(PresenceState& state, LogFn) {
        const bool outPinActive =
            config_.out_pin >= 0 && digitalRead(config_.out_pin) == HIGH;
        const PresenceState next = buildRadarPresenceState(outPinActive, false, false);
        return {updatePresenceState(state, next), false};
    }

protected:
    bool uartConfigured() const {
        return config_.uart_rx_pin >= 0 || config_.uart_tx_pin >= 0;
    }

private:
    PresenceProviderConfig config_;
    const char* provider_name_ = "ld2410";
    bool ready_ = false;
};

template<>
class PresenceProvider<SH_PRESENCE_PROVIDER_LD2410C> : public Ld2410PresenceProviderCommon {
public:
    explicit PresenceProvider(const PresenceProviderConfig& config)
        : Ld2410PresenceProviderCommon(config, "ld2410c") {}
};

template<>
class PresenceProvider<SH_PRESENCE_PROVIDER_LD2410B> : public Ld2410PresenceProviderCommon {
public:
    explicit PresenceProvider(const PresenceProviderConfig& config)
        : Ld2410PresenceProviderCommon(config, "ld2410b") {}
};

}  // namespace ShSensors
}  // namespace SmartHome
