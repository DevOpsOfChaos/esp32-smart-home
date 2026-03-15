#pragma once

#define SH_NET_ERL_PROFILE_PILOT          1U
#define SH_NET_ERL_PROFILE_HALL_LIGHT     2U
#define SH_NET_ERL_PROFILE_KITCHEN_LIGHT  3U

#ifndef NET_ERL_PROFILE
#define NET_ERL_PROFILE SH_NET_ERL_PROFILE_PILOT
#endif

#if NET_ERL_PROFILE == SH_NET_ERL_PROFILE_PILOT
  #ifndef NET_ERL_DEVICE_ID
    #define NET_ERL_DEVICE_ID "net_erl_01"
  #endif
  #ifndef NET_ERL_DEVICE_NAME
    #define NET_ERL_DEVICE_NAME "NET-ERL Pilot"
  #endif
  #ifndef NET_ERL_FW_VARIANT
    #define NET_ERL_FW_VARIANT "net_erl_pilot"
  #endif
  #ifndef NET_ERL_PIN_BUTTON_1
    #define NET_ERL_PIN_BUTTON_1 -1
  #endif
  #ifndef NET_ERL_PIN_PIR
    #define NET_ERL_PIN_PIR -1
  #endif
  #ifndef NET_ERL_PIN_LED_RING
    #define NET_ERL_PIN_LED_RING -1
  #endif
  #ifndef NET_ERL_LED_RING_COUNT
    #define NET_ERL_LED_RING_COUNT 0
  #endif
#elif NET_ERL_PROFILE == SH_NET_ERL_PROFILE_HALL_LIGHT
  #ifndef NET_ERL_DEVICE_ID
    #define NET_ERL_DEVICE_ID "net_erl_flr_01"
  #endif
  #ifndef NET_ERL_DEVICE_NAME
    #define NET_ERL_DEVICE_NAME "NET-ERL Flurlicht"
  #endif
  #ifndef NET_ERL_FW_VARIANT
    #define NET_ERL_FW_VARIANT "net_erl_hall_light"
  #endif
  #ifndef NET_ERL_PIN_BUTTON_1
    #define NET_ERL_PIN_BUTTON_1 -1
  #endif
  #ifndef NET_ERL_PIN_PIR
    #define NET_ERL_PIN_PIR -1
  #endif
  #ifndef NET_ERL_PIN_LED_RING
    #define NET_ERL_PIN_LED_RING -1
  #endif
  #ifndef NET_ERL_LED_RING_COUNT
    #define NET_ERL_LED_RING_COUNT 0
  #endif
#elif NET_ERL_PROFILE == SH_NET_ERL_PROFILE_KITCHEN_LIGHT
  #ifndef NET_ERL_DEVICE_ID
    #define NET_ERL_DEVICE_ID "net_erl_kit_01"
  #endif
  #ifndef NET_ERL_DEVICE_NAME
    #define NET_ERL_DEVICE_NAME "NET-ERL Kuechenlicht"
  #endif
  #ifndef NET_ERL_FW_VARIANT
    #define NET_ERL_FW_VARIANT "net_erl_kitchen_light"
  #endif
  #ifndef NET_ERL_PIN_BUTTON_1
    #define NET_ERL_PIN_BUTTON_1 -1
  #endif
  #ifndef NET_ERL_PIN_PIR
    #define NET_ERL_PIN_PIR -1
  #endif
  #ifndef NET_ERL_PIN_LED_RING
    #define NET_ERL_PIN_LED_RING -1
  #endif
  #ifndef NET_ERL_LED_RING_COUNT
    #define NET_ERL_LED_RING_COUNT 17
  #endif
#else
  #error "Unbekanntes NET_ERL_PROFILE."
#endif

#ifndef NET_ERL_DEBUG_ENABLED
#define NET_ERL_DEBUG_ENABLED 1
#endif

#ifndef NET_ERL_HELLO_RETRY_INTERVAL_MS
#define NET_ERL_HELLO_RETRY_INTERVAL_MS 5000UL
#endif

#ifndef NET_ERL_HEARTBEAT_INTERVAL_MS
#define NET_ERL_HEARTBEAT_INTERVAL_MS 5000UL
#endif

#ifndef NET_ERL_WLAN_CHANNEL
#define NET_ERL_WLAN_CHANNEL 6
#endif

#ifndef NET_ERL_SETUP_REENTRY_HOLD_MS
#define NET_ERL_SETUP_REENTRY_HOLD_MS 5000UL
#endif

#ifndef NET_ERL_FACTORY_RESET_HOLD_MS
#define NET_ERL_FACTORY_RESET_HOLD_MS 10000UL
#endif

#ifndef NET_ERL_RELAY_DEFAULT_ON_BOOT
#define NET_ERL_RELAY_DEFAULT_ON_BOOT 0
#endif

#ifndef NET_ERL_INPUT_EVENT_DEBOUNCE_MS
#define NET_ERL_INPUT_EVENT_DEBOUNCE_MS 35UL
#endif

#ifndef NET_ERL_PIR_EVENT_DEBOUNCE_MS
#define NET_ERL_PIR_EVENT_DEBOUNCE_MS 50UL
#endif

#ifndef NET_ERL_LOCAL_BUTTON_LONGPRESS_MS
#define NET_ERL_LOCAL_BUTTON_LONGPRESS_MS 800UL
#endif

#ifndef NET_ERL_LOCAL_AUTO_OFF_DELAY_MS
#define NET_ERL_LOCAL_AUTO_OFF_DELAY_MS 30000UL
#endif

#ifndef NET_ERL_LOOP_INTERVAL_MS
#define NET_ERL_LOOP_INTERVAL_MS 10UL
#endif

// ============================================================
// NET-ERL - Geraetekonfiguration vor dem Upload
// ============================================================
// Netzbetriebener Node mit einem Relais.
//
// Typische Einsaetze:
//   - einfache Lampensteuerung
//   - Schaltaktor mit lokalem Taster
//   - Flurlicht (mit PIR-Erweiterung)
//   - Kuechenlampe (mit Sensor-Erweiterung und WS2812-Ring)
//
// Sondergeraete dieser Klasse erhalten eigene fw_variant,
// bleiben aber als NET-ERL-xxx Device-IDs im System.
// ============================================================

// Feste Pilot-ID fuer die erste lauffaehige Vertikalstrecke.
constexpr char DEVICE_ID[]   = NET_ERL_DEVICE_ID;

// Lesbarer Name fuer Logs und HELLO.
constexpr char DEVICE_NAME[] = NET_ERL_DEVICE_NAME;

// Profilbezogene Firmware-Variante.
constexpr char FW_VARIANT[]  = NET_ERL_FW_VARIANT;

// Debug aktiv lassen in Teststaenden.
constexpr bool DEVICE_DEBUG_AKTIV = NET_ERL_DEBUG_ENABLED != 0;

// HELLO wird bis zum ersten HELLO_ACK erneut versucht.
constexpr unsigned long HELLO_RETRY_INTERVAL_MS = NET_ERL_HELLO_RETRY_INTERVAL_MS;

// HEARTBEAT haelt die Online-Erkennung des Masters am Leben.
constexpr unsigned long HEARTBEAT_INTERVAL_MS = NET_ERL_HEARTBEAT_INTERVAL_MS;

// Muss mit dem Master-/AP-Kanal uebereinstimmen.
constexpr int WLAN_KANAL = NET_ERL_WLAN_CHANNEL;

// 5 s halten oeffnet den lokalen Setup-Modus erneut,
// ohne die persistierten Basiswerte sofort zu loeschen.
constexpr unsigned long SETUP_REENTRY_HOLD_MS = NET_ERL_SETUP_REENTRY_HOLD_MS;

// 10 s halten fuehrt den gemeinsamen Factory Reset aus
// und startet danach wieder im Setup-Modus.
constexpr unsigned long FACTORY_RESET_HOLD_MS = NET_ERL_FACTORY_RESET_HOLD_MS;

// Relais-Standardzustand beim Boot (0 = aus, 1 = an).
constexpr bool RELAY_DEFAULT_ON_BOOT = NET_ERL_RELAY_DEFAULT_ON_BOOT != 0;

// Entprellung fuer lokalen Taster und PIR-Flanken.
constexpr unsigned long INPUT_EVENT_DEBOUNCE_MS = NET_ERL_INPUT_EVENT_DEBOUNCE_MS;
constexpr unsigned long PIR_EVENT_DEBOUNCE_MS = NET_ERL_PIR_EVENT_DEBOUNCE_MS;

// Long-Press-Schwelle fuer lokale Diagnostik-/Eventmeldungen.
constexpr unsigned long LOCAL_BUTTON_LONGPRESS_MS = NET_ERL_LOCAL_BUTTON_LONGPRESS_MS;

// Nachlauf fuer lokale PIR-Automatik.
constexpr unsigned long LOCAL_AUTO_OFF_DELAY_MS = NET_ERL_LOCAL_AUTO_OFF_DELAY_MS;

// Verzoegerung der Hauptschleife.
constexpr unsigned long LOOP_INTERVAL_MS = NET_ERL_LOOP_INTERVAL_MS;
