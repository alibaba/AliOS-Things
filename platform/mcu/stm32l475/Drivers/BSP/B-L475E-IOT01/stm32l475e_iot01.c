/**
  ******************************************************************************
  * @file    stm32l475e_iot01.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    17-March-2017
  * @brief   STM32L475E-IOT01 board support package
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
#include "stm32l475e_iot01.h"
/** @defgroup BSP BSP
  * @{
  */ 

/** @defgroup STM32L475E_IOT01 STM32L475E_IOT01
  * @{
  */

/** @defgroup STM32L475E_IOT01_LOW_LEVEL LOW LEVEL
  * @{
  */

/** @defgroup STM32L475E_IOT01_LOW_LEVEL_Private_Defines LOW LEVEL Private Def
  * @{
  */
/**
 * @brief STM32L475E IOT01 BSP Driver version number V1.0.0
   */
#define __STM32L475E_IOT01_BSP_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __STM32L475E_IOT01_BSP_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __STM32L475E_IOT01_BSP_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __STM32L475E_IOT01_BSP_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __STM32L475E_IOT01_BSP_VERSION        ((__STM32L475E_IOT01_BSP_VERSION_MAIN << 24)\
                                                 |(__STM32L475E_IOT01_BSP_VERSION_SUB1 << 16)\
                                                 |(__STM32L475E_IOT01_BSP_VERSION_SUB2 << 8 )\
                                                 |(__STM32L475E_IOT01_BSP_VERSION_RC))
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_LOW_LEVEL_Private_Variables LOW LEVEL Variables 
  * @{
  */

const uint32_t GPIO_PIN[LEDn] = {LED2_PIN};


GPIO_TypeDef* GPIO_PORT[LEDn] = {LED2_GPIO_PORT};


GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT};

const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN};

const uint16_t BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn};

USART_TypeDef* COM_USART[COMn] = {DISCOVERY_COM1,DISCOVERY_COM4};

GPIO_TypeDef* COM_TX_PORT[COMn] = {DISCOVERY_COM1_TX_GPIO_PORT,DISCOVERY_COM4_TX_GPIO_PORT};

GPIO_TypeDef* COM_RX_PORT[COMn] = {DISCOVERY_COM1_RX_GPIO_PORT,DISCOVERY_COM4_RX_GPIO_PORT};

const uint16_t COM_TX_PIN[COMn] = {DISCOVERY_COM1_TX_PIN,DISCOVERY_COM4_TX_PIN};

const uint16_t COM_RX_PIN[COMn] = {DISCOVERY_COM1_RX_PIN,DISCOVERY_COM4_RX_PIN};

const uint16_t COM_TX_AF[COMn] = {DISCOVERY_COM1_TX_AF,DISCOVERY_COM4_TX_AF};

const uint16_t COM_RX_AF[COMn] = {DISCOVERY_COM1_RX_AF,DISCOVERY_COM4_RX_AF};

I2C_HandleTypeDef hI2cHandler;
UART_HandleTypeDef hDiscoUart;
/**
  * @}
  */
/** @defgroup STM32L475E_IOT01_LOW_LEVEL_Private_FunctionPrototypes LOW LEVEL Private Function Prototypes
  * @{
  */
static void     I2Cx_MspInit(I2C_HandleTypeDef *i2c_handler);
static void     I2Cx_MspDeInit(I2C_HandleTypeDef *i2c_handler);
static void     I2Cx_Init(I2C_HandleTypeDef *i2c_handler);
static void     I2Cx_DeInit(I2C_HandleTypeDef *i2c_handler);
static HAL_StatusTypeDef I2Cx_ReadMultiple(I2C_HandleTypeDef *i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddSize, uint8_t *Buffer, uint16_t Length);
static HAL_StatusTypeDef I2Cx_WriteMultiple(I2C_HandleTypeDef *i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddSize, uint8_t *Buffer, uint16_t Length);
static HAL_StatusTypeDef I2Cx_IsDeviceReady(I2C_HandleTypeDef *i2c_handler, uint16_t DevAddress, uint32_t Trials);
static void              I2Cx_Error(I2C_HandleTypeDef *i2c_handler, uint8_t Addr);

/* Sensors IO functions */
void     SENSOR_IO_Init(void);
void     SENSOR_IO_DeInit(void);
void     SENSOR_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
uint8_t  SENSOR_IO_Read(uint8_t Addr, uint8_t Reg);
uint16_t SENSOR_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);
void     SENSOR_IO_WriteMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);
HAL_StatusTypeDef SENSOR_IO_IsDeviceReady(uint16_t DevAddress, uint32_t Trials);
void     SENSOR_IO_Delay(uint32_t Delay);

/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_LOW_LEVEL_Private_Functions LOW LEVEL Private Functions
  * @{
  */ 

/**
  * @brief  This method returns the STM32L475E IOT01 BSP Driver revision
  * @retval version: 0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t BSP_GetVersion(void)
{
  return __STM32L475E_IOT01_BSP_VERSION;
}

/**
  * @brief  Configures LEDs.
  * @param  Led: LED to be configured. 
  *          This parameter can be one of the following values:
  *            @arg  LED2
  */
void BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpio_init_structure;
  
  LEDx_GPIO_CLK_ENABLE(Led);
  /* Configure the GPIO_LED pin */
  gpio_init_structure.Pin   = GPIO_PIN[Led];
  gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull  = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  
  HAL_GPIO_Init(GPIO_PORT[Led], &gpio_init_structure);
}

/**
  * @brief  DeInit LEDs.
  * @param  Led: LED to be configured. 
  *          This parameter can be one of the following values:
  *            @arg  LED2
  */
void BSP_LED_DeInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpio_init_structure;
  
  /* DeInit the GPIO_LED pin */
  gpio_init_structure.Pin = GPIO_PIN[Led];
  
  /* Turn off LED */
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
  HAL_GPIO_DeInit(GPIO_PORT[Led], gpio_init_structure.Pin);
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: LED to be set on 
  *          This parameter can be one of the following values:
  *            @arg  LED2
  */
void BSP_LED_On(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET);
}

/**
  * @brief  Turns selected LED Off. 
  * @param  Led: LED to be set off
  *          This parameter can be one of the following values:
  *            @arg  LED2
  */
void BSP_LED_Off(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: LED to be toggled
  *          This parameter can be one of the following values:
  *            @arg  LED2
  */
void BSP_LED_Toggle(Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]);
}

/**
  * @brief  Configures button GPIO and EXTI Line.
  * @param  Button: Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button
  * @param  ButtonMode: Button mode
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_MODE_GPIO: Button will be used as simple IO
  *            @arg  BUTTON_MODE_EXTI: Button will be connected to EXTI line 
  *                                    with interrupt generation capability  
  */
void BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  GPIO_InitTypeDef gpio_init_structure;
  
  /* Enable the BUTTON clock */
  USER_BUTTON_GPIO_CLK_ENABLE();
  
  if(ButtonMode == BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    gpio_init_structure.Pin = BUTTON_PIN[Button];
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BUTTON_PORT[Button], &gpio_init_structure);
  }
  
  if(ButtonMode == BUTTON_MODE_EXTI)
  {
    /* Configure Button pin as input with External interrupt */
    gpio_init_structure.Pin = BUTTON_PIN[Button];
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    
    gpio_init_structure.Mode = GPIO_MODE_IT_RISING;
    
    HAL_GPIO_Init(BUTTON_PORT[Button], &gpio_init_structure);
    
    /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((IRQn_Type)(BUTTON_IRQn[Button]), 0x0F, 0x00);
    HAL_NVIC_EnableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  }
}

/**
  * @brief  Push Button DeInit.
  * @param  Button: Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button 
  * @note PB DeInit does not disable the GPIO clock
  */
void BSP_PB_DeInit(Button_TypeDef Button)
{
  GPIO_InitTypeDef gpio_init_structure;

  gpio_init_structure.Pin = BUTTON_PIN[Button];
  HAL_NVIC_DisableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  HAL_GPIO_DeInit(BUTTON_PORT[Button], gpio_init_structure.Pin);
}


/**
  * @brief  Returns the selected button state.
  * @param  Button: Button to be checked
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button 
  * @retval The Button GPIO pin value (GPIO_PIN_RESET = button pressed)
  */
uint32_t BSP_PB_GetState(Button_TypeDef Button)
{
  return HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

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

  /* Configure the NVIC for UART */
  /* NVIC for USART */
  if(COM==COM1){
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }else if(COM==COM4){
     HAL_NVIC_SetPriority(UART4_IRQn, 0, 1);
     HAL_NVIC_EnableIRQ(UART4_IRQn);   
  }
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
  /* USART configuration */
  huart->Instance = COM_USART[COM];
  HAL_UART_DeInit(huart);

  /* Enable USART clock */
  DISCOVERY_COMx_CLK_DISABLE(COM);

  /* DeInit GPIO pins can be done in the application 
     (by surcharging this __weak function) */

  /* GPIO pins clock, FMC clock and DMA clock can be shut down in the application 
     by surcharging this __weak function */ 
}


/*******************************************************************************
                            BUS OPERATIONS
*******************************************************************************/

/******************************* I2C Routines *********************************/
/**
  * @brief  Initializes I2C MSP.
  * @param  i2c_handler : I2C handler
  * @retval None
  */
static void I2Cx_MspInit(I2C_HandleTypeDef *i2c_handler)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /*** Configure the GPIOs ***/
  /* Enable GPIO clock */
  DISCOVERY_I2Cx_SCL_SDA_GPIO_CLK_ENABLE();

  /* Configure I2C Tx, Rx as alternate function */
  gpio_init_structure.Pin = DISCOVERY_I2Cx_SCL_PIN | DISCOVERY_I2Cx_SDA_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_OD;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = DISCOVERY_I2Cx_SCL_SDA_AF;
  HAL_GPIO_Init(DISCOVERY_I2Cx_SCL_SDA_GPIO_PORT, &gpio_init_structure);

  HAL_GPIO_Init(DISCOVERY_I2Cx_SCL_SDA_GPIO_PORT, &gpio_init_structure);

  /*** Configure the I2C peripheral ***/
  /* Enable I2C clock */
  DISCOVERY_I2Cx_CLK_ENABLE();

  /* Force the I2C peripheral clock reset */
  DISCOVERY_I2Cx_FORCE_RESET();

  /* Release the I2C peripheral clock reset */
  DISCOVERY_I2Cx_RELEASE_RESET();

  /* Enable and set I2Cx Interrupt to a lower priority */
  HAL_NVIC_SetPriority(DISCOVERY_I2Cx_EV_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(DISCOVERY_I2Cx_EV_IRQn);

  /* Enable and set I2Cx Interrupt to a lower priority */
  HAL_NVIC_SetPriority(DISCOVERY_I2Cx_ER_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(DISCOVERY_I2Cx_ER_IRQn);
}

/**
  * @brief  DeInitializes I2C MSP.
  * @param  i2c_handler : I2C handler
  * @retval None
  */
static void I2Cx_MspDeInit(I2C_HandleTypeDef *i2c_handler)
{
  GPIO_InitTypeDef  gpio_init_structure;
  
  /* Configure I2C Tx, Rx as alternate function */
  gpio_init_structure.Pin = DISCOVERY_I2Cx_SCL_PIN | DISCOVERY_I2Cx_SDA_PIN;
  HAL_GPIO_DeInit(DISCOVERY_I2Cx_SCL_SDA_GPIO_PORT, gpio_init_structure.Pin);
  /* Disable GPIO clock */
  DISCOVERY_I2Cx_SCL_SDA_GPIO_CLK_DISABLE();
  
  /* Disable I2C clock */
  DISCOVERY_I2Cx_CLK_DISABLE();
}

/**
  * @brief  Initializes I2C HAL.
  * @param  i2c_handler : I2C handler
  * @retval None
  */
static void I2Cx_Init(I2C_HandleTypeDef *i2c_handler)
{
  /* I2C configuration */
  i2c_handler->Instance              = DISCOVERY_I2Cx;
  i2c_handler->Init.Timing           = DISCOVERY_I2Cx_TIMING;
  i2c_handler->Init.OwnAddress1      = 0;
  i2c_handler->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  i2c_handler->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  i2c_handler->Init.OwnAddress2      = 0;
  i2c_handler->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  i2c_handler->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
  
  /* Init the I2C */
  I2Cx_MspInit(i2c_handler);
  HAL_I2C_Init(i2c_handler);
  
  /**Configure Analogue filter */
  HAL_I2CEx_ConfigAnalogFilter(i2c_handler, I2C_ANALOGFILTER_ENABLE);  
}

/**
  * @brief  DeInitializes I2C HAL.
  * @param  i2c_handler : I2C handler
  * @retval None
  */
static void I2Cx_DeInit(I2C_HandleTypeDef *i2c_handler)
{  /* DeInit the I2C */
  I2Cx_MspDeInit(i2c_handler);
  HAL_I2C_DeInit(i2c_handler); 
}

/**
  * @brief  Reads multiple data.
  * @param  i2c_handler : I2C handler
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @param  MemAddress: memory address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval HAL status
  */
static HAL_StatusTypeDef I2Cx_ReadMultiple(I2C_HandleTypeDef *i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t *Buffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Read(i2c_handler, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, 1000);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* I2C error occured */
    I2Cx_Error(i2c_handler, Addr);
  }
  return status;
}


/**
  * @brief  Writes a value in a register of the device through BUS in using DMA mode.
  * @param  i2c_handler : I2C handler
  * @param  Addr: Device address on BUS Bus.
  * @param  Reg: The target register address to write
  * @param  MemAddress: memory address
  * @param  Buffer: The target register value to be written
  * @param  Length: buffer size to be written
  * @retval HAL status
  */
static HAL_StatusTypeDef I2Cx_WriteMultiple(I2C_HandleTypeDef *i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t *Buffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Write(i2c_handler, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, 1000);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Re-Initiaize the I2C Bus */
    I2Cx_Error(i2c_handler, Addr);
  }
  return status;
}

/**
  * @brief  Checks if target device is ready for communication. 
  * @note   This function is used with Memory devices
  * @param  i2c_handler : I2C handler
  * @param  DevAddress: Target device address
  * @param  Trials: Number of trials
  * @retval HAL status
  */
static HAL_StatusTypeDef I2Cx_IsDeviceReady(I2C_HandleTypeDef *i2c_handler, uint16_t DevAddress, uint32_t Trials)
{ 
  return (HAL_I2C_IsDeviceReady(i2c_handler, DevAddress, Trials, 1000));
}

/**
  * @brief  Manages error callback by re-initializing I2C.
  * @param  i2c_handler : I2C handler
  * @param  Addr: I2C Address
  * @retval None
  */
static void I2Cx_Error(I2C_HandleTypeDef *i2c_handler, uint8_t Addr)
{
  /* De-initialize the I2C communication bus */
  HAL_I2C_DeInit(i2c_handler);
  
  /* Re-Initialize the I2C communication bus */
  I2Cx_Init(i2c_handler);
}

/**
  * @}
  */

/*******************************************************************************
                            LINK OPERATIONS
*******************************************************************************/
/******************************** LINK Sensors ********************************/

/**
  * @brief  Initializes Sensors low level.
  * @retval None
  */
void SENSOR_IO_Init(void)
{
  I2Cx_Init(&hI2cHandler);
}

/**
  * @brief  DeInitializes Sensors low level.
  * @retval None
  */
void SENSOR_IO_DeInit(void)
{
  I2Cx_DeInit(&hI2cHandler);
}

/**
  * @brief  Writes a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @param  Value: Data to be written
  * @retval None
  */
void SENSOR_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
  I2Cx_WriteMultiple(&hI2cHandler, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT,(uint8_t*)&Value, 1);
}

/**
  * @brief  Reads a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @retval Data to be read
  */
uint8_t SENSOR_IO_Read(uint8_t Addr, uint8_t Reg)
{
  uint8_t read_value = 0;

  I2Cx_ReadMultiple(&hI2cHandler, Addr, Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&read_value, 1);

  return read_value;
}

/**
  * @brief  Reads multiple data with I2C communication
  *         channel from TouchScreen.
  * @param  Addr: I2C address
  * @param  Reg: Register address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval HAL status
  */
uint16_t SENSOR_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length)
{
 return I2Cx_ReadMultiple(&hI2cHandler, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, Buffer, Length);
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
void SENSOR_IO_WriteMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length)
{
  I2Cx_WriteMultiple(&hI2cHandler, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, Buffer, Length);
}

/**
  * @brief  Checks if target device is ready for communication. 
  * @note   This function is used with Memory devices
  * @param  DevAddress: Target device address
  * @param  Trials: Number of trials
  * @retval HAL status
  */
HAL_StatusTypeDef SENSOR_IO_IsDeviceReady(uint16_t DevAddress, uint32_t Trials)
{ 
  return (I2Cx_IsDeviceReady(&hI2cHandler, DevAddress, Trials));
}

/**
  * @brief  Delay function used in TouchScreen low level driver.
  * @param  Delay: Delay in ms
  * @retval None
  */
void SENSOR_IO_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
