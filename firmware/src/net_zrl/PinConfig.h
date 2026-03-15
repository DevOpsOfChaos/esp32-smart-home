#pragma once

#include "../../include/HardwarePinStandard.h"

#ifndef NET_ZRL_PIN_STATUS_LED
#define NET_ZRL_PIN_STATUS_LED -1
#endif

#ifndef NET_ZRL_PIN_BOOT_BUTTON
#define NET_ZRL_PIN_BOOT_BUTTON SmartHome::HardwarePinStandard::PIN_BOOT_BUTTON
#endif

#ifndef NET_ZRL_PIN_BUTTON_1
#define NET_ZRL_PIN_BUTTON_1 -1
#endif

#ifndef NET_ZRL_PIN_BUTTON_2
#define NET_ZRL_PIN_BUTTON_2 -1
#endif

#ifndef NET_ZRL_PIN_BUTTON_3
#define NET_ZRL_PIN_BUTTON_3 -1
#endif

#ifndef NET_ZRL_PIN_RELAY_1
#define NET_ZRL_PIN_RELAY_1 SmartHome::HardwarePinStandard::PIN_RELAY_1
#endif

#ifndef NET_ZRL_PIN_RELAY_2
#define NET_ZRL_PIN_RELAY_2 SmartHome::HardwarePinStandard::PIN_RELAY_2
#endif

#ifndef NET_ZRL_PIN_SENSOR_SDA
#define NET_ZRL_PIN_SENSOR_SDA SmartHome::HardwarePinStandard::PIN_I2C_SDA
#endif

#ifndef NET_ZRL_PIN_SENSOR_SCL
#define NET_ZRL_PIN_SENSOR_SCL SmartHome::HardwarePinStandard::PIN_I2C_SCL
#endif

#ifndef NET_ZRL_PIN_PIR
#define NET_ZRL_PIN_PIR -1
#endif

#ifndef NET_ZRL_PIN_INTERNAL_NEOPIXEL
#define NET_ZRL_PIN_INTERNAL_NEOPIXEL SmartHome::HardwarePinStandard::GPIO_INTERNAL_NEOPIXEL
#endif

#ifndef NET_ZRL_PIN_LED_UP
#define NET_ZRL_PIN_LED_UP -1
#endif

#ifndef NET_ZRL_PIN_LED_DOWN
#define NET_ZRL_PIN_LED_DOWN -1
#endif

// ============================================================
// NET-ZRL - Pinbelegung
// ============================================================

constexpr int PIN_STATUS_LED   = NET_ZRL_PIN_STATUS_LED;
constexpr int PIN_BOOT_BUTTON  = NET_ZRL_PIN_BOOT_BUTTON;
constexpr int PIN_BUTTON_1     = NET_ZRL_PIN_BUTTON_1; // Taster fuer Relais 0 / Rolladen Auf
constexpr int PIN_BUTTON_2     = NET_ZRL_PIN_BUTTON_2; // Taster fuer Relais 1 / Rolladen Ab
constexpr int PIN_BUTTON_3     = NET_ZRL_PIN_BUTTON_3; // Stop-Taster (Rolladen)
constexpr int PIN_RELAY_1      = NET_ZRL_PIN_RELAY_1; // Relais 1 / Kreis 1
constexpr int PIN_RELAY_2      = NET_ZRL_PIN_RELAY_2; // Relais 2 / Kreis 2
constexpr int PIN_SENSOR_SDA   = NET_ZRL_PIN_SENSOR_SDA; // I2C fuer optionalen Sensor
constexpr int PIN_SENSOR_SCL   = NET_ZRL_PIN_SENSOR_SCL;
constexpr int PIN_PIR          = NET_ZRL_PIN_PIR; // optionaler PIR/Radar
constexpr int PIN_INTERNAL_NEOPIXEL = NET_ZRL_PIN_INTERNAL_NEOPIXEL;
constexpr int PIN_LED_UP       = NET_ZRL_PIN_LED_UP;   // Zusatz-LED fuer Auf
constexpr int PIN_LED_DOWN     = NET_ZRL_PIN_LED_DOWN; // Zusatz-LED fuer Ab
