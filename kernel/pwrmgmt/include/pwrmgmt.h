/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#ifndef PWRMGMT_H
#define PWRMGMT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <k_api.h>

#include "pwrmgmt_default_config.h"

typedef enum pwrmgmt_user_module {
    PWRMGMT_RHINO   = 0,
    PWRMGMT_OTA     = 1,
    PWRMGMT_NETMGR  = 2,
    PWRMGMT_NETWORK = 3,
    PWRMGMT_PWRMGMT = 4,
    PWRMGMT_APP     = 16,
} PWRMGMT_USER_MODULE;

int pwrmgmt_init();
int pwrmgmt_lowpower_suspend(uint32_t suspend_module);
int pwrmgmt_lowpower_resume(uint32_t resume_module);

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
int pwrmgmt_cpu_lowpower_suspend(uint32_t suspend_module);
int pwrmgmt_cpu_lowpower_resume(uint32_t resume_module);

int      pwrmgmt_cpu_minisleep_msec_set(uint32_t time_ms);
uint32_t pwrmgmt_cpu_minisleep_msec_get(void);
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
int pwrmgmt_wifi_powersave_enable(void);
int pwrmgmt_wifi_powersave_resume(uint32_t resume_module);
int pwrmgmt_wifi_powersave_suspend(uint32_t suspend_module);
#endif

#if ((PWRMGMT_CONFIG_CPU_LOWPOWER > 0) && (PWRMGMT_CONFIG_CPU_ACTIVE > 0))
int pwrmgmt_cpu_active_msec_set(uint32_t active_time);
#endif

#ifdef __cplusplus
}
#endif

#endif /* PWRMGMT_H */
