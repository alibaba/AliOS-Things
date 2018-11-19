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
#ifndef INCLUDE_PERIPHERALDRIVERPUBLIC_H_
#define INCLUDE_PERIPHERALDRIVERPUBLIC_H_

/* Include all headers which are needed by this file. */
#include "port_os/PeripheralDriverPort.h"

/* Put the type and macro definitions here */

typedef void * PeripheralDriver_InterfaceHandle_T; /**< Generic Interface reference */
typedef void * PeripheralDriver_LockHandle_T;
typedef void * PeripheralDriver_SignalHandle_T;

/**
 * @brief Declares an enumeration for various possible communication interface states call modes.
 */
typedef enum PeripheralDriver_InterfaceType_E
{
    PERIPHERAL_DRIVER_INTERFACE_TYPE_SPI, /**< @todo - update documentation */
    PERIPHERAL_DRIVER_INTERFACE_TYPE_I2C, /**< @todo - update documentation */
    PERIPHERAL_DRIVER_INTERFACE_TYPE_UART, /**< @todo - update documentation */
} PeripheralDriver_InterfaceType_T;

/**
 * @brief Declares an enumeration for various possible peripheral driver's states
 */
typedef enum PeripheralDriver_InterfaceState_E
{
    PERIPHERAL_DRIVER_STATE_RESET = 0, /**< PERIPHERAL_DRIVER_INTERFACE_STATE_RESET the Interface is at reset state */
    PERIPHERAL_DRIVER_STATE_READY, /**< PERIPHERAL_DRIVER_INTERFACE_STATE_INIT the interface has been initialized */
    PERIPHERAL_DRIVER_STATE_OPEN, /**< PERIPHERAL_DRIVER_INTERFACE_STATE_READY the interface is ready for use*/
    PERIPHERAL_DRIVER_STATE_ERROR, /**< PERIPHERAL_DRIVER_INTERFACE_STATE_ERROR the interface is at error state */
} PeripheralDriver_State_T;

struct PeripheralDriver_S
{
    PeripheralDriver_State_T State; /**< maintain a defined state for the driver */
    PeripheralDriver_InterfaceType_T InterfaceType; /**< The type of the communication interface used */
    PeripheralDriver_InterfaceHandle_T Interface; /**< The handle of the mcu communication interface */
    struct PeripheralDriverPrivate_S private; /**< the private member struct defined in the OS port */
};

/* Put the function declarations here */

#endif /* INCLUDE_PERIPHERALDRIVERPUBLIC_H_ */

