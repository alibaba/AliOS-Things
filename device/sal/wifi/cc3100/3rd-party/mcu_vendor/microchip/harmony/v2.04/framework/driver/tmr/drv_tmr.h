/*******************************************************************************
  Timer Device Driver Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_tmr.h

  Summary:
    Timer device driver interface header file.

  Description:
    This header file contains the function prototypes and definitions of the
    data types and constants that make up the interface to the Timer device
    driver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _DRV_TMR_H
#define _DRV_TMR_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "system_config.h"
#include "driver/driver_common.h"
#if defined (__PIC32C__)
#include "driver/tmr/tmr_definitions_pic32c.h"
#else
#include "peripheral/tmr/plib_tmr.h"
#include "driver/tmr/tmr_definitions_pic32m.h"
#endif
#include "system/int/sys_int.h"            // System Interrupt Definitions
#include "system/clk/sys_clk.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Timer Driver Module Index Numbers

  Summary:
    Timer driver index definitions

  Description:
    These constants provide Timer driver index definitions.

  Remarks:
    These constants should be used in place of hard-coded numeric literals.
    These values should be passed into the DRV_TMR_Initialize and  DRV_TMR_Open
    functions to identify the driver instance in use.
*/

#define DRV_TMR_INDEX_0                             0
#define DRV_TMR_INDEX_1                             1
#define DRV_TMR_INDEX_2                             2
#define DRV_TMR_INDEX_3                             3
#define DRV_TMR_INDEX_4                             4
#define DRV_TMR_INDEX_5                             5
#define DRV_TMR_INDEX_6                             6
#define DRV_TMR_INDEX_7                             7
#define DRV_TMR_INDEX_8                             8
#define DRV_TMR_INDEX_9                             9
#define DRV_TMR_INDEX_10                            10
#define DRV_TMR_INDEX_11                            11

// *****************************************************************************
/* Timer Driver Module Index Count

  Summary:
    Number of valid Timer driver indices.

  Description:
    This constant identifies Timer driver index definitions.

  Remarks:
    This constant should be used in place of hard-coded numeric literals.

    This value is device-specific.
*/

#define DRV_TMR_INDEX_COUNT                             TMR_NUMBER_OF_MODULES

// *****************************************************************************
/* Timer Driver Client Status

  Summary:
    Identifies the client-specific status of the Timer driver

  Description:
    This enumeration identifies the client-specific status of the Timer driver.

  Remarks:
    None.
*/

typedef enum
{
    /* Driver is invalid (or unopened) state */
    DRV_TMR_CLIENT_STATUS_INVALID
        /*DOM-IGNORE-BEGIN*/  = DRV_CLIENT_STATUS_ERROR /*DOM-IGNORE-END*/,

    /* An operation is currently in progress */
    DRV_TMR_CLIENT_STATUS_BUSY
        /*DOM-IGNORE-BEGIN*/  = DRV_CLIENT_STATUS_BUSY      /*DOM-IGNORE-END*/,

    /* Ready, no operations running */
    DRV_TMR_CLIENT_STATUS_READY
        /*DOM-IGNORE-BEGIN*/  = DRV_CLIENT_STATUS_READY + 0 /*DOM-IGNORE-END*/,

    /* Timer started and running, processing transactions */
    DRV_TMR_CLIENT_STATUS_RUNNING
        /*DOM-IGNORE-BEGIN*/  = DRV_CLIENT_STATUS_READY + 1 /*DOM-IGNORE-END*/

} DRV_TMR_CLIENT_STATUS;

// *****************************************************************************
/* Timer Driver Clock sources

  Summary:
    Lists the clock sources available for timer driver.

  Description:
    This enumeration lists all the available clock sources for the timer
    hardware.

  Remarks:
    Not all modes are available on all devices.

    'Synchronization' may not be applicable for all the instances of the timer.
    The driver discards the Synchronization mode selected if it is not
    applicable for the selected hardware.
*/



// *****************************************************************************
/* Timer Driver Operation mode

  Summary:
    Lists the operation modes available for timer driver.

  Description:
    This enumeration lists all the available operation modes that are valid for
    the timer hardware.

  Remarks:
    Not all modes are available on all devices.
*/

typedef enum
{
    /* The timer module operating mode none/invalid */
    DRV_TMR_OPERATION_MODE_NONE,

    /* The timer module operates in 16 bit mode */
    DRV_TMR_OPERATION_MODE_16_BIT,

    /* The timer module operates in 32 bit mode */
    /* This will combine two 16 bit timer modules */
    DRV_TMR_OPERATION_MODE_32_BIT,

} DRV_TMR_OPERATION_MODE;

// *****************************************************************************
/* Timer Driver divider operating specification

  Summary:
    This data structure specifies the divider values that can be obtained by the
    timer module.

  Description:
    This data structure specifies the divider values that can be obtained by the
    timer hardware.

  Remarks:
    None.
*/

typedef struct
{
    /* The minimum divider value that the timer module can obtain */
    uint32_t    dividerMin;

    /* The maximum divider value that the timer module can obtain */
    uint32_t    dividerMax;

    /* The divider step value, between 2 divider values */
    /* Should be 1 for most timers */
    uint32_t    dividerStep;

} DRV_TMR_DIVIDER_RANGE;

// *****************************************************************************
/* Timer Driver Initialize Data

  Summary:
    Defines the Timer driver initialization data.

  Description:
    This data type defines data required to initialize the Timer driver.

  Remarks:
    Not all initialization features are available on all devices.
*/

typedef struct
{
    /* System module initialization. */
    SYS_MODULE_INIT moduleInit;

    /* Identifies timer hardware module (PLIB-level) ID */
    TMR_MODULE_ID tmrId;

    /* Clock Source select. */
    DRV_TMR_CLK_SOURCES clockSource;

    /* Prescaler Selection from the processor enumeration */
    TMR_PRESCALE prescale;

    /* Interrupt Source for TMR module.  If 'DRV_TMR_OPERATION_MODE_32_BIT' flag
       is selected the interrupt will be generated by the 2nd timer of the pair,
       the odd numbered one. */
    INT_SOURCE interruptSource;

    /* Select 16/32 bit operation mode.  32 bit mode will combine two 16 bit
       timer modules to form a 32 bit one.  This is usually only necessary for
       very long delays. */
    DRV_TMR_OPERATION_MODE mode;

    /* Asynchronous write enable configuration.  If true the asynchronous write
       is enabled.  For timers that do not support this feature the value is
       ignored */
    bool asyncWriteEnable;

} DRV_TMR_INIT;

// *****************************************************************************
/* Timer Driver Callback Function Pointer

  Summary:
    Pointer to a Timer driver callback function data type.

  Description:
    This data type defines a pointer to a Timer driver callback function.

  Remarks:
    Useful only when timer alarm callback support is enabled by defining the
    DRV_TMR_ALARM_ENABLE configuration option.
*/

typedef void ( *DRV_TMR_CALLBACK ) ( uintptr_t context, uint32_t alarmCount );

// *****************************************************************************
// *****************************************************************************
// Section: Timer Driver Module Interface Functions
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
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function initializes the Timer driver, making it ready for clients to
    open and use it.

  Precondition:
    None.

  Parameters:
    drvIndex        - Index for the driver instance to be initialized

    init            - Pointer to a data structure containing any data necessary
                      to initialize the driver.

  Returns:
    If successful, returns a valid handle to a driver object.  Otherwise, it
    returns SYS_MODULE_OBJ_INVALID. The returned object must be passed as
    argument to  DRV_TMR_Deinitialize, DRV_TMR_Tasks and DRV_TMR_Status functions.

  Example:
    <code>
    DRV_TMR_INIT    init;
    SYS_MODULE_OBJ  objectHandle;

    // Populate the timer initialization structure
    init.moduleInit.value = SYS_MODULE_POWER_RUN_FULL;
    init.tmrId            = TMR_ID_2;
    init.clockSource      = DRV_TMR_CLKSOURCE_INTERNAL;
    init.prescale         = TMR_PRESCALE_VALUE_256;
    init.interruptSource  = INT_SOURCE_TIMER_2;
    init.mode             = DRV_TMR_OPERATION_MODE_16_BIT;
    init.asyncWriteEnable = false;

    // Do something

    objectHandle = DRV_TMR_Initialize ( DRV_TMR_INDEX_0, (SYS_MODULE_INIT*)&init );

    if ( SYS_MODULE_OBJ_INVALID == objectHandle )
    {
        // Handle error
    }
    </code>

  Remarks:
    This function must be called before any other Timer driver function is
    called.

    This function should only be called once during system initialization unless
    DRV_TMR_Deinitialize is called to deinitialize the driver instance.

    This function will NEVER block for hardware access. The system must use
    DRV_TMR_Status to find out when the driver is in the ready state.

    Build configuration options may be used to statically override options in
    the "init" structure and will take precedence over initialization data
    passed using this function.
*/

SYS_MODULE_OBJ DRV_TMR_Initialize
(
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT * const init
);

// *****************************************************************************
/* Function:
    void DRV_TMR_Deinitialize ( SYS_MODULE_OBJ object )

  Summary:
    Deinitializes the specified instance of the Timer driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    Deinitializes the specified instance of the Timer driver, disabling
    its operation (and any hardware). All internal data is invalidated.

  Precondition:
    The DRV_TMR_Initialize function must have been called before calling this
    function and a valid SYS_MODULE_OBJ must have been returned.

  Parameters:
    object          - Driver object handle, returned from DRV_TMR_Initialize

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      tmrObject;     //  Returned from DRV_TMR_Initialize
    SYS_STATUS          tmrStatus;

    DRV_TMR_Deinitialize ( tmrObject );

    tmrStatus = DRV_TMR_Status ( tmrObject );

    if ( SYS_MODULE_UNINITIALIZED == tmrStatus )
    {
        // Check again later if you need to know
        // when the driver is deinitialized.
    }
    </code>

  Remarks:
    Once the Initialize operation has been called, the Deinitialize operation
    must be called before the Initialize operation can be called again.

    This function will NEVER block waiting for hardware. If the operation
    requires time to allow the hardware to complete, this will be reported by
    the DRV_TMR_Status operation.  The system has to use DRV_TMR_Status to find
    out when the module is in the ready state.
*/

void DRV_TMR_Deinitialize ( SYS_MODULE_OBJ object );

// *****************************************************************************
/* Function:
    SYS_STATUS DRV_TMR_Status ( SYS_MODULE_OBJ object )

  Summary:
    Provides the current status of the Timer driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function provides the current status of the Timer driver.

  Precondition:
    The DRV_TMR_Initialize function must have been called before calling this
    function.

  Parameters:
    object          - Driver object handle, returned from DRV_TMR_Initialize

  Returns:
    SYS_STATUS_READY          - Indicates that the driver is initialized and ready
                                for operation

                                Note: Any value greater than SYS_STATUS_READY is
                                also a normal running state in which the driver
                                is ready to accept new operations.

    SYS_STATUS_ERROR          - Indicates that the driver is in an error state

                                Note: Any value less than SYS_STATUS_ERROR is
                                also an error state.

    SYS_MODULE_UNINITIALIZED  - Indicates that the driver has been deinitialized

                                Note: This value is less than SYS_STATUS_ERROR.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_TMR_Initialize
    SYS_STATUS          tmrStatus;

    tmrStatus = DRV_TMR_Status ( object );

    else if ( SYS_STATUS_ERROR >= tmrStatus )
    {
        // Handle error
    }
    </code>

  Remarks:
    The this operation can be used to determine when any of the driver's module
    level operations has completed.

	Once the status operation returns SYS_STATUS_READY, the driver is ready for
	operation.

    The value of SYS_STATUS_ERROR is negative (-1).  Any value less than that is
    also an error state.

    This function will NEVER block waiting for hardware.
*/

SYS_STATUS DRV_TMR_Status ( SYS_MODULE_OBJ object );

// *****************************************************************************
/* Function:
    void DRV_TMR_Tasks ( SYS_MODULE_OBJ object )

  Summary:
    Maintains the driver's state machine.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function is used to maintain the driver's internal state machine and
    processes the timer events..

  Precondition:
    The DRV_TMR_Initialize function must have been called for the specified Timer
    driver instance.

  Parameters:
    object      - Object handle for the specified driver instance (returned from
                  DRV_TMR_Initialize)

  Returns:
    None

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from DRV_TMR_Initialize

    while (true)
    {
        DRV_TMR_Tasks ( object );

        // Do other tasks
    }
    </code>

  Remarks:
    This function is normally not called directly by an application.  It is
    called by the system's Tasks routine (SYS_Tasks)
*/

void DRV_TMR_Tasks ( SYS_MODULE_OBJ object );

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
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function opens the specified Timer driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver.  Timer driver does not
    support multiple clients. If two tasks want to use the timer, one should
    wait until the other one gets closed.

  Precondition:
    The DRV_TMR_Initialize function must have been called before calling this
    function.

  Parameters:
    index      - Identifier for the object instance to be opened

    intent      - Zero or more of the values from the enumeration
                  DRV_IO_INTENT ORed together to indicate the intended use
                  of the driver

  Returns:
    If successful, the function returns a valid open instance handle (a number
    identifying both the caller and the module instance).

    If an error occurs, the return value is DRV_HANDLE_INVALID.

  Example:
    <code>

    DRV_HANDLE  handle;

    handle = DRV_TMR_Open ( DRV_TMR_INDEX_0, DRV_IO_INTENT_EXCLUSIVE );

    if ( DRV_HANDLE_INVALID == handle )
    {
        // Unable to open the driver
    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_TMR_Close function is called.

    This function will NEVER block waiting for hardware.

    If the requested intent flags are not supported, the function will return
    DRV_HANDLE_INVALID.

    The Timer driver does not support DRV_IO_INTENT_SHARED.
    Only exclusive access is supported for now.
*/

DRV_HANDLE DRV_TMR_Open
(
    const SYS_MODULE_INDEX index,
    const DRV_IO_INTENT intent
);

// *****************************************************************************
/* Function:
    void DRV_TMR_Close ( DRV_HANDLE handle )

  Summary:
    Closes an opened instance of the Timer driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function closes an opened instance of the Timer driver, invalidating
    the handle.

  Precondition:
    The DRV_TMR_Initialize function must have been called for the specified
    Timer driver instance.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None

  Example:
    <code>
    DRV_HANDLE handle;  // Returned from DRV_TMR_Open

    DRV_TMR_Close ( handle );
    </code>

  Remarks:
    After calling this function, the handle passed in "handle" must not be used
    with any of the remaining driver functions.  A new handle must be obtained
    by calling DRV_TMR_Open before the caller may use the driver again.

    Note: Usually there is no need for the driver client to verify that the Close
    operation has completed.
*/

void DRV_TMR_Close ( DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    DRV_TMR_CLIENT_STATUS DRV_TMR_ClientStatus ( DRV_HANDLE handle )

  Summary:
    Gets the status of the client operation.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function gets the status of the recently completed client level
    operation.

  Precondition:
    The DRV_TMR_Initialize function must have been called for the specified
    Timer driver instance.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None

  Example:
    <code>
    DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open
    DRV_TMR_CLIENT_STATUS tmrDrvStatus;

    tmrDrvStatus = DRV_TMR_ClientStatus ( tmrHandle );
    </code>

  Remarks:
    None.
*/

DRV_TMR_CLIENT_STATUS DRV_TMR_ClientStatus ( DRV_HANDLE handle );

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Counting functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void DRV_TMR_CounterValueSet ( DRV_HANDLE handle, uint32_t counterPeriod )

  Summary:
    Updates the Timer's counter register.
	<p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function updates the Timer's value in the counter register.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle          - A valid open-instance handle, returned from the driver's
						open routine

    counterPeriod   - counter period value
                      - a 16 bit value if the timer is configured in 16 bit mode
                      - a 32 bit value if the timer is configured in 32 bit mode

  Returns:
    None.

  Example:
    <code>
    </code>

  Remarks:
    None.
*/

void DRV_TMR_CounterValueSet ( DRV_HANDLE handle, uint32_t counterPeriod );

// *****************************************************************************
/* Function:
    uint32_t DRV_TMR_CounterValueGet ( DRV_HANDLE handle )

  Summary:
    Reads the Timer's counter register.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function returns the Timer's value in the counter register.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       	- A valid open-instance handle, returned from the driver's
                        open routine

  Returns:
    Timer current period:
                      - a 16 bit value if the timer is configured in 16 bit mode
                      - a 32 bit value if the timer is configured in 32 bit mode

  Example:
    <code>
    //Example to use timer for precision time measurement
    //without configuring an alarm (interrupt based)
    char appState = 0;
    DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open

    switch ( appState )
    {
        case 0:
            //Calculate and set the counter period
            DRV_TMR_CounterValueSet ( tmrHandle, ( 0xFFFF - 0x1000 ) );

            //counter starts
            DRV_TMR_Start ( tmrHandle );

            //Trigger an application operation
            app_trigger_operation();

            //Check for time-out in the next state
            appState++;
        case 1:
            //Overflows and stops at 0 if no alarm is set
            if ( DRV_TMR_CounterValueGet ( tmrHandle ) == 0 )
            {
                //Time-out
                return false;
            }
            else if ( app_operation_isComplete( ) )
            {
                //Operation is complete before time-out
                return true;
            }
    </code>

  Remarks:
    None.
*/

uint32_t DRV_TMR_CounterValueGet ( DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    void DRV_TMR_CounterClear ( DRV_HANDLE handle )

  Summary:
    Clears the Timer's counter register.
	<p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function clears the Timer's value in the counter register.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle          - A valid open-instance handle, returned from the driver's
						open routine

  Returns:
    None.

  Example:
    <code>
	</code>

  Remarks:
    None.
*/

void DRV_TMR_CounterClear ( DRV_HANDLE handle );

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
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets up an alarm, allowing the client to receive a callback
    from the driver when the timer counter reaches zero.  Alarms can be one-shot
    or periodic.  A periodic alarm will reload the timer and generate alarm
    until stopped.  The alarm frequency is: DRV_TMR_CounterFrequencyGet() /
    divider;

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

    divider value has to be within the timer divider range (see DRV_TMR_DividerSpecGet).

  Parameters:
    handle      - A valid handle, returned from DRV_TMR_Open

    divider     - The value to divide the timer clock source to obtain the
                  required alarm frequency.
                - a 16 bit value if the timer is configured in 16 bit mode
                - a 32 bit value if the timer is configured in 32 bit mode

	isPeriodic  - Flag indicating whether the alarm should be one-shot or periodic.

	context  	- A reference, call back function will be called with the same
					reference.

	callBack 	- A call back function which will be called on time out.

  Returns:
    - true  - if the call succeeded
    - false - the obtained divider could not be obtained or the passed handle was invalid

  Example:
    <code>
    //Do the initialization with 'mode' set to DRV_TMR_OPERATION_MODE_16_BIT

	void setupTask ()
	{
		DRV_HANDLE              tmrHandle;  // Returned from DRV_TMR_Open

        uint32_t myFreq = 1000; // 1KHz
        uint32_t clkFreq = DRV_TMR_CounterFrequencyGet(tmrHandle);   // timer running frequency

        // calculate the divider needed
        uint32_t divider = clkFreq / myFreq;

		// Start the alarm
		if(!DRV_TMR_AlarmRegister ( tmrHandle, divider, true, 0, CallBackFreq ))
        {
            // divider value could not be obtain;
            // handle the error
            //
        }
    }

    </code>

  Remarks:
    The divider value will be truncated to a 16 bit value if the timer is
    configured in 16 bit mode. The timer should be started using DRV_TMR_Start API to get a
	callback.

*/

bool DRV_TMR_AlarmRegister
(
    DRV_HANDLE handle,
    uint32_t divider,
    bool isPeriodic,
    uintptr_t context,
    DRV_TMR_CALLBACK callBack
);

// *****************************************************************************
/* Function:
    bool DRV_TMR_AlarmDisable ( DRV_HANDLE handle);

  Summary:
    Disables an alarm signal.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function allows the client to disable an alarm generation.
    Use DRV_TMR_AlarmEnable to re-enable.


  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

    A client alarm must be active.

  Parameters:
    handle      - A valid handle, returned from DRV_TMR_Open


  Returns:
    The current status of the alarm:
    - true if the alarm was currently enabled
    - false if the alarm was currently disabled

  Example:
    <code>
    </code>

  Remarks:
    When the driver operates in interrupts this call resolves to a device
    interrupt disable.

    Do NOT disable the timer except for very short periods of time.  If the time
    that the interrupt is disabled is longer than a wrap around period and the
    interrupt is missed, the hardware has no means of recovering and the
    resulting timing will be inaccurate.
*/

bool DRV_TMR_AlarmDisable ( DRV_HANDLE handle);

// *****************************************************************************
/* Function:
    void DRV_TMR_AlarmEnable ( DRV_HANDLE handle, bool enable );

  Summary:
    Re-enables an alarm signal.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function allows the client to re-enable an alarm after it has been
    disabled by a DRV_TMR_AlarmDisable call.

  Precondition:
    The DRV_TMR_Initialize function must have been called.
    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - A valid handle, returned from DRV_TMR_Open

    enable      - boolean to enable the current callback


  Returns:
    None

  Example:
    <code>
    </code>

  Remarks:
    When the driver operates in interrupts this call resolves to a device
    interrupt re-enable.
*/

void DRV_TMR_AlarmEnable ( DRV_HANDLE handle, bool enable );

// *****************************************************************************
/* Function:
    void DRV_TMR_AlarmPeriodSet ( DRV_HANDLE handle, uint32_t value )

  Summary:
    Updates the Timer's period.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function updates the Timer's period.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

    value        - Period value
                      - a 16 bit value if the timer is configured in 16 bit mode
                      - a 32 bit value if the timer is configured in 32 bit mode

  Returns:
    None.

  Example:
    <code>
    DRV_HANDLE handle;  // Returned from DRV_TMR_Open

    DRV_TMR_AlarmPeriodSet ( handle, 0x1000 );
    </code>

  Remarks:
    - The period value will be truncated to a 16 bit value if the timer is
    configured in 16 bit mode.
*/

void DRV_TMR_AlarmPeriodSet ( DRV_HANDLE handle, uint32_t value );

// *****************************************************************************
/* Function:
    uint32_t DRV_TMR_AlarmPeriodGet ( DRV_HANDLE handle )

  Summary:
    Provides the Timer's period.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function gets the Timer's period.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    Timer period value:
                      - a 16 bit value if the timer is configured in 16 bit mode
                      - a 32 bit value if the timer is configured in 32 bit mode

  Example:
    <code>
    DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open
    uint32_t period;

    period = DRV_TMR_AlarmPeriodGet ( tmrHandle );
    </code>

  Remarks:
    None.
*/

uint32_t DRV_TMR_AlarmPeriodGet ( DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    void DRV_TMR_AlarmDeregister ( DRV_HANDLE handle )

  Summary:
    Removes a previously set alarm.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function removes a previously set alarm.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

	DRV_TMR_AlarmRegister function must have been called before.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None.

  Example:
    <code>
    // Example of a key debounce check

    static unsigned int lastReadKey, readKey, keyCount, globalKeyState;
    DRV_HANDLE              tmrHandle;  // Returned from DRV_TMR_Open

    void keyPressDetect ()
    {
        // Calculate the count to be passed on from the clock input
        DRV_TMR_AlarmRegister ( tmrHandle, 0xFF00, true, DebounceCheck );
    }

    void DebounceCheck ( uintptr_t context )
    {
        readKey = AppReadKey();

        if ( readKey != lastReadKey )
        {
            lastReadKey = readKey;
            keyCount = 0;
        }
        else
        {
            if ( keyCount > 20 )
            {
                globalKeyState = readKey;
                DRV_TMR_AlarmDeregister ( tmrHandle );
            }
            keyCount++;
        }
    }
    </code>

  Remarks:
    None.
*/

void DRV_TMR_AlarmDeregister ( DRV_HANDLE handle );

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Operation Control Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    bool DRV_TMR_Start ( DRV_HANDLE handle )

  Summary:
    Starts the Timer counting.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function starts the Timer counting.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

    Timer parameters must have been set by a call to DRV_TMR_AlarmRegister.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    - true      - if the operation succeeded
    - false     - the supplied handle is invalid or the client doesn't have
                  the needed parameters to run (alarm callback and period )

  Example:
    <code>
    DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open

    DRV_TMR_AlarmRegister(tmrHandle, 0x100, true, 0, myTmrCallback);
    DRV_TMR_Start ( tmrHandle );
    </code>

  Remarks:
    None.

*/

bool DRV_TMR_Start ( DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    void DRV_TMR_Stop ( DRV_HANDLE handle )

  Summary:
    Stops the Timer from counting.
    <p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function stops the running Timer from counting.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None.

  Example:
    <code>
    DRV_HANDLE handle;  // Returned from DRV_TMR_Open

    DRV_TMR_Stop ( handle );
    </code>

  Remarks:
    None.
*/

void DRV_TMR_Stop ( DRV_HANDLE handle );

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Operation Status Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    unsigned int DRV_TMR_AlarmHasElapsed ( DRV_HANDLE handle )

  Summary:
    Provides the status of Timer's period elapse.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the number of times Timer's period has elapsed since
    last call to this API has made. On calling this API, the internally
    maintained counter will be cleared and count will be started again from next
    elapse.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - A valid handle, returned from the DRV_TMR_Open

  Returns:
    Number of times timer has elapsed since the last call.

  Example:
    <code>
    DRV_HANDLE      tmrHandle;  // Returned from DRV_TMR_Open
    bool            elapseStatus;
	SYS_MODULE_OBJ  tmrObject // Returned by DRV_TMR_Initialize
	unsigned int 	appInternalTime = 0;

    Sys_Tasks()
	{
		//Timer task will be called from ISR

		APP_TimeUpdate_Task();

		//Other Tasks
	}

	void APP_TimeUpdate_Task ( void )
	{
		//We will not miss a count even though we are late
		appInternalTime += DRV_TMR_AlarmHasElapsed ( tmrHandle );
	}
    </code>

  Remarks:
    None.
*/

uint32_t DRV_TMR_AlarmHasElapsed ( DRV_HANDLE handle );

// *****************************************************************************
// *****************************************************************************
// Section: TMR Driver Control and Information Functions
// *****************************************************************************
// *****************************************************************************

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
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the timer clock by selecting the source and prescaler.
    The clock sources are device specific, refer device datasheet for supported
    clock sources. If unsupported clock source is passed then the behaviour of
    this function is unpredictable.

  Precondition:
    The DRV_TMR_Initialize function must have been called. Must have selected
	32-Bit timer mode if mode selection is applicable.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

    clockSource  - Clock source of the timer

    preScale     - Timer's Prescaler divisor

  Returns:
    - true      - if the operation is successful
    - false     - either the handle is invalid or the clockSource
                  and/or prescaler are not supported

  Example:
    <code>
	DRV_HANDLE              tmrHandle;  // Returned from DRV_TMR_Open

	DRV_TMR_ClockSet ( tmrHandle, DRV_TMR_CLKSOURCE_INTERNAL, TMR_PRESCALE_VALUE_256 );

    </code>

  Remarks:
    None.
*/

bool DRV_TMR_ClockSet
(
    DRV_HANDLE handle,
    DRV_TMR_CLK_SOURCES clockSource,
	TMR_PRESCALE  preScale
);

// *****************************************************************************
/* Function:
    TMR_PRESCALE DRV_TMR_PrescalerGet ( DRV_HANDLE handle )

  Summary:
    This function gets the currently selected prescaler.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function gets the currently selected prescaler.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    Timer prescaler.

  Example:
    <code>
    DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open
    TMR_PRESCALE preScale;

    preScale = DRV_TMR_PrescalerGet ( tmrHandle );
    </code>

  Remarks:
    None.
*/

TMR_PRESCALE DRV_TMR_PrescalerGet ( DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    DRV_TMR_OPERATION_MODE DRV_TMR_OperationModeGet(DRV_HANDLE handle)

  Summary:
    This function gets the currently selected operation mode.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function gets the currently selected 16/32 bit operation mode.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    A DRV_TMR_OPERATION_MODE value showing how the timer is currently
    configured.  DRV_TMR_OPERATION_MODE_NONE is returned for an invalid client
    handle.


  Example:
    <code>
    DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open
	DRV_TMR_OPERATION_MODE operMode;

	operMode = DRV_TMR_OperationModeGet ( tmrHandle );
    </code>

  Remarks:
    None.
*/

DRV_TMR_OPERATION_MODE DRV_TMR_OperationModeGet(DRV_HANDLE handle);

// *****************************************************************************
/* Function:
    bool DRV_TMR_GateModeSet ( DRV_HANDLE handle )

  Summary:
    Enables the Gate mode.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables the Gated mode of Timer. User can measure the duration
    of an external signal in this mode. Once the Gate mode is enabled, Timer
    will start on the raising edge of the external signal. It will keep counting
    until the next falling edge.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    - true      - if the operation is successful
    - false     - either the handle is invalid or the gate mode
                  is not supported

  Example:
    <code>
    DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open

    DRV_TMR_GateModeSet ( tmrHandle );
    </code>

  Remarks:
    None.
*/

bool DRV_TMR_GateModeSet ( DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    bool DRV_TMR_GateModeClear ( DRV_HANDLE handle )

  Summary:
    Enables the Gate mode.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables the Gated mode of Timer. User can measure the duration
    of an external signal in this mode. Once the Gate mode is enabled, Timer
    will start on the raising edge of the external signal. It will keep counting
    until the next falling edge.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    - true      - if the operation is successful
    - false     - either the handle is invalid or the gate mode
                  is not supported

  Example:
    <code>
    DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open

    DRV_TMR_GateModeClear ( tmrHandle );
    </code>

  Remarks:
    None.
*/

bool DRV_TMR_GateModeClear ( DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    uint32_t DRV_TMR_CounterFrequencyGet ( DRV_HANDLE handle )

  Summary:
    Provides the Timer input frequency.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function provides the Timer input frequency. Input frequency is the
    clock to the Timer register and it is considering the prescaler divisor.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - A valid open-instance handle, returned from the driver's
                  open routine

  Returns:
    32-bit value corresponding to the running frequency.
    If Timer clock source is external, then this function returns 0.

  Example:
    <code>
    DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open
    uint32_t   clkFreqHz;

    clkFreqHz = DRV_TMR_CounterFrequencyGet ( tmrHandle );
    </code>

  Remarks:
    On most processors, the Timer's base frequency is the same as the peripheral
    bus clock.
*/

uint32_t DRV_TMR_CounterFrequencyGet ( DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    DRV_TMR_OPERATION_MODE DRV_TMR_DividerRangeGet
    (
        DRV_HANDLE handle,
        DRV_TMR_DIVIDER_RANGE* pDivRange
    )

  Summary:
    Returns the Timer divider values.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function provides the Timer operating mode and divider range.

  Precondition:
    The DRV_TMR_Initialize function must have been called.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - A valid open-instance handle, returned from the driver's
                  open routine

   pDivRange    - Address to store the timer divider range.

  Returns:
    - A DRV_TMR_OPERATION_MODE value showing how the timer is currently configured.
      The pDivRange is updated with the supported range values.

    - DRV_TMR_OPERATION_MODE_NONE for invalid client handle

  Example:
    <code>
    DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open
    DRV_TMR_OPERATION_MODE  timerMode;
    DRV_TMR_DIVIDER_RANGE   timerRange;

    DRV_TMR_DividerRangeGet(handle, &timerRange);
    uint32_t   clkFreqHz = DRV_TMR_CounterFrequencyGet ( tmrHandle );

    uint32_t    maxFreqHz = clkFreqHz / timerRange.dividerMin;
    uint32_t    minFreqHz = clkFreqHz / timerRange.dividerMax;

    </code>

  Remarks:
    None.
*/

DRV_TMR_OPERATION_MODE DRV_TMR_DividerRangeGet
(
    DRV_HANDLE handle,
    DRV_TMR_DIVIDER_RANGE* pDivRange
);

// *****************************************************************************
// *****************************************************************************
// Section: File includes for the APIs which are required for backward
//          compatibility.
// *****************************************************************************
// *****************************************************************************

#include "drv_tmr_compatibility.h"

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif // #ifndef _DRV_TMR_H

/*******************************************************************************
 End of File
*/

