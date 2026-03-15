#pragma once

#include "../../include/HardwarePinStandard.h"

#ifndef NET_SEN_PIN_STATUS_LED
#define NET_SEN_PIN_STATUS_LED -1
#endif

#ifndef NET_SEN_PIN_BOOT_BUTTON
#define NET_SEN_PIN_BOOT_BUTTON SmartHome::HardwarePinStandard::PIN_BOOT_BUTTON
#endif

#ifndef NET_SEN_PIN_BUTTON_1
#define NET_SEN_PIN_BUTTON_1 -1
#endif

#ifndef NET_SEN_PIN_RELAY_1
#define NET_SEN_PIN_RELAY_1 -1
#endif

#ifndef NET_SEN_PIN_RELAY_2
#define NET_SEN_PIN_RELAY_2 -1
#endif

#ifndef NET_SEN_PIN_SENSOR_SDA
#define NET_SEN_PIN_SENSOR_SDA SmartHome::HardwarePinStandard::PIN_I2C_SDA
#endif

#ifndef NET_SEN_PIN_SENSOR_SCL
#define NET_SEN_PIN_SENSOR_SCL SmartHome::HardwarePinStandard::PIN_I2C_SCL
#endif

#ifndef NET_SEN_PIN_DHT22_DATA
#define NET_SEN_PIN_DHT22_DATA 6
#endif

#ifndef NET_SEN_PIN_PIR
#define NET_SEN_PIN_PIR -1
#endif

#ifndef NET_SEN_PIN_PRESENCE_OUT
#define NET_SEN_PIN_PRESENCE_OUT NET_SEN_PIN_PIR
#endif

#ifndef NET_SEN_PIN_PRESENCE_UART_RX
#define NET_SEN_PIN_PRESENCE_UART_RX -1
#endif

#ifndef NET_SEN_PIN_PRESENCE_UART_TX
#define NET_SEN_PIN_PRESENCE_UART_TX -1
#endif

#ifndef NET_SEN_PIN_INTERNAL_NEOPIXEL
#define NET_SEN_PIN_INTERNAL_NEOPIXEL SmartHome::HardwarePinStandard::GPIO_INTERNAL_NEOPIXEL
#endif

#ifndef NET_SEN_PIN_LED_RING
#define NET_SEN_PIN_LED_RING -1
#endif

#ifndef NET_SEN_LED_RING_COUNT
#define NET_SEN_LED_RING_COUNT 0
#endif

// ============================================================
// NET-SEN - Pinbelegung
// ============================================================
constexpr int PIN_STATUS_LED   = NET_SEN_PIN_STATUS_LED;
constexpr int PIN_BOOT_BUTTON  = NET_SEN_PIN_BOOT_BUTTON;
constexpr int PIN_BUTTON_1     = NET_SEN_PIN_BUTTON_1; // optionaler Reset-Taster
constexpr int PIN_RELAY_1      = NET_SEN_PIN_RELAY_1; // nicht bestueckt
constexpr int PIN_RELAY_2      = NET_SEN_PIN_RELAY_2; // nicht bestueckt
constexpr int PIN_SENSOR_SDA   = NET_SEN_PIN_SENSOR_SDA; // I2C (BME/BMP/SHT/ENS/VEML)
constexpr int PIN_SENSOR_SCL   = NET_SEN_PIN_SENSOR_SCL;
constexpr int PIN_DHT22_DATA   = NET_SEN_PIN_DHT22_DATA;
constexpr int PIN_PIR          = NET_SEN_PIN_PIR;
constexpr int PIN_PRESENCE_OUT = NET_SEN_PIN_PRESENCE_OUT;
constexpr int PIN_PRESENCE_UART_RX = NET_SEN_PIN_PRESENCE_UART_RX;
constexpr int PIN_PRESENCE_UART_TX = NET_SEN_PIN_PRESENCE_UART_TX;
constexpr int PIN_INTERNAL_NEOPIXEL = NET_SEN_PIN_INTERNAL_NEOPIXEL;
constexpr int PIN_LED_RING     = NET_SEN_PIN_LED_RING;
constexpr int LED_RING_COUNT   = NET_SEN_LED_RING_COUNT;
