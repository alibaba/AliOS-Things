/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

/* js engine low memory enabled */
// #define AMP_LOWMEMORY_ENABLE

/* enable network */
#define AMP_NETWORK_ENABLE

/* enable kv */
#define AMP_KV_ENABLE

/* enable high-level jsapi */
#define JSE_HIGHLEVEL_JSAPI

/* os */
#define JSE_CORE_ADDON_BUILDIN
#define JSE_CORE_ADDON_SYSTEM
#define JSE_CORE_ADDON_FS
#define JSE_CORE_ADDON_KV
#define JSE_CORE_ADDON_LPM
#define JSE_CORE_ADDON_SYSTIMER
#define JSE_CORE_ADDON_CRYPTO
#define JSE_CORE_ADDON_INITJS

/* periperal */
#define JSE_HW_ADDON_ADC
// #define JSE_HW_ADDON_DAC
// #define JSE_HW_ADDON_CAN
#define JSE_HW_ADDON_GPIO
#define JSE_HW_ADDON_I2C
#define JSE_HW_ADDON_SPI
#define JSE_HW_ADDON_TIMER
#define JSE_HW_ADDON_PWM
#define JSE_HW_ADDON_RTC
#define JSE_HW_ADDON_UART
#define JSE_HW_ADDON_WDG

/* network */
#define JSE_NET_ADDON_UDP
#define JSE_NET_ADDON_TCP
#define JSE_NET_ADDON_MQTT
#define JSE_NET_ADDON_HTTP
#define JSE_NET_ADDON_IOT
#define JSE_NET_ADDON_WIFI
#define JSE_NET_ADDON_CELLULAR

/* recovery switch & status led */
#define AMP_RECOVERY_ENABLE
#define AMP_REC_UART 2
#define AMP_REC_IO 39
#define AMP_REC_IO_ON 1
#define AMP_STATUS_IO 40
#define AMP_STATUS_IO_ON 1

/* repl */
#define AMP_REPL_STDIO_EXTERNAL /* not init stdio in repl */
#define AMP_REPL_PROMPT         "amp> "
#define AMP_REPL_STDIO          2
#define AMP_REPL_STDIO_BANDRATE 115200
#define AMP_REPL_STACK_SIZE     2048

/* advanced component */
#define DEBUG