#pragma once

// Compile-time provider identifiers for optional module scaffolding.
// These values are intentionally macro-based so PlatformIO build_flags can
// switch providers without patching device-local headers.

// net_sen environment providers
#define SH_NET_SEN_ENV_PROVIDER_NONE          0U
#define SH_NET_SEN_ENV_PROVIDER_DHT22         1U
#define SH_NET_SEN_ENV_PROVIDER_BMP280_STUB   101U
#define SH_NET_SEN_ENV_PROVIDER_BME280_STUB   102U

// net_sen air-quality providers
#define SH_NET_SEN_AIR_PROVIDER_NONE          0U
#define SH_NET_SEN_AIR_PROVIDER_ENS160_STUB   101U

// net_sen motion providers
#define SH_NET_SEN_MOTION_PROVIDER_NONE       0U
#define SH_NET_SEN_MOTION_PROVIDER_PIR_PIN    1U
#define SH_NET_SEN_MOTION_PROVIDER_PIR_STUB   101U

// bat_sen optional input providers
#define SH_BAT_SEN_REED_PROVIDER_NONE         0U
#define SH_BAT_SEN_REED_PROVIDER_PIN          1U
#define SH_BAT_SEN_REED_PROVIDER_STUB         101U

#define SH_BAT_SEN_BUTTON_PROVIDER_NONE       0U
#define SH_BAT_SEN_BUTTON_PROVIDER_PIN        1U
#define SH_BAT_SEN_BUTTON_PROVIDER_STUB       101U

#define SH_BAT_SEN_RAIN_PROVIDER_NONE         0U
#define SH_BAT_SEN_RAIN_PROVIDER_ADC          1U
#define SH_BAT_SEN_RAIN_PROVIDER_STUB         101U
