/**
  ******************************************************************************
  * @file    stm32l496g_discovery_io.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the IO pins
  *          on STM32L496G-DISCO evaluation board.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. How To use this driver:
--------------------------
   - This driver is used to drive the IO module of the STM32L496G-DISCO evaluation
     board.
   - The STMPE811 and STMPE1600 IO expander device component driver must be included with this
     driver in order to run the IO functionalities commanded by the IO expander
     device mounted on the evaluation board.

2. Driver description:
---------------------
  + Initialization steps:
     o Initialize the IO module using the BSP_IO_Init() function. This
       function includes the MSP layer hardware resources initialization and the
       communication layer configuration to start the IO functionalities use.

  + IO functionalities use
     o The IO pin mode is configured when calling the function BSP_IO_ConfigPin(), you
       must specify the desired IO mode by choosing the "IO_ModeTypedef" parameter
       predefined value.
     o If an IO pin is used in interrupt mode, the function BSP_IO_ITGetStatus() is
       needed to get the interrupt status. To clear the IT pending bits, you should
       call the function BSP_IO_ITClear() with specifying the IO pending bit to clear.
     o The IT is handled using the corresponding external interrupt IRQ handler,
       the user IT callback treatment is implemented on the same external interrupt
       callback.
     o To get/set an IO pin combination state you can use the functions
       BSP_IO_ReadPin()/BSP_IO_WritePin() or the function BSP_IO_TogglePin() to toggle the pin
       state.

------------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "stm32l496g_discovery_io.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L496G-DISCO
  * @{
  */

/** @defgroup STM32L496G-DISCO_IO STM32L496G-DISCO IO
  * @{
  */

/* Private constants ---------------------------------------------------------*/

/** @defgroup STM32L496G-DISCO_IO_Private_Constants Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros -------------------------------------------------------------*/

/** @defgroup STM32L496G-DISCO_IO_Private_Macros Private Macros
  * @{
  */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/

/** @defgroup STM32L496G-DISCO_IO_Private_Variables Private Variables
  * @{
  */
static IO_DrvTypeDef *io1_driver;

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/

/** @defgroup STM32L496G-DISCO_IO_Private_Functions Private Functions
  * @{
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/

/** @addtogroup STM32L496G-DISCO_IO_Exported_Functions
  * @{
  */

/**
  * @brief  Initializes and configures the IO functionalities and configures all
  *         necessary hardware resources (GPIOs, clocks..).
  * @note   BSP_IO_Init() is using HAL_Delay() function to ensure that stmpe811
  *         IO Expander is correctly reset. HAL_Delay() function provides accurate
  *         delay (in milliseconds) based on variable incremented in SysTick ISR.
  *         This implies that if BSP_IO_Init() is called from a peripheral ISR process,
  *         then the SysTick interrupt must have higher priority (numerically lower)
  *         than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
  * @retval IO_OK: if all initializations are OK. Other value if error.
  */
uint8_t BSP_IO_Init(void)
{
  uint8_t ret = IO_ERROR;
  uint8_t mfxstm32l152_id = 0;

  if (io1_driver == NULL) /* Checks if MFX initialization has been already done */
  {
    mfxstm32l152_idd_drv.WakeUp(IO1_I2C_ADDRESS);

    HAL_Delay(10);

    /* Read ID and verify the IO expander is ready */
    mfxstm32l152_id = mfxstm32l152_io_drv.ReadID(IO1_I2C_ADDRESS);

    if ((mfxstm32l152_id == MFXSTM32L152_ID_1) || (mfxstm32l152_id == MFXSTM32L152_ID_2))
    {
      /* Initialize the MFX */
      io1_driver = &mfxstm32l152_io_drv;

      /* Initialize the MFX IO driver structure  */
      if (io1_driver->Init != NULL)
      {
        io1_driver->Init(IO1_I2C_ADDRESS);
        io1_driver->Start(IO1_I2C_ADDRESS, IO1_PIN_ALL >> IO1_PIN_OFFSET);

        ret = IO_OK;
      }
    }
  }
  else
  {
    ret = IO_ALREADY_INITIALIZED;
  }

  return ret;
}

/**
  * @brief  Gets the selected pins IT status.
  * @param  IO_Pin: Selected pins to check the status.
  *          This parameter can be any combination of the IO pins.
  * @retval Status of the checked IO pin(s).
  */
uint32_t BSP_IO_ITGetStatus(uint32_t IO_Pin)
{
  uint32_t status = 0;
  uint32_t io1_pin = 0;

  io1_pin = (IO_Pin & IO1_PIN_ALL) >> IO1_PIN_OFFSET;

  /* Return the MFX  Pin IT status */
  status |= (io1_driver->ITStatus(IO1_I2C_ADDRESS, io1_pin)) << IO1_PIN_OFFSET;

  return status;
}

/**
  * @brief  Clears the selected IO IT pending bit.
  * @param  IO_Pin: Selected pins to check the status.
  *          This parameter can be any combination of the IO pins.
  * @retval None
  */
void BSP_IO_ITClear(uint32_t IO_Pin)
{
  uint32_t io1_pin = 0;

  io1_pin = (IO_Pin & IO1_PIN_ALL) >> IO1_PIN_OFFSET;

  /* Clears the selected IO Expander 1 pin(s) mode */
  io1_driver->ClearIT(IO1_I2C_ADDRESS, io1_pin);

}

/**
  * @brief  Configures the IO pin(s) according to IO mode structure value.
  * @param  IO_Pin: Output pin to be set or reset.
  *          This parameter can be any combination of the IO pins.
  * @param  IO_Mode: IO pin mode to configure
  *          This parameter can be one of the following values:
  *            @arg  IO_MODE_INPUT
  *            @arg  IO_MODE_OUTPUT
  *            @arg  IO_MODE_IT_RISING_EDGE
  *            @arg  IO_MODE_IT_FALLING_EDGE
  *            @arg  IO_MODE_IT_LOW_LEVEL
  *            @arg  IO_MODE_IT_HIGH_LEVEL
  * @retval IO_OK: if all initializations are OK. Other value if error.
  */
uint8_t BSP_IO_ConfigPin(uint32_t IO_Pin, IO_ModeTypedef IO_Mode)
{
  uint32_t io1_pin = 0;

  io1_pin = (IO_Pin & IO1_PIN_ALL) >> IO1_PIN_OFFSET;


  /* Configure the selected IO Expander 1 pin(s) mode */
  io1_driver->Config(IO1_I2C_ADDRESS, io1_pin, IO_Mode);

  return IO_OK;
}

/**
  * @brief  Sets the selected pins state.
  * @param  IO_Pin: Selected pins to write.
  *          This parameter can be any combination of the IO pins.
  * @param  PinState: New pins state to write
  * @retval None
  */
void BSP_IO_WritePin(uint32_t IO_Pin, uint8_t PinState)
{
  uint32_t io1_pin = 0;

  io1_pin = (IO_Pin & IO1_PIN_ALL) >> IO1_PIN_OFFSET;

  /* Sets the IO Expander 1 selected pins state */
  io1_driver->WritePin(IO1_I2C_ADDRESS, io1_pin, PinState);

}

/**
  * @brief  Gets the selected pins current state.
  * @param  IO_Pin: Selected pins to read.
  *          This parameter can be any combination of the IO pins.
  * @retval The current pins state
  */
uint32_t BSP_IO_ReadPin(uint32_t IO_Pin)
{
  uint32_t pin_state = 0;
  uint32_t io1_pin = 0;

  io1_pin = (IO_Pin & IO1_PIN_ALL) >> IO1_PIN_OFFSET;

  /* Gets the IO Expander 1 selected pins current state */
  pin_state |= (io1_driver->ReadPin(IO1_I2C_ADDRESS, io1_pin)) << IO1_PIN_OFFSET;


  return pin_state;
}

/**
  * @brief  Toggles the selected pins state
  * @param  IO_Pin: Selected pins to toggle.
  *          This parameter can be any combination of the IO pins.
  * @retval None
  */
void BSP_IO_TogglePin(uint32_t IO_Pin)
{
  uint32_t io1_pin = 0;

  io1_pin = (IO_Pin & IO1_PIN_ALL) >> IO1_PIN_OFFSET;

  /* Toggles the IO Expander 1 selected pins state */
  if (io1_driver->ReadPin(IO1_I2C_ADDRESS, io1_pin) == RESET) /* Set */
  {
    BSP_IO_WritePin(io1_pin, GPIO_PIN_SET); /* Reset */
  }
  else
  {
    BSP_IO_WritePin(io1_pin, GPIO_PIN_RESET);  /* Set */
  }

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

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
