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
#include "ameba_soc.h"

#if (AOS_COMP_PWRMGMT > 0)

#include <cpu_pwr.h>
#include <cpu_pwr_hal_lib.h>
#include <pwrmgmt_debug.h>
#include <cpu_tickless.h>

#include "pwrmgmt_api.h"

/* forward declarations */
extern one_shot_timer_t rtc_one_shot;  /* wakeup source for C3,C4 */

extern uint64_t  expeted_sleep_ms;
static cpu_pwr_t cpu_pwr_node_core_0;

KM4SLEEP_ParamDef sleep_param;
/* psm dd hook info */
PSM_DD_HOOK_INFO gPsmDdHookInfo[PMU_MAX];

extern uint32_t sleep_type; /* 0 is power gate, 1 is clock gate */
static u32      system_can_yield = 1; /* default is can */
static uint32_t deepwakelock     = DEFAULT_DEEP_WAKELOCK;

void sleep_ex_cg(uint32_t sleep_duration)
{
    if (deepwakelock == 0) {
        sleep_param.dlps_enable = ENABLE;
        sleep_param.sleep_time  = 0;
    } else {
        sleep_param.dlps_enable = DISABLE;
        sleep_param.sleep_time  = sleep_duration;
    }

    sleep_param.sleep_type  = sleep_type;

    IPCM0_DEV->IPCx_USR[IPC_INT_CHAN_SHELL_SWITCH] = 0x00000000;
    InterruptDis(UART_LOG_IRQ);

    if (sleep_type == SLEEP_PG) {
        SOCPS_SleepPG();
    } else {
        SOCPS_SleepCG();
    }
}


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
            if ((uint32_t)expeted_sleep_ms < 3) {
                return PWR_OK;
            }
            /* put CPU into C1 state, for ARM we can call WFI instruction
               to put CPU into C1 state. */
            PWR_DBG(DBG_DBG, "enter C1\n");
            sleep_ex_cg((uint32_t)expeted_sleep_ms);
            PWR_DBG(DBG_DBG, "exit C1\n");
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

    tickless_one_shot_timer_save(CPU_CSTATE_C1, &rtc_one_shot);

    /*
    Tell the CPU PWR MGMT module which C state is supported with
    tickless function through tickless_c_states_add(c_state_x).
    */
    tickless_c_states_add(CPU_STATE_BIT(CPU_CSTATE_C0)
                          | CPU_STATE_BIT(CPU_CSTATE_C1)
                         );

#if RHINO_CONFIG_CPU_PWR_SHOW
    cpu_pwr_info_show();
    cpu_pwr_state_show();
#endif

    return retVal;
}

uint32_t pmu_yield_os_check(void)
{
    return system_can_yield;
}

uint32_t pmu_yield_os_set(int value)
{
    system_can_yield = value;
}

uint32_t pmu_set_sysactive_time(uint32_t timeout)
{
    pwrmgmt_cpu_active_msec_set(timeout);
}

void pmu_release_wakelock(uint32_t nDeviceId)
{
    nDeviceId = nDeviceId + 17;
    pwrmgmt_cpu_lowpower_resume(nDeviceId);
}

void pmu_acquire_wakelock(uint32_t nDeviceId)
{
    nDeviceId = nDeviceId + 17;
    pwrmgmt_cpu_lowpower_suspend(nDeviceId);
}

void pmu_exec_wakeup_hook_funs(u32 nDeviceIdMax)
{
    	PSM_DD_HOOK_INFO *pPsmDdHookInfo = NULL;
	u32 nDeviceIdOffset = 0;

	for( nDeviceIdOffset = 0; nDeviceIdOffset < nDeviceIdMax; nDeviceIdOffset++) {
		pPsmDdHookInfo = &gPsmDdHookInfo[nDeviceIdOffset];

		/*if this device register and sleep_hook_fun not NULL*/
		if(pPsmDdHookInfo && pPsmDdHookInfo->wakeup_hook_fun) {
			pPsmDdHookInfo->wakeup_hook_fun(0, pPsmDdHookInfo->wakeup_param_ptr);
		}
	}
}

u32 pmu_exec_sleep_hook_funs(void)
{
	PSM_DD_HOOK_INFO *pPsmDdHookInfo = NULL;
	u32 nDeviceIdOffset = 0;
	u32 ret = TRUE;

	for( nDeviceIdOffset = 0; nDeviceIdOffset < PMU_MAX; nDeviceIdOffset++) {
		pPsmDdHookInfo = &gPsmDdHookInfo[nDeviceIdOffset];

		/*if this device register and sleep_hook_fun not NULL*/
		if(pPsmDdHookInfo && pPsmDdHookInfo->sleep_hook_fun) {
			ret = pPsmDdHookInfo->sleep_hook_fun(0, pPsmDdHookInfo->sleep_param_ptr);

            if (ret == FALSE) {
				break;
            }
		}
	}

	return nDeviceIdOffset;
}

void pmu_register_sleep_callback(u32 nDeviceId, PSM_HOOK_FUN sleep_hook_fun, void* sleep_param_ptr, PSM_HOOK_FUN wakeup_hook_fun, void* wakeup_param_ptr)
{
	PSM_DD_HOOK_INFO *pPsmDdHookInfo = NULL;

	assert_param(nDeviceId < PMU_MAX);
	assert_param((sleep_hook_fun != NULL) || (wakeup_hook_fun != NULL));

	pPsmDdHookInfo = &gPsmDdHookInfo[nDeviceId];  
	pPsmDdHookInfo->nDeviceId			= nDeviceId;
	pPsmDdHookInfo->sleep_hook_fun		= sleep_hook_fun;
	pPsmDdHookInfo->sleep_param_ptr		= sleep_param_ptr;
	pPsmDdHookInfo->wakeup_hook_fun		= wakeup_hook_fun;
	pPsmDdHookInfo->wakeup_param_ptr	= wakeup_param_ptr;
}

void pmu_unregister_sleep_callback(u32 nDeviceId)
{
	PSM_DD_HOOK_INFO *pPsmDdHookInfo = NULL;

	assert_param(nDeviceId < PMU_MAX);
	
	pPsmDdHookInfo = &gPsmDdHookInfo[nDeviceId];

	_memset(pPsmDdHookInfo, 0x00, sizeof(PSM_DD_HOOK_INFO));
}

void pmu_acquire_deepwakelock(uint32_t nDeviceId)
{
	deepwakelock |= BIT(nDeviceId);
}

void pmu_release_deepwakelock(uint32_t nDeviceId)
{
	deepwakelock &= ~BIT(nDeviceId);
}

#else
uint32_t pmu_yield_os_check(void)
{
    return 1;
}
void pmu_release_wakelock(uint32_t nDeviceId)
{

}
void pmu_acquire_wakelock(uint32_t nDeviceId)
{

}
uint32_t pmu_set_sysactive_time(uint32_t timeout)
{
    return 0;
}

void pmu_register_sleep_callback(u32 nDeviceId, PSM_HOOK_FUN sleep_hook_fun, void* sleep_param_ptr, PSM_HOOK_FUN wakeup_hook_fun, void* wakeup_param_ptr)
{

}

void pmu_unregister_sleep_callback(u32 nDeviceId){

}
#endif /* AOS_COMP_PWRMGMT */

