#pragma once

#include "../../include/HardwarePinStandard.h"

#ifndef BAT_SEN_PIN_STATUS_LED
#define BAT_SEN_PIN_STATUS_LED -1
#endif

#ifndef BAT_SEN_PIN_BOOT_BUTTON
#define BAT_SEN_PIN_BOOT_BUTTON SmartHome::HardwarePinStandard::PIN_BOOT_BUTTON
#endif

#ifndef BAT_SEN_PIN_RELAY_1
#define BAT_SEN_PIN_RELAY_1 -1
#endif

#ifndef BAT_SEN_PIN_RELAY_2
#define BAT_SEN_PIN_RELAY_2 -1
#endif

#ifndef BAT_SEN_PIN_SENSOR_SDA
#define BAT_SEN_PIN_SENSOR_SDA SmartHome::HardwarePinStandard::PIN_I2C_SDA
#endif

#ifndef BAT_SEN_PIN_SENSOR_SCL
#define BAT_SEN_PIN_SENSOR_SCL SmartHome::HardwarePinStandard::PIN_I2C_SCL
#endif

#ifndef BAT_SEN_PIN_BUTTON_1
#define BAT_SEN_PIN_BUTTON_1 -1
#endif

#ifndef BAT_SEN_PIN_BUTTON_2
#define BAT_SEN_PIN_BUTTON_2 -1
#endif

#ifndef BAT_SEN_PIN_BUTTON_3
#define BAT_SEN_PIN_BUTTON_3 -1
#endif

#ifndef BAT_SEN_PIN_BUTTON_4
#define BAT_SEN_PIN_BUTTON_4 -1
#endif

#ifndef BAT_SEN_PIN_REED
#define BAT_SEN_PIN_REED -1
#endif

#ifndef BAT_SEN_PIN_RAIN_ADC
#define BAT_SEN_PIN_RAIN_ADC -1
#endif

#ifndef BAT_SEN_PIN_BATTERY_ADC
#define BAT_SEN_PIN_BATTERY_ADC SmartHome::HardwarePinStandard::PIN_BATTERY_ADC
#endif

#ifndef BAT_SEN_PIN_INTERNAL_NEOPIXEL
#define BAT_SEN_PIN_INTERNAL_NEOPIXEL SmartHome::HardwarePinStandard::GPIO_INTERNAL_NEOPIXEL
#endif

// ============================================================
// BAT-SEN - Pinbelegung
// ============================================================
// Batteriemessung ueber festen Spannungsteiler-Eingang.
// Reed-Kontakt und Taster mit INPUT_PULLUP, LOW-aktiv.
// ============================================================
constexpr int PIN_STATUS_LED    = BAT_SEN_PIN_STATUS_LED;
constexpr int PIN_BOOT_BUTTON   = BAT_SEN_PIN_BOOT_BUTTON;
constexpr int PIN_RELAY_1       = BAT_SEN_PIN_RELAY_1; // nicht bestueckt
constexpr int PIN_RELAY_2       = BAT_SEN_PIN_RELAY_2; // nicht bestueckt
constexpr int PIN_SENSOR_SDA    = BAT_SEN_PIN_SENSOR_SDA; // I2C fuer optionale Sensoren
constexpr int PIN_SENSOR_SCL    = BAT_SEN_PIN_SENSOR_SCL;
constexpr int PIN_BUTTON_1      = BAT_SEN_PIN_BUTTON_1; // Taster/Wandschalter Kanal 1
constexpr int PIN_BUTTON_2      = BAT_SEN_PIN_BUTTON_2; // Kanal 2 (optional)
constexpr int PIN_BUTTON_3      = BAT_SEN_PIN_BUTTON_3; // Kanal 3 (optional)
constexpr int PIN_BUTTON_4      = BAT_SEN_PIN_BUTTON_4; // Kanal 4 (optional)
constexpr int PIN_REED          = BAT_SEN_PIN_REED; // Fensterkontakt (Reed-Schalter)
constexpr int PIN_RAIN_ADC      = BAT_SEN_PIN_RAIN_ADC; // Regensensor ADC-Eingang
constexpr int PIN_BATTERY_ADC   = BAT_SEN_PIN_BATTERY_ADC; // Batteriespannungsmessung
constexpr int PIN_INTERNAL_NEOPIXEL = BAT_SEN_PIN_INTERNAL_NEOPIXEL;
