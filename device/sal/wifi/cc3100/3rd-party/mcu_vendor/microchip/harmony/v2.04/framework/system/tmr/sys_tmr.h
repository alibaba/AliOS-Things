/*******************************************************************************
  Timer System Service Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_tmr.h

  Summary:
    Timer System Service interface definition.

  Description:
    This file contains the interface definition for the Timer System
    Service.  It provides a way to interact with the Timer subsystem to
    manage the timing requests supported by the system.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _SYS_TMR_H
#define _SYS_TMR_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "driver/tmr/drv_tmr.h"         // TMR Driver Library Header
#include "system/clk/sys_clk.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: SYS TMR Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* SYS Timer Module Index Numbers

  Summary:
    Timer System Service index definitions.

  Description:
    These constants provide Timer System Service index definitions.

  Remarks:
    These constants should be used in place of hard-coded numeric literals.

*/

#define SYS_TMR_INDEX_0                 0


// *****************************************************************************
/* SYS Timer invalid handle macro definition

  Summary:
    Identifies the invalid handle of the system timer.

  Description:
    This enumeration identifies the invalid handle of the system timer.

  Remarks:
    None.
*/

#define SYS_TMR_HANDLE_INVALID          ( ( uintptr_t ) -1 )


// *****************************************************************************
/* SYS TMR Handle

  Summary
    Identifies a client timer object.

  Description
    This handle identifies a registered instance of a client timer object.
    Access to a timer object parameters and functionality has to use
    a valid client timer handle.

  Remarks:
    None.
*/

typedef uintptr_t SYS_TMR_HANDLE;

// *****************************************************************************
/* SYS TMR Flags

  Summary
    Defines the types and flags supported by a client timer object.

  Description
    This enumerated type describes the types and run time flags supported
    by a client timer object.

  Remarks:
    Multiple flags can be ORed.
*/

typedef enum
{
    SYS_TMR_FLAG_PERIODIC       = 0x0001,       // the timer is of periodic type
                                                // as the time-out is achieved the timer
                                                // is reloaded so it will periodically provide notifications
    SYS_TMR_FLAG_SINGLE         = 0x0000,       // the timer is a single shot type
                                                // it fires just once, when the time-out occurs

    SYS_TMR_FLAG_AUTO_DELETE    = 0x1000,       // as time-out occurs, the timer deletes itself
                                                // only SINGLE and DELAY timer types support this feature

}SYS_TMR_FLAGS;


// *****************************************************************************
/* SYS TMR Callback Function

  Summary
    This data type defines a pointer callback function.

  Description
    Defines a pointer to a client notification function that's called
    when a timer object times out.

  Parameters:
	context     - A client parameter that's passed in the callback function.
                  This will help to identify the callback.
    currTick    - The current system tick when the notification is called.


  Remarks:
    The client notification function should be kept
    as short as possible and non blocking.
    It is meant for some quick operation (like flag setting) and 
    not for extensive operation.
    It can be called within interrupt context.
*/

typedef void ( * SYS_TMR_CALLBACK ) ( uintptr_t context, uint32_t currTick );


// *****************************************************************************
/* SYS TMR Initialize structure

  Summary:
    Identifies the system timer initialize structure.

  Description:
    This structure identifies the system timer initialize structure.

  Remarks:
    None.
*/

typedef struct
{
    /* System module initialization */
    SYS_MODULE_INIT                 moduleInit;

    /* Driver Module index */
    SYS_MODULE_INDEX                drvIndex;

    /* System timer required frequency, Hz */
    /* This is the base frequency for all time keeping */
    uint32_t                        tmrFreq;



} SYS_TMR_INIT;


// *****************************************************************************
// *****************************************************************************
// Section: SYS TMR Module Initialization Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ SYS_TMR_Initialize ( const SYS_MODULE_INDEX           index,
                                       const SYS_MODULE_INIT * const    init )

  Summary:
    Initializes hardware and data for the instance of the Timer module and opens
    the specific module instance.

  Description:
    This function initializes hardware for the instance of the Timer module,
    using the specified hardware initialization data. It also initializes any
    internal data structures.

  Precondition:
    None.

  Parameters:
    index           - Index for the instance to be initialized

    init            - Pointer to a data structure containing any data necessary
                      to initialize the sys timer. This pointer may be null if no
                      data is required because static overrides have been
                      provided.

  Returns:
    If successful, returns a valid handle to an object.  Otherwise, it
    returns SYS_MODULE_OBJ_INVALID. The returned object must be passed as
    argument to SYS_TMR_Reinitialize, SYS_TMR_Deinitialize, SYS_TMR_Tasks and
    SYS_TMR_Status routines.

  Example:
    <code>
    SYS_MODULE_OBJ  objectHandle;
    SYS_TMR_INIT    initConfig;

    // Populate the timer initialization structure
    initConfig.moduleInit.value     = SYS_MODULE_POWER_RUN_FULL;
    initConfig.drvIndex             = DRV_TMR_INDEX_0;
    initConfig.tmrFreq              = 1000;
   
    objectHandle = SYS_TMR_Initialize (SYS_TMR_INDEX_0, (SYS_MODULE_INIT*)&initConfig);
    if (SYS_MODULE_OBJ_INVALID == objectHandle)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine should only be called once during system initialization.
*/

SYS_MODULE_OBJ SYS_TMR_Initialize ( const SYS_MODULE_INDEX           index,
                                   const SYS_MODULE_INIT * const    init );


// *****************************************************************************
/* Function:
    void SYS_TMR_Deinitialize ( SYS_MODULE_OBJ object )

  Summary:
    Deinitializes the specific module instance of the TMR module

  Description:
    This function deinitializes the specific module instance disabling its operation
    (and any hardware for driver modules). Resets all of the internal data
    structures and fields for the specified instance to the default settings.

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    object          - SYS TMR object handle, returned from SYS_TMR_Initialize

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     //  Returned from SYS_TMR_Initialize
    SYS_STATUS          status;

    SYS_TMR_Deinitialize (object);

    status = SYS_TMR_Status (object);
	
    if (SYS_MODULE_UNINITIALIZED == status)
    {
        // Check again later if you need to know
        // when the SYS TMR is deinitialized.
    }
    </code>

  Remarks:
    Once the Initialize operation has been called, the De-initialize
    operation must be called before the Initialize operation can be called
    again.
*/

void SYS_TMR_Deinitialize ( SYS_MODULE_OBJ object );


// *****************************************************************************
/* Function:
    SYS_STATUS SYS_TMR_Status ( SYS_MODULE_OBJ object )

  Summary:
    Returns status of the specific instance of the Timer module.

  Description:
    This function returns the status of the specific module instance.

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    object          - SYS TMR object handle, returned from SYS_TMR_Initialize

  Returns:
    - SYS_STATUS_READY          - Indicates that the driver initialization is
    							  complete and ready to be used.

                                  Note: Any value greater than SYS_STATUS_READY is
                                  also a normal running state in which the driver
                                  is ready to accept new operations
    - SYS_STATUS_ERROR          - Indicates that the driver is in an error state. 
	                              Note: Any value less than SYS_STATUS_ERROR is 
								  also an error state.
    - SYS_MODULE_UNINITIALIZED  - Indicates that the driver has been deinitialized. 
	                              Note:  This value is less than SYS_STATUS_ERROR.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from SYS_TMR_Initialize
    SYS_STATUS          tmrStatus;

    tmrStatus = SYS_TMR_Status (object);
	
    if ( SYS_STATUS_ERROR <= tmrStatus )
    {
        // Handle error
    }
    </code>

  Remarks:
    None.
*/

SYS_STATUS SYS_TMR_Status ( SYS_MODULE_OBJ object );


// *****************************************************************************
/* Function:
    SYS_STATUS SYS_TMR_ModuleStatusGet ( const SYS_MODULE_INDEX index )

  Summary:
    Returns status of the specific instance of the Timer module.

  Description:
    This function returns the status of the specific module instance.
    It uses the index of the instance rather than an object handle.

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    object          - SYS TMR object handle, returned from SYS_TMR_Initialize

  Returns:
    - SYS_STATUS_READY          - Indicates that the service initialization is
    							  complete and the timer is ready to be used.
    - SYS_STATUS_ERROR          - Indicates that the timer service is in an error state
                                  Note: Any value less than SYS_STATUS_ERROR is
                                  also an error state.
    - SYS_MODULE_UNINITIALIZED  - Indicates that the timer service has been deinitialized

  Example:
    <code>

    SYS_STATUS tmrStatus = SYS_TMR_ModuleStatusGet (SYS_TMR_INDEX_0);
	
    if ( SYS_STATUS_ERROR <= tmrStatus )
    {
        // Handle error
    }

    </code>

  Remarks:
    None.
*/

SYS_STATUS SYS_TMR_ModuleStatusGet ( const SYS_MODULE_INDEX index );

// *****************************************************************************
/* Function:
    void SYS_TMR_Tasks ( SYS_MODULE_OBJ object )

  Summary:
    Maintains the system timer's state machine and implements its ISR.

  Description:
    This function is used to maintain the system timer's internal state machine.
	It should be always be called from 'Sys_Tasks' function.

  Precondition:
    The SYS_TMR_Initialize function must have been called for the specified TMR
    driver instance.
	
  Parameters:
    object          - SYS TMR object handle, returned from SYS_TMR_Initialize

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from SYS_TMR_Initialize

    while (true)
    {
        SYS_TMR_Tasks ( object );

        // Do other tasks
    }
    </code>

  Remarks:
    This function is normally not called directly by an application.  It is
    called by the system's Tasks routine (SYS_Tasks) or by the appropriate raw
    ISR.
*/

void SYS_TMR_Tasks ( SYS_MODULE_OBJ object );


// *****************************************************************************
// *****************************************************************************
// Section: SYS TMR Object Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_TMR_HANDLE SYS_TMR_ObjectCreate ( uint32_t periodMs, uintptr_t context, 
	                            SYS_TMR_CALLBACK callback,  SYS_TMR_FLAGS flags)

  Summary:
    Creates a new timer object.

  Description:
    This function creates a new timer object of requested type.
    If a callback is supplied, it will notify the user once the time-out occurs.
    The timer object will be deleted by a call to SYS_TMR_ObjectDelete
   
  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    periodMs    - Periodic delay in milliseconds
	
	context     - A client parameter that's passed in the callback function.
                  This will help to identify the callback.
                  Can be 0 if not needed.
    callback    - Pointer to a callback routine that will be called periodically
                  once the time-out occurs.
                  Can be NULL if not needed.
    flags       - multiple flags specifying what type of object is created:
                  * SYS_TMR_FLAG_PERIODIC    - a periodic timer object is requested
                  * SYS_TMR_FLAG_SINGLE      - a single timer object is requested
                  * SYS_TMR_FLAG_AUTO_DELETE - auto-delete object (SYS_TMR_FLAG_SINGLE only)

  Returns:
    A valid timer object handle of type SYS_TMR_HANDLE if the call succeeds.
    SYS_TMR_HANDLE_INVALID otherwise.
    
  Example:
    <code>
    SYS_TMR_HANDLE handle20ms, handle30ms;
   
    void Test_Callback ( uintptr_t context, uint32_t currTick );

    handle20ms = SYS_TMR_ObjectCreate ( 20, 1, Test_Callback, SYS_TMR_FLAG_PERIODIC );
	handle30ms = SYS_TMR_ObjectCreate ( 30, 2, Test_Callback, SYS_TMR_FLAG_PERIODIC );
	
	void Test_Callback ( uintptr_t context, uint32_t currTick )
	{
		if ( context == 1 )
		{
			//20 ms
		}
		else
		{
			//30 ms
		}
	}

    // when done with the timer objects
    SYS_TMR_ObjectDelete(handle20ms);
    SYS_TMR_ObjectDelete(handle30ms);

    </code>

  Remarks:
    The created timer object needs to be deleted with
    SYS_TMR_ObjectDelete when no longer needed
    (unless is auto-delete type).

    The auto-delete objects are deleted if the time-out has occurred and
    the client has been notified, meaning:
    - The object has a notification/callback function, which has been 
      called upon time-out.
    - SYS_TMR_ObjectCountGet()/SYS_TMR_DelayStatusGet has been called 
*/

SYS_TMR_HANDLE SYS_TMR_ObjectCreate ( uint32_t periodMs, uintptr_t context, SYS_TMR_CALLBACK callback,  SYS_TMR_FLAGS flags);

// *****************************************************************************
/* Function:
    void SYS_TMR_ObjectDelete ( SYS_TMR_HANDLE handle)

  Summary:
    Deletes an existent timer object.

  Description:
    This function deletes a timer object and release the associated resources.
   
  Precondition:
    The SYS_TMR_ObjectCreate function should have been called to get a valid timer 
	object handle.

  Parameters:
    handle      - A valid timer handle, returned by a SYS_TMR_ObjectCreate call.

  Returns:
    None.
 
  Remarks:
    The created timer objects need to be deleted when it is no longer needed 
	(unless of auto-delete type).

*/

void SYS_TMR_ObjectDelete ( SYS_TMR_HANDLE handle);

// *****************************************************************************
/* Function:
    bool SYS_TMR_ObjectReload ( SYS_TMR_HANDLE handle, uint32_t periodMs, 
	                            uintptr_t context, SYS_TMR_CALLBACK callback)

  Summary:
    Reloads an existing timer object.

  Description:
    This function changes the parameters for an already existing timer object.
    The timer object should be valid.

   
  Precondition:
    The SYS_TMR_ObjectCreate function should have been called to get a valid timer object handle.

  Parameters:
    handle      - A valid timer handle, returned by a SYS_TMR_ObjectCreate call.
    periodMs    - Periodic delay in milliseconds

	context     - A client parameter that's passed in the callback function.
                  This will help to identify the callback.
                  Can be '0' if not needed.
    callback    - Pointer to a callback routine that will be called periodically
                  once the time-out occurs. Can be NULL if not needed.

  Returns:
    - true	- If the call succeeds
    - false - If the call fails

  Remarks:
    The function works for any kind of timer object.
    It will fail if the timer object was auto-delete type
    and the time-out has already expired.

*/

bool SYS_TMR_ObjectReload ( SYS_TMR_HANDLE handle, uint32_t periodMs, 
                            uintptr_t context, SYS_TMR_CALLBACK callback);

// *****************************************************************************
/* Function:
    uint32_t SYS_TMR_ObjectCountGet ( SYS_TMR_HANDLE handle, uint32_t* pRateMs)

  Summary:
    Returns the current millisecond count of a timer object.

  Description:
    This function returns the current count and the time-out values of a timer object,
    in milliseconds.
   
  Precondition:
    The SYS_TMR_ObjectCreate function should have been called to get a valid timer object handle.

  Parameters:
    handle  - A valid timer handle, returned by a SYS_TMR_ObjectCreate call.
    pRateMs - pointer to address to store the current object reload rate, in milliseconds.
              Could be NULL, if not needed.


  Returns:
    - > 0	- the current timer object millisecond count
    - = 0	- time-out has occurred
    - < 0	- if invalid timer object

  Remarks:
    This function will destroy the auto-delete timer objects
    if the time-out has occurred.

*/

uint32_t SYS_TMR_ObjectCountGet ( SYS_TMR_HANDLE handle, uint32_t* pRateMs);

// *****************************************************************************
/* Function:
    SYS_TMR_HANDLE SYS_TMR_CallbackPeriodic ( uint32_t periodMs, uintptr_t context, 
	                                          SYS_TMR_CALLBACK   callback )

  Summary:
    Creates a permanent timer object that calls back to the client periodically.

  Description:
    This function creates a timer object that will continuously notify the client
    once the time-out occurs.
    The timer object will be deleted by a call to SYS_TMR_CallbackStop
   
  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    periodMs    - Periodic delay in milliseconds
	context     - A client parameter that's passed in the callback function.
                  This will help to identify the callback.
    callback    - Pointer to a callback routine that will be called periodically
                  once the time-out occurs.


  Returns:
    A valid timer object handle of type SYS_TMR_HANDLE if the call succeeds.
    SYS_TMR_HANDLE_INVALID otherwise.
 
  Example:
    <code>
    SYS_TMR_HANDLE handle20ms, handle30ms;
   
    void Test_Callback ( uintptr_t context, uint32_t currTick );

    handle20ms = SYS_TMR_CallbackPeriodic ( 20, 1, Test_Callback );
	handle30ms = SYS_TMR_CallbackPeriodic ( 20, 2, Test_Callback );
	
	void Test_Callback ( uintptr_t context, uint32_t currTick )
	{
		if ( context == 1 )
		{
			//20 ms
		}
		else
		{
			//30 ms
		}
	}
    </code>

  Remarks:
    The created timer object is persistent and needs to be deleted with
    SYS_TMR_CallbackStop when no longer needed.

    This function is equivalent to
    SYS_TMR_ObjectCreate(periodMs, context, callback, SYS_TMR_FLAG_PERIODIC);

*/

SYS_TMR_HANDLE SYS_TMR_CallbackPeriodic ( uint32_t periodMs, uintptr_t context, 
                                          SYS_TMR_CALLBACK   callback );


// *****************************************************************************
/* Function:
    void SYS_TMR_CallbackStop ( SYS_TMR_HANDLE handle )

  Summary:
    Stops a periodic timer object.

  Description:
    This function deletes a previously created periodic timer object.
    All the associated resources are released.

  Precondition:
    The SYS_TMR_CallbackPeriodic function should have been called to obtain a valid 
	timer handle.

  Parameters:
    handle - A valid periodic timer handle, returned by a SYS_TMR_CallbackPeriodic call.

  Returns:
    None.

  Example:
    <code>
	int count, state;
	void Test_Callback ( uintptr_t context, uint32_t currTick )
	{
		count++;
	}

	void DelayTask ( )
	{
		SYS_TMR_HANDLE handle;
		switch (state)
		{
			case 1:
				handle = SYS_TMR_CallbackPeriodic ( 20, 1, Test_Callback );
				state++;
				break;
			case 2:
				if ( count > 100 )
				{
					SYS_TMR_CallbackStop ( handle );
					state++;
				}
				count++;
				break;
		}
	}
    </code>

  Remarks:
    This function is equivalent to SYS_TMR_ObjectDelete(handle);.
    
*/

void SYS_TMR_CallbackStop ( SYS_TMR_HANDLE handle );


// *****************************************************************************
/* Function:
    SYS_TMR_HANDLE SYS_TMR_CallbackSingle ( uint32_t periodMs, uintptr_t context, 
	                                        SYS_TMR_CALLBACK callback )

  Summary:
    Creates a one-shot/single callback timer object.

  Description:
    This function creates a self-destroying one-shot/single timer object that will
    notify the client once the time-out occurs.
    Once the time-out occurs and the notification is sent the timer object will 
	be invalidated.

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    periodMs    - Periodic delay in milliseconds
	context     - A client parameter that's passed in the callback function.
                  This will help to identify the callback.
    callback    - Pointer to a callback routine which will be called once

  Returns:
    A valid timer object handle of type SYS_TMR_HANDLE if the call succeeds.
    SYS_TMR_HANDLE_INVALID otherwise.

  Example:
    <code>
    SYS_TMR_HANDLE handle;
    void Test_Callback20ms ( uintptr_t context, uint32_t currTick );

    handle = SYS_TMR_CallbackSingle ( 20, 1, Test_Callback20ms );
    </code>

  Remarks:
    The timer object will delete itself and free the associated resources.
    The SYS_TMR_HANDLE will be no longer valid.

    However, SYS_TMR_CallbackStop() can be called to kill the timer object before its 
    time-out expires, if not needed anymore.
    
    This function is equivalent to SYS_TMR_ObjectCreate(periodMs, context, callback, 
	                                 SYS_TMR_FLAG_SINGLE | SYS_TMR_FLAG_AUTO_DELETE);
*/

SYS_TMR_HANDLE SYS_TMR_CallbackSingle ( uint32_t periodMs, uintptr_t context, 
                                        SYS_TMR_CALLBACK callback );


// *****************************************************************************
/* Function:
    SYS_TMR_HANDLE SYS_TMR_DelayMS ( uint32_t delayMs )

  Summary:
    Creates a timer object that times out after the specified delay.

  Description:
    This function creates a self-destroying timer object
    that times out after the required delay.
    The delay status can be checked using SYS_TMR_DelayStatusGet();

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    delayMs   - Indicates the delay required in milliseconds

  Returns:
    A valid timer handler of type SYS_TMR_HANDLE is returned if the call is 
	successful. SYS_TMR_HANDLE_INVALID otherwise.

  Example:
    <code>
    SYS_TMR_HANDLE tmrHandle;

	case APP_ADD_DELAY:
		tmrHandle = SYS_TMR_DelayMS ( 50 );
		state = APP_CHECK_DELAY;
		break;
	case APP_CHECK_DELAY:
		if ( SYS_TMR_DelayStatusGet (tmrHandle ) == true )
		{
			state = APP_DELAY_COMPLETE;
		}
		break;
    </code>

  Remarks:
    Once the required delay is achieved and SYS_TMR_DelayStatusGet()
    returns true the SYS_TMR_HANDLE will be no longer valid
    and the created timer object is deleted.

    However, SYS_TMR_CallbackStop() can be called to kill the timer object before its 
    time-out expires, if not needed anymore.
    
    This function is equivalent to
    SYS_TMR_ObjectCreate(delayMs, 0, 0, SYS_TMR_FLAG_SINGLE | SYS_TMR_FLAG_AUTO_DELETE);
*/

SYS_TMR_HANDLE SYS_TMR_DelayMS ( uint32_t delayMs );


// *****************************************************************************
/* Function:
    bool SYS_TMR_DelayStatusGet ( SYS_TMR_HANDLE handle )

  Summary:
    Checks the status of the previously requested delay timer object.

  Description:
    This function checks the status of the previously requested delay timer object.

  Precondition:
    The SYS_TMR_DelayMS function should have been called before calling this
    function.

  Parameters:
    handle      - A valid timer object handle, returned by SYS_TMR_DelayMS

  Returns:
    - true	- If the delay is elapsed
    - false - If the delay is not elapsed

  Example:
    <code>
    SYS_TMR_HANDLE tmrHandle;

	case APP_ADD_DELAY:
		tmrHandle = SYS_TMR_DelayMS ( 50 );
		state = APP_CHECK_DELAY;
		break;
	case APP_CHECK_DELAY:
		if ( SYS_TMR_DelayStatusGet ( tmrHandle ) == true )
		{
			state = APP_DELAY_COMPLETE;
		}
		break;
    </code>

  Remarks:
    Once the required delay is achieved and SYS_TMR_DelayStatusGet
    returns true the SYS_TMR_HANDLE will be no longer valid
    and the created timer object is deleted.
*/

bool SYS_TMR_DelayStatusGet ( SYS_TMR_HANDLE handle );


// *****************************************************************************
// *****************************************************************************
// Section: SYS TMR Module Count and resolution functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint32_t SYS_TMR_TickCountGet( void )

  Summary:
    Provides the current counter value.

  Description:
    This function provides the current tick counter value.

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    None.

  Returns:
    The current system timer tick count value.

  Example:
    <code>
    uint32_t count;
    count = SYS_TMR_TickCountGet ();
    </code>

  Remarks:
    None.
*/

uint32_t SYS_TMR_TickCountGet( void );

// *****************************************************************************
/* Function:
    uint64_t SYS_TMR_TickCountGetLong( void )

  Summary:
    Provides the current counter value with 64-bit resolution.

  Description:
    This function provides the current tick counter value as a 64-bit value.

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    None.

  Returns:
    The current system timer tick count value.

  Example:
    <code>
    uint64_t lCount;
    lCount = SYS_TMR_TickCountGetLong ();
    </code>

  Remarks:
    None.
*/

uint64_t SYS_TMR_TickCountGetLong( void );

// *****************************************************************************
/* Function:
    uint32_t SYS_TMR_TickCounterFrequencyGet ( void )

  Summary:
    Provides the number of ticks per second

  Description:
    This function provides the system tick resolution,
    the number of ticks per second

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    None.

  Returns:
    The current system timer ticks per second.

  Example:
    <code>
    uint32_t tickFrequencyHz;
 
    tickFrequencyHz = SYS_TMR_TickCounterFrequencyGet ( );
    </code>

  Remarks:
    None.
*/

uint32_t SYS_TMR_TickCounterFrequencyGet ( void );

// *****************************************************************************
/* Function:
    uint64_t SYS_TMR_SystemCountGet( void )

  Summary:
    Provides the current system time count value.

  Description:
    This function provides the current system time count value with the
    greatest possible resolution.
    It is the number of counts from the system start up as counted
    with the system count frequency (SYS_TMR_SystemCountFrequencyGet).

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    None.

  Returns:
    The current system count value.

  Example:
    <code>
    uint64_t sysCount;
    sysCount = SYS_TMR_SystemCountGet ();
    // the system has been running for sysCount/SYS_TMR_SystemCountFrequencyGet seconds
    </code>

  Remarks:
    None.
*/

uint64_t SYS_TMR_SystemCountGet( void );

// *****************************************************************************
/* Function:
    uint32_t SYS_TMR_SystemCountFrequencyGet( void )

  Summary:
    Provides the current system count frequency.

  Description:
    This function provides the current system count frequency in Hz.
    The tick count measurement is based on this value.
    This is the resolution of the system time base.
   
  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    None.

  Returns:
    The current system frequency value in Hertz.

  Example:
    <code>
    uint32_t sysCountFreq;
    sysCountFreq = SYS_TMR_SystemCountFrequencyGet ();
    </code>

  Remarks:
    None.
*/

uint32_t SYS_TMR_SystemCountFrequencyGet( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif //_SYS_TMR_H

/*******************************************************************************
 End of File
*/