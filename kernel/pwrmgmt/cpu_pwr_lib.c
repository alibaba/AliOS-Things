/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include "cpu_pwr_lib.h"
#include "pwrmgmt_debug.h"

static CPU_IDLE_MODE cpu_pwr_idle_mode    = CPU_IDLE_MODE_RUN;
static int           cpu_pwr_init_flag    = 0;
static int           cpu_pwr_suspend_flag = 0;

static kspinlock_t cpu_pwr_spin;

void (*_func_cpu_tickless_down)(void);
void (*_func_cpu_tickless_up)(void);

/**
 * cpu_pwrmgmt_init() is the entry of whole cpu power manamgement system.
 */
int cpu_pwrmgmt_init(void)
{
    /* avoid reentry */
    if (cpu_pwr_init_flag) {
        return -1;
    }

    krhino_spin_lock_init(&cpu_pwr_spin);

    /* create cpu power management framework and cpu topology structure */
    cpu_pwr_hal_lib_init();

    /* init cpu tickless framework */
    tickless_init();

    /* save the job complete flag */
    cpu_pwr_init_flag = 1;

    return 0;
}

/**
 * cpu_pwr_down() is invoked from idle task when it detects that the kernel
 * can go idle.  cpu_pwr_down() checks the current cpu idle mode and put
 * cpu into lower power mode according different idle mode.
 */
void cpu_pwr_down(void)
{
    if (cpu_pwr_init_flag == 0) {
        return;
    }

    /* This check is not enough. cpu_pwr_is_suspend() must be called to check
     * again before entering low power.
     */
    if (cpu_pwr_suspend_flag == 1) {
        return;
    }

    if ((cpu_pwr_idle_mode == CPU_IDLE_MODE_TICKLESS) &&
        (_func_cpu_tickless_down != NULL)) {
        _func_cpu_tickless_down();
    } else if (cpu_pwr_idle_mode == CPU_IDLE_MODE_SLEEP) {
        (void)cpu_pwr_c_state_set(CPU_CSTATE_C1);
    } else {
        /* CPU_IDLE_MODE_RUN */
        return;
    }
}

/**
 * cpu_pwr_up() is invoked from all interrupt and exception stubs to wakeup
 * the CPU in case it was in lower power mode.  This is necessary so that
 * the kernel can adjust its tick count to reflect the length of time that
 * it was in lower power mode.
 */
void cpu_pwr_up(void)
{
    if (cpu_pwr_init_flag == 0) {
        return;
    }

    (void)cpu_pwr_c_state_set(CPU_CSTATE_C0);

    /* execute tickless up hook if existed */
    if ((cpu_pwr_idle_mode == CPU_IDLE_MODE_TICKLESS) &&
        (_func_cpu_tickless_up != NULL)) {
        _func_cpu_tickless_up();
    }
}

/**
 * cpu_pwr_idle_mode_set() sets the power mode used when CPUs are idle. The
 * following <mode> settings are supported: CPU_IDLE_MODE_RUN     - CPUs are
 * continuously running when idle CPU_IDLE_MODE_SLEEP   - system clock interupts
 * occur at every tick. CPU_IDLE_MODE_TICKLESS- CPUs sleep for multiple tick
 * durations, system tick interupts occur only when kernel event expirations
 *                         occur (timout, delays, etc).
 * @return  PWR_OK, or PWR_ERR in case of failure
 */
pwr_status_t cpu_pwr_idle_mode_set(CPU_IDLE_MODE request_mode)
{
    if (request_mode > CPU_IDLE_MODE_TICKLESS) {
        return (PWR_ERR);
    }

    if ((request_mode == CPU_IDLE_MODE_TICKLESS) &&
        (_func_cpu_tickless_down == NULL)) {
        return (PWR_ERR);
    }

    if (request_mode == cpu_pwr_idle_mode) {
        return (PWR_OK);
    }

    cpu_pwr_idle_mode = request_mode;

    return PWR_OK;
}

/**
 * cpu_pwr_idle_mode_get() returns the current idle CPU power mode
 *
 * @return  current CPU idle mode
 */
CPU_IDLE_MODE cpu_pwr_idle_mode_get(void)
{
    return cpu_pwr_idle_mode;
}

/**
 * cpu_pwr_suspend suspend cpu low power. cpu_pwr_is_suspend() must be called
 * to check cpu_pwr_suspend_flag before entering low power.
 *
 * @return  PWR_OK, or PWR_ERR in case of failure
 */
pwr_status_t cpu_pwr_suspend(void)
{
    cpu_pwr_suspend_flag = 1;
    return PWR_OK;
}

/**
 * cpu_pwr_resume  resume cpu low power. This function does not need critical
 * region protection because the idle task will try again to enter low power.
 *
 * @return  PWR_OK, or PWR_ERR in case of failure
 */
pwr_status_t cpu_pwr_resume(void)
{
    cpu_pwr_suspend_flag = 0;
    return PWR_OK;
}

/**
 * cpu_pwr_is_suspend return the current status of cpu low power suspend
 *
 * @return  1 if cpu power is suspend, or 0 if cpu power is not suspend */
int cpu_pwr_is_suspend(void)
{
    return cpu_pwr_suspend_flag;
}

