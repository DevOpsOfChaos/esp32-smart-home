#pragma once

#define SH_NET_ZRL_PROFILE_BASE            1U
#define SH_NET_ZRL_PROFILE_ROLLER_SHUTTER  2U

#ifndef NET_ZRL_PROFILE
#define NET_ZRL_PROFILE SH_NET_ZRL_PROFILE_BASE
#endif

#if NET_ZRL_PROFILE == SH_NET_ZRL_PROFILE_BASE
  #ifndef NET_ZRL_DEVICE_ID
    #define NET_ZRL_DEVICE_ID "net_zrl_01"
  #endif
  #ifndef NET_ZRL_DEVICE_NAME
    #define NET_ZRL_DEVICE_NAME "NET-ZRL Base"
  #endif
  #ifndef NET_ZRL_FW_VARIANT
    #define NET_ZRL_FW_VARIANT "net_zrl_base"
  #endif
  #ifndef NET_ZRL_COVER_MODE_ACTIVE
    #define NET_ZRL_COVER_MODE_ACTIVE 0
  #endif
#elif NET_ZRL_PROFILE == SH_NET_ZRL_PROFILE_ROLLER_SHUTTER
  #ifndef NET_ZRL_DEVICE_ID
    #define NET_ZRL_DEVICE_ID "net_zrl_rol_01"
  #endif
  #ifndef NET_ZRL_DEVICE_NAME
    #define NET_ZRL_DEVICE_NAME "NET-ZRL Rolladen"
  #endif
  #ifndef NET_ZRL_FW_VARIANT
    #define NET_ZRL_FW_VARIANT "net_zrl_roller_shutter"
  #endif
  #ifndef NET_ZRL_COVER_MODE_ACTIVE
    #define NET_ZRL_COVER_MODE_ACTIVE 1
  #endif
#else
  #error "Unbekanntes NET_ZRL_PROFILE."
#endif

#ifndef NET_ZRL_DEBUG_ENABLED
#define NET_ZRL_DEBUG_ENABLED 1
#endif

#ifndef NET_ZRL_HELLO_RETRY_INTERVAL_MS
#define NET_ZRL_HELLO_RETRY_INTERVAL_MS 5000UL
#endif

#ifndef NET_ZRL_HEARTBEAT_INTERVAL_MS
#define NET_ZRL_HEARTBEAT_INTERVAL_MS 5000UL
#endif

#ifndef NET_ZRL_STATE_INTERVAL_MS
#define NET_ZRL_STATE_INTERVAL_MS 30000UL
#endif

#ifndef NET_ZRL_WLAN_CHANNEL
#define NET_ZRL_WLAN_CHANNEL 6
#endif

#ifndef NET_ZRL_SETUP_REENTRY_HOLD_MS
#define NET_ZRL_SETUP_REENTRY_HOLD_MS 5000UL
#endif

#ifndef NET_ZRL_FACTORY_RESET_HOLD_MS
#define NET_ZRL_FACTORY_RESET_HOLD_MS 10000UL
#endif

#ifndef NET_ZRL_RELAY_1_DEFAULT_ON_BOOT
#define NET_ZRL_RELAY_1_DEFAULT_ON_BOOT 0
#endif

#ifndef NET_ZRL_RELAY_2_DEFAULT_ON_BOOT
#define NET_ZRL_RELAY_2_DEFAULT_ON_BOOT 0
#endif

#ifndef NET_ZRL_INPUT_EVENT_DEBOUNCE_MS
#define NET_ZRL_INPUT_EVENT_DEBOUNCE_MS 35UL
#endif

#ifndef NET_ZRL_LOCAL_BUTTON_LONGPRESS_MS
#define NET_ZRL_LOCAL_BUTTON_LONGPRESS_MS 800UL
#endif

#ifndef NET_ZRL_COVER_REVERSE_LOCK_MS
#define NET_ZRL_COVER_REVERSE_LOCK_MS 500UL
#endif

#ifndef NET_ZRL_LOOP_INTERVAL_MS
#define NET_ZRL_LOOP_INTERVAL_MS 10UL
#endif

// ============================================================
// NET-ZRL - Geraetekonfiguration vor dem Upload
// ============================================================
// Netzbetriebener Zwei-Relais-Node.
// Basis fuer unterschiedliche Geraetetypen innerhalb derselben
// Basisklasse.
// ============================================================

constexpr char DEVICE_ID[]   = NET_ZRL_DEVICE_ID;
constexpr char DEVICE_NAME[] = NET_ZRL_DEVICE_NAME;
constexpr char FW_VARIANT[]  = NET_ZRL_FW_VARIANT;
constexpr bool DEVICE_DEBUG_AKTIV = NET_ZRL_DEBUG_ENABLED != 0;

constexpr unsigned long HELLO_RETRY_INTERVAL_MS = NET_ZRL_HELLO_RETRY_INTERVAL_MS;
constexpr unsigned long HEARTBEAT_INTERVAL_MS = NET_ZRL_HEARTBEAT_INTERVAL_MS;
constexpr unsigned long STATE_INTERVAL_MS = NET_ZRL_STATE_INTERVAL_MS;
constexpr int WLAN_KANAL = NET_ZRL_WLAN_CHANNEL;

// 5 s halten oeffnet den lokalen Setup-Modus erneut,
// ohne die persistierten Basiswerte sofort zu loeschen.
constexpr unsigned long SETUP_REENTRY_HOLD_MS = NET_ZRL_SETUP_REENTRY_HOLD_MS;

// 10 s halten fuehrt den gemeinsamen Factory Reset aus
// und startet danach wieder im Setup-Modus.
constexpr unsigned long FACTORY_RESET_HOLD_MS = NET_ZRL_FACTORY_RESET_HOLD_MS;

constexpr bool RELAY_1_DEFAULT_ON_BOOT = NET_ZRL_RELAY_1_DEFAULT_ON_BOOT != 0;
constexpr bool RELAY_2_DEFAULT_ON_BOOT = NET_ZRL_RELAY_2_DEFAULT_ON_BOOT != 0;

// Entprellung fuer lokale Taster.
constexpr unsigned long INPUT_EVENT_DEBOUNCE_MS = NET_ZRL_INPUT_EVENT_DEBOUNCE_MS;

// Long-Press-Schwelle fuer lokale Button-Events.
constexpr unsigned long LOCAL_BUTTON_LONGPRESS_MS = NET_ZRL_LOCAL_BUTTON_LONGPRESS_MS;

// Sperrzeit nach Richtungswechsel (ms).
constexpr unsigned long COVER_REVERSE_LOCK_MS = NET_ZRL_COVER_REVERSE_LOCK_MS;

// Rolladen-Sondermodus aktiv?
constexpr bool COVER_MODUS_AKTIV = NET_ZRL_COVER_MODE_ACTIVE != 0;
constexpr unsigned long LOOP_INTERVAL_MS = NET_ZRL_LOOP_INTERVAL_MS;
