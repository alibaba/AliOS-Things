/**
  ******************************************************************************
  * @file    stm32l496g_discovery.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to manage Leds,
  *          push-button and joystick of STM32L496G-Discovery board (MB1261)
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

/* Includes ------------------------------------------------------------------*/
#include "stm32l496g_discovery.h"
#include "stm32l496g_discovery_io.h"
#include "stm32l496g_discovery_lcd.h"

/** @addtogroup BSP
  * @{
  */

/** @defgroup STM32L496G_DISCOVERY STM32L496G-DISCOVERY
  * @{
  */

/** @defgroup STM32L496G_DISCOVERY_Common STM32L496G-DISCOVERY Common
  * @{
  */

/** @defgroup STM32L496G_DISCOVERY_Private_TypesDefinitions Private Types Definitions
  * @brief This file provides firmware functions to manage Leds, push-buttons,
  *        COM ports, SD card on SPI and temperature sensor (TS751) available on
  *        STM32L496G-DISCOVERY discoveryuation board from STMicroelectronics.
  * @{
  */

/**
  * @}
  */

/** @defgroup STM32L496G_DISCOVERY_Private_Defines Private Defines
  * @{
  */

/**
 * @brief STM32L496G DISCOVERY BSP Driver version number
   */
#define __STM32L496G_DISCOVERY_BSP_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __STM32L496G_DISCOVERY_BSP_VERSION_SUB1   (0x01) /*!< [23:16] sub1 version */
#define __STM32L496G_DISCOVERY_BSP_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __STM32L496G_DISCOVERY_BSP_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __STM32L496G_DISCOVERY_BSP_VERSION            ((__STM32L496G_DISCOVERY_BSP_VERSION_MAIN << 24)\
                                                      |(__STM32L496G_DISCOVERY_BSP_VERSION_SUB1 << 16)\
                                                      |(__STM32L496G_DISCOVERY_BSP_VERSION_SUB2 << 8 )\
                                                      |(__STM32L496G_DISCOVERY_BSP_VERSION_RC))
/**
  * @}
  */


/** @defgroup STM32L496G_DISCOVERY_Private_Macros Private Macros
  * @{
  */

/**
  * @}
  */


/** @defgroup STM32L496G_DISCOVERY_Exported_Variables Exported Variables
  * @{
  */

/**
 * @brief LED variables
 */
const uint16_t  LED_PIN[LEDn] =                 {LED1_PIN,
                                                 LED2_PIN
                                                };

/**
 * @brief JOYSTICK variables
 */
GPIO_TypeDef *JOY_PORT[JOYn] =  {SEL_JOY_GPIO_PORT,
                                 LEFT_JOY_GPIO_PORT,
                                 RIGHT_JOY_GPIO_PORT,
                                 DOWN_JOY_GPIO_PORT,
                                 UP_JOY_GPIO_PORT
                                };

const uint16_t JOY_PIN[JOYn] =   {SEL_JOY_PIN,
                                  LEFT_JOY_PIN,
                                  RIGHT_JOY_PIN,
                                  DOWN_JOY_PIN,
                                  UP_JOY_PIN
                                 };

const uint8_t JOY_IRQn[JOYn] =   {SEL_JOY_EXTI_IRQn,
                                  LEFT_JOY_EXTI_IRQn,
                                  RIGHT_JOY_EXTI_IRQn,
                                  DOWN_JOY_EXTI_IRQn,
                                  UP_JOY_EXTI_IRQn
                                 };


/**
 * @brief COM port variables
 */
#if defined(HAL_UART_MODULE_ENABLED)
USART_TypeDef* COM_USART[COMn] = {DISCOVERY_COM1};
GPIO_TypeDef* COM_TX_PORT[COMn] = {DISCOVERY_COM1_TX_GPIO_PORT};
GPIO_TypeDef* COM_RX_PORT[COMn] = {DISCOVERY_COM1_RX_GPIO_PORT};
const uint16_t COM_TX_PIN[COMn] = {DISCOVERY_COM1_TX_PIN};
const uint16_t COM_RX_PIN[COMn] = {DISCOVERY_COM1_RX_PIN};
const uint16_t COM_TX_AF[COMn] = {DISCOVERY_COM1_TX_AF};
const uint16_t COM_RX_AF[COMn] = {DISCOVERY_COM1_RX_AF};
#endif /* HAL_UART_MODULE_ENABLED */


/**
 * @brief BUS variables
 */
#if defined(HAL_I2C_MODULE_ENABLED)
uint32_t I2c1Timeout = DISCOVERY_I2C2_TIMEOUT_MAX;  /*<! Value of Timeout when I2C1 communication fails */
uint32_t I2c2Timeout = DISCOVERY_I2C2_TIMEOUT_MAX;  /*<! Value of Timeout when I2C2 communication fails */
uint32_t ts_io_init = 0;
static I2C_HandleTypeDef I2c2Handle = {0};
static I2C_HandleTypeDef hI2cTSHandler = {0}; /* Touch Screen (Capacitive Touch Panel) Handler */
static I2C_HandleTypeDef hI2cAudioHandler = {0};
static I2C_HandleTypeDef hI2cCameraHandler = {0} ;
#endif /* HAL_I2C_MODULE_ENABLED */

/**
  * @}
  */

/** @defgroup STM32L496G_DISCOVERY_Private_FunctionPrototypes Private Functions
  * @{
  */
/**************************** Bus functions ************************************/
/* I2C2 bus function */
#if defined(HAL_I2C_MODULE_ENABLED)
static void               I2C2_MspInit(I2C_HandleTypeDef *hi2c);
static void               I2C2_MspDeInit(I2C_HandleTypeDef *hi2c);
static void               I2C2_WriteData(uint16_t Addr, uint16_t Reg, uint16_t RegSize, uint8_t Value);
static HAL_StatusTypeDef  I2C2_WriteBuffer(uint16_t Addr, uint16_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length);
static uint8_t            I2C2_ReadData(uint16_t Addr, uint16_t Reg, uint16_t RegSize);
static HAL_StatusTypeDef  I2C2_ReadBuffer(uint16_t Addr, uint16_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length);
static void               I2C2_Error(void);

#endif/* HAL_I2C_MODULE_ENABLED */


/**************************** Link functions ***********************************/
#if defined(HAL_I2C_MODULE_ENABLED)

static void     FMC_BANK1_WriteData(uint16_t Data);
static void     FMC_BANK1_WriteReg(uint8_t Reg);
static uint16_t FMC_BANK1_ReadData(void);
static void     FMC_BANK1_Init(void);


/* LCD IO functions */
void            LCD_IO_Init(void);
void            LCD_IO_WriteData(uint16_t RegValue);
void            LCD_IO_WriteReg(uint8_t Reg);
void            LCD_IO_WriteMultipleData(uint16_t *pData, uint32_t Size);
uint16_t        LCD_IO_ReadData(void);
void            LCD_IO_Delay(uint32_t Delay);

/* Link functions for Audio Codec peripheral */      /* TBC */
void                      AUDIO_IO_Init(void);
void                      AUDIO_IO_DeInit(void);
void                      AUDIO_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
uint8_t                   AUDIO_IO_Read(uint8_t Addr, uint8_t Reg);
void                      AUDIO_IO_Delay(uint32_t delay);

/* TouchScreen (TS) IO functions */
void     TS_IO_Init(void);
void     TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
uint8_t  TS_IO_Read(uint8_t Addr, uint8_t Reg);
uint16_t TS_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);
void     TS_IO_WriteMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);
void     TS_IO_Delay(uint32_t Delay);

/* CAMERA IO functions */
void            CAMERA_IO_Init(void);
void            CAMERA_Delay(uint32_t Delay);
void            CAMERA_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
uint8_t         CAMERA_IO_Read(uint8_t Addr, uint8_t Reg);

#endif/* HAL_I2C_MODULE_ENABLED */


#if defined(HAL_I2C_MODULE_ENABLED)
/*  Link functions IOExpander */
void                      IOE_Init(void);
void                      IOE_ITConfig(void);
void                      IOE_Delay(uint32_t Delay);
void                      IOE_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
uint8_t                   IOE_Read(uint8_t Addr, uint8_t Reg);
uint16_t                  IOE_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);

/* Link functions for IDD measurment */
void                      MFX_IO_Init(void);
void                      MFX_IO_DeInit(void);
void                      MFX_IO_ITConfig(void);
void                      MFX_IO_EnableWakeupPin(void);
void                      MFX_IO_Wakeup(void);
void                      MFX_IO_Delay(uint32_t delay);
void                      MFX_IO_Write(uint16_t addr, uint8_t reg, uint8_t value);
uint8_t                   MFX_IO_Read(uint16_t addr, uint8_t reg);
void                      MFX_IO_WriteMultiple(uint16_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);
uint16_t                  MFX_IO_ReadMultiple(uint16_t addr, uint8_t reg, uint8_t *buffer, uint16_t length);

static void     I2Cx_Init(I2C_HandleTypeDef *i2c_handler);

static HAL_StatusTypeDef I2Cx_ReadMultiple(I2C_HandleTypeDef *i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddSize, uint8_t *Buffer, uint16_t Length);
static HAL_StatusTypeDef I2Cx_WriteMultiple(I2C_HandleTypeDef *i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddSize, uint8_t *Buffer, uint16_t Length);
static void              I2Cx_Error(I2C_HandleTypeDef *i2c_handler, uint8_t Addr);

static uint8_t I2C2_isDeviceReady(uint16_t Addr, uint32_t trial);

#endif/* HAL_I2C_MODULE_ENABLED */
/**
  * @}
  */

__weak void BSP_ErrorHandler(void)
{
  while (1);
}

/** @defgroup STM32L496G_DISCOVERY_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  This method returns the STM32L496 DISCOVERY BSP Driver revision
  * @retval version : 0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t BSP_GetVersion(void)
{
  return __STM32L496G_DISCOVERY_BSP_VERSION;
}



/**
  * @brief  Configures LED GPIOs.
  * @param  Led: Specifies the Led to be configured.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  if (Led == LED2)
  {
    /* Enable the GPIO_LED clock */
    LED2_GPIO_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.Pin = LED_PIN[Led];
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    HAL_GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);
    /* By default, turn off LED */
    HAL_GPIO_WritePin(LED2_GPIO_PORT, GPIO_InitStructure.Pin, GPIO_PIN_SET);
  }
  else
  {
    /* Initialize the IO functionalities */
    if (BSP_IO_Init() == IO_ERROR)
    {
      BSP_ErrorHandler();
    }

    BSP_IO_ConfigPin(LED_PIN[Led], IO_MODE_OUTPUT);

    /* By default, turn off LED */
    BSP_IO_WritePin(LED_PIN[Led], GPIO_PIN_SET);
  }

}


/**
  * @brief  Unconfigures LED GPIOs.
  * @param  Led: Specifies the Led to be unconfigured.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void BSP_LED_DeInit(Led_TypeDef Led)
{
  if (Led == LED2)
  {
    /* Enable the GPIO_LED clock */
    LED2_GPIO_CLK_ENABLE();

    HAL_GPIO_DeInit(LED2_GPIO_PORT, LED_PIN[Led]);
  }
}


/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void BSP_LED_On(Led_TypeDef Led)
{
  if (Led == LED2)
  {
    HAL_GPIO_WritePin(LED2_GPIO_PORT, LED_PIN[Led], GPIO_PIN_RESET);
  }
  else
  {
    BSP_IO_WritePin(LED_PIN[Led], GPIO_PIN_RESET);
  }
}


/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void BSP_LED_Off(Led_TypeDef Led)
{
  if (Led == LED2)
  {
    HAL_GPIO_WritePin(LED2_GPIO_PORT, LED_PIN[Led], GPIO_PIN_SET);
  }
  else
  {
    BSP_IO_WritePin(LED_PIN[Led], GPIO_PIN_SET);
  }
}


/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void BSP_LED_Toggle(Led_TypeDef Led)
{
  if (Led == LED2)
  {
    HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED_PIN[Led]);
  }
  else
  {
    BSP_IO_TogglePin(LED_PIN[Led]);
  }
}

/**
  * @brief  Configures all buttons of the joystick in GPIO or EXTI modes.
  * @param  Joy_Mode: Joystick mode.
  *    This parameter can be one of the following values:
  *     @arg  JOY_MODE_GPIO: Joystick pins will be used as simple IOs
  *     @arg  JOY_MODE_EXTI: Joystick pins will be connected to EXTI line
  *                                 with interrupt generation capability
  * @retval HAL_OK: if all initializations are OK. Other value if error.
  */
uint8_t BSP_JOY_Init(JOYMode_TypeDef Joy_Mode)
{
  JOYState_TypeDef joykey;
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Initialized the Joystick. */
  for (joykey = JOY_SEL; joykey < (JOY_SEL + JOYn) ; joykey++)
  {
    /* Enable the JOY clock */
    JOYx_GPIO_CLK_ENABLE(joykey);

    GPIO_InitStruct.Pin = JOY_PIN[joykey];
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    if (Joy_Mode == JOY_MODE_GPIO)
    {
      /* Configure Joy pin as input */
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      HAL_GPIO_Init(JOY_PORT[joykey], &GPIO_InitStruct);
    }
    else if (Joy_Mode == JOY_MODE_EXTI)
    {
      /* Configure Joy pin as input with External interrupt */
      GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
      HAL_GPIO_Init(JOY_PORT[joykey], &GPIO_InitStruct);
      /* Enable and set Joy EXTI Interrupt to the lowest priority */
      HAL_NVIC_SetPriority((IRQn_Type)(JOY_IRQn[joykey]), 0x0F, 0x00);
      HAL_NVIC_EnableIRQ((IRQn_Type)(JOY_IRQn[joykey]));
    }
  }

  return HAL_OK;
}

/**
  * @brief  Unconfigures all GPIOs used as buttons of the joystick.
  * @retval None.
  */
void BSP_JOY_DeInit(void)
{
  JOYState_TypeDef joykey;

  /* Initialized the Joystick. */
  for (joykey = JOY_SEL; joykey < (JOY_SEL + JOYn) ; joykey++)
  {
    /* Enable the JOY clock */
    JOYx_GPIO_CLK_ENABLE(joykey);

    HAL_GPIO_DeInit(JOY_PORT[joykey], JOY_PIN[joykey]);
  }
}

/**
* @brief  Returns the current joystick status.
* @retval Code of the joystick key pressed
*          This code can be one of the following values:
*            @arg  JOY_NONE
*            @arg  JOY_SEL
*            @arg  JOY_DOWN
*            @arg  JOY_LEFT
*            @arg  JOY_RIGHT
*            @arg  JOY_UP
*/
JOYState_TypeDef BSP_JOY_GetState(void)
{
  JOYState_TypeDef joykey;

  for (joykey = JOY_SEL; joykey < (JOY_SEL + JOYn) ; joykey++)
  {
    if (HAL_GPIO_ReadPin(JOY_PORT[joykey], JOY_PIN[joykey]) == GPIO_PIN_SET)
    {
      /* Return Code Joystick key pressed */
      return joykey;
    }
  }

  /* No Joystick key pressed */
  return JOY_NONE;
}

#if defined(HAL_UART_MODULE_ENABLED)
/**
  * @brief  Configures COM port.
  * @param  COM: COM port to be configured.
  *          This parameter can be one of the following values:
  *            @arg  COM1 
  * @param  huart: Pointer to a UART_HandleTypeDef structure that contains the
  *                configuration information for the specified USART peripheral.
  */
void BSP_COM_Init(COM_TypeDef COM, UART_HandleTypeDef *huart) 
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Enable GPIO clock */
  DISCOVERY_COMx_TX_GPIO_CLK_ENABLE(COM);
  DISCOVERY_COMx_RX_GPIO_CLK_ENABLE(COM);

  /* Enable USART clock */
  DISCOVERY_COMx_CLK_ENABLE(COM);

  /* Configure USART Tx as alternate function */
  gpio_init_structure.Pin = COM_TX_PIN[COM];
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Alternate = COM_TX_AF[COM];
  HAL_GPIO_Init(COM_TX_PORT[COM], &gpio_init_structure);

  /* Configure USART Rx as alternate function */
  gpio_init_structure.Pin = COM_RX_PIN[COM];
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = COM_RX_AF[COM];
  HAL_GPIO_Init(COM_RX_PORT[COM], &gpio_init_structure);

  /* USART configuration */
  huart->Instance = COM_USART[COM];
  HAL_UART_Init(huart);
}

/**
  * @brief  DeInit COM port.
  * @param  COM: COM port to be configured.
  *          This parameter can be one of the following values:
  *            @arg  COM1 
  * @param  huart: Pointer to a UART_HandleTypeDef structure that contains the
  *                configuration information for the specified USART peripheral.
  */
void BSP_COM_DeInit(COM_TypeDef COM, UART_HandleTypeDef *huart)
{
  /* USART deinitialization */
  huart->Instance = COM_USART[COM];
  HAL_UART_DeInit(huart);

  /* Disable USART clock */
  DISCOVERY_COMx_CLK_DISABLE(COM);

  /* USART TX/RX pins deinitializations */
  HAL_GPIO_DeInit(COM_TX_PORT[COM], COM_TX_PIN[COM]);
  HAL_GPIO_DeInit(COM_RX_PORT[COM], COM_RX_PIN[COM]);

  /* Disable GPIOs clock is left for application */
}
#endif /* HAL_UART_MODULE_ENABLED */

/**
  * @}
  */

/** @defgroup STM32L496G_DISCOVERY_BusOperations_Functions Bus Operations Functions
  * @{
  */

/*******************************************************************************
                            BUS OPERATIONS
*******************************************************************************/

#if defined(HAL_I2C_MODULE_ENABLED)
/******************************* I2C Routines**********************************/
/**
  * @brief Discovery I2C2 Bus initialization
  * @retval None
  */
void I2C2_Init(void)
{
  if (HAL_I2C_GetState(&I2c2Handle) == HAL_I2C_STATE_RESET)
  {
    I2c2Handle.Instance              = DISCOVERY_I2C2;
    I2c2Handle.Init.Timing           = DISCOVERY_I2C2_TIMING;
    I2c2Handle.Init.OwnAddress1      = 0x70;
    I2c2Handle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
    I2c2Handle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
    I2c2Handle.Init.OwnAddress2      = 0xFF;
    I2c2Handle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
    I2c2Handle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

    /* Init the I2C */
    I2C2_MspInit(&I2c2Handle);
    HAL_I2C_Init(&I2c2Handle);
  }
}

/**
  * @brief Discovery I2C2 MSP Initialization
  * @param hi2c: I2C2 handle
  * @retval None
  */
static void I2C2_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_PeriphCLKInitTypeDef  RCC_PeriphCLKInitStruct;

  if (hi2c->Instance == DISCOVERY_I2C2)
  {
    /*##-1- Configure the Discovery I2C2 clock source. The clock is derived from the SYSCLK #*/
    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
    RCC_PeriphCLKInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_SYSCLK;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

    /*##-2- Configure the GPIOs ################################################*/
    /* Enable GPIO clock */
    DISCOVERY_I2C2_SDA_GPIO_CLK_ENABLE();
    DISCOVERY_I2C2_SCL_GPIO_CLK_ENABLE();

    /* Configure I2C Rx/Tx as alternate function  */
    GPIO_InitStructure.Pin       = DISCOVERY_I2C2_SCL_PIN;
    GPIO_InitStructure.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Pull      = /*GPIO_NOPULL*/ GPIO_PULLUP;
    GPIO_InitStructure.Speed     = /*GPIO_SPEED_MEDIUM*/ GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStructure.Alternate = DISCOVERY_I2C2_SCL_SDA_AF;
    HAL_GPIO_Init(DISCOVERY_I2C2_SCL_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.Pin       = DISCOVERY_I2C2_SDA_PIN;
    HAL_GPIO_Init(DISCOVERY_I2C2_SDA_GPIO_PORT, &GPIO_InitStructure);

    /*##-3- Configure the Discovery I2C2 peripheral #############################*/
    /* Enable Discovery_I2C2 clock */
    DISCOVERY_I2C2_CLK_ENABLE();

    /* Force and release the I2C Peripheral Clock Reset */
    DISCOVERY_I2C2_FORCE_RESET();
    DISCOVERY_I2C2_RELEASE_RESET();

    /* Enable and set Discovery I2C2 Interrupt to the highest priority */
    HAL_NVIC_SetPriority(DISCOVERY_I2C2_EV_IRQn, 0x00, 0);
    HAL_NVIC_EnableIRQ(DISCOVERY_I2C2_EV_IRQn);

    /* Enable and set Discovery I2C2 Interrupt to the highest priority */
    HAL_NVIC_SetPriority(DISCOVERY_I2C2_ER_IRQn, 0x00, 0);
    HAL_NVIC_EnableIRQ(DISCOVERY_I2C2_ER_IRQn);
  }
}

/**
  * @brief Discovery I2C2 Bus Deinitialization
  * @retval None
  */
void I2C2_DeInit(void)
{
  if (HAL_I2C_GetState(&I2c2Handle) != HAL_I2C_STATE_RESET)
  {
    /* DeInit the I2C */
    HAL_I2C_DeInit(&I2c2Handle);
    I2C2_MspDeInit(&I2c2Handle);
  }
}

/**
  * @brief Discovery I2C2 MSP DeInitialization
  * @param hi2c: I2C2 handle
  * @retval None
  */
static void I2C2_MspDeInit(I2C_HandleTypeDef *hi2c)
{
  if (hi2c->Instance == DISCOVERY_I2C2)
  {
    /*##-1- Unconfigure the GPIOs ################################################*/
    /* Enable GPIO clock */
    DISCOVERY_I2C2_SDA_GPIO_CLK_ENABLE();
    DISCOVERY_I2C2_SCL_GPIO_CLK_ENABLE();

    /* Configure I2C Rx/Tx as alternate function  */
    HAL_GPIO_DeInit(DISCOVERY_I2C2_SCL_GPIO_PORT, DISCOVERY_I2C2_SCL_PIN);
    HAL_GPIO_DeInit(DISCOVERY_I2C2_SDA_GPIO_PORT,  DISCOVERY_I2C2_SDA_PIN);

    /*##-2- Unconfigure the Discovery I2C2 peripheral ############################*/
    /* Force and release I2C Peripheral */
    DISCOVERY_I2C2_FORCE_RESET();
    DISCOVERY_I2C2_RELEASE_RESET();

    /* Disable Discovery I2C2 clock */
    DISCOVERY_I2C2_CLK_DISABLE();

    /* Disable Discovery I2C2 interrupts */
    HAL_NVIC_DisableIRQ(DISCOVERY_I2C2_EV_IRQn);
    HAL_NVIC_DisableIRQ(DISCOVERY_I2C2_ER_IRQn);
  }
}

/**
  * @brief  Write a value in a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.
  * @param  Reg: The target register address to write
  * @param  RegSize: The target register size (can be 8BIT or 16BIT)
  * @param  Value: The target register value to be written
  * @retval None
  */
static void I2C2_WriteData(uint16_t Addr, uint16_t Reg, uint16_t RegSize, uint8_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;

  __disable_irq();

  status = HAL_I2C_Mem_Write(&I2c2Handle, Addr, (uint16_t)Reg, RegSize, &Value, 1, I2c2Timeout);

  __enable_irq();


  /* Check the communication status */
  if (status != HAL_OK)
  {
    /* Re-Initiaize the BUS */
    I2C2_Error();
  }
}

/**
  * @brief  Write a value in a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.
  * @param  Reg: The target register address to write
  * @param  RegSize: The target register size (can be 8BIT or 16BIT)
  * @param  pBuffer: The target register value to be written
  * @param  Length: buffer size to be written
  * @retval None
  */
static HAL_StatusTypeDef I2C2_WriteBuffer(uint16_t Addr, uint16_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  __disable_irq();

  status = HAL_I2C_Mem_Write(&I2c2Handle, Addr, (uint16_t)Reg, RegSize, pBuffer, Length, I2c2Timeout);

  __enable_irq();


  /* Check the communication status */
  if (status != HAL_OK)
  {
    /* Re-Initiaize the BUS */
    I2C2_Error();
  }

  return status;
}

/**
  * @brief  Read a register of the device through BUS
  * @param  Addr: Device address on BUS
  * @param  Reg: The target register address to read
  * @param  RegSize: The target register size (can be 8BIT or 16BIT)
  * @retval read register value
  */
static uint8_t I2C2_ReadData(uint16_t Addr, uint16_t Reg, uint16_t RegSize)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t value = 0x0;

  __disable_irq();

  status = HAL_I2C_Mem_Read(&I2c2Handle, Addr, Reg, RegSize, &value, 1, I2c2Timeout);

  __enable_irq();

  /* Check the communication status */
  if (status != HAL_OK)
  {
    /* Re-Initiaize the BUS */
    I2C2_Error();
    HAL_Delay(200);
  }

  return value;
}

static uint8_t I2C2_isDeviceReady(uint16_t Addr, uint32_t trial)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t value = 0x0;

  __disable_irq();

  status = HAL_I2C_IsDeviceReady(&I2c2Handle, Addr, trial, 50);

  __enable_irq();

  /* Check the communication status */
  if (status != HAL_OK)
  {
    /* Re-Initiaize the BUS */
    I2C2_Error();
    HAL_Delay(200);
  }

  return value;
}

/**
  * @brief  Reads multiple data on the BUS.
  * @param  Addr: I2C Address
  * @param  Reg: Reg Address
  * @param  RegSize : The target register size (can be 8BIT or 16BIT)
  * @param  pBuffer: pointer to read data buffer
  * @param  Length: length of the data
  * @retval 0 if no problems to read multiple data
  */
static HAL_StatusTypeDef I2C2_ReadBuffer(uint16_t Addr, uint16_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  __disable_irq();

  status = HAL_I2C_Mem_Read(&I2c2Handle, Addr, (uint16_t)Reg, RegSize, pBuffer, Length, I2c2Timeout);

  __enable_irq();

  /* Check the communication status */
  if (status != HAL_OK)
  {
    /* Re-Initiaize the BUS */
    I2C2_Error();
  }

  return status;
}

/**
  * @brief Discovery I2C2 error treatment function
  * @retval None
  */
static void I2C2_Error(void)
{
  BSP_ErrorHandler();

  /* De-initialize the I2C communication BUS */
  HAL_I2C_DeInit(&I2c2Handle);

  /* Re- Initiaize the I2C communication BUS */
  I2C2_Init();
}



/******************************* I2C Routines *********************************/
/**
  * @brief  Initializes I2C HAL.
  * @param  i2c_handler : I2C handler
  * @retval None
  */
static void I2Cx_Init(I2C_HandleTypeDef *i2c_handler)
{

  if (HAL_I2C_GetState(i2c_handler) == HAL_I2C_STATE_RESET)
  {
    if (i2c_handler == (I2C_HandleTypeDef *)(&hI2cTSHandler))
    {
      /* TS (Capacitive Touch Panel) and LCD I2C configuration */
      i2c_handler->Instance = DISCOVERY_TS_I2Cx;

      /* Need to enable MFX, and in doing so, initialize I2C at the same time */
      /* MFX_IO_Init();*/

    }
    else if (i2c_handler == (I2C_HandleTypeDef *)(&hI2cAudioHandler))
    {
      /* Audio and LCD I2C configuration */
      i2c_handler->Instance = DISCOVERY_AUDIO_I2Cx;
    }
    else if (i2c_handler == (I2C_HandleTypeDef *)(&hI2cCameraHandler))
    {
      i2c_handler->Instance = DISCOVERY_CAMERA_I2Cx;
    }
    else
    {
      /* External, EEPROM and Arduino connector I2C configuration */
      i2c_handler->Instance = DISCOVERY_EXT_I2Cx;
    }

    i2c_handler->Init.Timing           = DISCOVERY_I2C_TIMING;
    i2c_handler->Init.OwnAddress1      = 0x70;
    i2c_handler->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
    i2c_handler->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
    i2c_handler->Init.OwnAddress2      = 0xFF;
    i2c_handler->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
    i2c_handler->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;


    /* Init the I2C */
    HAL_I2C_Init(i2c_handler);

  }

}

/**
  * @brief  Reads multiple data.
  * @param  i2c_handler : I2C handler
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @param  MemAddress: Memory address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval Number of read data
  */
static HAL_StatusTypeDef I2Cx_ReadMultiple(I2C_HandleTypeDef *i2c_handler,
                                           uint8_t Addr,
                                           uint16_t Reg,
                                           uint16_t MemAddress,
                                           uint8_t *Buffer,
                                           uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Read(i2c_handler, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, 1000);

  /* Check the communication status */
  if (status != HAL_OK)
  {
    /* I2C error occurred */
    I2Cx_Error(i2c_handler, Addr);
  }
  return status;
}

/**
  * @brief  Writes a value in a register of the device through BUS in using DMA mode.
  * @param  i2c_handler : I2C handler
  * @param  Addr: Device address on BUS Bus.
  * @param  Reg: The target register address to write
  * @param  MemAddress: Memory address
  * @param  Buffer: The target register value to be written
  * @param  Length: buffer size to be written
  * @retval HAL status
  */
static HAL_StatusTypeDef I2Cx_WriteMultiple(I2C_HandleTypeDef *i2c_handler,
                                            uint8_t Addr,
                                            uint16_t Reg,
                                            uint16_t MemAddress,
                                            uint8_t *Buffer,
                                            uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Write(i2c_handler, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, 1000);

  /* Check the communication status */
  if (status != HAL_OK)
  {
    /* Re-Initialize the I2C Bus */
    I2Cx_Error(i2c_handler, Addr);
  }
  return status;
}

/**
  * @brief  Manages error callback by re-initializing I2C.
  * @param  i2c_handler : I2C handler
  * @param  Addr: I2C Address
  * @retval None
  */
static void I2Cx_Error(I2C_HandleTypeDef *i2c_handler, uint8_t Addr)
{
  BSP_ErrorHandler();

  /* De-initialize the I2C communication bus */
  if (i2c_handler == (I2C_HandleTypeDef *)(&hI2cTSHandler))
  {
    I2C2_DeInit();
  }
  else
  {
    HAL_I2C_DeInit(i2c_handler);
  }

  /* Re-Initialize the I2C communication bus */
  I2Cx_Init(i2c_handler);
}
#endif /*HAL_I2C_MODULE_ENABLED*/


/*******************************************************************************
                            LINK OPERATIONS
*******************************************************************************/

#if defined(HAL_I2C_MODULE_ENABLED)
/********************************* LINK MFX ***********************************/
/**
  * @brief  Initializes MFX low level.
  * @retval None
  */
void MFX_IO_Init(void)
{
  /* I2C2 init */
  I2C2_Init();

  /* Wait for device ready */
  if (I2C2_isDeviceReady(IO1_I2C_ADDRESS, 4) != HAL_OK)
  {
    BSP_ErrorHandler();
  }
}
/**
  * @brief  Deinitializes MFX low level.
  * @retval None
  */
void MFX_IO_DeInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable wakeup gpio clock */
  MFX_WAKEUP_GPIO_CLK_ENABLE();

  /* MFX wakeup pin configuration */
  GPIO_InitStruct.Pin   = MFX_WAKEUP_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
  HAL_GPIO_Init(MFX_WAKEUP_GPIO_PORT, &GPIO_InitStruct);

  /* DeInit interrupt pin : disable IRQ before to avoid spurious interrupt */
  HAL_NVIC_DisableIRQ((IRQn_Type)(MFX_INT_EXTI_IRQn));
  MFX_INT_GPIO_CLK_ENABLE();
  HAL_GPIO_DeInit(MFX_INT_GPIO_PORT, MFX_INT_PIN);

  /* I2C2 Deinit */
  I2C2_DeInit();
}

/**
  * @brief  Configures MFX low level interrupt.
  * @retval None
  */
void MFX_IO_ITConfig(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable the GPIO clock */
  MFX_INT_GPIO_CLK_ENABLE();

  /* MFX_OUT_IRQ (normally used for EXTI_WKUP) */
  GPIO_InitStruct.Pin   = MFX_INT_PIN;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING;
  HAL_GPIO_Init(MFX_INT_GPIO_PORT, &GPIO_InitStruct);

  /* Enable and set GPIO EXTI Interrupt to the lowest priority */
  HAL_NVIC_SetPriority((IRQn_Type)(MFX_INT_EXTI_IRQn), 0x0F, 0x0F);
  HAL_NVIC_EnableIRQ((IRQn_Type)(MFX_INT_EXTI_IRQn));
}

/**
  * @brief  Configures MFX wke up  pin.
  * @retval None
  */
void MFX_IO_EnableWakeupPin(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable wakeup gpio clock */
  MFX_WAKEUP_GPIO_CLK_ENABLE();

  /* MFX wakeup pin configuration */
  GPIO_InitStruct.Pin   = MFX_WAKEUP_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(MFX_WAKEUP_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  Wakeup MFX.
  * @retval None
  */
void MFX_IO_Wakeup(void)
{
  /* Set Wakeup pin to high to wakeup Idd measurement component from standby mode */
  HAL_GPIO_WritePin(MFX_WAKEUP_GPIO_PORT, MFX_WAKEUP_PIN, GPIO_PIN_SET);

  /* Wait */
  HAL_Delay(1);

  /* Set gpio pin basck to low */
  HAL_GPIO_WritePin(MFX_WAKEUP_GPIO_PORT, MFX_WAKEUP_PIN, GPIO_PIN_RESET);
}

/**
  * @brief  MFX writes single data.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @param  Value: Data to be written
  * @retval None
  */
void MFX_IO_Write(uint16_t Addr, uint8_t Reg, uint8_t Value)
{
  I2C2_WriteData(Addr, Reg, I2C_MEMADD_SIZE_8BIT, Value);
}

/**
  * @brief  MFX reads single data.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @retval Read data
  */
uint8_t MFX_IO_Read(uint16_t Addr, uint8_t Reg)
{
  return I2C2_ReadData(Addr, Reg, I2C_MEMADD_SIZE_8BIT);
}

/**
  * @brief  MFX reads multiple data.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval Number of read data
  */
uint16_t MFX_IO_ReadMultiple(uint16_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length)
{
  return I2C2_ReadBuffer(Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, Buffer, Length);
}

/**
  * @brief  MFX writes multiple data.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval None
  */
void MFX_IO_WriteMultiple(uint16_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length)
{
  I2C2_WriteBuffer(Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, Buffer, Length);
}

/**
  * @brief  MFX delay
  * @param  Delay: Delay in ms
  * @retval None
  */
void MFX_IO_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}


/********************************* LINK AUDIO *********************************/
/**
  * @brief  Initializes Audio low level.
  * @retval None
  */
void AUDIO_IO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  uint8_t Value;

  /* Enable Reset GPIO Clock */
  AUDIO_RESET_GPIO_CLK_ENABLE();

  /* Audio reset pin configuration */
  GPIO_InitStruct.Pin   = AUDIO_RESET_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(AUDIO_RESET_GPIO, &GPIO_InitStruct);

  /* I2C bus init */
  I2C2_Init();

  /* Power off the codec */
  CODEC_AUDIO_POWER_OFF();

  /* wait until power supplies are stable */
  HAL_Delay(10);

  /* Power on the codec */
  CODEC_AUDIO_POWER_ON();

  /* Set the device in standby mode */
  Value = AUDIO_IO_Read(AUDIO_I2C_ADDRESS, 0x02);
  AUDIO_IO_Write(AUDIO_I2C_ADDRESS, 0x02, (Value | 0x01));

  /* Set all power down bits to 1 */
  AUDIO_IO_Write(AUDIO_I2C_ADDRESS, 0x02, 0x7F);
  Value = AUDIO_IO_Read(AUDIO_I2C_ADDRESS, 0x03);
  AUDIO_IO_Write(AUDIO_I2C_ADDRESS, 0x03, (Value | 0x0E));
}

/**
  * @brief  Deinitializes Audio low level.
  * @retval None
  */
void AUDIO_IO_DeInit(void)
{
  uint8_t Value;

  /* Mute DAC and ADC */
  Value = AUDIO_IO_Read(AUDIO_I2C_ADDRESS, 0x08);
  AUDIO_IO_Write(AUDIO_I2C_ADDRESS, 0x08, (Value | 0x03));
  Value = AUDIO_IO_Read(AUDIO_I2C_ADDRESS, 0x07);
  AUDIO_IO_Write(AUDIO_I2C_ADDRESS, 0x07, (Value | 0x03));

  /* Disable soft ramp and zero cross */
  Value = AUDIO_IO_Read(AUDIO_I2C_ADDRESS, 0x06);
  AUDIO_IO_Write(AUDIO_I2C_ADDRESS, 0x06, (Value & 0xF0));

  /* Set PDN to 1 */
  Value = AUDIO_IO_Read(AUDIO_I2C_ADDRESS, 0x02);
  AUDIO_IO_Write(AUDIO_I2C_ADDRESS, 0x02, (Value | 0x01));

  /* Set all power down bits to 1 */
  AUDIO_IO_Write(AUDIO_I2C_ADDRESS, 0x02, 0x7F);
  Value = AUDIO_IO_Read(AUDIO_I2C_ADDRESS, 0x03);
  AUDIO_IO_Write(AUDIO_I2C_ADDRESS, 0x03, (Value | 0x0E));

  /* Power off the codec */
  CODEC_AUDIO_POWER_OFF();

}

/**
  * @brief  Writes a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @param  Value: Data to be written
  * @retval None
  */
void AUDIO_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
  I2C2_WriteBuffer(Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1);
}

/**
  * @brief  Reads a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @retval Data to be read
  */
uint8_t AUDIO_IO_Read(uint8_t Addr, uint8_t Reg)
{
  uint8_t Read_Value = 0;

  I2C2_ReadBuffer((uint16_t) Addr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, &Read_Value, 1);

  return Read_Value;
}

/**
  * @brief  AUDIO Codec delay
  * @param  Delay: Delay in ms
  * @retval None
  */
void AUDIO_IO_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}




/*************************** FMC Routines ************************************/
/**
  * @brief  Initializes FMC_BANK1_LCD_IO MSP.
  * @param  None
  * @retval None
  */
void FMC_BANK1_MspInit(void)
{

  GPIO_InitTypeDef GPIO_Init_Structure;

  /* Enable FMC clock */
  __HAL_RCC_FMC_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();

  /* Set PD11 */
  GPIO_Init_Structure.Pin =   GPIO_PIN_11;
  GPIO_Init_Structure.Mode =  GPIO_MODE_OUTPUT_PP;
  GPIO_Init_Structure.Pull =  GPIO_NOPULL;
  GPIO_Init_Structure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_Init_Structure.Alternate = 0;

  HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);


  GPIO_Init_Structure.Mode      = GPIO_MODE_AF_PP;
  GPIO_Init_Structure.Pull      = GPIO_PULLUP;
  GPIO_Init_Structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_Init_Structure.Alternate = GPIO_AF12_FMC;
  /* GPIOD configuration */ /* GPIO_PIN_7 is  FMC_NE1 */
  GPIO_Init_Structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | \
                              GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_7;

  HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);




  /* GPIOE configuration */
  GPIO_Init_Structure.Pin   = GPIO_PIN_7     | \
                              GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | \
                              GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &GPIO_Init_Structure);

  /* GPIOF configuration */
  GPIO_Init_Structure.Pin   = GPIO_PIN_13 ;
  HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);

  GPIO_Init_Structure.Pin   = GPIO_PIN_0;
  HAL_GPIO_Init(GPIOF, &GPIO_Init_Structure);
}


/**
  * @brief  Initializes LCD IO.
  * @param  None
  * @retval None
  */
void FMC_BANK1_Init(void)
{
  SRAM_HandleTypeDef hsram;
  FMC_NORSRAM_TimingTypeDef sram_timing;
  FMC_NORSRAM_TimingTypeDef sram_timing_write;

  /*** Configure the SRAM Bank 1 ***/
  /* Configure IPs */
  hsram.Instance  = FMC_NORSRAM_DEVICE;
  hsram.Extended  = FMC_NORSRAM_EXTENDED_DEVICE;


  /* Timing for READING */

  sram_timing.AddressSetupTime       = 1;
  sram_timing.AddressHoldTime        = 1;
  sram_timing.DataSetupTime          = 1;
  sram_timing.BusTurnAroundDuration  = 0;
  sram_timing.CLKDivision            = 2;
  sram_timing.DataLatency            = 2;
  sram_timing.AccessMode             = FMC_ACCESS_MODE_A;
  /* Timing for WRITING */
  sram_timing_write.AddressSetupTime      = 5;
  sram_timing_write.AddressHoldTime       = 1;
  sram_timing_write.DataSetupTime         = 3;
  sram_timing_write.BusTurnAroundDuration = 2;
  sram_timing_write.CLKDivision           = 2;
  sram_timing_write.DataLatency           = 2;
  sram_timing_write.AccessMode            = FMC_ACCESS_MODE_A;


  hsram.Init.NSBank             = FMC_NORSRAM_BANK1;
  hsram.Init.DataAddressMux     = FMC_DATA_ADDRESS_MUX_DISABLE;
  hsram.Init.MemoryType         = FMC_MEMORY_TYPE_SRAM;
  hsram.Init.MemoryDataWidth    = FMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram.Init.BurstAccessMode    = FMC_BURST_ACCESS_MODE_DISABLE;
  hsram.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram.Init.WaitSignalActive   = FMC_WAIT_TIMING_BEFORE_WS;
  hsram.Init.WriteOperation     = FMC_WRITE_OPERATION_ENABLE;
  hsram.Init.WaitSignal         = FMC_WAIT_SIGNAL_DISABLE;
  hsram.Init.ExtendedMode       = FMC_EXTENDED_MODE_DISABLE;
  hsram.Init.AsynchronousWait   = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram.Init.WriteBurst         = FMC_WRITE_BURST_DISABLE;
  hsram.Init.PageSize           = FMC_PAGE_SIZE_NONE;
  hsram.Init.WriteFifo          = FMC_WRITE_FIFO_DISABLE;
  hsram.Init.ContinuousClock    = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
  /* Initialize the SRAM controller */
  FMC_BANK1_MspInit();
  HAL_SRAM_Init(&hsram, &sram_timing, &sram_timing_write);

}


/**
  * @brief  DeInitializes FMC_BANK1_LCD_IO MSP.
  * @param  None
  * @retval None
  */
void FMC_BANK1_MspDeInit(void)
{

  /* Enable FMC clock */
  __HAL_RCC_FMC_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();


  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | \
                  GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_7 | GPIO_PIN_11 | GPIO_PIN_13);



  HAL_GPIO_DeInit(GPIOE, GPIO_PIN_7 | \
                  GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | \
                  GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

  HAL_GPIO_DeInit(GPIOF, GPIO_PIN_0);


}


/**
  * @brief  Writes register value.
  * @param  Data: Data to be written
  * @retval None
  */
static void FMC_BANK1_WriteData(uint16_t Data)
{
  /* Write 16-bit Reg */
  LCD_ADDR->REG = Data;
}

/**
  * @brief  Writes register address.
  * @param  Reg: Register to be written
  * @retval None
  */
static void FMC_BANK1_WriteReg(uint8_t Reg)
{
  /* Write 16-bit Index, then write register */
  FMC_BANK1_ADDR->REG = Reg;
}

/**
  * @brief  Reads register value.
  * @param  None
  * @retval Read value
  */
static uint16_t FMC_BANK1_ReadData(void)
{
  return LCD_ADDR->REG;
}

/*******************************************************************************
                            LINK OPERATIONS
*******************************************************************************/

/********************************* LINK LCD ***********************************/

/**
  * @brief  Initializes LCD low level.
  * @param  None
  * @retval None
  */
void LCD_IO_Init(void)
{
  FMC_BANK1_Init();
}

/**
  * @brief  Writes data on LCD data register.
  * @param  Data: Data to be written
  * @retval None
  */
void LCD_IO_WriteData(uint16_t RegValue)
{
  /* Write 16-bit Reg */
  FMC_BANK1_WriteData(RegValue);
}

/**
  * @brief  Writes several data on LCD data register.
  * @param  Data: pointer on data to be written
  * @param  Size: data amount in 16bits short unit
  * @retval None
  */
void LCD_IO_WriteMultipleData(uint16_t *pData, uint32_t Size)
{
  uint32_t  i;

  for (i = 0; i < Size; i++)
  {
    FMC_BANK1_WriteData(pData[i]);
  }
}

/**
  * @brief  Writes register on LCD register.
  * @param  Reg: Register to be written
  * @retval None
  */
void LCD_IO_WriteReg(uint8_t Reg)
{
  /* Write 16-bit Index, then Write Reg */
  FMC_BANK1_WriteReg(Reg);
}

/**
  * @brief  Reads data from LCD data register.
  * @param  None
  * @retval Read data.
  */
uint16_t LCD_IO_ReadData(void)
{
  return FMC_BANK1_ReadData();
}

/**
  * @brief  LCD delay
  * @param  Delay: Delay in ms
  * @retval None
  */
void LCD_IO_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}


/************************** LINK TS (TouchScreen) *****************************/
/**
  * @brief  Initializes Touchscreen low level.
  * @retval None
  */
void TS_IO_Init(void)
{
  I2Cx_Init(&hI2cTSHandler);

  if (ts_io_init == 0)
  {
    if (BSP_LCD_Init() == LCD_ERROR)
    {
      BSP_ErrorHandler();
    }

    BSP_IO_ConfigPin(TS_RST_PIN, IO_MODE_OUTPUT);

    BSP_IO_WritePin(TS_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(10);
    BSP_IO_WritePin(TS_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(200);

    ts_io_init = 1;
  }
}

/**
  * @brief  Writes a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @param  Value: Data to be written
  * @retval None
  */
void TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
  I2Cx_WriteMultiple(&hI2cTSHandler, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&Value, 1);
}

/**
  * @brief  Reads a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @retval Data to be read
  */
uint8_t TS_IO_Read(uint8_t Addr, uint8_t Reg)
{
  return I2C2_ReadData(Addr, Reg, I2C_MEMADD_SIZE_8BIT);
}

/**
  * @brief  Reads multiple data with I2C communication
  *         channel from TouchScreen.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval Number of read data
  */
uint16_t TS_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length)
{
  return I2Cx_ReadMultiple(&hI2cTSHandler, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, Buffer, Length);
}

/**
  * @brief  Writes multiple data with I2C communication
  *         channel from MCU to TouchScreen.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval None
  */
void TS_IO_WriteMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length)
{
  I2Cx_WriteMultiple(&hI2cTSHandler, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, Buffer, Length);
}

/**
  * @brief  Delay function used in TouchScreen low level driver.
  * @param  Delay: Delay in ms
  * @retval None
  */
void TS_IO_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}


/************************** Camera  *****************************/
/**
  * @brief  Initializes Camera low level.
  * @retval None
  */
void CAMERA_IO_Init(void)
{
  I2Cx_Init(&hI2cCameraHandler);
}

/**
  * @brief  Camera writes single data.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @param  Value: Data to be written
  * @retval None
  */
void CAMERA_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
  I2Cx_WriteMultiple(&hI2cCameraHandler, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&Value, 1);
}

/**
  * @brief  Camera reads single data.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @retval Read data
  */
uint8_t CAMERA_IO_Read(uint8_t Addr, uint8_t Reg)
{
  uint8_t read_value = 0;

  I2Cx_ReadMultiple(&hI2cCameraHandler, Addr, Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&read_value, 1);

  return read_value;
}

/**
  * @brief  Camera delay
  * @param  Delay: Delay in ms
  * @retval None
  */
void CAMERA_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}

#endif /* HAL_I2C_MODULE_ENABLED */


__weak uint8_t BSP_LCD_Init(void)
{
  uint8_t ret = 0;
  return ret;
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
