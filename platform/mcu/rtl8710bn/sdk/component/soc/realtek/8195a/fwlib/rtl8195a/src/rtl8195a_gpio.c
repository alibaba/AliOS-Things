/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"
#include "hal_gpio.h"
#include "rtl8195a_gpio.h"
#include "gpio_irq_api.h"

extern PHAL_GPIO_ADAPTER _pHAL_Gpio_Adapter;

/**
  * @brief  Clear the pending interrupt of a specified pin
  *
  * @param  GPIO_Pin: The data structer which contains the parameters for the GPIO Pin.
  *
  * @retval None
  */
HAL_Status
HAL_GPIO_ClearISR_8195a(
    HAL_GPIO_PIN  *GPIO_Pin
)
{
    u8 port_num;
    u8 pin_num;
    HAL_GPIO_PIN_MODE pin_mode;

    port_num = HAL_GPIO_GET_PORT_BY_NAME(GPIO_Pin->pin_name);
    pin_num = HAL_GPIO_GET_PIN_BY_NAME(GPIO_Pin->pin_name);
    pin_mode = GPIO_Pin->pin_mode;

    if ((pin_mode & HAL_GPIO_PIN_INT_MODE)==0 || (port_num != GPIO_PORT_A)) {
        DBG_GPIO_WARN("HAL_GPIO_ClearISR_8195a: This pin(%x:%x) is'nt an interrupt pin\n", GPIO_Pin->pin_name, GPIO_Pin->pin_mode);
        return HAL_ERR_PARA;
    }

    if (GPIO_Lock() != HAL_OK) {
        return HAL_BUSY;
    }

    // Clear pending interrupt before unmask it
    HAL_WRITE32(GPIO_REG_BASE, GPIO_PORTA_EOI, (1<<pin_num));

    GPIO_UnLock();
    return HAL_OK;
    
}

