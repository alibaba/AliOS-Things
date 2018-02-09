/** mbed Microcontroller Library
  ******************************************************************************
  * @file    sleep.c
  * @author 
  * @version V1.0.0
  * @date    2016-08-01
  * @brief   This file provides mbed API for SLEEP.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
#include "sleep_ex_api.h"
#include "cmsis.h"

SLEEP_WAKEUP_EVENT DStandbyWakeupEvent={0};

/**
  * @brief  To make the system entering the Clock Gated power saving.
  *         This function just make the system to enter the clock gated 
  *         power saving mode and pending on wake up event waitting.
  *         The user application need to configure the peripheral to 
  *         generate system wake up event, like GPIO interrupt, 
  *         G-Timer timeout, etc. befor entering power saving mode.
  * @param  wakeup_event: A bit map of wake up event.
  *   This parameter can be any combination of the following values:
  *		 @arg SLEEP_WAKEUP_BY_STIMER
  *		 @arg SLEEP_WAKEUP_BY_GPIO_INT
  *		 @arg SLEEP_WAKEUP_BY_WLAN
  *		 @arg SLEEP_WAKEUP_BY_SDIO
  *		 @arg SLEEP_WAKEUP_BY_USB
  *		 @arg SLEEP_WAKEUP_BY_GPIO
  *		 @arg SLEEP_WAKEUP_BY_UART
  *		 @arg SLEEP_WAKEUP_BY_I2C
  *		 @arg SLEEP_WAKEUP_BY_RTC
  * @param  sleep_duration: the system sleep duration in ms, only valid
  *         for SLEEP_WAKEUP_BY_STIMER wake up event.
  * @retval None
  */
void sleep_ex(uint32_t wakeup_event,  uint32_t sleep_duration)
{
	__asm volatile( "cpsid i" );

	SOCPS_SleepInit();

	/* user setting have high priority */
	SOCPS_SetWakeEvent(wakeup_event, ENABLE);
	if (sleep_duration > 0) {
		SOCPS_SET_REGUTIMER(sleep_duration, 0xFFFFFFFF);
	}
	
	SOCPS_SleepPG();
}

/**
  * @brief  To make the system entering the Clock Gated power saving.
  *         This function just make the system to enter the clock gated 
  *         power saving mode and pending on wake up event waitting.
  *         The user application need to configure the peripheral to 
  *         generate system wake up event, like GPIO interrupt
  *         , G-Timer timeout, etc. befor entering power saving mode.
  * @param  wakeup_event: A bit map of wake up event. 
  *   This parameter can be any combination of the following values:
  *		 @arg SLEEP_WAKEUP_BY_STIMER
  *		 @arg SLEEP_WAKEUP_BY_GPIO_INT
  *		 @arg SLEEP_WAKEUP_BY_WLAN
  *		 @arg SLEEP_WAKEUP_BY_SDIO
  *		 @arg SLEEP_WAKEUP_BY_USB
  *		 @arg SLEEP_WAKEUP_BY_GPIO
  *		 @arg SLEEP_WAKEUP_BY_UART
  *		 @arg SLEEP_WAKEUP_BY_I2C
  *		 @arg SLEEP_WAKEUP_BY_RTC
  * @param  sleep_duration: the system sleep duration in ms, only valid
  *         for SLEEP_WAKEUP_BY_STIMER wake up event.
  * @param  clk_sourec_enable: the option for SCLK on(1)/off(0)
  * @param  sdr_enable: the option for turn off the SDR controller (1:off, 0:on)
  * @retval None
  */
void sleep_ex_selective(uint32_t wakeup_event,  uint32_t sleep_duration, uint32_t clk_sourec_enable, uint32_t sdr_enable)
{
	sleep_ex(wakeup_event, sleep_duration);
}


/**
  * @brief  To add a wake up event to wake up the system from the
  *         deep standby power saving mode.
  * @param  wakeup_event: A bit map of wake up event.
  *   This parameter can be any combination of the following values:
  *		 @arg STANDBY_WAKEUP_BY_STIMER
  *		 @arg STANDBY_WAKEUP_BY_GPIO
  *		 @arg STANDBY_WAKEUP_BY_RTC
  * @param  sleep_duration_ms: the system sleep duration in ms, only valid
  *             for STANDBY_WAKEUP_BY_STIMER wake up event.
  * @param  gpio_active: for a GPIO pin to wake up the system by goes high or low
  *   This parameter can be any combination of the following values:
  *		 @arg WAKEUP_BY_GPIO_NONE	
  *		 @arg WAKEUP_BY_GPIO_WAKEUP0_LOW
  *		 @arg WAKEUP_BY_GPIO_WAKEUP0_HIG
  *		 @arg WAKEUP_BY_GPIO_WAKEUP1_LOW
  *		 @arg WAKEUP_BY_GPIO_WAKEUP1_HIG
  *		 @arg WAKEUP_BY_GPIO_WAKEUP2_LOW
  *		 @arg WAKEUP_BY_GPIO_WAKEUP2_HIG
  *		 @arg WAKEUP_BY_GPIO_WAKEUP3_LOW
  *		 @arg WAKEUP_BY_GPIO_WAKEUP3_HIG
  * @retval None
  */
void standby_wakeup_event_add(uint32_t wakeup_event, u32 gpio_active)
{
	DStandbyWakeupEvent.wakeup_event |= wakeup_event;

	if (gpio_active != 0) {
		DStandbyWakeupEvent.gpio_option |= gpio_active;
	}
}

/**
  * @brief  To delete a wake up event for wakeing up the system from the
  *         deep standby power saving mode.
  * @param  wakeup_event: A bit map of wake up event.
  *   This parameter can be any combination of the following values:
  *		 @arg STANDBY_WAKEUP_BY_STIMER
  *		 @arg STANDBY_WAKEUP_BY_GPIO
  *		 @arg STANDBY_WAKEUP_BY_RTC
  * @retval None
  */
void standby_wakeup_event_del(uint32_t wakeup_event)
{
	DStandbyWakeupEvent.wakeup_event &= ~wakeup_event;
}

/**
  * @brief  To make the system entering the Deep Standby power saving.
  *         The CPU, memory and part fo peripheral power is off when 
  *         entering deep standby power saving mode. The program needs 
  *         to be reload from the flash at system resume.
  * @retval None
  */
void deepstandby_ex(uint32_t sleep_duration_ms) 
{
	__asm volatile( "cpsid i" );

	/* Clear event */
	SOCPS_ClearWakeEvent();
	
	/* power mode option:  */
	SOCPS_DstandbyInit();
	
	if (DStandbyWakeupEvent.wakeup_event & STANDBY_WAKEUP_BY_STIMER) {		
		DStandbyWakeupEvent.timer_duration = sleep_duration_ms;
	}
	
	/* user setting have high priority */
	if (DStandbyWakeupEvent.wakeup_event != 0) {
		SOCPS_SetWakeEvent(DStandbyWakeupEvent.wakeup_event, ENABLE);
	}
	if (DStandbyWakeupEvent.gpio_option != WAKEUP_BY_GPIO_NONE) {
		SOCPS_WakePinsCtrl(DStandbyWakeupEvent.gpio_option);
	}
	if (DStandbyWakeupEvent.timer_duration > 0) {
		SOCPS_SET_REGUTIMER(DStandbyWakeupEvent.timer_duration, 0xFFFFFFFF);
	}	
	
	SOCPS_DeepStandby_RAM();
}

/**
  * @brief  To make the system entering the Deep Sleep power saving mode.
  *         The CPU, memory and peripheral power is off when entering
  *         deep sleep power saving mode. The program needs to be reload
  *         and all peripheral needs be re-configure when system resume.
  * @param  wakeup_event: A bit map of wake up event.
  *   This parameter can be any combination of the following values:
  *		 @arg DSLEEP_WAKEUP_BY_TIMER
  *		 @arg DSLEEP_WAKEUP_BY_GPIO
  * @param  sleep_duration: the system sleep duration in ms, only valid
  *         for DSLEEP_WAKEUP_BY_TIMER wake up event.
  * @retval None
  */
void deepsleep_ex(uint32_t wakeup_event, uint32_t sleep_duration)
{
	__asm volatile( "cpsid i" );
	SOCPS_DsleepInit();

	if (sleep_duration > 0) {
		SOCPS_SET_REGUTIMER(sleep_duration, 0xFFFFFFFF);
	}

	SOCPS_DeepSleep_RAM();
}

/**
  * @brief  get deep sleep wakeup reason.
  * @retval BIT(0): Timer, BIT(2): GPIO
  */
int deepsleep_get_bootup_reason()
{
	int Reason = SOCPS_DsleepWakeReason();
	
	if (Reason & BIT_SYSON_DSLP_WTIMER33)
		return DSLEEP_WAKEUP_BY_TIMER;
	else if (Reason & BIT_SYSON_DSLP_GPIO)
		return DSLEEP_WAKEUP_BY_GPIO;
	else
		return 0;
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
