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

/** @defgroup AmebaD_Mbed_API 
  * @{
  */
  
/** @addtogroup MBED_PWM
 *  @brief      MBED_PWM driver modules.
 *  @{
 */

 /** @defgroup MBED_PWM_Exported_Types MBED_PWM Exported Types
  * @{
  */

#define PWM_TIMER	5
#define BIT_PWM_TIM_IDX_FLAG		BIT(7)
#define BIT_PWM_TIM_IDX_SHIFT	7

/**
  * @brief  Table elements express the pin to PWM channel number, they are:
  *           {pinName, km0_pin2chan, km4_pin2chan}
  */
const u32 pin2chan[19][3] = {
	{PA_12,	0,	0},
	{PA_13,	1,	1},
	{PA_23,	2,	2},
	{PA_24,	3,	3},
	{PA_25,	4,	4},
	{PA_26,	5,	5},
	{PA_28,	0,	6},
	{PA_30,	1,	7},
	{PB_4,	2,	8},
	{PB_5,	3,	9},
	{PB_18,	4,	10},
	{PB_19,	5,	11},
	{PB_20,	0,	12},
	{PB_21,	1,	13},
	{PB_22,	2,	14},
	{PB_23,	3,	15},
	{PB_24,	4,	16},
	{PB_25,	5,	17},
	{PB_7,	5,	17}
};

u8 timer5_start = 0;
u8 timer05_start = 0;
u8 km4_ch_start[18] = {0};
u8 km0_ch_start[6] = {0};
u8 prescaler = 0;
RTIM_TypeDef* PWM_TIM[2] = {TIMM05, TIM5};
/** 
  * @}
  */

 /** @defgroup MBED_PWM_Exported_Functions MBED_PWM Exported Functions
  * @{
  */

/**
  * @brief  Return PWM channel number according to pin name.
  * @param  pin: The pin to be told.
  * @retval value: PWM channel number corresponding to the pin.  
  */
u32 pwmout_pin2chan(PinName pin)
{
	int i = 0;
	for(;i < 19;i++){
		if(pin2chan[i][0] == pin){
			return (pin2chan[i][2] | BIT_PWM_TIM_IDX_FLAG);
		}
	}
	return NC;
}

/**
  * @brief  Initializes the TIM5/TIMM05 device, include TIM5/TIMM05 registers and function.
  * @param  obj: PWM object define in application software.
  * @retval none  
  */
void pwmout_timer5_init(pwmout_t* obj)
{
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;
	RTIM_TimeBaseStructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_Idx = PWM_TIMER;

	RTIM_TimeBaseInit(PWM_TIM[obj->pwm_idx >>BIT_PWM_TIM_IDX_SHIFT], &TIM_InitStruct, TIMER5_IRQ, NULL, (u32)&TIM_InitStruct);
	RTIM_Cmd(PWM_TIM[obj->pwm_idx >>BIT_PWM_TIM_IDX_SHIFT], ENABLE);

	if (obj->pwm_idx & BIT_PWM_TIM_IDX_FLAG) {	
		timer5_start = 1;
	} else {
		timer05_start = 1;
	}
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
	uint8_t pwm_tim_idx;
	
	pwm_chan = pwmout_pin2chan(pin);
	if(pwm_chan == NC)
		DBG_8195A("PinName error: pwm channel of PinName doesn't exist!\n");

	obj->pwm_idx = pwm_chan;
	obj->period = 0x10000 * (prescaler + 1) / 40;
	obj->pulse = 0x1000 * (prescaler + 1) / 40;

	pwm_tim_idx = obj->pwm_idx >> BIT_PWM_TIM_IDX_SHIFT;
	pwm_chan = pwm_chan & (~BIT_PWM_TIM_IDX_FLAG);

	if(pwm_tim_idx && (!timer5_start)) {
		pwmout_timer5_init(obj);
	}else if ((!pwm_tim_idx) && (!timer05_start)) {
		pwmout_timer5_init(obj);
	}

	RTIM_CCStructInit(&TIM_CCInitStruct);
	RTIM_CCxInit(PWM_TIM[pwm_tim_idx], &TIM_CCInitStruct, pwm_chan);
	RTIM_CCxCmd(PWM_TIM[pwm_tim_idx], pwm_chan, TIM_CCx_Enable);

	if ((obj->pwm_idx & BIT_PWM_TIM_IDX_FLAG)) {
		Pinmux_Config(pin, PINMUX_FUNCTION_PWM_HS);
	} else {
		Pinmux_Config(pin, PINMUX_FUNCTION_PWM_LP);
	}
	
	if (obj->pwm_idx & BIT_PWM_TIM_IDX_FLAG) {	
		km4_ch_start[pwm_chan] = 1;
	} else {
		km0_ch_start[pwm_chan] = 1;
	}

}


/**
  * @brief  Deinitializes the PWM device of the specified channel.
  * @param  obj: PWM object define in application software.
  * @retval none
  * @note  If all channels are released, TIM5/TIMM05 will also be disabled.
  */
void pwmout_free(pwmout_t* obj) 
{
	/* disable pwm channel */
	uint8_t pwm_chan = obj->pwm_idx & (~BIT_PWM_TIM_IDX_FLAG);
	uint8_t pwm_tim_idx = obj->pwm_idx >> BIT_PWM_TIM_IDX_SHIFT;;
	
	
	if(pwm_tim_idx && km4_ch_start[pwm_chan]){
		km4_ch_start[pwm_chan] = 0;
		RTIM_CCxCmd(PWM_TIM[pwm_tim_idx], pwm_chan, TIM_CCx_Disable);
		
		/* stop timer5 if no pwm channels starts */
		for(pwm_chan = 0;pwm_chan < 18;pwm_chan++){
			if(km4_ch_start[pwm_chan]){
				return;
			}
		}
		
		RTIM_Cmd(PWM_TIM[pwm_tim_idx], DISABLE);
		timer5_start = 0;
	} else if (!(pwm_tim_idx) && km0_ch_start[pwm_chan]) {
		km0_ch_start[pwm_chan] = 0;
		RTIM_CCxCmd(PWM_TIM[pwm_tim_idx], pwm_chan, TIM_CCx_Disable);
		/* stop timer05 if no pwm channels starts */
		for(pwm_chan = 0;pwm_chan < 6;pwm_chan++){
			if(km0_ch_start[pwm_chan]){
				return;
			}
		}

		RTIM_Cmd(PWM_TIM[pwm_tim_idx], DISABLE);
		timer05_start = 0;
	}

	return;
}

/**
  * @brief  Set the duty cycle of the specified channel.
  * @param  obj: PWM object define in application software.
  * @param  percent: The duty cycle value to be set.
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
	
	RTIM_CCRxSet(PWM_TIM[obj->pwm_idx >>BIT_PWM_TIM_IDX_SHIFT], ccrx, obj->pwm_idx & (~BIT_PWM_TIM_IDX_FLAG));
}

/**
  * @brief  Get the duty cycle value of the specified channel.
  * @param  obj: PWM object define in application software.
  * @retval : the duty cycle value of the specified channel.
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
		RTIM_PrescalerConfig(PWM_TIM[obj->pwm_idx >>BIT_PWM_TIM_IDX_SHIFT], prescaler, TIM_PSCReloadMode_Update);
	}

	obj->period = us;
	arr = us * 40 / (prescaler + 1) - 1;
	
	RTIM_ChangePeriod(PWM_TIM[obj->pwm_idx >>BIT_PWM_TIM_IDX_SHIFT], arr);
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
	RTIM_CCRxSet(PWM_TIM[obj->pwm_idx >>BIT_PWM_TIM_IDX_SHIFT], ccrx, obj->pwm_idx & (~BIT_PWM_TIM_IDX_FLAG)); 
}

/**
  * @brief  Enable the specified channel to output PWM.
  * @param  obj: PWM object define in application software.
  * @retval none
  */
void pwmout_start(pwmout_t* obj) 
{
	RTIM_CCxCmd(PWM_TIM[obj->pwm_idx >>BIT_PWM_TIM_IDX_SHIFT], obj->pwm_idx & (~BIT_PWM_TIM_IDX_FLAG), TIM_CCx_Enable);
}

/**
  * @brief  Disable the specified channel to output PWM.
  * @param  obj: PWM object define in application software.
  * @retval none
  */
void pwmout_stop(pwmout_t* obj) 
{
	RTIM_CCxCmd(PWM_TIM[obj->pwm_idx >>BIT_PWM_TIM_IDX_SHIFT], obj->pwm_idx & (~BIT_PWM_TIM_IDX_FLAG), TIM_CCx_Disable);
}
/** 
  * @}
  */

/** 
  * @}
  */

/** 
  * @}
  */

