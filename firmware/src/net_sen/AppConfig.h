#pragma once

#include <stdint.h>

#include "../../lib/ShSensors/src/ProviderIds.h"

#define SH_NET_SEN_PROFILE_BASE         1U
#define SH_NET_SEN_PROFILE_ENVIRONMENT  2U

#ifndef NET_SEN_PROFILE
#define NET_SEN_PROFILE SH_NET_SEN_PROFILE_BASE
#endif

#if NET_SEN_PROFILE == SH_NET_SEN_PROFILE_BASE
  #ifndef NET_SEN_DEVICE_ID
    #define NET_SEN_DEVICE_ID "net_sen_01"
  #endif
  #ifndef NET_SEN_DEVICE_NAME
    #define NET_SEN_DEVICE_NAME "NET-SEN Base"
  #endif
  #ifndef NET_SEN_FW_VARIANT
    #define NET_SEN_FW_VARIANT "net_sen_base"
  #endif
  #ifndef NET_SEN_ENV_PROVIDER
    #define NET_SEN_ENV_PROVIDER SH_NET_SEN_ENV_PROVIDER_DHT22
  #endif
  #ifndef NET_SEN_AIR_PROVIDER
    #define NET_SEN_AIR_PROVIDER SH_NET_SEN_AIR_PROVIDER_NONE
  #endif
  #ifndef NET_SEN_LIGHT_PROVIDER
    #define NET_SEN_LIGHT_PROVIDER SH_NET_SEN_LIGHT_PROVIDER_NONE
  #endif
  #ifndef NET_SEN_MOTION_PROVIDER
    #define NET_SEN_MOTION_PROVIDER SH_NET_SEN_MOTION_PROVIDER_NONE
  #endif
  #ifndef NET_SEN_PIN_DHT22_DATA
    #define NET_SEN_PIN_DHT22_DATA 6
  #endif
#elif NET_SEN_PROFILE == SH_NET_SEN_PROFILE_ENVIRONMENT
  #ifndef NET_SEN_DEVICE_ID
    #define NET_SEN_DEVICE_ID "net_sen_env_01"
  #endif
  #ifndef NET_SEN_DEVICE_NAME
    #define NET_SEN_DEVICE_NAME "NET-SEN Umwelt"
  #endif
  #ifndef NET_SEN_FW_VARIANT
    #define NET_SEN_FW_VARIANT "net_sen_environment"
  #endif
  #ifndef NET_SEN_ENV_PROVIDER
    #define NET_SEN_ENV_PROVIDER SH_NET_SEN_ENV_PROVIDER_BME280_STUB
  #endif
  #ifndef NET_SEN_AIR_PROVIDER
    #define NET_SEN_AIR_PROVIDER SH_NET_SEN_AIR_PROVIDER_ENS160_STUB
  #endif
  #ifndef NET_SEN_LIGHT_PROVIDER
    #define NET_SEN_LIGHT_PROVIDER SH_NET_SEN_LIGHT_PROVIDER_NONE
  #endif
  #ifndef NET_SEN_MOTION_PROVIDER
    #define NET_SEN_MOTION_PROVIDER SH_NET_SEN_MOTION_PROVIDER_NONE
  #endif
  #ifndef NET_SEN_PIN_DHT22_DATA
    #define NET_SEN_PIN_DHT22_DATA -1
  #endif
#else
  #error "Unbekanntes NET_SEN_PROFILE."
#endif

#ifndef NET_SEN_DEBUG_ENABLED
#define NET_SEN_DEBUG_ENABLED 1
#endif

#ifndef NET_SEN_HELLO_RETRY_INTERVAL_MS
#define NET_SEN_HELLO_RETRY_INTERVAL_MS 5000UL
#endif

#ifndef NET_SEN_HEARTBEAT_INTERVAL_MS
#define NET_SEN_HEARTBEAT_INTERVAL_MS 5000UL
#endif

#ifndef NET_SEN_STATE_INTERVAL_MS
#define NET_SEN_STATE_INTERVAL_MS 60000UL
#endif

#ifndef NET_SEN_WLAN_CHANNEL
#define NET_SEN_WLAN_CHANNEL 6
#endif

#ifndef NET_SEN_SETUP_REENTRY_HOLD_MS
#define NET_SEN_SETUP_REENTRY_HOLD_MS 5000UL
#endif

#ifndef NET_SEN_FACTORY_RESET_HOLD_MS
#define NET_SEN_FACTORY_RESET_HOLD_MS 10000UL
#endif

#ifndef NET_SEN_TEMP_DELTA_01C
#define NET_SEN_TEMP_DELTA_01C 10
#endif

#ifndef NET_SEN_HUM_DELTA_01PCT
#define NET_SEN_HUM_DELTA_01PCT 50
#endif

#ifndef NET_SEN_LUX_DELTA
#define NET_SEN_LUX_DELTA 100
#endif

#ifndef NET_SEN_PRESSURE_DELTA_PA
#define NET_SEN_PRESSURE_DELTA_PA 150UL
#endif

#ifndef NET_SEN_AQI_DELTA
#define NET_SEN_AQI_DELTA 1U
#endif

#ifndef NET_SEN_TVOC_DELTA_PPB
#define NET_SEN_TVOC_DELTA_PPB 25U
#endif

#ifndef NET_SEN_ECO2_DELTA_PPM
#define NET_SEN_ECO2_DELTA_PPM 50U
#endif

#ifndef NET_SEN_SENSOR_I2C_CLOCK_HZ
#define NET_SEN_SENSOR_I2C_CLOCK_HZ 100000UL
#endif

#ifndef NET_SEN_SENSOR_I2C_TIMEOUT_MS
#define NET_SEN_SENSOR_I2C_TIMEOUT_MS 50U
#endif

#ifndef NET_SEN_SENSOR_I2C_SCAN_ON_BEGIN
#define NET_SEN_SENSOR_I2C_SCAN_ON_BEGIN 1
#endif

#ifndef NET_SEN_SENSOR_I2C_RECOVER_COOLDOWN_MS
#define NET_SEN_SENSOR_I2C_RECOVER_COOLDOWN_MS 5000UL
#endif

#ifndef NET_SEN_DHT_WARMUP_MS
#define NET_SEN_DHT_WARMUP_MS 2500UL
#endif

#ifndef NET_SEN_DHT_READ_INTERVAL_MS
#define NET_SEN_DHT_READ_INTERVAL_MS 2500UL
#endif

#ifndef NET_SEN_DHT_DIAG_HINT_INTERVAL_MS
#define NET_SEN_DHT_DIAG_HINT_INTERVAL_MS 15000UL
#endif

#ifndef NET_SEN_I2C_ENV_WARMUP_MS
#define NET_SEN_I2C_ENV_WARMUP_MS 1000UL
#endif

#ifndef NET_SEN_I2C_ENV_READ_INTERVAL_MS
#define NET_SEN_I2C_ENV_READ_INTERVAL_MS 5000UL
#endif

#ifndef NET_SEN_I2C_DIAG_HINT_INTERVAL_MS
#define NET_SEN_I2C_DIAG_HINT_INTERVAL_MS 15000UL
#endif

#ifndef NET_SEN_LIGHT_READ_INTERVAL_MS
#define NET_SEN_LIGHT_READ_INTERVAL_MS 2500UL
#endif

#ifndef NET_SEN_AIR_READ_INTERVAL_MS
#define NET_SEN_AIR_READ_INTERVAL_MS 5000UL
#endif

#ifndef NET_SEN_AIR_WARMUP_MS
#define NET_SEN_AIR_WARMUP_MS 120000UL
#endif

#ifndef NET_SEN_PRESENCE_UART_BAUD
#define NET_SEN_PRESENCE_UART_BAUD 256000UL
#endif

#ifndef NET_SEN_LOOP_INTERVAL_MS
#define NET_SEN_LOOP_INTERVAL_MS 50UL
#endif

// ============================================================
// NET-SEN - Geraetekonfiguration vor dem Upload
// ============================================================
// Netzbetriebener Sensor-Node ohne Relais.
// Typische Einsaetze: Raumklima, Luftqualitaet, Praesenz.
// ============================================================

constexpr char DEVICE_ID[]   = NET_SEN_DEVICE_ID;
constexpr char DEVICE_NAME[] = NET_SEN_DEVICE_NAME;
constexpr char FW_VARIANT[]  = NET_SEN_FW_VARIANT;
constexpr bool DEVICE_DEBUG_AKTIV = NET_SEN_DEBUG_ENABLED != 0;

constexpr unsigned long HELLO_RETRY_INTERVAL_MS = NET_SEN_HELLO_RETRY_INTERVAL_MS;
constexpr unsigned long HEARTBEAT_INTERVAL_MS = NET_SEN_HEARTBEAT_INTERVAL_MS;
constexpr unsigned long STATE_INTERVAL_MS = NET_SEN_STATE_INTERVAL_MS;
constexpr int WLAN_KANAL = NET_SEN_WLAN_CHANNEL;

// 5 s halten oeffnet den lokalen Setup-Modus erneut.
constexpr unsigned long SETUP_REENTRY_HOLD_MS = NET_SEN_SETUP_REENTRY_HOLD_MS;

// 10 s halten fuehrt den gemeinsamen Factory Reset aus.
constexpr unsigned long FACTORY_RESET_HOLD_MS = NET_SEN_FACTORY_RESET_HOLD_MS;

// Sofort-Meldeschwellen (Differenz zum letzten gemeldeten Wert).
constexpr int16_t  TEMP_DELTA_01C  = NET_SEN_TEMP_DELTA_01C;
constexpr uint16_t HUM_DELTA_01PCT = NET_SEN_HUM_DELTA_01PCT;
constexpr uint16_t LUX_DELTA       = NET_SEN_LUX_DELTA;
constexpr uint32_t PRESSURE_DELTA_PA = NET_SEN_PRESSURE_DELTA_PA;
constexpr uint16_t AQI_DELTA = NET_SEN_AQI_DELTA;
constexpr uint16_t TVOC_DELTA_PPB = NET_SEN_TVOC_DELTA_PPB;
constexpr uint16_t ECO2_DELTA_PPM = NET_SEN_ECO2_DELTA_PPM;
constexpr uint32_t SENSOR_I2C_CLOCK_HZ = NET_SEN_SENSOR_I2C_CLOCK_HZ;
constexpr uint16_t SENSOR_I2C_TIMEOUT_MS = NET_SEN_SENSOR_I2C_TIMEOUT_MS;
constexpr bool SENSOR_I2C_SCAN_ON_BEGIN = NET_SEN_SENSOR_I2C_SCAN_ON_BEGIN != 0;
constexpr unsigned long SENSOR_I2C_RECOVER_COOLDOWN_MS = NET_SEN_SENSOR_I2C_RECOVER_COOLDOWN_MS;
constexpr unsigned long DHT_WARMUP_MS = NET_SEN_DHT_WARMUP_MS;
constexpr unsigned long DHT_READ_INTERVAL_MS = NET_SEN_DHT_READ_INTERVAL_MS;
constexpr unsigned long DHT_DIAG_HINT_INTERVAL_MS = NET_SEN_DHT_DIAG_HINT_INTERVAL_MS;
constexpr unsigned long I2C_ENV_WARMUP_MS = NET_SEN_I2C_ENV_WARMUP_MS;
constexpr unsigned long I2C_ENV_READ_INTERVAL_MS = NET_SEN_I2C_ENV_READ_INTERVAL_MS;
constexpr unsigned long I2C_DIAG_HINT_INTERVAL_MS = NET_SEN_I2C_DIAG_HINT_INTERVAL_MS;
constexpr unsigned long LIGHT_READ_INTERVAL_MS = NET_SEN_LIGHT_READ_INTERVAL_MS;
constexpr unsigned long AIR_READ_INTERVAL_MS = NET_SEN_AIR_READ_INTERVAL_MS;
constexpr unsigned long AIR_WARMUP_MS = NET_SEN_AIR_WARMUP_MS;

// LD2410 nutzt spaeter typischerweise 256000 Baud.
constexpr unsigned long PRESENCE_UART_BAUD = NET_SEN_PRESENCE_UART_BAUD;

constexpr uint8_t NET_SEN_ENV_PROVIDER_KIND = NET_SEN_ENV_PROVIDER;
constexpr uint8_t NET_SEN_AIR_PROVIDER_KIND = NET_SEN_AIR_PROVIDER;
constexpr uint8_t NET_SEN_LIGHT_PROVIDER_KIND = NET_SEN_LIGHT_PROVIDER;
constexpr uint8_t NET_SEN_MOTION_PROVIDER_KIND = NET_SEN_MOTION_PROVIDER;

constexpr unsigned long LOOP_INTERVAL_MS = NET_SEN_LOOP_INTERVAL_MS;
