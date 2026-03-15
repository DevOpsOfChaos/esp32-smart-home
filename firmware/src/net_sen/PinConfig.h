#pragma once

#include "../../include/HardwarePinStandard.h"
// ============================================================
// NET-SEN – Pinbelegung
// ============================================================
constexpr int PIN_STATUS_LED   = -1;
constexpr int PIN_BOOT_BUTTON  = SmartHome::HardwarePinStandard::PIN_BOOT_BUTTON;
constexpr int PIN_BUTTON_1     = -1; // optionaler Reset-Taster
constexpr int PIN_RELAY_1      = -1; // nicht bestückt
constexpr int PIN_RELAY_2      = -1; // nicht bestückt
constexpr int PIN_SENSOR_SDA   = SmartHome::HardwarePinStandard::PIN_I2C_SDA; // I2C (AHT/ENS/VEML)
constexpr int PIN_SENSOR_SCL   = SmartHome::HardwarePinStandard::PIN_I2C_SCL;
constexpr int PIN_DHT22_DATA   = 6;  // realer Minimalpfad fuer net_sen_01
constexpr int PIN_PIR          = -1; // PIR oder Radar
constexpr int PIN_PRESENCE_OUT = PIN_PIR;
constexpr int PIN_PRESENCE_UART_RX = -1;
constexpr int PIN_PRESENCE_UART_TX = -1;
constexpr int PIN_INTERNAL_NEOPIXEL = SmartHome::HardwarePinStandard::GPIO_INTERNAL_NEOPIXEL;
constexpr int PIN_LED_RING     = -1; // WS2812-Ring (optional)
constexpr int LED_RING_COUNT   = 0;
