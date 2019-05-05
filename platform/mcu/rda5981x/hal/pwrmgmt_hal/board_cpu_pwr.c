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

#if RHINO_CONFIG_CPU_PWR_MGMT

#include <pwrmgmt_api.h>
#include <cpu_pwr_hal_lib.h>
#include <pwrmgmt_debug.h>
#include <cpu_tickless.h>
#include "rda_def.h"
#include "rda5981.h"


/* forward declarations */
extern one_shot_timer_t lpticker_one_shot;  /* wakeup source for C3,C4 */

static cpu_pwr_t cpu_pwr_node_core_0;
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
    int cnt;
    switch (cpuCState) {
        case CPU_CSTATE_C0:

            if (master) {
                /*
                 * do something needed when CPU waked up from C1 or higher
                 * Cx state.
                 */
                 //printf("enter C0\r\n");
				 //wland_set_sta_sleep(1);
            }
            break;

        case CPU_CSTATE_C1: /* SLEEP_NORMAL */

            /* put CPU into C1 state, for ARM we can call WFI instruction
               to put CPU into C1 state. */
            PWR_DBG(DBG_INFO, "enter C1\n");
            __DSB();
            __WFI();
            __ISB();
            PWR_DBG(DBG_INFO, "exit C1\n");
            break;

        default:
            PWR_DBG(DBG_ERR, "invalid C state: C%d\n", cpuCState);
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
    cpu_pwr_t    *pCpuNode = NULL;
    pwr_status_t  retVal   = PWR_OK;
    uint32_t      cpuIndex = 0;    /* 0 for UP */

    pCpuNode = &cpu_pwr_node_core_0;
    retVal   = cpu_pwr_node_init_static("core", 0, pCpuNode);
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
     */

    retVal = cpu_pwr_c_method_set(cpuIndex, board_cpu_c_state_set);
    if (retVal == PWR_ERR) {
        return PWR_ERR;
    }

    /* save support C status bitset : C0,C1 */
    cpu_pwr_c_state_capability_set(cpuIndex, CPU_STATE_BIT(CPU_CSTATE_C0)
                                   | CPU_STATE_BIT(CPU_CSTATE_C1)
                                   );
    if (retVal == PWR_ERR) {
        return PWR_ERR;
    }

    /*
     * According reference manual of STM32L496G-DISCOVERY,
     * the wakeup latency of Cx is:
     * resume from C1 (Low Power mode)       : immediate
     */
    cpu_pwr_c_state_latency_save(cpuIndex, CPU_CSTATE_C0, 0);
    cpu_pwr_c_state_latency_save(cpuIndex, CPU_CSTATE_C1, 0);

    tickless_one_shot_timer_save(CPU_CSTATE_C1, &lpticker_one_shot);

    /*
    Tell the CPU PWR MGMT module which C state is supported with
    tickless function through tickless_c_states_add(c_state_x).
    */
    tickless_c_states_add(CPU_STATE_BIT(CPU_CSTATE_C1));

#if RHINO_CONFIG_CPU_PWR_SHOW
    cpu_pwr_info_show();
    cpu_pwr_state_show();
#endif

    return retVal;
}

#endif /* RHINO_CONFIG_CPU_PWR_MGMT */
