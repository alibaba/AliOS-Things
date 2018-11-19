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
#ifndef INCLUDE_PORT_OS_PERIPHERALDRIVER_NOOS_H_
#define INCLUDE_PORT_OS_PERIPHERALDRIVER_NOOS_H_

/* Include all headers which are needed by this file. */

/* Put the type and macro definitions here */
struct PeripheralDriverPrivate_S
{
    uint32_t Signal;
    uint32_t Lock;
};
///* Put the type and macro definitions here */
//struct PeripheralDriver_S
//{
//	struct PeripheralDriverPublic_S public;
//	struct PeropheralDriverPrivate_S private;
//};

/* Put the function declarations here */

#endif /* INCLUDE_PORT_OS_PERIPHERALDRIVER_NOOS_H_ */

