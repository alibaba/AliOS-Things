/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2015, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */
 
#ifndef MBED_GPIO_IRQ_EX_API_H
#define MBED_GPIO_IRQ_EX_API_H

#include "device.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup gpio_irq_ex_api GPIO_IRQ_EX
 *  @ingroup    hal
 *  @brief      gpio IRQ extented functions
 *  @{
 */
 
///@name Ameba Common 
///@{
typedef enum {
    IRQ_LOW = 3,
    IRQ_HIGH = 4,
    IRQ_FALL_RISE = 5   // dual edge trigger, available for 8195B only
} gpio_irq_event_ex;

/**
  * @brief  Deinitializes the GPIO device interrupt mode, include mode/trigger/polarity registers.
  * @param  obj: gpio irq object define in application software.
  * @retval none
  */
void gpio_irq_deinit(gpio_irq_t *obj);

/**
  * @brief  Sets pull type to the selected interrupt pin.
  * @param  obj: gpio irq object define in application software.
  * @param  pull_type: this parameter can be one of the following values:
  *		@arg PullNone: HighZ, user can input high or low use this pin
  *		@arg OpenDrain(is OpenDrain output): no pull + OUT + GPIO[gpio_bit] = 0  
  *		@arg PullDown: pull down
  *		@arg PullUp: pull up 
  * @retval none  
  */
void gpio_irq_pull_ctrl(gpio_irq_t *obj, PinMode pull_type);
///@}

#if (defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1))  ||  (defined(CONFIG_PLATFORM_8195BLP) && (CONFIG_PLATFORM_8195BLP == 1))
///@name AmebaPro Only 
///@{
/** 
 *  @brief To enables or disable the debounce function of the given GPIO IRQ pin.
 *         The debounce resource(circuit) is limited, not all GPIO pin 
 *         can has debounce function.
 *
 *  @param[in]  pgpio_irq_adapter  The GPIO IRQ pin adapter.
 *  @param[in]  debounce_us  The time filter for the debounce, in micro-second. 
 *                           But the time resolution is 31.25us (1/32K) and the
 *                           maximum time is 512 ms.
 * @param[in]  enable: this parameter can be one of the following values:
 *     @arg 0 disable gpio debounce interrupt
 *     @arg 1 enable gpio debounce interrupt
 *  @return     0:  Setting Succeed.
 *  @return     -1:  Setting Fail.
 */
int gpio_irq_debounce_set (gpio_irq_t *obj, uint32_t debounce_us, u8 enable);
///@}
#endif //CONFIG_PLATFORM_8195BHP || CONFIG_PLATFORM_8195BLP


/*\@}*/

#ifdef __cplusplus
}
#endif

#endif  // end of #ifndef MBED_GPIO_IRQ_EX_API_H