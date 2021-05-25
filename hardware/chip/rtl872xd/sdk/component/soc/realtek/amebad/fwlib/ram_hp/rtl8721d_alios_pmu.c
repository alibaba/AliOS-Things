#include <k_api.h>
#include "sys_api.h"
#include "sleep_ex_api.h"
//#include "alios_service.h"

uint32_t missing_tick = 0;

static uint32_t wakelock     = DEFAULT_WAKELOCK;
static uint32_t sleepwakelock_timeout     = 0;
static u32 system_can_yield = 1; /* default is can */
static uint32_t sleep_type = SLEEP_PG; /* 0 is power gate, 1 is clock gate */
static uint32_t max_sleep_time = 0; /* if user want wakeup peridically, can set this timer*/
KM4SLEEP_ParamDef sleep_param;

static uint32_t deepwakelock     = DEFAULT_DEEP_WAKELOCK;
static uint32_t deepwakelock_timeout     = 0;
static uint32_t sysactive_timeout_temp = 0;
static uint32_t sysactive_timeout_flag = 0;

static int           cpu_pwr_init_flag = 0;
static kspinlock_t cpu_pwr_spin;
static uint32_t wakeup_time_ms = 0;

uint32_t missing_tick_alios = 0;


u32 tickless_debug = 0;
void pmu_acquire_wakelock(uint32_t nDeviceId)
{
        wakelock |= BIT(nDeviceId);
}

void pmu_release_wakelock(uint32_t nDeviceId)
{
        wakelock &= ~BIT(nDeviceId);
}

uint32_t pmu_get_wakelock_status(void)
{
        return wakelock;
}

/* can not yield CPU under suspend/resume process */
uint32_t pmu_yield_os_check(void)
{
	return system_can_yield;
}
CONFIG_FW_CRITICAL_CODE_SECTION
int freertos_ready_to_sleep(void)
{
	u32 current_tick = SYSTIMER_TickGet();

	/* timeout */
	if (current_tick < sleepwakelock_timeout) {
		return FALSE;
	}

	if (wakelock == 0) {
		return TRUE;
	}
	else
		return FALSE;
}

/*
 *  It is called in freertos pre_sleep_processing.
 *
 *  @return  true  : System is ready to check conditions that if it can enter dsleep.
 *           false : System can't enter deep sleep.
 **/
CONFIG_FW_CRITICAL_CODE_SECTION
int freertos_ready_to_dsleep(void)
{
	u32 current_tick = SYSTIMER_TickGet();

	/* timeout */
	if (current_tick < deepwakelock_timeout) {
		return FALSE;
	}

	if (deepwakelock == 0)
		return TRUE;
	else
		return FALSE;
}
#define AliOS_ready_to_sleep           freertos_ready_to_sleep

uint32_t pmu_set_sysactive_time(uint32_t timeout)
{
	u32 TimeOut = 0;

	if(sysactive_timeout_flag){
		if (timeout > sysactive_timeout_temp) {
			sysactive_timeout_temp = timeout;
		}
		return 1;
	}

	if(sysactive_timeout_temp > timeout) {
		timeout = sysactive_timeout_temp;
	}
	sysactive_timeout_temp = 0;

	TimeOut =  SYSTIMER_TickGet() + timeout; //xTaskGetTickCount() + timeout;

	if (TimeOut > sleepwakelock_timeout) {
		sleepwakelock_timeout = TimeOut;
	}
	return 0;
}

uint32_t pmu_yield_os_set(int value)
{
	system_can_yield = value;
}

/*
 *  It is called in idle task.
 *
 *  @return  true  : System is ready to check conditions that if it can enter sleep.
 *           false : System keep awake.
 **/


/**
 * tickless_announce_n() is called to announces elapsed ticks to the kernel.
 */
static void tickless_announce_n(tick_t n_ticks)
{
    tick_list_update((tick_t)n_ticks);
}

/**
 * tickless_enter() is called when a CPU is going to enter idle state, a one
 * shot interrupt is planned at sametime which is used to wake up CPU.
 * @return  N/A
 */
static void tickless_enter(void)
{
	uint32_t tick_before_sleep;
	uint32_t tick_passed;
	uint32_t tick_after_sleep;
	volatile uint32_t ms_passed = 0;
	uint32_t ms_before_sleep = SYSTIMER_GetPassTime(0);
	uint32_t wakeup_time_ms = 0;

	if (freertos_ready_to_dsleep()) {
		sleep_param.sleep_time = 0;// do not wake on system schedule tick
		sleep_param.dlps_enable = ENABLE;
	} else {
		sleep_param.sleep_time = max_sleep_time;//*expected_idle_time;
		max_sleep_time = 0;
		sleep_param.dlps_enable = DISABLE;
	}
	sleep_param.sleep_type = sleep_type;

	//*expected_idle_time = 0;

	/*  Store gtimer timestamp before sleep */
	tick_before_sleep = SYSTIMER_TickGet();
	sysactive_timeout_flag = 1;

	IPCM0_DEV->IPCx_USR[IPC_INT_CHAN_SHELL_SWITCH] = 0x00000000;
	InterruptDis(UART_LOG_IRQ);

	//BKUP_Set(BKUP_REG0, BIT_KM4_WAKE_DELAY);
	
	if (sleep_type == SLEEP_PG) {
		SOCPS_SleepPG();
	} else {
		SOCPS_SleepCG();
	}

	//BKUP_Clear(BKUP_REG0, BIT_KM4_WAKE_DELAY);

	/*  update kernel tick by calculating passed tick from gtimer */
	/*  get current gtimer timestamp */
	tick_after_sleep = SYSTIMER_TickGet();

	/*  calculated passed time */
	if (tick_after_sleep > tick_before_sleep) {
		tick_passed = tick_after_sleep - tick_before_sleep;
	} else {
		/*  overflow */
		tick_passed = (0xffffffff - tick_before_sleep) + tick_after_sleep;
	}

	tick_passed += missing_tick;
	missing_tick = tick_passed & 0x1F;

	/* timer clock is 32768, 0x20 is 1ms */
	ms_passed = (((tick_passed & 0xFFFFFFE0) * 1000)/32768);
	tickless_announce_n(ms_passed); /*  update kernel tick */
	
	wakeup_time_ms = SYSTIMER_GetPassTime(0);

	sysactive_timeout_flag = 0;
	pmu_set_sysactive_time(2);

	if (tickless_debug) {
		DBG_8195A("m4 sleeped:[%d] ms\n", wakeup_time_ms - ms_before_sleep);
	}
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
}


/**
 * cpu_pwrmgmt_init() is the entry of whole cpu power manamgement system.
 */
void cpu_pwrmgmt_init(void)
{
    /* avoid reentry */
    if (cpu_pwr_init_flag) {
        return;
    }

//    tickless_debug = 1; 
    
    krhino_spin_lock_init(&cpu_pwr_spin);

    /* create cpu power management framework and cpu topology structure */
    //cpu_pwr_hal_lib_init();

    /* init cpu tickless framework */
    //tickless_init();

    /* save the job complete flag */
    cpu_pwr_init_flag = 1;
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

    krhino_spin_lock_irq_save(&cpu_pwr_spin);
    pmu_yield_os_set(0);
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    	

    if(!AliOS_ready_to_sleep()){
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;    
        pmu_yield_os_set(1);
        krhino_spin_unlock_irq_restore(&cpu_pwr_spin);        
        return;
    }	
    
	/* Enter a critical section but don't use the taskENTER_CRITICAL()
	method as that will mask interrupts that should exit sleep mode. */
	//__asm volatile( "cpsid i" );

    tickless_enter();

   	/* Re-enable interrupts - see comments above the cpsid instruction()
	above. */
	//__asm volatile( "cpsie i" );
    	
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    pmu_yield_os_set(1);	
    krhino_spin_unlock_irq_restore(&cpu_pwr_spin);
    
}

void cpu_pwr_up(void)
{
    if (cpu_pwr_init_flag == 0) {
        return;
    }
}
