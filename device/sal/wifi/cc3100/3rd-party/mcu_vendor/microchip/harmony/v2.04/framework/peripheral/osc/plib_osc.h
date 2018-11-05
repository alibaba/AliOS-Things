/*******************************************************************************
  Oscillator Peripheral Library Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    plib_osc.h

  Summary:
    Defines the Oscillator (OSC) Peripheral Library interface.

  Description:
    This header file contains the function prototypes and definitions of
    the data types and constants that make up the interface to the Oscillator
    (OSC) Peripheral Library for Microchip microcontrollers.  The definitions
    in this file are for the Oscillator module.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2013-2015 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END

#ifndef _PLIB_OSC_H
#define _PLIB_OSC_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Include files
// *****************************************************************************
// *****************************************************************************

#include "peripheral/osc/processor/osc_processor.h"

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - General Setup
// *****************************************************************************
// *****************************************************************************
/*  APIs general configuration of the oscillator module */

//******************************************************************************
/* Function:
    void PLIB_OSC_OnWaitActionSet ( OSC_MODULE_ID index,
    				    OSC_OPERATION_ON_WAIT onWait )

  Summary:
    Selects the operation to be performed when a WAIT instruction is executed.

  Description:
    This function selects the operation to be performed when a WAIT
    instruction is executed.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module
    onWait     - Operation to be performed when a WAIT instruction is
                 executed. One of the possible values of OSC_OPERATION_ON_WAIT.

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_OnWaitActionSet(OSC_ID_0, OSC_ON_WAIT_SLEEP);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsOnWaitAction
	in your application to determine whether this feature is available.

    If this function is not called, the device will enter Idle mode on execution of a
    WAIT instruction.
*/

void PLIB_OSC_OnWaitActionSet ( OSC_MODULE_ID index,
								OSC_OPERATION_ON_WAIT onWait );


//******************************************************************************
/* Function:
    OSC_OPERATION_ON_WAIT PLIB_OSC_OnWaitActionGet ( OSC_MODULE_ID index )

  Summary:
    Gets the configured operation to be performed when a WAIT instruction is
    executed.

  Description:
    This function gets the configured operation that is to be performed when a
    WAIT instruction is executed.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    On a WAIT action, one of the possible values of OSC_OPERATION_ON_WAIT.

  Example:
    <code>
    if (PLIB_OSC_OnWaitActionGet(OSC_ID_0) == OSC_ON_WAIT_SLEEP)
    {
		//Do some action
	}
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsOnWaitAction
	in your application to determine whether this feature is available.
*/

OSC_OPERATION_ON_WAIT PLIB_OSC_OnWaitActionGet ( OSC_MODULE_ID index );


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Secondary Oscillator Setup
// *****************************************************************************
// *****************************************************************************
/*  APIs for configuring Secondary Oscillator */

//******************************************************************************
/* Function:
    void PLIB_OSC_SecondaryEnable ( OSC_MODULE_ID index )

  Summary:
    Enables the Secondary Oscillator.

  Description:
    This function enables the Secondary Oscillator.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_SecondaryEnable(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSecondaryEnable
	in your application to determine whether this feature is available.
*/

void PLIB_OSC_SecondaryEnable ( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_OSC_SecondaryDisable ( OSC_MODULE_ID index );

  Summary:
    Disables the Secondary Oscillator.

  Description:
    This function disables the Secondary Oscillator.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_SecondaryDisable(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSecondaryEnable
	in your application to determine whether this feature is available.
*/

void PLIB_OSC_SecondaryDisable ( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_OSC_SecondaryIsEnabled ( OSC_MODULE_ID index );

  Summary:
    Returns 'true' if the Secondary Oscillator is enabled.

  Description:
    This function returns 'true' if the Secondary Oscillator is enabled.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    - true    - The Secondary Oscillator is enabled
    - false   - The Secondary Oscillator is disabled

  Example:
    <code>
    bool secOscEnable;

    secOscEnable = PLIB_OSC_SecondaryIsEnabled(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSecondaryEnable
	in your application to determine whether this feature is available.
*/

bool PLIB_OSC_SecondaryIsEnabled ( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_OSC_SecondaryIsReady ( OSC_MODULE_ID index )

  Summary:
    Returns 'true' if the Secondary Oscillator is ready.

  Description:
    This function returns the ready status of the Secondary Oscillator.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    - true    - Indicates that the Secondary Oscillator is running and is stable
    - false   - Indicates that the Secondary Oscillator is either turned off or 
	            is still warming up

  Example:
    <code>
    bool secOscReady;

    secOscReady = PLIB_OSC_SecondaryIsReady(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSecondaryReady
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_SecondaryIsReady( OSC_MODULE_ID index );

//******************************************************************************
/* Function:
    bool PLIB_OSC_ClockIsReady ( OSC_MODULE_ID index, OSC_CLOCK_ID clk)

  Summary:
    Get the ready status of clock.

  Description:
    This function returns the ready status of selected clock.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module.
    clk        - One of the values of OSC_CLOCK_ID.
    
  Returns:
    - true    - Indicates that the selected clock is running and is stable
    - false   - Indicates that the selected clock is either turned off or is 
	            still warming up

  Example:
    <code>
    bool clkReady;
    clkReady = PLIB_OSC_ClockIsReady(OSC_ID_0, OSC_CLOCK_FAST_RC);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use 
    PLIB_OSC_ExistsClockReadyStatus in your application to determine
    whether this feature is available.
 */

bool PLIB_OSC_ClockIsReady( OSC_MODULE_ID index, OSC_CLOCK_ID clk );

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Reference Oscillator Setup
// *****************************************************************************
// *****************************************************************************
/*  APIs for configuring Reference Oscillator */

//******************************************************************************
/* Function:
    void PLIB_OSC_ReferenceOscEnable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc )

  Summary:
    Enables the reference oscillator.

  Description:
    This function enables the reference oscillator.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_ReferenceOscEnable(OSC_ID_0, OSC_REFERENCE_1);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOscEnable
	in your application to determine whether this feature is available.

	If the device has a reference clock output enable control, calling this function
	may not give the reference clock output. Use the PLIB_OSC_ReferenceOutputEnable
	function to enable the output.

 */

void PLIB_OSC_ReferenceOscEnable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc );


//******************************************************************************
/* Function:
    void PLIB_OSC_ReferenceOscDisable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc )

  Summary:
    Disables the reference oscillator output.

  Description:
    This function disables output from the reference oscillator.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_ReferenceOscDisable(OSC_ID_0, OSC_REFERENCE_1);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOscEnable
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_ReferenceOscDisable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc );


//******************************************************************************
/* Function:
    bool PLIB_OSC_ReferenceOscIsEnabled ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc )

  Summary:
    Gets the enable status of the reference oscillator output.

  Description:
    This function gets the enable status of the reference oscillator output.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator

  Returns:
    - true   - The reference oscillator is enabled
    - false  - The reference oscillator is disabled

  Example:
    <code>
    if(PLIB_OSC_ReferenceOscIsEnabled(OSC_ID_0, OSC_REFERENCE_1))
    {
	//Do some action
    }
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOscEnable
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_ReferenceOscIsEnabled ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc );


//******************************************************************************
/* Function:
    void PLIB_OSC_ReferenceOscStopInSleepEnable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc )

  Summary:
    Configures the reference oscillator to stop operating in Sleep mode.

  Description:
    This function configures the reference oscillator to stop operating in
    Sleep mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_ReferenceOscStopInSleepEnable(OSC_ID_0, OSC_REFERENCE_1);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the specific
    device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOscStopInSleep
    in your application to determine whether this feature is available.

    The default state of the device is for the reference oscillator to be
    enabled in Sleep mode.
 */

void PLIB_OSC_ReferenceOscStopInSleepEnable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc );


//******************************************************************************
/* Function:
    void PLIB_OSC_ReferenceOscStopInSleepDisable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc )

  Summary:
    Enables the reference oscillator in Sleep mode.

  Description:
    This function enables the reference oscillator in Sleep mode. The reference
    oscillator continues to run in Sleep mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_ReferenceOsctopInSleepDisable(OSC_ID_0, OSC_REFERENCE_1);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the specific
    device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOscStopInSleep
    in your application to determine whether this feature is available.

    The reference clock output will be stopped in Sleep mode if the base clock
    selected is 'System Clock' or 'Peripheral Clock' regardless of this function.

    The default state of the device is for the reference oscillator to be
    enabled in Sleep mode. Therefore, calling this function is necessary only if the
    PLIB_OSC_ReferenceOscStopInSleepEnable function was previously called, and
    the software wants to enable oscillator operation in Sleep mode.
 */

void PLIB_OSC_ReferenceOscStopInSleepDisable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc );


//******************************************************************************
/* Function:
    bool PLIB_OSC_ReferenceOscStopInSleepIsEnabled ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc )

  Summary:
    Returns 'true' if the reference oscillator is disabled in Sleep mode.

  Description:
    This function returns 'true' if the reference oscillator is disabled in
    Sleep mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator

  Returns:
    - true   - The reference oscillator is disabled in Sleep mode
    - false  - The reference oscillator is enabled in Sleep mode

  Example:
    <code>
    bool refOscSleep;

    refOscSleep = PLIB_OSC_ReferenceOscStopInSleepIsEnabled(OSC_ID_0, OSC_REFERENCE_1);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOscStopInSleep
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_ReferenceOscStopInSleepIsEnabled ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc );


//******************************************************************************
/* Function:
    void PLIB_OSC_ReferenceOscStopInIdleEnable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc )

  Summary:
    Configures the reference oscillator to stop operating in Idle mode.

  Description:
    This function configures the reference oscillator to stop operating in
    Idle mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_ReferenceOscStopInIdleEnable(OSC_ID_0, OSC_REFERENCE_1);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the specific
    device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOscStopInIdleEnable
    in your application to determine whether this feature is available.

    The default state of the device is reference oscillator enabled in Idle
    mode. Therefore, calling this function is necessary only if the
    PLIB_OSC_ReferenceOscStopInIdleDisable function was previously called, and
    the software wants to enable oscillator operation in Sleep mode.
 */

void PLIB_OSC_ReferenceOscStopInIdleEnable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc );


//******************************************************************************
/* Function:
    void PLIB_OSC_ReferenceOscStopInIdleDisable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc )

  Summary:
    Enables the reference oscillator in Idle mode.

  Description:
    This function enables the reference oscillator in Idle mode. The reference
    oscillator continues to run in Idle mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_ReferenceOsctopInSleepDisable(OSC_ID_0, OSC_REFERENCE_1);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the specific
    device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOscStopInIdleEnable
    in your application to determine whether this feature is available.

    The default state of the device is the reference oscillator is enabled in
    Idle mode.
 */

void PLIB_OSC_ReferenceOscStopInIdleDisable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc );


//******************************************************************************
/* Function:
    bool PLIB_OSC_ReferenceOscStopInIdleIsEnabled ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc )

  Summary:
    Returns 'true' if the reference oscillator is disabled in Idle mode.

  Description:
    This function returns 'true' if the reference oscillator is disabled in
    Idle mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator

  Returns:
    - true   - The reference oscillator is disabled in Idle mode
    - false  - The reference oscillator is enabled in Idle mode

  Example:
    <code>
    bool refOscIdle;

    refOscIdle = PLIB_OSC_ReferenceOscStopInIdleIsEnabled(OSC_ID_0, OSC_REFERENCE_1);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOscStopInIdleEnable
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_ReferenceOscStopInIdleIsEnabled ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc );


//******************************************************************************
/* Function:
    void PLIB_OSC_ReferenceOutputEnable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc )

  Summary:
    Enables the reference oscillator output.

  Description:
    This function enables the reference oscillator output.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_ReferenceOutputEnable(OSC_ID_0, OSC_REFERENCE_1);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the specific
    device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOutputEnable
    in your application to determine whether this feature is available.

    The default state of the device is reference oscillator output disabled.
 */

void PLIB_OSC_ReferenceOutputEnable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc );


//******************************************************************************
/* Function:
    void PLIB_OSC_ReferenceOutputDisable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc )

  Summary:
    Disables the reference oscillator output.

  Description:
    This function disables the reference oscillator output.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_ReferenceOutputDisable(OSC_ID_0, OSC_REFERENCE_1);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the specific
    device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOutputEnable
    in your application to determine whether this feature is available.

    The default state of the device is reference oscillator output disabled.
 */

void PLIB_OSC_ReferenceOutputDisable ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc );


//******************************************************************************
/* Function:
    bool PLIB_OSC_ReferenceOutputIsEnabled ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc )

  Summary:
    Returns 'true' if the reference oscillator output is enabled.

  Description:
    This function returns 'true' if the reference oscillator output is enabled.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator

  Returns:
    - true   - The reference oscillator output is enabled
    - false  - The reference oscillator output is disabled

  Example:
    <code>
    bool refOscIdle;

    refOscIdle = PLIB_OSC_ReferenceOutputIsEnabled(OSC_ID_0, OSC_REFERENCE_1);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOutputEnable
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_ReferenceOutputIsEnabled ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc );


//******************************************************************************
/* Function:
    bool PLIB_OSC_ReferenceOscSourceChangeIsActive ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc )

  Summary:
    Returns 'true' if a reference oscillator source change request is active.

  Description:
    This function returns 'true' if the reference oscillator source change
    is in progress. The software is not allowed to give a new source change
    request.

  Precondition:
    None.

  Parameters:
    index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator

  Returns:
    - true   - The reference oscillator change request is active
    - false  - The reference oscillator change request is not active

  Example:
    <code>
    if (!PLIB_OSC_ReferenceOscSourceChangeIsActive(OSC_ID_0, OSC_REFERENCE_1))
    {
        //Allowed to change the reference clock source
    }
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOscChangeActive
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_ReferenceOscSourceChangeIsActive ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc );


//******************************************************************************
/* Function:
    void PLIB_OSC_ReferenceOscBaseClockSelect ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc,
    						                    OSC_REF_BASECLOCK refOscBaseClock )

  Summary:
    Sets the base clock for the reference oscillator.

  Description:
    This function sets the base clock for the reference oscillator. There are
    multiple clock sources by which the user can configure the module to output
    to the pin. Users can check the accuracy of the clock by probing the pin.
    Use the PLIB_OSC_ReferenceOscDivisorValueSet function to divide the clock if
    it is a very high value.

  Precondition:
    None.

  Parameters:
    index      	  - Identifies the desired oscillator module
	referenceOsc  - Identifies the desired reference oscillator
    refOscBaseClk - One of the possible values of OSC_REF_BASECLOCK

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_ReferenceOscBaseClockSelect(OSC_ID_0, OSC_REFERENCE_1, OSC_REF_BASECLOCK_PBCLK);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOscBaseClock
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_ReferenceOscBaseClockSelect ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc, 
                                            OSC_REF_BASECLOCK refOscBaseClock );


//******************************************************************************
/* Function:
    bool PLIB_OSC_ReferenceOscSwitchIsComplete ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc )

  Summary:
    Returns 'true' if the reference oscillator base clock switching is complete.

  Description:
    This function returns 'true' if the reference oscillator base clock
    switching is complete.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator

  Returns:
    - true   - The reference clock base clock switching is complete
    - false  - The reference clock base clock switching is not complete; switching
	           is not started

  Example:
    <code>
    bool refOscIdle;

    refOscIdle = PLIB_OSC_ReferenceOscSwitchIsComplete(OSC_ID_0, OSC_REFERENCE_1);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOscChange
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_ReferenceOscSwitchIsComplete ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc );


//******************************************************************************
/* Function:
    void PLIB_OSC_ReferenceOscDivisorValueSet ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc,
    						OSC_REF_DIVISOR_TYPE refOscDivValue )

  Summary:
    Selects the reference oscillator divisor value.

  Description:
    This function selects the reference oscillator divisor value.

  Precondition:
    None.

  Parameters:
  	index          - Identifies the desired oscillator module
	referenceOsc   - Identifies the desired reference oscillator
    refOscDivValue - Value for Reference Oscillator Divisor (RODIV) field.
                     If it is '0', the divider is not used.

  Returns:
    None.

  Example:
    <code>
    // Select the clock source.
    PLIB_OSC_ReferenceOscBaseClockSelect(OSC_ID_0, OSC_REFERENCE_1, OSC_REF_BASECLOCK_PBCLK);

    PLIB_OSC_ReferenceOscDivisorValueSet(OSC_ID_0, OSC_REFERENCE_1, 128);

    PLIB_OSC_ReferenceOscEnable(OSC_ID_0, OSC_REFERENCE_1);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOscDivisor
	in your application to determine whether this feature is available.

	The value entered may not be the actual divisor. Please refer to the
	specific device data sheet to determine the actual divisor corresponding
    to the value entered.

 */

void PLIB_OSC_ReferenceOscDivisorValueSet ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc,
					    OSC_REF_DIVISOR_TYPE refOscDivValue );


//******************************************************************************
/* Function:
    void PLIB_OSC_ReferenceOscTrimSet ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc,
    					OSC_REF_TRIM_TYPE trimValue )

  Summary:
    Sets the reference oscillator divisor trim value.

  Description:
    This function selects the reference oscillator divisor trim value. The
    value selected divided by OSC_REF_TRIM_MAX_VALUE will be added to
    the oscillator divisor value.

  Precondition:
    None.

  Parameters:
  	index        - Identifies the desired oscillator module
	referenceOsc - Identifies the desired reference oscillator
    trimValue    - Reference oscillator trim value

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_ReferenceOscTrimSet(OSC_ID_0, OSC_REFERENCE_1, 50);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsReferenceOscTrim
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_ReferenceOscTrimSet ( OSC_MODULE_ID index, OSC_REFERENCE referenceOsc,
                                    OSC_REF_TRIM_TYPE trimValue );


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Fast RC Oscillator Setup
// *****************************************************************************
// *****************************************************************************
/*  APIs for configuring RC Oscillator */

//******************************************************************************
/* Function:
    void PLIB_OSC_FRCDivisorSelect ( OSC_MODULE_ID index,
                                     OSC_FRC_DIV divisorFRC )

  Summary:
    Sets the FRC clock divisor to the specified value.

  Description:
    This function sets the FRC clock divisor to the specified value.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module
    divisorFRC - One of the possible values from OSC_FRC_DIV

  Returns:
    None.


  Example:
    <code>
    PLIB_OSC_FRCDivisorSelect ( OSC_ID_0, OSC_FRC_DIV_4 );
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsFRCDivisor
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_FRCDivisorSelect( OSC_MODULE_ID index,
				OSC_FRC_DIV divisorFRC );


//******************************************************************************
/* Function:
    uint16_t PLIB_OSC_FRCDivisorGet( OSC_MODULE_ID index )

  Summary:
    Gets the FRC clock divisor.

  Description:
    This function gets the FRC clock divisor. The value returned will be direct
    number and not enum equivalent.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    The FRC divisor value.


  Example:
    <code>
    uint16_t divisorFRC;

    divisorFRC = PLIB_OSC_FRCDivisorGet(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsFRCDivisor
	in your application to determine whether this feature is available.
*/

uint16_t PLIB_OSC_FRCDivisorGet( OSC_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_OSC_FRCTuningSelect ( OSC_MODULE_ID index,
                                    OSC_FRC_TUNE_TYPE tuningValue )

  Summary:
    Sets the FRC tuning value.

  Description:
    This function tunes the FRC oscillator to the value specified. The
    application is supposed to try different values and find the best one.
    If the device has different tuning modes, this function will be used
    differently. See the example provided for the PLIB_OSC_FRCTuningSequenceValueSet
    function for details.

  Precondition:
    None.

  Parameters:
    index       - Identifies the desired oscillator module
    tuningValue - Tuning value. One of the possible values from OSC_FRC_TUNE_TYPE.

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_FRCTuningSelect(OSC_ID_0, 0x05);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the specific
    device data sheet to determine availability or use PLIB_OSC_ExistsFRCTuning
    in your application to determine whether this feature is available.

 */

void PLIB_OSC_FRCTuningSelect ( OSC_MODULE_ID index,
				OSC_FRC_TUNE_TYPE tuningValue );

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Oscillator Switch Setup
// *****************************************************************************
// *****************************************************************************
/*  APIs for configuring oscillator switch */

//*************************************************************************
/*  Function:
       void PLIB_OSC_SysClockSelect ( OSC_MODULE_ID index,
                                       OSC_SYS_TYPE newOsc )

  Summary:
    Selects the new oscillator.

  Description:
    This function selects the new oscillator.

  Conditions:
    None.

  Input:
    index  -  Identifies the desired oscillator module
    newOsc -  One of the possible values from OSC_SYS_TYPE

  Return:
    None.

  Example:
    <code>

    PLIB_OSC_SysClockSelect(OSC_ID_0, OSC_PRIMARY);

    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_OSC_ExistsOscSelect in your application to determine whether this
    feature is available.

    This function adds the necessary delay (NOP instructions) after switching
    the oscillator. Therefore, the user need not add any delay as specified
    in the device data sheet.
  *************************************************************************/

void PLIB_OSC_SysClockSelect ( OSC_MODULE_ID index, OSC_SYS_TYPE newOsc );


//******************************************************************************
/* Function:
    void PLIB_OSC_ClockSwitchingAbort ( OSC_MODULE_ID index )

  Summary:
    Aborts an oscillator switch.

  Description:
    This function aborts the oscillator switch to the selection specified by the
    new oscillator selection bits.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_ClockSwitchingAbort(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsOscSwitchInit
	in your application to determine whether this feature is available.
*/

void PLIB_OSC_ClockSwitchingAbort ( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_OSC_ClockSwitchingIsComplete( OSC_MODULE_ID index );

  Summary:
    Gets the oscillator switch progress status.

  Description:
    This function gets the status of the oscillator switch progress.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    - true    - The oscillator switch is complete
    - false   - The oscillator switch is in progress

  Example:
    <code>
    PLIB_OSC_SysClockSelect(OSC_ID_0, OSC_PRIMARY);

    while(!PLIB_OSC_ClockSwitchingIsComplete(OSC_ID_0));

    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the specific
    device data sheet to determine availability or use PLIB_OSC_ExistsOscSwitchInit
    in your application to determine whether this feature is available.

*/

bool PLIB_OSC_ClockSwitchingIsComplete ( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    OSC_SYS_TYPE PLIB_OSC_CurrentSysClockGet ( OSC_MODULE_ID index )

  Summary:
    Gets the current oscillator selected.

  Description:
    This function gets the current oscillator. If the application has not changed
    the oscillator selection, this will be same as the oscillator selected
    through the Configuration bits.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    One of the possible values from OSC_SYS_TYPE.

  Example:
    <code>
    OSC_SYS_TYPE oscCurrent;

    oscCurrent = PLIB_OSC_CurrentSysClockGet(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsOscCurrentGet
	in your application to determine whether this feature is available.
*/

OSC_SYS_TYPE PLIB_OSC_CurrentSysClockGet ( OSC_MODULE_ID index );

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - USB and Display Clock Setup
// *****************************************************************************
// *****************************************************************************
/*  APIs for USB and Display clock */

//******************************************************************************
/* Function:
    void PLIB_OSC_UsbClockSourceSelect ( OSC_MODULE_ID index,
    					 OSC_USBCLOCK_SOURCE usbClock )

  Summary:
    Sets the USB module clock source.

  Description:
    This function sets the USB module clock source.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module
    usbClock   - Select the USB module clock source. One of the possible values
		         from OSC_USBCLOCK_SOURCE.
  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_UsbClockSourceSelect(OSC_ID_0, SYS_OSC_USBCLK_FRC);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the specific
    device data sheet to determine availability or use PLIB_OSC_ExistsUsbClockSource
    in your application to determine whether this feature is available.

    Before placing the USB module in Suspend mode, use this function to enable the
    FRC clock.
*/

void PLIB_OSC_UsbClockSourceSelect ( OSC_MODULE_ID index,
                                     OSC_USBCLOCK_SOURCE usbClock );


//******************************************************************************
/* Function:
    OSC_USBCLOCK_SOURCE PLIB_OSC_UsbClockSourceGet ( OSC_MODULE_ID index )

  Summary:
    Gets the USB module clock source.

  Description:
    This function gets the USB module clock source.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    USB module clock source. One of the possible values from OSC_USBCLOCK_SOURCE.

  Example:
    <code>
    if (SYS_OSC_USBCLK_FRC == PLIB_OSC_UsbClockSourceGet(OSC_ID_0))
    {
	//Do some action
    }
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsUsbClockSource
	in your application to determine whether this feature is available.
*/

OSC_USBCLOCK_SOURCE PLIB_OSC_UsbClockSourceGet ( OSC_MODULE_ID index );

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - PLL Setup
// *****************************************************************************
// *****************************************************************************
/*  APIs for PLL Setup */

//******************************************************************************
/* Function:
    bool PLIB_OSC_PLLIsLocked ( OSC_MODULE_ID index, OSC_PLL_SELECT pllselect )

  Summary:
    Returns 'true' if the selected PLL module is locked.

  Description:
    This function returns the lock status of the selected PLL module.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module
    pllselect  - Selects the PLL module

  Returns:
    - true    - The PLL module is in lock or the PLL module start-up timer is 
	            satisfied
    - false   - The PLL module is out of lock, the PLL start-up timer is running, 
	            or the PLL module is disabled

  Example:
    <code>
    bool lockPLL_status;

    lockPLL_status = PLIB_OSC_PLLIsLocked(OSC_ID_0, OSC_PLL_SYSTEM);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the specific
    device data sheet to determine availability or use PLIB_OSC_ExistsPLLLockStatus
    in your application to determine whether this feature is available.

    If the PLL does not stabilize properly during start-up, this function may not
    reflect the actual status of PLL lock, nor does it detect when the PLL
    loses lock during normal operation.
*/

bool PLIB_OSC_PLLIsLocked ( OSC_MODULE_ID index, OSC_PLL_SELECT pllselect );


//******************************************************************************
/* Function:
    void PLIB_OSC_PLLClockLock ( OSC_MODULE_ID index )

  Summary:
    Locks the clock and PLL selections.

  Description:
    This function locks the clock and PLL selections.
    This operation is atomic.

  Precondition:
    The Fail-Safe Clock Monitor (FSCM) should be enabled.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_PLLClockLock(OSC_ID_0);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the specific
    device data sheet to determine availability or use PLIB_OSC_ExistsPLLClockLock
    in your application to determine whether this feature is available.

    The data given by this function is only valid if clock switching and monitoring
    are enabled. Otherwise Clock and PLL selections are never locked and may
    be modified.
*/

void PLIB_OSC_PLLClockLock ( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_OSC_PLLClockUnlock ( OSC_MODULE_ID index )

  Summary:
    Unlocks the clock and PLL selections.

  Description:
    This function unlocks the clock and PLL selection so that the clock and PLL
    may be modified.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_PLLClockUnlock(OSC_ID_0);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the specific
    device data sheet to determine availability or use PLIB_OSC_ExistsPLLClockLock
    in your application to determine whether this feature is available.

*/

void PLIB_OSC_PLLClockUnlock ( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_OSC_PLLClockIsLocked ( OSC_MODULE_ID index )

  Summary:
    Gets the lock status for the clock and PLL selections.

  Description:
    This function gets the lock status for the clock and PLL selections.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index     - Identifies the desired oscillator module

  Returns:
    - true    - The clock and PLL selections are locked
    - false   - The clock and PLL selections are not locked

  Example:
    <code>
    bool clockPLL_st;

    clockPLL_st = PLIB_OSC_PLLClockIsLocked(OSC_ID_0);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the specific
    device data sheet to determine availability or use PLIB_OSC_ExistsPLLClockLock
    in your application to determine whether this feature is available.

    If the PLL does not stabilize properly during start-up, this function may not
    reflect the actual status of PLL lock, nor does it detect when the PLL
    loses lock during normal operation.
*/

bool PLIB_OSC_PLLClockIsLocked ( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_OSC_SysPLLMultiplierSelect ( OSC_MODULE_ID index,
                                        OSC_SYSPLL_MULTIPLIER_TYPE pll_multiplier )

  Summary:
    Sets the PLL multiplier to the specified value.

  Description:
    This function sets the PLL multiplier to the specified value.

  Precondition:
    None.

  Parameters:
  	index           - Identifies the desired oscillator module
    pll_multiplier  - One of the possible values of PB clock divisor
					  of type OSC_PLL_MUL_TYPE

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_SysPLLMultiplierSelect (OSC_ID_0, 0x08);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSysPLLMultiplier
	in your application to determine whether this feature is available.

	Use the PLL Multiplier value directly for the parameter 'pll_multiplier', 
	and NOT the  value of the 'PLLMULT' field. Use of the PLL Multiplier value is not 
	supported by the selected device, and therefore, library behavior is undefined.
	Refer to the specific device data sheet for information.
 */

void PLIB_OSC_SysPLLMultiplierSelect( OSC_MODULE_ID index,
                                   OSC_SYSPLL_MULTIPLIER_TYPE pll_multiplier );


//******************************************************************************
/* Function:
    OSC_SYSPLL_MULTIPLIER_TYPE PLIB_OSC_SysPLLMultiplierGet ( OSC_MODULE_ID index )

  Summary:
    Gets the PLL multiplier.

  Description:
    This function returns the PLL multiplier.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    One of the possible values of PB clock divisor of type OSC_SYSPLL_MULTIPLIER_TYPE.

  Example:
    <code>
    OSC_SYSPLL_MULTIPLIER_TYPE pll_multiply;

    pll_multiply = PLIB_OSC_SysPLLMultiplierGet(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSysPLLMultiplier
	in your application to determine whether this feature is available.

	The actual multiplier value will be returned, and NOT the 'PLLMULT' field value.
    Refer to the specific device data sheet for information.
 */

OSC_SYSPLL_MULTIPLIER_TYPE PLIB_OSC_SysPLLMultiplierGet( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_OSC_SysPLLOutputDivisorSet ( OSC_MODULE_ID index,
                                           OSC_SYSPLL_OUT_DIV PLLOutDiv )

  Summary:
    Sets the output divider for the PLL to the specified value.

  Description:
    This function sets the output divider for the PLL to the specified value.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
    PLLOutDiv  - One of the possible values from OSC_SYSPLL_OUT_DIV

  Returns:
    None.


  Example:
    <code>
    PLIB_OSC_SysPLLOutputDivisorSelect(OSC_ID_0, OSC_SYSPLL_OUT_DIV_1);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSysPLLOutputDivisor
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_SysPLLOutputDivisorSet ( OSC_MODULE_ID index,
                                       OSC_SYSPLL_OUT_DIV PLLOutDiv );


//******************************************************************************
/* Function:
    uint16_t PLIB_OSC_SysPLLOutputDivisorGet ( OSC_MODULE_ID index )

  Summary:
    Gets the output divisor for the PLL.

  Description:
    This function returns the output divisor for the System PLL. The value is the
    actual divisor and not the enum value corresponding to it.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    System PLL output divisor value.


  Example:
    <code>
    uint16_t pllOutDiv;

    pllOutDiv = PLIB_OSC_SysPLLOutputDivisorGet(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSysPLLOutputDivisor
	in your application to determine whether this feature is available.
 */

uint16_t PLIB_OSC_SysPLLOutputDivisorGet ( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_OSC_SysPLLInputDivisorSet ( OSC_MODULE_ID index,
                                           uint16_t PLLInDiv )

  Summary:
    Sets the input divider for the PLL to the specified value.

  Description:
    This function sets the input divider for the PLL to the specified value.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
    PLLInDiv   - System PLL input divisor value

  Returns:
    None.


  Example:
    <code>
    PLIB_OSC_SysPLLInputDivisorSet( OSC_ID_0, 3 );
    </code>

  Remarks:
  	Pass the direct divisor instead of the register value equivalent.

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSysPLLInputDivisor
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_SysPLLInputDivisorSet ( OSC_MODULE_ID index,
                                       uint16_t PLLInDiv );


//******************************************************************************
/* Function:
    uint16_t PLIB_OSC_SysPLLInputDivisorGet ( OSC_MODULE_ID index )

  Summary:
    Gets the input divisor for the PLL.

  Description:
    This function returns the input divisor for the PLL.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    The System PLL input divisor as a number.


  Example:
    <code>
    uint16_t pllInDiv;

    PLLInDiv = PLIB_OSC_SysPLLInputDivisorGet(OSC_ID_0);
    </code>

  Remarks:
  	The direct register value itself is returned instead of the register value
  	equivalent.

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSysPLLInputDivisor
	in your application to determine whether this feature is available.
 */

uint16_t PLIB_OSC_SysPLLInputDivisorGet( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_OSC_SysPLLInputClockSourceSet ( OSC_MODULE_ID index,
                                              OSC_SYSPLL_IN_CLK_SOURCE PLLInClockSource )

  Summary:
    Sets the input clock source for the PLL module.

  Description:
    This function sets the input clock source for the PLL module

  Precondition:
    None.

  Parameters:
  	index            - Identifies the desired oscillator module
    PLLInClockSource - One of the possible values from OSC_SYSPLL_IN_CLK_SOURCE

  Returns:
    None.


  Example:
    <code>
    PLIB_OSC_SysPLLInputClockSourceSet(OSC_ID_0, OSC_SYSPLL_IN_CLK_SOURCE_FRC);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSysPLLInputClockSource
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_SysPLLInputClockSourceSet ( OSC_MODULE_ID index,
                                       OSC_SYSPLL_IN_CLK_SOURCE PLLInClockSource );


//******************************************************************************
/* Function:
    OSC_SYSPLL_IN_CLK_SOURCE PLIB_OSC_SysPLLInputClockSourceGet ( OSC_MODULE_ID index )

  Summary:
    Gets the input clock source for the PLL module.

  Description:
    This function returns the input clock source for the PLL module.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module


  Returns:
    * PLLInClockSource - One of the possible values from OSC_SYSPLL_IN_CLK_SOURCE


  Example:
    <code>
	OSC_SYSPLL_IN_CLK_SOURCE PLLInClockSource;
    PLLInClockSource = PLIB_OSC_SysPLLInputClockSourceGet(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSysPLLInputClockSource
	in your application to determine whether this feature is available.
 */

OSC_SYSPLL_IN_CLK_SOURCE PLIB_OSC_SysPLLInputClockSourceGet ( OSC_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_OSC_SysPLLFrequencyRangeSet ( OSC_MODULE_ID index,
                                           OSC_SYSPLL_FREQ_RANGE PLLFrequencyRange )

  Summary:
    Sets the frequency range for the PLL module.

  Description:
    This function sets the frequency range for the PLL module.

  Precondition:
    None.

  Parameters:
  	index             - Identifies the desired oscillator module
    PLLFrequencyRange - One of the possible values from OSC_SYSPLL_FREQ_RANGE

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_SysPLLFrequencyRangeSet(OSC_ID_0, OSC_SYSPLL_FREQ_RANGE_5M_TO_10M);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSysPLLFrequencyRange
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_SysPLLFrequencyRangeSet ( OSC_MODULE_ID index,
                                       OSC_SYSPLL_FREQ_RANGE PLLFrequencyRange );


//******************************************************************************
/* Function:
    OSC_SYSPLL_FREQ_RANGE PLIB_OSC_SysPLLFrequencyRangeGet ( OSC_MODULE_ID index )

  Summary:
    Gets the frequency range for the PLL module.

  Description:
    This function returns the frequency range set for the PLL module.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module

  Returns:
    * PLLFrequencyRange - One of the possible values from OSC_SYSPLL_FREQ_RANGE

  Example:
    <code>
	OSC_SYSPLL_FREQ_RANGE PLLFrequencyRange;
    PLLFrequencyRange = PLIB_OSC_SysPLLFrequencyRangeGet(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSysPLLFrequencyRange
	in your application to determine whether this feature is available.
 */

OSC_SYSPLL_FREQ_RANGE PLIB_OSC_SysPLLFrequencyRangeGet ( OSC_MODULE_ID index );

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Peripheral Bus Clock Setup
// *****************************************************************************
// *****************************************************************************
/*  APIs for Peripheral Bus Clock Setup */

//******************************************************************************
/* Function:
    bool PLIB_OSC_PBClockDivisorIsReady( OSC_MODULE_ID index,
										OSC_PERIPHERAL_BUS peripheralBusNumber )

  Summary:
    Checks whether the peripheral bus clock divisor is ready to be written.

  Description:
    This function checks whether the peripheral bus clock divisor is ready to
    be written.

  Precondition:
    None.

  Parameters:
    index               - Identifies the desired oscillator module
	peripheralBusNumber - Identifies the desired peripheral bus

  Returns:
    - true  - The peripheral bus clock divisor can be written
    - false - The peripheral bus clock divisor cannot be written

  Example:
    <code>
    if (PLIB_OSC_PBClockDivisorIsReady(OSC_ID_0, OSC_PERIPHERAL_BUS_1))
    {
        PLIB_OSC_PBClockDivisorSet (OSC_ID_0, OSC_PERIPHERAL_BUS_1,
									0x01);
    }
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsPBClockReady
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_PBClockDivisorIsReady( OSC_MODULE_ID index,
									OSC_PERIPHERAL_BUS peripheralBusNumber );


//******************************************************************************
/* Function:
    void PLIB_OSC_PBClockDivisorSet( OSC_MODULE_ID index,
									 OSC_PERIPHERAL_BUS peripheralBusNumber,
                                     OSC_PB_CLOCK_DIV_TYPE peripheralBusClkDiv )

  Summary:
    Sets the peripheral bus clock divisor to the specified value.

  Description:
    This function sets the peripheral bus clock divisor to the specified value.

  Precondition:
    Peripheral bus clock divisor should be ready to be written.

  Parameters:
    index  	            - Identifies the desired oscillator module
	peripheralBusNumber - Identifies the desired peripheral bus
    peripheralBusClkDiv - One of the possible values of PB clock divisor
						  of type OSC_PB_CLOCK_DIV_TYPE

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_PBClockDivisorSet (OSC_ID_0, OSC_PERIPHERAL_BUS_1, 0x01);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsPBClockDivisor
	in your application to determine whether this feature is available.

	Use PB Divider value directly for the parameter 'peripheralBusClkDiv', and NOT the
	value of the 'PBDIV' field. Use of the PB Divider value is not supported by 
	the selected device, and therefore, library behavior is undefined.
	Refer the the specific device data sheet for information.
 */

void PLIB_OSC_PBClockDivisorSet( OSC_MODULE_ID index,
								 OSC_PERIPHERAL_BUS peripheralBusNumber,
                                 OSC_PB_CLOCK_DIV_TYPE peripheralBusClkDiv );


//******************************************************************************
/* Function:
    OSC_PB_CLOCK_DIV_TYPE PLIB_OSC_PBClockDivisorGet ( OSC_MODULE_ID index,
										       OSC_PERIPHERAL_BUS peripheralBusNumber )

  Summary:
    Gets the peripheral bus clock divisor.

  Description:
    This function returns the peripheral bus clock divisor.

  Precondition:
    None.

  Parameters:
    index               - Identifies the desired oscillator module
	peripheralBusNumber - Identifies the desired peripheral bus

  Returns:
    One of the possible values of PB clock divisor of type OSC_PB_CLOCK_DIV_TYPE.

  Example:
    <code>
    OSC_PB_CLOCK_DIV_TYPE peripheralBusClkDiv;

    peripheralBusClkDiv = PLIB_OSC_PBClockDivisorGet(OSC_ID_0,
													OSC_PERIPHERAL_BUS_1);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsPBClockDivisor
	in your application to determine whether this feature is available.

	Note: Actual Divisor value will be returned, NOT the 'PBDIV' field value.
		  Refer the data sheet of the device for the detail.
 */

OSC_PB_CLOCK_DIV_TYPE PLIB_OSC_PBClockDivisorGet( OSC_MODULE_ID index,
										OSC_PERIPHERAL_BUS peripheralBusNumber );


//******************************************************************************
/* Function:
    void PLIB_OSC_PBOutputClockEnable( OSC_MODULE_ID index,
										OSC_PERIPHERAL_BUS peripheralBusNumber )

  Summary:
    Enables the peripheral bus output clock

  Description:
    This function enables the peripheral bus output clock

  Precondition:
    None.

  Parameters:
    index               - Identifies the desired oscillator module
	peripheralBusNumber - Identifies the desired peripheral bus

  Returns:
    None

  Example:
    <code>

        PLIB_OSC_PBOutputClockEnable (OSC_ID_0, OSC_PERIPHERAL_BUS_2);

    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsPBClockOutputEnable
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_PBOutputClockEnable( OSC_MODULE_ID index,
									OSC_PERIPHERAL_BUS peripheralBusNumber );


//******************************************************************************
/* Function:
    void PLIB_OSC_PBOutputClockDisable( OSC_MODULE_ID index,
										OSC_PERIPHERAL_BUS peripheralBusNumber )

  Summary:
    Disables the peripheral bus output clock.

  Description:
    This function disables the peripheral bus output clock.

  Precondition:
    None.

  Parameters:
    index               - Identifies the desired oscillator module
	peripheralBusNumber - Identifies the desired peripheral bus

  Returns:
    None

  Example:
    <code>

        PLIB_OSC_PBOutputClockDisable (OSC_ID_0, OSC_PERIPHERAL_BUS_2);

    </code>

  Remarks:
	The clock for peripheral bus 1 cannot be turned off.

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsPBClockOutputEnable
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_PBOutputClockDisable( OSC_MODULE_ID index,
									OSC_PERIPHERAL_BUS peripheralBusNumber );


//******************************************************************************
/* Function:
    bool PLIB_OSC_PBOutputClockIsEnabled( OSC_MODULE_ID index,
										OSC_PERIPHERAL_BUS peripheralBusNumber )

  Summary:
    Checks whether or not the peripheral bus clock output is enabled.

  Description:
    This function checks whether or not the peripheral bus clock output is enabled.

  Precondition:
    None.

  Parameters:
    index               - Identifies the desired oscillator module
	peripheralBusNumber - Identifies the desired peripheral bus

  Returns:
    - true  - The peripheral bus clock output is enabled
    - false - The peripheral bus clock output is disabled

  Example:
    <code>
    if (PLIB_OSC_PBOutputClockIsEnabled(OSC_ID_0, OSC_PERIPHERAL_BUS_2))
    {
        PLIB_OSC_PBOutputClockDisable(OSC_ID_0, OSC_PERIPHERAL_BUS_2);
    }
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsPBClockOutputEnable
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_PBOutputClockIsEnabled( OSC_MODULE_ID index,
									OSC_PERIPHERAL_BUS peripheralBusNumber );


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Clock Fail Monitoring
// *****************************************************************************
// *****************************************************************************
/*  APIs for Clock Fail Monitoring */

//*******************************************************************************
/*  Function:
    bool PLIB_OSC_ClockHasFailed ( OSC_MODULE_ID index );

  Summary:
    Returns 'true' if the clock fails.

  Description:
    This function returns 'true' if the clock fails. Monitors the Fail-Safe Clock
    Monitor (FSCM).
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    - true    - The FSCM detected a clock failure
    - false   - The no clock failure has been detected

  Example:
    <code>
    bool clockStatus;
    clockStatus = PLIB_OSC_ClockHasFailed(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsClockFail
	in your application to determine whether this feature is available.
*/

bool PLIB_OSC_ClockHasFailed ( OSC_MODULE_ID index );

//*************************************************************************
/*  Function:
     void PLIB_OSC_SleepToStartupClockSelect 
     ( 
        OSC_MODULE_ID index, 
        OSC_SLEEP_TO_STARTUP_CLK_TYPE startupOsc 
     )

  Summary:
    Selects the clock duration for when the device wakes from sleep 
	and the clock is ready.

  Description:
    When a device enters Sleep mode, the source of the system clock usually stops and
    takes some time after waking up to start running at full speed. This
    function allows the user to select a clock for that duration in which
    the actual clock source is starting up.

  Conditions:
    None.

  Input:
    index      -   Identifies the desired oscillator module
    startupOsc -   One of the possible values from OSC_SLEEP_TO_STARTUP_CLK_TYPE

  Return:
    None.

  Example:
    <code>
        PLIB_OSC_SleepToStartupClockSelect(OSC_ID_0, OSC_SLEEP_TO_STARTUP_CLK_FRC);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_OSC_ExistsSleepToStartupClock in your application to determine whether
    this feature is available.
*/

void PLIB_OSC_SleepToStartupClockSelect 
     ( 
        OSC_MODULE_ID index, 
        OSC_SLEEP_TO_STARTUP_CLK_TYPE startupOsc 
     );

//*************************************************************************
/*  Function:
     OSC_SLEEP_TO_STARTUP_CLK_TYPE PLIB_OSC_SleepToStartupClockGet 
     ( 
        OSC_MODULE_ID index  
     )

  Summary:
    Returns the clock used for the duration when the device wakes from sleep and
    the clock ready. 

  Description:
    When a device enters Sleep mode, the source of the system clock usually stops and
    takes some time after waking up to start running at full speed. This
    function is to return the clock set for that duration in which
    the actual clock source is starting up.
    This operation is atomic.

  Conditions:
    None.

  Input:
    index      -   Identifies the desired oscillator module

  Return:
    * startupOsc -   One of the possible values from OSC_SLEEP_TO_STARTUP_CLK_TYPE

  Example:
    <code>
        OSC_SLEEP_TO_STARTUP_CLK_TYPE startupClk;
        startupClk = PLIB_OSC_SleepToStartupClockGet(OSC_ID_0);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_OSC_ExistsSleepToStartupClock in your application to determine whether
    this feature is available.
*/

OSC_SLEEP_TO_STARTUP_CLK_TYPE PLIB_OSC_SleepToStartupClockGet 
     ( 
        OSC_MODULE_ID index  
     );

//******************************************************************************
/* Function:
    bool PLIB_OSC_ClockSlewingIsActive ( OSC_MODULE_ID index )

  Summary:
    Returns the status of clock slewing.

  Description:
    This function returns the current status of clock switching slewing.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module.

  Returns:
    - true   - The clock frequency is being actively slewed to the new frequency.
    - false  - The clock switch has reached its final value

  Example:
    <code>
    if (PLIB_OSC_ClockSlewingIsActive(OSC_ID_0))
    {
        //wait for clock switch to complete
    }
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_OSC_ExistsClockSlewingStatus in your application to determine
    whether this feature is available.
 */

bool PLIB_OSC_ClockSlewingIsActive ( OSC_MODULE_ID index );
     
//******************************************************************************
/* Function:
    void PLIB_OSC_SlewEnable ( OSC_MODULE_ID index, OSC_CLOCK_SLEW_TYPE slewType )

  Summary:
    Enables the selected type of slewing.

  Description:
    This function enables slewing to an upward or downward frequency based on
    the selection.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module
	slewType   - One of the possible value from OSC_CLOCK_SLEW_TYPE

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_SlewEnable(OSC_ID_0, OSC_CLOCK_SLEW_DOWNWARD);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_OSC_ExistsSlewEnableControl in your application to determine
    whether this feature is available.
 */

void PLIB_OSC_SlewEnable ( OSC_MODULE_ID index, OSC_CLOCK_SLEW_TYPE slewType );


//******************************************************************************
/* Function:
    void PLIB_OSC_SlewDisable ( OSC_MODULE_ID index, OSC_CLOCK_SLEW_TYPE slewType )

  Summary:
    Disables the selected type of slewing.

  Description:
    This function disables slewing to an upward or downward frequency based on
    the selection.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module
	slewType   - One of the possible value from OSC_CLOCK_SLEW_TYPE

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_SlewDisable(OSC_ID_0, OSC_CLOCK_SLEW_DOWNWARD);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_OSC_ExistsSlewEnableControl in your application to determine
    whether this feature is available.
 */

void PLIB_OSC_SlewDisable ( OSC_MODULE_ID index, OSC_CLOCK_SLEW_TYPE slewType );


//******************************************************************************
/* Function:
    bool PLIB_OSC_SlewIsEnabled ( OSC_MODULE_ID index, OSC_CLOCK_SLEW_TYPE slewType )

  Summary:
    Returns 'true' if the reference oscillator is disabled in Idle mode.

  Description:
    This function returns 'true' if the reference oscillator is disabled in
    Idle mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module
	slewType   - One of the possible value from OSC_CLOCK_SLEW_TYPE.

  Returns:
    - true   - The selected type of Slewing is enabled
    - false  - The selected type of Slewing is disabled

  Example:
    <code>
    bool slewStatus;
    slewStatus = PLIB_OSC_SlewIsEnabled(OSC_ID_0, OSC_CLOCK_SLEW_DOWNWARD);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_OSC_ExistsSlewEnableControl in your application to determine
    whether this feature is available.
 */

bool PLIB_OSC_SlewIsEnabled ( OSC_MODULE_ID index, OSC_CLOCK_SLEW_TYPE slewType );

//*************************************************************************
/*  Function:
     void PLIB_OSC_SlewDivisorStepSelect 
     ( 
        OSC_MODULE_ID index, 
        uint32_t slewSteps 
     )

  Summary:
    Selects division steps used while slewing.

  Description:
    This API selects number of division steps to be used when slewing during a
    frequency change. If the number of steps chosen is 0, no divisor will be
    used while slewing. If the number of steps chosen is 1, slewing will start 
	with divide by 2, and then no divisor. Similarly, if the number of steps 
	chosen is 2, slewing will start with divisor 4. Then,after a few seconds 
	the divisor will be 2, and then after a few more seconds no divisor will 
	be used and so on. Therefore, the largest step would be equal to  
    2^(number of steps), and then the step size will reduce by half every
    few seconds.
    
  Preconditions:
    None.

  Parameters:
    index      -   Identifies the desired oscillator module
    slewSteps  -   Number of steps in which slewing is desired

  Returns:
    None.

  Example:
    <code>
        PLIB_OSC_SlewDivisorStepSelect(OSC_ID_0, OSC_SLEW_DIVISOR_STEP_MAX_32);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_OSC_ExistsSlewDivisorStepControl in your application to determine whether
    this feature is available.
*/

void PLIB_OSC_SlewDivisorStepSelect 
     ( 
        OSC_MODULE_ID index, 
        uint32_t slewSteps 
     );

//*************************************************************************
/*  Function:
     uint32_t PLIB_OSC_SlewDivisorStepGet 
     ( 
        OSC_MODULE_ID index  
     )

  Summary:
    Get the slew divisor maximum step.

  Description:
    This function returns the number of division steps used when slewing during a
    frequency change.
    This operation is atomic.

  Conditions:
    None.

  Input:
    index     -   Identifies the desired oscillator module

  Return:
    * slewSteps -   Number of steps in which slewing is occurring

  Example:
    <code>
        uint32_t slewSteps;
        slewSteps = PLIB_OSC_SlewDivisorStepGet(OSC_ID_0);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_OSC_ExistsSlewDivisorStepControl in your application to determine whether
    this feature is available.
*/

uint32_t PLIB_OSC_SlewDivisorStepGet(OSC_MODULE_ID index);

//*************************************************************************
/*  Function:
     void PLIB_OSC_SystemClockDivisorSelect 
     ( 
        OSC_MODULE_ID index, 
        uint32_t systemClkDivisor 
     )

  Summary:
    Selects system clock divisor.

  Description:
    This function selects the system clock divisor.

  Conditions:
    None.

  Input:
    index             -  Identifies the desired oscillator module
    systemClkDivisor  -  The value by which the system clock is to be divided

  Return:
    None.

  Example:
    <code>
        PLIB_OSC_SystemClockDivisorSelect(OSC_ID_0, OSC_SYSTEM_CLOCK_DIVIDED_BY_10);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_OSC_ExistsSystemClockDivisorControl in your application to determine whether
    this feature is available.
*/

void PLIB_OSC_SystemClockDivisorSelect 
     ( 
        OSC_MODULE_ID index, 
        uint32_t systemClkDivisor 
     );

//*************************************************************************
/*  Function:
     uint32_t PLIB_OSC_SystemClockDivisorGet 
     ( 
        OSC_MODULE_ID index  
     )

  Summary:
    Get the system clock divisor value.

  Description:
    This function returns the divisor set for system clock.
    This operation is atomic.

  Conditions:
    None.

  Input:
    index      -   Identifies the desired oscillator module

  Return:
    * systemClkDivisor - The value by which system clock is divided.

  Example:
    <code>
        uint32_t systemClkDivisor;
        systemClkDivisor = PLIB_OSC_SystemClockDivisorGet(OSC_ID_0);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_OSC_ExistsSystemClockDivisorControl in your application to determine whether
    this feature is available.
*/

uint32_t PLIB_OSC_SystemClockDivisorGet(OSC_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_OSC_UPLLMultiplierSelect ( OSC_MODULE_ID index,
                                        OSC_SYSPLL_MULTIPLIER_TYPE pll_multiplier )

  Summary:
    Sets the USB PLL multiplier to the specified value.

  Description:
    This function sets the USB PLL multiplier to the specified value.

  Precondition:
    None.

  Parameters:
  	index           - Identifies the desired oscillator module
    pll_multiplier  - One of the possible values between 0 and 255

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_UPLLMultiplierSelect (OSC_ID_0, 0x08);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsUPLLMultiplier
	in your application to determine whether this feature is available.

	Use the USB PLL Multiplier value directly for the parameter 'pll_multiplier', 
	and NOT the  value of the 'UPLLMULT' field. Use of the PLL Multiplier value is not 
	supported by the selected device, and therefore, library behavior is undefined.
	Refer to the specific device data sheet for information.
 */

void PLIB_OSC_UPLLMultiplierSelect( OSC_MODULE_ID index,
                                   OSC_SYSPLL_MULTIPLIER_TYPE pll_multiplier );


//******************************************************************************
/* Function:
    OSC_SYSPLL_MULTIPLIER_TYPE PLIB_OSC_UPLLMultiplierGet ( OSC_MODULE_ID index )

  Summary:
    Gets the USB PLL multiplier.

  Description:
    This function returns the USB PLL multiplier.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    One of the possible values of USB Multiplier of type OSC_SYSPLL_MULTIPLIER_TYPE.

  Example:
    <code>
    OSC_SYSPLL_MULTIPLIER_TYPE pll_multiply;

    pll_multiply = PLIB_OSC_UPLLMultiplierGet(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsUPLLMultiplier
	in your application to determine whether this feature is available.

	The actual multiplier value will be returned, and NOT the 'UPLLMULT' field value.
    Refer to the specific device data sheet for information.
 */

OSC_SYSPLL_MULTIPLIER_TYPE PLIB_OSC_UPLLMultiplierGet( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_OSC_UPLLOutputDivisorSet ( OSC_MODULE_ID index,
                                           OSC_UPLL_OUT_DIV PLLOutDiv )

  Summary:
    Sets the output divider for the USB PLL to the specified value.

  Description:
    This function sets the output divider for the USB PLL to the specified value.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
    PLLOutDiv  - One of the possible values from OSC_UPLL_OUT_DIV

  Returns:
    None.


  Example:
    <code>
    PLIB_OSC_UPLLOutputDivisorSelect(OSC_ID_0, OSC_UPLL_OUT_DIV_1);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsUPLLOutputDivisor
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_UPLLOutputDivisorSet ( OSC_MODULE_ID index,
                                       OSC_UPLL_OUT_DIV PLLOutDiv );


//******************************************************************************
/* Function:
    uint16_t PLIB_OSC_UPLLOutputDivisorGet ( OSC_MODULE_ID index )

  Summary:
    Gets the output divisor for the PLL.

  Description:
    This function returns the output divisor for the USB PLL. The value is the
    actual divisor and not the enum value corresponding to it.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    USB PLL output divisor value.


  Example:
    <code>
    uint16_t pllOutDiv;

    pllOutDiv = PLIB_OSC_UPLLOutputDivisorGet(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsUPLLOutputDivisor
	in your application to determine whether this feature is available.
 */

uint16_t PLIB_OSC_UPLLOutputDivisorGet ( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_OSC_UPLLInputDivisorSet ( OSC_MODULE_ID index,
                                           uint16_t PLLInDiv )

  Summary:
    Sets the input divider for the USB PLL to the specified value.

  Description:
    This function sets the input divider for the USB PLL to the specified value.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
    PLLInDiv   - USB PLL input divisor value

  Returns:
    None.


  Example:
    <code>
    PLIB_OSC_UPLLInputDivisorSet( OSC_ID_0, 3 );
    </code>

  Remarks:
  	Pass the direct divisor instead of the register value equivalent.

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsUPLLInputDivisor
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_UPLLInputDivisorSet ( OSC_MODULE_ID index,
                                       uint16_t PLLInDiv );


//******************************************************************************
/* Function:
    uint16_t PLIB_OSC_UPLLInputDivisorGet ( OSC_MODULE_ID index )

  Summary:
    Gets the input divisor for the USB PLL.

  Description:
    This function returns the input divisor for the USB PLL.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    The USB PLL input divisor as a number.


  Example:
    <code>
    uint16_t pllInDiv;

    PLLInDiv = PLIB_OSC_UPLLInputDivisorGet(OSC_ID_0);
    </code>

  Remarks:
  	The direct register value itself is returned instead of the register value
  	equivalent.

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsUPLLInputDivisor
	in your application to determine whether this feature is available.
 */

uint16_t PLIB_OSC_UPLLInputDivisorGet( OSC_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_OSC_BTPLLMultiplierSelect ( OSC_MODULE_ID index,
                                        OSC_SYSPLL_MULTIPLIER_TYPE pll_multiplier )

  Summary:
    Sets the Bluetooth PLL multiplier to the specified value.

  Description:
    This function sets the Bluetooth PLL multiplier to the specified value.

  Precondition:
    None.

  Parameters:
  	index           - Identifies the desired oscillator module
    pll_multiplier  - One of the possible values between 0 and 255

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_BTPLLMultiplierSelect (OSC_ID_0, 0x08);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsBTPLLMultiplier
	in your application to determine whether this feature is available.

	Use the USB PLL Multiplier value directly for the parameter 'pll_multiplier', 
	and NOT the  value of the 'BTPLLMULT' field. Use of the PLL Multiplier value is not 
	supported by the selected device, and therefore, library behavior is undefined.
	Refer to the specific device data sheet for information.
 */

void PLIB_OSC_BTPLLMultiplierSelect( OSC_MODULE_ID index,
                                   OSC_SYSPLL_MULTIPLIER_TYPE pll_multiplier );


//******************************************************************************
/* Function:
    OSC_SYSPLL_MULTIPLIER_TYPE PLIB_OSC_BTPLLMultiplierGet ( OSC_MODULE_ID index )

  Summary:
    Gets the Bluetooth PLL multiplier.

  Description:
    This function returns the Bluetooth PLL multiplier.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    One of the possible values of Bluetooth Multiplier of type OSC_SYSPLL_MULTIPLIER_TYPE.

  Example:
    <code>
    OSC_SYSPLL_MULTIPLIER_TYPE pll_multiply;

    pll_multiply = PLIB_OSC_BTPLLMultiplierGet(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsBTPLLMultiplier
	in your application to determine whether this feature is available.

	The actual multiplier value will be returned, and NOT the 'BTPLLMULT' field value.
    Refer to the specific device data sheet for information.
 */

OSC_SYSPLL_MULTIPLIER_TYPE PLIB_OSC_BTPLLMultiplierGet( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_OSC_BTPLLOutputDivisorSet ( OSC_MODULE_ID index,
                                           OSC_BTPLL_OUT_DIV PLLOutDiv )

  Summary:
    Sets the output divider for the Bluetooth PLL to the specified value.

  Description:
    This function sets the output divider for the Bluetooth PLL to the specified value.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
    PLLOutDiv  - One of the possible values from OSC_BTPLL_OUT_DIV

  Returns:
    None.


  Example:
    <code>
    PLIB_OSC_BTPLLOutputDivisorSelect(OSC_ID_0, OSC_BTPLL_OUT_DIV_1);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsBTPLLOutputDivisor
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_BTPLLOutputDivisorSet ( OSC_MODULE_ID index,
                                       OSC_BTPLL_OUT_DIV PLLOutDiv );


//******************************************************************************
/* Function:
    uint16_t PLIB_OSC_BTPLLOutputDivisorGet ( OSC_MODULE_ID index )

  Summary:
    Gets the output divisor for the PLL.

  Description:
    This function returns the output divisor for the Bluetooth PLL. The value is the
    actual divisor and not the enum value corresponding to it.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    Bluetooth PLL output divisor value.


  Example:
    <code>
    uint16_t pllOutDiv;

    pllOutDiv = PLIB_OSC_BTPLLOutputDivisorGet(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsBTPLLOutputDivisor
	in your application to determine whether this feature is available.
 */

uint16_t PLIB_OSC_BTPLLOutputDivisorGet ( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_OSC_BTPLLInputDivisorSet ( OSC_MODULE_ID index, uint16_t PLLInDiv )

  Summary:
    Sets the input divider for the Bluetooth PLL to the specified value.

  Description:
    This function sets the input divider for the Bluetooth PLL to the specified value.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
    PLLInDiv   - USB PLL input divisor value

  Returns:
    None.


  Example:
    <code>
    PLIB_OSC_BTPLLInputDivisorSet( OSC_ID_0, 3 );
    </code>

  Remarks:
  	Pass the direct divisor instead of the register value equivalent.

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsBTPLLInputDivisor
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_BTPLLInputDivisorSet ( OSC_MODULE_ID index, uint16_t PLLInDiv );


//******************************************************************************
/* Function:
    uint16_t PLIB_OSC_BTPLLInputDivisorGet ( OSC_MODULE_ID index )

  Summary:
    Gets the input divisor for the Bluetooth PLL.

  Description:
    This function returns the input divisor for the Bluetooth PLL.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired oscillator module

  Returns:
    The Bluetooth PLL input divisor as a number.


  Example:
    <code>
    uint16_t pllInDiv;

    PLLInDiv = PLIB_OSC_BTPLLInputDivisorGet(OSC_ID_0);
    </code>

  Remarks:
  	The direct register value itself is returned instead of the register value
  	equivalent.

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsBTPLLInputDivisor
	in your application to determine whether this feature is available.
 */

uint16_t PLIB_OSC_BTPLLInputDivisorGet( OSC_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_OSC_BTPLLInputClockSourceSet ( OSC_MODULE_ID index,
                                              OSC_BTPLL_IN_CLK_SOURCE PLLInClockSource )

  Summary:
    Sets the input clock source for the Bluetooth PLL module.

  Description:
    This function sets the input clock source for the Bluetooth PLL module

  Precondition:
    None.

  Parameters:
  	index            - Identifies the desired oscillator module
    PLLInClockSource - One of the possible values from OSC_BTPLL_IN_CLK_SOURCE

  Returns:
    None.


  Example:
    <code>
    PLIB_OSC_BTPLLInputClockSourceSet(OSC_ID_0, OSC_BTPLL_IN_CLK_SOURCE_FRC);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsBTPLLInputClockSource
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_BTPLLInputClockSourceSet ( OSC_MODULE_ID index,
                                       OSC_BTPLL_IN_CLK_SOURCE PLLInClockSource );


//******************************************************************************
/* Function:
    OSC_BTPLL_IN_CLK_SOURCE PLIB_OSC_BTPLLInputClockSourceGet ( OSC_MODULE_ID index )

  Summary:
    Gets the input clock source for the Bluetooth PLL module.

  Description:
    This function returns the input clock source for the Bluetooth PLL module.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module


  Returns:
    * PLLInClockSource - One of the possible values from OSC_BTPLL_IN_CLK_SOURCE


  Example:
    <code>
	OSC_BTPLL_IN_CLK_SOURCE PLLInClockSource;
    PLLInClockSource = PLIB_OSC_BTPLLInputClockSourceGet(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsBTPLLInputClockSource
	in your application to determine whether this feature is available.
 */

OSC_SYSPLL_IN_CLK_SOURCE PLIB_OSC_BTPLLInputClockSourceGet ( OSC_MODULE_ID index);

//******************************************************************************
/* Function:
    void PLIB_OSC_UPLLFrequencyRangeSet ( OSC_MODULE_ID index,
                                           OSC_UPLL_FREQ_RANGE PLLFrequencyRange )

  Summary:
    Sets the frequency range for the USB PLL module.

  Description:
    This function sets the frequency range for the USB PLL module.

  Precondition:
    None.

  Parameters:
  	index             - Identifies the desired oscillator module
    PLLFrequencyRange - One of the possible values from OSC_UPLL_FREQ_RANGE

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_UPLLFrequencyRangeSet(OSC_ID_0, OSC_UPLL_FREQ_RANGE_5M_TO_10M);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsUPLLFrequencyRange
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_UPLLFrequencyRangeSet ( OSC_MODULE_ID index,
                                       OSC_UPLL_FREQ_RANGE PLLFrequencyRange );


//******************************************************************************
/* Function:
    OSC_UPLL_FREQ_RANGE PLIB_OSC_UPLLFrequencyRangeGet ( OSC_MODULE_ID index )

  Summary:
    Gets the frequency range for the USB PLL module.

  Description:
    This function returns the frequency range set for the USB PLL module.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module

  Returns:
    * PLLFrequencyRange - One of the possible values from OSC_UPLL_FREQ_RANGE

  Example:
    <code>
	OSC_UPLL_FREQ_RANGE PLLFrequencyRange;
    PLLFrequencyRange = PLIB_OSC_UPLLFrequencyRangeGet(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsUPLLFrequencyRange
	in your application to determine whether this feature is available.
 */

OSC_UPLL_FREQ_RANGE PLIB_OSC_UPLLFrequencyRangeGet ( OSC_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_OSC_BTPLLFrequencyRangeSet ( OSC_MODULE_ID index,
                                           OSC_BTPLL_FREQ_RANGE PLLFrequencyRange )

  Summary:
    Sets the frequency range for the Bluetooth PLL module.

  Description:
    This function sets the frequency range for the Bluetooth PLL module.

  Precondition:
    None.

  Parameters:
  	index             - Identifies the desired oscillator module
    PLLFrequencyRange - One of the possible values from OSC_BTPLL_FREQ_RANGE

  Returns:
    None.

  Example:
    <code>
    PLIB_OSC_BTPLLFrequencyRangeSet(OSC_ID_0, OSC_BTPLL_FREQ_RANGE_5M_TO_10M);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsBTPLLFrequencyRange
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_BTPLLFrequencyRangeSet ( OSC_MODULE_ID index,
                                       OSC_BTPLL_FREQ_RANGE PLLFrequencyRange );


//******************************************************************************
/* Function:
    OSC_BTPLL_FREQ_RANGE PLIB_OSC_BTPLLFrequencyRangeGet ( OSC_MODULE_ID index )

  Summary:
    Gets the frequency range for the Bluetooth PLL module.

  Description:
    This function returns the frequency range set for the Bluetooth PLL module.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module

  Returns:
    * PLLFrequencyRange - One of the possible values from OSC_BTPLL_FREQ_RANGE

  Example:
    <code>
	OSC_BTPLL_FREQ_RANGE PLLFrequencyRange;
    PLLFrequencyRange = PLIB_OSC_BTPLLFrequencyRangeGet(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsBTPLLFrequencyRange
	in your application to determine whether this feature is available.
 */

OSC_BTPLL_FREQ_RANGE PLIB_OSC_BTPLLFrequencyRangeGet ( OSC_MODULE_ID index );
    
//******************************************************************************
/* Function:
    void PLIB_OSC_ClockStop ( OSC_MODULE_ID index, OSC_CLOCK_DIAG clk)

  Summary:
    Stops the specified clock source.

  Description:
    This function is used to stop a specified clock source.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
	clk        - Clock to be stopped

  Returns:
    None.

  Example:
    <code>	
    PLIB_OSC_ClockStop(OSC_ID_0, OSC_CLOCK_POSC_STOP);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsClockDiagStatus
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_ClockStop ( OSC_MODULE_ID index, OSC_CLOCK_DIAG clk);

//******************************************************************************
/* Function:
    void PLIB_OSC_ClockStart ( OSC_MODULE_ID index, OSC_CLOCK_DIAG clk)

  Summary:
    Starts the specified clock source.

  Description:
    This function is used to start a specified clock source if it has already 
	been stopped.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
	clk        - Clock to be started

  Returns:
    None.

  Example:
    <code>	
    PLIB_OSC_ClockStart(OSC_ID_0, OSC_CLOCK_POSC_STOP);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsClockDiagStatus
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_ClockStart ( OSC_MODULE_ID index, OSC_CLOCK_DIAG clk);

//******************************************************************************
/* Function:
    bool PLIB_OSC_ClockStopStatus ( OSC_MODULE_ID index, OSC_CLOCK_DIAG clk)

  Summary:
    returns the status of clock stop bit for the specified clock source.

  Description:
    This function is used to get the status of clock stop bit for the specified 
	clock source.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
	clk        - Clock source whose status to be checked

  Returns:
    Status of the clock stop bit for a specified clock source.

  Example:
    <code>	
    bool PoscStat = PLIB_OSC_ClockStart(OSC_ID_0, OSC_CLOCK_POSC_STOP);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsClockDiagStatus
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_ClockStopStatus ( OSC_MODULE_ID index, OSC_CLOCK_DIAG clk);

//******************************************************************************
/* Function:
    void PLIB_OSC_DreamModeEnable ( OSC_MODULE_ID index)

  Summary:
    Enables the dream mode.

  Description:
    This function is used to enable the dream mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module

  Returns:
    None.

  Example:
    <code>	
    PLIB_OSC_DreamModeEnable(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsDreamModeControl
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_DreamModeEnable ( OSC_MODULE_ID index);

//******************************************************************************
/* Function:
    void PLIB_OSC_DreamModeDisable ( OSC_MODULE_ID index)

  Summary:
    Disables the dream mode.

  Description:
    This function is used to disable the dream mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module

  Returns:
    None.

  Example:
    <code>	
    PLIB_OSC_DreamModeDisable(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsDreamModeControl
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_DreamModeDisable ( OSC_MODULE_ID index);

//******************************************************************************
/* Function:
    bool PLIB_OSC_DreamModeStatus ( OSC_MODULE_ID index)

  Summary:
    gets the status of the dream mode.

  Description:
    This function is used to get the status of the dream mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module

  Returns:
    Status of dream mode bit.

  Example:
    <code>	
    bool drmMode = PLIB_OSC_DreamModeStatus(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsDreamModeControl
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_DreamModeStatus ( OSC_MODULE_ID index);

//******************************************************************************
/* Function:
    void PLIB_OSC_ForceSPLLLockEnable ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel)

  Summary:
    Enables the Force PLL Lock feature for specified PLL.

  Description:
    This function is used to enable the Force PLL Lock feature for the specified
	PLL.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
	pllSel	   - PLL to be force locked
	
  Returns:
    None.

  Example:
    <code>	
    PLIB_OSC_ForceSPLLLockEnable(OSC_ID_0, OSC_PLL_USB);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsForceLock
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_ForceSPLLLockEnable ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel);

//******************************************************************************
/* Function:
    void PLIB_OSC_ForceSPLLLockDisable ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel)

  Summary:
    Disables the Force PLL Lock feature for specified PLL.

  Description:
    This function is used to disable the Force PLL Lock feature for the specified
	PLL.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
	pllSel	   - PLL for which force locked to be disabled
	
  Returns:
    None.

  Example:
    <code>	
    PLIB_OSC_ForceSPLLLockDisable(OSC_ID_0, OSC_PLL_USB);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsForceLock
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_ForceSPLLLockDisable ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel);

//******************************************************************************
/* Function:
    bool PLIB_OSC_ForceSPLLLockStatus ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel)

  Summary:
    gets the status of the force PLL Lock bit of the specified PLL.

  Description:
    This function is used to get the status of the force PLL Lock bit of the 
	specified PLL.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
	pllSel	   - PLL for which force lock status to be read.
	
  Returns:
    Status of Force PLL Lock Status for the specified PLL.

  Example:
    <code>	
    bool spllFLock = PLIB_OSC_ForceSPLLLockStatus(OSC_ID_0, OSC_PLL_USB);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsForceLock
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_ForceSPLLLockStatus ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel);

//******************************************************************************
/* Function:
    void PLIB_OSC_ResetPLLAssert ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel)

  Summary:
    Asserts the PLL reset for selected PLL.

  Description:
    This function is used to assert the PLL reset for the selected PLL.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
	pllSel	   - PLL for which reset to be asserted
	
  Returns:
    None.

  Example:
    <code>	
    PLIB_OSC_ResetPLLAssert(OSC_ID_0, OSC_PLL_USB);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsResetPLL
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_ResetPLLAssert ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel);

//******************************************************************************
/* Function:
    void PLIB_OSC_ResetPLLDeassert ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel)

  Summary:
    Deasserts the PLL reset for selected PLL.

  Description:
    This function is used to deassert the PLL reset for the selected PLL.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
	pllSel	   - PLL for which reset to be deasserted
	
  Returns:
    None.

  Example:
    <code>	
    PLIB_OSC_ResetPLLDeassert(OSC_ID_0, OSC_PLL_USB);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsResetPLL
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_ResetPLLDeassert ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel);

//******************************************************************************
/* Function:
    bool PLIB_OSC_ResetPLLStatus ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel)

  Summary:
    gets the status of the PLL reset bit for the specified PLL.

  Description:
    This function is used to get the status of the PLL reset bit for the 
	specified PLL.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
	pllSel	   - PLL whose reset status to be checked
	
  Returns:
    Status of PLL reset bit for the specified PLL.

  Example:
    <code>	
    bool upllReset = PLIB_OSC_ResetPLLStatus(OSC_ID_0, OSC_PLL_USB);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsResetPLL
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_ResetPLLStatus ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel);

//******************************************************************************
/* Function:
    void PLIB_OSC_PLLBypassEnable ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel )

  Summary:
    Enables the PLL Bypass.

  Description:
    This function is used to enable the PLL Bypass for a specified PLL.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
	pllSel     - PLL for which bypass has to be enabled
	
  Returns:
    None.

  Example:
    <code>	
    PLIB_OSC_PLLBypassEnable(OSC_ID_0, OSC_PLL_USB);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsPLLBypass
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_PLLBypassEnable ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel );

//******************************************************************************
/* Function:
    void PLIB_OSC_SPLLBypassDisable ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel )

  Summary:
    Disables the PLL Bypass.

  Description:
    This function is used to disable the PLL Bypass for a specified PLL.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
	pllSel     - PLL for which bypass has to be disabled
	
  Returns:
    None.

  Example:
    <code>	
    PLIB_OSC_PLLBypassDisable(OSC_ID_0, OSC_PLL_USB);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSPLLBypass
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_PLLBypassDisable ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel );

//******************************************************************************
/* Function:
    bool PLIB_OSC_SPLLBypassStatus ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel )

  Summary:
    gets the status of the PLL Bypass.

  Description:
    This function is used to get the status of the PLL Bypass bit for a specified
	PLL.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
	pllSel     - PLL whose Bypass status has to be read
	
  Returns:
    Status of SPLL Bypass bit.

  Example:
    <code>	
    bool spllBypass = PLIB_OSC_SPLLBypassStatus(OSC_ID_0. OSC_PLL_USB);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsSPLLBypass
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_PLLBypassStatus ( OSC_MODULE_ID index, OSC_PLL_SELECT pllSel );

//******************************************************************************
/* Function:
    void PLIB_OSC_BTPLLClockOutEnable ( OSC_MODULE_ID index)

  Summary:
    Enables the Bluetooth PLL clock Ouput.

  Description:
    This function is used to enable the Bluetooth PLL clock Output to the SoC pin.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
	
  Returns:
    None.

  Example:
    <code>	
    PLIB_OSC_BTPLLClockOutEnable(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsBTPLLClockOut
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_BTPLLClockOutEnable ( OSC_MODULE_ID index);

//******************************************************************************
/* Function:
    void PLIB_OSC_BTPLLClockOutDisable ( OSC_MODULE_ID index)

  Summary:
    Disables the Bluetooth PLL Clock Output.

  Description:
    This function is used to disable the Bluetooth PLL clock output to SoC pin.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module
	
  Returns:
    None.

  Example:
    <code>	
    PLIB_OSC_BTPLLClockOutDisable(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsBTPLLClockOut
	in your application to determine whether this feature is available.
 */

void PLIB_OSC_BTPLLClockOutDisable ( OSC_MODULE_ID index);

//******************************************************************************
/* Function:
    bool PLIB_OSC_BTPLLClockOutStatus ( OSC_MODULE_ID index)

  Summary:
    gets the status of the Bluetooth PLL clock Output.

  Description:
    This function is used to get the status of the Bluetooth PLL clock Output.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
  	index      - Identifies the desired oscillator module

  Returns:
    Status of Bluetooth PLL clock output.

  Example:
    <code>	
    bool btpllClkOut = PLIB_OSC_BTPLLClockOutStatus(OSC_ID_0);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_OSC_ExistsBTPLLClockOut
	in your application to determine whether this feature is available.
 */

bool PLIB_OSC_BTPLLClockOutStatus ( OSC_MODULE_ID index);

// *****************************************************************************
// *****************************************************************************
// Section: OSC Peripheral Library Exists Functions
// *****************************************************************************
// *****************************************************************************
/* The following functions indicate the existence of the features on the device.
*/

//******************************************************************************
/* Function:
    PLIB_OSC_ExistsOnWaitAction( OSC_MODULE_ID index )

  Summary:
    Identifies whether the OnWaitAction feature exists on the Oscillator module.

  Description:
    This function identifies whether the OnWaitAction feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_OnWaitActionSet
    - PLIB_OSC_OnWaitActionGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the OnWaitAction feature is supported on the device
    - false  - If the OnWaitAction feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsOnWaitAction( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsSecondaryEnable( OSC_MODULE_ID index )

  Summary:
    Identifies whether the SecondaryEnable feature exists on the Oscillator module.

  Description:
    This function identifies whether the SecondaryEnable feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_SecondaryEnable
    - PLIB_OSC_SecondaryDisable
    - PLIB_OSC_SecondaryIsEnabled
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the SecondaryEnable feature is supported on the device
    - false  - If the SecondaryEnable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsSecondaryEnable( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsSecondaryReady( OSC_MODULE_ID index )

  Summary:
    Identifies whether the SecondaryReady feature exists on the Oscillator module.

  Description:
    This function identifies whether the SecondaryReady feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_SecondaryIsReady
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the SecondaryReady feature is supported on the device
    - false  - If the SecondaryReady feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsSecondaryReady( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsFRCDivisor( OSC_MODULE_ID index )

  Summary:
    Identifies whether the FRCDivisor feature exists on the Oscillator module.

  Description:
    This function identifies whether the FRCDivisor feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_FRCDivisorSelect
    - PLIB_OSC_FRCDivisorGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the FRCDivisor feature is supported on the device
    - false  - If the FRCDivisor feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsFRCDivisor( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsOscSelect( OSC_MODULE_ID index )

  Summary:
    Identifies whether the OscSelect feature exists on the Oscillator module.

  Description:
    This function identifies whether the OscSelect feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_SysClockSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the OscSelect feature is supported on the device
    - false  - If the OscSelect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsOscSelect( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsOscSwitchInit( OSC_MODULE_ID index )

  Summary:
    Identifies whether the OscSwitchInit feature exists on the Oscillator module.

  Description:
    This function identifies whether the OscSwitchInit feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_ClockSwitchingAbort
    - PLIB_OSC_ClockSwitchingIsComplete
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the OscSwitchInit feature is supported on the device
    - false  - If the OscSwitchInit feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsOscSwitchInit( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsOscCurrentGet( OSC_MODULE_ID index )

  Summary:
    Identifies whether the OscCurrentGet feature exists on the Oscillator module.

  Description:
    This function identifies whether the OscCurrentGet feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_CurrentSysClockGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the OscCurrentGet feature is supported on the device
    - false  - If the OscCurrentGet feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsOscCurrentGet( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsPBClockDivisor( OSC_MODULE_ID index )

  Summary:
    Identifies whether the PBClockDivisor feature exists on the Oscillator module.

  Description:
    This function identifies whether the PBClockDivisor feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_PBClockDivisorGet
    - PLIB_OSC_PBClockDivisorSet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the PBClockDivisor feature is supported on the device
    - false  - If the PBClockDivisor feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsPBClockDivisor( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsPBClockReady( OSC_MODULE_ID index )

  Summary:
    Identifies whether the PBClockReady feature exists on the Oscillator module.

  Description:
    This function identifies whether the PBClockReady feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_PBClockDivisorIsReady
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the PBClockReady feature is supported on the device
    - false  - If the PBClockReady feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsPBClockReady( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsUsbClockSource( OSC_MODULE_ID index )

  Summary:
    Identifies whether the UsbClockSource feature exists on the Oscillator module.

  Description:
    This function identifies whether the UsbClockSource feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_UsbClockSourceSelect
    - PLIB_OSC_UsbClockSourceGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the UsbClockSource feature is supported on the device
    - false  - If the UsbClockSource feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsUsbClockSource( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsPLLLockStatus( OSC_MODULE_ID index )

  Summary:
    Identifies whether the PLLLockStatus feature exists on the Oscillator module.

  Description:
    This function identifies whether the PLLLockStatus feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_PLLIsLocked
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the PLLLockStatus feature is supported on the device
    - false  - If the PLLLockStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsPLLLockStatus( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsPLLClockLock( OSC_MODULE_ID index )

  Summary:
    Identifies whether the PLLClockLock feature exists on the Oscillator module.

  Description:
    This function identifies whether the PLLClockLock feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_PLLClockLock
    - PLIB_OSC_PLLClockUnlock
    - PLIB_OSC_PLLClockIsLocked
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the PLLClockLock feature is supported on the device
    - false  - If the PLLClockLock feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsPLLClockLock( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsSysPLLMultiplier( OSC_MODULE_ID index )

  Summary:
    Identifies whether the PLLMultiplier feature exists on the Oscillator module.

  Description:
    This function identifies whether the PLLMultiplier feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_SysPLLMultiplierSelect
    - PLIB_OSC_SysPLLMultiplierGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the PLLMultiplier feature is supported on the device
    - false  - If the PLLMultiplier feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsSysPLLMultiplier( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsSysPLLOutputDivisor( OSC_MODULE_ID index )

  Summary:
    Identifies whether the PLLOutputDivisor feature exists on the Oscillator 
	module.

  Description:
    This function identifies whether the PLLOutputDivisor feature is available on 
	the Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_SysPLLOutputDivisorSet
    - PLIB_OSC_SysPLLOutputDivisorGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the PLLOutputDivisor feature is supported on the device
    - false  - If the PLLOutputDivisor feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsSysPLLOutputDivisor( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsClockFail( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ClockFail feature exists on the Oscillator module.

  Description:
    This function identifies whether the ClockFail feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_ClockHasFailed
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the ClockFail feature is supported on the device
    - false  - If the ClockFail feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsClockFail( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsFRCTuning( OSC_MODULE_ID index )

  Summary:
    Identifies whether the FRCTuning feature exists on the Oscillator module.

  Description:
    This function identifies whether the FRCTuning feature is available on the 
	Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_FRCTuningSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the FRCTuning feature is supported on the device
    - false  - If the FRCTuning feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsFRCTuning( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsReferenceOscBaseClock( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ReferenceOscBaseClock feature exists on the Oscillator 
	module.

  Description:
    This function identifies whether the ReferenceOscBaseClock feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_ReferenceOscBaseClockSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the ReferenceOscBaseClock feature is supported on the device
    - false  - If the ReferenceOscBaseClock feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsReferenceOscBaseClock( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsReferenceOscChange( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ReferenceOscChange feature exists on the Oscillator 
	module.

  Description:
    This function identifies whether the ReferenceOscChange feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_ReferenceOscSwitchIsComplete
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the ReferenceOscChange feature is supported on the device
    - false  - If the ReferenceOscChange feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsReferenceOscChange( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsReferenceOscChangeActive( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ReferenceOscChangeActive feature exists on the Oscillator 
	module.

  Description:
    This function identifies whether the ReferenceOscChangeActive feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_ReferenceOscSourceChangeIsActive
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the ReferenceOscChangeActive feature is supported on the 
	           device
    - false  - If the ReferenceOscChangeActive feature is not supported on 
	           the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsReferenceOscChangeActive( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsReferenceOscStopInSleep( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ReferenceOscStopInSleep feature exists on the Oscillator 
	module.

  Description:
    This function identifies whether the ReferenceOscStopInSleep feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_ReferenceOscStopInSleepEnable
    - PLIB_OSC_ReferenceOscStopInSleepDisable
    - PLIB_OSC_ReferenceOscStopInSleepIsEnabled
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the ReferenceOscStopInSleep feature is supported on the 
	           device
    - false  - If the ReferenceOscStopInSleep feature is not supported on 
	           the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsReferenceOscStopInSleep( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsReferenceOutputEnable( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ReferenceOutputEnable feature exists on the Oscillator 
	module.

  Description:
    This function identifies whether the ReferenceOutputEnable feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_ReferenceOutputEnable
    - PLIB_OSC_ReferenceOutputDisable
    - PLIB_OSC_ReferenceOutputIsEnabled
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the ReferenceOutputEnable feature is supported on the device
    - false  - If the ReferenceOutputEnable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsReferenceOutputEnable( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsReferenceOscStopInIdleEnable( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ReferenceOscStopInIdleEnable feature exists on the 
	Oscillator module.

  Description:
    This function identifies whether the ReferenceOscStopInIdleEnable feature is 
	available on the Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_ReferenceOscStopInIdleEnable
    - PLIB_OSC_ReferenceOscStopInIdleDisable
    - PLIB_OSC_ReferenceOscStopInIdleIsEnabled
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the ReferenceOscStopInIdleEnable feature is supported on the 
	           device
    - false  - If the ReferenceOscStopInIdleEnable feature is not supported on 
	           the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsReferenceOscStopInIdleEnable( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsReferenceOscEnable( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ReferenceOscEnable feature exists on the Oscillator 
	module.

  Description:
    This function identifies whether the ReferenceOscEnable feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_ReferenceOscEnable
    - PLIB_OSC_ReferenceOscDisable
    - PLIB_OSC_ReferenceOscIsEnabled
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the ReferenceOscEnable feature is supported on the device
    - false  - If the ReferenceOscEnable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsReferenceOscEnable( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsReferenceOscDivisor( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ReferenceOscDivisor feature exists on the Oscillator 
	module.

  Description:
    This function identifies whether the ReferenceOscDivisor feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_ReferenceOscDivisorValueSet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the ReferenceOscDivisor feature is supported on the device
    - false  - If the ReferenceOscDivisor feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsReferenceOscDivisor( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsReferenceOscTrim( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ReferenceOscTrim feature exists on the Oscillator 
	module.

  Description:
    This function identifies whether the ReferenceOscTrim feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_ReferenceOscTrimSet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the ReferenceOscTrim feature is supported on the device
    - false  - If the ReferenceOscTrim feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsReferenceOscTrim( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsPBClockOutputEnable( OSC_MODULE_ID index )

  Summary:
    Identifies whether the PBClockOutputEnable feature exists on the Oscillator 
	module.

  Description:
    This function identifies whether the PBClockOutputEnable feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_PBOutputClockEnable
    - PLIB_OSC_PBOutputClockDisable
    - PLIB_OSC_PBOutputClockIsEnabled
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the PBClockOutputEnable feature is supported on the device
    - false  - If the PBClockOutputEnable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsPBClockOutputEnable( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsSysPLLInputDivisor( OSC_MODULE_ID index )

  Summary:
    Identifies whether the PLLInputDivisor feature exists on the Oscillator 
	module.

  Description:
    This function identifies whether the PLLInputDivisor feature is available on 
	the Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_SysPLLInputDivisorSet
    - PLIB_OSC_SysPLLInputDivisorGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the PLLInputDivisor feature is supported on the device
    - false  - If the PLLInputDivisor feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsSysPLLInputDivisor( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsSysPLLInputClockSource( OSC_MODULE_ID index )

  Summary:
    Identifies whether the PLLInputClockSource feature exists on the Oscillator 
	module.

  Description:
    This function identifies whether the PLLInputClockSource feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_SysPLLInputClockSourceSet
    - PLIB_OSC_SysPLLInputClockSourceGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the PLLInputClockSource feature is supported on the device
    - false  - If the PLLInputClockSource feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsSysPLLInputClockSource( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsSysPLLFrequencyRange( OSC_MODULE_ID index )

  Summary:
    Identifies whether the PLLFrequencyRange feature exists on the Oscillator 
	module.

  Description:
    This function identifies whether the PLLFrequencyRange feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device:
    - PLIB_OSC_SysPLLFrequencyRangeSet
    - PLIB_OSC_SysPLLFrequencyRangeGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - If the PLLFrequencyRange feature is supported on the device
    - false  - If the PLLFrequencyRange feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsSysPLLFrequencyRange( OSC_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_OSC_ExistsSleepToStartupClock( OSC_MODULE_ID index )

  Summary:
    Identifies whether the SleepToStartupClock feature exists on the Oscillator 
	module. 

  Description:
    This function identifies whether the SleepToStartupClock feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_SleepToStartupClockSelect
    - PLIB_OSC_SleepToStartupClockGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The SleepToStartupClock feature is supported on the device
    - false  - The SleepToStartupClock feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsSleepToStartupClock( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsClockReadyStatus( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ClockReadyStatus feature exists on the Oscillator 
	module. 

  Description:
    This function identifies whether the ClockReadyStatus feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_ClockIsReady
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ClockReadyStatus feature is supported on the device
    - false  - The ClockReadyStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsClockReadyStatus( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsClockSlewingStatus( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ClockSlewingStatus feature exists on the Oscillator 
	module. 

  Description:
    This function identifies whether the ClockSlewingStatus feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_ClockSlewingIsActive
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ClockSlewingStatus feature is supported on the device
    - false  - The ClockSlewingStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsClockSlewingStatus( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsSlewEnableControl( OSC_MODULE_ID index )

  Summary:
    Identifies whether the SlewEnableControl feature exists on the Oscillator 
	module. 

  Description:
    This function identifies whether the SlewEnableControl feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_SlewEnable
    - PLIB_OSC_SlewDisable
    - PLIB_OSC_SlewIsEnabled
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The SlewEnableControl feature is supported on the device
    - false  - The SlewEnableControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsSlewEnableControl( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsSlewDivisorStepControl( OSC_MODULE_ID index )

  Summary:
    Identifies whether the SlewDivisorStepControl feature exists on the Oscillator 
	module. 

  Description:
    This function identifies whether the SlewDivisorStepControl feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_SlewDivisorStepSelect
    - PLIB_OSC_SlewDivisorStepGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The SlewDivisorStepControl feature is supported on the device
    - false  - The SlewDivisorStepControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsSlewDivisorStepControl( OSC_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_OSC_ExistsSystemClockDivisorControl( OSC_MODULE_ID index )

  Summary:
    Identifies whether the SystemClockDivisorControl feature exists on the 
	Oscillator module.

  Description:
    This function identifies whether the SystemClockDivisorControl feature is available 
	on the Oscillator module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_SystemClockDivisorSelect
    - PLIB_OSC_SystemClockDivisorGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The SystemClockDivisorControl feature is supported on the device
    - false  - The SystemClockDivisorControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsSystemClockDivisorControl( OSC_MODULE_ID index );

//******************************************************************************
/* Function :  PLIB_OSC_ExistsBTPLLInputDivisor( OSC_MODULE_ID index )

  Summary:
    Identifies whether the BTPLLInputDivisor feature exists on the OSC module 

  Description:
    This function identifies whether the BTPLLInputDivisor feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_BTPLLInputDivisorSet
    - PLIB_OSC_BTPLLInputDivisorGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BTPLLInputDivisor feature is supported on the device
    - false  - The BTPLLInputDivisor feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsBTPLLInputDivisor( OSC_MODULE_ID index );

//******************************************************************************
/* Function :  PLIB_OSC_ExistsBTPLLInputClockSource( OSC_MODULE_ID index )

  Summary:
    Identifies whether the BTPLLInputClockSource feature exists on the OSC module 

  Description:
    This function identifies whether the BTPLLInputClockSource feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_BTPLLInputClockSourceSet
    - PLIB_OSC_BTPLLInputClockSourceGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BTPLLInputClockSource feature is supported on the device
    - false  - The BTPLLInputClockSource feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsBTPLLInputClockSource( OSC_MODULE_ID index );

//******************************************************************************
/* Function :  PLIB_OSC_ExistsBTPLLFrequencyRange( OSC_MODULE_ID index )

  Summary:
    Identifies whether the BTPLLFrequencyRange feature exists on the OSC module 

  Description:
    This function identifies whether the BTPLLFrequencyRange feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_BTPLLFrequencyRangeSet
    - PLIB_OSC_BTPLLFrequencyRangeGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BTPLLFrequencyRange feature is supported on the device
    - false  - The BTPLLFrequencyRange feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsBTPLLFrequencyRange( OSC_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_OSC_ExistsUPLLFrequencyRange( OSC_MODULE_ID index )

  Summary:
    Identifies whether the UPLLFrequencyRange feature exists on the OSC module 

  Description:
    This function identifies whether the UPLLFrequencyRange feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_UPLLFrequencyRangeSet
    - PLIB_OSC_UPLLFrequencyRangeGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The UPLLFrequencyRange feature is supported on the device
    - false  - The UPLLFrequencyRange feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsUPLLFrequencyRange( OSC_MODULE_ID index );

//******************************************************************************
/* Function :  PLIB_OSC_ExistsDreamModeControl( OSC_MODULE_ID index )

  Summary:
    Identifies whether the DreamModeControl feature exists on the OSC module 

  Description:
    This function identifies whether the DreamModeControl feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_DreamModeEnable
    - PLIB_OSC_DreamModeDisable
    - PLIB_OSC_DreamModeStatus
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The DreamModeControl feature is supported on the device
    - false  - The DreamModeControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsDreamModeControl( OSC_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_OSC_ExistsForceLock( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ForceLock feature exists on the OSC module 

  Description:
    This function identifies whether the ForceLock feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_ForceSPLLLockEnable
    - PLIB_OSC_ForceSPLLLockDisable
    - PLIB_OSC_ForceSPLLLockStatus
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ForceLock feature is supported on the device
    - false  - The ForceLock feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsForceLock( OSC_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_OSC_ExistsResetPLL( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ResetPLL feature exists on the OSC module 

  Description:
    This function identifies whether the ResetPLL feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_ResetPLLAssert
    - PLIB_OSC_ResetPLLDeassert
    - PLIB_OSC_ResetPLLStatus
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ResetPLL feature is supported on the device
    - false  - The ResetPLL feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsResetPLL( OSC_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_OSC_ExistsPLLBypass( OSC_MODULE_ID index )

  Summary:
    Identifies whether the SPLLBypass feature exists on the OSC module 

  Description:
    This function identifies whether the PLLBypass feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_PLLBypassEnable
    - PLIB_OSC_PLLBypassDisable
    - PLIB_OSC_PLLBypassStatus
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The PLLBypass feature is supported on the device
    - false  - The PLLBypass feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsPLLBypass( OSC_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_OSC_ExistsBTPLLClockOut( OSC_MODULE_ID index )

  Summary:
    Identifies whether the BTPLLClockOut feature exists on the OSC module 

  Description:
    This function identifies whether the BTPLLClockOut feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_BTPLLClockOutEnable
    - PLIB_OSC_BTPLLClockOutDisable
    - PLIB_OSC_BTPLLClockOutStatus
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BTPLLClockOut feature is supported on the device
    - false  - The BTPLLClockOut feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsBTPLLClockOut( OSC_MODULE_ID index );

//******************************************************************************
/* Function :  PLIB_OSC_ExistsUPLLMultiplier( OSC_MODULE_ID index )

  Summary:
    Identifies whether the UPLLMultiplier feature exists on the OSC module 

  Description:
    This function identifies whether the UPLLMultiplier feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_UPLLMultiplierSelect
    - PLIB_OSC_UPLLMultiplierGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The UPLLMultiplier feature is supported on the device
    - false  - The UPLLMultiplier feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsUPLLMultiplier( OSC_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_OSC_ExistsBTPLLMultiplier( OSC_MODULE_ID index )

  Summary:
    Identifies whether the BTPLLMultiplier feature exists on the OSC module 

  Description:
    This function identifies whether the BTPLLMultiplier feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_BTPLLMultiplierSelect
    - PLIB_OSC_BTPLLMultiplierGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BTPLLMultiplier feature is supported on the device
    - false  - The BTPLLMultiplier feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsBTPLLMultiplier( OSC_MODULE_ID index );

//******************************************************************************
/* Function :  PLIB_OSC_ExistsUPLLOutputDivisor( OSC_MODULE_ID index )

  Summary:
    Identifies whether the UPLLOutputDivisor feature exists on the OSC module 

  Description:
    This function identifies whether the UPLLOutputDivisor feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_UPLLOutputDivisorSet
    - PLIB_OSC_UPLLOutputDivisorGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The UPLLOutputDivisor feature is supported on the device
    - false  - The UPLLOutputDivisor feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsUPLLOutputDivisor( OSC_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_OSC_ExistsBTPLLOutputDivisor( OSC_MODULE_ID index )

  Summary:
    Identifies whether the BTPLLOutputDivisor feature exists on the OSC module 

  Description:
    This function identifies whether the BTPLLOutputDivisor feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_BTPLLOutputDivisorSet
    - PLIB_OSC_BTPLLOutputDivisorGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BTPLLOutputDivisor feature is supported on the device
    - false  - The BTPLLOutputDivisor feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsBTPLLOutputDivisor( OSC_MODULE_ID index );

//******************************************************************************
/* Function :  PLIB_OSC_ExistsUPLLInputDivisor( OSC_MODULE_ID index )

  Summary:
    Identifies whether the UPLLInputDivisor feature exists on the OSC module 

  Description:
    This function identifies whether the UPLLInputDivisor feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_UPLLInputDivisorSet
    - PLIB_OSC_UPLLInputDivisorGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The UPLLInputDivisor feature is supported on the device
    - false  - The UPLLInputDivisor feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsUPLLInputDivisor( OSC_MODULE_ID index );

//******************************************************************************
/* Function :  PLIB_OSC_ExistsClockDiagStatus( OSC_MODULE_ID index )

  Summary:
    Identifies whether the ClockDiagStatus feature exists on the OSC module 

  Description:
    This function identifies whether the ClockDiagStatus feature is available on the OSC module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_OSC_ClockStop
    - PLIB_OSC_ClockStart
    - PLIB_OSC_ClockStopStatus
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ClockDiagStatus feature is supported on the device
    - false  - The ClockDiagStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_OSC_ExistsClockDiagStatus( OSC_MODULE_ID index );


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // _PLIB_OSC_H
/*******************************************************************************
 End of File
*/