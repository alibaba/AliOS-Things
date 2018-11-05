/*******************************************************************************
  Timer Device Driver dynamic implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_tmr.c

  Summary:
    Timer device driver dynamic implementation.

  Description:
    The Timer device driver provides a simple interface to manage the Timer 
    modules on Microchip micro controllers.  This file implements the core 
    interface routines for the Timer driver in dynamic mode. 
    
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


// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>
#include <string.h>
#include "driver/tmr/src/drv_tmr_local.h"
#include "system/int/sys_int.h"
#include  "driver/tmr/src/dynamic/pic32cz/timer_pic32cz.h"
#include "system/clk/sys_clk.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

// each driver module can support multiple clients
//
typedef struct
{
    DRV_TMR_MODULE_INSTANCE         tmrInstance;     // the current timer module
    DRV_TMR_CLIENT_OBJ              tmrClients[1];   // Timer driver supports only Single Client. If you have multiple clients looking for timer, then use Timer System Service.
}DRV_TMR_MODULE_DESCRIPTOR;


static DRV_TMR_MODULE_DESCRIPTOR    gDrvTmrDcpt[DRV_TMR_INSTANCES_NUMBER];       // the supported timer modules
static unsigned int                 gDrvTmrInstances = 0;                        // current number of initialized instances 

static const DRV_TMR_INIT defaultTmrInit = 
{
    {SYS_MODULE_POWER_RUN_FULL},
    TMR_ID_1,
    DRV_TMR_CLKSOURCE_MCK_PRESCALE_128,
    0,
    TC0_CH0_IRQn,
    DRV_TMR_OPERATION_MODE_16_BIT,
    false,
};


// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static void DRV_TMR_ProcessEvents ( DRV_TMR_MODULE_DESCRIPTOR* pDcpt );


// sets a clean status
static void _DRV_TMR_ClientClear(DRV_TMR_CLIENT_OBJ* pObj)
{
    pObj->alarmCallback = 0 ;
    pObj->alarmPeriodic = false;
    pObj->pModInst->timerPeriod = 0;
    pObj->alarmCount = 0;

    pObj->clientStatus = DRV_TMR_CLIENT_OBJ_READY;
}

// sets a client parameters
static void _DRV_TMR_ClientSetParams(
        DRV_TMR_CLIENT_OBJ* pObj, 
        uint32_t period, 
        bool isPeriodic,                        
        uintptr_t context, 
        DRV_TMR_CALLBACK callBack
)
{
    pObj->alarmCallback = callBack ;
    pObj->alarmPeriodic = isPeriodic;
    pObj->context = context;
    pObj->pModInst->timerPeriod = period;
    pObj->alarmCount = 0;
}


static bool _DRV_TMR_Suspend(DRV_TMR_CLIENT_OBJ* pObj)
{
    if(pObj->clientStatus == DRV_TMR_CLIENT_OBJ_RUNNING)
    {
        SYS_INT_SourceDisable( pObj->pModInst->interruptSource );
    if(pObj->pModInst->operMode == DRV_TMR_OPERATION_MODE_16_BIT)
    {
        tmr_stop(pObj->pModInst->tmrId);
    }
    else
    {
        tmr_32bit_stop(pObj->pModInst->tmrId);
    }
        return true;
    }

    return false;
}

static void _DRV_TMR_Resume(DRV_TMR_CLIENT_OBJ* pObj, bool resume)
{
    if(resume)
    {
        SYS_INT_SourceStatusClear ( pObj->pModInst->interruptSource );
        SYS_INT_SourceEnable( pObj->pModInst->interruptSource );

        /* Start the Timer */
    if(pObj->pModInst->operMode == DRV_TMR_OPERATION_MODE_16_BIT)
    {
        tmr_start(pObj->pModInst->tmrId);
    }
    else
    {
        tmr_32bit_start(pObj->pModInst->tmrId);
    }
    }
}

// releases a client
static __inline__ void __attribute__((always_inline)) _DRV_TMR_ClientDelete(DRV_TMR_CLIENT_OBJ* pObj, bool suspend)
{
    if(suspend)
    {
        _DRV_TMR_Suspend(pObj);
    }
    // add other delete code here
    pObj->clientStatus = DRV_TMR_CLIENT_OBJ_CLOSED;
}

static bool _DRV_TMR_ClockSourceSet(DRV_TMR_MODULE_INSTANCE *pTmr, DRV_TMR_CLK_SOURCES clockSource)
{
    bool clockSet = true;
    /* Clock Source Selection */
    if(pTmr->operMode == DRV_TMR_OPERATION_MODE_16_BIT)
    {
        tmr_ClockSourceSelect ( pTmr->tmrId, clockSource );
    }
    else
    {
        tmr_32bit_ClockSourceSelect ( pTmr->tmrId, clockSource );
    }
    return clockSet;
}

static bool _DRV_TMR_InstanceSetup ( DRV_TMR_MODULE_INSTANCE *pTmrInst)
{    
    TMR_MODULE_ID timerId = pTmrInst->tmrId;

    /* 1. Clock Source Selection and prescaler */
    if(!_DRV_TMR_ClockSourceSet(pTmrInst, pTmrInst->clockSource))
    {
        return false;
    }
    if(pTmrInst->operMode == DRV_TMR_OPERATION_MODE_16_BIT)
    {
        tmr_setup(timerId);
    }
    else
    {
        tmr_32bit_setup(timerId);
    }
    /* Successfully initialized */
    return true;

}

static bool _DRV_TMR_RootInitialize(void)
{
    memset(&gDrvTmrDcpt, 0, sizeof(gDrvTmrDcpt));

    return true;
}

static /*__inline__*/ DRV_TMR_MODULE_DESCRIPTOR* /*__attribute__((always_inline))*/ _DRV_TMR_ModuleObj(SYS_MODULE_OBJ object, bool checkReady)
{
    if(gDrvTmrInstances != 0)
    {   // module initialized
        DRV_TMR_MODULE_DESCRIPTOR* pDcpt = (DRV_TMR_MODULE_DESCRIPTOR*)object;

        /* check the validity of the handle */
        if(pDcpt - gDrvTmrDcpt  <= sizeof(gDrvTmrDcpt)/sizeof(*gDrvTmrDcpt))
        {
            if(pDcpt->tmrInstance.inUse != false)
            {
                if(checkReady == false || pDcpt->tmrInstance.status == SYS_STATUS_READY)
                {   // success
                    return pDcpt;
                }
            }
        }
    }

    return 0;
}

static void DRV_TMR_ProcessEvents ( DRV_TMR_MODULE_DESCRIPTOR* pDcpt )
{
    DRV_TMR_MODULE_INSTANCE* pTmrInst = &pDcpt->tmrInstance;

    /* Check if the Timer Interrupt/Status is set */
    if ( tmr_interrupt_status ( pTmrInst->tmrId ) != false )
    {
        /* Clear Timer Interrupt/Status Flag */
        SYS_INT_SourceStatusClear ( pTmrInst->interruptSource );

        // process clients
        int ix;
        DRV_TMR_CLIENT_OBJ* pClient = pDcpt->tmrClients;
        for(ix = 0; ix < sizeof(pDcpt->tmrClients) / sizeof(*pDcpt->tmrClients); ix++, pClient++)
        {
            if(pClient->clientStatus == DRV_TMR_CLIENT_OBJ_RUNNING)
            {
                /* increment the alarm */
                pClient->alarmCount++;

                /* callback the application routine */
                if( pClient->alarmCallback != NULL )
                {
                    (*pClient->alarmCallback)( pClient->context, pClient->alarmCount);
                }

                /* Alarm in one shot mode */
                if ( pClient->alarmPeriodic != true )
                {
                    _DRV_TMR_Suspend(pClient);     // Stop Timer
                    _DRV_TMR_ClientClear(pClient); // Clear timer client
                }
            }
        }

    }
} 

static __inline__ DRV_TMR_CLIENT_OBJ* __attribute__((always_inline)) _DRV_TMR_ClientObj(DRV_HANDLE handle)
{
    /* check the validity of the handle */
    if(handle != DRV_HANDLE_INVALID)
    {
        DRV_TMR_CLIENT_OBJ* pObj = (DRV_TMR_CLIENT_OBJ*)handle;
        if(pObj->clientStatus > 0)
        {
            return pObj;
        }
    }

    return 0;
}

// *****************************************************************************
// *****************************************************************************
// Section: Driver System Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_TMR_Initialize 
    ( 
        const SYS_MODULE_INDEX drvIndex,
        const SYS_MODULE_INIT * const init 
    )

  Summary:
    Initializes the Timer driver.

  Description:
    This function initializes the Timer driver, making it ready for clients to
    open and use it.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

SYS_MODULE_OBJ DRV_TMR_Initialize 
( 
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT  * const init 
)
{
    if(gDrvTmrInstances == 0)
    {
        if(!_DRV_TMR_RootInitialize())
        {   // failed to initialize
            return SYS_MODULE_OBJ_INVALID;
        }
    }

    DRV_TMR_MODULE_DESCRIPTOR* pTmrDcpt;
    DRV_TMR_MODULE_INSTANCE*   pTmrInst;
    const DRV_TMR_INIT * tmrInit;
    
    /* Validate the driver index. Check if the specified driver
    index is in valid range */
    if ( drvIndex >= DRV_TMR_INSTANCES_NUMBER )
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Check if this hardware instance was already initialized */
    pTmrDcpt = gDrvTmrDcpt + drvIndex; 
    if ( pTmrDcpt->tmrInstance.inUse != false )
    {
        return (SYS_MODULE_OBJ)pTmrDcpt;
    }

    /* Copy to local variables */
    tmrInit = ( const DRV_TMR_INIT * ) init;
    if(tmrInit == 0)
    {
        tmrInit = &defaultTmrInit;
    }

    // TODO: verify the validity of the passed in tmrId!

    /* Object is valid, set it in use */
    pTmrInst = &pTmrDcpt->tmrInstance;
    pTmrInst->inUse = true;

    /* Initialize the Interrupt Source */
    pTmrInst->tmrId = tmrInit->tmrId;
    pTmrInst->clockSource = tmrInit->clockSource;
    pTmrInst->prescale = tmrInit->prescale;
    pTmrInst->interruptSource = tmrInit->interruptSource;
    pTmrInst->operMode = tmrInit->mode;
    pTmrInst->asyncWriteEnable = tmrInit->asyncWriteEnable;

    /* Setup the Hardware */
    if ( _DRV_TMR_InstanceSetup (pTmrInst) == false )
    {   /* Hardware update fail. Set the status. */
        pTmrInst->inUse = false;
        return SYS_MODULE_OBJ_INVALID;
    }

    SYS_INT_SourceDisable( pTmrInst->interruptSource );

    /* Set the current driver state */
    pTmrInst->status = SYS_STATUS_READY;

    gDrvTmrInstances++;

    /* Return the driver handle */
    return (SYS_MODULE_OBJ)pTmrDcpt;

} /* DRV_TMR_Initialize */

// *****************************************************************************
/* Function:
    void DRV_TMR_Deinitialize ( SYS_MODULE_OBJ object )

  Summary:
    Deinitializes the specified instance of the Timer driver.

  Description:
    Deinitializes the specified instance of the Timer driver, disabling
    its operation (and any hardware). All internal data is invalidated.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_Deinitialize ( SYS_MODULE_OBJ object )
{
    int ix;
    DRV_TMR_MODULE_DESCRIPTOR* pDcpt = _DRV_TMR_ModuleObj(object, false);

    if(pDcpt)
    {
        SYS_INT_SourceDisable(pDcpt->tmrInstance.interruptSource);

        DRV_TMR_CLIENT_OBJ* pObj = pDcpt->tmrClients; 
        for(ix = 0; ix < sizeof(pDcpt->tmrClients) / sizeof(*pDcpt->tmrClients); ix++, pObj++)
        {
            if(pObj->clientStatus > 0)
            {
                _DRV_TMR_ClientDelete(pObj, true);
            }
        }

        pDcpt->tmrInstance.inUse = false;
        /* Update the Timer Driver status */
        pDcpt->tmrInstance.status =  SYS_STATUS_UNINITIALIZED ;
    }
} 

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_TMR_Status ( SYS_MODULE_OBJ object )

  Summary:
    Provides the current status of the Timer driver.

  Description:
    This function provides the current status of the Timer driver.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

SYS_STATUS DRV_TMR_Status ( SYS_MODULE_OBJ object )
{
    DRV_TMR_MODULE_DESCRIPTOR* pDcpt = _DRV_TMR_ModuleObj(object, false);

    return pDcpt ? pDcpt->tmrInstance.status : SYS_STATUS_ERROR;

}

// *****************************************************************************
/* Function:
    void DRV_TMR_Tasks ( SYS_MODULE_OBJ object )

  Summary:
    Maintains the driver's state machine.

  Description:
    This function is used to maintain the driver's internal state machine and
    processes the timer events.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_Tasks ( SYS_MODULE_OBJ object )
{
    DRV_TMR_MODULE_DESCRIPTOR* pDcpt = _DRV_TMR_ModuleObj(object, true);

    if(pDcpt)
    {
        DRV_TMR_ProcessEvents(pDcpt);
    }

}

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Client Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_TMR_Open 
    ( 
        const SYS_MODULE_INDEX index,
        const DRV_IO_INTENT    intent
    )

  Summary:
    Opens the specified Timer driver instance and returns a handle to it.
  
  Description:
    This function opens the specified Timer driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver.  Timer driver does not
    support multiple clients. If two tasks want to use the timer, one should
    wait until the other one gets closed. 

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

DRV_HANDLE DRV_TMR_Open 
( 
    const SYS_MODULE_INDEX index, 
    const DRV_IO_INTENT ioIntent 
)
{

    if (index >= DRV_TMR_INSTANCES_NUMBER || ( ioIntent & DRV_IO_INTENT_SHARED ) != 0)
    {   // only exclusive access for now.
        return DRV_HANDLE_INVALID;
    }

    if(gDrvTmrInstances != 0)
    {   // module initialized
        DRV_TMR_MODULE_DESCRIPTOR* pDcpt = gDrvTmrDcpt + index;
        if(pDcpt->tmrInstance.inUse != false)
        {   // search a free client
            int ix;

            DRV_TMR_CLIENT_OBJ* dObj = pDcpt->tmrClients + 0;
            for(ix = 0; ix < sizeof(pDcpt->tmrClients) / sizeof(*pDcpt->tmrClients); ix++, dObj++)
            {
                if(dObj->clientStatus == DRV_TMR_CLIENT_OBJ_CLOSED)
                {   // found free client
                    dObj->pModInst = &pDcpt->tmrInstance;
                    dObj->clientStatus = DRV_TMR_CLIENT_OBJ_READY;
                    return ( ( DRV_HANDLE ) dObj );
                }
            }
        }
    }

    return DRV_HANDLE_INVALID;
}

// *****************************************************************************
/* Function:
    void DRV_TMR_Close ( DRV_HANDLE handle )

  Summary:
    Closes an opened instance of the Timer driver.

  Description:
    This function closes an opened instance of the Timer driver, invalidating
    the handle.
  
  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_Close ( DRV_HANDLE handle )
{
    DRV_TMR_CLIENT_OBJ *pObj = _DRV_TMR_ClientObj(handle);

    if (pObj)
    {
        _DRV_TMR_ClientDelete(pObj, true);
    }
} 

// *****************************************************************************
/* Function:
    DRV_TMR_CLIENT_STATUS DRV_TMR_ClientStatus ( DRV_HANDLE handle )

  Summary:
    Gets the status of the client operation.

  Description:
    This function gets the status of the recently completed client level
    operation.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

DRV_TMR_CLIENT_STATUS DRV_TMR_ClientStatus ( DRV_HANDLE handle )
{
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);
    DRV_TMR_CLIENT_STATUS cliStat = DRV_TMR_CLIENT_STATUS_INVALID;
    
    if(dObj)
    {
        if( dObj->clientStatus == DRV_TMR_CLIENT_OBJ_RUNNING)
        {
            cliStat = DRV_TMR_CLIENT_STATUS_RUNNING;
        }
        else
        {
            cliStat = DRV_TMR_CLIENT_STATUS_READY;
        }
    }
   
   return cliStat;

}

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Counter functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void DRV_TMR_CounterValueSet ( DRV_HANDLE handle, uint32_t counterPeriod )

  Summary:
    Updates the Timer's counter register.

  Description:
    This function updates the Timer's value in the counter register.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_CounterValueSet ( DRV_HANDLE handle, uint32_t value )
{
	return;
}

// *****************************************************************************
/* Function:
    uint32_t DRV_TMR_CounterValueGet ( DRV_HANDLE handle )

  Summary:
    Reads the Timer's counter register.

  Description:
    This function returns the Timer's value in the counter register.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

uint32_t DRV_TMR_CounterValueGet ( DRV_HANDLE handle )
{

    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);

    if(dObj)
    {
        if(dObj->pModInst->operMode == DRV_TMR_OPERATION_MODE_16_BIT)
        {
	        return tmr_counter_get( dObj->pModInst->tmrId );
        }
        else
        {
            return tmr_counter_32bit_get( dObj->pModInst->tmrId );
        }
    }
    return 0;
}

// *****************************************************************************
/* Function:
    void DRV_TMR_CounterClear ( DRV_HANDLE handle )

  Summary:
    Clears the Timer's counter register.

  Description:
    This function clears the Timer's value in the counter register.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_CounterClear ( DRV_HANDLE handle )
{
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);

    if(dObj)
    {
        if(dObj->pModInst->operMode == DRV_TMR_OPERATION_MODE_16_BIT)
        {
	        return tmr_counter_clear( dObj->pModInst->tmrId );
        }
        else
        {
            return tmr_counter_32bit_clear( dObj->pModInst->tmrId );
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Alarm functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    bool DRV_TMR_AlarmRegister 
    ( 
        DRV_HANDLE handle, 
        uint32_t divider, 
        bool isPeriodic, 
        uintptr_t context, 
        DRV_TMR_CALLBACK callBack 
    )

  Summary:
    Sets up an alarm.

  Description:
    This function sets up an alarm, allowing the client to receive a callback
    from the driver when the timer counter reaches zero.  Alarms can be one-shot
    or periodic.  A periodic alarm will reload the timer and generate alarm
    until stopped.  The alarm frequency is: DRV_TMR_CounterFrequencyGet() /
    divider;

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

bool DRV_TMR_AlarmRegister 
( 
    DRV_HANDLE handle, 
    uint32_t divider, 
    bool isPeriodic,
    uintptr_t context, 
    DRV_TMR_CALLBACK callBack 
)
{
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);

    if(dObj == 0)
    {
        return false;
    }

    DRV_TMR_OPERATION_MODE operMode = dObj->pModInst->operMode;
    bool success = false;

    if(operMode == DRV_TMR_OPERATION_MODE_32_BIT)
    {
        success = (divider >= DRV_TIMER_DIVIDER_MIN_32BIT && divider <= DRV_TIMER_DIVIDER_MAX_32BIT);
    }
    else
    {
        success = (divider >= DRV_TIMER_DIVIDER_MIN_16BIT && divider <= DRV_TIMER_DIVIDER_MAX_16BIT);
    }
    if(success)
    {
        bool resume = _DRV_TMR_Suspend(dObj);
        _DRV_TMR_ClientSetParams(dObj, divider - 1, isPeriodic, context, callBack);

        if(dObj->pModInst->operMode == DRV_TMR_OPERATION_MODE_16_BIT)
        {
            tmr_period_set ( dObj->pModInst->tmrId, (uint16_t)divider);
        }
        else
        {
            tmr_period_32bit_set ( dObj->pModInst->tmrId, divider);
        }
		
        if(resume)
        {
            _DRV_TMR_Resume(dObj, resume);
        }
        else
        {
            dObj->clientStatus = DRV_TMR_CLIENT_OBJ_ARMED;
        }
    }

    return success;
}

// *****************************************************************************
/* Function:
    bool DRV_TMR_AlarmDisable ( DRV_HANDLE handle);

  Summary:
    Disables an alarm signal.

  Description:
    This function allows the client to disable an alarm generation.
    Use DRV_TMR_AlarmEnable to re-enable.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/


bool DRV_TMR_AlarmDisable ( DRV_HANDLE handle)
{
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);

    if(dObj && dObj->clientStatus == DRV_TMR_CLIENT_OBJ_RUNNING)
    {
        return SYS_INT_SourceDisable ( dObj->pModInst->interruptSource ) ;
    }

    return false;
}

// *****************************************************************************
/* Function:
    void DRV_TMR_AlarmEnable ( DRV_HANDLE handle, bool enable );

  Summary:
    Re-enables an alarm signal.

  Description:
    This function allows the client to re-enable an alarm after it has been
    disabled by a DRV_TMR_AlarmDisable call.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_AlarmEnable ( DRV_HANDLE handle, bool enable )
{
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);

    if(dObj && dObj->clientStatus == DRV_TMR_CLIENT_OBJ_RUNNING && enable)
    {
        SYS_INT_SourceEnable ( dObj->pModInst->interruptSource ) ;
    }
}

// *****************************************************************************
/* Function:
    void DRV_TMR_AlarmPeriodSet ( DRV_HANDLE handle, uint32_t value )

  Summary:
    Updates the Timer's period.

  Description:
    This function updates the Timer's period.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_AlarmPeriodSet ( DRV_HANDLE handle, uint32_t   period )
{
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);

    if(dObj)
    {
        bool resume = _DRV_TMR_Suspend(dObj);
        /* Client update with the new count value */
        dObj->pModInst->timerPeriod = period;


        if(dObj->pModInst->operMode == DRV_TMR_OPERATION_MODE_16_BIT)
        {
            tmr_period_set ( dObj->pModInst->tmrId, (uint16_t)period);
        }
        else
        {
            tmr_period_32bit_set ( dObj->pModInst->tmrId, period);
        }

        if(resume)
        {
            _DRV_TMR_Resume(dObj, resume);
        }
        else
        {
            dObj->clientStatus = DRV_TMR_CLIENT_OBJ_ARMED;
        }
    }
} 

// *****************************************************************************
/* Function:
    uint32_t DRV_TMR_AlarmPeriodGet ( DRV_HANDLE handle )

  Summary:
    Provides the Timer's period.

  Description:
    This function gets the Timer's period.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

uint32_t DRV_TMR_AlarmPeriodGet ( DRV_HANDLE handle )
{
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);

    if(dObj)
    {
        return  dObj->pModInst->timerPeriod;
    }

    return 0;
} 

// *****************************************************************************
/* Function:
    void DRV_TMR_AlarmDeregister ( DRV_HANDLE handle )

  Summary:
    Removes a previously set alarm.

  Description:
    This function removes a previously set alarm.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_AlarmDeregister ( DRV_HANDLE handle )
{
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);

    if(dObj)
    {
        _DRV_TMR_Suspend(dObj);
        _DRV_TMR_ClientClear(dObj);
    }
} 

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Operation Control functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    bool DRV_TMR_Start ( DRV_HANDLE handle )

  Summary:
    Starts the Timer counting.

  Description:
    This function starts the Timer counting.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

bool DRV_TMR_Start ( DRV_HANDLE handle )
{
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);

    if(dObj && dObj->clientStatus == DRV_TMR_CLIENT_OBJ_ARMED)
    {
        _DRV_TMR_Resume(dObj, true);

        /* Update the Client Status */
        dObj->clientStatus = DRV_TMR_CLIENT_OBJ_RUNNING;
        return true;
    }
    return false;
}

// *****************************************************************************
/* Function:
    void DRV_TMR_Stop ( DRV_HANDLE handle )

  Summary:
    Stops the Timer from counting.

  Description:
    This function stops the running Timer from counting.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

void DRV_TMR_Stop ( DRV_HANDLE handle )
{
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);

    if(dObj && dObj->clientStatus == DRV_TMR_CLIENT_OBJ_RUNNING)
    {   /* Stop the Timer from running */
        _DRV_TMR_Suspend(dObj);
        SYS_INT_SourceStatusClear ( dObj->pModInst->interruptSource );
        /* Update the Client Status */
        dObj->clientStatus = DRV_TMR_CLIENT_OBJ_ARMED;
    }
}

// *****************************************************************************
/* Function:
    uint32_t DRV_TMR_AlarmHasElapsed ( DRV_HANDLE handle )

  Summary:
    Provides the status of Timer's period elapse.

  Description:
    This function returns the number of times Timer's period has elapsed since
    last call to this API has made. On calling this API, the internally
    maintained counter will be cleared and count will be started again from next
    elapse.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

uint32_t DRV_TMR_AlarmHasElapsed ( DRV_HANDLE handle )
{
    uint32_t alarmCountTemp;
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);

    if(dObj)
    {
        alarmCountTemp = dObj->alarmCount;
        dObj->alarmCount = 0;
        return( alarmCountTemp );
    }

    return 0;
}

// *****************************************************************************
/* Function:
    DRV_TMR_OPERATION_MODE DRV_TMR_OperationModeGet(DRV_HANDLE handle)

  Summary:
    This function gets the currently selected operation mode.

  Description:
    This function gets the currently selected 16/32 bit operation mode.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

DRV_TMR_OPERATION_MODE DRV_TMR_OperationModeGet(DRV_HANDLE handle)
{
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);

    if(dObj)
    {
        return dObj->pModInst->operMode;
    }

    return DRV_TMR_OPERATION_MODE_NONE;
}

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Miscellaneous information functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint32_t DRV_TMR_CounterFrequencyGet ( DRV_HANDLE handle )

  Summary:
    Provides the Timer input frequency.

  Description:
    This function provides the Timer input frequency. Input frequency is the
    clock to the Timer register. 

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

uint32_t DRV_TMR_CounterFrequencyGet ( DRV_HANDLE handle )
{
    
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);

    if(dObj)
    {
		switch(dObj->pModInst->clockSource)
		{
			case DRV_TMR_CLKSOURCE_MCK_PRESCALE_8: 
				{
					return(SYS_CLK_FrequencyGet(SYS_CLK_MASTER)/8);
					break;
				}
			case DRV_TMR_CLKSOURCE_MCK_PRESCALE_32: 
				{
					return(SYS_CLK_FrequencyGet(SYS_CLK_MASTER)/32);
					break;
				}
			case DRV_TMR_CLKSOURCE_MCK_PRESCALE_128: 
				{
					return(SYS_CLK_FrequencyGet(SYS_CLK_MASTER)/128);
				}
			case DRV_TMR_CLKSOURCE_PCK: 
				{
					return(SYS_CLK_FrequencyGet(SYS_CLK_PROGRAMMABLE_6));
				}				
			default:
            {
				return 0;
                break;
            }				
		}
	}
    return 0;
}

// *****************************************************************************
/* Function:
    DRV_TMR_OPERATION_MODE DRV_TMR_DividerRangeGet 
    ( 
        DRV_HANDLE handle, 
        DRV_TMR_DIVIDER_RANGE* pDivRange
    )

  Summary:
    Returns the Timer divider values.

  Description:
    This function provides the Timer operating mode and divider range.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

DRV_TMR_OPERATION_MODE DRV_TMR_DividerRangeGet 
( 
    DRV_HANDLE handle,
    DRV_TMR_DIVIDER_RANGE * pDivRange
)
{
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);

    if(dObj)
    {
        DRV_TMR_OPERATION_MODE operMode = dObj->pModInst->operMode;

        if(pDivRange)
        {

            if(operMode == DRV_TMR_OPERATION_MODE_32_BIT)
            {
                pDivRange->dividerMax = DRV_TIMER_DIVIDER_MAX_32BIT;
                pDivRange->dividerMin = DRV_TIMER_DIVIDER_MIN_32BIT;
            }
            else
            {
                pDivRange->dividerMax = DRV_TIMER_DIVIDER_MAX_16BIT;
                pDivRange->dividerMin = DRV_TIMER_DIVIDER_MIN_16BIT;
            }
            pDivRange->dividerStep = 1;
        }
        return operMode;
    }

    return DRV_TMR_OPERATION_MODE_NONE;

}

// *****************************************************************************
// /* Function:
//     bool DRV_TMR_GateModeSet ( DRV_HANDLE handle )
// 
//   Summary:
//     Enables the Gate mode.
// 
//   Description:
//     This function enables the Gated mode of Timer. User can measure the duration
//     of an external signal in this mode. Once the Gate mode is enabled, Timer
//     will start on the raising edge of the external signal. It will keep counting
//     until the next falling edge. 
// 
//   Remarks:
//     Refer to drv_tmr.h for usage information.
// */
// 
// bool DRV_TMR_GateModeSet ( DRV_HANDLE handle )
// {
//     DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);
//     if(dObj)
//     {
// #if defined(PLIB_TMR_ExistsGatedTimeAccumulation)
//         if(PLIB_TMR_ExistsGatedTimeAccumulation( dObj->pModInst->tmrId))
//         {
//             bool resume = _DRV_TMR_Suspend(dObj);
//             PLIB_TMR_GateEnable( dObj->pModInst->tmrId );
//             _DRV_TMR_Resume(dObj, resume);
//             return true;
//         }
// #endif
//     }
// 
//     return false;
// }
//  
// *****************************************************************************
// /* Function:
//     bool DRV_TMR_GateModeClear ( DRV_HANDLE handle )
// 
//   Summary:
//     Enables the Gate mode.
// 
//   Description:
//     This function enables the Gated mode of Timer. User can measure the duration
//     of an external signal in this mode. Once the Gate mode is enabled, Timer
//     will start on the raising edge of the external signal. It will keep counting
//     until the next falling edge. 
// 
//   Remarks:
//     Refer to drv_tmr.h for usage information.
// */
// 
// bool DRV_TMR_GateModeClear ( DRV_HANDLE handle )
// {
//     DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);
//     if(dObj)
//     {
// #if defined(PLIB_TMR_ExistsGatedTimeAccumulation)
//         if(PLIB_TMR_ExistsGatedTimeAccumulation( dObj->pModInst->tmrId))
//         {
//             bool resume = _DRV_TMR_Suspend(dObj);
//             PLIB_TMR_GateDisable( dObj->pModInst->tmrId );
//             _DRV_TMR_Resume(dObj, resume);
//             return true;
//         }
// #endif
//     }
//     return false;
// }

// *****************************************************************************
/* Function:
    bool DRV_TMR_ClockSet 
    ( 
        DRV_HANDLE handle, 
        DRV_TMR_CLK_SOURCES clockSource, 
        TMR_PRESCALE  preScale 
    )

  Summary:
    Sets the timers clock by selecting the source and prescaler.

  Description:
    This function sets the timers clock by selecting the source and prescaler.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

bool DRV_TMR_ClockSet 
( 
    DRV_HANDLE handle, 
    DRV_TMR_CLK_SOURCES clockSource, 
    TMR_PRESCALE  prescale 
)
{
    DRV_TMR_CLIENT_OBJ *dObj = _DRV_TMR_ClientObj(handle);


    if(dObj)
    {
        bool success = false;
        bool resume = _DRV_TMR_Suspend(dObj);
        
        if(_DRV_TMR_ClockSourceSet(dObj->pModInst, clockSource))
        {
                success = true;
        }

        _DRV_TMR_Resume(dObj, resume);
        return success;
    }

    return false;
}

// *****************************************************************************
/* Function:
    TMR_PRESCALE DRV_TMR_PrescalerGet ( DRV_HANDLE handle )

  Summary:
    This function gets the currently selected prescaler.

  Description:
    This function gets the currently selected prescaler.

  Remarks:
    Refer to drv_tmr.h for usage information.
*/

TMR_PRESCALE DRV_TMR_PrescalerGet ( DRV_HANDLE handle )
{
    return 0;
} 


