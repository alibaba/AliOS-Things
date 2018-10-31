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
 * @ingroup BCDS_ESSENTIALS
 *
 * @defgroup BCDS_HAL HAL
 * @{
 * @brief The Hardware Abstraction Layer Interface
 * @details This component defines several modules that provide interfaces to abstract
 * certain hardware related aspects.
 * @file BCDS_HAL.h
 */

#ifndef BCDS_HAL_H_
#define BCDS_HAL_H_

/* Include all headers which are needed by this file. */
#include "BCDS_Retcode.h"

#ifndef BCDS_HAL_CONFIG_H_
#include "BCDS_HALConfig.h"
#else
#error "Don't include BCDS_HALConfig.h directly instead of BCDS_HAL.h"
#endif /* BCDS_HAL_CONFIG_H_ */

/**
 * @brief Defines the number of return codes that can be used by each
 * HAL component.
 * @details Defines the number of return codes that can be defined for each
 * HAL component.
 */
#define HAL_RETURN_CODE_RANGE   (UINT32_C(50))


/**
 * @brief Declares the return codes used by the HAL components
 * @details For each component there is a defined range of return codes
 * reserved. The maximum number of codes available for a component is
 * defined by HAL_RETURN_CODE_RANGE.
 */
enum BCDS_HAL_HalRetcode_E
{
    /* HAL GENERAL RETURN CODES */
    RETCODE_HAL_GENERAL = RETCODE_FIRST_CUSTOM_CODE,
    /* ... */
    RETCODE_HAL_GENERAL_MAX = RETCODE_HAL_GENERAL + HAL_RETURN_CODE_RANGE - 1,

    /* HAL UART RETURN CODES */
    RETCODE_UART_GENERAL, /**< Start of UART return codes */
    RETCODE_UART_INIT_FAIL,
    RETCODE_UART_HAL_ERROR,
    RETCODE_UART_HAL_BUSY,
    RETCODE_UART_HAL_TIMEOUT,
    RETCODE_UART_DMA_ERROR,
    RETCODE_UART_MAX = RETCODE_UART_GENERAL + HAL_RETURN_CODE_RANGE - 1, /**< End of UART return codes */

    /* HAL LEUART RETURN CODES */
    RETCODE_LEUART_GENERAL, /**< Start of LEUART return codes */
    RETCODE_LEUART_INIT_FAIL,
    RETCODE_LEUART_HAL_ERROR,
    RETCODE_LEUART_HAL_BUSY,
    RETCODE_LEUART_HAL_TIMEOUT,
    RETCODE_LEUART_DMA_ERROR,
    RETCODE_LEUART_MAX = RETCODE_LEUART_GENERAL + HAL_RETURN_CODE_RANGE - 1, /**< End of LEUART return codes */

    /* HAL I2C RETURN CODES */
    RETCODE_IC2_GENERAL, /**< General I2C return code */

    /* ... */
    RETCODE_IC2_MAX = RETCODE_IC2_GENERAL + HAL_RETURN_CODE_RANGE - 1, /**< End of I2C return codes  */

    /* HAL SLEEP RETURN CODES */
    RETCODE_SLEEP_GENERAL, /**< General Sleep return code */
    RETCODE_SLEEP_NOSLEEP, /**< No Sleep Possible */
    RETCODE_SLEEP_MAX = RETCODE_SLEEP_GENERAL + HAL_RETURN_CODE_RANGE - 1, /**< End of SLEEP return codes */

    /* HAL TIMER RETURN CODES */
    RETCODE_TIMER_GENERAL, /**< General Timer return code */
    RETCODE_TIMER_NOHWHANDLE, /**< No hardware handle was provided */
    RETCODE_TIMER_NOINIT, /**< Timer handle was not initialized */
    RETCODE_TIMER_MAX = RETCODE_TIMER_GENERAL + HAL_RETURN_CODE_RANGE - 1, /**< End of TIMER return codes */

    /* HAL RNG RETURN CODES  */
    RETCODE_RNG_HAL_ERROR,
    RETCODE_RNG_HAL_BUSY,
    RETCODE_RNG_HAL_TIMEOUT,

    /* HAL MCU CRC RETURN CODES  */
    RETCODE_CRC_HAL_INIT_FAIL,
    RETCODE_CRC_HAL_DEINIT_FAIL,

    /* Put new declarations of component return codes here */
};

/**
* @brief Declares an enumeration for various possible communication interfaces.
* @details A field of this type is e.g. used by the BSP in the handlers
* that are created for communication devices to inform the MCU module about
* the interface type that is configured for a device.
*/
enum BCDS_HAL_InterfaceType_E
{
    BCDS_HAL_INTERFACE_NONE, /**< BCDS_HAL_INTERFACE_NONE  */
    BCDS_HAL_INTERFACE_SPI, /**< BCDS_HAL_INTERFACE_SPI specifies SPI as interface for the device */
    BCDS_HAL_INTERFACE_I2C, /**< BCDS_HAL_INTERFACE_I2C specifies I2C as interface for the device*/
    BCDS_HAL_INTERFACE_UART, /**< BCDS_HAL_INTERFACE_UART specifies UART as interface for the device*/
};

/**
* @brief Declares an enumeration for various possible transfer modes of
* communication interfaces.
* @details A field of this type is e.g. used by the BSP in the handlers
* that are created for communication devices to inform the MCU module about
* the transfer mode that is configured for a device.
*/
enum BCDS_HAL_TransferMode_E
{
    BCDS_HAL_TRANSFER_MODE_BLOCKING = 1, /**< @deprecated. Use BCDS_HAL_CallMode_E instead */
    BCDS_HAL_TRANSFER_MODE_INTERRUPT = 2, /**< BCDS_HAL_TRANSFER_MODE_INTERRUPT */
    BCDS_HAL_TRANSFER_MODE_DMA = 3, /**< BCDS_HAL_TRANSFER_MODE_DMA */
    BCDS_HAL_TRANSFER_MODE_INTERRUPT_RX_DMA_TX = 4, /** BCDS_HAL_TRANSFER_MODE_INTERRUPT_RX_DMA_TX */
    BCDS_HAL_TRANSFER_MODE_INTERRUPT_TX_DMA_RX = 5, /** BCDS_HAL_TRANSFER_MODE_INTERRUPT_TX_DMA_RX */
};

/**
* @brief Declares an enumeration for various possible  API call modes.
*/
enum BCDS_HAL_CallMode_E
{
    BCDS_HAL_CALL_MODE_NON_BLOCKING = 1,    /**< Returns before operation completed and caller expects a callback upon  completion */
    BCDS_HAL_CALL_MODE_BLOCKING = 2,        /**< Blocks function return until operation finished */
};

/**
 * @brief A data type for a general purpose hardware handle.
 */
typedef void * HWHandle_T;

/**
 * @brief A data type for a general purpose software handle.
 */
typedef void * SWHandle_T;

/**
 * @brief The data type for the Device specific handle
 */
typedef struct HAL_DeviceHandle_S HAL_DeviceHandle_T;

/**
 * @brief return the Status if we run in ISR Context or not
 *
 * @retval TRUE if we are in Isr Context
 *
 */
bool HAL_IsInISR(void);

#endif /* BCDS_HAL_H_ */
/** @} *//* End of documentation group BCDS_HAL */
/**
 * @defgroup BCDS_HAL_MCU_IF MCU
 * @brief The MCU abstraction
 * @details
 * @ingroup BCDS_HAL
 *
 * @defgroup BCDS_HAL_BSP_IF BSP
 * @brief The board support package
 * @details
 * @ingroup BCDS_HAL
 *
 */
