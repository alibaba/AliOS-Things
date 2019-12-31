/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <hal/wifi.h>

#include "pwrmgmt_api.h"
#include "pwrmgmt_debug.h"
#include "cpu_pwr_lib.h"

#define SET_BIT(off) ((uint32_t)1 << (off))

static kmutex_t pwrmgmt_mutex;

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
static uint32_t cpu_suspend_lock = 0;
#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER > 0 */

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
static uint32_t wifi_suspend_lock = 0;
#endif

int32_t pwrmgmt_init(void) {
    kstat_t stat;

    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "pwrmgmt init\r\n");
    stat = krhino_mutex_create(&pwrmgmt_mutex, "pwrmgmt_mutex");
    if (stat != RHINO_SUCCESS) {
        return -1;
    }

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
    cpu_pwrmgmt_init();
#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER > 0 */
    return 0;
}

int32_t pwrmgmt_lowpower_suspend(uint32_t suspend_module)
{
    hal_wifi_module_t *module = NULL;

    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "pwrmgmt suspend lowpower\r\n");

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
    if ((cpu_suspend_lock & SET_BIT(suspend_module)) != 0) {
        PWRMGMT_LOG(PWRMGMT_LOG_WARN, "cpu lowpower is already suspend by module=%x,"
                    "current suspend_lock=%x\r\n", SET_BIT(suspend_module), cpu_suspend_lock);
    }
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
    if ((wifi_suspend_lock & SET_BIT(suspend_module)) != 0) {
        PWRMGMT_LOG(PWRMGMT_LOG_WARN, "wifi lowpower is already suspend by module=%x,"
                    "current suspend_lock=%x\r\n", SET_BIT(suspend_module), wifi_suspend_lock);
    }
#endif

    krhino_mutex_lock(&pwrmgmt_mutex, RHINO_WAIT_FOREVER);

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
    if (cpu_suspend_lock == 0) {
        cpu_pwr_suspend();
    }

    cpu_suspend_lock |= SET_BIT(suspend_module);
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
    if (wifi_suspend_lock == 0) {
        module = hal_wifi_get_default_module();
        hal_wifi_exit_powersave(module);
    }

    wifi_suspend_lock |= SET_BIT(suspend_module);
#endif

    krhino_mutex_unlock(&pwrmgmt_mutex);
    return 0;
}

int32_t pwrmgmt_lowpower_resume(uint32_t resume_module)
{
    hal_wifi_module_t *module = NULL;

    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "pwrmgmt resume lowpower\r\n");

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
    if ((cpu_suspend_lock & SET_BIT(resume_module)) != SET_BIT(resume_module)) {
        PWRMGMT_LOG(PWRMGMT_LOG_WARN, "cpu lowpower is already resumed by module=%x,"
                    "current suspend_lock=%x\r\n", SET_BIT(resume_module), cpu_suspend_lock);
    }
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
    if ((wifi_suspend_lock & SET_BIT(resume_module)) != SET_BIT(resume_module)) {
        PWRMGMT_LOG(PWRMGMT_LOG_WARN, "wifi lowpower is already resumed by module=%x,"
                    "current suspend_lock=%x\r\n", SET_BIT(resume_module), wifi_suspend_lock);
    }
#endif

    krhino_mutex_lock(&pwrmgmt_mutex, RHINO_WAIT_FOREVER);

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
    cpu_suspend_lock &= ~SET_BIT(resume_module);

    if (cpu_suspend_lock == 0) {
        cpu_pwr_resume();
    }
#endif

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
    wifi_suspend_lock &= ~SET_BIT(resume_module);

    if (wifi_suspend_lock == 0) {
        module = hal_wifi_get_default_module();
        hal_wifi_enter_powersave(module, WIFI_CONFIG_RECEIVE_DTIM);
    }
#endif

    krhino_mutex_unlock(&pwrmgmt_mutex);
    return 0;
}

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
int32_t pwrmgmt_cpu_lowpower_suspend(uint32_t suspend_module)
{
    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "pwrmgmt suspend cpu low power\r\n");

    if ((cpu_suspend_lock & SET_BIT(suspend_module)) != 0) {
        PWRMGMT_LOG(PWRMGMT_LOG_WARN, "cpu lowpower is already suspend by module=%x,"
                    "current suspend_lock=%x\r\n", SET_BIT(suspend_module), cpu_suspend_lock);
    }

    krhino_mutex_lock(&pwrmgmt_mutex, RHINO_WAIT_FOREVER);

    if (cpu_suspend_lock == 0) {
        cpu_pwr_suspend();
    }

    cpu_suspend_lock |= SET_BIT(suspend_module);
    krhino_mutex_unlock(&pwrmgmt_mutex);
    return 0;
}

int32_t pwrmgmt_cpu_lowpower_resume(uint32_t resume_module)
{
    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "pwrmgmt resume cpu low power\r\n");

    if ((cpu_suspend_lock & SET_BIT(resume_module)) != SET_BIT(resume_module)) {
        PWRMGMT_LOG(PWRMGMT_LOG_WARN, "cpu lowpower is already resumed by module=%x,"
                    "current suspend_lock=%x\r\n", SET_BIT(resume_module), cpu_suspend_lock);
    }

    krhino_mutex_lock(&pwrmgmt_mutex, RHINO_WAIT_FOREVER);

    cpu_suspend_lock &= ~SET_BIT(resume_module);

    if (cpu_suspend_lock == 0) {
        cpu_pwr_resume();
    }
    krhino_mutex_unlock(&pwrmgmt_mutex);
    return 0;
}
#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER > 0 */

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
int32_t pwrmgmt_wifi_powersave_enable(void)
{
    hal_wifi_module_t *module = NULL;

    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "pwrmgmt wifi lowpower enable\r\n");

    if (wifi_suspend_lock == 0) {
        module = hal_wifi_get_default_module();
        hal_wifi_enter_powersave(module, WIFI_CONFIG_RECEIVE_DTIM);
    } else {
        PWRMGMT_LOG(PWRMGMT_LOG_INFO, "wifi lowpower is suspend\r\n");
    }
    return 0;
}

int32_t pwrmgmt_wifi_powersave_resume(uint32_t resume_module)
{
    hal_wifi_module_t *module = NULL;

    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "pwrmgmt resume wifi power save\r\n");

    if ((wifi_suspend_lock & SET_BIT(resume_module)) != SET_BIT(resume_module)) {
        PWRMGMT_LOG(PWRMGMT_LOG_WARN, "wifi lowpower is already resumed by module=%x,"
                    "current suspend_lock=%x\r\n", SET_BIT(resume_module), wifi_suspend_lock);
    }

    krhino_mutex_lock(&pwrmgmt_mutex, RHINO_WAIT_FOREVER);

    wifi_suspend_lock &= ~SET_BIT(resume_module);

    if (wifi_suspend_lock == 0) {
        module = hal_wifi_get_default_module();
        hal_wifi_enter_powersave(module, WIFI_CONFIG_RECEIVE_DTIM);
    }

    krhino_mutex_unlock(&pwrmgmt_mutex);
    return 0;
}

int32_t pwrmgmt_wifi_powersave_suspend(uint32_t suspend_module)
{
    hal_wifi_module_t *module = NULL;

    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "pwrmgmt suspend wifi power save\r\n");

    if ((wifi_suspend_lock & SET_BIT(suspend_module)) != 0) {
        PWRMGMT_LOG(PWRMGMT_LOG_WARN, "wifi lowpower is already suspend by module=%x,"
                    "current spend_lock=%x\r\n", SET_BIT(suspend_module), wifi_suspend_lock);
    }

    krhino_mutex_lock(&pwrmgmt_mutex, RHINO_WAIT_FOREVER);

    if (wifi_suspend_lock == 0) {
        module = hal_wifi_get_default_module();
        hal_wifi_exit_powersave(module);
    }

    wifi_suspend_lock |= SET_BIT(suspend_module);

    krhino_mutex_unlock(&pwrmgmt_mutex);

    return 0;
}
#endif /* WIFI_CONFIG_SUPPORT_LOWPOWER > 0 */

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
#if (PWRMGMT_CONFIG_MINISLEEP > 0)
int32_t pwrmgmt_cpu_minisleep_msec_set(uint32_t time_ms)
{
    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "set the minimum sleep time %dms\r\n", time_ms);
    cpu_pwr_minisleep_msec_set(time_ms);
    return 0;
}

uint32_t pwrmgmt_cpu_minisleep_msec_get(void)
{
    return cpu_pwr_minisleep_msec_get();
}
#endif /* PWRMGMT_CONFIG_MINISLEEP > 0 */

#if (PWRMGMT_CONFIG_CPU_ACTIVE > 0)
int32_t pwrmgmt_cpu_active_msec_set(uint32_t active_time)
{
    tick_t active_tick = 0;

    PWRMGMT_LOG(PWRMGMT_LOG_INFO, "start to keep cpu be active for %d ms\r\n",
                active_time);

    if (active_time == 0) {
        return 0;
    }

    active_tick = krhino_ms_to_ticks(active_time);

    /* if active time is less than 1 tick */
    if (active_tick == 0) {
        active_tick = 1;
    }

    cpu_active_ticks_set(active_tick);

    return 0;
}
#endif /* PWRMGMT_CONFIG_CPU_ACTIVE > 0 */

#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER > 0 */

