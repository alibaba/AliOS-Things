/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Bleeper board GPIO driver implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
/******************************************************************************
 * @file    hw_gpio.h
 * @author  MCD Application Team
 * @version V1.1.1
 * @date    01-June-2017
 * @brief   Header for driver hw_rtc.c module
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_GPIO_H__
#define __HW_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_ll_gpio.h"

/*
 * Force the include of HAL GPIO in order to benefit from enum and structure
 * definition which are used in LoRa modules
 * Note that the code size is slighty increased by about 40 bytes on classA application because
 * "Pin" of structure "GPIO_InitTypeDef" is not used in our HW_GPIO_xxx() implementation
 */
#include "stm32l0xx_hal_gpio.h"
#include "stm32l0xx_hal_gpio_ex.h"
#include "system/gpio.h"

/* Exported types ------------------------------------------------------------*/

typedef void ( GpioIrqHandler )(void);

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


/**
 * @brief Initializes the given GPIO object
 *
 * @param  GPIOx: where x can be (A..E and H)
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @param [IN] initStruct  GPIO_InitTypeDef intit structure
 * @retval None
 */
void HW_GPIO_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_InitTypeDef *initStruct);

/**
 * @brief  De-initializes the GPIOx peripheral registers to their default reset values.
 * @param  GPIOx: where x can be (A..E and H) to select the GPIO peripheral for STM32L0XX family devices.
 *                Note that GPIOE is not available on all devices.
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @retval None
 * @note this is an implementation of HAL_GPIO_DeInit() using LL
 */
void HW_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);

/**
 * @brief  This function handles EXTI interrupt request.
 * @param  GPIO_Pin: Specifies the pins connected to the EXTI line.
 * @retval None
 * @note this is an implementation of HAL_GPIO_EXTI_IRQHandler() using LL
 */
void HW_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);

/**
 * @brief Records the interrupt handler for the GPIO  object
 *
 * @param  GPIOx: where x can be (A..E and H)
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @param [IN] prio       NVIC priority (0 is highest)
 * @param [IN] irqHandler  points to the  function to execute
 * @retval None
 */
void HW_GPIO_SetIrq(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t prio,  GpioIrqHandler *irqHandler);

/**
 * @brief Execute the interrupt from the object
 *
 * @param  GPIOx: where x can be (A..E and H)
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @retval None
 */
void HW_GPIO_IrqHandler(uint16_t GPIO_Pin);

/**
 * @brief Writes the given value to the GPIO output
 *
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @param [IN] value New GPIO output value
 * @retval None
 */
__STATIC_INLINE void HW_GPIO_Write(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,  uint32_t value)
{
  /*
   * Function HAL_GPIO_WritePin is inlined using LL Driver. It saves code size
   * as the value to set is known at compile time most of the time
   */
  if (value != RESET)
  {
    LL_GPIO_SetOutputPin(GPIOx, GPIO_Pin);
  }
  else
  {
    LL_GPIO_ResetOutputPin(GPIOx, GPIO_Pin);
  }
}

/**
 * @brief Reads the current GPIO input value
 *
 * @param  GPIOx: where x can be (A..E and H)
 * @param  GPIO_Pin: specifies the port bit to be written.
 *                   This parameter can be one of GPIO_PIN_x where x can be (0..15).
 *                   All port bits are not necessarily available on all GPIOs.
 * @retval value   Current GPIO input value
 */
uint32_t HW_GPIO_Read(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

/**
 * @brief  Toggles the specified GPIO pins.
 * @param  GPIOx: Where x can be (A..E and H) to select the GPIO peripheral for STM32L0xx family devices.
 *                Note that GPIOE is not available on all devices.
 *                All port bits are not necessarily available on all GPIOs.
 * @param  GPIO_Pin: Specifies the pins to be toggled.
 * @retval None
 */
__STATIC_INLINE void HW_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  LL_GPIO_TogglePin(GPIOx, PinMask);
}

#ifdef __cplusplus
}
#endif

#endif /* __HW_GPIO_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

