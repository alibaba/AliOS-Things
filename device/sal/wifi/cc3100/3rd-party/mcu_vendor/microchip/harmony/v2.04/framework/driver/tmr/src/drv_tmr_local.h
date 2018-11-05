/*******************************************************************************
  Timer Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_tmr_local.h

  Summary:
    Timer driver local declarations and definitions.

  Description:
    This file contains the Timer driver's local declarations and definitions.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _DRV_TMR_LOCAL_H
#define _DRV_TMR_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "system_config.h"
#include "system_definitions.h"
#include "driver/tmr/drv_tmr.h"
#include "driver/tmr/src/drv_tmr_variant_mapping.h"
#include "osal/osal.h"


// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// maximum divider value for 32 bit operation mode
#define     DRV_TIMER_DIVIDER_MAX_32BIT     0xffffffff

// minimum divider value for 32 bit operation mode
#define     DRV_TIMER_DIVIDER_MIN_32BIT     0x2

// maximum divider value for 16 bit operation mode
#define     DRV_TIMER_DIVIDER_MAX_16BIT     0x10000

// minimum divider value for 16 bit operation mode
#define     DRV_TIMER_DIVIDER_MIN_16BIT     0x2


// *****************************************************************************
/* TMR Driver Module Instance Object

  Summary:
    Defines the object required for the maintenance of the hardware instance.

  Description:
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
*/

typedef struct
{
    /* The status of the driver */
    SYS_STATUS                                              status;

    /* Time Period Value */
    uint32_t                                                timerPeriod;

    /* Pre-scaler Selection from the processor enumeration */
    TMR_PRESCALE                                            prescale;

    /* Timer Peripheral Id */
    TMR_MODULE_ID                                           tmrId;

	/* Timer Peripheral Id associated with Timer's 32-bi mode */
    // TMR_MODULE_ID                                         	tmrNextId;

    /* Timer clock source */
    DRV_TMR_CLK_SOURCES                                     clockSource;

    /* Interrupt Source for TMR Interrupt */
    INT_SOURCE                                              interruptSource;

    /* TMR Driver HW instance operational flags */
    bool                                                    inUse;

	/* operation mode */
	DRV_TMR_OPERATION_MODE                                  operMode;

    /* Asynchronous write enable, if supported */
    bool                                                    asyncWriteEnable;       

    /* Semaphore to protect 'alarm count' */
    OSAL_SEM_HANDLE_TYPE                                    alarmCountSemaphore;

} DRV_TMR_MODULE_INSTANCE;


// internal client status
typedef enum
{
    DRV_TMR_CLIENT_OBJ_ERROR    = -1,     // some error
    DRV_TMR_CLIENT_OBJ_CLOSED   = 0,      // closed, free
    DRV_TMR_CLIENT_OBJ_READY    = 1,      // open only, not programmed/running
    DRV_TMR_CLIENT_OBJ_ARMED    = 2,      // open, valid data, not yet running
    DRV_TMR_CLIENT_OBJ_RUNNING  = 3,      // open, running
}DRV_TMR_CLIENT_OBJ_STATUS;

// *****************************************************************************
/* TMR Driver Client Object instance

  Summary:
    Defines the object required for the maintenance of the timer driver client.

  Description:
    This defines the object required for the maintenance of the timer
    driver client.

  Remarks:
    None.
*/


typedef struct
{
    /* Modul einstance this client belongs to */
    DRV_TMR_MODULE_INSTANCE*                    pModInst;

	/* Do the alarm periodically */
	bool                                        alarmPeriodic;
	
    /* Status of the client object */
    DRV_TMR_CLIENT_OBJ_STATUS                   clientStatus;

    /* Client's alarm counter */
    uint32_t                                    alarmCount;

    /* Alarm callback */
    DRV_TMR_CALLBACK                            alarmCallback;

    /* Client context */
    uintptr_t                                   context;


} DRV_TMR_CLIENT_OBJ;



// *****************************************************************************
/*  Definitions of functions used internally in the timer driver
*/

static bool _DRV_TMR_InstanceSetup ( DRV_TMR_MODULE_INSTANCE *pTmrInst);

#endif //#ifndef _DRV_TMR_LOCAL_H

/*******************************************************************************
 End of File
*/

