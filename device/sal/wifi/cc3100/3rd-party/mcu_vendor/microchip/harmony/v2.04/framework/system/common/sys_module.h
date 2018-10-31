/*******************************************************************************
  System Module Header

  Company:
    Microchip Technology Inc.

  File Name:
    sys_module.h

  Summary:
    Defines definitions and declarations related to system modules.

  Description:
    This file defines definitions and interfaces related to system modules.

  Remarks:
    This file is included via "system.h" and does not normally need to be included
    directly.
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

#ifndef _SYS_MODULE_H
#define _SYS_MODULE_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "assert.h"
#include "system/common/sys_common.h"


#ifdef __cplusplus
    extern "C" {
#endif


// *****************************************************************************
/* System Module Index

  Summary:
    Identifies which instance of a system module should be initialized or opened.

  Description:
    This data type identifies to which instance of a system module a call to
    that module's "Initialize" and "Open" routines refers.

  Remarks:
    Each individual module will usually define macro names for the index values
    it supports (e.g., DRV_TMR_INDEX_1, DRV_TMR_INDEX_2, ...).
*/

typedef unsigned short int SYS_MODULE_INDEX;


// *****************************************************************************
/* System Module Object

  Summary:
    Handle to an instance of a system module.

  Description:
    This data type is a handle to a specific instance of a system module (such
    as a device driver).

  Remarks:
    Code outside of a specific module should consider this as an opaque type
    (much like a void *).  Do not make any assumptions about base type as it
    may change in the future or about the value stored in a variable of this
    type.
*/

typedef uintptr_t SYS_MODULE_OBJ;


// *****************************************************************************
/* System Module Object Invalid

  Summary:
    Object handle value returned if unable to initialize the requested instance
    of a system module.

  Description:
    This is the object handle value returned if unable to initialize the
    requested instance of a system module.

  Remarks:
    Do not rely on the actual value of this constant.  It may change in future
    implementations.
*/

#define SYS_MODULE_OBJ_INVALID      ((SYS_MODULE_OBJ) -1 )


// *****************************************************************************
/* System Module Object Static

  Summary:
    Object handle value returned by static modules.

  Description:
    This is the object handle value returned by static system modules.

  Remarks:
    Do not rely on the actual value of this constant.  It may change in future
    implementations.
*/

#define SYS_MODULE_OBJ_STATIC       ((SYS_MODULE_OBJ) 0 )


// *****************************************************************************
/* System Module Status

  Summary:
    Identifies the current status/state of a system module (including device
    drivers).

  Description:
    This enumeration identifies the current status/state of a system module
    (including device drivers).

  Remarks:
    This enumeration is the return type for the system-level status routine
    defined by each device driver or system module (for example, DRV_I2C_Status).
*/

typedef enum
{
    // Indicates that a non-system defined error has occurred.  The caller
    // must call the extended status routine for the module in question to
    // identify the error.
    SYS_STATUS_ERROR_EXTENDED   = -10,

    /*An unspecified error has occurred.*/
    SYS_STATUS_ERROR            = -1,

    // The module has not yet been initialized
    SYS_STATUS_UNINITIALIZED    = 0,

    // An operation is currently in progress
    SYS_STATUS_BUSY             = 1,

    // Any previous operations have succeeded and the module is ready for
    // additional operations
    SYS_STATUS_READY            = 2,

    // Indicates that the module is in a non-system defined ready/run state.
    // The caller must call the extended status routine for the module in
    // question to identify the state.
    SYS_STATUS_READY_EXTENDED   = 10

} SYS_STATUS;


// *****************************************************************************
/* System Module Power Off State

  Summary:
    Module power-state power off state code.

  Description:
    This value identifies the current power status/state of a system module
    (including device drivers).  It is used to indicate that the module should
    prepare to enter a full power-off state.

  Remarks:
    A power off state indicates that power may be completely removed (0 Volts).

    This value is passed in the powerState field of the SYS_MODULE_INIT
    structure that takes part in all modules initialization and reinitialization.

    The power state codes between SYS_MODULE_POWER_IDLE_RUN (with a value of 3)
    and SYS_MODULE_POWER_RUN_FULL (with a value of 15) are available for 
    module-specific definition and usage.
*/

#define SYS_MODULE_POWER_OFF        0


// *****************************************************************************
/* System Module Power Sleep State

  Summary:
    Module power-state sleep state code.

  Description:
    This value identifies the current power status/state of a system module
    (including device drivers).  It is used to indicate that the module should
    prepare to enter a sleep state.

  Remarks:
    A Sleep state indicates that the core CPU and peripheral clocks may be
    stopped and no code will execute and any module hardware will be stopped.

    This value is passed in the powerState field of the SYS_MODULE_INIT
    structure that takes part in all modules initialization and reinitialization.

    The power state codes between SYS_MODULE_POWER_IDLE_RUN (with a value of 3)
    and SYS_MODULE_POWER_RUN_FULL (with a value of 15) are available for 
    module-specific definition and usage.
*/

#define SYS_MODULE_POWER_SLEEP      1


// *****************************************************************************
/* System Module Power Idle-Stop State

  Summary:
    Module power-state idle-stop state code.

  Description:
    This value identifies the current power status/state of a system module
    (including device drivers).  It is used to indicate that the module should
    prepare to enter an idle-stop state.

  Remarks:
    An idle-stop state indicates that the core CPU clock may be stopped, but the
    module's peripheral clock may continue running.  However, the peripheral
    should prepare to stop operations when the idle state is entered.

    This value is passed in the powerState field of the SYS_MODULE_INIT
    structure that takes part in all modules initialization and reinitialization.

    The power state codes between SYS_MODULE_POWER_IDLE_RUN (with a value of 3)
    and SYS_MODULE_POWER_RUN_FULL (with a value of 15) are available for 
    module-specific definition and usage.
*/

#define SYS_MODULE_POWER_IDLE_STOP  2


// *****************************************************************************
/* System Module Power Idle-Run State

  Summary:
    Module power-state idle-run state code.

  Description:
    This value identifies the current power status/state of a system module
    (including device drivers).  It is used to indicate that the module should
    prepare to enter an idle-run state.

  Remarks:
    An idle-run state indicates that the core CPU clock may be stopped, but the
    module's peripheral clock may continue running and peripheral operations may
    continue as long as no code needs to be executed.  If code needs to execute,
    the module must cause an interrupt.

    This value is passed in the powerState field of the SYS_MODULE_INIT
    structure that takes part in all modules initialization and reinitialization.

    The power state codes between SYS_MODULE_POWER_IDLE_RUN (with a value of 3)
    and SYS_MODULE_POWER_RUN_FULL (with a value of 15) are available for 
    module-specific definition and usage.
*/

#define SYS_MODULE_POWER_IDLE_RUN   3


//**************************************************************************
/*
  Summary:
    Module power-state run-full state code.
    
  Description:
    System Module Power Run-Full State. This value identifies the current power 
	status/state of a system module (including device drivers). It is used to 
	indicate that the module should prepare to enter an run-full state.
    
  Remarks:
    An run-full state indicates that the core CPU and peripheral clocks are
    operational at their normal configured speed and the module should be
    ready for normal operation.
    
    This value is passed in the powerState field of the SYS_MODULE_INIT
    structure that takes part in all modules initialization and
    reinitialization.
    
    The power state codes between SYS_MODULE_POWER_IDLE_RUN (with a value
    of 3) and SYS_MODULE_POWER_RUN_FULL (with a value of 15) are available
    for module-specific definition and usage.                              
*/

#define SYS_MODULE_POWER_RUN_FULL   15


// *****************************************************************************
/* System Module Init

  Summary:
    Initializes a module (including device drivers) in a current power status as
    requested by the system or power manager.

  Description:
    This structure provides the necessary data to initialize or reinitialize
    a module (including device drivers) into a requested power state.
    The structure can be extended in a module specific way as to carry
    module specific initialization data.

  Remarks:
    This structure is used in the device driver routines DRV_<module>_Initialize
    and DRV_<module>_Reinitialize that are defined by each device driver.

    The "powerState" member has several predefined values (shown below).  All
    other available values (within the 4-bit field) are available for 
    module-specific meaning.

    Predefined powerState Values:

    - 0.  SYS_MODULE_POWER_OFF        - Module power-state power off state code
    - 1.  SYS_MODULE_POWER_SLEEP      - Module power-state sleep state code
    - 2.  SYS_MODULE_POWER_IDLE_STOP  - Module power-state idle-stop state code
    - 3.  SYS_MODULE_POWER_IDLE_RUN   - Module power-state idle-run state code
    - 4.  through 14.                 - Module-specific meaning
    - 15. SYS_MODULE_POWER_RUN_FULL   - Module power-state run-full state code
*/

typedef union
{
    uint8_t         value;

    struct
    {
        // Requested power state
        uint8_t     powerState  : 4;

        // Module-definable field, module-specific usage
        uint8_t     reserved    : 4;
    }sys;

} SYS_MODULE_INIT;


// *****************************************************************************
// *****************************************************************************
// Section: System Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* System Initialization Function

  Function:
    void SYS_Initialize( void *data )

  Summary:
    Function that initializes all modules in the system.
    
  Description:
    This function initializes all modules in the system, including any drivers, 
    services, middleware, and applications.

  Precondition:
    None.

  Parameters:
    data            - Pointer to the data structure containing any data
                      necessary to initialize the module. This pointer may
                      be null if no data is required and default initialization
                      is to be used.

  Returns:
    None.

  Example:
    <code>
    SYS_Initialize ( NULL );

    while ( true )
    {
        SYS_Tasks ( );
    }
    </code>

  Remarks:
    This function will only be called once, after system reset.
*/

void SYS_Initialize( void *data );


// *****************************************************************************
/* System Tasks Function

  Function:
    void SYS_Tasks ( void );

  Summary:
    Function that performs all polled system tasks.

  Description:
    This function performs all polled system tasks by calling the state machine 
    "tasks" functions for all polled modules in the system, including drivers, 
    services, middleware and applications.

  Precondition:
    The SYS_Initialize function must have been called and completed.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    SYS_Initialize ( NULL );

    while ( true )
    {
        SYS_Tasks ( );
    }
    </code>

  Remarks:
    If the module is interrupt driven, the system will call this routine from
    an interrupt context.
*/

void SYS_Tasks ( void );


// *****************************************************************************
// *****************************************************************************
// Section:  Pointers to System Module Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* System Module Initialization Routine Pointer

  Function:
    SYS_MODULE_OBJ (* SYS_MODULE_INITIALIZE_ROUTINE) (
                        const SYS_MODULE_INDEX index,
                        const SYS_MODULE_INIT * const init )

  Summary:
    Pointer to a routine that initializes a system module (driver, library, or
    system-maintained application).

  Description:
    This data type is a pointer to a routine that initializes a system module
    (driver, library, or system-maintained application).

  Precondition:
    The low-level board initialization must have (and will be) completed before
    the system will call the initialization routine for any modules.

  Parameters:
    index           - Identifier for the module instance to be initialized

    init            - Pointer to the data structure containing any data
                      necessary to initialize the module. This pointer may
                      be null if no data is required and default initialization
                      is to be used.

  Returns:
    A handle to the instance of the system module that was initialized.  This
    handle is a necessary parameter to all of the other system-module level
    routines for that module.

  Remarks:
    This function will only be called once during system initialization.
*/

typedef SYS_MODULE_OBJ (* SYS_MODULE_INITIALIZE_ROUTINE) ( const SYS_MODULE_INDEX index,
                                                           const SYS_MODULE_INIT * const init );



// *****************************************************************************
/* System Module Reinitialization Routine Pointer

  Function:
    void (* SYS_MODULE_REINITIALIZE_ROUTINE) ( SYS_MODULE_OBJ object,
                const SYS_MODULE_INIT * const init)

  Summary:
    Pointer to a routine that reinitializes a system module (driver, library,
    or system-maintained application)

  Description:
    This data type is a pointer to a routine that reinitializes a system
    module (driver, library, or system-maintained application).

  Precondition:
    The low-level board initialization must have (and will be) completed and the
    module's initialization routine will have been called before the system will
    call the reinitialization routine for any modules.

  Parameters:
    object          - Handle to the module instance

    init            - Pointer to the data structure containing any data
                      necessary to initialize the module. This pointer may
                      be null if no data is required and default initialization
                      is to be used.

  Returns:
    None.

  Example:
    None.

  Remarks:
    This operation uses the same initialization data structure as the Initialize
    operation.

    This operation can be used to change the power state of a module.

    This operation can also be used to refresh the hardware state as defined
    by the initialization data, thus it must guarantee that all hardware
    state has been refreshed.

    This function can be called multiple times to reinitialize the module.
*/

typedef void (* SYS_MODULE_REINITIALIZE_ROUTINE) ( SYS_MODULE_OBJ object,
                                                   const SYS_MODULE_INIT * const init );


//*************************************************************************
/*
  Function:
    void (* SYS_MODULE_DEINITIALIZE_ROUTINE) (  SYS_MODULE_OBJ object )
    
  Summary:
    Pointer to a routine that deinitializes a system module (driver,
    library, or system-maintained application).
	
  Description:
    System Module Deinitialization Routine Pointer. This data type is a 
	pointer to a routine that deinitializes a system module (driver, library, 
	or system-maintained application).
	
  Preconditions:
    The low-level board initialization must have (and will be) completed
    and the module's initialization routine will have been called before
    the system will call the deinitialization routine for any modules.
	
  Parameters:
    object -  Handle to the module instance
	
  Returns:
    None.
	
  Example:
    None.
	
  Remarks:
    If the module instance has to be used again, the module's "initialize"
    function must first be called.                                       
*/

typedef void (* SYS_MODULE_DEINITIALIZE_ROUTINE) (  SYS_MODULE_OBJ object );


// *****************************************************************************
/* System Module Status Routine Pointer

  Function:
    SYS_STATUS (* SYS_MODULE_STATUS_ROUTINE) (  SYS_MODULE_OBJ object )

  Summary:
    Pointer to a routine that gets the current status of a system module
    (driver, library, or system-maintained application).

  Description:
    This data type is a pointer to a routine that gets the current status of a
    system module (driver, library, or system-maintained application).

  Precondition:
    The low-level board initialization must have (and will be) completed and the
    module's initialization routine will have been called before the system will
    call the status routine for any modules.

  Parameters:
    object          - Handle to the module instance

  Returns:
    One of the possible status codes from SYS_STATUS

  Example:
    None.

  Remarks:
    A module's status operation can be used to determine when any of the
    other module level operations has completed as well as to obtain general
    status of the module.  The value returned by the status routine will be
    checked after calling any of the module operations to find out when they
    have completed.

    If the status operation returns SYS_STATUS_BUSY, the previous operation
    has not yet completed. Once the status operation returns SYS_STATUS_READY,
    any previous operations have completed.

    The value of SYS_STATUS_ERROR is negative (-1). A module may define
    module-specific error values of less or equal SYS_STATUS_ERROR_EXTENDED
    (-10).

    The status function must NEVER block.

    If the status operation returns an error value, the error may be cleared by
    calling the reinitialize operation. If that fails, the deinitialize
    operation will need to be called, followed by the initialize operation to
    return to normal operations.
*/

typedef SYS_STATUS (* SYS_MODULE_STATUS_ROUTINE) (  SYS_MODULE_OBJ object );


// *****************************************************************************
/* System Module Tasks Routine Pointer

  Function:
    void (* SYS_MODULE_TASKS_ROUTINE) ( SYS_MODULE_OBJ object )

  Summary:
    Pointer to a routine that performs the tasks necessary to maintain a state
    machine in a module system module (driver, library, or system-maintained
    application).

  Description:
    This data type is a pointer to a routine that performs the tasks necessary
    to maintain a state machine in a module system module (driver, library, or
    system-maintained application).

  Precondition:
    The low-level board initialization must have (and will be) completed and the
    module's initialization routine will have been called before the system will
    call the deinitialization routine for any modules.

  Parameters:
    object          - Handle to the module instance

  Returns:
    None.

  Example:
    None.

  Remarks:
    If the module is interrupt driven, the system will call this routine from
    an interrupt context.
*/

typedef void (* SYS_MODULE_TASKS_ROUTINE) ( SYS_MODULE_OBJ object );


#ifdef __cplusplus
    }
#endif

#endif // _SYS_MODULE_H

/*******************************************************************************
 End of File
*/
