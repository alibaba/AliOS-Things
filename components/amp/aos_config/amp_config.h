/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_board_config.h"

#define AMP_FS_ROOT_DIR "/data/jsamp"

#define JSE_CORE_ADDON_INITJS

/* enable high-level jsapi */
#define JSE_HIGHLEVEL_JSAPI

/* js engine low memory enabled */
// #define AMP_LOWMEMORY_ENABLE

/* manager channel */
#define AMP_NETWORK_ENABLE

/* das(device security) service */
// #define AMP_DAS_ENABLED

/* ntp service */
// #define AMP_NTP_ENABLED

/* location service */
// #define AMP_LOCATION_SERVICE_ENABLED

/* debug mode, use test pk and ps*/
#define AMP_DEBUG_MODE

/* ota service */
#if AMP_ADVANCED_ADDON_OTA
#define JSE_ADVANCED_ADDON_OTA
#endif

/* os */
#if AMP_CORE_ADDON_BUILDIN
#define JSE_CORE_ADDON_BUILDIN
#endif

#if AMP_CORE_ADDON_CHECKSUM
#define JSE_CORE_ADDON_CHECKSUM
#endif

#if AMP_CORE_ADDON_SYSTEM
#define JSE_CORE_ADDON_SYSTEM
#endif

#if AMP_CORE_ADDON_FS
#define JSE_CORE_ADDON_FS
#endif

#if AMP_CORE_ADDON_KV
#define JSE_CORE_ADDON_KV
#endif

#if AMP_CORE_ADDON_LOG
#define JSE_CORE_ADDON_LOG
#endif

#if AMP_CORE_ADDON_PM
#define JSE_CORE_ADDON_PM
#endif

#if AMP_CORE_ADDON_BATTERY
#define JSE_CORE_ADDON_BATTERY
#endif

#if AMP_CORE_ADDON_CHARGER
#define JSE_CORE_ADDON_CHARGER
#endif

#if AMP_CORE_ADDON_SYSTIMER
#define JSE_CORE_ADDON_SYSTIMER
#endif

#if AMP_CORE_ADDON_CRYPTO
#define JSE_CORE_ADDON_CRYPTO
#endif

/* periperal */
#if AMP_HW_ADDON_ADC
#define JSE_HW_ADDON_ADC
#endif

#if AMP_HW_ADDON_CAN
#define JSE_HW_ADDON_CAN
#endif

#if AMP_HW_ADDON_DAC
#define JSE_HW_ADDON_DAC
#endif

#if AMP_HW_ADDON_GPIO
#define JSE_HW_ADDON_GPIO
#endif

#if AMP_HW_ADDON_I2C
#define JSE_HW_ADDON_I2C
#endif

#if AMP_HW_ADDON_SPI
#define JSE_HW_ADDON_SPI
#endif

#if AMP_HW_ADDON_TIMER
#define JSE_HW_ADDON_TIMER
#endif

#if AMP_HW_ADDON_PWM
#define JSE_HW_ADDON_PWM
#endif

#if AMP_HW_ADDON_RTC
#define JSE_HW_ADDON_RTC
#endif

#if AMP_HW_ADDON_UART
#define JSE_HW_ADDON_UART
#endif

#if AMP_HW_ADDON_LCD
#define JSE_HW_ADDON_LCD
#endif

#if AMP_HW_ADDON_WDG
#define JSE_HW_ADDON_WDG
#endif

/* network */

#if AMP_NET_ADDON_UDP
#define JSE_NET_ADDON_UDP
#endif

#if AMP_NET_ADDON_TCP
#define JSE_NET_ADDON_TCP
#endif

#if AMP_NET_ADDON_MQTT
#define JSE_NET_ADDON_MQTT
#endif

#if AMP_NET_ADDON_HTTP
#define JSE_NET_ADDON_HTTP
#endif

#if AMP_NET_ADDON_NETMGR
#define JSE_NET_ADDON_NETMGR
#endif

#if AMP_NET_ADDON_WIFI
#define JSE_NET_ADDON_WIFI
#endif

#if AMP_NET_ADDON_CELLULAR
#define JSE_NET_ADDON_CELLULAR
#endif

/* advanced component */
#if AMP_ADVANCED_ADDON_AIOT_DEVICE
#define JSE_ADVANCED_ADDON_AIOT_DEVICE
#endif

#if AMP_ADVANCED_ADDON_AIOT_GATEWAY
#define JSE_ADVANCED_ADDON_AIOT_GATEWAY
#endif

#if AMP_ADVANCED_ADDON_AUDIOPLAYER
#define JSE_ADVANCED_ADDON_AUDIOPLAYER
#endif

#if AMP_ADVANCED_ADDON_TTS
#define JSE_ADVANCED_ADDON_TTS
#endif

#if AMP_ADVANCED_ADDON_LOCATION
#define JSE_ADVANCED_ADDON_LOCATION
#endif

#if AMP_ADVANCED_ADDON_KEYPAD
#define JSE_ADVANCED_ADDON_KEYPAD
#endif

#if AMP_ADVANCED_ADDON_UND
#define JSE_ADVANCED_ADDON_UND
#endif

/* wireless */
#if AMP_WIRELESS_ADDON_BT_HOST
#define JSE_WIRELESS_ADDON_BT_HOST
#endif

/* utils */
#if AMP_UTILS_ADDON_CHECKSUM
#define JSE_UTILS_ADDON_CHECKSUM
#endif

#if AMP_ADVANCED_ADDON_BLECFGNET
#define JSE_ADVANCED_ADDON_BLECFGNET
#endif

#if AMP_ADVANCED_ADDON_UI
#define JSE_ADVANCED_ADDON_UI
#endif

/* recovery switch & status led */
#define AMP_REPL_PROMPT     "amp > "

/* manager channel device info for porject */
#define AMP_INTERNAL_PRODUCTKEY_VALUE       ""
#define AMP_INTERNAL_PRODUCTSECRET_VALUE    ""
