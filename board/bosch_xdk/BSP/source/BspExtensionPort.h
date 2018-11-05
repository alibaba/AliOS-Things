/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @file
 * @brief Add a brief description here.
 *
 * @details Put here the documentation of this header file. Explain the interface exposed
 * by this header, e.g. what is the purpose of use, how to use it, etc.
 */
#ifndef _EXTENSIONPORT_H_
#define _EXTENSIONPORT_H_

#if BCDS_FEATURE_BSP_EXTENSIONPORT
/* Include all headers which are needed by this file. */
#include "BSP_ExtensionPort.h"
#include "em_gpio.h"

/* Put the type and macro definitions here */

/**
 * Enumeration representing gpio owner information
 */
enum BSP_ExtensionPort_gpioOwner_E
{
    BSP_EXTENSIONPORT_RESET_STATE=0,
    BSP_EXTENSIONPORT_PIN_USEDBY_I2C,
    BSP_EXTENSIONPORT_PIN_USEDBY_SPI,
    BSP_EXTENSIONPORT_PIN_USEDBY_UART,
    BSP_EXTENSIONPORT_PIN_USEDBY_TIMER,
    BSP_EXTENSIONPORT_PIN_USEDBY_ADC,
    BSP_EXTENSIONPORT_PIN_USEDBY_GPIO,
};
typedef enum BSP_ExtensionPort_gpioOwner_E BSP_ExtensionPort_gpioOwner_T;


/**
 * Structure representing gpio table
 */
struct BSP_ExtensionPort_Table_S
{
    GPIO_Port_TypeDef Port;                    /*!< GPIO port identifier */
    uint32_t Pin;                              /*!< GPIO pin identifier  */
};
typedef struct BSP_ExtensionPort_Table_S  BSP_ExtensionPort_Table_T;


/**
 * Structure representing gpio handle
 */
struct BSP_ExtensionPort_Handle_S
{
    GPIO_Mode_TypeDef Mode; /*!< GPIO pin mode identifier  */
    uint32_t gpioUsedFor :8 ; /*!< GPIO pin owner identifier, value of type defined in BSP_ExtensionPort_gpioOwner_T   */
    uint32_t gpioEnableInterrupt :1 ; /*!< GPIO pin enable interrupt identifier, value(true or false)  */
    uint32_t gpioPinValue :1 ; /*!< GPIO pin value identifier, value (low or high) */
    uint32_t gpioInterruptType :1 ; /*!< GPIO pin interrupt type identifier, value(true  or false) */
    uint32_t gpioInterruptRaisingEdge :1 ; /*!< GPIO pin interrupt raising edge value identifier,value(true or false) */
    uint32_t gpioInterruptFallingEdge :1 ; /*!< GPIO pin interrupt falling edge value identifier,value(true or false) */
    uint32_t gpioInterruptLevelValue :1 ; /*!< GPIO pin interrupt level trigger value identifier, value(low or high) */
    uint32_t gpioPinDirection :1 ; /*!< GPIO pin direction identifier, value (true or false) */
};
typedef struct BSP_ExtensionPort_Handle_S  BSP_ExtensionPort_Handle_T;

/**
 * @brief  Get function for getting the information of the current gpio owner.
 * @details This function is private to bsp package and this interface is to query, if the user is interested to
 *          to know who own this before configuring gpio to other operation during execution
 * @param [in] configItem : configuration item which needs to be configured
 *
 * @param [in] pinID : pinID for which the gpio that needs to be inquired
 *
 * @param [in] getOwner : pointer to collect the gpio owner value
 *
 * @retval RETCODE_OK in the case of success , error otherwise
 **/
Retcode_T  BSP_ExtensionPort_GetGpioOwner (BSP_ExtensionPort_PinId_T pinID,BSP_ExtensionPort_gpioOwner_T *getOwner);

/**
 * @brief  Set function for setting the information of the current gpio owner.
 * @details This function is private to bsp package and this interface is set the owner after user successfully
 *          configured the gpio for his required operation.
 *
 * @param [in] pinID : pinID for which the gpio that needs to be inquired
 *
 * @param [in] getOwner : pointer to collect the gpio owner value
 *
 * @retval RETCODE_OK in the case of success , error otherwise
 **/
Retcode_T  BSP_ExtensionPort_SetGpioOwner (BSP_ExtensionPort_PinId_T pinID, BSP_ExtensionPort_gpioOwner_T setOwner);

/**
 * @brief SPI Rx interrupt service routine used to capture events at the extension port SPI
 * resource and to forward them to the MCU driver for processing.
 */
void BSP_ExtensionPort_SpiRxIsr(void);

/**
 * @brief SPI Tx interrupt service routine used to capture events at the extension port SPI
 * resource and to forward them to the MCU driver for processing.
 */
void BSP_ExtensionPort_SpiTxIsr(void);


/**
 * @brief I2C interrupt service routine used to capture events at the extension port I2C
 * resource and to forward them to the MCU driver for processing.
 */
void BSP_ExtensionPort_I2cIsr(void);

/**
 * @brief UART Rx interrupt service routine used to capture events at the extension port UART
 * resource and to forward them to the MCU driver for processing.
 */
void BSP_ExtensionPort_UARTRxIsr(void);

/**
 * @brief UART Tx interrupt service routine used to capture events at the extension port UART
 * resource and to forward them to the MCU driver for processing.
 */
void BSP_ExtensionPort_UARTTxIsr(void);
/* Put the function declarations here */

#endif /*BCDS_FEATURE_BSP_EXTENSIONPORT */
#endif /* _EXTENSIONPORT_H_ */

