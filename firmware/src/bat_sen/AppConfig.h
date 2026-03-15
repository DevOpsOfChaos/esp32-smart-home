#pragma once

#include <stdint.h>

#include "../../lib/ShSensors/src/ProviderIds.h"

#define SH_BAT_SEN_PROFILE_BASE            1U
#define SH_BAT_SEN_PROFILE_WINDOW_CONTACT  2U
#define SH_BAT_SEN_PROFILE_DOOR_CONTACT    3U
#define SH_BAT_SEN_PROFILE_RAIN_NODE       4U
#define SH_BAT_SEN_PROFILE_WALL_SWITCH     5U

#ifndef BAT_SEN_PROFILE
#define BAT_SEN_PROFILE SH_BAT_SEN_PROFILE_BASE
#endif

enum BatteryProfileV1 : uint8_t {
    BAT_PROFILE_COIN_3V_PRIMARY = 1U,
    BAT_PROFILE_ALKALINE_2X = 2U,
    BAT_PROFILE_LIION_1S = 3U,
};

struct BatteryProfileVoltageRange {
    uint16_t leer_mv;
    uint16_t voll_mv;
};

#if BAT_SEN_PROFILE == SH_BAT_SEN_PROFILE_BASE
  #ifndef BAT_SEN_DEVICE_ID
    #define BAT_SEN_DEVICE_ID "bat_sen_01"
  #endif
  #ifndef BAT_SEN_DEVICE_NAME
    #define BAT_SEN_DEVICE_NAME "BAT-SEN Base"
  #endif
  #ifndef BAT_SEN_FW_VARIANT
    #define BAT_SEN_FW_VARIANT "bat_sen_base"
  #endif
  #ifndef BAT_SEN_REED_PROVIDER
    #define BAT_SEN_REED_PROVIDER SH_BAT_SEN_REED_PROVIDER_NONE
  #endif
  #ifndef BAT_SEN_BUTTON_PROVIDER
    #define BAT_SEN_BUTTON_PROVIDER SH_BAT_SEN_BUTTON_PROVIDER_NONE
  #endif
  #ifndef BAT_SEN_RAIN_PROVIDER
    #define BAT_SEN_RAIN_PROVIDER SH_BAT_SEN_RAIN_PROVIDER_NONE
  #endif
  #ifndef BAT_SEN_BUTTON_CHANNELS
    #define BAT_SEN_BUTTON_CHANNELS 1U
  #endif
#elif BAT_SEN_PROFILE == SH_BAT_SEN_PROFILE_WINDOW_CONTACT
  #ifndef BAT_SEN_DEVICE_ID
    #define BAT_SEN_DEVICE_ID "bat_sen_win_01"
  #endif
  #ifndef BAT_SEN_DEVICE_NAME
    #define BAT_SEN_DEVICE_NAME "BAT-SEN Fenster"
  #endif
  #ifndef BAT_SEN_FW_VARIANT
    #define BAT_SEN_FW_VARIANT "bat_sen_window_contact"
  #endif
  #ifndef BAT_SEN_REED_PROVIDER
    #define BAT_SEN_REED_PROVIDER SH_BAT_SEN_REED_PROVIDER_STUB
  #endif
  #ifndef BAT_SEN_BUTTON_PROVIDER
    #define BAT_SEN_BUTTON_PROVIDER SH_BAT_SEN_BUTTON_PROVIDER_NONE
  #endif
  #ifndef BAT_SEN_RAIN_PROVIDER
    #define BAT_SEN_RAIN_PROVIDER SH_BAT_SEN_RAIN_PROVIDER_NONE
  #endif
  #ifndef BAT_SEN_BUTTON_CHANNELS
    #define BAT_SEN_BUTTON_CHANNELS 1U
  #endif
#elif BAT_SEN_PROFILE == SH_BAT_SEN_PROFILE_DOOR_CONTACT
  #ifndef BAT_SEN_DEVICE_ID
    #define BAT_SEN_DEVICE_ID "bat_sen_door_01"
  #endif
  #ifndef BAT_SEN_DEVICE_NAME
    #define BAT_SEN_DEVICE_NAME "BAT-SEN Tuer"
  #endif
  #ifndef BAT_SEN_FW_VARIANT
    #define BAT_SEN_FW_VARIANT "bat_sen_door_contact"
  #endif
  #ifndef BAT_SEN_REED_PROVIDER
    #define BAT_SEN_REED_PROVIDER SH_BAT_SEN_REED_PROVIDER_STUB
  #endif
  #ifndef BAT_SEN_BUTTON_PROVIDER
    #define BAT_SEN_BUTTON_PROVIDER SH_BAT_SEN_BUTTON_PROVIDER_NONE
  #endif
  #ifndef BAT_SEN_RAIN_PROVIDER
    #define BAT_SEN_RAIN_PROVIDER SH_BAT_SEN_RAIN_PROVIDER_NONE
  #endif
  #ifndef BAT_SEN_BUTTON_CHANNELS
    #define BAT_SEN_BUTTON_CHANNELS 1U
  #endif
#elif BAT_SEN_PROFILE == SH_BAT_SEN_PROFILE_RAIN_NODE
  #ifndef BAT_SEN_DEVICE_ID
    #define BAT_SEN_DEVICE_ID "bat_sen_rain_01"
  #endif
  #ifndef BAT_SEN_DEVICE_NAME
    #define BAT_SEN_DEVICE_NAME "BAT-SEN Regen"
  #endif
  #ifndef BAT_SEN_FW_VARIANT
    #define BAT_SEN_FW_VARIANT "bat_sen_rain_node"
  #endif
  #ifndef BAT_SEN_REED_PROVIDER
    #define BAT_SEN_REED_PROVIDER SH_BAT_SEN_REED_PROVIDER_NONE
  #endif
  #ifndef BAT_SEN_BUTTON_PROVIDER
    #define BAT_SEN_BUTTON_PROVIDER SH_BAT_SEN_BUTTON_PROVIDER_NONE
  #endif
  #ifndef BAT_SEN_RAIN_PROVIDER
    #define BAT_SEN_RAIN_PROVIDER SH_BAT_SEN_RAIN_PROVIDER_STUB
  #endif
  #ifndef BAT_SEN_BUTTON_CHANNELS
    #define BAT_SEN_BUTTON_CHANNELS 1U
  #endif
#elif BAT_SEN_PROFILE == SH_BAT_SEN_PROFILE_WALL_SWITCH
  #ifndef BAT_SEN_DEVICE_ID
    #define BAT_SEN_DEVICE_ID "bat_sen_wsw_01"
  #endif
  #ifndef BAT_SEN_DEVICE_NAME
    #define BAT_SEN_DEVICE_NAME "BAT-SEN Wandschalter"
  #endif
  #ifndef BAT_SEN_FW_VARIANT
    #define BAT_SEN_FW_VARIANT "bat_sen_wall_switch"
  #endif
  #ifndef BAT_SEN_REED_PROVIDER
    #define BAT_SEN_REED_PROVIDER SH_BAT_SEN_REED_PROVIDER_NONE
  #endif
  #ifndef BAT_SEN_BUTTON_PROVIDER
    #define BAT_SEN_BUTTON_PROVIDER SH_BAT_SEN_BUTTON_PROVIDER_STUB
  #endif
  #ifndef BAT_SEN_RAIN_PROVIDER
    #define BAT_SEN_RAIN_PROVIDER SH_BAT_SEN_RAIN_PROVIDER_NONE
  #endif
  #ifndef BAT_SEN_BUTTON_CHANNELS
    #define BAT_SEN_BUTTON_CHANNELS 2U
  #endif
#else
  #error "Unbekanntes BAT_SEN_PROFILE."
#endif

#ifndef BAT_SEN_DEBUG_ENABLED
#define BAT_SEN_DEBUG_ENABLED 1
#endif

#ifndef BAT_SEN_HELLO_RETRY_INTERVAL_MS
#define BAT_SEN_HELLO_RETRY_INTERVAL_MS 5000UL
#endif

#ifndef BAT_SEN_WLAN_CHANNEL
#define BAT_SEN_WLAN_CHANNEL 6
#endif

#ifndef BAT_SEN_SETUP_REENTRY_HOLD_MS
#define BAT_SEN_SETUP_REENTRY_HOLD_MS 5000UL
#endif

#ifndef BAT_SEN_FACTORY_RESET_HOLD_MS
#define BAT_SEN_FACTORY_RESET_HOLD_MS 10000UL
#endif

#ifndef BAT_SEN_SLEEP_INTERVAL_S
#define BAT_SEN_SLEEP_INTERVAL_S 300UL
#endif

#ifndef BAT_SEN_RX_WINDOW_MS
#define BAT_SEN_RX_WINDOW_MS 300UL
#endif

#ifndef BAT_SEN_WACH_NACH_EVENT_MS
#define BAT_SEN_WACH_NACH_EVENT_MS 1000UL
#endif

#ifndef BAT_SEN_LOW_BATTERY_PCT
#define BAT_SEN_LOW_BATTERY_PCT 20
#endif

#ifndef BAT_SEN_BATTERY_PROFILE_ID
#define BAT_SEN_BATTERY_PROFILE_ID BAT_PROFILE_COIN_3V_PRIMARY
#endif

#ifndef BAT_SEN_BATTERY_DIVIDER_TOP_OHM
#define BAT_SEN_BATTERY_DIVIDER_TOP_OHM 100000UL
#endif

#ifndef BAT_SEN_BATTERY_DIVIDER_BOTTOM_OHM
#define BAT_SEN_BATTERY_DIVIDER_BOTTOM_OHM 100000UL
#endif

#ifndef BAT_SEN_BATTERY_ADC_SAMPLE_COUNT
#define BAT_SEN_BATTERY_ADC_SAMPLE_COUNT 4U
#endif

#ifndef BAT_SEN_RAIN_DELTA_RAW
#define BAT_SEN_RAIN_DELTA_RAW 32U
#endif

#ifndef BAT_SEN_INPUT_EVENT_DEBOUNCE_MS
#define BAT_SEN_INPUT_EVENT_DEBOUNCE_MS 35UL
#endif

#ifndef BAT_SEN_WINDOW_DEBOUNCE_MS
#define BAT_SEN_WINDOW_DEBOUNCE_MS 40UL
#endif

#ifndef BAT_SEN_MULTI_BUTTON_LONGPRESS_MS
#define BAT_SEN_MULTI_BUTTON_LONGPRESS_MS 800UL
#endif

#ifndef BAT_SEN_REED_ACTIVE_LOW
#define BAT_SEN_REED_ACTIVE_LOW 1
#endif

#ifndef BAT_SEN_REED_USE_PULLUP
#define BAT_SEN_REED_USE_PULLUP 1
#endif

#ifndef BAT_SEN_BUTTON_ACTIVE_LOW
#define BAT_SEN_BUTTON_ACTIVE_LOW 1
#endif

#ifndef BAT_SEN_BUTTON_USE_PULLUP
#define BAT_SEN_BUTTON_USE_PULLUP 1
#endif

#ifndef BAT_SEN_LOOP_INTERVAL_MS
#define BAT_SEN_LOOP_INTERVAL_MS 25UL
#endif

// ============================================================
// BAT-SEN - Geraetekonfiguration vor dem Upload
// ============================================================
// Batteriebetriebener Sensor- und Event-Node.
// Das Geraet schlaeft die meiste Zeit und wacht nur auf
// bei Ereignissen oder nach dem Schlafintervall.
// ============================================================

constexpr char DEVICE_ID[]   = BAT_SEN_DEVICE_ID;
constexpr char DEVICE_NAME[] = BAT_SEN_DEVICE_NAME;
constexpr char FW_VARIANT[]  = BAT_SEN_FW_VARIANT;
constexpr bool DEVICE_DEBUG_AKTIV = BAT_SEN_DEBUG_ENABLED != 0;

constexpr unsigned long HELLO_RETRY_INTERVAL_MS = BAT_SEN_HELLO_RETRY_INTERVAL_MS;
constexpr int WLAN_KANAL = BAT_SEN_WLAN_CHANNEL;

// 5 s halten oeffnet den lokalen Setup-Modus erneut.
constexpr unsigned long SETUP_REENTRY_HOLD_MS = BAT_SEN_SETUP_REENTRY_HOLD_MS;

// 10 s halten fuehrt den gemeinsamen Factory Reset aus.
constexpr unsigned long FACTORY_RESET_HOLD_MS = BAT_SEN_FACTORY_RESET_HOLD_MS;

// Schlafintervall (Sekunden).
constexpr unsigned long SLEEP_INTERVAL_S = BAT_SEN_SLEEP_INTERVAL_S;
constexpr unsigned long HEARTBEAT_INTERVAL_MS = SLEEP_INTERVAL_S * 1000UL;
constexpr unsigned long STATE_INTERVAL_MS = SLEEP_INTERVAL_S * 1000UL;

// Empfangsfenster nach TX (Millisekunden).
constexpr unsigned long RX_WINDOW_MS = BAT_SEN_RX_WINDOW_MS;

// Wachhaltezeit nach Ereignis vor Schlaf (Millisekunden).
constexpr unsigned long WACH_NACH_EVENT_MS = BAT_SEN_WACH_NACH_EVENT_MS;

// Batterie: Warnung unter diesem Prozentsatz.
constexpr uint8_t LOW_BATTERY_PCT = BAT_SEN_LOW_BATTERY_PCT;

// V1 supports exactly these three profiles.
constexpr BatteryProfileV1 BATTERY_PROFILE = static_cast<BatteryProfileV1>(BAT_SEN_BATTERY_PROFILE_ID);

constexpr uint32_t BATTERY_DIVIDER_TOP_OHM = BAT_SEN_BATTERY_DIVIDER_TOP_OHM;
constexpr uint32_t BATTERY_DIVIDER_BOTTOM_OHM = BAT_SEN_BATTERY_DIVIDER_BOTTOM_OHM;
constexpr uint8_t BATTERY_ADC_SAMPLE_COUNT = BAT_SEN_BATTERY_ADC_SAMPLE_COUNT;
constexpr uint16_t RAIN_DELTA_RAW = BAT_SEN_RAIN_DELTA_RAW;
constexpr unsigned long INPUT_EVENT_DEBOUNCE_MS = BAT_SEN_INPUT_EVENT_DEBOUNCE_MS;
constexpr unsigned long WINDOW_DEBOUNCE_MS = BAT_SEN_WINDOW_DEBOUNCE_MS;
constexpr unsigned long MULTI_BUTTON_LONGPRESS_MS = BAT_SEN_MULTI_BUTTON_LONGPRESS_MS;

constexpr BatteryProfileVoltageRange BATTERY_PROFILE_COIN_3V_PRIMARY_RANGE = {2000U, 3000U};
constexpr BatteryProfileVoltageRange BATTERY_PROFILE_ALKALINE_2X_RANGE = {2000U, 3200U};
constexpr BatteryProfileVoltageRange BATTERY_PROFILE_LIION_1S_RANGE = {3200U, 4200U};

static_assert(
    BAT_SEN_BATTERY_PROFILE_ID == BAT_PROFILE_COIN_3V_PRIMARY ||
    BAT_SEN_BATTERY_PROFILE_ID == BAT_PROFILE_ALKALINE_2X ||
    BAT_SEN_BATTERY_PROFILE_ID == BAT_PROFILE_LIION_1S,
    "BAT-SEN V1 supports only the explicit coin/alcaline/liion battery profiles.");

constexpr uint8_t BAT_SEN_REED_PROVIDER_KIND = BAT_SEN_REED_PROVIDER;
constexpr bool BAT_SEN_REED_SIGNAL_ACTIVE_LOW = BAT_SEN_REED_ACTIVE_LOW != 0;
constexpr bool BAT_SEN_REED_SIGNAL_USE_PULLUP = BAT_SEN_REED_USE_PULLUP != 0;
constexpr uint8_t BAT_SEN_BUTTON_PROVIDER_KIND = BAT_SEN_BUTTON_PROVIDER;
constexpr bool BAT_SEN_BUTTON_SIGNAL_ACTIVE_LOW = BAT_SEN_BUTTON_ACTIVE_LOW != 0;
constexpr bool BAT_SEN_BUTTON_SIGNAL_USE_PULLUP = BAT_SEN_BUTTON_USE_PULLUP != 0;
constexpr uint8_t BAT_SEN_RAIN_PROVIDER_KIND = BAT_SEN_RAIN_PROVIDER;
constexpr uint8_t BAT_SEN_BUTTON_CHANNEL_COUNT = BAT_SEN_BUTTON_CHANNELS;

constexpr unsigned long LOOP_INTERVAL_MS = BAT_SEN_LOOP_INTERVAL_MS;
