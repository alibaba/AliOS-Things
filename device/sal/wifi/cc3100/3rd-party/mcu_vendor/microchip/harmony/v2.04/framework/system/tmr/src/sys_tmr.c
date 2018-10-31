/*******************************************************************************
  Timer System Service Definition

Company:
Microchip Technology Inc.

File Name:
sys_tmr.c

Summary:
Timer System Service implementation.

Description:
The Timer System Service provides a simple interface to manage the Timer modules
on Microchip micro controllers.  This file implements the core interface routines
for the System Timer Service.
While building the SYS TMR from source, ALWAYS use this file in the build.
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
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
*/

#include "system/tmr/src/sys_tmr_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: Variable Definitions
// *****************************************************************************
// *****************************************************************************



/* System timer instance */
static SYS_TMR_OBJECT           sSysTmrObject = { 0 };

/* Client object array */
static SYS_TMR_CLIENT_OBJECT    sClientObjects [ SYS_TMR_MAX_CLIENT_OBJECTS ];


// *****************************************************************************
/* OSAL user protection 

Summary:
    Enforces the user threads protection

Description:
    Protects against multiple users using concurrently the TMR service.
    For efficiency reasons, there is NO PROTECTION for each API call except
    when creating and deleting new timer clients!
    What it means is that:
    - A client timer cannot be used concurrently from multiple threads!
    It's ok to pass a handle from one thread to another as long as
    there's is no access from more than one thread at a time

Remarks:

None.
*/

static __inline__ bool  __attribute__((always_inline))   _UserGblLockCreate(void)
{
    // create the shared Data Lock
    return OSAL_SEM_Create(&sSysTmrObject.userSem, OSAL_SEM_TYPE_BINARY, 1, 1) == OSAL_RESULT_TRUE;
}    

static __inline__ void  __attribute__((always_inline))          _UserGblLockDelete(void)
{
    OSAL_SEM_Delete(&sSysTmrObject.userSem);
}    

// locks access to shared resources
static __inline__ void  __attribute__((always_inline))          _UserGblLock(void)
{
    // Shared Data Lock
    OSAL_SEM_Pend(&sSysTmrObject.userSem, OSAL_WAIT_FOREVER);
}    

// unlocks access to shared resources
static __inline__ void  __attribute__((always_inline))          _UserGblUnlock(void)
{
    // Shared Data unlock
    OSAL_SEM_Post(&sSysTmrObject.userSem);
}

// ISR protection
// when the timer driver delivers the notification in interrupts
#if (SYS_TMR_INTERRUPT_NOTIFICATION)
// do not re-schedule while locked the timer ISR
// this needs to be real quick
static __inline__ bool __attribute__((always_inline)) _IsrTmrLock(OSAL_CRITSECT_DATA_TYPE* pCritStat)
{
    *pCritStat = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    return DRV_TMR_AlarmDisable(sSysTmrObject.driverHandle);
}

static __inline__ void __attribute__((always_inline)) _IsrTmrUnlock(bool lock, OSAL_CRITSECT_DATA_TYPE critStat)
{
    DRV_TMR_AlarmEnable(sSysTmrObject.driverHandle, lock);
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critStat);
}

#else   // !(SYS_TMR_INTERRUPT_NOTIFICATION)
static __inline__ bool __attribute__((always_inline)) _IsrTmrLock(OSAL_CRITSECT_DATA_TYPE* pCritStat)
{
    return false;
}

static __inline__ void __attribute__((always_inline)) _IsrTmrUnlock(bool lock, OSAL_CRITSECT_DATA_TYPE critStat)
{
}

#endif  // (SYS_TMR_INTERRUPT_NOTIFICATION)


static __inline__ void __attribute__((always_inline)) _SYS_TMR_ClientSrvUnlock(void)
{
    _UserGblUnlock();
}


// access to the object is protected from threads and ISR
// make it quick
static __inline__ void __attribute__((always_inline)) _SYS_TMR_ClientDelete(SYS_TMR_CLIENT_OBJECT* pClient)
{
    pClient->isrEnable = 0;
    pClient->isrState = SYS_TMR_CLIENT_ISR_IDLE;
    pClient->currState = SYS_TMR_CLIENT_FREE;
    // do other clean up here
}

static __inline__ bool __attribute__((always_inline)) _SYS_TMR_ObjectCheck(SYS_MODULE_OBJ object)
{
    // basic sanity check we're the right object
    return (SYS_TMR_OBJECT*)object == &sSysTmrObject;
}

static __inline__ bool __attribute__((always_inline)) _SYS_TMR_ReadyCheck(void)
{
    // basic sanity check the SYS_TMR is in the up and running state
    return sSysTmrObject.status == SYS_STATUS_READY;
}

// check the validity of a client handle and locks the user threads
// returns a valid pointer if success and _SYS_TMR_ClientSrvUnlock() needs to be called
// returns 0 if invalid, do NOT call _SYS_TMR_ClientSrvUnlock !
//
static SYS_TMR_CLIENT_OBJECT* _SYS_TMR_ClientCheckSrvLock(SYS_TMR_HANDLE handle)
{
    if(!_SYS_TMR_ReadyCheck())
    {
        return 0;
    }

    // lock client access
    _UserGblLock();
    if(handle != 0 && handle != SYS_TMR_HANDLE_INVALID)
    {   // extra sanity check could be added here
        if( ((SYS_TMR_CLIENT_OBJECT*)handle)->currState > 0)
        {
            return (SYS_TMR_CLIENT_OBJECT*)handle;
        }
    }

    // not a valid client
    _UserGblUnlock();
    return 0;
}

// *****************************************************************************
// *****************************************************************************
// Section: SYS TMR Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
   SYS_MODULE_OBJ SYS_TMR_Initialize ( const SYS_MODULE_INDEX index,
   const SYS_MODULE_INIT * const init )

Summary:
Initializes hardware and data for the instance of the Timer module and opens
the specific module instance.

Description:
This function initializes hardware for the instance of the Timer module,
using the specified hardware initialization data. It also initializes any
internal data structures.

Parameters:
index           - Index for the instance to be initialized

init            - Pointer to a data structure containing any data necessary
to initialize the driver. This pointer may be null if no
data is required because static overrides have been
provided.

Returns:
None
*/

SYS_MODULE_OBJ SYS_TMR_Initialize ( const SYS_MODULE_INDEX index,
        const SYS_MODULE_INIT * const init )
{

    while(true)
    {
        memset(&sSysTmrObject, 0, sizeof(sSysTmrObject));

        if(init == 0 || index != SYS_TMR_INDEX_0)
        {
            break;
        }

        const SYS_TMR_INIT * sysTmrInit;
        sysTmrInit = ( const SYS_TMR_INIT * ) init;
        // clear the client structures
        memset(sClientObjects, 0, sizeof(sClientObjects));

        /* Set the status for the state machine to advance */
        sSysTmrObject.drvIndex = sysTmrInit->drvIndex;
        sSysTmrObject.sysTickFreq = sysTmrInit->tmrFreq;

        if(!_UserGblLockCreate())
        {   // failed to create OSAL protection
            break;
        }

        // success; proceed with the initialization
        sSysTmrObject.status = SYS_STATUS_BUSY;
        /* Return timer object */
        return (SYS_MODULE_OBJ)&sSysTmrObject;
    }

    // initialization failed
    sSysTmrObject.status  = SYS_STATUS_ERROR;
    return SYS_MODULE_OBJ_INVALID;
}


// *****************************************************************************
/* Function:
   void SYS_TMR_Deinitialize ( SYS_MODULE_OBJ object )

Summary:
De-initializes the specific module instance of the Timer module.

Description:
De-initializes the specific module instance disabling its operation (and
any hardware for driver modules). Resets all the internal data
structures and fields for the specified instance to the default
settings.

PreCondition:
The SYS_TMR_Initialize function should have been called before calling
this function.

Parameters:
object          - SYS TMR object handle, returned from SYS_TMR_Initialize

Returns:
None.
*/

void SYS_TMR_Deinitialize ( SYS_MODULE_OBJ object )
{
    int ix;

    if(!_SYS_TMR_ObjectCheck(object))
    {
        return;
    }

    // we're up and running
    // make sure no new clients are accepted
    _UserGblLock();

    // now kill all clients
    if ( sSysTmrObject.driverHandle != 0 && sSysTmrObject.driverHandle != DRV_HANDLE_INVALID )
    { 
        DRV_TMR_Close(sSysTmrObject.driverHandle);
        sSysTmrObject.driverHandle = 0;
    }

    // release all clients
    SYS_TMR_CLIENT_OBJECT* pClient = sClientObjects + 0;
    for ( ix = 0; ix < sizeof(sClientObjects)/sizeof(*sClientObjects); ix++, pClient++ )
    {
        pClient->isrEnable = 0;
        if ( pClient->currState != SYS_TMR_CLIENT_FREE)
        {
            _SYS_TMR_ClientDelete(pClient);
        }
    }

    _UserGblLockDelete();

    sSysTmrObject.status = SYS_STATUS_UNINITIALIZED;

} 


// *****************************************************************************
/* Function:
   SYS_STATUS SYS_TMR_Status ( SYS_MODULE_OBJ object )

Summary:
Returns status of the specific module instance of the Timer module.

Description:
This function returns the status of the specific module instance disabling its
operation (and any hardware for driver modules).

PreCondition:
The SYS_TMR_Initialize function should have been called before calling
this function.

Parameters:
object          - SYS TMR object handle, returned from SYS_TMR_Initialize

Returns:
SYS_STATUS_READY    Indicates that any previous module operation for the
specified module has completed

SYS_STATUS_BUSY     Indicates that a previous module operation for the
specified module has not yet completed

SYS_STATUS_ERROR    Indicates that the specified module is in an error state
*/

SYS_STATUS SYS_TMR_Status ( SYS_MODULE_OBJ object )
{
    if(!_SYS_TMR_ObjectCheck(object))
    {
        return SYS_STATUS_UNINITIALIZED;
    }

    return ( sSysTmrObject.status );
}

SYS_STATUS SYS_TMR_ModuleStatusGet ( const SYS_MODULE_INDEX index )
{
    return (index == SYS_TMR_INDEX_0) ? sSysTmrObject.status : SYS_STATUS_UNINITIALIZED;
}



// *****************************************************************************
/* Function:
   void SYS_TMR_Tasks ( SYS_MODULE_OBJ object )

Summary:
Maintains the system Timer's state machine and implements its ISR.

Description:
This routine is used to maintain the system Timer's internal state machine and
implement its ISR for interrupt-driven implementations.

Precondition:
The SYS_TMR_Initialize function must have been called for the specified Timer
driver instance.

Parameters:
object          - SYS TMR object handle, returned from SYS_TMR_Initialize

Returns:
None.
*/

void SYS_TMR_Tasks ( SYS_MODULE_OBJ object )
{
    if(!_SYS_TMR_ObjectCheck(object))
    {
        return;
    }

    switch ( sSysTmrObject.status )
    {
        case SYS_STATUS_BUSY:
            // Performing the initialization
            // Try to open a TMR driver
            sSysTmrObject.driverHandle =  DRV_TMR_Open ( sSysTmrObject.drvIndex, DRV_IO_INTENT_EXCLUSIVE );

            if ( sSysTmrObject.driverHandle == DRV_HANDLE_INVALID )
            {   // spin here until the driver is opened
                break;
            }

            // try to set up the service parameters
            if(!_SYS_TMR_Setup(&sSysTmrObject))
            {   // some parameter error occurred
                DRV_TMR_Close(sSysTmrObject.driverHandle);
                sSysTmrObject.driverHandle = 0;
                sSysTmrObject.status  = SYS_STATUS_ERROR;
                break;
            }

            // success
            /* Set the alarm */
            DRV_TMR_AlarmRegister ( sSysTmrObject.driverHandle, sSysTmrObject.driverPeriod, true,
                                    (uintptr_t)&sSysTmrObject, _SYS_TMR_AlarmCallback );
            /*Enable the alarm*/
            DRV_TMR_AlarmEnable(sSysTmrObject.driverHandle, true);
            /* Start the timer */
            DRV_TMR_Start ( sSysTmrObject.driverHandle );

            /* Status of the state machine is changed to indicate initialization complete */
            sSysTmrObject.status = SYS_STATUS_READY;
            break;

        case SYS_STATUS_READY:

            /* If there is no activity we need not run the loop */
#if (SYS_TMR_INTERRUPT_NOTIFICATION)
            // block user access; we may delete clients
            _UserGblLock();
            _SYS_TMR_ProcessIsrClients();
            _UserGblUnlock();
#else
            if ( sSysTmrObject.alarmReceived == true )
            {
                sSysTmrObject.alarmReceived = false;
                // block user access; we may delete clients
                _UserGblLock();
                _SYS_TMR_ProcessTmrAlarm();
                _UserGblUnlock();
            }
#endif  // (SYS_TMR_INTERRUPT_NOTIFICATION)
            break;

        default:    // SYS_STATUS_ERROR, SYS_STATUS_UNINITIALIZED, etc.
            // do nothing
            break;
    }

}

static bool _SYS_TMR_Setup(SYS_TMR_OBJECT* tmrObject)
{
    uint32_t    drvFreq, drvPeriod, errorFreq;
    uint32_t    tickFreq, tickUnitCount;

    // sanity check, protect against user error
    if(sSysTmrObject.sysTickFreq == 0 || SYS_TMR_UNIT_RESOLUTION < 1000 ||  SYS_TMR_UNIT_RESOLUTION / sSysTmrObject.sysTickFreq < 1 )
    {
        return false;
    }

    drvFreq = DRV_TMR_CounterFrequencyGet ( tmrObject->driverHandle );
    // sanity check that we can obtain the requested frequency
    // with the underlying 16 bit timer (period = [2, 0xffff]);
    drvPeriod = drvFreq / tmrObject->sysTickFreq;

    if(drvPeriod < 2 || drvPeriod > 0xffff)
    {   // required freq not within obtainable range
        return false;
    }
    // calculate the actual tick frequency
    tickFreq = drvFreq / drvPeriod;
    // check we're within requested limits
    if(tickFreq >= tmrObject->sysTickFreq)
    {
        errorFreq = tickFreq - tmrObject->sysTickFreq;
    }
    else
    {
        errorFreq = tmrObject->sysTickFreq - tickFreq;
    }

    if((errorFreq * 100) / tmrObject->sysTickFreq > SYS_TMR_FREQUENCY_TOLERANCE)
    {   // too great an error
        return false;
    }

    // calculate the timer units
    tickUnitCount = SYS_TMR_UNIT_RESOLUTION / tickFreq;
    if(tickUnitCount < 1)
    {    // not enough resolution
        return false;
    }

    // success
    tmrObject->sysTickFreq = tickFreq;
    tmrObject->sysTickUnitCount = tickUnitCount;
    tmrObject->driverFreq = drvFreq;
    tmrObject->driverPeriod = drvPeriod;

 
    return true;
}


// *****************************************************************************
/* Function:
   SYS_TMR_HANDLE _SYS_TMR_ClientCreate ( unsigned int periodMs,  
   uintptr_t context, SYS_TMR_CALLBACK callback, SYS_TMR_FLAGS flags )

Summary:
    Creates a new timer client, if possible.

Description:
    It tries to add a new timer client using the requested period in milliseconds.

Parameters:
    periodMs    - alarm time
    context     - client supplied parameter to be passed in the callback
                  not used by the TMR service itself
    callback    - client function to be called when the alarm expired                
    flags       - SYS_TMR_FLAGS object flags 

Returns:
    A valid SYS_TMR_HANDLE if success
    SYS_TMR_HANDLE_INVALID if failed
*/

static SYS_TMR_HANDLE _SYS_TMR_ClientCreate ( uint32_t periodMs, uintptr_t context, SYS_TMR_CALLBACK callback, SYS_TMR_FLAGS flags )
{
    SYS_TMR_CLIENT_OBJECT *timerObj;
    SYS_TMR_CLIENT_OBJECT *newObj;
    int index;

    if(!_SYS_TMR_ReadyCheck())
    {
        return SYS_TMR_HANDLE_INVALID;
    }

    // lock user access
    _UserGblLock();

    newObj = (SYS_TMR_CLIENT_OBJECT*)SYS_TMR_HANDLE_INVALID;
    timerObj = sClientObjects + 0;
    for ( index = 0; index < sizeof(sClientObjects)/sizeof(*sClientObjects); index++, timerObj++ )
    {
        if ( timerObj->currState == SYS_TMR_CLIENT_FREE )
        {   // found free object			
            memset(timerObj, 0x0, sizeof(*timerObj));
            if(_SYS_TMR_ClientParamSet(timerObj, periodMs, context, callback))
            {
                if((flags & SYS_TMR_FLAG_PERIODIC) != 0)
                {
                    timerObj->flags.periodic = true;
                }
                else if((flags & SYS_TMR_FLAG_AUTO_DELETE) != 0)
                {
                    timerObj->flags.auto_del = true;
                }

                // enable the object, seen by the ISR thread
                timerObj->currState = SYS_TMR_CLIENT_ACTIVE;
                timerObj->isrState = SYS_TMR_CLIENT_ISR_ACTIVE;
                timerObj->isrEnable = 1;
                newObj = timerObj;
            }
            // else wrong parameters
            break;
        }
    }

    _UserGblUnlock();

    return (SYS_TMR_HANDLE)newObj;
            
} 

// Assigns the timing parameters
static bool _SYS_TMR_ClientParamSet(SYS_TMR_CLIENT_OBJECT *timerObj, uint32_t periodMs, uintptr_t context, SYS_TMR_CALLBACK callback)
{
    uint32_t upError, lowError, error;
    uint32_t unitCount, upUnits, lowUnits;

    // sanity check
    if(periodMs == 0 || periodMs >= (0xffffffff / SYS_TMR_UNIT_RESOLUTION))
    {
        return false;
    }

    unitCount =  (periodMs * (sSysTmrObject.sysTickFreq * sSysTmrObject.sysTickUnitCount)) / 1000;
    if(unitCount < sSysTmrObject.sysTickUnitCount)
    {   // cannot go lower than that
        unitCount = sSysTmrObject.sysTickUnitCount;
    }

    // calculate errors: truncation and round up
    lowUnits = (unitCount / sSysTmrObject.sysTickUnitCount) * sSysTmrObject.sysTickUnitCount;
    lowError = (unitCount >= lowUnits) ? unitCount - lowUnits : lowUnits - unitCount;

    upUnits = ((unitCount + sSysTmrObject.sysTickUnitCount - 1) / sSysTmrObject.sysTickUnitCount) * sSysTmrObject.sysTickUnitCount;
    upError = (unitCount >= upUnits) ? unitCount - upUnits : upUnits - unitCount;

    error = (upError > lowError) ? lowError : upError;

    if((error * 100 / unitCount) > SYS_TMR_CLIENT_TOLERANCE )
    {   // too great an error
        return false;
    }

    timerObj->tuRate = timerObj->tuCount =  unitCount;
    timerObj->context = context;
    timerObj->callback = callback;

    return true;
} 




// delete clients that are marked for delition
#if (SYS_TMR_INTERRUPT_NOTIFICATION)

// process timer alarm, ISR context
static void _SYS_TMR_ProcessIsrAlarm(void)
{
    int ix;
    SYS_TMR_CLIENT_OBJECT* pClient;

    // get number of elapsed counts
    uint32_t    nTicks = sSysTmrObject.sysTickCount - sSysTmrObject.sysTickPrevCount; 
    sSysTmrObject.sysTickPrevCount =  sSysTmrObject.sysTickCount;
    uint32_t nUnitsElapsed = (nTicks * sSysTmrObject.sysTickUnitCount);


    pClient = sClientObjects + 0;
    for ( ix = 0; ix < sizeof(sClientObjects)/sizeof(*sClientObjects); ix++, pClient++ )
    {
        if ( pClient->isrEnable && pClient->isrState == SYS_TMR_CLIENT_ISR_ACTIVE)
        {
            if( (pClient->tuCount -= nUnitsElapsed) <= 0 )
            {   // timeout
                /* invoke callback routine */
                if ( pClient->callback != 0 )
                {
                    pClient->callback ( pClient->context, sSysTmrObject.sysTickCount);
                }
                if ( pClient->flags.periodic == true )
                {   // reload
                    pClient->tuCount += pClient->tuRate;
                }
                else if (pClient->callback != 0 && pClient->flags.auto_del == true)
                {   // client notified; delete this object
                    // mark it to be deleted later on
                    pClient->isrState = SYS_TMR_CLIENT_ISR_MARK_DEL;
                }
                else
                {   // non delete non periodic object; store the timeout condition
                    pClient->isrState = SYS_TMR_CLIENT_ISR_MARK_INACTIVE;
                    pClient->tuCount = 0;
                }
            }
        }
    }

}


static void _SYS_TMR_ProcessIsrClients(void)
{
    int ix;
    SYS_TMR_CLIENT_OBJECT* pClient;


    bool isrLock;
    OSAL_CRITSECT_DATA_TYPE critSect;

    isrLock = _IsrTmrLock(&critSect);

    pClient = sClientObjects + 0;
    for ( ix = 0; ix < sizeof(sClientObjects)/sizeof(*sClientObjects); ix++, pClient++ )
    {
        if ( pClient->isrState == SYS_TMR_CLIENT_ISR_MARK_DEL)
        {
            _SYS_TMR_ClientDelete(pClient);
        }
        else if(pClient->isrState == SYS_TMR_CLIENT_ISR_MARK_INACTIVE)
        {
            pClient->isrEnable = 0;
            pClient->currState = SYS_TMR_CLIENT_INACTIVE;
        }
    }

    _IsrTmrUnlock(isrLock, critSect);
}

#else // !(SYS_TMR_INTERRUPT_NOTIFICATION)

// process timer alarm
// occurs within TMR thread
static void _SYS_TMR_ProcessTmrAlarm(void)
{
    int ix;
    SYS_TMR_CLIENT_OBJECT* pClient;

    // get number of elapsed counts
    uint32_t    nTicks = sSysTmrObject.sysTickCount - sSysTmrObject.sysTickPrevCount; 
    sSysTmrObject.sysTickPrevCount =  sSysTmrObject.sysTickCount;
    uint32_t nUnitsElapsed = (nTicks * sSysTmrObject.sysTickUnitCount);
    

    pClient = sClientObjects + 0;
    for ( ix = 0; ix < sizeof(sClientObjects)/sizeof(*sClientObjects); ix++, pClient++ )
    {
        if ( pClient->currState == SYS_TMR_CLIENT_ACTIVE)
        {
            if( (pClient->tuCount -= nUnitsElapsed) <= 0 )
            {   // timeout
                /* invoke callback routine */
                if ( pClient->callback != 0 )
                {
                    pClient->callback ( pClient->context, sSysTmrObject.sysTickCount);
                }
                if ( pClient->flags.periodic == true )
                {   // reload
                    pClient->tuCount += pClient->tuRate;
                }
                else if (pClient->callback != 0 && pClient->flags.auto_del == true)
                {   // client notified; delete this object
                    _SYS_TMR_ClientDelete(pClient);
                }
                else
                {   // non delete non periodic object; store the timeout condition
                    pClient->currState = SYS_TMR_CLIENT_INACTIVE;
                    pClient->tuCount = 0;
                }
            }
        }
    }

}

#endif  // (SYS_TMR_INTERRUPT_NOTIFICATION)

// *****************************************************************************
// *****************************************************************************
// Section: SYS TMR Client Setup Routines
// *****************************************************************************
// *****************************************************************************

SYS_TMR_HANDLE SYS_TMR_ObjectCreate ( uint32_t periodMs, uintptr_t context, SYS_TMR_CALLBACK callback,  SYS_TMR_FLAGS flags)
{
    return _SYS_TMR_ClientCreate ( periodMs, context, callback, flags );
}

void SYS_TMR_ObjectDelete ( SYS_TMR_HANDLE handle)
{

    SYS_TMR_CLIENT_OBJECT * timerObj = _SYS_TMR_ClientCheckSrvLock(handle);

    if(timerObj)
    {   // valid client; lock other users access   
        _SYS_TMR_ClientDelete(timerObj);
        _SYS_TMR_ClientSrvUnlock();
    }

}

void SYS_TMR_CallbackStop ( SYS_TMR_HANDLE handle )
{
    SYS_TMR_ObjectDelete(handle);
} 


bool SYS_TMR_ObjectReload ( SYS_TMR_HANDLE handle, uint32_t periodMs, uintptr_t context, SYS_TMR_CALLBACK callback)
{
    SYS_TMR_CLIENT_OBJECT * timerObj = _SYS_TMR_ClientCheckSrvLock(handle);

    if(timerObj)
    {   
        timerObj->isrEnable = 0;
        bool res = _SYS_TMR_ClientParamSet(timerObj, periodMs, context, callback);

        if(timerObj->isrState == SYS_TMR_CLIENT_ISR_MARK_DEL)
        {   // client is too late
            _SYS_TMR_ClientDelete(timerObj);
            res = false;
        }

        timerObj->isrState = SYS_TMR_CLIENT_ISR_ACTIVE;
        timerObj->isrEnable = 1;
        _SYS_TMR_ClientSrvUnlock();
        return res;
    }

    return false;
}

uint32_t SYS_TMR_ObjectCountGet ( SYS_TMR_HANDLE handle, uint32_t* pRateMs)
{
    uint32_t obCount;


    SYS_TMR_CLIENT_OBJECT * timerObj = _SYS_TMR_ClientCheckSrvLock(handle);

    if(timerObj)
    {   
        if(pRateMs)
        {   // transform to ms
            *pRateMs = (timerObj->tuRate * 1000) / SYS_TMR_UNIT_RESOLUTION;
        }

        obCount = timerObj->tuCount;
        if(obCount == 0 && timerObj->flags.auto_del != 0)
        {   // timed out, delete it
            _SYS_TMR_ClientDelete(timerObj);
        }
        _SYS_TMR_ClientSrvUnlock();
        // round up so that don't return 0 if there are still counts
        return (obCount * 1000 + SYS_TMR_UNIT_RESOLUTION - 1) / SYS_TMR_UNIT_RESOLUTION; 
    }

    return -1;

}



SYS_TMR_HANDLE SYS_TMR_CallbackPeriodic ( uint32_t periodMs, uintptr_t context, SYS_TMR_CALLBACK callback )
{
    return _SYS_TMR_ClientCreate ( periodMs, context, callback, SYS_TMR_FLAG_PERIODIC );
}


SYS_TMR_HANDLE SYS_TMR_CallbackSingle ( uint32_t periodMs, uintptr_t context, SYS_TMR_CALLBACK callback )
{
    return _SYS_TMR_ClientCreate ( periodMs, context, callback, SYS_TMR_FLAG_SINGLE | SYS_TMR_FLAG_AUTO_DELETE );
}


SYS_TMR_HANDLE SYS_TMR_DelayMS ( uint32_t delayMs )
{
    return _SYS_TMR_ClientCreate ( delayMs, 0, 0, SYS_TMR_FLAG_SINGLE | SYS_TMR_FLAG_AUTO_DELETE );
}

bool SYS_TMR_DelayStatusGet ( SYS_TMR_HANDLE handle )
{
    return SYS_TMR_ObjectCountGet(handle, 0) == 0;
}




// *****************************************************************************
/* Function:
   static void _SYS_TMR_AlarmCallback ( uintptr_t context, uint32_t alarmCount )

Summary:
    System timer alarm call back function.

Description:
    This function is called by the underlying timer driver
    when the registered alarm time has elapsed
    (i.e. another system tick has occurred).

Precondition:
    None

Parameters:
    context    - callback parameter
    alarmCount - current alarm count
Returns:
    None.

*/

static void _SYS_TMR_AlarmCallback ( uintptr_t context, uint32_t alarmCount )
{

    SYS_TMR_OBJECT* pTmrObj = (SYS_TMR_OBJECT*)context;

    if(alarmCount < pTmrObj->sysTickCount)
    {   // overflow
        pTmrObj->sysTickCountHigh++;
    }
    // set new count
    pTmrObj->sysTickCount = alarmCount;

#if (SYS_TMR_INTERRUPT_NOTIFICATION)
    _SYS_TMR_ProcessIsrAlarm();
#else
    pTmrObj->alarmReceived = true;
#endif

} 


// for the following functions no lock is performed
// because they need to be really fast and
// they are info only anyway
uint32_t SYS_TMR_TickCountGet ( void )
{
    return _SYS_TMR_ReadyCheck() ? sSysTmrObject.sysTickCount : 0;
}

uint64_t SYS_TMR_TickCountGetLong ( void )
{
    if(!_SYS_TMR_ReadyCheck())
    {
        return 0;
    }
    
    uint64_t  tick1, tick2;

    do
    {
        tick1 = (uint64_t)sSysTmrObject.sysTickCountHigh << 32 | sSysTmrObject.sysTickCount;
        tick2 = (uint64_t)sSysTmrObject.sysTickCountHigh << 32 | sSysTmrObject.sysTickCount;
    }
    while(tick1 != tick2);

    return tick2;
}

uint32_t SYS_TMR_TickCounterFrequencyGet ( void )
{
    return _SYS_TMR_ReadyCheck() ? sSysTmrObject.sysTickFreq : 0;
}

uint32_t SYS_TMR_SystemCountFrequencyGet( void )
{
    return _SYS_TMR_ReadyCheck() ? sSysTmrObject.driverFreq : 0;
}


// for this one we need to access the driver
// we need to make sure the object is valid
// use a 64 bit values to avoid overflow
uint64_t SYS_TMR_SystemCountGet( void )
{

    if(!_SYS_TMR_ReadyCheck())
    {
        return 0;
    }

    uint16_t drvCount1, drvCount2;
    union
    {
        uint64_t ull;
        struct
        {
            uint32_t low;
            uint32_t high;
        };
    }sll1, sll2;

    do
    {
        drvCount1 = DRV_TMR_CounterValueGet(sSysTmrObject.driverHandle);
        sll1.low = sSysTmrObject.sysTickCount;
        sll1.high = sSysTmrObject.sysTickCountHigh;
        drvCount2 = DRV_TMR_CounterValueGet(sSysTmrObject.driverHandle);
        sll2.low = sSysTmrObject.sysTickCount;
        sll2.high = sSysTmrObject.sysTickCountHigh;
    }while(drvCount2 < drvCount1 || sll1.ull != sll2.ull);


    return (sll1.ull * sSysTmrObject.driverFreq ) / sSysTmrObject.sysTickFreq + drvCount2;
}

