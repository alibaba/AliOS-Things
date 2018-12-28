/*******************************************************************************
 * @file    low_power.c
 * @author  MCD Application Team
 * @version V1.1.1
 * @date    01-June-2017
 * @brief   driver for low power
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
#include "low_power.h"
#include "lorawan_port.h"
#include "debug.h"
#include "board.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
 * \brief Flag to indicate if MCU can go to low power mode
 *        When 0, MCU is authorized to go in low power mode
 */
static uint32_t LowPower_State = 0;

/* Private function prototypes -----------------------------------------------*/


/* Exported functions
 * ---------------------------------------------------------*/

/**
 * \brief API to set flag allowing power mode
 *
 * \param [IN] enum e_LOW_POWER_State_Id_t
 */
void LowPower_Disable(e_LOW_POWER_State_Id_t state)
{
    CRITICAL_SECTION_BEGIN( );
    LowPower_State |= state;
    CRITICAL_SECTION_END( );
}

/**
 * \brief API to reset flag allowing power mode
 *
 * \param [IN] enum e_LOW_POWER_State_Id_t
 */
void LowPower_Enable(e_LOW_POWER_State_Id_t state)
{
    CRITICAL_SECTION_BEGIN( );
    LowPower_State &= ~state;
    CRITICAL_SECTION_END( );
}

/**
 * \brief API to get flag allowing power mode
 * \note When flag is 0, low power mode is allowed
 * \param [IN] state
 * \retval flag state
 */
uint32_t LowPower_GetState(void)
{
    return LowPower_State;
}

/**
 * @brief  Handle Low Power
 * @param  None
 * @retval None
 */

void BoardLowPowerHandler(void)
{
    CRITICAL_SECTION_BEGIN( );

    if (LowPower_State == 0) {
        aos_lrwan_chg_mode.enter_stop_mode();
        /* mcu dependent. to be implemented by user*/
        aos_lrwan_chg_mode.exit_stop_mode();
        RtcSetMcuWakeUpTime();
    } else {
        aos_lrwan_chg_mode.enter_sleep_mode();
    }

    CRITICAL_SECTION_END( );
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
