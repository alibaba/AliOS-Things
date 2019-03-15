/******************************************************************************
 * @file    debug.c
 * @author  MCD Application Team
 * @version V1.1.1
 * @date    01-June-2017
 * @brief   debug API
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

/* Includes ------------------------------------------------------------------*/
#include "hw.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

Gpio_t g_stDbgGpio;

void DBG_Init( void )
{
#ifdef LORA_DEBUG
    GPIO_InitTypeDef gpioinitstruct;

    /* Enable the GPIO_B Clock */
    /* Clock is enabled in HW_GPIO_Init  -  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB); */

    /* Configure the GPIO pin */
    GpioInit( &g_stDbgGpio, PB_12, PIN_OUTPUT, PIN_PUSH_PULL , PIN_PULL_UP, 0);
    GpioInit( &g_stDbgGpio, PB_13, PIN_OUTPUT, PIN_PUSH_PULL , PIN_PULL_UP, 0);
    GpioInit( &g_stDbgGpio, PB_14, PIN_OUTPUT, PIN_PUSH_PULL , PIN_PULL_UP, 0);
    GpioInit( &g_stDbgGpio, PB_15, PIN_OUTPUT, PIN_PUSH_PULL , PIN_PULL_UP, 0);

    LL_APB2_GRP1_EnableClock( LL_APB2_GRP1_PERIPH_DBGMCU );
    LL_DBGMCU_EnableDBGSleepMode( );
    LL_DBGMCU_EnableDBGStopMode( );
    LL_DBGMCU_EnableDBGStandbyMode( );

#else /* DEBUG */
#if 0
    /* sw interface off*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* Clock is enabled in HW_GPIO_Init  -  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA); */
    HW_GPIO_Init(GPIOA, (GPIO_PIN_13 | GPIO_PIN_14), &GPIO_InitStructure);
    LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOA);

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_DBGMCU);
    LL_DBGMCU_DisableDBGSleepMode();
    LL_DBGMCU_DisableDBGStopMode();
    LL_DBGMCU_DisableDBGStandbyMode();
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_DBGMCU);
#endif
#endif
}

void Error_Handler( void )
{
    DBG_PRINTF( "Error_Handler\n\r" );
    while ( 1 )
    {
        ;
    }
}

void _printf_float()
{
}

/* Private functions ---------------------------------------------------------*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
