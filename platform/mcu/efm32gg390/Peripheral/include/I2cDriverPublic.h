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
#ifndef INCLUDE_I2CDRIVERPUBLIC_H_
#define INCLUDE_I2CDRIVERPUBLIC_H_

/* Include all headers which are needed by this file. */
#include "port_hw/I2cDriverPort.h"
#include "BCDS_Assert.h"

/* Put the type and macro definitions here */

struct I2cDriver_S
{
    struct PeripheralDriver_S peripheral;
    struct I2cDriverPrivate_S private;
};

struct I2cDriver_Storage_S
{
    union
    {
        uint8_t mem[sizeof(struct I2cDriver_S)];
    } storage;
};
static_assert(sizeof(struct I2cDriver_Storage_S) == sizeof(struct I2cDriver_S), "size does matter!");

/* Put the function declarations here */

#endif /* INCLUDE_I2CDRIVERPUBLIC_H_ */

