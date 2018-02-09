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

/**
  * @brief  Get GPIO port pin name
  * @param  port: PortName according to pinmux spec, this parameter can be one of the following values:
  *		@arg PortA: port number is A, has 32 pins
  *		@arg PortB: port number is B, has 7 pins
  * @param  pin_n: pin number.
  * @note  pin_n must be set to a value in the 0~31 range when PortA
  * @note  pin_n must be set to a value in the 0~6 range when PortB
  * @retval none
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
  *		@arg PortB: port B, has 7 pins
  * @param  mask: One bit one gpio pin, select one or multiple pins of the specified port.
  * @param  dir: gpio port direction, this parameter can be one of the following values:
  *		@arg PIN_INPUT: port pins are input
  *		@arg PIN_OUTPUT: port pins are output
  * @retval none
  */
void port_init(port_t *obj, PortName port, int mask, PinDirection dir) 
{
	u32 i;
	RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

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
  *		@arg OpenDrain(is OpenDrain output): no pull + OUT + GPIO[gpio_bit] = 0  
  *		@arg PullDown: pull down
  *		@arg PullUp: pull up
  * @retval none  
  */
void port_mode(port_t *obj, PinMode mode) 
{
	uint32_t pin_idx;
	uint32_t max_num = 0;

	if (obj->port == PORT_A) {
		max_num = 32;
	} else {
		max_num = 7;
	}
	
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
  * @retval state of the specified gpio port pins
  * @note corresponding bit is 1, pin state is high; corresponding bit is 0, pin state is low
  */
int port_read(port_t *obj) 
{
	u32 value = GPIO_PortRead(obj->port, obj->mask);

	return value;
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
