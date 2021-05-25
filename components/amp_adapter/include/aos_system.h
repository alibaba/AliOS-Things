/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AMP_SYSTEM_H
#define AMP_SYSTEM_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "stdarg.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"

/* log system operation wrapper */
#define amp_debug(mod, fmt, ...)     LOGD(mod, fmt, ##__VA_ARGS__)
#define amp_info(mod, fmt, ...)      LOGI(mod, fmt, ##__VA_ARGS__)
#define amp_warn(mod, fmt, ...)      LOGW(mod, fmt, ##__VA_ARGS__)
#define amp_error(mod, fmt, ...)     LOGE(mod, fmt, ##__VA_ARGS__)
#define amp_fatal(mod, fmt, ...)     LOGF(mod, fmt, ##__VA_ARGS__)

typedef struct amp_heap_info {
    unsigned long heap_total; /* total heap memory */
    unsigned long heap_used;  /* used heap memory */
    unsigned long heap_free;  /* free heap memory */
} amp_heap_info_t;

typedef struct _amp_wireless_info_t {
    int rssi; /*  Received Signal Strength Indication */
    int snr;  /*  Signal to Noise Ratio */
    int per;  /*  Packet Error Rate (Unit: PPM, Part Per Million) */
} aos_wireless_info_t;

void aos_printf(const char *fmt, ...);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int aos_snprintf(char *str, const int len, const char *fmt, ...);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int aos_vsnprintf(char *str, const int len, const char *format, va_list ap);

/**
 * Get system version.
 *
 * @return  sysinfo_version.
 */
const char *aos_get_system_version(void);

/**
 * Get system platform type.
 *
 * @return  platform_type.
 */
const char *aos_get_platform_type(void);

/**
 * Get device name.
 *
 * @return  device_name.
 */
const char *aos_get_device_name(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int aos_system_sleep(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int aos_get_wireless_info(struct _amp_wireless_info_t *info);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int aos_get_network_status(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int aos_get_ip(char *ip);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int aos_get_mac_addr(unsigned char mac[6]);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int amp_heap_memory_info(amp_heap_info_t *heap_info);

/**
 * Initialize system
 */
int aos_system_init(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
const char *aos_get_module_hardware_version(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
const char *aos_get_module_software_version(void);

#if defined(__cplusplus)
}
#endif

#endif /* AMP_SYSTEM_H */
