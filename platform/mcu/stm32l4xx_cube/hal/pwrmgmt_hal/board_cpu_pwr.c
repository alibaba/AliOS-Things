/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
DESCRIPTION
This library provides the support for the STM32L496G-DISCOVERY
CPU power state control.

CPU power management:
provides low-level interface for setting CPU C-states.
provides low-level interface for setting CPU P-states.
*/

#include <k_api.h>

#if (AOS_COMP_PWRMGMT > 0)

#include "cpu_pwr_hal_lib.h"
#include "pwrmgmt_debug.h"
#include "cpu_tickless.h"
#include "stm32l4xx_hal.h"

/* forward declarations */
extern one_shot_timer_t tim5_one_shot; /* wakeup source for C1,C2 */
extern one_shot_timer_t rtc_one_shot;  /* wakeup source for C3,C4 */

static cpu_pwr_t cpu_pwr_node_core_0;

/*
For verifying the RTC could wakeup system in C4 mode, you can define
C4_WAKE_UP_BY_RTC_EXAMPLE here
*/

#undef C4_WAKE_UP_BY_RTC_EXAMPLE

/**
 * board_cpu_c_state_set - program CPU into Cx idle state
 *
 * RUN Context: could be called from ISR context or task context.
 *
 * SMP Consider: STM32L496G-DISCOVERY do not support SMP, so only UP is enough.
 *
 * @return  PWR_OK or PWR_ERR when failed.
 */
static pwr_status_t board_cpu_c_state_set(uint32_t cpuCState, int master)
{
    switch (cpuCState) {
        case CPU_CSTATE_C0:

            if (master) {
                /*
                 * do something needed when CPU waked up from C1 or higher
                 * Cx state.
                 */
            }

            break;
        case CPU_CSTATE_C1:

            /* put CPU into C1 state, for ARM we can call WFI instruction
               to put CPU into C1 state. */
            PWRMGMT_LOG(PWRMGMT_LOG_DBG, "enter C1\n");
            HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

            break;
        case CPU_CSTATE_C2:

            /* put CPU into C2 state, normally different SOC has different
               way for C2. */
            PWRMGMT_LOG(PWRMGMT_LOG_DBG, "enter C2\n");

            HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);

            break;
        case CPU_CSTATE_C3:

            /* put CPU into C3 state, normally different SOC has different
               way for C3. */
            PWRMGMT_LOG(PWRMGMT_LOG_DBG, "enter C3\n");
            HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

            break;
        case CPU_CSTATE_C4:

            /* put CPU into C4 state, normally different SOC has different
               way for C4. */
            PWRMGMT_LOG(PWRMGMT_LOG_DBG, "enter C4\n");
            HAL_PWR_EnterSTANDBYMode();

            break;
        default:

            PWRMGMT_LOG(PWRMGMT_LOG_ERR, "invalid C state: C%d\n", cpuCState);
            break;
    }

    return PWR_OK;
}

/**
 * board_cpu_pwr_init() is called by HAL lib to
 * init board powr manage configure.
 *
 * RUN Context: could be called from task context only, ISR context is not
 * supported.
 *
 * SMP Consider: STM32L496G-DISCOVERY do not support SMP, so only UP is enough.
 *
 * @return  PWR_OK or PWR_ERR when failed.
 */
pwr_status_t board_cpu_pwr_init(void)
{
    cpu_pwr_t    *pCpuNode  = NULL;
    pwr_status_t  retVal    = PWR_OK;
    uint32_t      cpuIndex  = 0;    /* 0 for UP */

    pCpuNode = &cpu_pwr_node_core_0;
    retVal = cpu_pwr_node_init_static("core", 0, pCpuNode);
    if (retVal != PWR_OK) {
        return PWR_ERR;
    }

    /* record this node */
    retVal = cpu_pwr_node_record(pCpuNode, cpuIndex);
    if (retVal == PWR_ERR) {
        return PWR_ERR;
    }

    /*
     * According reference manual of STM32L496G-DISCOVERY
     *
     * C0 - RUN,  Power supplies are on,all clocks are on.
     * C1 - Sleep mode, CPU clock off, all peripherals including
     *      Cortex®-M4 core peripherals such as NVIC, SysTick, etc. can run
     *      and wake up the CPU when an interrupt or an event occurs.
     * C2 - low power sleep mode, Low-power sleep mode: This mode is entered
     *      from the Low-power run mode: Cortex®-M4 is off.
     * C3 - stop mode.
     * C4 - standby mode.
     *
     * Currently, C0, C1, C2, C3, C4 is supported,
     */

    /* C0,C1,C2,C3,C4 is controlled by level 2 : core */
    retVal = cpu_pwr_c_method_set(cpuIndex, board_cpu_c_state_set);
    if (retVal == PWR_ERR) {
        return PWR_ERR;
    }

    /* save support C status bitset in level 2: C0,C1,C2,C3,C4 */
    cpu_pwr_c_state_capability_set(cpuIndex,CPU_STATE_BIT(CPU_CSTATE_C0)
                                                     | CPU_STATE_BIT(CPU_CSTATE_C1)
                                                     | CPU_STATE_BIT(CPU_CSTATE_C2)
                                                     | CPU_STATE_BIT(CPU_CSTATE_C3)
                                                     | CPU_STATE_BIT(CPU_CSTATE_C4));
    if (retVal == PWR_ERR) {
        return PWR_ERR;
    }

    /*
     * According reference manual of STM32L496G-DISCOVERY,
     * the wakeup latency of Cx is:
     * resume from C1 (Low Power mode)       : immediate
     * resume from C2 (Low Power Sleep mode) : immediate
     * resume from C3 (stop mode)            : immediate
     */
    cpu_pwr_c_state_latency_save(cpuIndex, CPU_CSTATE_C0, 0);
    cpu_pwr_c_state_latency_save(cpuIndex, CPU_CSTATE_C1, 0);
    cpu_pwr_c_state_latency_save(cpuIndex, CPU_CSTATE_C2, 0);
    cpu_pwr_c_state_latency_save(cpuIndex, CPU_CSTATE_C3, 0);
    cpu_pwr_c_state_latency_save(cpuIndex, CPU_CSTATE_C4, 0);

    /*
    Although C1/C2/C3/C4 is supported with CPU PWR MGMT, that is not
    meaning all of these Cx is suitable with tickless. For example,
    if we config C4(CPU of STM32L496G enter standby mode which is
    defined in reference maunal, the standby mode is something similar
    power off the board) with tickless mode, when system has nothing
    to do and decide enter tickless mode with one second
    (such as the shortest sleep time of task) sleep time with C4,
    the RTC is programmed to fire interrupt at one second, and then
    CPU goto C4, and CPU goto standby mode which is something similar
    power off the board, and with one second passed, the RTC fired
    interrupt to wake up CPU, the CPU will start with first instruction
    not the WFI, this lead system reboot in one second and find it
    could goto C4 again and reboot in one second again and again.

    With tickless mode, there is no need for user to decide when and
    which Cx state is selected to enter tickless mode, the tickless
    module will decide itself, C1/C2/C3 is suitable for tickless here.

    But for C4-standby mode(not all standby mode of all SOC has same
    meaning) here, it's better for user to decide when to put CPU
    into C4 state, and it's responsibility of user to prepare/programm
    the wakeup source before CPU enter C4 mode, normally the wakeup
    source could be RTC, sensor and anything, please reference your
    SOC user manual to check the detail which resource would be used.

    The RTC here for C4 is for example showing how to wakeup CPU in
    C4, you could open it for testing.
    */
    tickless_one_shot_timer_save(CPU_CSTATE_C1, &tim5_one_shot);
    tickless_one_shot_timer_save(CPU_CSTATE_C2, &tim5_one_shot);
    tickless_one_shot_timer_save(CPU_CSTATE_C3, &rtc_one_shot);

    /*
    it could be opened for showing how to wakeup CPU
    in C4(standby mode) by RTC.
    */

#ifdef C4_WAKE_UP_BY_RTC_EXAMPLE
    tickless_one_shot_timer_save(CPU_CSTATE_C4, &rtc_one_shot);
#endif /* C4_WAKE_UP_BY_RTC_EXAMPLE */

    /*
    Tell the CPU PWR MGMT module which C state is supported with
    tickless function through tickless_c_states_add(c_state_x).

    NOTE, although C4 is support by this board, but C4 is not suitable
    to work together with tickless mode, because C4 of STM32L496G means
    standby which is something similar with shutdown mode, when system
    enter C4 mode, the board need external wakeup source to trigger
    to reboot.
    */

    tickless_c_states_add(CPU_STATE_BIT(CPU_CSTATE_C0)
                          | CPU_STATE_BIT(CPU_CSTATE_C1)
                          | CPU_STATE_BIT(CPU_CSTATE_C2)
                          | CPU_STATE_BIT(CPU_CSTATE_C3)
#ifdef C4_WAKE_UP_BY_RTC_EXAMPLE
                          | CPU_STATE_BIT(CPU_CSTATE_C4)
#endif /* C4_WAKE_UP_BY_RTC_EXAMPLE */
                         );

    /*
    after init, we will show a cpu power capabilites like this:

              CPU NODE                      CAPABILITIES
     ------------------------------  --------------------------
             core0 (CPU0)

                                     P-States:
                                       P0:   80 MHz   1200 mv
                                     C-States:
                                       C0:    0 us latency
                                       C1:    0 us latency
                                       C2:    0 us latency
                                       C3:    0 us latency
    */
#if RHINO_CONFIG_CPU_PWR_SHOW
    cpu_pwr_info_show();
    cpu_pwr_state_show();
#endif

    return retVal;
}

#endif /* AOS_COMP_PWRMGMT */
