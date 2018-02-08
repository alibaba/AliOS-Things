 /** mbed Microcontroller Library
  ******************************************************************************
  * @file    timer_api.c
  * @author 
  * @version V1.0.0
  * @date    2016-08-01
  * @brief   This file provides mbed API for gtimer.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "objects.h"
#include "timer_api.h"

/**
  * @brief  gtimer interrupt handler function.
  * @param  data: timer IRQ callback parameter
  * @retval   none
  */
static void gtimer_timeout_handler (uint32_t data)
{
	gtimer_t *obj = (gtimer_t *)data;
	uint32_t tid = obj->timer_id;
	gtimer_irq_handler handler;

	RTIM_INTClear(TIMx[tid]);

	if (obj->handler != NULL) {
		handler = (gtimer_irq_handler)obj->handler;
		handler(obj->hid);
	}

	if (!obj->is_periodcal) {
		gtimer_stop(obj);
	}
}

/**
  * @brief  Initializes the timer device, include timer registers and interrupt.
  * @param  obj: timer object define in application software.
  * @param  tid: general timer ID, which can be one of the following parameters:
  *              @arg TIMER0
  *              @arg TIMER1
  *              @arg TIMER2
  *              @arg TIMER3
  * @note TIMER0 is reserved, TIMER1/2/3 are recommended.
  * @retval none  
  */
void gtimer_init (gtimer_t *obj, uint32_t tid)
{
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;

	assert_param((tid < GTIMER_MAX) && (tid > TIMER0));

	obj->timer_id = tid;

	RTIM_TimeBaseStructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_Idx = (u8)tid;

	TIM_InitStruct.TIM_UpdateEvent = ENABLE; /* UEV enable */
	TIM_InitStruct.TIM_UpdateSource = TIM_UpdateSource_Overflow;
	TIM_InitStruct.TIM_ARRProtection = ENABLE;
	
	RTIM_TimeBaseInit(TIMx[tid], &TIM_InitStruct, TIMx_irq[tid], (IRQ_FUN) gtimer_timeout_handler, (u32)obj);
}

/**
  * @brief  Deinitializes the timer device, include timer function and interrupt.
  * @param  obj: timer object define in application software.
  * @retval none  
  */
void gtimer_deinit (gtimer_t *obj)
{
	uint32_t tid = obj->timer_id;

	assert_param((tid < GTIMER_MAX) && (tid > TIMER0));
	
	RTIM_DeInit(TIMx[tid]);
}

/**
  * @brief  Get counter value of the specified timer.
  * @param  obj: timer object define in application software.
  * @retval value: counter value
  */
uint32_t gtimer_read_tick (gtimer_t *obj)
{
	uint32_t tid = obj->timer_id;

	assert_param((tid < GTIMER_MAX) && (tid > TIMER0));
	
	return (RTIM_GetCount(TIMx[tid]));
}

/**
  * @brief  Get count value in microseconds of the specified timer.
  * @param  obj: timer object define in application software.
  * @retval value: count value in microseconds.
  */
uint64_t gtimer_read_us (gtimer_t *obj)  //need to be test in IAR(64bit computing)
{
	assert_param((obj->timer_id < GTIMER_MAX) && (obj->timer_id > TIMER0));
	
	uint64_t time_us;
	time_us = gtimer_read_tick(obj) *1000000 /32768;

	return (time_us);
}

/**
  * @brief  Change period of the specified timer.
  * @param  obj: timer object define in application software.
  * @param  duration_us: the new period to be set in microseconds.
  * @retval none
  */
void gtimer_reload (gtimer_t *obj, uint32_t duration_us)
{
	uint32_t tid = obj->timer_id;
	uint32_t temp = (uint32_t)((float)duration_us  / 1000000 *32768);

	assert_param((tid < GTIMER_MAX) && (tid > TIMER0));
	
    	RTIM_ChangePeriod(TIMx[tid], temp);
}

/**
  * @brief  Start the specified timer and enable update interrupt.
  * @param  obj: timer object define in application software.
  * @retval none
  */
void gtimer_start (gtimer_t *obj)
{
	uint32_t tid = obj->timer_id;

	assert_param((tid < GTIMER_MAX) && (tid > TIMER0));

	RTIM_INTConfig(TIMx[tid], TIM_IT_Update, ENABLE);
	RTIM_Cmd(TIMx[tid], ENABLE);
}

/**
  * @brief Start the specified timer in one-shot mode with specified period and interrupt handler.
  * @param  obj: timer object define in application software.
  * @param  duration_us: the new period to be set in microseconds.
  * @param  handler: user defined IRQ callback function
  * @param  hid: user defined IRQ callback parameter
  * @retval none
  * @note This function set the timer into one-shot mode which stops after the first time the counter overflows.
  */
void gtimer_start_one_shout (gtimer_t *obj, uint32_t duration_us, void* handler, uint32_t hid)
{
	assert_param((obj->timer_id < GTIMER_MAX) && (obj->timer_id > TIMER0));

	obj->is_periodcal = _FALSE;
	obj->handler = handler;
	obj->hid = hid;

	gtimer_reload(obj, duration_us);
	gtimer_start(obj);
}

/**
  * @brief Start the specified timer in periodical mode with specified period and interrupt handler.
  * @param  obj: timer object define in application software.
  * @param  duration_us: the new period to be set in microseconds.
  * @param  handler: user defined IRQ callback function
  * @param  hid: user defined IRQ callback parameter
  * @retval none
  * @note This functon set the timer into periodical mode which will restart to count from 0 each time the counter overflows.
  */
void gtimer_start_periodical (gtimer_t *obj, uint32_t duration_us, void* handler, uint32_t hid)
{
	assert_param((obj->timer_id < GTIMER_MAX) && (obj->timer_id > TIMER0));

	obj->is_periodcal = _TRUE;
	obj->handler = handler;
	obj->hid = hid;

	gtimer_reload(obj, duration_us);
	gtimer_start(obj);
}

/**
  * @brief Disable the specified timer peripheral.
  * @param  obj: timer object define in application software.
  * @retval none
  */
void gtimer_stop (gtimer_t *obj)
{
	uint32_t tid = obj->timer_id;

	assert_param((tid < GTIMER_MAX) && (tid > TIMER0));
	
	RTIM_Cmd(TIMx[tid], DISABLE);    
}
