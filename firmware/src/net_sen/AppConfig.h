#pragma once

#include <stdint.h>

#include "../../lib/ShSensors/src/ProviderIds.h"

// ============================================================
// NET-SEN – Gerätekonfiguration vor dem Upload
// ============================================================
// Netzbetriebener Sensor-Node ohne Relais.
// Typische Einsätze: Raumklima, Luftqualität, Präsenz.
// ============================================================

constexpr char DEVICE_ID[]   = "net_sen_01";
constexpr char DEVICE_NAME[] = "NET-SEN Base";
constexpr char FW_VARIANT[]  = "net_sen_base";
constexpr bool DEVICE_DEBUG_AKTIV = true;

constexpr unsigned long HELLO_RETRY_INTERVAL_MS = 5000UL;
constexpr unsigned long HEARTBEAT_INTERVAL_MS = 5000UL;
constexpr unsigned long STATE_INTERVAL_MS = 60000UL;
constexpr int WLAN_KANAL = 6;

// 5 s halten oeffnet den lokalen Setup-Modus erneut,
// ohne die persistierten Basiswerte sofort zu loeschen.
constexpr unsigned long SETUP_REENTRY_HOLD_MS = 5000UL;

// 10 s halten fuehrt den gemeinsamen Factory Reset aus
// und startet danach wieder im Setup-Modus.
constexpr unsigned long FACTORY_RESET_HOLD_MS = 10000UL;

// Sofort-Meldeschwellen (Differenz zum letzten gemeldeten Wert):
// Begründung: Deutliche Änderungen (1 °C, 5 % rF) sofort senden,
// damit der Server nicht auf das nächste Intervall warten muss.
constexpr int16_t  TEMP_DELTA_01C  = 10;  // 1.0 °C in 0.1-°C-Schritten
constexpr uint16_t HUM_DELTA_01PCT = 50;  // 5.0 % in 0.1-%-Schritten
constexpr uint16_t LUX_DELTA       = 100; // 100 Lux
constexpr uint32_t PRESSURE_DELTA_PA = 150UL;
constexpr uint16_t AQI_DELTA = 1U;
constexpr uint16_t TVOC_DELTA_PPB = 25U;
constexpr uint16_t ECO2_DELTA_PPM = 50U;
constexpr uint32_t SENSOR_I2C_CLOCK_HZ = 100000UL;
constexpr uint16_t SENSOR_I2C_TIMEOUT_MS = 50U;
constexpr bool SENSOR_I2C_SCAN_ON_BEGIN = true;
constexpr unsigned long SENSOR_I2C_RECOVER_COOLDOWN_MS = 5000UL;
constexpr unsigned long DHT_WARMUP_MS = 2500UL;
constexpr unsigned long DHT_READ_INTERVAL_MS = 2500UL;
constexpr unsigned long DHT_DIAG_HINT_INTERVAL_MS = 15000UL;
constexpr unsigned long I2C_ENV_WARMUP_MS = 1000UL;
constexpr unsigned long I2C_ENV_READ_INTERVAL_MS = 5000UL;
constexpr unsigned long I2C_DIAG_HINT_INTERVAL_MS = 15000UL;
constexpr unsigned long LIGHT_READ_INTERVAL_MS = 2500UL;
constexpr unsigned long AIR_READ_INTERVAL_MS = 5000UL;
constexpr unsigned long AIR_WARMUP_MS = 120000UL;

// LD2410 nutzt spaeter typischerweise 256000 Baud.
// In diesem Stand reserviert der Wert nur die gemeinsame Presence-Konfiguration;
// ein UART-Parser wird damit noch nicht behauptet.
constexpr unsigned long PRESENCE_UART_BAUD = 256000UL;

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

constexpr uint8_t NET_SEN_ENV_PROVIDER_KIND = NET_SEN_ENV_PROVIDER;
constexpr uint8_t NET_SEN_AIR_PROVIDER_KIND = NET_SEN_AIR_PROVIDER;
constexpr uint8_t NET_SEN_LIGHT_PROVIDER_KIND = NET_SEN_LIGHT_PROVIDER;
constexpr uint8_t NET_SEN_MOTION_PROVIDER_KIND = NET_SEN_MOTION_PROVIDER;

constexpr unsigned long LOOP_INTERVAL_MS = 50UL;
