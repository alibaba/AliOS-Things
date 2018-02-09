 /** mbed Microcontroller Library
  ******************************************************************************
  * @file	 pwmout_api.c
  * @author 
  * @version V1.0.0
  * @date	 2016-08-01
  * @brief	 This file provides mbed API for PWM.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */


#include "device.h"
#include "objects.h"
#include "pinmap.h"

#include "pwmout_api.h"

#define PWM_TIMER	5
u32 pin2chan[13][2] = {
	{PA_23, 0},
	{PA_15, 1},
	{PA_0  , 2},
	{PA_30, 3},
	{PA_13, 4},
	{PA_22, 5},
	{PA_14, 0},
	{PA_16, 1},
	{PA_17, 2},
	{PA_12, 3},
	{PA_5  , 4},
	{PA_21, 3},
	{PA_29, 4}
};

u8 timer5_start = 0;
u8 ch_start[6] = {0};
u8 prescaler = 0;

/**
  * @brief  Return PWM channel number according to pin name.
  * @param  pin: The pin to be told.
  * @retval value: PWM channel number corresponding to the pin.  
  */
u32 pwmout_pin2chan(PinName pin)
{
	int i = 0;
	for(;i < 13;i++){
		if(pin2chan[i][0] == pin){
			return pin2chan[i][1];
		}
	}
	return NC;
}

/**
  * @brief  Initializes the TIM5 device, include TIM5 registers and function.
  * @param  obj: PWM object define in application software.
  * @retval none  
  */
void pwmout_timer5_init(pwmout_t* obj)
{
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;
	RTIM_TimeBaseStructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_Idx = PWM_TIMER;

	RTIM_TimeBaseInit(TIM5, &TIM_InitStruct, TIMER5_IRQ, NULL, (u32)&TIM_InitStruct);
	RTIM_Cmd(TIM5, ENABLE);
	
	timer5_start = 1;
}

/**
  * @brief  Initializes the PWM function/registers of the specified pin with default parameters.
  * @param  obj: PWM object define in application software.
  * @param  pin: the pinname of specified channel to be set.
  * @retval none
  * @note  
  *             - default period: 1638us
  *             - default pulse width: 102us
  *             - default duty cycle: 6.227%
  */
void pwmout_init(pwmout_t* obj, PinName pin) 
{
	u32 pwm_chan;
	TIM_CCInitTypeDef TIM_CCInitStruct;
	
	pwm_chan = pwmout_pin2chan(pin);
	if(pwm_chan == NC)
		DBG_8195A("PinName error: pwm channel of PinName doesn't exist!\n");

	if(!timer5_start)
		pwmout_timer5_init(obj);

	RTIM_CCStructInit(&TIM_CCInitStruct);
	RTIM_CCxInit(TIM5, &TIM_CCInitStruct, pwm_chan);
	RTIM_CCxCmd(TIM5, pwm_chan, TIM_CCx_Enable);

	/* loguart may close here */
	Pinmux_Config(pin, PINMUX_FUNCTION_PWM);
	
	ch_start[pwm_chan] = 1;
	obj->pwm_idx = pwm_chan;
	obj->period = 0x10000 * (prescaler + 1) / 40;
	obj->pulse = 0x1000 * (prescaler + 1) / 40;

}


/**
  * @brief  Deinitializes the PWM device of the specified channel.
  * @param  obj: PWM object define in application software.
  * @retval none
  * @note  If all channels are released, TIM5 will also be disabled.
  */
void pwmout_free(pwmout_t* obj) 
{
	/* disable pwm channel */
	int i = obj->pwm_idx;
	
	if(ch_start[i]){
		ch_start[i] = 0;
		RTIM_CCxCmd(TIM5, obj->pwm_idx, TIM_CCx_Disable);
	}

	/* stop timer5 if no pwm channels starts */
	for(i = 0;i < 6;i++){
		if(ch_start[i]){
			return;
		}
	}
	RTIM_Cmd(TIM5, DISABLE);
	timer5_start = 0;
	
	return;
}

/**
  * @brief  Set the duty cycle of the specified channel.
  * @param  obj: PWM object define in application software.
  * @param  value: The duty cycle value to be set.
  * @retval none
  */
void pwmout_write(pwmout_t* obj, float percent) //write duty-cycle
{
	u32 ccrx;
	
	if (percent < (float)0.0) {
	    percent = 0.0;
	} 
	else if (percent > (float)1.0) {
	    percent = 1.0;
	}

	obj->pulse = (percent * obj->period);	
	
	ccrx = (u32)(obj->pulse  * 40 / (prescaler + 1)) & 0x0000ffff;
	
	RTIM_CCRxSet(TIM5, ccrx, obj->pwm_idx);	
}

/**
  * @brief  Get the duty cycle value of the specified channel.
  * @param  obj: PWM object define in application software.
  * @retval value: the duty cycle value of the specified channel.
  */
float pwmout_read(pwmout_t* obj) //read duty-cycle
{
	float value = 0;
	if (obj->period > 0) {
	    value = (float)obj->pulse / (float)obj->period;
	}
	return ((value > 1.0) ? (1.0) : (value));
}

/**
  * @brief  Set the period of the specified channel in seconds.
  * @param  obj: PWM object define in application software.
  * @param  seconds: The period value to be set in seconds.
  * @retval none
  */
void pwmout_period(pwmout_t* obj, float seconds) 
{
	pwmout_period_us(obj, (int)(seconds * 1000000.0f));
}

/**
  * @brief  Set the period of the specified channel in millseconds.
  * @param  obj: PWM object define in application software.
  * @param  ms: The period value to be set in millseconds.
  * @retval none
  */
void pwmout_period_ms(pwmout_t* obj, int ms) 
{
	pwmout_period_us(obj, (int)(ms * 1000));
}

/**
  * @brief  Set the period of the specified channel in microseconds.
  * @param  obj: PWM object define in application software.
  * @param  us: The period value to be set in microseconds.
  * @retval none
  */
void pwmout_period_us(pwmout_t* obj, int us) 
{
	u32 arr;
	float dc = pwmout_read(obj);
	u32 tmp = us * 40 / (prescaler + 1);

	if(tmp > 0x10000){
		prescaler = us * 40 / 0x10000;
		RTIM_PrescalerConfig(TIM5, prescaler, TIM_PSCReloadMode_Update);
	}

	obj->period = us;
	arr = us * 40 / (prescaler + 1) - 1;
	
	RTIM_ChangePeriod(TIM5, arr);
	pwmout_write(obj, dc);
}

/**
  * @brief  Set the pulse width of the specified channel in seconds.
  * @param  obj: PWM object define in application software.
  * @param  seconds: The pulse width value to be set in seconds.
  * @retval none
  */
void pwmout_pulsewidth(pwmout_t* obj, float seconds) 
{
	pwmout_pulsewidth_us(obj, (int)(seconds * 1000000.0f));
}

/**
  * @brief  Set the pulse width of the specified channel in milliseconds.
  * @param  obj: PWM object define in application software.
  * @param  ms: The pulse width value to be set in milliseconds.
  * @retval none
  */
void pwmout_pulsewidth_ms(pwmout_t* obj, int ms) 
{
	pwmout_pulsewidth_us(obj, ms * 1000);
}

/**
  * @brief  Set the pulse width of the specified channel in microseconds.
  * @param  obj: PWM object define in application software.
  * @param  us: The pulse width value to be set in microseconds.
  * @retval none
  */
void pwmout_pulsewidth_us(pwmout_t* obj, int us) 
{
	u32 ccrx;
	
	obj->pulse = (float)us;
	ccrx = (u32)(obj->pulse  * 40 / (prescaler + 1)) & 0x0000ffff;
	RTIM_CCRxSet(TIM5, ccrx, obj->pwm_idx);	
}

void pwmout_start(pwmout_t* obj) 
{
	RTIM_CCxCmd(TIM5, obj->pwm_idx, TIM_CCx_Enable);
}

void pwmout_stop(pwmout_t* obj) 
{
	RTIM_CCxCmd(TIM5, obj->pwm_idx, TIM_CCx_Disable);
}

