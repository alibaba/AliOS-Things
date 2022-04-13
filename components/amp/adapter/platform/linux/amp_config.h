/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

/* js engine low memory enabled */
// #define AMP_LOWMEMORY_ENABLE

/* enable network */
#define AMP_NETWORK_ENABLE

/* enable high-level jsapi */
#define JSE_HIGHLEVEL_JSAPI

/* os */
#define JSE_CORE_ADDON_BUILDIN
#define JSE_CORE_ADDON_SYSTEM
#define JSE_CORE_ADDON_FS
#define JSE_CORE_ADDON_KV
// #define JSE_CORE_ADDON_LPM
// #define JSE_CORE_ADDON_WAKELOCK
#define JSE_CORE_ADDON_SYSTIMER
#define JSE_CORE_ADDON_CRYPTO
#define JSE_CORE_ADDON_INITJS

/* periperal */
// #define JSE_HW_ADDON_ADC
// #define JSE_HW_ADDON_DAC
// #define JSE_HW_ADDON_CAN
// #define JSE_HW_ADDON_GPIO
// #define JSE_HW_ADDON_I2C
// #define JSE_HW_ADDON_SPI
// #define JSE_HW_ADDON_TIMER
// #define JSE_HW_ADDON_PWM
// #define JSE_HW_ADDON_RTC
#define JSE_HW_ADDON_UART
// #define JSE_HW_ADDON_WDG

/* network */
#define JSE_NET_ADDON_UDP
#define JSE_NET_ADDON_TCP
#define JSE_NET_ADDON_MQTT
#define JSE_NET_ADDON_HTTP
#define JSE_NET_ADDON_IOT
// #define JSE_NET_ADDON_CELLULAR
// #define JSE_NET_ADDON_WIFI

/* advanced */
#define JSE_ADVANCED_ADDON_AUDIO
#define JSE_ADVANCED_ADDON_TTS
#define JSE_ADVANCED_ADDON_UND

/* recovery switch & status led */
#define AMP_RECOVERY_ENABLE
#define AMP_REC_UART 2
#define AMP_REC_IO 11
#define AMP_REC_IO_ON 1
#define AMP_STATUS_IO 33
#define AMP_STATUS_IO_ON 1

/* repl */
// #define AMP_REPL_STDIO_EXTERNAL /* external stdio in repl. when define, console.log & JS Error will redirect to AMP_REPL_STDIO */
#define AMP_REPL_PROMPT         "\033[32mamp> \033[39m"
#define AMP_REPL_STDIO          0
#define AMP_REPL_STDIO_BANDRATE 115200