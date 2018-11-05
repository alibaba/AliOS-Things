/*******************************************************************************
  USART Peripheral Library Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    plib_usart.h

  Summary:
    USART Peripheral Library interface header.

  Description:
    This header file contains the function prototypes and definitions of
    the data types and constants that make up the interface to the USART
    Peripheral Library for all families of Microchip microcontrollers. The
    functions in this file are common to the USART module.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2010-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _PLIB_USART_H
#define _PLIB_USART_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END 

// ****************************************************************************
// ****************************************************************************
// Section: Included Files
// ****************************************************************************
// ****************************************************************************
/*  This section lists the other files that are included in this file.
*/

#include "peripheral/usart/processor/usart_processor.h"

// ****************************************************************************
// ****************************************************************************
// Section: USART Peripheral Library Interface Functions
// ****************************************************************************
// ****************************************************************************

//******************************************************************************
/* Function:
    void PLIB_USART_RunInOverflowEnable ( USART_MODULE_ID index )

  Summary:
    Enables the USART module to continue to operate when an overflow error
    condition has occurred.

  Description:
    This function enables the USART module to continue to operate when an
    overflow error condition has occurred.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_RunInOverflowEnable (MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsRunInOverflow in your application to determine
    whether this feature is available.
*/

void PLIB_USART_RunInOverflowEnable ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_USART_RunInOverflowDisable ( USART_MODULE_ID index )

  Summary:
    Disables the Run in overflow condition mode.

  Description:
    This function disables the USART module from accepting new data when an overflow 
    error condition is detected.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_RunInOverflowDisable (MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsRunInOverflow in your application to determine
    whether this feature is available.
*/

void PLIB_USART_RunInOverflowDisable ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    bool PLIB_USART_RunInOverflowIsEnabled ( USART_MODULE_ID index )

  Summary:
    Gets the status of the Run in Overflow condition.

  Description:
    This function indicates if the USART module has been enabled to run in
    an overflow condition.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true          - Run in overflow condition is enabled
    - false         - Run in overflow condition is disabled

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    bool status;
    status = PLIB_USART_RunInOverflowIsEnabled (MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsRunInOverflow in your application to determine
    whether this feature is available.
*/

bool PLIB_USART_RunInOverflowIsEnabled ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_USART_BRGClockSourceSelect
    (
        USART_MODULE_ID index,
        USART_BRG_CLOCK_SOURCE brgClockSource
    )

  Summary:
    Configures the BRG clock source of the USART module.

  Description:
    This function configures the BRG Clock source of the USART. Refer to 
	USART_BRG_CLOCK_SOURCE for the possible clock sources.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    brgClockSource  - One of the possible values from USART_BRG_CLOCK_SOURCE

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_BRGClockSourceSelect (MY_USART_INSTANCE, USART_BRG_CLOCK_SOURCE_FRC_IN_SLEEP);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsBRGClockSourceSelect in your application to determine
    whether this feature is available.
*/

void PLIB_USART_BRGClockSourceSelect 
(
    USART_MODULE_ID index,
    USART_BRG_CLOCK_SOURCE brgClockSource
);

//******************************************************************************
/* Function:
    USART_BRG_CLOCK_SOURCE PLIB_USART_BRGClockSourceGet ( USART_MODULE_ID index )

  Summary:
    Gets the BRG clock source of the USART module.

  Description:
    This function returns the BRG Clock source of the USART. Refer to
    USART_BRG_CLOCK_SOURCE for the possible clock sources.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    One of the possible values of USART_BRG_CLOCK_SOURCE

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    USART_BRG_CLOCK_SOURCE brgClockSource;
    brgClockSource = PLIB_USART_BRGClockSourceGet (MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsBRGClockSourceSelect in your application to determine
    whether this feature is available.
*/

USART_BRG_CLOCK_SOURCE PLIB_USART_BRGClockSourceGet ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    bool PLIB_USART_ModuleIsBusy ( USART_MODULE_ID index )

  Summary:
    Returns the USART module's running status.

  Description:
    This function checks if the USART module status is busy. The following
    functions should not be used when the module status is busy:
    - PLIB_USART_LineControlModeSelect
    - PLIB_USART_BaudRateHighSet
    - PLIB_USART_BaudRateHighEnable
    - PLIB_USART_BaudRateHighDisable
    - PLIB_USART_ReceiverIdleStateLowEnable
    - PLIB_USART_ReceiverIdleStateLowDisable
    - PLIB_USART_BaudRateAutoDetectEnable
    - PLIB_USART_LoopbackEnable
    - PLIB_USART_LoopbackDisable
    - PLIB_USART_WakeOnStartEnable
    - PLIB_USART_WakeOnStartDisable
    - PLIB_USART_OperationModeSelect
    - PLIB_USART_HandshakeModeSelect
    - PLIB_USART_IrDAEnable
    - PLIB_USART_IrDADisable
    - PLIB_USART_StopInIdleEnable
    - PLIB_USART_StopInIdleDisable
    - PLIB_USART_RunInOverflowEnable
    - PLIB_USART_RunInOverflowDisable
    - PLIB_USART_BRGClockSourceSelect
    - PLIB_USART_RunInSleepModeEnable
    - PLIB_USART_RunInSleepModeDisable

    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true          - USART module is busy
    - false         - USART module is idle

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1
    bool moduleStatus;

    moduleStatus = PLIB_USART_ModuleIsBusy (MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsModuleBusyStatus in your application to determine
    whether this feature is available.
*/
bool PLIB_USART_ModuleIsBusy ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_USART_RunInSleepModeEnable ( USART_MODULE_ID index )

  Summary:
    Allows the USART module's BRG clock to run when the device enters Sleep mode.

  Description:
    This function enables the USART module's BRG clock to continue operation when
    the device enters the Sleep mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_RunInSleepModeEnable (MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsRunInSleepMode in your application to determine
    whether this feature is available.
*/

void PLIB_USART_RunInSleepModeEnable ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_USART_RunInSleepModeDisable ( USART_MODULE_ID index )

  Summary:
    Turns off the USART module's BRG clock during Sleep mode.

  Description:
    This function turns off the USART module's BRG clock during Sleep mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_RunInSleepModeDisable (MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsRunInSleepMode in your application to determine
    whether this feature is available.
*/

void PLIB_USART_RunInSleepModeDisable ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    bool PLIB_USART_RunInSleepModeIsEnabled ( USART_MODULE_ID index )

  Summary:
    Gets the status of Run in Sleep mode.

  Description:
    This function indicates if the USART module has been enabled to run in
    Sleep mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true          - Run in Sleep mode is enabled
    - false         - Run in Sleep mode is disabled

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    bool status;
    status = PLIB_USART_RunInSleepModeIsEnabled (MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsRunInSleepMode in your application to determine
    whether this feature is available.
*/

bool PLIB_USART_RunInSleepModeIsEnabled ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_USART_Enable ( USART_MODULE_ID index )

  Summary:
    Enables the specific USART module.

  Description:
    This function enables the specific USART module.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_Enable(MY_USART_INSTANCE);
    </code>

  Remarks:
    By calling this function, the USART pins are controlled by the USART
    module.
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsEnable in your application to determine whether
    this feature is available.
*/

void PLIB_USART_Enable( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_Disable ( USART_MODULE_ID index )

  Summary:
    Disables the specific USART module

  Description:
    This function disables the specific USART module.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_Disable(MY_USART_INSTANCE);
    </code>

  Remarks:
    The default state after any reset for a USART module is the disable state.
    If the USART is disabled, all of the related pins are in control of the
    general purpose I/O logic.
	
    Disabling the USART module resets the buffers to empty states. Any data
    characters in the buffers are lost and the baud rate is reset. All error
    and status bits are also reset.
	
    Disabling the USART while the USART is active, will abort all pending
    transmissions and receptions.  Re-enabling the USART will restart the
    module in the same configuration.
	
    When disabled, the USART power consumption is minimal.
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsEnable in your application to determine whether
    this feature is available.
*/

void PLIB_USART_Disable ( USART_MODULE_ID index ) ;


//******************************************************************************
/* Function:
    void PLIB_USART_TransmitterEnable ( USART_MODULE_ID index )

  Summary:
    Enables the specific USART module transmitter.

  Description:
    This function enables the specific USART module transmitter.
    This operation is atomic.

  Precondition:
    The USART module should be enabled using the function PLIB_USART_Enable
    before this function is called.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_TransmitterEnable(MY_USART_INSTANCE);
    </code>

  Remarks:
    The transmitter should not be enabled until the USART is enabled.
    The transmissions will not be enabled otherwise.
	
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsTransmitterEnable in your application to determine whether
    this feature is available.
*/

void PLIB_USART_TransmitterEnable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_TransmitterDisable ( USART_MODULE_ID index )

  Summary:
    Disables the specific USART module transmitter.

  Description:
    This function disables the specific USART module transmitter.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_TransmitterDisable(MY_USART_INSTANCE);
    </code>

  Remarks:
    Disabling the transmitter during a transmission will cause the
    transmission to be aborted.
	
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsTransmitterEnable in your application to determine whether
    this feature is available.
*/

void PLIB_USART_TransmitterDisable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_BaudRateSet ( USART_MODULE_ID index, uint32_t clockFrequency,
            uint32_t baudRate );

  Summary:
    Sets the baud rate to the desired value.

  Description:
    This function sets the baud rate to the desired value.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    baudRate        - Baud Rate Value
    clockFrequency  - Clock Frequency

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    uint32_t baudRateValue ;
    PLIB_USART_BaudRateSet(MY_USART_INSTANCE, MY_CLOCK_FREQUENCY, 9600);
    baudRateValue = PLIB_USART_BaudRateGet(MY_USART_INSTANCE, MY_CLOCK_FREQUENCY);
    </code>

  Remarks:
    Setting a new baud rate value causes the baud rate timer to reset.
    This ensures that the baud rate timer does not have to overflow before
    outputting the new baud rate.
	
    If the system clock is changed during an active receive operation, a
    receiver error or data loss may result.  To avoid this issue verify that
    no receptions are in progress before changing the system clock.
	
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsBaudRate in your application to determine whether
    this feature is available.
*/

void PLIB_USART_BaudRateSet ( USART_MODULE_ID index, uint32_t clockFrequency, uint32_t baudRate );


//******************************************************************************
/* Function:
    void PLIB_USART_BaudRateHighSet ( USART_MODULE_ID index,
            uint32_t clockFrequency, uint32_t baudRate );

  Summary:
    Sets the baud rate to the desired value.

  Description:
    This function sets the baud rate to the desired value.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    baudRate        - Baud Rate Value, it is the baud rate value
    clockFrequency  - Clock Frequency

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    uint32_t baudRateValue ;
    PLIB_USART_BaudRateHighSet(MY_USART_INSTANCE, MY_CLOCK_FREQUENCY, 9600);
    baudRateValue = PLIB_USART_BaudRateGet(MY_USART_INSTANCE, MY_CLOCK_FREQUENCY);
    </code>

  Remarks:
    Setting a new baud rate value causes the baud rate timer to reset.
    This ensures that the baud rate timer does not have to overflow before
    outputting the new baud rate.
	
    If the system clock is changed during an active receive operation, a
    receiver error or data loss may result.  To avoid this issue verify that
    no receptions are in progress before changing the system clock.
	
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsBaudRateHigh in your application to determine whether
    this feature is available.
*/

void PLIB_USART_BaudRateHighSet ( USART_MODULE_ID index, uint32_t clockFrequency, uint32_t baudRate );


//******************************************************************************
/* Function:
      uint32_t PLIB_USART_BaudRateGet ( USART_MODULE_ID index,
                    int32_t clockFrequency );

  Summary:
    Gets the baud rate current in use.

  Description:
    This function gets the baud rate that is currently in use. The clock frequency
    needs to be passed to the function.

  Conditions:
    None.

  Input:
    index       -  Identifier for the device instance to be configured
    clockFrequency  - Clock Frequency

  Returns:
    - BaudRate    - Baud rate value

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    uint32_t baudRate ;
    PLIB_USART_BaudRateSet(MY_USART_INSTANCE, MY_CLOCK_FREQUENCY, 9600);
    baudRate = PLIB_USART_BaudRateGet(MY_USART_INSTANCE, MY_CLOCK_FREQUENCY);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsBaudRate in your application to determine whether
    this feature is available.
*/

uint32_t PLIB_USART_BaudRateGet ( USART_MODULE_ID index, int32_t clockFrequency );

//******************************************************************************
/* Function:
    void PLIB_USART_AddressSet
    (
        USART_MODULE_ID index,
        uint8_t address
    )

  Summary:
    Sets the address for the Address Detect mode.

  Description:
    This function sets the address for the Address Detect mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    address         - Address

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1
    uint8_t address = 0x02;
    PLIB_USART_AddressSet (MY_USART_INSTANCE, address);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverAddress in your application to determine
    whether this feature is available.
*/

void PLIB_USART_AddressSet
(
    USART_MODULE_ID index,
    uint8_t address
);

//******************************************************************************
/* Function:
    uint8_t PLIB_USART_AddressGet 
    (
        USART_MODULE_ID index
    )

  Summary:
    Gets the address for the Address Detect mode.

  Description:
    This function returns the address value being used for the Address
    Detect mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - address         - The address being used

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1
    uint8_t address = 0;
    address = PLIB_USART_AddressGet (MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverAddress in your application to determine
    whether this feature is available.
*/

uint8_t PLIB_USART_AddressGet
(
    USART_MODULE_ID index
);

//******************************************************************************
/* Function:
    void PLIB_USART_AddressMaskSet
    (
        USART_MODULE_ID index,
        uint8_t mask
    )

  Summary:
    Sets the address mask for the Address Detect mode.

  Description:
    This function sets the address mask for the Address Detect mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    mask            - Address match mask bits

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1
    uint8_t mask = 0x0F;
    PLIB_USART_AddressMaskSet (MY_USART_INSTANCE, mask);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverAddressMask in your application to determine
    whether this feature is available.
*/

void PLIB_USART_AddressMaskSet
(
    USART_MODULE_ID index,
    uint8_t mask
);

//******************************************************************************
/* Function:
    uint8_t PLIB_USART_AddressMaskGet 
    (
        USART_MODULE_ID index
    )

  Summary:
    Gets the address mask for the Address Detect mode.

  Description:
    This function returns the address mask value being used for the Address
    Detect mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - mask            - Address mask being used

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1
    uint8_t mask = 0;
    mask = PLIB_USART_AddressMaskGet (MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverAddressMask in your application to determine
    whether this feature is available.
*/

uint8_t PLIB_USART_AddressMaskGet
(
    USART_MODULE_ID index
);

//******************************************************************************
/* Function:
    void PLIB_USART_ReceiverAddressDetectEnable ( USART_MODULE_ID index )

  Summary:
    Enables the Address Detect mode.

  Description:
    This function enables the Address Detect mode. If it is enabled, and the
    device is addressed, disable the Address Detect mode to continue receiving
    bytes.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_ReceiverAddressDetectEnable(MY_USART_INSTANCE);
    </code>

  Remarks:
    If 9 data bits are not selected, this bit has no effect.

    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverAddressDetect in your application to determine whether
    this feature is available.
*/

void PLIB_USART_ReceiverAddressDetectEnable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_ReceiverAddressDetectDisable ( USART_MODULE_ID index )

  Summary:
    Enables the Address Detect mode.

  Description:
    This function disables the Address Detect mode. If it is enabled, and the
    device is addressed, disable the Address Detect mode to continue receiving
    bytes.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_ReceiverAddressDetectDisable(MY_USART_INSTANCE);
    </code>

  Remarks:
    All bytes are received, and the 9th bit can be used as the parity bit.
    By default, the address detect is disabled.
	
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverAddressDetect in your application to determine whether
    this feature is available.
*/

void PLIB_USART_ReceiverAddressDetectDisable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_USART_BaudRateAutoDetectIsComplete ( USART_MODULE_ID index )

  Summary:
    Gets the state of the automatic baud detection.

  Description:
    This function gets the state of the automatic baud detection and returns a '0'
    if the baud rate auto detection is complete.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true            - Baud rate detection is not complete
    - false           - Baud rate detection is complete

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_BaudRateAutoDetectEnable(MY_USART_INSTANCE);

    // Wait until the baud rate is detected.
    while(!PLIB_USART_BaudRateAutoDetectIsComplete(MY_USART_INSTANCE));
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsBaudRateAutoDetect in your application to determine whether
    this feature is available.
*/

bool PLIB_USART_BaudRateAutoDetectIsComplete ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_BaudRateAutoDetectEnable ( USART_MODULE_ID index )

  Summary:
    Enables baud rate measurement on the next character, which requires reception
    of the Sync character.

  Description:
    This function enables the baud rate measurement on the next character,
    which requires reception of the Sync character.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_BaudRateAutoDetectEnable(MY_USART_INSTANCE);

    // Wait until the baud rate is detected.
    while(!PLIB_USART_BaudRateAutoDetectIsComplete(MY_USART_INSTANCE));
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsBaudRateAutoDetect in your application to determine whether
    this feature is available.
*/

void PLIB_USART_BaudRateAutoDetectEnable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_TransmitterByteSend ( USART_MODULE_ID index, int8_t data )

  Summary:
    Data to be transmitted in the Byte mode.

  Description:
    The data is transmitted in the Byte mode for the specified USART module.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

    data            - Data to be transmitted.

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    uint8_t data = 'a';
    if(!PLIB_USART_TransmitterBufferIsFull(MY_USART_INSTANCE))
    {
        PLIB_USART_TransmitterByteSend(MY_USART_INSTANCE, data);
    }
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsTransmitter in your application to determine whether
    this feature is available.
*/

void PLIB_USART_TransmitterByteSend ( USART_MODULE_ID index, int8_t data );


//******************************************************************************
/* Function:
    int8_t PLIB_USART_ReceiverByteReceive ( USART_MODULE_ID index )

  Summary:
    Data to be received in the Byte mode.

  Description:
    The data to be received in Byte mode from the specified USART module.
    This operation is atomic.
    Call the functions PLIB_USART_ReceiverFramingErrorHasOccurred,
    PLIB_USART_ReceiverParityErrorHasOccurred and
    PLIB_USART_ReceiverOverrunHasOccurred to get any error that occurred.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - data            - Data to be received

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    bool isError;
    uint8_t mydata;

  	if(PLIB_USART_ReceiverDataIsAvailable(MY_USART_INSTANCE))
    {
        mydata = PLIB_USART_ReceiverByteReceive(MY_USART_INSTANCE);
    }

    isError = PLIB_USART_ReceiverFramingErrorHasOccurred(MY_USART_INSTANCE)     |
                   PLIB_USART_ReceiverParityErrorHasOccurred(MY_USART_INSTANCE) |
                   PLIB_USART_ReceiverOverrunHasOccurred(MY_USART_INSTANCE);

    if(PLIB_USART_ReceiverOverrunHasOccurred(MY_USART_INSTANCE))
    {
        PLIB_USART_ReceiverOverrunErrorClear(MY_USART_INSTANCE);
    }
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiver in your application to determine whether
    this feature is available.
*/

int8_t PLIB_USART_ReceiverByteReceive ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    int16_t PLIB_USART_Receiver9BitsReceive ( USART_MODULE_ID index )

  Summary:
    Data to be received in the byte mode with the 9th bit.

  Description:
    The data to be received in Byte mode from the specified USART module.
    with the 9th bit.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - data            - Data to be received

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    uint16_t mydata;

  	if(PLIB_USART_ReceiverDataIsAvailable(MY_USART_INSTANCE))
    {
        mydata = PLIB_USART_Receiver9BitsReceive (MY_USART_INSTANCE);
    }

    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiver9Bits in your application to determine
    whether this feature is available.
*/

int16_t PLIB_USART_Receiver9BitsReceive ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    bool PLIB_USART_ReceiverAddressIsReceived ( USART_MODULE_ID index )

  Summary:
    Checks and return if the data received is an address.

  Description:
    Checks and return if the data received is an address. The address has
    the 9th bit set. If data received has the 9th bit set, the function returns true;
	otherwise, the function returns false.
    This operation is atomic.

  Precondition:
    The USART module should be configured to use the 9 data bits.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true          - if the data received has the 9th bit set
    - false         - if the address received has the 9th bit cleared

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    int8_t address;

    if(PLIB_USART_ReceiverAddressIsReceived(MY_USART_INSTANCE))
    {
        address = PLIB_USART_ReceiverByteReceive(MY_USART_INSTANCE);
    }
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverAddressDetect in your application to determine whether
    this feature is available.
*/

bool PLIB_USART_ReceiverAddressIsReceived ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_Transmitter9BitsSend ( USART_MODULE_ID index,
            int8_t data, bool 9thBit )

  Summary:
    Data to be transmitted in the byte mode with the 9th bit.

  Description:
    The data is transmitted in the byte mode for the specified USART module,
    with 9th bit.
    This operation is atomic.

  Precondition:
    The USART module should be configured to use the 9 data bits.

  Parameters:
    index           - Identifier for the device instance to be configured
    data            - Data to be transmitted.
    9thBit          - 9th bit of the data to be transmitted.

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    uint8_t data = 'a';

    if(!PLIB_USART_TransmitterBufferIsFull(MY_USART_INSTANCE))
    {
        PLIB_USART_Transmitter9BitsSend(MY_USART_INSTANCE, data, false);
    }
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsTransmitter9BitsSend in your application to determine whether
    this feature is available.
*/

void PLIB_USART_Transmitter9BitsSend( USART_MODULE_ID index, int8_t data, bool Bit9th );

//******************************************************************************
/* Function:
    void PLIB_USART_TransmitterBreakSend ( USART_MODULE_ID index )

  Summary:
    Transmits the break character.

  Description:
    This function transmits the break character.
    This operation is atomic.

  Precondition:
    The application should wait for the transmitter to be idle, before calling
    this function.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    // Wait for the Transmit buffer to be empty.
    while(PLIB_USART_TransmitterBufferIsFull(MY_USART_INSTANCE));

    // Transmit the break character.
    PLIB_USART_TransmitterBreakSend(MY_USART_INSTANCE);

    // wait for the break transmission to complete
    while(!PLIB_USART_TransmitterBreakSendIsComplete(MY_USART_INSTANCE));

    </code>

  Remarks:
    After the break has been transmitted, the application can start
    transmitting next bytes.
	
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsTransmitterBreak in your application to determine whether
    this feature is available.
*/

void PLIB_USART_TransmitterBreakSend ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_USART_TransmitterBreakSendIsComplete ( USART_MODULE_ID index )

  Summary:
    Returns the status of the break transmission

  Description:
    The function returns the status of the break transmission.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true          - Transmit break on the next transmission
    - false         - Break transmission completed or not started

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    // Wait for the Transmit buffer to be empty.
    while(PLIB_USART_TransmitterBufferIsFull(MY_USART_INSTANCE));

    // Transmit the break character.
    PLIB_USART_TransmitterBreakSend(MY_USART_INSTANCE);

    // wait for the break transmission to complete
    while(!PLIB_USART_TransmitterBreakSendIsComplete(MY_USART_INSTANCE));

    </code>

  Remarks:
    After the break has been transmitted, the application can start
    transmitting next bytes.
	
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsTransmitterBreak in your application to determine whether
    this feature is available.
*/

bool PLIB_USART_TransmitterBreakSendIsComplete ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_USART_ReceiverDataIsAvailable ( USART_MODULE_ID index )

  Summary:
    Identifies if the receive data is available for the specified USART module.

  Description:
    This function identifies if the receive data is available for the specified
    USART module.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true            - The data is available
    - false           - The data is not available

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    int8_t mydata;

    if(PLIB_USART_ReceiverDataIsAvailable(MY_USART_INSTANCE))
    {
        mydata = PLIB_USART_ReceiverByteReceive(MY_USART_INSTANCE);
    }
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverDataAvailableStatus in your application to determine
    whether this feature is available.
*/

bool PLIB_USART_ReceiverDataIsAvailable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_ReceiverOverrunErrorClear ( USART_MODULE_ID index )

  Summary:
   Clears a USART vverrun error.

  Description:
    This function clears an overrun error. Clearing the error, resets the receive buffer.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    if(PLIB_USART_ReceiverOverrunHasOccurred(MY_USART_INSTANCE))
    {
        PLIB_USART_ReceiverOverrunErrorClear(MY_USART_INSTANCE);
    }
    </code>

  Remarks:
  	WARNING: Calling this API will clear all of the previously received data.
	
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverOverrunStatus in your application to determine
    whether this feature is available.
*/

void PLIB_USART_ReceiverOverrunErrorClear ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_USART_TransmitterBufferIsFull ( USART_MODULE_ID index )

  Summary:
    Gets the transmit buffer full status.

  Description:
    This function gets the transmit status of the specified USART module.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true            - The transmit buffer is full
    - false           - The transmit buffer is not full, at least one more
                        character can be written

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    // Wait for the Transmit buffer to be empty.
    while(PLIB_USART_TransmitterBufferIsFull(MY_USART_INSTANCE));

    // Transmit the break character.
    PLIB_USART_TransmitterBreakSend(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsTransmitterBufferFullStatus in your application to determine
    whether this feature is available.
*/

bool PLIB_USART_TransmitterBufferIsFull ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
     bool PLIB_USART_TransmitterIsEmpty ( USART_MODULE_ID index )

  Summary:
    Gets the transmit shift register empty status.

  Description:
    This function gets the transmit shift register empty status.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true            - The transmit shift register is empty
    - false           - The transmit shift register is not empty

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    // Wait for the Transmit buffer to be empty.
    while(!PLIB_USART_TransmitterIsEmpty(MY_USART_INSTANCE));

    // Transmit the break character.
    PLIB_USART_TransmitterBreakSend(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsTransmitterEmptyStatus in your application to determine
    whether this feature is available.
*/

bool PLIB_USART_TransmitterIsEmpty ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
     bool PLIB_USART_ReceiverFramingErrorHasOccurred ( USART_MODULE_ID index )

  Summary:
    Gets the framing error status.

  Description:
    This function gets the framing error status.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true            - The framing error was detected on the current character
    - false           - The framing error was not detected on the current
                        character

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    status = PLIB_USART_ReceiverFramingErrorHasOccurred(MY_USART_INSTANCE);
    </code>

  Remarks:
    Reading the error clears the error.

    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverFramingErrorStatus in your application to determine
    whether this feature is available.
*/

bool PLIB_USART_ReceiverFramingErrorHasOccurred ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_USART_ReceiverParityErrorHasOccurred ( USART_MODULE_ID index )

  Summary:
    Gets the parity error status.

  Description:
    This function gets the parity error status.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true          - The parity error was detected on the current character
    - false         - The parity error was not detected on the current character

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    status = PLIB_USART_ReceiverParityErrorHasOccurred(MY_USART_INSTANCE);
    </code>

  Remarks:
    Reading the error clears the error.
    A Parity error is irrelevant in case of 9-bit mode.
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverParityErrorStatus in your application to determine
    whether this feature is available.
*/

bool PLIB_USART_ReceiverParityErrorHasOccurred ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_USART_ReceiverOverrunHasOccurred ( USART_MODULE_ID index )

  Summary:
    Identifies if there was a receiver overrun error.

  Description:
    This function identifies if there was a receiver overrun error.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true          - The receive buffer has overflowed
    - false         - The receive buffer has not overflowed

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    if(PLIB_USART_ReceiverOverrunHasOccurred(MY_USART_INSTANCE))
    {
        PLIB_USART_ReceiverOverrunErrorClear(MY_USART_INSTANCE);
    }
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverOverrunStatus in your application to determine
    whether this feature is available.
*/

bool PLIB_USART_ReceiverOverrunHasOccurred ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_USART_ReceiverIsIdle ( USART_MODULE_ID index )

  Summary:
    Identifies if the receiver is idle.

  Description:
    This function identifies if the receiver is idle.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true          - The receive buffer is idle
    - false         - The receive buffer is not idle

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    int8_t mydata;

    if(PLIB_USART_ReceiverIsIdle(MY_USART_INSTANCE))
    {
        mydata = PLIB_USART_ReceiverByteReceive(MY_USART_INSTANCE);
    }
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverIdleStatus in your application to determine
    whether this feature is available.
*/

bool PLIB_USART_ReceiverIsIdle ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_ReceiverEnable ( USART_MODULE_ID index )

  Summary:
    Enables the USART receiver.

  Description:
    This function enables the USART receiver.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_ReceiverEnable(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverEnable in your application to determine
    whether this feature is available.
*/

void PLIB_USART_ReceiverEnable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_ReceiverDisable ( USART_MODULE_ID index )

  Summary:
    Disables the USART receiver.

  Description:
    This function disables the USART receiver.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_ReceiverDisable(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverEnable in your application to determine
    whether this feature is available.
*/

void PLIB_USART_ReceiverDisable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_TransmitterIdleIsLowEnable ( USART_MODULE_ID index )

  Summary:
    Enables the Transmit Idle Low state.

  Description:
    This function enables the Transmit Idle Low state. In the USART Synchronous
    mode, this function configures that the TX polarity, the idle state is low.

    When IrDA is enabled, this function sets that IrDA encoded Transmit Idle
    state to a '1'.

    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_TransmitterIdleIsLowEnable(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsTransmitterIdleIsLow in your application to determine
    whether this feature is available.
*/

void PLIB_USART_TransmitterIdleIsLowEnable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_TransmitterIdleIsLowDisable ( USART_MODULE_ID index )

  Summary:
    Disables the Transmit Idle Low state.

  Description:
    This function disables the Transmit Idle Low state. In USART Synchronous mode,
    this function configures that the TX polarity the idle state is high.

    When IrDA is enabled, this function sets the IrDA encoded Transmit Idle
    state to a '0'.

    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_TransmitterIdleIsLowDisable(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsTransmitterIdleIsLow in your application to determine
    whether this feature is available.
*/

void PLIB_USART_TransmitterIdleIsLowDisable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_ReceiverIdleStateLowEnable ( USART_MODULE_ID index )

  Summary:
    Enables receive polarity inversion.

  Description:
    This function enables receive polarity inversion. In the USART Synchronous mode,
    this function configures that the data is inverted.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_ReceiverIdleStateLowEnable(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverIdleStateLowEnable in your application to determine
    whether this feature is available.
*/

void PLIB_USART_ReceiverIdleStateLowEnable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_ReceiverIdleStateLowDisable ( USART_MODULE_ID index );

  Summary:
    Disables receive polarity inversion.
    This operation is atomic.

  Description:
    This function disables receive polarity inversion. In the USART Synchronous
    mode, this function configures that the data is not inverted.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_ReceiverIdleStateLowDisable(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverIdleStateLowEnable in your application to determine
    whether this feature is available.
*/

void PLIB_USART_ReceiverIdleStateLowDisable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_BaudRateHighEnable ( USART_MODULE_ID index )

  Summary:
    Enables high baud rate selection.

  Description:
    This function enables high baud rate selection.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_BaudRateHighEnable(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsBaudRateHigh in your application to determine
    whether this feature is available.
*/

void PLIB_USART_BaudRateHighEnable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_BaudRateHighDisable ( USART_MODULE_ID index )

  Summary:
    Disables the high baud rate selection.

  Description:
    This function disables the high baud rate selection.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_BaudRateHighDisable(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsBaudRateHigh in your application to determine
    whether this feature is available.
*/

void PLIB_USART_BaudRateHighDisable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_TransmitterInterruptModeSelect( USART_MODULE_ID index,
            USART_TRANSMIT_INTR_MODE interruptMode )

  Summary:
    Sets the USART transmitter interrupt mode.

  Description:
    This function sets the condition in which the USART module should generate
    an interrupt.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    interruptMode   - Interrupt mode; for possible configurations, refer to
                      USART_TRANSMIT_INTR_MODE

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_TransmitterInterruptModeSelect(MY_USART_INSTANCE,
        USART_TRANSMIT_FIFO_EMPTY );
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsTransmitterInterruptMode in your application to determine
    whether this feature is available.
*/

void PLIB_USART_TransmitterInterruptModeSelect(USART_MODULE_ID index, USART_TRANSMIT_INTR_MODE fifolevel);


//******************************************************************************
/* Function:
    void PLIB_USART_ReceiverInterruptModeSelect( USART_MODULE_ID index,
            USART_RECEIVE_INTR_MODE interruptMode )

  Summary:
    Sets the USART receiver FIFO level.

  Description:
    This function sets the USART receiver FIFO level.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    fifolevel       - For possible configurations, refer to USART_RECEIVE_INTR_MODE

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_ReceiverInterruptModeSelect(MY_USART_INSTANCE,
        USART_RECEIVE_FIFO_ONE_CHAR );
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverInterruptMode in your application to determine
    whether this feature is available.
*/

void PLIB_USART_ReceiverInterruptModeSelect(USART_MODULE_ID index, USART_RECEIVE_INTR_MODE interruptMode);


//******************************************************************************
/* Function:
     void PLIB_USART_LineControlModeSelect(USART_MODULE_ID index,
                USART_LINECONTROL_MODE dataFlowConfig)

  Summary:
    Sets the data flow configuration.

  Description:
    This function sets the USART data flow configuration based on the mask
    provided and the specified baud rate.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    mode            - For possible data flow configurations, refer to
                      USART_LINECONTROL_MODE

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_LineControlModeSelect(MY_USART_INSTANCE, \
                                        USART_8N1);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsLineControlMode in your application to determine
    whether this feature is available.
*/

void PLIB_USART_LineControlModeSelect(USART_MODULE_ID index,
									  USART_LINECONTROL_MODE dataFlowConfig);


//******************************************************************************
/* Function:
     void PLIB_USART_HandshakeModeSelect(USART_MODULE_ID index,
                USART_HANDSHAKE_MODE handshakeConfig)

  Summary:
    Sets the data flow configuration.

  Description:
    This function sets the USART data flow configuration based on the mask
    provided and the specified baud rate.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    mode            - For possible data flow configurations, refer to
                      USART_HANDSHAKE_MODE

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_HandshakeModeSelect(MY_USART_INSTANCE, \
                                        USART_HANDSHAKE_MODE_SIMPLEX);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsHandshakeMode in your application to determine
    whether this feature is available.
*/

void PLIB_USART_HandshakeModeSelect(USART_MODULE_ID index, USART_HANDSHAKE_MODE handshakeConfig);


//******************************************************************************
/* Function:
    void PLIB_USART_ReceiverAddressAutoDetectEnable(USART_MODULE_ID index,
            int8_t Mask)

  Summary:
    Setup the automatic Address Detect mode.

  Description:
    This function configures the automatic Address Detect mode. Uses the mask as
    the address character for automatic address detection.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    Mask            - Address character to be used, when enabled

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_ReceiverAddressAutoDetectEnable(MY_USART_INSTANCE,
    											MY_DEVICE_ADDRESS);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverAddressAutoDetect in your application to determine
    whether this feature is available.
*/

void PLIB_USART_ReceiverAddressAutoDetectEnable(USART_MODULE_ID index, int8_t Mask);


//******************************************************************************
/* Function:
    void PLIB_USART_ReceiverAddressAutoDetectDisable (
            USART_MODULE_ID index )

  Summary:
    Disables the automatic Address Detect mode.

  Description:
    This function disables the automatic Address Detect mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_ReceiverAddressAutoDetectDisable(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsReceiverAddressAutoDetect in your application to determine
    whether this feature is available.
*/

void PLIB_USART_ReceiverAddressAutoDetectDisable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_IrDAEnable ( USART_MODULE_ID index )

  Summary:
    Enables the IrDA encoder and decoder.

  Description:
    This function enables the IrDA encoder and decoder.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_IrDAEnable(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsIrDA in your application to determine
    whether this feature is available.
*/

void PLIB_USART_IrDAEnable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_IrDADisable ( USART_MODULE_ID index )

  Summary:
    Disables the IrDA encoder and decoder.

  Description:
    This function disables the IrDA encoder and decoder.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_IrDADisable(MY_USART_INSTANCE);
    </code>

  Remarks:
    By default, the IrDA Encoder and Decoder are disabled.
	
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsIrDA in your application to determine
    whether this feature is available.
*/

void PLIB_USART_IrDADisable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_LoopbackEnable ( USART_MODULE_ID index )

  Summary:
    Enables Loopback mode.

  Description:
    This function enables Loopback mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_LoopbackEnable(MY_USART_INSTANCE);
    </code>

  Remarks:
    By default, Loopback mode is disabled.
	
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsLoopback in your application to determine
    whether this feature is available.
*/

void PLIB_USART_LoopbackEnable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_LoopbackDisable ( USART_MODULE_ID index )

  Summary:
    Disables Loopback mode.

  Description:
    This function disables Loopback mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_LoopbackDisable(MY_USART_INSTANCE);
    </code>

  Remarks:
    By default, Loopback mode is disabled.
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsLoopback in your application to determine
    whether this feature is available.
*/

void PLIB_USART_LoopbackDisable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_OperationModeSelect(USART_MODULE_ID index,
            USART_OPERATION_MODE operationmode)

  Summary:
    Configures the operation mode of the USART module.

  Description:
    This function configures the operation mode of the USART (i.e., controls the
    pins used by the USART module). Refer to USART_OPERATION_MODE for the
    possible settings.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    operationmode   - One of the possible values from USART_OPERATION_MODE

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_OperationModeSelect(MY_USART_INSTANCE, USART_ENABLE_TX_RX_BCLK_USED);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsOperationMode in your application to determine
    whether this feature is available.
*/

void PLIB_USART_OperationModeSelect(USART_MODULE_ID index, USART_OPERATION_MODE operationmode);

//******************************************************************************
/* Function:
    void PLIB_USART_StopInIdleEnable ( USART_MODULE_ID index )

  Summary:
    Discontinues operation when the device enters Idle mode.

  Description:
    This function enables the USART module to discontinue operation when the
    device enters Idle mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_StopInIdleEnable(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsStopInIdle in your application to determine
    whether this feature is available.
*/

void PLIB_USART_StopInIdleEnable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_StopInIdleDisable ( USART_MODULE_ID index )

  Summary:
    Disables the Stop in Idle mode (the USART module continues operation when
    the device is in Idle mode).

  Description:
    This function disables the Stop in Idle mode. The USART module continues
    operation when the device is in Idle mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_StopInIdleDisable(MY_USART_INSTANCE);
    </code>

  Remarks:
    By default, the USART module will continue operation in Idle mode.
	
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsStopInIdle in your application to determine
    whether this feature is available.
*/

void PLIB_USART_StopInIdleDisable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_WakeOnStartEnable ( USART_MODULE_ID index )

  Summary:
    Enables the wake-up on start bit detection feature during Sleep mode.

  Description:
    This function enables the wake-up on start feature during Sleep mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_WakeOnStartEnable(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsWakeOnStart in your application to determine
    whether this feature is available.
*/

void PLIB_USART_WakeOnStartEnable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_USART_WakeOnStartDisable ( USART_MODULE_ID index )

  Summary:
    Disables the wake-up on start bit detection feature during Sleep mode.

  Description:
    This function disables the wake-up on start bit detection feature during
    Sleep mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    PLIB_USART_WakeOnStartDisable(MY_USART_INSTANCE);
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsWakeOnStart in your application to determine
    whether this feature is available.
*/

void PLIB_USART_WakeOnStartDisable ( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_USART_WakeOnStartIsEnabled ( USART_MODULE_ID index )

  Summary:
    Gets the state of the sync break event completion.

  Description:
    This function returns the status of the sync break event, when called
    after enabling using PLIB_USART_WakeOnStartEnable.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    //Call the interface just prior to entering the sleep mode.
    PLIB_USART_WakeOnStartEnable(MY_USART_INSTANCE);

    // Check the status if the Sync break event is over.
    if(PLIB_USART_WakeOnStartIsEnabled(MY_USART_INSTANCE))
    {
       // Do Something
    }
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_USART_ExistsWakeOnStart in your application to determine
    whether this feature is available.
*/

bool PLIB_USART_WakeOnStartIsEnabled ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    void PLIB_USART_InitializeModeGeneral( USART_MODULE_ID index, bool autobaud,
        bool loopBackMode, bool wakeFromSleep, bool irdaMode, bool stopInIdle );

  Summary:
    Enables or disables general features of the USART module.

  Description:
    This function enables or disables general features of the USART module.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    autobaud        - If true, auto baud rate detection is enabled. If false
                      the feature is disabled.
    loopBackMode    - If true, loop back is enabled. If false the feature is
                      disabled.
    wakeFromSleep   - If true, the USART module will wake up the CPU from sleep
                      mode on USART activity. If false the feature is disabled.
    irdaMode        - If true, the IrDA mode is enabled. If false the feature
                      is disabled.
    stopInIdle      - If true, module will stop functioning when CPU enters Idle
                      mode. If false, the feature is disabled.

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    // Enable loopback, disable IrDA, disable auto baud detection and disable
    // wake from sleep. Enable stop in idle
    PLIB_USART_InitializeModeGeneral(MY_USART_INSTANCE, false, true,
                                                        false, false, true);
    </code>

  Remarks:

    Enabling the wake from sleep feature will cause the first character that is
    received by the USART module to be discarded. This feature should only be
    enabled if the CPU is to placed in power saving mode.
	
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability. Availability of this
    function can also be determined if all of the following functions return
    true:
    - PLIB_USART_ExistsLoopback
    - PLIB_USART_ExistsBaudRateAutoDetect
    - PLIB_USART_ExistsWakeOnStart
    - PLIB_USART_ExistsIrDA
    - PLIB_USART_ExistsStopInIdle
*/

void PLIB_USART_InitializeModeGeneral( USART_MODULE_ID index, bool autobaud, bool loopBackMode, bool wakeFromSleep, bool irdaMode, bool stopInIdle );

//******************************************************************************
/* Function:
    void PLIB_USART_InitializeOperation( USART_MODULE_ID index ,
            USART_RECEIVE_INTR_MODE receiveInterruptMode,
            USART_TRANSMIT_INTR_MODE transmitInterruptMode,
            USART_OPERATION_MODE operationMode);

  Summary:
    Configures the Receive and Transmit FIFO interrupt levels and the hardware
    lines to be used by the module.

  Description:
    This function configures the Receive and Transmit FIFO interrupt levels and
    the hardware lines to be used by the module.

  Precondition:
    None.

  Parameters:
    index                   - Identifier for the device instance to be configured
    receiveInterruptMode    - Receiver FIFO interrupt level
    transmitInterruptMode   - Transmit FIFO interrupt level
    operationMode           - Hardware lines to be used by the USART.

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    // Set receive FIFO to interrupt when FIFO is 3/4 level full
    // Set Transmit FIFO to interrupt when FIFO is empty
    // USART module will only use RX and TX hardware lines

    PLIB_USART_InitializeOperation(MY_USART_INSTANCE, USART_RECEIVE_FIFO_3B4FULL,
        USART_TRANSMIT_FIFO_EMPTY  , USART_ENABLE_TX_RX_USED);
    </code>

  Remarks:

    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability. Availability of this
    function can also be determined if all of the following functions return
    true:
    - PLIB_USART_ExistsReceiverInterruptMode
    - PLIB_USART_ExistsTransmitterInterruptMode
    - PLIB_USART_ExistsOperationMode
*/


void PLIB_USART_InitializeOperation( USART_MODULE_ID index , USART_RECEIVE_INTR_MODE receiveInterruptMode, USART_TRANSMIT_INTR_MODE transmitInterruptMode, USART_OPERATION_MODE operationMode);

//******************************************************************************
/* Function:
    void PLIB_USART_BaudSetAndEnable ( USART_MODULE_ID index, uint32_t
                                        clockFrequency, uint32_t baudRate );

  Summary:
    Sets the baud rate to the desired value and enables the USART receiver,
    transmitter and the USART module.

  Description:
    This function sets the baud rate to the desired value and enables the USART
    receiver, USART transmitter and USART module.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    baudRate        - Baud Rate Value
    clockFrequency  - Clock Frequency

  Returns:
    None.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    uint32_t baudRateValue ;
    PLIB_USART_BaudSetAndEnable(MY_USART_INSTANCE, MY_CLOCK_FREQUENCY, 9600);
    </code>

  Remarks:
    Setting a new baud rate value causes the baud rate timer to reset.
    This ensures that the baud rate timer does not have to overflow before
    outputting the new baud rate.
	
    If the system clock is changed during an active receive operation, a
    receiver error or data loss may result.  To avoid this issue verify that
    no receptions are in progress before changing the system clock.
	
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability. Availability of this
    function can also be determined if all of the following functions return
    true:
    - PLIB_USART_ExistsBaudRate
    - PLIB_USART_ExistsTransmitterEnable
    - PLIB_USART_ExistsReceiverEnable
    - PLIB_USART_ExistsEnable
*/

void PLIB_USART_BaudSetAndEnable( USART_MODULE_ID index, uint32_t systemClock, uint32_t baud );

//******************************************************************************
/* Function:
    USART_ERROR PLIB_USART_ErrorsGet ( USART_MODULE_ID index )

  Summary:
    Return the status of all errors in the specified USART module.

  Description:
    This function returns status of all errors in the specified USART module.
    The return value can be bitwise AND'ed with a USART_ERROR type to know the
    status of a specific error.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    Returns a bitmap of USART error status.

  Example:
    <code>
    #define MY_USART_INSTANCE USART_ID_1

    USART_ERROR error;

    // Get the status of all errors.
    error = PLIB_USART_ErrorsGet(MY_USART_INSTANCE);

    // Check if parity error is active
    if(error & USART_ERROR_PARITY)
    {
        // Parity error is active.
    }
    else if(error & USART_ERROR_FRAMING)
    {
        // Framing error is active.
    }
    </code>

  Remarks:
    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability. Availability of this
    function can also be determined if all of the followings functions return
    true:
    - PLIB_USART_ExistsReceiverFramingErrorStatus
    - PLIB_USART_ExistsReceiverParityErrorStatus
    - PLIB_USART_ExistsReceiverOverrunStatus
*/

USART_ERROR PLIB_USART_ErrorsGet( USART_MODULE_ID index);

//******************************************************************************
/* Function:
    PLIB_USART_TransmitterAddressGet( USART_MODULE_ID index )

  Summary:
    Returns the address of the USART TX register

  Description:
    This function returns the address of the USART TX register.
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    Address of the USART TX register

  Remarks:
    None.
*/
void* PLIB_USART_TransmitterAddressGet( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ReceiverAddressGet( USART_MODULE_ID index )

  Summary:
    Returns the address of the USART RX register

  Description:
    This function returns the address of the USART RX register.
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    Address of the USART RX register

  Remarks:
    None.
*/

void* PLIB_USART_ReceiverAddressGet( USART_MODULE_ID index );

// *****************************************************************************
// *****************************************************************************
// Section: USART Peripheral Library Exists Functions
// *****************************************************************************
// *****************************************************************************

/* The following functions indicate the existence of the features on the device.
*/

//******************************************************************************
/* Function:
    PLIB_USART_ExistsRunInOverflow ( USART_MODULE_ID index )

  Summary:
    Identifies whether the Run in overflow condition feature exists on the USART module.

  Description:
    This function identifies whether the Run in Overflow condition feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_RunInOverflowEnable
    - PLIB_USART_RunInOverflowDisable
    - PLIB_USART_RunInOverflowIsEnabled

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Run in Overflow condition feature is supported on the device
    - false  - The Run in Overflow condition feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsRunInOverflow ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ExistsBRGClockSourceSelect ( USART_MODULE_ID index )

  Summary:
    Identifies whether the BRG Clock source select feature exists on the 
	USART module.

  Description:
    This function identifies whether the BRG Clock source select feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_BRGClockSourceSelect
    - PLIB_USART_BRGClockSourceGet

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BRG clock source select feature is supported on the device
    - false  - The BRG clock source select feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsBRGClockSourceSelect ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ExistsModuleBusyStatus ( USART_MODULE_ID index )

  Summary:
    Identifies whether the module running status feature exists on the 
	USART module.

  Description:
    This function identifies whether the module running status feature is
    available on the USART module. When this function returns true, this
    function is supported on the device:
    - PLIB_USART_ModuleIsBusy

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Module running status feature is supported on the device
    - false  - The Module running status feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsModuleBusyStatus ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ExistsRunInSleepMode ( USART_MODULE_ID index )

  Summary:
    Identifies whether the Run in Sleep mode feature exists on the USART module.

  Description:
    This function identifies whether the Run in Sleep mode feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_RunInSleepModeEnable
    - PLIB_USART_RunInSleepModeDisable
    - PLIB_USART_RunInSleepModeIsEnabled

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Run in Sleep mode feature is supported on the device
    - false  - The Run in Sleep mode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsRunInSleepMode ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ExistsEnable( USART_MODULE_ID index )

  Summary:
    Identifies whether the EnableControl feature exists on the USART module.

  Description:
    This function identifies whether the EnableControl feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_Disable
    - PLIB_USART_Enable

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

bool PLIB_USART_ExistsEnable( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ExistsHandshakeMode( USART_MODULE_ID index )

  Summary:
    Identifies whether the HandShakeMode feature exists on the USART module.

  Description:
    This function identifies whether the HandShakeMode feature is
    available on the USART module. When this function returns true, this
    function is supported on the device:
    - PLIB_USART_HandshakeModeSelect

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The HandShakeMode feature is supported on the device
    - false  - The HandShakeMode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsHandshakeMode( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsIrDA( USART_MODULE_ID index )

  Summary:
    Identifies whether the IrDAControl feature exists on the USART module.

  Description:
    This function identifies whether the IrDAControl feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_IrDADisable
    - PLIB_USART_IrDAEnable

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The IrDAControl feature is supported on the device
    - false  - The IrDAControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsIrDA( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsLineControlMode( USART_MODULE_ID index )

  Summary:
    Identifies whether the LineControlMode feature exists on the USART module.

  Description:
    This function identifies whether the LineControlMode feature is
    available on the USART module. When this function returns true, this
    function is supported on the device:
    - PLIB_USART_LineControlModeSelect

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The LineControlMode feature is supported on the device
    - false  - The LineControlMode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsLineControlMode( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsLoopback( USART_MODULE_ID index )

  Summary:
    Identifies whether the Loopback feature exists on the USART module.

  Description:
    This function identifies whether the Loopback feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_LoopbackEnable
    - PLIB_USART_LoopbackDisable

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Loopback feature is supported on the device
    - false  - The Loopback feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsLoopback( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsOperationMode( USART_MODULE_ID index )

  Summary:
    Identifies whether the OperationMode feature exists on the USART module.

  Description:
    This function identifies whether the OperationMode feature is
    available on the USART module. When this function returns true, this
    function is supported on the device:
    - PLIB_USART_OperationModeSelect

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The OperationMode feature is supported on the device
    - false  - The OperationMode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsOperationMode( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsStopInIdle( USART_MODULE_ID index )

  Summary:
    Identifies whether the StopInIdle feature exists on the USART module.

  Description:
    This function identifies whether the StopInIdle feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_StopInIdleEnable
    - PLIB_USART_StopInIdleDisable

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

bool PLIB_USART_ExistsStopInIdle( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ExistsWakeOnStart( USART_MODULE_ID index )

  Summary:
    Identifies whether the WakeOnStart feature exists on the USART module.

  Description:
    This function identifies whether the WakeOnStart feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_WakeOnStartEnable
    - PLIB_USART_WakeOnStartDisable
    - PLIB_USART_WakeOnStartIsEnabled

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The WakeOnStart feature is supported on the device
    - false  - The WakeOnStart feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsWakeOnStart( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsBaudRate( USART_MODULE_ID index )

  Summary:
    Identifies whether the BaudRate feature exists on the USART module.

  Description:
    This function identifies whether the BaudRate feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_BaudRateSet
    - PLIB_USART_BaudRateGet

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BaudRate feature is supported on the device
    - false  - The BaudRate feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsBaudRate( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsBaudRateAutoDetect( USART_MODULE_ID index )

  Summary:
    Identifies whether the BaudRateAutoDetect feature exists on the USART module.

  Description:
    This function identifies whether the BaudRateAutoDetect feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_BaudRateAutoDetectEnable
    - PLIB_USART_BaudRateAutoDetectIsComplete

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BaudRateAutoDetect feature is supported on the device
    - false  - The BaudRateAutoDetect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsBaudRateAutoDetect( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ExistsBaudRateHigh( USART_MODULE_ID index )

  Summary:
    Identifies whether the BaudRateHigh feature exists on the USART module.

  Description:
    This function identifies whether the BaudRateHigh feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_BaudRateHighSet
    - PLIB_USART_BaudRateHighDisable
    - PLIB_USART_BaudRateHighEnable

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BaudRateHigh feature is supported on the device
    - false  - The BaudRateHigh feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsBaudRateHigh( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ExistsReceiverAddress ( USART_MODULE_ID index )

  Summary:
    Identifies whether the Receiver Address feature exists on the USART
    module.

  Description:
    This function identifies whether the Receiver Address feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_AddressSet
    - PLIB_USART_AddressGet

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Receiver address feature is supported on the device
    - false  - The Receiver address feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsReceiverAddress ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ExistsReceiverAddressMask ( USART_MODULE_ID index )

  Summary:
    Identifies whether the Receiver Address Mask feature exists on the USART
    module.

  Description:
    This function identifies whether the Receiver Address Mask feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_AddressMaskSet
    - PLIB_USART_AddressMaskGet

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Receiver address mask feature is supported on the device
    - false  - The Receiver address mask feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsReceiverAddressMask ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ExistsReceiver( USART_MODULE_ID index )

  Summary:
    Identifies whether the Receiver feature exists on the USART module.

  Description:
    This function identifies whether the Receiver feature is available on the
    USART module. When this function returns true, these functions are supported
    on the device:
    - PLIB_USART_ReceiverByteReceive
    - PLIB_USART_ReceiverAddressGet

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Receiver feature is supported on the device
    - false  - The Receiver feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsReceiver( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ExistsReceiver9Bits ( USART_MODULE_ID index )

  Summary:
    Identifies whether the 9 Bits Receiver feature exists on the USART module.

  Description:
    This function identifies whether the 9 Bits Receiver feature is available on the
    USART module. When this function returns true, this function is supported
    on the device:
    - PLIB_USART_Receiver9BitsReceive

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The feature is supported on the device
    - false  - The feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsReceiver9Bits ( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ExistsReceiverAddressAutoDetect( USART_MODULE_ID index )

  Summary:
    Identifies whether the ReceiverAddressAutoDetect feature exists on the USART module.

  Description:
    This function identifies whether the ReceiverAddressAutoDetect feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_ReceiverAddressAutoDetectEnable
    - PLIB_USART_ReceiverAddressAutoDetectDisable

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiverAddressAutoDetect feature is supported on the device
    - false  - The ReceiverAddressAutoDetect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsReceiverAddressAutoDetect( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsReceiverAddressDetect( USART_MODULE_ID index )

  Summary:
    Identifies whether the ReceiverAddressDetect feature exists on the USART module.

  Description:
    This function identifies whether the ReceiverAddressDetect feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_ReceiverAddressDetectEnable
    - PLIB_USART_ReceiverAddressDetectDisable
    - PLIB_USART_ReceiverAddressIsReceived

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiverAddressDetect feature is supported on the device
    - false  - The ReceiverAddressDetect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsReceiverAddressDetect( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsReceiverDataAvailableStatus( USART_MODULE_ID index )

  Summary:
    Identifies whether the ReceiverDataAvailable feature exists on the USART
    module

  Description:
    This function identifies whether the ReceiverDataAvailable feature is
    available on the USART module. When this function returns true, this
    function is supported on the device:
    - PLIB_USART_ReceiverDataIsAvailable

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiverDataAvailable feature is supported on the device
    - false  - The ReceiverDataAvailable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsReceiverDataAvailableStatus( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ExistsReceiverEnable( USART_MODULE_ID index )

  Summary:
    Identifies whether the ReceiverEnableControl feature exists on the USART module.

  Description:
    This function identifies whether the ReceiverEnableControl feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_ReceiverEnable
    - PLIB_USART_ReceiverDisable

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiverEnableControl feature is supported on the device
    - false  - The ReceiverEnableControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsReceiverEnable( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsReceiverIdleStatus( USART_MODULE_ID index )

  Summary:
    Identifies whether the ReceiverIdle feature exists on the USART module.

  Description:
    This function identifies whether the ReceiverIdle feature is
    available on the USART module. When this function returns true, this
    function is supported on the device:
    - PLIB_USART_ReceiverIsIdle

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiverIdle feature is supported on the device
    - false  - The ReceiverIdle feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsReceiverIdleStatus( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsReceiverFramingErrorStatus( USART_MODULE_ID index )

  Summary:
    Identifies whether the ReceiverFramingError feature exists on the USART module.

  Description:
    This function identifies whether the ReceiverFramingError feature is
    available on the USART module. When this function returns true, this
    function is supported on the device:
    - PLIB_USART_ReceiverFramingErrorHasOccurred

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiverFramingError feature is supported on the device
    - false  - The ReceiverFramingError feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsReceiverFramingErrorStatus( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsReceiverInterruptMode( USART_MODULE_ID index )

  Summary:
    Identifies whether the ReceiverInterruptMode feature exists on the USART module.

  Description:
    This function identifies whether the ReceiverInterruptMode feature is
    available on the USART module. When this function returns true, this
    function is supported on the device:
    - PLIB_USART_ReceiverInterruptModeSelect

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiverInterruptMode feature is supported on the device
    - false  - The ReceiverInterruptMode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsReceiverInterruptMode( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsReceiverIdleStateLowEnable( USART_MODULE_ID index )

  Summary:
    Identifies whether the ReceiverPolarityInvert feature exists on the USART module.

  Description:
    This function identifies whether the ReceiverPolarityInvert feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_ReceiverIdleStateLowEnable
    - PLIB_USART_ReceiverIdleStateLowDisable

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiverPolarityInvert feature is supported on the device
    - false  - The ReceiverPolarityInvert feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsReceiverIdleStateLowEnable( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsReceiverParityErrorStatus( USART_MODULE_ID index )

  Summary:
    Identifies whether the ReceiverParityError feature exists on the USART module.

  Description:
    This function identifies whether the ReceiverParityError feature is
    available on the USART module. When this function returns true, this
    function is supported on the device:
    - PLIB_USART_ReceiverParityErrorHasOccurred

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiverParityError feature is supported on the device
    - false  - The ReceiverParityError feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsReceiverParityErrorStatus( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsReceiverOverrunStatus( USART_MODULE_ID index )

  Summary:
    Identifies whether the ReceiverOverrunError feature exists on the USART module.

  Description:
    This function identifies whether the ReceiverOverrunError feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_ReceiverOverrunErrorClear
    - PLIB_USART_ReceiverOverrunHasOccurred

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiverOverrunError feature is supported on the device
    - false  - The ReceiverOverrunError feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsReceiverOverrunStatus( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsTransmitter( USART_MODULE_ID index )

  Summary:
    Identifies whether the Transmitter feature exists on the USART module.

  Description:
    This function identifies whether the Transmitter feature is
    available on the USART module. When this function returns true, these 
    functions are supported on the device:
    - PLIB_USART_TransmitterByteSend
    - PLIB_USART_TransmitterAddressGet

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Transmitter feature is supported on the device
    - false  - The Transmitter feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsTransmitter( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsTransmitter9BitsSend( USART_MODULE_ID index )

  Summary:
    Identifies whether the Transmitter9Bits feature exists on the USART module.

  Description:
    This function identifies whether the Transmitter9Bits feature is
    available on the USART module. When this function returns true, this
    function is supported on the device:
    - PLIB_USART_Transmitter9BitsSend

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Transmitter9Bits feature is supported on the device
    - false  - The Transmitter9Bits feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsTransmitter9BitsSend( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsTransmitterBreak( USART_MODULE_ID index )

  Summary:
    Identifies whether the TransmitterBreak feature exists on the USART module.

  Description:
    This function identifies whether the TransmitterBreak feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_TransmitterBreakSend
    - PLIB_USART_TransmitterBreakSendIsComplete

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The TransmitterBreak feature is supported on the device
    - false  - The TransmitterBreak feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsTransmitterBreak( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsTransmitterBufferFullStatus( USART_MODULE_ID index )

  Summary:
    Identifies whether the TransmitterBufferFull feature exists on the USART module.

  Description:
    This function identifies whether the TransmitterBufferFull feature is
    available on the USART module. When this function returns true, this
    function is supported on the device:
    - PLIB_USART_TransmitterBufferIsFull

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The TransmitterBufferFull feature is supported on the device
    - false  - The TransmitterBufferFull feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsTransmitterBufferFullStatus( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsTransmitterEmptyStatus( USART_MODULE_ID index )

  Summary:
    Identifies whether the TransmitterEmpty feature exists on the USART module.

  Description:
    This function identifies whether the TransmitterEmpty feature is
    available on the USART module. When this function returns true, this
    function is supported on the device:
    - PLIB_USART_TransmitterIsEmpty

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The TransmitterEmpty feature is supported on the device
    - false  - The TransmitterEmpty feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsTransmitterEmptyStatus( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsTransmitterEnable( USART_MODULE_ID index )

  Summary:
    Identifies whether the TransmitterEnableControl feature exists on the USART
    module

  Description:
    This function identifies whether the TransmitterEnableControl feature is
    available on the USART module. When this function returns true, these
    functions are supported on the device:
    - PLIB_USART_TransmitterEnable
    - PLIB_USART_TransmitterDisable

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The TransmitterEnableControl feature is supported on the device
    - false  - The TransmitterEnableControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsTransmitterEnable( USART_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_USART_ExistsTransmitterInterruptMode( USART_MODULE_ID index )

  Summary:
    Identifies whether the TransmitterInterruptMode feature exists on the USART module.

  Description:
    This function identifies whether the TransmitterInterruptMode feature is
    available on the USART module. When this function returns true, this function
    is supported on the device:
    - PLIB_USART_TransmitterInterruptModeSelect

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The TransmitterInterruptMode feature is supported on the device
    - false  - The TransmitterInterruptMode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsTransmitterInterruptMode( USART_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_USART_ExistsTransmitterIdleIsLow( USART_MODULE_ID index )

  Summary:
    Identifies whether the TransmitterIdleIsLow feature exists on the USART module.

  Description:
    This function identifies whether the TransmitterIdleIsLow feature is available
    on the USART module. When this function returns true, these functions are
    supported on the device:
    - PLIB_USART_TransmitterIdleIsLowDisable
    - PLIB_USART_TransmitterIdleIsLowEnable

    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The TransmitterIdleIsLow feature is supported on the device
    - false  - The TransmitterIdleIsLow feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_USART_ExistsTransmitterIdleIsLow( USART_MODULE_ID index );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // #ifndef _PLIB_USART_H

/******************************************************************************
 End of File
*/
