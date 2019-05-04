/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include "pwrmgmt.h"
#include "cpu_pwr.h"
#include "pwrmgmt_debug.h"

#define SET_BIT(off) ((uint32_t)1 << (off))

static kmutex_t pwrmgmt_mutex;
static uint32_t suspend_lock = 0;

int pwrmgmt_init() {
    PWR_DBG(DBG_INFO, "pwrmgmt init\r\n");
    krhino_mutex_create(&pwrmgmt_mutex, "pwrmgmt_mutex");

#if (RHINO_CONFIG_PWRMGMT > 0)
    cpu_pwrmgmt_init();
#endif
    return 0;
}

int pwrmgmt_lowpower_suspend(uint32_t suspend_module)
{
    krhino_mutex_lock(&pwrmgmt_mutex, RHINO_WAIT_FOREVER);

    if ((suspend_lock & SET_BIT(suspend_module)) != 0) {
        PWR_DBG(DBG_ERR, "suspend low power error suspend_lock=%x, suspend_module=%x\r\n", suspend_lock, SET_BIT(suspend_module));
    }

    if (suspend_lock == 0) {
#if (RHINO_CONFIG_PWRMGMT > 0)
        cpu_pwr_suspend();
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
        wifi_powersave_exit();
#endif
    }

    suspend_lock |= SET_BIT(suspend_module);

    krhino_mutex_unlock(&pwrmgmt_mutex);
    return 0;
}

int pwrmgmt_lowpower_resume(uint32_t resume_module)
{
    krhino_mutex_lock(&pwrmgmt_mutex, RHINO_WAIT_FOREVER);

    if ((suspend_lock & SET_BIT(resume_module)) != SET_BIT(resume_module)) {
        PWR_DBG(DBG_ERR, "resume low power error suspend_lock=%x, reusme_module=%x\r\n", suspend_lock, SET_BIT(resume_module));
    }

    suspend_lock &= ~SET_BIT(resume_module);

    if (suspend_lock == 0) {
#if (RHINO_CONFIG_PWRMGMT > 0)
        cpu_pwr_resume();
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
        wifi_powersave_enter();
#endif
    }

    krhino_mutex_unlock(&pwrmgmt_mutex);
    return 0;
}

