/*******************************************************************************
  Timer System Service Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    sys_tmr_local.h

  Summary:
    Timer System Service local declarations and definitions.

  Description:
    This file contains the Timer System Service local declarations and definitions.
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


#ifndef _SYS_TMR_LOCAL_H
#define _SYS_TMR_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "system_config.h"
#include "system/tmr/sys_tmr.h"
#include "system/common/sys_common.h"
#include "osal/osal.h"


// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* SYS TMR OBJECT INSTANCE structure

  Summary:
    System Timer object instance structure.

  Description:
    This data type defines the System Timer object instance.

  Remarks:
    None.
*/

typedef struct
{
    /* Driver Module index */
    SYS_MODULE_INDEX                    drvIndex;
    
    /* Client Handle */
    DRV_HANDLE                          driverHandle;

    /* The status of the SYS TMR Module */
    SYS_STATUS                          status;

    /* System timer operating frequency, ticks per second */ 
    uint32_t                            sysTickFreq;

    /* Number of ticks for each tick interrupt: SYS_TMR_UNIT_RESOLUTION/sysTickFreq */ 
    uint32_t                            sysTickUnitCount;

    /* Current system tick counter. Increments ONCE every tick! */
    volatile uint32_t                   sysTickCount;

    /* Previous system tick count processed. Try to adjust for
     * lost signals */
    volatile uint32_t                   sysTickPrevCount;

    /* keep an 32 bit MSB for the sysTickCount to avoid overflow */
    volatile uint32_t                   sysTickCountHigh;

    /* Underlying timer driver operating frequency */
    uint32_t                            driverFreq;

    /* Underlying timer driver divider period */
    uint32_t                            driverPeriod;

    /* Do not start polling before we receive the first alarm */
    bool                                alarmReceived;

    /* user threads protection semaphore */
    OSAL_SEM_HANDLE_TYPE                userSem;
    
} SYS_TMR_OBJECT;


// *****************************************************************************
/* Timer object state

  Summary
    Defines the enum type representing the current state of a client timer object

  Description
    This type defines the the current state of a client timer object

  Remarks:
    None
*/

typedef enum
{
    SYS_TMR_CLIENT_FREE     = 0,    // not used object
    SYS_TMR_CLIENT_ACTIVE,          // used object, counting
    SYS_TMR_CLIENT_INACTIVE,        // used object, not counting

}SYS_TMR_CLIENT_STATE;


// *****************************************************************************
/* Timer object ISR state

  Summary
    Defines the enum type representing the current ISR state of a client timer object

  Description
    This type defines the the current ISR state of a client timer object

  Remarks:
    None
*/

typedef enum
{
    SYS_TMR_CLIENT_ISR_IDLE      = 0,   // idle state
    SYS_TMR_CLIENT_ISR_ACTIVE,          // used object, counting
    SYS_TMR_CLIENT_ISR_MARK_DEL,        // used object, marked for deletion
    SYS_TMR_CLIENT_ISR_MARK_INACTIVE,   // used object, marked for becoming inactive
}SYS_TMR_CLIENT_ISR_STATE;



// *****************************************************************************
/* Timer client

  Summary
    Defines the structure required for managing a client timer object

  Description
    This defines the structure required for managing a client timer object.

  Remarks:
    Useful calculations, translations:
    - F = sysTickFreq; U = SYS_TMR_UNIT_RESOLUTION; UC = U/F (unit count);

    - ms -> ticks;          ticks = (ms x F)/1000; 
    - ms -> units;          units = ticks x UC = ((ms x F)/1000) x UC -> (ms x U)/1000; 
    - units -> ms;          ms    = (units x 1000) / (F x UC) -> (units x 1000)/U;
*/

typedef struct
{
    /* object is in use, busy, etc. SYS_TMR_CLIENT_STATE value */
    /* written by the threads, read by threads, ISR */
    int8_t                              currState;   

    /* enables ISR routine action on this client */
    uint8_t                             isrEnable;

    /* state written by the ISR, read by the threads. SYS_TMR_CLIENT_ISR_STATE */
    uint8_t                             isrState;

    /*  object flags. SYS_TMR_CLIENT_FLAGS */
    union
    {
        uint8_t         val;
        struct
        {
            uint8_t     periodic:   1;  // periodic object; periodic object are persistent
            uint8_t     auto_del:   1;   // auto delete non-periodic object
        };
    }flags;

    /* The current client tick unit count. When it reaches 0 the client alarm expired */
    int32_t                             tuCount;

    /* The client update tick unit rate */
    int32_t                             tuRate;

    /* Give this back with callback function. */
    uintptr_t                           context;

    /* Event callback */
    SYS_TMR_CALLBACK                    callback;

} SYS_TMR_CLIENT_OBJECT;


// *****************************************************************************
// *****************************************************************************
// Section: Local scope function Prototypes
// *****************************************************************************
// *****************************************************************************

static void _SYS_TMR_AlarmCallback ( uintptr_t context, uint32_t alarmCount );
static SYS_TMR_HANDLE _SYS_TMR_ClientCreate ( uint32_t periodMs, uintptr_t context, SYS_TMR_CALLBACK callback, SYS_TMR_FLAGS flags );
static bool _SYS_TMR_ClientParamSet(SYS_TMR_CLIENT_OBJECT *timerObj, uint32_t periodMs, uintptr_t context, SYS_TMR_CALLBACK callback);
static bool _SYS_TMR_Setup(SYS_TMR_OBJECT* tmrObject);

#if (SYS_TMR_INTERRUPT_NOTIFICATION)
static void _SYS_TMR_ProcessIsrAlarm(void);
static void _SYS_TMR_ProcessIsrClients(void);
#else   // ! (SYS_TMR_INTERRUPT_NOTIFICATION)
static void _SYS_TMR_ProcessTmrAlarm(void);
#endif


// *****************************************************************************
// *****************************************************************************
// Section: Extern data Definitions
// *****************************************************************************
// *****************************************************************************


#endif //#ifndef _SYS_TMR_LOCAL_H

/*******************************************************************************
 End of File
*/

