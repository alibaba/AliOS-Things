/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "aos/pwrmgmt.h"

#include "pwrmgmt_api.h"

int aos_pwrmgmt_lowpower_suspend(uint32_t suspend_module)
{
    pwrmgmt_lowpower_suspend(suspend_module);
    return 0;
}

int aos_pwrmgmt_lowpower_resume(uint32_t resume_module)
{
    pwrmgmt_lowpower_resume(resume_module);
    return 0;
}

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
int aos_pwrmgmt_cpu_lowpower_suspend(uint32_t suspend_module)
{
    pwrmgmt_cpu_lowpower_suspend(suspend_module);
    return 0;
}

int aos_pwrmgmt_cpu_lowpower_resume(uint32_t resume_module)
{
    pwrmgmt_cpu_lowpower_resume(resume_module);
    return 0;
}
#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER > 0 */

#if (WIFI_CONFIG_SUPPORT_LOWPOWER > 0)
int aos_pwrmgmt_wifi_powersave_enable(void)
{
    pwrmgmt_wifi_powersave_enable();
    return 0;
}

int aos_pwrmgmt_wifi_powersave_resume(uint32_t resume_module)
{
    pwrmgmt_wifi_powersave_resume(resume_module);
    return 0;
}

int aos_pwrmgmt_wifi_powersave_suspend(uint32_t suspend_module)
{
    pwrmgmt_wifi_powersave_suspend(suspend_module);
    return 0;
}
#endif /* WIFI_CONFIG_SUPPORT_LOWPOWER > 0 */

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
#if (PWRMGMT_CONFIG_MINISLEEP > 0)
int aos_pwrmgmt_cpu_minisleep_set(uint32_t time_ms)
{
    pwrmgmt_cpu_minisleep_msec_set(time_ms);
    return 0;
}

uint32_t aos_pwrmgmt_cpu_minisleep_get(void)
{
    return pwrmgmt_cpu_minisleep_msec_get();
}
#endif /* PWRMGMT_CONFIG_MINISLEEP > 0 */

#if (PWRMGMT_CONFIG_CPU_ACTIVE > 0)
int aos_pwrmgmt_cpu_active_set(uint32_t active_time)
{
    pwrmgmt_cpu_active_msec_set(active_time);
    return 0;
}
#endif /* PWRMGMT_CONFIG_CPU_ACTIVE > 0 */

#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER > 0 */

