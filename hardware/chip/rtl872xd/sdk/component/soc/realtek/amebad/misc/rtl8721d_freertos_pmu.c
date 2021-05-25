#include "FreeRTOS.h"
#include <task.h>
#include <timers.h>
#include "freertos_pmu.h"
#include "platform_autoconf.h"
#include "ameba_soc.h"
#include "osdep_service.h"


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

u32 tickless_debug = 0;

/* ++++++++ FreeRTOS macro implementation ++++++++ */

/* psm dd hook info */
PSM_DD_HOOK_INFO gPsmDdHookInfo[PMU_MAX];

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

			if (ret == FALSE)
				break;
		}
	}

	return nDeviceIdOffset;
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

	TimeOut = xTaskGetTickCount() + timeout;

	if (TimeOut > sleepwakelock_timeout) {
		sleepwakelock_timeout = TimeOut;
	}
	return 0;
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


/* can not yield CPU under suspend/resume process */
uint32_t pmu_yield_os_check(void)
{
	return system_can_yield;
}

/*
 *  It is called in idle task.
 *
 *  @return  true  : System is ready to check conditions that if it can enter sleep.
 *           false : System keep awake.
 **/
CONFIG_FW_CRITICAL_CODE_SECTION
int freertos_ready_to_sleep(void)
{
	u32 current_tick = xTaskGetTickCount();

	/* timeout */
	if (current_tick < sleepwakelock_timeout) {
		return FALSE;
	}

	if (wakelock == 0) {
#if defined (ARM_CORE_CM0)
		/* timeout */
		if (current_tick >= km4_sleep_timeout) {
			return FALSE;
		} else {
			if (km4_sleep_timeout != 0xffffffff) {
				SOCPS_AONTimer(km4_sleep_timeout - current_tick);
				SOCPS_SetWakeEventAON(BIT_AON_WAKE_TIM0_MSK, ENABLE);
				SOCPS_AONTimerCmd(ENABLE);
			}
		}
#endif
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
	u32 current_tick = xTaskGetTickCount();

	/* timeout */
	if (current_tick < deepwakelock_timeout) {
		return FALSE;
	}

	if (deepwakelock == 0)
		return TRUE;
	else
		return FALSE;
}

/*
 *  It is called when freertos is going to sleep.
 *  At this moment, all sleep conditons are satisfied. All freertos' sleep pre-processing are done.
 *
 *  @param  expected_idle_time : The time that FreeRTOS expect to sleep.
 *                               If we set this value to 0 then FreeRTOS will do nothing in its sleep function.
 **/
#if defined (ARM_CORE_CM4)
void freertos_pre_sleep_processing(unsigned int *expected_idle_time)
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

	*expected_idle_time = 0;

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
	vTaskStepTick(ms_passed); /*  update kernel tick */
	
	wakeup_time_ms = SYSTIMER_GetPassTime(0);

	sysactive_timeout_flag = 0;
	pmu_set_sysactive_time(2);

	if (tickless_debug) {
		DBG_8195A("m4 sleeped:[%d] ms\n", wakeup_time_ms - ms_before_sleep);
	}
}

#else
CONFIG_FW_CRITICAL_CODE_SECTION
void freertos_pre_sleep_processing(unsigned int *expected_idle_time)
{

#ifdef CONFIG_SOC_PS_MODULE
	uint32_t tick_before_sleep;
	uint32_t tick_passed;
	uint32_t tick_after_sleep;
	volatile uint32_t ms_passed = 0;
	uint32_t ms_before_sleep = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_TSF_CNT_LOW);
	uint32_t wakeup_time_ms = 0;

	*expected_idle_time = 0;

	/*  Store gtimer timestamp before sleep */
	tick_before_sleep = SYSTIMER_TickGet();
	sysactive_timeout_flag = 1;
	/*Enter dlps if dsleep is ready, or enter sleep mode*/
	//if (freertos_ready_to_dsleep()) {
	//	SOCPS_AONTimer(ps_config.km0_dslp_wake_interval);
	//	SOCPS_AONTimerCmd(ENABLE);
		
	//	SOCPS_DeepSleep_RAM(); /* The function will not return. */
	//} 

	/* some function call like xTaskGetTickCount may cause IRQ ON, */
	/* so we close IRQ again here to avoid sys irq when enter or exit sleep */	
	//__asm volatile( "cpsid i" );

	SOCPS_SWRLDO_Suspend(ENABLE);

	/* make sure the following function dont call system API that may open IRQ */

	SOCPS_SleepInit();

	/* for test */
	if ((BKUP_Read(0) & BIT_WIFI_ENABLE) == 0) {
		SOCPS_AONTimer(102);
		SOCPS_SetWakeEventAON(BIT_AON_WAKE_TIM0_MSK, ENABLE);
		SOCPS_AONTimerCmd(ENABLE);
	}

	if (sleep_type == SLEEP_PG) {
		SOCPS_SleepPG();
	} else {
		SOCPS_SleepCG();
	}

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
	vTaskStepTick(ms_passed); /*  update kernel tick */
	
	wakeup_time_ms = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_TSF_CNT_LOW);

	if (ps_config.km0_tickles_debug) {
		DBG_8195A("m0 os sleeped:[%d->%d:%d] us\n", ms_before_sleep&0x7FFFFFFF,
			wakeup_time_ms&0x7FFFFFFF, wakeup_time_ms - ms_before_sleep);
	}

	sysactive_timeout_flag = 0;
	pmu_set_sysactive_time(2);

	SOCPS_SWRLDO_Suspend(DISABLE);
	
	//__asm volatile( "cpsie i" );
#else
	/*  If PS is not enabled, then use freertos sleep function */
#endif
}
#endif

CONFIG_FW_CRITICAL_CODE_SECTION
void freertos_post_sleep_processing(unsigned int *expected_idle_time)
{
#ifndef configSYSTICK_CLOCK_HZ
	*expected_idle_time = 1 + ( SysTick->VAL / ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) );
#else
	*expected_idle_time = 1 + ( SysTick->VAL / ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) );
#endif
}

/* NVIC will power off under sleep power gating mode, so we can */
/* not use systick like FreeRTOS default implementation */
CONFIG_FW_CRITICAL_CODE_SECTION
void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
	unsigned int xModifiableIdleTime;
	eSleepModeStatus eSleepStatus;

	system_can_yield = 0;
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	
	/* Enter a critical section but don't use the taskENTER_CRITICAL()
	method as that will mask interrupts that should exit sleep mode. */
#if defined (ARM_CORE_CM0)
	taskENTER_CRITICAL();
#else
	__asm volatile( "cpsid i" );
#endif

	eSleepStatus = eTaskConfirmSleepModeStatus();
	
	/* If a context switch is pending or a task is waiting for the scheduler
	to be unsuspended then abandon the low power entry. */
	if(eSleepStatus == eAbortSleep)
	{
	}
	else if(freertos_ready_to_sleep())
	{
		/* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
		set its parameter to 0 to indicate that its implementation contains
		its own wait for interrupt or wait for event instruction, and so wfi
		should not be executed again.  However, the original expected idle
		time variable must remain unmodified, so a copy is taken. */
		xModifiableIdleTime = (unsigned int)xExpectedIdleTime;
		configPRE_SLEEP_PROCESSING( xModifiableIdleTime );
		if( xModifiableIdleTime > 0 )
		{
			__asm volatile( "dsb" );
			__asm volatile( "wfi" );
			__asm volatile( "isb" );
		}
		configPOST_SLEEP_PROCESSING( xModifiableIdleTime );
	}
#if defined (ARM_CORE_CM4)
	else
	{
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		__asm volatile( "dsb" );
		__asm volatile( "wfi" );
		__asm volatile( "nop" ); //fix cpu fused instruction issue
		__asm volatile( "isb" );
	}
#endif
	/* Re-enable interrupts - see comments above the cpsid instruction()
	above. */
#if defined (ARM_CORE_CM0)
	taskEXIT_CRITICAL();
#else
	__asm volatile( "cpsie i" );
#endif
		
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	system_can_yield = 1;

#if defined (ARM_CORE_CM0)
	km4_wake_event_update();
	if (km4_get_wake_event()) {
		taskENTER_CRITICAL();
		km4_resume();
		taskEXIT_CRITICAL();
	}
#endif
}
	
/* -------- FreeRTOS macro implementation -------- */

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

uint32_t pmu_set_sleep_type(uint32_t type)
{
	sleep_type = type;
#if defined (ARM_CORE_CM0)
	if (ps_config.km0_pg_enable) {
		sleep_type = SLEEP_PG;
	}
#endif
	return 0;
}

uint32_t pmu_get_sleep_type(void)
{
	return sleep_type;
}

void pmu_set_max_sleep_time(uint32_t timer_ms)
{
	max_sleep_time = timer_ms;
}

void pmu_set_dsleep_active_time(uint32_t TimeOutMs)
{
	u32 timeout = 0;


	timeout = xTaskGetTickCount() + TimeOutMs;
	//DBG_8195A("pmu_set_dsleep_active_time: %d %d\n", timeout, deepwakelock_timeout);

	if (timeout > deepwakelock_timeout) {
		deepwakelock_timeout = timeout;
	}
}

void pmu_acquire_deepwakelock(uint32_t nDeviceId)
{
	deepwakelock |= BIT(nDeviceId);
}

void pmu_release_deepwakelock(uint32_t nDeviceId)
{
	deepwakelock &= ~BIT(nDeviceId);
}

uint32_t pmu_get_deepwakelock_status(void)
{
	return deepwakelock;
}


void pmu_tickless_debug(u32 NewStatus)
{
	if (NewStatus == ENABLE) {
		tickless_debug = 1;
	} else {
		tickless_debug = 0;
	}
}
