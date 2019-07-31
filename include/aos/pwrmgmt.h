/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef PWRMGMT_H
#define PWRMGMT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "k_api.h"

#include "pwrmgmt_default_config.h"

typedef enum pwrmgmt_user_module {
    PWRMGMT_RHINO   = 0,
    PWRMGMT_OTA     = 1,
    PWRMGMT_NETMGR  = 2,
    PWRMGMT_NETWORK = 3,
    PWRMGMT_PWRMGMT = 4,
    PWRMGMT_AT      = 5,
    PWRMGMT_APP     = 16,
} PWRMGMT_USER_MODULE;

int aos_pwrmgmt_lowpower_suspend(uint32_t suspend_module);
int aos_pwrmgmt_lowpower_resume(uint32_t resume_module);

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
int aos_pwrmgmt_cpu_lowpower_suspend(uint32_t suspend_module);
int aos_pwrmgmt_cpu_lowpower_resume(uint32_t resume_module);

#if (PWRMGMT_CONFIG_MINISLEEP > 0)
int      aos_pwrmgmt_cpu_minisleep_set(uint32_t time_ms);
uint32_t aos_pwrmgmt_cpu_minisleep_get(void);
#endif

#if (PWRMGMT_CONFIG_CPU_ACTIVE > 0)
int aos_pwrmgmt_cpu_active_set(uint32_t active_time);
#endif
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
int aos_pwrmgmt_wifi_powersave_enable(void);
int aos_pwrmgmt_wifi_powersave_resume(uint32_t resume_module);
int aos_pwrmgmt_wifi_powersave_suspend(uint32_t suspend_module);
#endif

#ifdef __cplusplus
}
#endif

#endif /* PWRMGMT_H */

