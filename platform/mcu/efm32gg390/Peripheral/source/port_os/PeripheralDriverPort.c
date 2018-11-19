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
#include "PeripheralDriverPort.h"

#if (BCDS_TARGET_OS == FREERTOS)
#include "PeripheralDriver_FreeRTOS.c"
#elif (BCDS_TARGET_OS == ALIOS)
#include "PeripheralDriver_AliOS.c"
#else
#include "PeripheralDriver_NoOS.c"
#endif

