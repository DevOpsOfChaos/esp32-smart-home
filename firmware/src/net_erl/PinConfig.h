#pragma once

#include "../../include/HardwarePinStandard.h"

#ifndef NET_ERL_PIN_STATUS_LED
#define NET_ERL_PIN_STATUS_LED -1
#endif

#ifndef NET_ERL_PIN_BOOT_BUTTON
#define NET_ERL_PIN_BOOT_BUTTON SmartHome::HardwarePinStandard::PIN_BOOT_BUTTON
#endif

#ifndef NET_ERL_PIN_BUTTON_1
#define NET_ERL_PIN_BUTTON_1 -1
#endif

#ifndef NET_ERL_PIN_RELAY_1
#define NET_ERL_PIN_RELAY_1 SmartHome::HardwarePinStandard::PIN_RELAY_1
#endif

#ifndef NET_ERL_PIN_RELAY_2
#define NET_ERL_PIN_RELAY_2 -1
#endif

#ifndef NET_ERL_PIN_SENSOR_SDA
#define NET_ERL_PIN_SENSOR_SDA SmartHome::HardwarePinStandard::PIN_I2C_SDA
#endif

#ifndef NET_ERL_PIN_SENSOR_SCL
#define NET_ERL_PIN_SENSOR_SCL SmartHome::HardwarePinStandard::PIN_I2C_SCL
#endif

#ifndef NET_ERL_PIN_PIR
#define NET_ERL_PIN_PIR -1
#endif

#ifndef NET_ERL_PIN_INTERNAL_NEOPIXEL
#define NET_ERL_PIN_INTERNAL_NEOPIXEL SmartHome::HardwarePinStandard::GPIO_INTERNAL_NEOPIXEL
#endif

#ifndef NET_ERL_PIN_LED_RING
#define NET_ERL_PIN_LED_RING -1
#endif

#ifndef NET_ERL_LED_RING_COUNT
#define NET_ERL_LED_RING_COUNT 0
#endif

// ============================================================
// NET-ERL - Pinbelegung
// ============================================================
// NET-ERL folgt dem verbindlichen Hardwarestandard der Platine.
// Nur optional nicht bestueckte Funktionen bleiben auf -1.
// ============================================================

// Separate Status-LED aktuell nicht bestueckt.
constexpr int PIN_STATUS_LED   = NET_ERL_PIN_STATUS_LED;

// Boot-/Setup-Taster des ESP32-C3-Boards.
constexpr int PIN_BOOT_BUTTON  = NET_ERL_PIN_BOOT_BUTTON;

// Lokaler Taster (INPUT_PULLUP, LOW bei Betaetigung).
constexpr int PIN_BUTTON_1     = NET_ERL_PIN_BUTTON_1;

// Relais 1 der Relais-Platine.
constexpr int PIN_RELAY_1      = NET_ERL_PIN_RELAY_1;

// Zweites Relais - bei NET-ERL nicht bestueckt.
constexpr int PIN_RELAY_2      = NET_ERL_PIN_RELAY_2;

// I2C Bus fuer optionale Sensoren.
constexpr int PIN_SENSOR_SDA   = NET_ERL_PIN_SENSOR_SDA;
constexpr int PIN_SENSOR_SCL   = NET_ERL_PIN_SENSOR_SCL;

// PIR-Sensor Eingang (optional, HIGH bei Bewegung).
constexpr int PIN_PIR          = NET_ERL_PIN_PIR;

// Interner Board-NeoPixel.
constexpr int PIN_INTERNAL_NEOPIXEL = NET_ERL_PIN_INTERNAL_NEOPIXEL;

// WS2812-Ring Datenleitung (optional, fuer Kuechenlampe).
constexpr int PIN_LED_RING     = NET_ERL_PIN_LED_RING;

// Anzahl WS2812-Pixel im Ring (0 = kein Ring bestueckt).
constexpr int LED_RING_COUNT   = NET_ERL_LED_RING_COUNT;
