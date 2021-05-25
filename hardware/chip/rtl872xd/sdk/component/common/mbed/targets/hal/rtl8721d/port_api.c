/** mbed Microcontroller Library
  ******************************************************************************
  * @file    port_api.c
  * @author 
  * @version V1.0.0
  * @date    2016-08-01
  * @brief   This file provides mbed API for GPIO PORT.
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
#include "port_api.h"
#include "pinmap.h"
#include "gpio_api.h"
#include "PinNames.h"
//#include "mbed_error.h"

#define GPIO_PORT_NUM 2

/** @addtogroup AmebaD_Mbed_API 
  * @{
  */

/** @defgroup MBED_GPIOPORT
 *  @brief      MBED_GPIOPORT driver modules.
 *  @{
 */

/** @defgroup MBED_GPIOPORT_Exported_Functions MBED_GPIOPORT Exported Functions
  * @{
  */

/**
  * @brief  Get GPIO port pin name
  * @param  port: PortName according to pinmux spec, this parameter can be one of the following values:
  *		@arg PortA: port number is A, has 32 pins
  *		@arg PortB: port number is B, has 32 pins
  * @param  pin_n: pin number.
  * @retval port pin name
  * @note  pin_n must be set to a value in the 0~31 range
  */
PinName port_pin(PortName port, int pin_n)
{
	return (PinName)(port << 5 | pin_n);
}

/**
  * @brief  Initializes the GPIO device port, include data direction registers.
  * @param  obj: gpio port object define in application software.
  * @param  port: PortName according to pinmux spec, this parameter can be one of the following values:
  *		@arg PortA: port A, has 32 pins
  *		@arg PortB: port B, has 32 pins
  * @param  mask: One bit one gpio pin, select one or multiple pins of the specified port.
  * @param  dir: gpio port direction, this parameter can be one of the following values:
  *		@arg PIN_INPUT: port pins are input
  *		@arg PIN_OUTPUT: port pins are output
  * @retval none
  */
void port_init(port_t *obj, PortName port, int mask, PinDirection dir) 
{
	assert_param(port < GPIO_PORT_NUM);

	obj->port      = port;
	obj->mask      = mask;

	port_dir(obj, dir);
}

/**
  * @brief  Set GPIO port pins data direction.
  * @param  obj: gpio port object define in application software.
  * @param  dir: this parameter can be one of the following values:
  *		@arg PIN_INPUT: port pins are input
  *		@arg PIN_OUTPUT: port pins are output  
  * @retval none  
  */
void port_dir(port_t *obj, PinDirection dir) 
{
	if (dir == PIN_OUTPUT) {
		GPIO_PortDirection(obj->port, obj->mask, GPIO_Mode_OUT);
	} else {
		GPIO_PortDirection(obj->port, obj->mask, GPIO_Mode_IN);
	}
}

/**
  * @brief  Configure GPIO port pins pull up/pull down.
  * @param  obj: gpio port object define in application software.
  * @param  mode: this parameter can be one of the following values:
  *		@arg PullNone: HighZ
  *		@arg PullDown: pull down
  *		@arg PullUp: pull up
  * @retval none  
  */
void port_mode(port_t *obj, PinMode mode) 
{
	uint32_t pin_idx;
	uint32_t max_num = 32;
	
	for (pin_idx = 0; pin_idx < max_num; pin_idx++) {
		if (obj->mask & BIT(pin_idx)) {
			/* PinName = (obj->port << 5 | pin_idx) */
			pin_mode((obj->port << 5 | pin_idx), mode);
		}
	}
}

/**
  * @brief  Sets value to the selected port pins.
  * @param  obj: gpio port object define in application software.
  * @param  value: One bit one gpio pin, set value to one or multiple pins of the specified port.
  * @note corresponding bit is 1, pin state set to high; corresponding bit is 0, pin state set to low
  * @retval none  
  */
void port_write(port_t *obj, int value) 
{
	GPIO_PortWrite(obj->port, obj->mask, value);
}

/**
  * @brief  Reads the specified gpio port pins.
  * @param  obj: gpio port object define in application software.
  * @retval : state of the specified gpio port pins
  * @note corresponding bit is 1, pin state is high; corresponding bit is 0, pin state is low
  */
int port_read(port_t *obj) 
{
	u32 value = GPIO_PortRead(obj->port, obj->mask);

	return value;
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
