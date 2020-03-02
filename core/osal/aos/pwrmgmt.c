/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "aos/pwrmgmt.h"

#include "pwrmgmt_api.h"

int32_t aos_pwrmgmt_lowpower_suspend(uint32_t suspend_module)
{
    int32_t ret;

    ret = pwrmgmt_lowpower_suspend(suspend_module);
    return ret;
}

int32_t aos_pwrmgmt_lowpower_resume(uint32_t resume_module)
{
    int32_t ret;

    ret = pwrmgmt_lowpower_resume(resume_module);
    return ret;
}

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
int32_t aos_pwrmgmt_cpu_lowpower_suspend(uint32_t suspend_module)
{
    int32_t ret;

    ret = pwrmgmt_cpu_lowpower_suspend(suspend_module);
    return ret;
}

int32_t aos_pwrmgmt_cpu_lowpower_resume(uint32_t resume_module)
{
    int32_t ret;

    ret = pwrmgmt_cpu_lowpower_resume(resume_module);
    return ret;
}
#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER > 0 */

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
int32_t aos_pwrmgmt_wifi_powersave_enable(void)
{
    int32_t ret;

    ret = pwrmgmt_wifi_powersave_enable();
    return ret;
}

int32_t aos_pwrmgmt_wifi_powersave_resume(uint32_t resume_module)
{
    int32_t ret;

    ret = pwrmgmt_wifi_powersave_resume(resume_module);
    return ret;
}

int32_t aos_pwrmgmt_wifi_powersave_suspend(uint32_t suspend_module)
{
    int32_t ret;

    ret = pwrmgmt_wifi_powersave_suspend(suspend_module);
    return ret;
}
#endif /* WIFI_CONFIG_SUPPORT_LOWPOWER > 0 */

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
#if (PWRMGMT_CONFIG_MINISLEEP > 0)
int32_t aos_pwrmgmt_cpu_minisleep_set(uint32_t time_ms)
{
    int32_t ret;

    ret = pwrmgmt_cpu_minisleep_msec_set(time_ms);
    return ret;
}

uint32_t aos_pwrmgmt_cpu_minisleep_get(void)
{
    return pwrmgmt_cpu_minisleep_msec_get();
}
#endif /* PWRMGMT_CONFIG_MINISLEEP > 0 */

#if (PWRMGMT_CONFIG_CPU_ACTIVE > 0)
int32_t aos_pwrmgmt_cpu_active_set(uint32_t active_time)
{
    int32_t ret;

    ret = pwrmgmt_cpu_active_msec_set(active_time);
    return ret;
}
#endif /* PWRMGMT_CONFIG_CPU_ACTIVE > 0 */

#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER > 0 */

