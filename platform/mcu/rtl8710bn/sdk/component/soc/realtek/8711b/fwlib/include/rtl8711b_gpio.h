/**
  ******************************************************************************
  * @file    rtl8711b_gpio.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the GPIO firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
  
#ifndef _RTL8711B_GPIO_H_
#define _RTL8711B_GPIO_H_

/** @addtogroup AmebaZ_Periph_Driver
  * @{
  */

/** @defgroup GPIO 
  * @brief GPIO driver modules
  * @{
  */

/** @addtogroup GPIO
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * GPIO:
  * 	- Base Address: GPIO
  * 	- Port number: support A/B/C/D four ports, AmebaZ only use A/B two ports
  * 	- Pin number: 0 ~ 31(portA),  0 ~ 6(portB)
  * 	- IRQ: GPIO_IRQ
  * 	- Only portA can be configured to interrupt mode
  *
  *****************************************************************************************
  * How to use GPIO in normal mode
  *****************************************************************************************
  *      To use GPIO peripheral in normal mode, the following steps are mandatory:
  *
  *      1. Enable the GPIO interface clock using
  *			RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
  *
  *      2. Fill the GPIO_InitStruct with the desired parameters.
  *
  *      3. configure GPIO with the configuration(GPIO mode, pull up/down) of step2:
  *			GPIO_Init(&GPIO_InitStruct)
  *
  *      4. Read or write GPIO pin according to GPIO out/in mode using
  *			GPIO_ReadDataBit() or GPIO_WriteBit()
  *
  *****************************************************************************************
  * How to use GPIO in interrupt mode
  *****************************************************************************************
  * 	  To use GPIO in interrupt mode, the following steps are mandatory:
  *
  *      1. Enable the GPIO interface clock using
  *			RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
  *
  *      2. Fill the GPIO_InitStruct with the desired parameters.
  *
  *      3. configure GPIO with the configuration(GPIO mode, pull up/down) of step2:
  *			GPIO_Init(&GPIO_InitStruct)
  *
  *      4. Register a user interrupt handler:
  *			GPIO_UserRegIrq
  *
  *      5. Configure interrupt mode(trigger, polarity, debounce):
  *			GPIO_INTMode()
  *
  *      6. Enable the interrupt of a specified pin:
  *		GPIO_INTConfig()
  *
  *      @note  Only PortA can configure interrupt mode.
  *
  *****************************************************************************************
  * How to use GPIO port (multiple GPIO pins)
  *****************************************************************************************
  * 	  To use GPIO port, the following steps are mandatory:
  *
  *      1. Enable the GPIO interface clock using
  *			RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
  *
  *      2. Configure GPIO data direction(IN/OUT)
  *			GPIO_PortDirection()
  *
  *      3. Read or write GPIO pin according to GPIO out/in mode using
  *			GPIO_PortRead()
  *			GPIO_PortWrite()
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup GPIO_Exported_Types GPIO Exported Types
  * @{
  */

/** 
  * @brief  GPIO Init structure definition  
  */
typedef struct {
	u32 GPIO_Mode;      	/*!< Specifies the operating mode for the selected pins.
								This parameter can be a value of @ref GPIO_Mode_parameter_definitions */

	u32 GPIO_PuPd;     		/*!< Specifies the operating Pull-up/Pull down for the selected pins.
								This parameter can be a value of @ref GPIO_Pull_parameter_definitions */

	u32 GPIO_ITTrigger;  	/*!< Specifies interrupt mode is level or edge trigger
								This parameter can be a value of @ref GPIO_INT_Trigger_parameter_definitions */

	u32 GPIO_ITPolarity; 	/*!< Specifies interrupt mode is high or low active trigger
								This parameter can be a value of @ref GPIO_INT_Polarity_parameter_definitions */

	u32 GPIO_ITDebounce; 	/*!< Specifies enable or disable de-bounce for interrupt
								This parameter can be a value of @ref GPIO_INT_Debounce_parameter_definitions*/

	u32 GPIO_Pin;    		/*!< Specifies the selected pins.
								This parameter contains two parts: Pin: [7:5]: port number; [4:0]: pin number */
} GPIO_InitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup GPIO_Exported_Constants GPIO Exported Constants
  * @{
  */ 

/** @defgroup GPIO_Mode_parameter_definitions
  * @{
  */ 
#define GPIO_Mode_IN			0x00 /*!< GPIO Input Mode             */
#define GPIO_Mode_OUT			0x01 /*!< GPIO Output Mode                */
#define GPIO_Mode_INT			0x02 /*!< GPIO Interrupt Mode                */
/**
  * @}
  */

/** @defgroup GPIO_Pull_parameter_definitions 
  * @{
  */ 
#define GPIO_PuPd_NOPULL		0x00 /*!< GPIO Interrnal HIGHZ */
#define GPIO_PuPd_DOWN			0x01 /*!< GPIO Interrnal Pull DOWN */
#define GPIO_PuPd_UP			0x02 /*!< GPIO Interrnal Pull UP */
#define GPIO_PuPd_SHUTDOWN		0x03 /*!< GPIO Interrnal PAD shutdown */

/**
  * @}
  */

/** @defgroup GPIO_INT_Trigger_parameter_definitions 
  * @{
  */ 
#define GPIO_INT_Trigger_LEVEL	0x0 /*!< This interrupt is level trigger  */
#define GPIO_INT_Trigger_EDGE	0x1 /*!< This interrupt is edge trigger  */

#define IS_GPIOIT_LEVEL_TYPE(TYPE) (((TYPE) == GPIO_INT_Trigger_LEVEL)\
                                    || ((TYPE) == GPIO_INT_Trigger_EDGE))                            
/**
  * @}
  */

/** @defgroup GPIO_INT_Polarity_parameter_definitions 
  * @{
  */ 
#define GPIO_INT_POLARITY_ACTIVE_LOW		0x0 /*!< Setting interrupt to low active: falling edge or low level  */
#define GPIO_INT_POLARITY_ACTIVE_HIGH		0x1 /*!< Setting interrupt to high active: rising edge or high level */

#define IS_GPIOIT_POLARITY_TYPE(TYPE) (((TYPE) == GPIO_INT_POLARITY_ACTIVE_LOW)\
                                        || ((TYPE) == GPIO_INT_POLARITY_ACTIVE_HIGH)) 
/**
  * @}
  */

/** @defgroup GPIO_INT_Debounce_parameter_definitions 
  * @{
  */ 
#define GPIO_INT_DEBOUNCE_DISABLE			0x0 /*!< Disable interrupt debounce  */
#define GPIO_INT_DEBOUNCE_ENABLE			0x1 /*!< Enable interrupt debounce   */

#define IS_GPIOIT_DEBOUNCE_TYPE(TYPE) (((TYPE) == GPIO_INT_DEBOUNCE_DISABLE)\
                                        || ((TYPE) == GPIO_INT_DEBOUNCE_ENABLE)) 
/**
  * @}
  */

/** @defgroup GPIO_Pin_State_definitions 
  * @{
  */ 
#define GPIO_PIN_LOW			0				/*!< Pin state is low  */
#define GPIO_PIN_HIGH			1				/*!< Pin state is high  */
/**
  * @}
  */

/** @defgroup GPIO_Port_definitions 
  * @{
  */   
#define GPIO_PORT_A				0				/*!< Port number A  */
#define GPIO_PORT_B				1				/*!< Port number B  */
/**
  * @}
  */

/** @defgroup GPIO_IRQ_Event_definitions 
  * @{
  */ 
#define HAL_IRQ_NONE			0				/*!< No interrupt event  */
#define HAL_IRQ_RISE			1				/*!< Rising edge or high level interrupt event  */
#define HAL_IRQ_FALL			2				/*!< Falling edge or low level interrupt event  */
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup GPIO_Exported_Functions GPIO Exported Functions
  * @{
  */ 
_LONG_CALL_ void GPIO_WriteBit(u32 GPIO_Pin, u32 BitVal);
_LONG_CALL_ u32 GPIO_ReadDataBit(u32 GPIO_Pin);
_LONG_CALL_ void GPIO_DeInit(u32 GPIO_Pin);
_LONG_CALL_ void GPIO_UserRegIrq(u32 GPIO_Pin, VOID *IrqHandler, VOID *IrqData);
_LONG_CALL_ void GPIO_INTMode(u32 GPIO_Pin, u32 NewState, u32 GPIO_ITTrigger, u32 GPIO_ITPolarity, u32 GPIO_ITDebounce);
_LONG_CALL_ void GPIO_INTConfig(u32 GPIO_Pin, u32 NewState);
_LONG_CALL_ void GPIO_Init(GPIO_InitTypeDef  *GPIO_InitStruct);
_LONG_CALL_ u32 GPIO_INTHandler(void *pData);
_LONG_CALL_ void GPIO_Direction(u32 GPIO_Pin, u32 data_direction);
_LONG_CALL_ u32 GPIO_PortRead(u32 GPIO_Port, u32 GPIO_Mask);
_LONG_CALL_ void GPIO_PortWrite(u32 GPIO_Port, u32 GPIO_Mask, u32 Port_State);
_LONG_CALL_ void GPIO_PortDirection(u32 GPIO_Port, u32 GPIO_Mask, u32 data_direction);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 


typedef void (*GPIO_IRQ_FUN)(VOID *Data, u32 Id);
typedef void (*GPIO_USER_IRQ_FUN)(u32 Id);

#endif  // end of "#define _RTL8711B_GPIO_H_"
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
