/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_board_config.h"

#define AMP_FS_ROOT_DIR "./test/"

/* js engine low memory enabled */
// #define AMP_LOWMEMORY_ENABLE

/* manager channel */
// #define AMP_NETWORK_ENABLE

/* das(device security) service */
// #define AMP_DAS_ENABLED

/* ntp service */
// #define AMP_NTP_ENABLED

/* location service */
// #define AMP_LOCATION_SERVICE_ENABLED

/* enable high-level jsapi */
#define JSE_HIGHLEVEL_JSAPI

/* debug mode, use test pk and ps*/
#define AMP_DEBUG_MODE

/* os */
#define JSE_CORE_ADDON_BUILDIN
// #define JSE_CORE_ADDON_CHECKSUM
#define JSE_CORE_ADDON_SYSTEM
#define JSE_CORE_ADDON_FS
#define JSE_CORE_ADDON_KV
// #define JSE_CORE_ADDON_PM
// #define JSE_CORE_ADDON_BATTERY
// #define JSE_CORE_ADDON_CHARGER
#define JSE_CORE_ADDON_SYSTIMER
// #define JSE_CORE_ADDON_CRYPTO
#define JSE_CORE_ADDON_INITJS

/* periperal */
#define JSE_HW_ADDON_ADC
// #define JSE_HW_ADDON_CAN
#define JSE_HW_ADDON_DAC
#define JSE_HW_ADDON_GPIO
//#define JSE_HW_ADDON_IR
#define JSE_HW_ADDON_I2C
// #define JSE_HW_ADDON_SPI
#define JSE_HW_ADDON_TIMER
#define JSE_HW_ADDON_PWM
#define JSE_HW_ADDON_ONEWIRE
#define JSE_HW_ADDON_RTC
#define JSE_HW_ADDON_UART
#define JSE_HW_ADDON_WDG
//#define JSE_HW_ADDON_LCD

#define JSE_HW_ADDON_DS18B20

/* network */
// #define JSE_NET_ADDON_UDP
// #define JSE_NET_ADDON_TCP
// #define JSE_NET_ADDON_MQTT
// #define JSE_NET_ADDON_HTTP
// #define JSE_NET_ADDON_WIFI
// #define JSE_NET_ADDON_CELLULAR
// #define JSE_NET_ADDON_NETMGR

/* advanced component */
// #define JSE_ADVANCED_ADDON_AIOT_DEVICE
// #define JSE_ADVANCED_ADDON_AIOT_GATEWAY
// #define JSE_ADVANCED_ADDON_AUDIOPLAYER
// #define JSE_ADVANCED_ADDON_TTS
// #define JSE_ADVANCED_ADDON_LOCATION
// #define JSE_ADVANCED_ADDON_KEYPAD
// #define JSE_ADVANCED_ADDON_UND
// #define JSE_ADVANCED_ADDON_OTA

/* ui component */
#ifdef CONFIG_AMP_UI_SUPPORT
#define JSE_ADVANCED_ADDON_UI
#endif

/* recovery switch & status led */
#define AMP_RECOVERY_ENABLE
#define AMP_REPL_PROMPT     "amp> "

/* manager channel device info for porject */
#define AMP_INTERNAL_PRODUCTKEY_VALUE       ""
#define AMP_INTERNAL_PRODUCTSECRET_VALUE    ""
