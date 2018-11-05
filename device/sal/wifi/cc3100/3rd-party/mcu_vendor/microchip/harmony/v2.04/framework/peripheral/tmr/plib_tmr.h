/*******************************************************************************
  Timer Peripheral Library Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    plib_tmr.h

  Summary:
    Timer Peripheral Library interface header.

  Description:
    This header file contains the function prototypes and definitions of
    the data types and constants that make up the interface to the Timer
    Peripheral Library.
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

#ifndef _PLIB_TMR_H
#define _PLIB_TMR_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END 

// *****************************************************************************
// *****************************************************************************
// Section: Included Files (continued at end of file)
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.  However,
    please see the end of the file for additional implementation header files
    that are also included
*/

#include "peripheral/tmr/processor/tmr_processor.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: TMR Peripheral Library Interface Routines
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Function:
    void PLIB_TMR_GateEnable(TMR_MODULE_ID index)

  Summary:
    Enables counting controlled by the corresponding gate function.

  Description:
    This function enables counting controlled by the gate function.
    This operation is atomic.

  Precondition:
    The timer must be enabled, for this function to take effect.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_GateEnable(MY_TMR_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsGatedTimeAccumulation in your application to determine whether
	this feature is available.
*/

void PLIB_TMR_GateEnable(TMR_MODULE_ID index);


// *****************************************************************************
/* Function:
    void PLIB_TMR_GateDisable(TMR_MODULE_ID index)

  Summary:
    Enables counting regardless of the corresponding timer gate function.

  Description:
    This function enables counting regardless of the gate function.
    This operation is atomic.

  Precondition:
    The timer must be enabled for this function to take effect.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_GateDisable(MY_TMR_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsGatedTimeAccumulation in your application to determine whether
	this feature is available.
*/
void PLIB_TMR_GateDisable(TMR_MODULE_ID index);

// *****************************************************************************
/* Function:
    void PLIB_TMR_ClockSourceSelect(TMR_MODULE_ID index, TMR_CLOCK_SOURCE source)

  Summary:
    Selects the clock source.

  Description:
    This function selects the clock source.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    source          - One of the possible values of TMR_CLOCK_SOURCE

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_ClockSourceSelect(MY_TMR_INSTANCE, TMR_CLOCK_SOURCE_PERIPHERAL_CLOCK);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsClockSource in your application to determine whether
	this feature is available.
*/

void PLIB_TMR_ClockSourceSelect(TMR_MODULE_ID index, TMR_CLOCK_SOURCE source);

// *****************************************************************************
/* Function:
    void PLIB_TMR_PrescaleSelect(TMR_MODULE_ID index, TMR_PRESCALE prescale)

  Summary:
    Selects the clock prescaler.

  Description:
    This function selects the clock prescaler.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    prescale        - One of the possible values of TMR_PRESCALE

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_PrescaleSelect(MY_TMR_INSTANCE, TMR_PRESCALE_VALUE_8);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsPrescale in your application to determine whether
	this feature is available.
*/

void PLIB_TMR_PrescaleSelect ( TMR_MODULE_ID index, TMR_PRESCALE prescale );


// *****************************************************************************
/* Function:
    uint16_t PLIB_TMR_PrescaleGet ( TMR_MODULE_ID index )

  Summary:
    Gets the prescaler divisor value.

  Description:
    This function returns the prescaler divisor value. The value returned will
    be direct number and not the enum equivalent.

  Precondition:
    None.

  Parameters:
    index  - Identifier for the device instance to be configured

  Returns:
    Prescaler divisor value.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    uint16_t prescale;
    prescale = PLIB_TMR_PrescaleGet (MY_TMR_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsPrescale in your application to determine whether
	this feature is available.
*/

uint16_t PLIB_TMR_PrescaleGet ( TMR_MODULE_ID index );


// *****************************************************************************
/* Function:
    uint16_t PLIB_TMR_PrescaleDivisorGet(TMR_MODULE_ID index, TMR_PRESCALE prescale)

  Summary:
    Gets the prescaler divisor information.

  Description:
    This function returns the prescaler divisor information.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    prescale        - One of the possible values of TMR_PRESCALE

  Returns:
    prescale divisor value

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    uint16_t div = PLIB_TMR_PrescaleDivisorGet (MY_TMR_INSTANCE, TMR_PRESCALE_VALUE_1);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsPrescale in your application to determine whether
	this feature is available.
*/

uint16_t PLIB_TMR_PrescaleDivisorGet(TMR_MODULE_ID index, TMR_PRESCALE prescale);


// *****************************************************************************
/* Function:
    void PLIB_TMR_ClockSourceExternalSyncEnable(TMR_MODULE_ID index)

  Summary:
    Enables the clock synchronization of the external input.

  Description:
    This function enables the clock synchronization of the external input.
    This operation is atomic.

  Precondition:
    The timer module must be configured to use the external clock using the
    function PLIB_TMR_ClockSourceSelect with the clock source as
    TMR_CLOCK_SOURCE_EXTERNAL_INPUT_PIN.
    If the function PLIB_TMR_ClockSourceSelect configures the clock with some
    other enumeration, this function will have no effect.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_ClockSourceExternalSyncEnable(MY_TMR_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsClockSourceSync in your application to determine whether
	this feature is available.
*/

void PLIB_TMR_ClockSourceExternalSyncEnable(TMR_MODULE_ID index);


// *****************************************************************************
/* Function:
    void PLIB_TMR_ClockSourceExternalSyncDisable(TMR_MODULE_ID index)

  Summary:
    Disables the clock synchronization of the external input.

  Description:
    This function disables the clock synchronization of the external input.
    This operation is atomic.

  Precondition:
    The timer module must be configured to use the external clock using the
    function  PLIB_TMR_ClockSourceSelect with the clock source as
    TMR_CLOCK_SOURCE_EXTERNAL_INPUT_PIN.
    If the function PLIB_TMR_ClockSourceSelect configures the clock with some
    other enumeration, this function will have no effect.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_ClockSourceExternalSyncDisable(MY_TMR_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsClockSourceSync in your application to determine whether
	this feature is available.
*/

void PLIB_TMR_ClockSourceExternalSyncDisable(TMR_MODULE_ID index);

// *****************************************************************************
/* Function:
    void PLIB_TMR_Mode16BitEnable(TMR_MODULE_ID index)

  Summary:
    Enables the Timer module for 16-bit operation and disables all other modes.

  Description:
    This function enables the Timer module for 16-bit operation and disables all
    other modes.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_Mode16BitEnable(MY_TMR_INSTANCE);
    </code>

  Remarks:
    Calling this function disables the operation of the Timer module 8-bit mode.

    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_TMR_ExistsMode16Bit in your application to determine whether
    this feature is available.

*/

void PLIB_TMR_Mode16BitEnable(TMR_MODULE_ID index);


// *****************************************************************************
/* Function:
    void PLIB_TMR_Mode32BitEnable(TMR_MODULE_ID index)

  Summary:
    Enables 32-bit operation on the Timer module combination.

  Description:
    This function enables the Timer module and the index +1 Timer module to act
    as one 32-bit timer module and disables all other modes.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_Mode32BitEnable(MY_TMR_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_TMR_ExistsMode32Bit in your application to determine whether
    this feature is available.

*/

void PLIB_TMR_Mode32BitEnable(TMR_MODULE_ID index);


// *****************************************************************************
/* Function:
    void PLIB_TMR_Start(TMR_MODULE_ID index)

  Summary:
    Starts/enables the indexed timer.

  Description:
    This function starts/enables the indexed timer.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_Start(MY_TMR_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsEnableControl in your application to determine whether
	this feature is available.
*/

void PLIB_TMR_Start(TMR_MODULE_ID index);


// *****************************************************************************
/* Function:
    void PLIB_TMR_Stop(TMR_MODULE_ID index)

  Summary:
    Stops/disables the indexed timer.

  Description:
    This function stops/disables the indexed timer.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_Stop(MY_TMR_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsEnableControl in your application to determine whether
	this feature is available.
*/

void PLIB_TMR_Stop(TMR_MODULE_ID index);

// *****************************************************************************
/* Function:
    void PLIB_TMR_StopInIdleEnable(TMR_MODULE_ID index)

  Summary:
    Discontinues module operation when the device enters Idle mode.

  Description:
    This function discontinues module operation when the device enters Idle mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_StopInIdleEnable(MY_TMR_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsStopInIdleControl in your application to determine whether
	this feature is available.
*/

void PLIB_TMR_StopInIdleEnable(TMR_MODULE_ID index);


// *****************************************************************************
/* Function:
    void PLIB_TMR_StopInIdleDisable(TMR_MODULE_ID index)

  Summary:
    Continue module operation when the device enters Idle mode.

  Description:
    This function continues module operation when the device enters Idle mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_StopInIdleDisable(MY_TMR_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsStopInIdleControl in your application to determine whether
	this feature is available.
*/

void PLIB_TMR_StopInIdleDisable(TMR_MODULE_ID index);

// *****************************************************************************
/* Function:
    void PLIB_TMR_CounterAsyncWriteEnable(TMR_MODULE_ID index)

  Summary:
    Enables back-to-back writes with legacy asynchronous timer functionality.

  Description:
    This function enables back-to-back writes with legacy asynchronous timer
    functionality.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_CounterAsyncWriteEnable(MY_TMR_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsCounterAsyncWriteControl in your application to determine
	whether this feature is available.
*/

void PLIB_TMR_CounterAsyncWriteEnable(TMR_MODULE_ID index);


// *****************************************************************************
/* Function:
    void PLIB_TMR_CounterAsyncWriteDisable(TMR_MODULE_ID index)

  Summary:
    Disables the writes to the counter register until the pending write operation
    completes.

  Description:
    This function disables the writes to the counter register until the pending
    write operation completes.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_CounterAsyncWriteDisable(MY_TMR_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsCounterAsyncWriteControl in your application to determine
	whether this feature is available.
*/

void PLIB_TMR_CounterAsyncWriteDisable(TMR_MODULE_ID index);


// *****************************************************************************
/* Function:
    bool PLIB_TMR_CounterAsyncWriteInProgress(TMR_MODULE_ID index)

  Summary:
    Returns the status of the counter write in Asynchronous mode.

  Description:
    This function returns the status of the counter write in Asynchronous mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true  - Write is in progress
    - false - Write is complete

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    bool inProgress = PLIB_TMR_CounterAsyncWriteInProgress(MY_TMR_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsCounterAsyncWriteInProgress in your application to determine
	whether this feature is available.
*/

bool PLIB_TMR_CounterAsyncWriteInProgress(TMR_MODULE_ID index);

// *****************************************************************************
/* Function:
    void PLIB_TMR_Counter16BitSet(TMR_MODULE_ID index, uint16_t value)

  Summary:
    Sets the 16-bit timer value.

  Description:
    This function sets the 16-bit timer value.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    value           - 16-bit value to be set

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_Counter16BitSet(MY_TMR_INSTANCE, 0x100);
    </code>

  Remarks:
    When the timer register is written to, the timer increment does not occur
    for that cycle. Writes to the timer register in the asynchronous counter
    mode should be avoided.

    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_TMR_ExistsCounter16Bit in your application to determine whether this
    feature is available.
*/

void PLIB_TMR_Counter16BitSet(TMR_MODULE_ID index, uint16_t value);


// *****************************************************************************
/* Function:
    uint16_t PLIB_TMR_Counter16BitGet(TMR_MODULE_ID index)

  Summary:
    Gets the 16-bit timer value.

  Description:
    This function gets the 16-bit timer value.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    16-bit timer value.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    uint16_t timerValue = PLIB_TMR_Counter16BitGet(MY_TMR_INSTANCE);
    </code>

  Remarks:
    PLIB_TMR_Counter16BitGet does not prevent the timer from incrementing
    during the same instruction cycle.

    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_TMR_ExistsCounter16Bit in your application to determine whether this
    feature is available.
*/

uint16_t PLIB_TMR_Counter16BitGet(TMR_MODULE_ID index);


// *****************************************************************************
/* Function:
    uint8_t PLIB_TMR_Counter16BitClear(TMR_MODULE_ID index)

  Summary:
    Clears the 16-bit timer value.

  Description:
    This function clears the 16-bit timer value.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_Counter16BitClear(MY_TMR_INSTANCE);
    </code>

  Remarks:
    When the timer register is written, the timer increment does not occur
    for that cycle. Writes to the timer register in the asynchronous counter
    mode should be avoided.

    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_TMR_ExistsCounter16Bit in your application to determine whether this
    feature is available.
*/

void PLIB_TMR_Counter16BitClear(TMR_MODULE_ID index);


// *****************************************************************************
/* Function:
    void PLIB_TMR_Counter32BitSet(TMR_MODULE_ID index, uint32_t value)

  Summary:
    Sets the 32-bit timer value.

  Description:
    This function sets the 32-bit timer value.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    value           - 32-bit value to be set

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_Counter32BitSet(MY_TMR_INSTANCE, 0x1000000);
    </code>

  Remarks:
    When the timer register is written, the timer increment does not occur
    for that cycle. Writes to the timer register in the asynchronous counter
    mode should be avoided.

    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_TMR_ExistsCounter32Bit in your application to determine whether this
    feature is available.
*/

void PLIB_TMR_Counter32BitSet(TMR_MODULE_ID index, uint32_t value);


// *****************************************************************************
/* Function:
    uint32_t PLIB_TMR_Counter32BitGet(TMR_MODULE_ID index)

  Summary:
    Gets the 32-bit timer value.

  Description:
    This function gets the 32-bit timer value.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    32 bit timer value.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    uint32_t timerValue = PLIB_TMR_Counter32BitGet(MY_TMR_INSTANCE);
    </code>

  Remarks:
    PLIB_TMR_Counter32BitGet does not prevent the timer from incrementing
    during the same instruction cycle.

    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_TMR_ExistsCounter32Bit in your application to determine whether this
    feature is available.
*/

uint32_t PLIB_TMR_Counter32BitGet(TMR_MODULE_ID index);


// *****************************************************************************
/* Function:
    uint8_t PLIB_TMR_Counter32BitClear(TMR_MODULE_ID index)

  Summary:
    Clears the 32-bit timer value.

  Description:
    This function clears the 32-bit timer value.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    PLIB_TMR_Counter32BitClear(MY_TMR_INSTANCE);
    </code>

  Remarks:
    When the timer register is written, the timer increment does not occur
    for that cycle. Writes to the timer register in the asynchronous counter
    mode should be avoided.

    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_TMR_ExistsCounter32Bit in your application to determine whether this
    feature is available.
*/

void PLIB_TMR_Counter32BitClear(TMR_MODULE_ID index);

// *****************************************************************************
/* Function:
    void PLIB_TMR_Period16BitSet(TMR_MODULE_ID index, uint16_t period)

  Summary:
    Sets the 16-bit period value.

  Description:
    This function sets the 16-bit period value.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    period          - 16-bit period register value

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    // where, MY_PERIOD_VALUE is the 16-bit value which needs to be stored in the
    // period register.
    PLIB_TMR_Period16BitSet(MY_TMR_INSTANCE, MY_PERIOD_VALUE);
    </code>

  Remarks:
    The timer period register may be written at any time before the timer is
    started or after the timer is stopped. The timer period register can also be
    written when servicing the interrupt for the timer. When the timer is in
    operation, it is not recommended to write to the period register.

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsPeriod16Bit in your application to determine whether this
	feature is available.
*/

void PLIB_TMR_Period16BitSet(TMR_MODULE_ID index, uint16_t period);


// *****************************************************************************
/* Function:
    uint16_t PLIB_TMR_Period16BitGet(TMR_MODULE_ID index)

  Summary:
    Gets the 16-bit period value.

  Description:
    This function gets the 16-bit period value.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    period          - 16-bit period value

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    uint16_t period = PLIB_TMR_Period16BitGet(MY_TMR_INSTANCE);
    </code>

  Remarks:
    The timer period register may be written at any time before the timer is
    started or after the timer is stopped. The timer period register can also be
    written when servicing the interrupt for the timer. When the timer is in
    operation, it is not recommended to write to the period register.

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsPeriod16Bit in your application to determine whether this
	feature is available.
*/

uint16_t PLIB_TMR_Period16BitGet(TMR_MODULE_ID index);


// *****************************************************************************
/* Function:
    void PLIB_TMR_Period32BitSet(TMR_MODULE_ID index, uint32_t period)

  Summary:
    Sets the 32-bit period value.

  Description:
    This function sets the 32-bit period value.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    period          - 32-bit period register value

  Returns:
    None.

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    // where, MY_PERIOD_VALUE is the 32-bit value which needs to be stored in the
    // period register.
    PLIB_TMR_Period32BitSet(MY_TMR_INSTANCE, MY_PERIOD_VALUE);
    </code>

  Remarks:
    The timer period register may be written at any time before the timer is
    started or after the timer is stopped. The timer period register can also be
    written when servicing the interrupt for the timer. When the timer is in
    operation, it is not recommended to write to the period register.

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsPeriod32Bit in your application to determine whether this
	feature is available.
*/

void PLIB_TMR_Period32BitSet(TMR_MODULE_ID index, uint32_t period);


// *****************************************************************************
/* Function:
    uint32_t PLIB_TMR_Period32BitGet(TMR_MODULE_ID index)

  Summary:
    Gets the 32-bit period value.

  Description:
    This function gets the 32-bit period value.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    period          - 32-bit period value

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    uint32_t period = PLIB_TMR_Period32BitGet(MY_TMR_INSTANCE);
    </code>

  Remarks:
    The timer period register may be written at any time before the timer is
    started or after the timer is stopped. The timer period register can also be
    written when servicing the interrupt for the timer. When the timer is in
    operation, it is not recommended to write to the period register.

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsPeriod32Bit in your application to determine whether this
	feature is available.
*/

uint32_t PLIB_TMR_Period32BitGet(TMR_MODULE_ID index);


// *****************************************************************************
/* Function:
    bool PLIB_TMR_IsPeriodMatchBased (TMR_MODULE_ID index)

  Summary:
    Gets the operating mode state of the Timer module based on Period Match or
    Overflow mode.

  Description:
    This function gets the operating mode state of the Timer module based on
    Period Match or Overflow mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true            - Operation in Period Match mode
    - false           - Operation in Overflow mode

  Example:
    <code>
    // Where MY_TMR_INSTANCE, is the timer instance selected for use by the
    // application developer.
    bool status = PLIB_TMR_IsPeriodMatchBased(MY_TMR_INSTANCE);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_TMR_ExistsTimerOperationMode in your application to determine whether
	this feature is available.
*/

bool PLIB_TMR_IsPeriodMatchBased (TMR_MODULE_ID index);


// *****************************************************************************
// *****************************************************************************
// Section: TMR Peripheral Library Exists Functions
// *****************************************************************************
// *****************************************************************************
/* The following functions indicate the existence of the features on the device.
*/

//******************************************************************************
/* Function:
    PLIB_TMR_ExistsGatedTimeAccumulation( TMR_MODULE_ID index )

  Summary:
    Identifies whether the GatedTimeAccumulation feature exists on the Timer module.

  Description:
    This function identifies whether the GatedTimeAccumulation feature is available
    on the Timer module. When this function returns true, these functions are
    supported on the device:
    - PLIB_TMR_GateEnable
    - PLIB_TMR_GateDisable
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The GatedTimeAccumulation feature is supported on the device
    - false  - The GatedTimeAccumulation feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsGatedTimeAccumulation( TMR_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_TMR_ExistsClockSource( TMR_MODULE_ID index )

  Summary:
    Identifies whether the ClockSource feature exists on the Timer module.

  Description:
    This function identifies whether the ClockSource feature is available
    on the Timer module. When this function returns true, this function is
    supported on the device:
    - PLIB_TMR_ClockSourceSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ClockSource feature is supported on the device
    - false  - The ClockSource feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsClockSource( TMR_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_TMR_ExistsPrescale( TMR_MODULE_ID index )

  Summary:
    Identifies whether the Prescale feature exists on the Timer module.

  Description:
    This function identifies whether the Prescale feature is available
    on the Timer module. When this function returns true, these functions are
    supported on the device:
    - PLIB_TMR_PrescaleSelect
    - PLIB_TMR_PrescaleGet
    - PLIB_TMR_PrescaleDivisorGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Prescale feature is supported on the device
    - false  - The Prescale feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsPrescale( TMR_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_TMR_ExistsClockSourceSync( TMR_MODULE_ID index )

  Summary:
    Identifies whether the ClockSourceSync feature exists on the Timer module.

  Description:
    This function identifies whether the ClockSourceSync feature is available
    on the Timer module. When this function returns true, these functions are
    supported on the device:
    - PLIB_TMR_ClockSourceExternalSyncEnable
    - PLIB_TMR_ClockSourceExternalSyncDisable
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ClockSourceSync feature is supported on the device
    - false  - The ClockSourceSync feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsClockSourceSync( TMR_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_TMR_ExistsMode16Bit( TMR_MODULE_ID index )

  Summary:
    Identifies whether the Mode16Bit feature exists on the Timer module.

  Description:
    This function identifies whether the Mode16Bit feature is available
    on the Timer module. When this function returns true, this function is
    supported on the device:
    - PLIB_TMR_Mode16BitEnable
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Mode16Bit feature is supported on the device
    - false  - The Mode16Bit feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsMode16Bit( TMR_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_TMR_ExistsMode32Bit( TMR_MODULE_ID index )

  Summary:
    Identifies whether the Mode32Bit feature exists on the Timer module.

  Description:
    This function identifies whether the Mode32Bit feature is available
    on the Timer module. When this function returns true, this function is
    supported on the device:
    - PLIB_TMR_Mode32BitEnable
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Mode32Bit feature is supported on the device
    - false  - The Mode32Bit feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsMode32Bit( TMR_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_TMR_ExistsEnableControl( TMR_MODULE_ID index )

  Summary:
    Identifies whether the EnableControl feature exists on the Timer module.

  Description:
    This function identifies whether the EnableControl feature is available
    on the Timer module. When this function returns true, these functions are
    supported on the device:
    - PLIB_TMR_Start
    - PLIB_TMR_Stop
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The EnableControl feature is supported on the device
    - false  - The EnableControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsEnableControl( TMR_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_TMR_ExistsStopInIdleControl( TMR_MODULE_ID index )

  Summary:
    Identifies whether the StopInIdle feature exists on the Timer module.

  Description:
    This function identifies whether the StopInIdle feature is available
    on the Timer module. When this function returns true, these functions are
    supported on the device:
    - PLIB_TMR_StopInIdleEnable
    - PLIB_TMR_StopInIdleDisable
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The StopInIdle feature is supported on the device
    - false  - The StopInIdle feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsStopInIdleControl( TMR_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_TMR_ExistsCounter16Bit( TMR_MODULE_ID index )

  Summary:
    Identifies whether the Counter16Bit feature exists on the Timer module.

  Description:
    This function identifies whether the Counter16Bit feature is available
    on the Timer module. When this function returns true, these functions are
    supported on the device:
    - PLIB_TMR_Counter16BitSet
    - PLIB_TMR_Counter16BitGet
    - PLIB_TMR_Counter16BitClear
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Counter16Bit feature is supported on the device
    - false  - The Counter16Bit feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsCounter16Bit( TMR_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_TMR_ExistsCounter32Bit( TMR_MODULE_ID index )

  Summary:
    Identifies whether the Counter32Bit feature exists on the Timer module.

  Description:
    This function identifies whether the Counter32Bit feature is available
    on the Timer module. When this function returns true, these functions are
    supported on the device:
    - PLIB_TMR_Counter32BitSet
    - PLIB_TMR_Counter32BitGet
    - PLIB_TMR_Counter32BitClear
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Counter32Bit feature is supported on the device
    - false  - The Counter32Bit feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsCounter32Bit( TMR_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_TMR_ExistsPeriod16Bit( TMR_MODULE_ID index )

  Summary:
    Identifies whether the Period16Bit feature exists on the Timer module.

  Description:
    This function identifies whether the Period16Bit feature is available
    on the Timer module. When this function returns true, these functions are
    supported on the device:
    - PLIB_TMR_Period16BitSet
    - PLIB_TMR_Period16BitGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Period16Bit feature is supported on the device
    - false  - The Period16Bit feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsPeriod16Bit( TMR_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_TMR_ExistsPeriod32Bit( TMR_MODULE_ID index )

  Summary:
    Identifies whether the Period32Bit feature exists on the Timer module.

  Description:
    This function identifies whether the Period32Bit feature is available
    on the Timer module. When this function returns true, these functions are
    supported on the device:
    - PLIB_TMR_Period32BitSet
    - PLIB_TMR_Period32BitGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Period32Bit feature is supported on the device
    - false  - The Period32Bit feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsPeriod32Bit( TMR_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_TMR_ExistsCounterAsyncWriteControl( TMR_MODULE_ID index )

  Summary:
    Identifies whether the CounterAsyncWriteControl feature exists on the Timer module.

  Description:
    This function identifies whether the CounterAsyncWriteControl feature is available
    on the Timer module. When this function returns true, these functions are
    supported on the device:
    - PLIB_TMR_CounterAsyncWriteEnable
    - PLIB_TMR_CounterAsyncWriteDisable
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CounterAsyncWriteControl feature is supported on the device
    - false  - The CounterAsyncWriteControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsCounterAsyncWriteControl( TMR_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_TMR_ExistsCounterAsyncWriteInProgress( TMR_MODULE_ID index )

  Summary:
    Identifies whether the CounterAsyncWriteInProgress feature exists on the Timer module.

  Description:
    This function identifies whether the CounterAsyncWriteInProgress feature is
    available on the Timer module. When this function returns true, this function
    is supported on the device:
    - PLIB_TMR_CounterAsyncWriteInProgress
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CounterAsyncWriteInProgress feature is supported on the device
    - false  - The CounterAsyncWriteInProgress feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsCounterAsyncWriteInProgress( TMR_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_TMR_ExistsTimerOperationMode( TMR_MODULE_ID index )

  Summary:
    Identifies whether the TimerOperationMode feature exists on the Timer module.

  Description:
    This function identifies whether the TimerOperationMode feature is
    available on the Timer module. When this function returns true, this function
    is supported on the device:
    - PLIB_TMR_IsPeriodMatchBased
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The TimerOperationMode feature is supported on the device
    - false  - The TimerOperationMode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_TMR_ExistsTimerOperationMode( TMR_MODULE_ID index );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _PLIB_TMR_H
/*******************************************************************************
 End of File
*/
