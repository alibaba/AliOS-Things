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

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)

#include <cpu_pwr_hal_lib.h>
#include <cpu_tickless.h>
#include <pwrmgmt_debug.h>

#include <rtl8710b.h>

typedef enum {
	PMU_OS					=0,
	PMU_WLAN_DEVICE		=1,
	PMU_LOGUART_DEVICE	=2,
	PMU_SDIO_DEVICE		=3,

	PMU_UART0_DEVICE		=4,
	PMU_UART1_DEVICE		=5,
	PMU_I2C0_DEVICE		=6,
	PMU_I2C1_DEVICE		=7,
	PMU_USOC_DEVICE		=8,
	PMU_DONGLE_DEVICE	=9,
	PMU_RTC_DEVICE		=10,
	PMU_CONSOL_DEVICE	=11,
	PMU_ADC_DEVICE	=12,
	PMU_WAKWLOCK_TIMEOUT=13,
	PMU_DEV_USER_BASE	=16,

	PMU_MAX				=31
} PMU_DEVICE;

enum SLEEP_TYPE {
	SLEEP_PG	= 0,
	SLEEP_CG	= 1,
};

#define PMU_DEV_OTA (PMU_DEV_USER_BASE + 1)

#define SLEEP_WAKEUP_BY_STIMER			(BIT_SYSON_WEVT_SYSTIM_MSK)    // wake up by system timer
#define SLEEP_WAKEUP_BY_GPIO_INT		(BIT_SYSON_WEVT_GPIO_MSK)      // wake up by GPIO Port A[31:0] Interrupt
#define SLEEP_WAKEUP_BY_WLAN			(BIT_SYSON_WEVT_WLAN_MSK)        // wake up by WLan event
#define SLEEP_WAKEUP_BY_SDIO			(BIT_SYSON_WEVT_SDIO_MSK)      // wake up by SDIO event
#define SLEEP_WAKEUP_BY_USB				(BIT_SYSON_WEVT_USB_MSK)       // wake up by USB event
#define SLEEP_WAKEUP_BY_GPIO			(BIT_SYSON_WEVT_GPIO_DSTBY_MSK)       // GPIO Port(PA_18, PA_5, PA_22, PA_23)
#define SLEEP_WAKEUP_BY_UART			(BIT_SYSON_WEVT_UART0_MSK | BIT_SYSON_WEVT_UART1_MSK)       // wake up by UART event
#define SLEEP_WAKEUP_BY_I2C				(BIT_SYSON_WEVT_I2C0_MSK | BIT_SYSON_WEVT_I2C1_MSK)		 // wake up by I2C event
#define SLEEP_WAKEUP_BY_RTC				(BIT_SYSON_WEVT_RTC_MSK)       // wake up by RTC event

/* forward declarations */
extern one_shot_timer_t rtc_one_shot;  /* wakeup source for C3,C4 */

static uint32_t  wakeup_event = SLEEP_WAKEUP_BY_STIMER | SLEEP_WAKEUP_BY_GPIO_INT | SLEEP_WAKEUP_BY_WLAN | SLEEP_WAKEUP_BY_UART | SLEEP_WAKEUP_BY_RTC;
extern uint64_t  expeted_sleep_ms;
static cpu_pwr_t cpu_pwr_node_core_0;

void pmu_release_wakelock(uint32_t nDeviceId);

void sleep_ex_cg(uint32_t wakeup_event,  uint32_t sleep_duration)
{
	SOCPS_SleepInit();

    static uint32_t CalibData = 0;
    if (CalibData == 0) {
         CalibData = SOCPS_CLKCal(A33CK);
    }

	/* user setting have high priority */
	SOCPS_SetWakeEvent(wakeup_event, ENABLE);
	if (sleep_duration > 0) {
		SOCPS_SET_REGUTIMER(sleep_duration, CalibData);
	}

	SOCPS_SleepCG();

    NVIC_SetPriority(UART_LOG_IRQ, 10);
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
            PWRMGMT_LOG(PWRMGMT_LOG_DBG, "enter C1\n");
            sleep_ex_cg(wakeup_event, (uint32_t)expeted_sleep_ms);
            PWRMGMT_LOG(PWRMGMT_LOG_DBG, "exit C1\n");
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

    pmu_set_sysactive_time(5000);
    pmu_set_sleep_type(SLEEP_CG);
    pmu_release_wakelock(PMU_OS);

    return retVal;
}

#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER */
