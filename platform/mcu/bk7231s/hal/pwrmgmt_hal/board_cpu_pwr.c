/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */


#include <k_api.h>

#if RHINO_CONFIG_CPU_PWR_MGMT

#include "cpu_pwr_hal_lib.h"
#include "pwrmgmt_debug.h"
#include "cpu_tickless.h"
#include "mcu_ps_pub.h"

/* forward declarations */
extern one_shot_timer_t mcu_one_shot; /* wakeup source for C1*/

static cpu_pwr_t cpu_pwr_node_core_0;


/**
 * board_cpu_c_state_set - program CPU into Cx idle state
 *
 * @return  PWR_OK or PWR_ERR when failed.
 */
static pwr_status_t board_cpu_c_state_set(uint32_t cpuCState, int master)
{
    switch (cpuCState)
    {
    case CPU_CSTATE_C0:

        if (master)
        {
            /*
             * do something needed when CPU waked up from C1 or higher
             * Cx state.
             */
        }

        break;
    case CPU_CSTATE_C1:
        /* put CPU into C1 state, for ARM we can call WFI instruction
           or sleep instruction. */
        PWRMGMT_LOG(PWRMGMT_LOG_DBG, "enter C1\n");
        aos_mcu_ps_sleep();


        break;
    case CPU_CSTATE_C2:
        PWRMGMT_LOG(PWRMGMT_LOG_DBG, "enter C2\n");

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
 * @return  PWR_OK or PWR_ERR when failed.
 */
pwr_status_t board_cpu_pwr_init(void)
{
    cpu_pwr_t    *pCpuNode  = NULL;
    pwr_status_t  retVal    = PWR_OK;
    uint32_t      cpuIndex  = 0;

    pCpuNode = &cpu_pwr_node_core_0;
    retVal = cpu_pwr_node_init_static("core", 0, pCpuNode);
    if (retVal != PWR_OK)
    {
        return PWR_ERR;
    }
    /* record this node */
    retVal = cpu_pwr_node_record(pCpuNode, cpuIndex);
    if (retVal == PWR_ERR)
    {
        return PWR_ERR;
    }

    /*
     * C0 - RUN,  Power supplies are on,all clocks are on.
     * C1 - Sleep mode
     */

    /* C0,C1,is controlled by level 2 : core */
    retVal = cpu_pwr_c_method_set(cpuIndex, board_cpu_c_state_set);
    if (retVal == PWR_ERR)
    {
        return PWR_ERR;
    }

    /* save support C status bitset in level 2: C0,C1, */
    cpu_pwr_c_state_capability_set(cpuIndex, CPU_STATE_BIT(CPU_CSTATE_C0)
                                   | CPU_STATE_BIT(CPU_CSTATE_C1)
                                  );
    if (retVal == PWR_ERR)
    {
        return PWR_ERR;
    }

    /*
     * resume from C1 (Low Power mode)       : immediate
     */
    cpu_pwr_c_state_latency_save(cpuIndex, CPU_CSTATE_C0, 0);
    cpu_pwr_c_state_latency_save(cpuIndex, CPU_CSTATE_C1, 1000);

    tickless_one_shot_timer_save(CPU_CSTATE_C1, &mcu_one_shot);


    /*
    Tell the CPU PWR MGMT module which C state is supported with
    tickless function through tickless_c_states_add(c_state_x).
    */

    tickless_c_states_add(CPU_STATE_BIT(CPU_CSTATE_C0)
                          | CPU_STATE_BIT(CPU_CSTATE_C1)
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
    */

#if (CPU_PWR_SHOW > 0)
    cpu_pwr_info_show();
    cpu_pwr_state_show();
#endif

    return retVal;
}

void cpu_pwr_suspend_devices()
{
}

void cpu_pwr_resume_devices()
{
}

#endif /* RHINO_CONFIG_CPU_PWR_MGMT */

