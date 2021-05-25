/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */
#include "objects.h"
#include <stddef.h>
#include "us_ticker_api.h"
#include "PeripheralNames.h"

#define TICK_READ_FROM_CPU		0 //1: read tick from CPU, 0: read tick from G-Timer
#define GTIMER_CLK_HZ			(32768)
#define GTIMER_TICK_US			(1000000/GTIMER_CLK_HZ)

#define SYS_TIM_ID				0 // the G-Timer ID for System
#define APP_TIM_ID				1 // the G-Timer ID for Application

static int us_ticker_inited = 0;

VOID _us_ticker_irq_handler(void *Data)
{
	/* To avoid gcc warnings */
	( void ) Data;
	
	us_ticker_irq_handler();
}

void us_ticker_init(void) 
{
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;
	
	if (us_ticker_inited)
		return;
	us_ticker_inited = 1;

	RTIM_TimeBaseStructInit(&TIM_InitStruct);

	TIM_InitStruct.TIM_Idx = APP_TIM_ID;
	TIM_InitStruct.TIM_Prescaler = 0;
	TIM_InitStruct.TIM_Period = 0xFFFFFFFF;

	TIM_InitStruct.TIM_UpdateEvent = ENABLE; /* UEV enable */
	TIM_InitStruct.TIM_UpdateSource = TIM_UpdateSource_Overflow;
	TIM_InitStruct.TIM_ARRProtection = ENABLE;

	RTIM_TimeBaseInit(TIMM01, &TIM_InitStruct, 0, (IRQ_FUN) _us_ticker_irq_handler, (u32)NULL);
	RTIM_Cmd(TIMM01, ENABLE);

	DBG_PRINTF(MODULE_TIMER, LEVEL_INFO, "%s: Timer_Id=%d\n", __FUNCTION__, APP_TIM_ID);
}

#if (!TICK_READ_FROM_CPU) || !defined(PLATFORM_FREERTOS)
uint32_t us_ticker_read(void) 
{
	uint32_t tick_cnt;
	uint64_t us_tick;

	tick_cnt = SYSTIMER_TickGet(); //up counter
	us_tick = tick_cnt * (1000000/32768);
	
	return ((uint32_t)us_tick);
}
#else
// if the system tick didn't be initialed, call delay function may got system hang
#define OS_CLOCK        (200000000UL/6*5)       // CPU clock = 166.66 MHz
#define OS_TICK         1000                    // OS ticks 1000/sec
#define OS_TRV          ((uint32_t)(((double)OS_CLOCK*(double)OS_TICK)/1E6)-1)
#define NVIC_ST_CTRL    (*((volatile uint32_t *)0xE000E010))
#define NVIC_ST_RELOAD  (*((volatile uint32_t *)0xE000E014))
#define NVIC_ST_CURRENT (*((volatile uint32_t *)0xE000E018))

extern uint32_t xTaskGetTickCount( void );

uint32_t us_ticker_read(void) 
{
	uint32_t tick_cnt;
	uint32_t us_tick, ms;
	static uint32_t last_us_tick=0;

	ms = xTaskGetTickCount();
	us_tick = (uint32_t)(ms*1000);

	tick_cnt = OS_TRV - NVIC_ST_CURRENT;
	us_tick += (uint32_t)((tick_cnt*1000)/(OS_TRV+1) );

	if ( (last_us_tick > us_tick) && (last_us_tick < 0xFFFFFC00) ) {
		us_tick += 1000;
	}
	last_us_tick = us_tick;
	
	return us_tick;        
}
#endif

void us_ticker_set_interrupt(timestamp_t timestamp) 
{
	uint32_t cur_time_us;
	uint32_t duration_us;
	uint32_t timer_tick = 0;

	cur_time_us = us_ticker_read();
	if ((uint32_t)timestamp >= cur_time_us) {
		duration_us = (uint32_t)timestamp - cur_time_us;
	}
	else {
		duration_us = 0xffffffff - cur_time_us + (uint32_t)timestamp;
	}    

	if (duration_us < TIMER_TICK_US) {
		duration_us = TIMER_TICK_US;       // at least 1 tick
	}

	timer_tick = (uint32_t)((float)duration_us  / 1000000 *32000);	
	RTIM_ChangePeriod(TIMx_LP[APP_TIM_ID], timer_tick);
}

void us_ticker_disable_interrupt(void) 
{
	RTIM_Cmd(TIMx_LP[APP_TIM_ID], DISABLE);
}

void us_ticker_clear_interrupt(void) 
{
	RTIM_INTClear(TIMx_LP[APP_TIM_ID]);//AembaD TODO
}
