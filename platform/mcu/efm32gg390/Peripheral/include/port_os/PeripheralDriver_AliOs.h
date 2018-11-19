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
#ifndef PERIPHERAL_DRIVER_PORT_OS_PERIPHERALDRIVERALIOS_H_
#define PERIPHERAL_DRIVER_PORT_OS_PERIPHERALDRIVERALIOS_H_

/* Include all headers which are needed by this file. */
#include <k_api.h>

struct PeripheralDriverPrivate_S
{
    kmutex_t *Lock;
    kbuf_queue_t *Signal;
};

/* Put the function declarations here */

#endif /* PERIPHERAL_DRIVER_PORT_OS_PERIPHERALDRIVERALIOS_H_ */

