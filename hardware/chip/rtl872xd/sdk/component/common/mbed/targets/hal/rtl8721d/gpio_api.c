/** mbed Microcontroller Library
  ******************************************************************************
  * @file    gpio_api.c
  * @author 
  * @version V1.0.0
  * @date    2016-08-01
  * @brief   This file provides mbed API for GPIO.
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
#include "pinmap.h"

#include "gpio_api.h"

/** @addtogroup AmebaD_Mbed_API 
  * @{
  */

/** @defgroup MBED_GPIO
 *  @brief      MBED_GPIO driver modules.
 *  @{
 */

/** @defgroup MBED_GPIO_Exported_Functions MBED_GPIO Exported Functions
  * @{
  */

/**
  * @brief  Set the given pin as GPIO.
  * @param  pin: PinName according to pinmux spec.
  * @retval : The given pin with GPIO function  
  */
uint32_t gpio_set(PinName pin) 
{
	u32 ip_pin;

	assert_param(pin != (PinName)NC);
	pin_function(pin, 0);
	
	ip_pin = pin;

	return ip_pin;
}

/**
  * @brief  Initializes the GPIO device, include mode/direction/pull control registers.
  * @param  obj: gpio object define in application software.
  * @param  pin: PinName according to pinmux spec.
  * @retval none  
  */
void gpio_init(gpio_t *obj, PinName pin) 
{
	GPIO_InitTypeDef  GPIO_InitStruct;
		
	if (pin == (PinName)NC)
		return;

	obj->pin = pin;
	
	GPIO_InitStruct.GPIO_Pin = obj->pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(&GPIO_InitStruct);
}

/**
  * @brief  Set GPIO mode.
  * @param  obj: gpio object define in application software.
  * @param  mode: this parameter can be one of the following values:
  *		@arg PullNone: HighZ, user can input high or low use this pin
  *		@arg PullDown: pull down
  *		@arg PullUp: pull up
  * @retval none  
  */
void gpio_mode(gpio_t *obj, PinMode mode) 
{
	u32 GPIO_PuPd;

	switch (mode) {
		case PullNone:/* No driver -> Input & High Impendance */
			GPIO_PuPd = GPIO_PuPd_NOPULL;
			//GPIO_Direction(obj->pin, GPIO_Mode_IN);
		break;
		
		case PullDown:
			GPIO_PuPd = GPIO_PuPd_DOWN;
		break;

		case PullUp:
			GPIO_PuPd = GPIO_PuPd_UP;
		break;

		default:
			GPIO_PuPd = GPIO_PuPd_NOPULL;
		break;
	}

	PAD_PullCtrl(obj->pin, GPIO_PuPd);
}

/**
  * @brief  Set GPIO direction.
  * @param  obj: gpio object define in application software.
  * @param  direction: this parameter can be one of the following values:
  *		@arg PIN_INPUT: this pin is input
  *		@arg PIN_OUTPUT: this pin is output  
  * @retval none  
  */
void gpio_dir(gpio_t *obj, PinDirection direction)
{
	assert_param(obj->pin != (PinName)NC);
	
	if (direction == PIN_OUTPUT) {
		GPIO_Direction(obj->pin, GPIO_Mode_OUT);
	} else {
		GPIO_Direction(obj->pin, GPIO_Mode_IN);
	}
}

/**
  * @brief  Set GPIO direction.
  * @param  obj: gpio object define in application software.
  * @param  direction: this parameter can be one of the following values:
  *		@arg PIN_INPUT: this pin is input
  *		@arg PIN_OUTPUT: this pin is output  
  * @retval none  
  */
void gpio_change_dir(gpio_t *obj, PinDirection direction)
{
	gpio_dir(obj, direction);
}

/**
  * @brief  Sets value to the selected output port pin.
  * @param  obj: gpio object define in application software.
  * @param  value: specifies the value to be written to the selected pin
  * 	This parameter can be one of the following values:
  *		@arg 0: Pin state set to low
  *		@arg 1: Pin state set to high 
  * @retval none  
  */
void gpio_write(gpio_t *obj, int value) 
{
	assert_param(obj->pin != (PinName)NC);

	GPIO_WriteBit(obj->pin, value);
}

/**
  * @brief  Sets value to the selected output port pin.
  * @param  obj: gpio object define in application software.
  * @param  value: specifies the value to be written to the selected pin
  * 	This parameter can be one of the following values:
  *		@arg 0: Pin state set to low
  *		@arg 1: Pin state set to high 
  * @retval none  
  */
void gpio_direct_write(gpio_t *obj, BOOL value)
{
	gpio_write(obj, value);
}

/**
  * @brief  Reads the specified gpio port pin.
  * @param  obj: gpio object define in application software.
  * @retval state of the specified gpio port pin
  *          - 1: pin state is high
  *          - 0: pin state is low
  */
int gpio_read(gpio_t *obj)
{
	assert_param(obj->pin != (PinName)NC);

	return GPIO_ReadDataBit(obj->pin);
}

/**
  * @brief  Sets pull type to the selected pin.
  * @param  obj: gpio object define in application software.
  * @param  pull_type: this parameter can be one of the following values:
  *		@arg PullNone: HighZ, user can input high or low use this pin
  *		@arg PullDown: pull down
  *		@arg PullUp: pull up 
  * @retval none  
  */
void gpio_pull_ctrl(gpio_t *obj, PinMode pull_type)
{
	u32 GPIO_PuPd;

	switch (pull_type) {
		case PullNone:/* No driver -> Input & High Impendance */
			GPIO_PuPd = GPIO_PuPd_NOPULL;
		break;

		case PullDown:
			GPIO_PuPd = GPIO_PuPd_DOWN;
		break;

		case PullUp:
			GPIO_PuPd = GPIO_PuPd_UP;
		break;

		default:
			GPIO_PuPd = GPIO_PuPd_NOPULL;
		break;
	}

	PAD_PullCtrl(obj->pin, GPIO_PuPd);
}

/**
  * @brief  Deinitializes the GPIO device, include mode/direction/pull control registers.
  * @param  obj: gpio object define in application software.
  * @retval none  
  */
void gpio_deinit(gpio_t *obj)
{
	GPIO_DeInit(obj->pin);
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
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
