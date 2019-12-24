/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
This file provides support for cpu tickless-idle.

Tickless-idle eliminates unnecessary timer interrupts when the processor is
otherwise idle.  When the processor exits the idle state, it advances time by
the number of ticks that elapsed during the idle period. For SMP, the tickless
idle is entered when all CPUs are ready.
*/

#include <stdlib.h>

#include "cpu_pwr_lib.h"
#include "cpu_pwr_hal_lib.h"
#include "cpu_tickless.h"
#include "pwrmgmt_debug.h"

/* 100 * 365 * 24 * 3600 * 1000 * 1000 = 0xB342EB7C38000 */
#define TIME_100_YEARS_IN_US 0xB342EB7C38000ULL
#define MAX_SLEEP_TICKS      (((uint64_t)-1) / 1000000)

static uint32_t     tickless_ctate_mask; /* C-states support set */
static cpu_cstate_t c_state_entered;
static int          is_current_tickless = FALSE;

static uint32_t          cStateConfig[RHINO_CONFIG_CPU_NUM];
static uint32_t          cStateLatency[RHINO_CONFIG_CPU_NUM][CPU_CSTATE_MAX + 1];
static one_shot_timer_t *cStateOneShotTimer[CPU_CSTATE_MAX + 1];

/*
 * tickless_live_cpu_set, after tickless module is initialized,
 * all the cores will set bit in tickless_live_cpu_set, each core
 * will clear the relative bit in tickless_live_cpu_set before
 * it is going to enter tickless idle mode, when all the cores
 * clears the bit in tickless_live_cpu_set which means all cores
 * want to enter tickless idle mode, the last core will close
 * the system tick interrupt and start a one-shot plan, and
 * then enter tickless idle mode.
 *
 * On a big-little system, there are some big cores(higher
 * frequency) and some little cores(lower frequency), normally
 * all big cores belong same cluster and all little cores belong
 * same cluster, and these two clusters could has different
 * Cx state, say big cluster could in C2 while little cluster
 * could in C0. For tickless mode, only when all cores want
 * to enter tickless idle mode, the system could enter tickless
 * mode, all cores here means include both big cores and little
 * cores.
 *
 * In the struct cpu_pwr_t, it also has a value named child_live_cpu_set,
 * that is not same thing as tickless_live_cpu_set, different cluster
 * could has different Cx state in one CPU package, child_live_cpu_set
 * in cpu_pwr_t is used to track idle state of each cores under same
 * cluster in this example.
 */

static unsigned int tickless_live_cpu_set = 0;
static kspinlock_t  ticklessSpin;

/* import */
extern void systick_suspend(void);
extern void systick_resume(void);

/* forward declare */
static pwr_status_t tickless_timer_init(void);
static void         tickless_enter(void);
static void         tickless_exit(void);
static void         tickless_enter_check(uint32_t cpu_idx, uint32_t cStatesCfg,
                                         uint64_t *p_sleeptime, cpu_cstate_t *p_cstate_to_set);
static pwr_status_t tickless_one_shot_start(uint64_t sleep_time, cpu_cstate_t c_state_to_enter);
static tick_t       tickless_one_shot_stop(cpu_cstate_t c_state_current);
static void         tickless_announce_n(tick_t n_ticks);
static pwr_status_t tickless_c_state_latency_init(uint32_t cpu_idx, uint32_t *p_latency);

/**
 * This routine is supplied by this module for board/platform,
 * it is responsibility of board/platform to provide information
 * that which C state is supported with tickless mode.
 */
void tickless_c_states_add(uint32_t c_state_set)
{
    tickless_ctate_mask |= c_state_set;
}

/**
 * This routine installs the static routines tickless_enter(),
 * tickless_exit(), into cpu power management facility.
 */
pwr_status_t tickless_init(void)
{
    uint32_t cpu_idx;

    if (tickless_ctate_mask == 0) {
        return PWR_ERR;
    }

    /* initialize the spinlock */
    krhino_spin_lock_init(&ticklessSpin);

    /* initialize the one-shot timers */
    if (tickless_timer_init() != PWR_OK) {
        return PWR_ERR;
    }

    /* set cpu idle mode to run until initialization completes */
    cpu_pwr_idle_mode_set(CPU_IDLE_MODE_RUN);

    for (cpu_idx = 0; cpu_idx < RHINO_CONFIG_CPU_NUM; cpu_idx++) {
        if (cpu_pwr_c_state_capability_get(cpu_idx, &cStateConfig[cpu_idx]) != PWR_OK) {
            return PWR_ERR;
        }

        cStateConfig[cpu_idx] &= tickless_ctate_mask;

        tickless_c_state_latency_init(cpu_idx, cStateLatency[cpu_idx]);

        /* set live flag for all cores */
        tickless_live_cpu_set |= 1 << cpu_idx;
    }

    _func_cpu_tickless_down = tickless_enter;
    _func_cpu_tickless_up   = tickless_exit;

    /*set cpu idle mode to tickless mode now */
    cpu_pwr_idle_mode_set(CPU_IDLE_MODE_TICKLESS);

    return (PWR_OK);
}

static pwr_status_t tickless_timer_init(void)
{
    uint32_t idx;

    for (idx = 0; idx < CPU_CSTATE_MAX; idx++) {
        if (cStateOneShotTimer[idx] != NULL) {
            if (cStateOneShotTimer[idx]->one_shot_init() != PWR_OK) {
                return PWR_ERR;
            }
        }
    }

    return PWR_OK;
}

static pwr_status_t tickless_one_shot_start(uint64_t sleep_time, cpu_cstate_t c_state_to_enter)
{
    if (cStateOneShotTimer[c_state_to_enter]->one_shot_start(sleep_time) != PWR_OK) {
        PWRMGMT_LOG(PWRMGMT_LOG_DBG, "start one shot(%lld us) fail\n", sleep_time);

        return (PWR_ERR);
    }

    return (PWR_OK);
}

static tick_t tickless_one_shot_stop(cpu_cstate_t c_state_current)
{
    tick_t          n_ticks;
    uint64_t        passed_micro_sec;
    uint32_t        us_per_tick = 1000000 / RHINO_CONFIG_TICKS_PER_SECOND;
    static uint64_t us_remain   = 0;

    if (cStateOneShotTimer[c_state_current]->one_shot_stop(&passed_micro_sec) !=
        PWR_OK) {
        PWRMGMT_LOG(PWRMGMT_LOG_ERR, "timerOneShotCancel fail\n");

        return 0;
    }

    n_ticks = (tick_t)((passed_micro_sec * RHINO_CONFIG_TICKS_PER_SECOND) / (1000000));

    /* fixs ticks drift issue */
    us_remain += passed_micro_sec % us_per_tick;
    if (us_remain >= us_per_tick) {
        n_ticks++;
        us_remain -= us_per_tick;
    }

    return n_ticks;
}

/**
 * tickless_c_state_latency_init() sets the latency times for entering and
 * exiting the various C states for idle power management. Latency table
 * is only used when CPU Power Manager is present.
 * @param[in]  cpu_idx      CPU index
 * @param[in]  p_latency[]  Latency array
 * @return  PWR_OK or PWR_ERR when failed.
 */
static pwr_status_t tickless_c_state_latency_init(uint32_t cpu_idx, uint32_t p_latency[])
{
    cpu_cstate_t cstate;
    uint32_t     cstate_all;
    uint32_t     latency;

    if (cpu_pwr_c_state_capability_get(cpu_idx, &cstate_all) != PWR_OK) {
        return (PWR_ERR);
    }

    for (cstate = CPU_CSTATE_C0; cstate <= CPU_CSTATE_MAX; cstate++) {
        p_latency[cstate] = 0;

        if (cstate_all & (1 << cstate)) {
            latency = cpu_pwr_c_state_latency_get(cpu_idx, cstate);

            if (latency == (uint32_t)CPU_LATENCY_UNKNOW) {
                p_latency[cstate] = 0;
            } else {
                p_latency[cstate] = latency;
            }
        }
    }

    return (PWR_OK);
}

/**
 * tickless_enter_check() calculates the amount of time until the next kernel
 * time event, adjusts it based on configuration and CPU capabilities.
 * It also deteremines the CPU idle state (C-state) to enter.
 * @param[in]   cpu_idx          current CPU
 * @param[in]   cstate_cfg,      enabled C states
 * @param[out]  p_sleeptime,     time to next event in us
 * @param[out]  p_cstate_to_set  C state to set
 * @return  N/A
 */
static void tickless_enter_check(uint32_t cpu_idx, uint32_t cstate_cfg,
                                 uint64_t *p_sleeptime, cpu_cstate_t *p_cstate_to_set)
{
    uint32_t cpu_c_state;
    uint64_t sleep_time_us;           /* sleep time in microseconds */
    uint64_t one_shot_max_us_support; /* max one shot time support */
    tick_t   n_ticks;

    n_ticks = krhino_next_sleep_ticks_get();

    if (n_ticks == RHINO_WAIT_FOREVER) {
        sleep_time_us = TIME_100_YEARS_IN_US;
    } else {
        if (n_ticks > MAX_SLEEP_TICKS) {
            k_err_proc(RHINO_SYS_FATAL_ERR);
        }

        sleep_time_us = 1000000ull * n_ticks / RHINO_CONFIG_TICKS_PER_SECOND;

#if (PWRMGMT_CONFIG_MINISLEEP > 0)
        if ((sleep_time_us / 1000) <= cpu_pwr_minisleep_msec_get()) {
            sleep_time_us = cpu_pwr_minisleep_msec_get() * 1000;
        }
#endif
    }

    /* get max valid Cx from cstate_cfg */
    if (cstate_cfg == 0) {
        cpu_c_state = 0;
    } else {
        cpu_c_state = 31 - krhino_find_first_bit(&cstate_cfg);
    }

    while (cpu_c_state > 0) {
        /* check if latency is less than idle time */
        if (cStateLatency[cpu_idx][cpu_c_state] < sleep_time_us) {
            /* hit! save cpu_c_state as the target C state */
            *p_cstate_to_set = (cpu_cstate_t)cpu_c_state;

            /* find the target, break here */
            break;
        }

        /* clear the current Cx from cstate_cfg */
        cstate_cfg &= ~(1 << cpu_c_state);

        /* get max valid Cx from cstate_cfg */
        if (cstate_cfg == 0) {
            cpu_c_state = 0;
        } else {
            cpu_c_state = 31 - krhino_find_first_bit(&cstate_cfg);
        }
    }

    one_shot_max_us_support = 0;

    if ((cStateOneShotTimer[cpu_c_state] != NULL) &&
        (cStateOneShotTimer[cpu_c_state]->one_shot_msec_max != NULL)) {
        one_shot_max_us_support =
          1000 * (uint64_t)cStateOneShotTimer[cpu_c_state]->one_shot_msec_max();
    }

    /* if request sleeptime is longer than support, cut it down
       to max support value. */
    if (sleep_time_us > one_shot_max_us_support) {
        sleep_time_us = one_shot_max_us_support;
    }

    /* real sleep time should be reduced by latency time. */
    sleep_time_us -= cStateLatency[cpu_idx][cpu_c_state];

    /* save the real sleep time into p_sleeptime and return. */
    *p_sleeptime = sleep_time_us;
}

/**
 * tickless_enter() is called when a CPU is going to enter idle state, a one
 * shot interrupt is planned at sametime which is used to wake up CPU.
 * @return N/A
 */
static void tickless_enter(void)
{
    uint64_t     sleep_time;
    cpu_cstate_t cstate_to_enter = CPU_CSTATE_C1;
    uint32_t     cpu_idx         = 0;
    tick_t       n_ticks         = 0;
    cpu_cpsr_t flags_cpsr;

    CPSR_ALLOC();

#if (PWRMGMT_CONFIG_DEBUG > 0)
    static tick_t     last_log_entersleep        = 0;
    static uint32_t   cpu_lowpower_enter_counter = 0;
#endif
    krhino_spin_lock_irq_save(&ticklessSpin, flags_cpsr);

    if (cpu_pwr_ready_status_get() == PWR_ERR) {
        krhino_spin_unlock_irq_restore(&ticklessSpin, flags_cpsr);
        return;
    }

    /* Check if tickless can be entered now */
    tickless_enter_check(cpu_idx, cStateConfig[cpu_idx], &sleep_time,
                         &cstate_to_enter);

    if ((sleep_time > 0) && (is_current_tickless == FALSE)) {
        /*
         * Enable a one shot timer to wake up the system from an idle power
         * management state. This one shot timer will wakeup the system
         * unless another asynchronous event has woken up the CPU already.
         */
        if (tickless_one_shot_start(sleep_time, cstate_to_enter) == PWR_OK) {

#if (PWRMGMT_CONFIG_DEBUG > 0)
            if (krhino_sys_tick_get() > (last_log_entersleep + RHINO_CONFIG_TICKS_PER_SECOND)) {
                last_log_entersleep = krhino_sys_tick_get();
                PWRMGMT_LOG(PWRMGMT_LOG_INFO, "enter sleep %d ms,enter counter %d\r\n",
                            (uint32_t)sleep_time / 1000, cpu_lowpower_enter_counter);
            }
            cpu_lowpower_enter_counter++;
#endif
            is_current_tickless = TRUE;
        }
    }

    c_state_entered = cstate_to_enter;

    if (is_current_tickless == TRUE) {
        /* suspend system tick interrupt */
        systick_suspend();

        /*
         * take CPU into relative C idle state which is decided by
         * tickless_enter_check().
         *
         * NOTE, the CPU interrupt is still locked here, so CPU will be put
         * into Cx state with cpu interrupt locked, but it is safe to wake
         * up CPU throught interrupt.
         *
         * For ARM cortex-M, the processor ignores the value of PRIMASK in
         * determining whether an asynchronous exception is a WFI wakeup event.
         *
         * For ARM cortex-A, when processor is in WFI state, it will be waked
         * up by:
         * a physical IRQ interrupt regardless of the value of the CPSR.I bit,
         * a physical FIQ interrupt regardless of the value of the CPSR.F bit.
         *
         * For Xtensa, WAITI sets the interrupt level in PS.INTLEVEL to imm4
         * and then, on some Xtensa ISA implementations, suspends processor
         * operation until an interrupt occurs. The combination of setting
         * the interrupt level and suspending operation avoids a race condition
         * where an interrupt between the interrupt level setting and the
         * suspension of operation would be ignored until a second interrupt
         * occurred.
         */

        (void)cpu_pwr_c_state_set(cstate_to_enter);
    }

    if (is_current_tickless == TRUE) {

        cpu_pwr_c_state_set(CPU_CSTATE_C0);

        n_ticks = tickless_one_shot_stop(c_state_entered);

        /* resume system tick interrupt */
        systick_resume();

        /* set is_current_tickless to FALSE */
        is_current_tickless = FALSE;

        if (n_ticks > 0) {
        /* announces elapsed ticks to the kernel */
            tickless_announce_n(n_ticks);
        }
    }

    krhino_spin_unlock_irq_restore(&ticklessSpin, flags_cpsr);

    RHINO_CRITICAL_ENTER();
    RHINO_CRITICAL_EXIT_SCHED();
}

/**
 * tickless_exit() is called when a CPU gets interrupted. If it determeines
 * that the system is waking up from tickless idle, it re-enables tick
 * interrupts and stop the one shot interrupt plan, also announces elapsed
 * ticks to the kernel.
 *
 * @return  N/A
 */
static void tickless_exit(void)
{
    tick_t n_ticks = 0;
    cpu_cpsr_t flags_cpsr;

    krhino_spin_lock_irq_save(&ticklessSpin, flags_cpsr);

    if (!is_current_tickless) {
        krhino_spin_unlock_irq_restore(&ticklessSpin, flags_cpsr);
        return;
    }

    n_ticks = tickless_one_shot_stop(c_state_entered);

    /* set is_current_tickless to FALSE */
    is_current_tickless = FALSE;

    /* resume system tick interrupt */
    systick_resume();

    krhino_spin_unlock_irq_restore(&ticklessSpin, flags_cpsr);

    if (n_ticks > 0) {
        /* announces elapsed ticks to the kernel */
        tickless_announce_n(n_ticks);
    }
}

/**
 * tickless_announce_n() is called to announces elapsed ticks to the kernel.
 */
static void tickless_announce_n(tick_t n_ticks)
{
    if (n_ticks > MAX_TIMER_TICKS) {
        k_err_proc(RHINO_INV_PARAM);
    }

    tick_list_update((tick_i_t)n_ticks);
}

/**
 * tickless_one_shot_timer_save() is used to build connection between
 * one shot timer and c state, different c state could has different
 * wake up timer.
 */
void tickless_one_shot_timer_save(cpu_cstate_t cstate, one_shot_timer_t *p_timer)
{
    cStateOneShotTimer[cstate] = p_timer;
}
