/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Robert Bosch GmbH.
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

#include "BCDS_Basics.h"
#include "BCDS_Retcode.h"
#include "BCDS_PeripheralDriver.h"

#include "PeripheralDriver.h"  /* include the private member functions headers */

typedef void * PeripheralDriver_InterfaceHandle_T; /**< Generic peripheral interface handle */
typedef void * PeripheralDriver_InterfaceLockHandle_T; /**< Generic lock reference */
typedef void * PeripheralDriver_InterfaceSignalHandle_T; /**< Generic signal reference */

Retcode_T PeripheralDriver_Initialize(PeripheralDriver_Handle_T peripheral, PeripheralDriver_InterfaceHandle_T interface)
{
    Retcode_T retcode = RETCODE_OK;
    /* check input parameters early exit*/
    PERIPHERALDRIVER_NULLCHECK_RETURN(peripheral);
    PERIPHERALDRIVER_NULLCHECK_RETURN(interface);

    /* call the OS ported version of PeripheralDriver_initialize */
    if (RETCODE_OK == retcode)
    {
        retcode = PeripheralDriver_initialize(peripheral);
    }
    if (RETCODE_OK == retcode)
    {
    	PeripheralDriver_setInterface(peripheral, interface);
    }

    return retcode;
}

Retcode_T PeripheralDriver_Open(PeripheralDriver_Handle_T peripheral, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;
    PERIPHERALDRIVER_NULLCHECK_RETURN(peripheral);

    retcode = PeripheralDriver_open(peripheral, timeout);

    return retcode;
}

Retcode_T PeripheralDriver_Close(PeripheralDriver_Handle_T peripheral)
{
    Retcode_T retcode = RETCODE_OK;
    PERIPHERALDRIVER_NULLCHECK_RETURN(peripheral);

    retcode = PeripheralDriver_close(peripheral);

    return retcode;
}

Retcode_T PeripheralDriver_Deinitialize(PeripheralDriver_Handle_T peripheral, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;
    PERIPHERALDRIVER_NULLCHECK_RETURN(peripheral);
    if (RETCODE_OK == retcode)
    {
        retcode = PeripheralDriver_deinitialize(peripheral, timeout);
    }
    return retcode;
}

Retcode_T PeripheralDriver_WaitSignal(PeripheralDriver_Handle_T peripheral, uint32_t *event, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;
    retcode = PeripheralDriver_waitSignal(peripheral, event, timeout);
    return retcode;

}

Retcode_T PeripheralDriver_Signal(PeripheralDriver_Handle_T peripheral, uint32_t event)
{
    Retcode_T retcode = RETCODE_OK;
    retcode = PeripheralDriver_signal(peripheral, event);
    return retcode;

}

/** private member function */

void PeripheralDriver_setInterface(PeripheralDriver_Handle_T peripheral, PeripheralDriver_InterfaceHandle_T interface)
{
    assert(peripheral!=NULL);
    assert(interface!=NULL);
    PeripheralDriverPtr_T peripheralDriverPtr = (PeripheralDriverPtr_T) peripheral;
    peripheralDriverPtr->Interface = interface;
}

PeripheralDriver_InterfaceHandle_T PeripheralDriver_getInterface(PeripheralDriver_Handle_T peripheral)
{
    assert(peripheral!=NULL);
    PeripheralDriverPtr_T peripheralDriverPtr = (PeripheralDriverPtr_T) peripheral;
    return peripheralDriverPtr->Interface;
}

