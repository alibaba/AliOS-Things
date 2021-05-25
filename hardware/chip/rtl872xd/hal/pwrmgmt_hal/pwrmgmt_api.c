#if (AOS_COMP_PWRMGMT > 0)
#include <hal/wifi.h>
#include "pwrmgmt_api.h"
#include <cpu_pwr.h>

#define SET_BIT(off) ((uint32_t)1 << (off))

uint32_t sleep_type = SLEEP_CG; /* 0 is power gate, 1 is clock gate */

static sys_time_t cpu_pwr_active_exit_tick = 0;
static uint32_t cpu_suspend_lock           = 0;

int cpu_pwr_ready_status_get(void)
{
    if (cpu_pwr_is_suspend() == 1) {
        return 0;
    }

    if(krhino_sys_tick_get() < cpu_pwr_active_exit_tick) {
        return 0;
    }

    return 1;
}

int pwrmgmt_cpu_active_msec_set(uint32_t active_time)
{
    tick_t active_tick = 0;
    sys_time_t current_active_exit_tick = 0;

    if (active_time == 0) {
        return 0;
    }

    active_tick = krhino_ms_to_ticks(active_time);

    /* if active time is less than 1 tick */
    if (active_tick == 0) {
        active_tick = 1;
    }

    current_active_exit_tick = krhino_sys_tick_get() + active_tick;
    if (current_active_exit_tick > cpu_pwr_active_exit_tick) {
        cpu_pwr_active_exit_tick = current_active_exit_tick;
    }

    return 0;
}

uint32_t pwrmgmt_sleep_type_set(uint32_t type)
{
    sleep_type = type;
    return 0;
}

int pwrmgmt_wifi_powersave_suspend(uint32_t suspend_module)
{
    hal_wifi_module_t *module = NULL;

    module = hal_wifi_get_default_module();
    hal_wifi_exit_powersave(module);
}

int pwrmgmt_wifi_powersave_resume(uint32_t resume_module)
{
    hal_wifi_module_t *module = NULL;

    module = hal_wifi_get_default_module();
    hal_wifi_enter_powersave(module, WIFI_CONFIG_RECEIVE_DTIM);
}

int pwrmgmt_cpu_lowpower_suspend(uint32_t suspend_module)
{
    if (cpu_suspend_lock == 0) {
        cpu_pwr_suspend();
    }

    cpu_suspend_lock |= SET_BIT(suspend_module);
}

int pwrmgmt_cpu_lowpower_resume(uint32_t resume_module)
{
    cpu_suspend_lock &= ~SET_BIT(resume_module);

    if (cpu_suspend_lock == 0) {
        cpu_pwr_resume();
    }
}

int pwrmgmt_cpu_minisleep_msec_set(uint32_t time_ms)
{
    cpu_pwr_minisleep_msec_set(time_ms);
    return 0;
}

uint32_t pwrmgmt_cpu_minisleep_msec_get(void)
{
    return cpu_pwr_minisleep_msec_get();
}

#endif
