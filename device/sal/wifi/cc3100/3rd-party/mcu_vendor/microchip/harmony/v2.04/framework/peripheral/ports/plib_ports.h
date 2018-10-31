/*******************************************************************************
  Ports Peripheral Library Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    plib_ports.h

  Summary:
    Ports Peripheral Library Interface header for Ports function definitions.

  Description:
    This header file contains the function prototypes and definitions of
    the data types and constants that make up the interface to the Ports
    Peripheral Library for all families of Microchip microcontrollers. The
    definitions in this file are common to the Ports peripheral.
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

#ifndef _PLIB_PORTS_H
#define _PLIB_PORTS_H

#include <stdint.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Constants & Data Types
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/*  Ports data mask definition

  Summary:
    Data type defining the Ports data mask

  Description:
    This data type defines the Ports data mask

  Remarks:
    None.
*/

typedef uint16_t        PORTS_DATA_MASK;


// *****************************************************************************
/*  Ports data type definition

  Summary:
    Data type defining the Ports data type.

  Description:
    This data type defines the Ports data type.

  Remarks:
    None.
*/

typedef uint32_t        PORTS_DATA_TYPE;


// *****************************************************************************
// *****************************************************************************
// Section: Included Files (continued at end of file)
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.  However,
    please see the end of the file for additional implementation header files
    that are also included.
*/

#include "peripheral/ports/processor/ports_processor.h"

// *****************************************************************************
// *****************************************************************************
// Section: Ports Peripheral Library Interface Routines
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Function:
    void PLIB_PORTS_RemapInput( PORTS_MODULE_ID      index,
							  PORTS_REMAP_INPUT_FUNCTION inputFunction,
							  PORTS_REMAP_INPUT_PIN      remapInputPin );

  Summary:
    Input function remapping.

  Description:
    This function controls the Input function remapping. It allows user to map
    any of the input functionality on any of the remappable input pin.
    This function does not perform atomic register access.

  Precondition:
    IOLOCK bit of configuration register should be clear to allow any remapping.
    PLIB_DEVCON_DeviceRegistersUnlock function can be used for that purpose.
    Refer DEVCON PLIB (or System Service) and the specific device data sheet to
    find more information.

  Parameters:
    index           - Identifier for the device instance to be configured
    inputFunction   - One of the possible values of PORTS_REMAP_INPUT_FUNCTION
    remapInputPin   - One of the possible values of PORTS_REMAP_INPUT_PIN

  Returns:
    None.

  Example:
    <code>
    
    // System Unlock
    PLIB_DEVCON_SystemUnlock(DEVCON_ID_0);
    // Unlock PPS registers
    PLIB_DEVCON_DeviceRegistersUnlock(DEVCON_ID_0, DEVCON_PPS_REGISTERS);
    // Remapping input function 'Input Capture 1' to the Remappable pin 'RPD2'
    PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_IC1, INPUT_PIN_RPD2 );

    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsRemapInputOutput in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_RemapInput( PORTS_MODULE_ID      index,
							  PORTS_REMAP_INPUT_FUNCTION inputFunction,
							  PORTS_REMAP_INPUT_PIN      remapInputPin );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_RemapOutput( PORTS_MODULE_ID      index,
                                  PORTS_REMAP_OUTPUT_FUNCTION outputFunction,
                                  PORTS_REMAP_OUTPUT_PIN      remapOutputPin );

  Summary:
    Output function remapping.

  Description:
    This function controls the Output function remapping. it allows user to map
    any of the output functionality on any of the remappable output pin.
    This function does not perform atomic register access.

  Precondition:
    IOLOCK bit of configuration register should be clear to allow any remapping.
    PLIB_DEVCON_DeviceRegistersUnlock function can be used for that purpose. 
    Refer DEVCON PLIB (or System Service) and the specific device data sheet to
    find more information.

  Parameters:
    index           - Identifier for the device instance to be configured
    outputFunction  - One of the possible values of PORTS_REMAP_OUTPUT_FUNCTION
    remapOutputPin  - One of the possible values of PORTS_REMAP_OUTPUT_PIN

  Returns:
    None.

  Example:
    <code>
    
    // System Unlock
    PLIB_DEVCON_SystemUnlock(DEVCON_ID_0);
    // Unlock PPS registers
    PLIB_DEVCON_DeviceRegistersUnlock(DEVCON_ID_0, DEVCON_PPS_REGISTERS);
    // Remapping output function 'UART3 Transmit' to the Remappable pin 'RPA14'
    PLIB_PORTS_RemapOutput(PORTS_ID_0, OUTPUT_FUNC_U3TX, OUTPUT_PIN_RPA14);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsRemapInputOutput in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_RemapOutput( PORTS_MODULE_ID      index,
                                  PORTS_REMAP_OUTPUT_FUNCTION outputFunction,
                                  PORTS_REMAP_OUTPUT_PIN      remapOutputPin );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_PinModeSelect( PORTS_MODULE_ID  index,
                               PORTS_ANALOG_PIN pin,
                               PORTS_PIN_MODE   mode );

  Summary:
    Enables the selected pin as analog or digital.

  Description:
    This function enables the selected pin as analog or digital.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    pin             - Possible values of PORTS_ANALOG_PIN
    mode            - Possible values of PORTS_PIN_MODE

  Returns:
    None.

  Example:
    <code>
    
    // Make AN0 pin as Analog
    PLIB_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_0, PORTS_PIN_MODE_ANALOG);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPinMode in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_PinModeSelect( PORTS_MODULE_ID  index,
                               PORTS_ANALOG_PIN pin,
                               PORTS_PIN_MODE   mode );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_PinModePerPortSelect( PORTS_MODULE_ID  index,
	                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos,
                               PORTS_PIN_MODE   mode );

  Summary:
    Enables the selected port pin as analog or digital.

  Description:
    This function enables the selected port pin as analog or digital.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    bitPos          - Position in the PORT pins
    mode            - Possible values of PORTS_PIN_MODE

  Returns:
    None.

  Example:
    <code>
    
    // Make RC5 pin Analog
    PLIB_PORTS_PinModePerPortSelect(PORTS_ID_0, PORT_CHANNEL_C,
                                  PORTS_BIT_POS_5, PORTS_PIN_MODE_ANALOG);
                                                                
    </code>

  Remarks:
    This function is only available in devices with PPS. For Non-PPS devices, use the
    PLIB_PORTS_PinModeSelect function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPinModePerPort in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_PinModePerPortSelect( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos,
                               PORTS_PIN_MODE   mode );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChangeNoticePullDownPerPortEnable( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos )

  Summary:
    Enables the pull-down for selected Change Notice pins.

  Description:
    This function enables the pull-down for selected Change Notice pins.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    bitPos          - Position in the PORT pins

  Returns:
    None.

  Example:
    <code>
    
    // Enable pull-down for RC5 pin
    PLIB_PORTS_ChangeNoticePullDownPerPortEnable(PORTS_ID_0, PORT_CHANNEL_C,
                                                                PORTS_BIT_POS_5);
                                                                
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePullDownPerPort in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_ChangeNoticePullDownPerPortEnable( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChangeNoticePullDownPerPortDisable( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos )

  Summary:
    Disables the pull-down for selected Change Notice pins.

  Description:
    This function disables the pull-down for selected Change Notice pins.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    bitPos          - Position in the PORT pins

  Returns:
    None.

  Example:
    <code>
    
    // Disable pull-down for RC5 pin
    PLIB_PORTS_ChangeNoticePullDownPerPortDisable(PORTS_ID_0, PORT_CHANNEL_C,
                                                                PORTS_BIT_POS_5);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePullDownPerPort in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_ChangeNoticePullDownPerPortDisable( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChangeNoticePullUpPerPortEnable ( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos )

  Summary:
    Enables the pull-up for selected Change Notice pins.

  Description:
    This function enables the pull-up for selected Change Notice pins.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    bitPos          - Position in the PORT pins

  Returns:
    None.

  Example:
    <code>
    
    // Enable pull-up for RC5 pin
    PLIB_PORTS_ChangeNoticePullUpPerPortEnable(PORTS_ID_0, PORT_CHANNEL_C,
                                                                PORTS_BIT_POS_5);
    </code>

  Remarks:
    This function is only available in devices with PPS. For Non-PPS devices, use the
    PLIB_PORTS_ChangeNoticePullUpEnable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePullUpPerPort in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_ChangeNoticePullUpPerPortEnable ( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChangeNoticePullUpPerPortDisable( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos );

  Summary:
    Disables weak pull-up for the selected pin.

  Description:
    This function disables weak pull-up for the selected port pin.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    bitPos          - Position in the PORT pins

  Returns:
    None.

  Example:
    <code>
    
    // Disable pull-up for RC5 pin
    PLIB_PORTS_ChangeNoticePullUpPerPortDisable(PORTS_ID_0, PORT_CHANNEL_C,
                                                                PORTS_BIT_POS_5);
    
    </code>

  Remarks:
    This function is only available in devices with PPS. For Non-PPS devices, use the
    PLIB_PORTS_ChangeNoticePullUpDisable function.
    
    Pull-ups on change notification pins should always be disabled when the
    port pin is configured as a digital output.
  
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePullUpPerPort in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_ChangeNoticePullUpPerPortDisable( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_PinChangeNoticePerPortEnable ( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos )

  Summary:
    Enables CN interrupt for the selected pin.

  Description:
    This function enables Change Notice interrupt for the selected port pin.
    This function does not perform atomic register access.
    
  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    bitPos          - Position in the PORT pins

  Returns:
    None.

  Example:
    <code>
    
    // Enable CN interrupt for RC5 pin
    PLIB_PORTS_PinChangeNoticePerPortEnable(PORTS_ID_0, PORT_CHANNEL_C,
                                                                PORTS_BIT_POS_5);
    
    </code>

  Remarks:
    This function is only available in devices with PPS. For Non-PPS devices, use the
    PLIB_PORTS_PinChangeNoticeEnable function.
  
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticeIntPerPort in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_PinChangeNoticePerPortEnable ( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos );


// *****************************************************************************
/* Function:
	void PLIB_PORTS_PinChangeNoticePerPortDisable( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos )

  Summary:
    Disables CN interrupt for the selected pin.

  Description:
    This function disables Change Notice interrupt for the selected port pin.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    bitPos          - Position in the PORT pins

  Returns:
    None.

  Example:
    <code>
    
    // Disable CN interrupt for RC5 pin
    PLIB_PORTS_PinChangeNoticePerPortDisable(PORTS_ID_0, PORT_CHANNEL_C,
                                                                PORTS_BIT_POS_5);
    
    </code>

  Remarks:
    This function is only available in devices with PPS. For Non-PPS devices, use the
    PLIB_PORTS_PinChangeNoticeDisable function.
  
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticeIntPerPort in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_PinChangeNoticePerPortDisable( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChangeNoticePerPortTurnOn ( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel );

  Summary:
    Enables the change notification for selected port.

  Description:
    This function enables the change notification for selected port.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel

  Returns:
    None.

  Example:
    <code>

    // Enable Change notification for Port C
    PLIB_PORTS_ChangeNoticePerPortTurnOn(PORTS_ID_0, PORT_CHANNEL_C);
    
    </code>

  Remarks:
    This function is only available in devices with PPS. For Non-PPS devices, use the
    PLIB_PORTS_ChangeNoticeEnable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePerPortTurnOn in your application to determine
    whether this feature is available.
*/

void PLIB_PORTS_ChangeNoticePerPortTurnOn ( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChangeNoticePerPortTurnOff( PORTS_MODULE_ID  index,
                                                        PORTS_CHANNEL channel );

  Summary:
    Disables the change notification for selected port.

  Description:
    This function disables the change notification for selected port.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel

  Returns:
    None.

  Example:
    <code>

    // Disable Change notification for Port C
    PLIB_PORTS_ChangeNoticePerPortTurnOff(PORTS_ID_0, PORT_CHANNEL_C);
    
    </code>

  Remarks:
    This function is only available in devices with PPS. For Non-PPS devices, use the
    PLIB_PORTS_ChangeNoticeDisable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePerPortTurnOn in your application to determine
    whether this feature is available.
*/

void PLIB_PORTS_ChangeNoticePerPortTurnOff( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel );


// *****************************************************************************
/* Function:
	void PLIB_PORTS_ChangeNoticeInIdlePerPortEnable ( PORTS_MODULE_ID  index,
                                                        PORTS_CHANNEL channel );

  Summary:
    Allows CN to be working in Idle mode for selected channel.

  Description:
    This function makes sure that change notification feature keeps working in
    Idle mode for the selected channel.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel

  Returns:
    None.

  Example:
    <code>
    
    // Change notification continues working in Idle mode for Port C
    PLIB_PORTS_ChangeNoticeInIdlePerPortEnable(PORTS_ID_0, PORT_CHANNEL_C);
    
    </code>

  Remarks:
    This function is only available in devices with PPS. For Non-PPS devices, use the
    PLIB_PORTS_ChangeNoticeInIdleEnable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePerPortInIdle in your application to determine
    whether this feature is available.
*/

void PLIB_PORTS_ChangeNoticeInIdlePerPortEnable ( PORTS_MODULE_ID  index,
                                                        PORTS_CHANNEL channel );   


// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChangeNoticeInIdlePerPortDisable( PORTS_MODULE_ID  index,
                                                        PORTS_CHANNEL channel );

  Summary:
    Change Notification halts in Idle mode for selected channel.

  Description:
    This function makes sure that change notification feature halts in
    Idle mode for the selected channel.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel

  Returns:
    None.

  Example:
    <code>
    
    // Change notification halts in Idle mode for Port C
    PLIB_PORTS_ChangeNoticeInIdlePerPortDisable(PORTS_ID_0, PORT_CHANNEL_C);
    
    </code>

  Remarks:
    This function is only available in devices with PPS. For Non-PPS devices, use the
    PLIB_PORTS_ChangeNoticeInIdleDisable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePerPortInIdle in your application to determine
    whether this feature is available.
*/

void PLIB_PORTS_ChangeNoticeInIdlePerPortDisable( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel );

// *****************************************************************************
/* Function:
   bool PLIB_PORTS_ChangeNoticePerPortHasOccurred ( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos );

  Summary:
    checks the status of change on the pin

  Description:
    This function checks if the change has occurred on the given pin or not.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    bitPos          - Position in the PORT pins

  Returns:
    None.

  Example:
    <code>
    
    if(PLIB_PORTS_ChangeNoticePerPortHasOccurred( PORTS_ID_0, 
                                    PORT_CHANNEL_C, PORTS_BIT_POS_4 ) == True)
    {
     //do something
    }
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePerPortStatus in your application to determine
    whether this feature is available.
*/

bool PLIB_PORTS_ChangeNoticePerPortHasOccurred ( PORTS_MODULE_ID  index,
                               PORTS_CHANNEL channel, PORTS_BIT_POS  bitPos );
                               
// *****************************************************************************
/* Function:
    bool PLIB_PORTS_PinGet( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                            PORTS_BIT_POS       bitPos )

  Summary:
    Reads/Gets data from the selected digital pin.

  Description:
    This function reads/gets data from the selected digital PORT i/o pin.
    This function should be used to read the live data at the pin.
    This function does not perform atomic register access.
    
  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    Port pin read data.

  Example:
    <code>

    // read port pin RC4
    bool bitStatus = PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_C,
                                                        PORTS_BIT_POS_4);
    </code>

  Remarks:
    For reading the Latched data, PLIB_PORTS_PinGetLatched function should be used.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsRead in your application to determine whether
	this feature is available.
*/

bool PLIB_PORTS_PinGet( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                        PORTS_BIT_POS       bitPos );

// *****************************************************************************
/* Function:
    bool PLIB_PORTS_PinGetLatched( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                            PORTS_BIT_POS       bitPos )

  Summary:
    Reads/Gets data from the selected latch.

  Description:
    This function reads/gets data from the selected PORTx Data Latch, not from
    the port I/O pins.
    This function does not perform atomic register access.
    
  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    Latch read data.

  Example:
    <code>

    // read latch RC4
    bool bitStatus = PLIB_PORTS_PinGetLatched(PORTS_ID_0, PORT_CHANNEL_C,
                                                        PORTS_BIT_POS_4);
    </code>

  Remarks:
    For reading the Live data from the i/o pin, PLIB_PORTS_PinGet function
    should be used.
    
    This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsLatchRead in your application to determine whether
	this feature is available.
*/

bool PLIB_PORTS_PinGetLatched( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                        PORTS_BIT_POS       bitPos );
                        
// *****************************************************************************
/* Function:
    void PLIB_PORTS_PinWrite( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                              PORTS_BIT_POS       bitPos,
                              bool            value )

  Summary:
    Writes the selected digital pin/latch.

  Description:
    This function writes to the selected digital pin/latch.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS
    value           - Value to be written to the specific pin/latch
                      true - sets the bit, false - clears the bit

  Returns:
    None.

  Example:
    <code>
    
    // write 'one' in port RC4
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_4, 1);
                                                        
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsWrite in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_PinWrite( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                          PORTS_BIT_POS       bitPos,
                          bool            value );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_PinSet( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                            PORTS_BIT_POS       bitPos )

  Summary:
    Sets the selected digital pin/latch.

  Description:
    This function sets the selected digital pin/latch.    
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    None.

  Example:
    <code>
    
    // Sets port pin RC4
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_4);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsWrite in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_PinSet( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                        PORTS_BIT_POS       bitPos );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_PinClear( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                              PORTS_BIT_POS       bitPos )

  Summary:
    Clears the selected digital pin/latch.

  Description:
    This function clears the selected digital pin/latch.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    None.

  Example:
    <code>
    
    // Clears port pin RC4
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_4);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsWrite in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_PinClear( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                          PORTS_BIT_POS       bitPos );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_PinToggle( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                               PORTS_BIT_POS       bitPos )

  Summary:
    Toggles the selected digital pin/latch.

  Description:
    This function toggles the selected digital pin/latch.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    None.

  Example:
    <code>
    
    // Toggles port pin RC4
    PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_4);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsWrite in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_PinToggle( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                           PORTS_BIT_POS       bitPos );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_PinDirectionInputSet( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                          PORTS_BIT_POS       bitPos )

  Summary:
    Makes the selected pin direction input

  Description:
    This function makes the selected pin direction as input
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS direction that has to
                      be made input

  Returns:
    None.

  Example:
    <code>
    
    // make pin RC4 as input
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_4);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsDirection in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_PinDirectionInputSet( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                      PORTS_BIT_POS       bitPos );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_PinDirectionOutputSet( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                           PORTS_BIT_POS       bitPos )

  Summary:
    Makes the selected pin direction output

  Description:
    This function makes the selected pin direction as output
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS direction that has to
                      be made output

  Returns:
    None.

  Example:
    <code>
    
    // make pin RC4 as output
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_4);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsDirection in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_PinDirectionOutputSet( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                       PORTS_BIT_POS       bitPos );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_PinOpenDrainEnable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                        PORTS_BIT_POS       bitPos )

  Summary:
    Enables the open drain functionality for the selected pin.

  Description:
    This function enables the open drain functionality for the selected pin.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    None.

  Example:
    <code>
    
    // Enable open drain for pin RC4
    PLIB_PORTS_PinOpenDrainEnable(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_4);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsOpenDrain in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_PinOpenDrainEnable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                    PORTS_BIT_POS       bitPos );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_PinOpenDrainDisable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                         PORTS_BIT_POS       bitPos )

  Summary:
    Disables the open drain functionality for the selected pin.

  Description:
    This function disables the open drain functionality for the selected pin.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    bitPos          - One of the possible values of PORTS_BIT_POS.

  Returns:
    None.

  Example:
    <code>
    
    // Disable open drain for pin RC4
    PLIB_PORTS_PinOpenDrainDisable(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_4);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsOpenDrain in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_PinOpenDrainDisable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                     PORTS_BIT_POS       bitPos );


// *****************************************************************************
/* Function:
    PORTS_DATA_TYPE PLIB_PORTS_ReadLatched
    ( 
        PORTS_MODULE_ID index, 
        PORTS_CHANNEL channel
    )

  Summary:
    Reads and returns data from the selected Latch.

  Description:
    This function reads and returns the data from the selected Latch.
    This function does not perform atomic register access.
    
  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.

  Returns:
    Latch read data.

  Example:
    <code>

    // Read latch C
    PORTS_DATA_TYPE bitStatus = PLIB_PORTS_ReadLatched(PORTS_ID_0, PORT_CHANNEL_C);
    
    </code>

  Remarks:
    For reading the Live data, PLIB_PORTS_Read function should be used.
    
    This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsLatchRead in your application to determine whether
	this feature is available.
*/

PORTS_DATA_TYPE PLIB_PORTS_ReadLatched
    ( 
        PORTS_MODULE_ID index, 
        PORTS_CHANNEL channel
    );
                        
// *****************************************************************************
/* Function:
    PORTS_DATA_TYPE PLIB_PORTS_Read( PORTS_MODULE_ID index, PORTS_CHANNEL channel )

  Summary:
    Reads the selected digital port.

  Description:
    This function reads from the selected digital port.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.

  Returns:
    data on a port with width PORTS_DATA_TYPE

  Example:
    <code>
    
    // Read PORT C
    PORTS_DATA_TYPE readData = PLIB_PORTS_Read(PORTS_ID_0, PORT_CHANNEL_C);
    
    </code>

  Remarks:
    For reading the Latched data, PLIB_PORTS_ReadLatched function should be used.
  
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsRead in your application to determine whether
	this feature is available.
*/

PORTS_DATA_TYPE PLIB_PORTS_Read( PORTS_MODULE_ID index, PORTS_CHANNEL channel );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_Write( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                           PORTS_DATA_TYPE value )

  Summary:
    Writes the selected digital port/latch.

  Description:
    This function writes to the selected digital port/latch.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    value           - Value to be written into a port of width PORTS_DATA_TYPE

  Returns:
    None.

  Example:
    <code>
    
    // Write 0x12 into PORT C
    PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_C, 0x12);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsWrite in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_Write( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                       PORTS_DATA_TYPE value );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_Set( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                         PORTS_DATA_TYPE value,
                         PORTS_DATA_MASK mask )

  Summary:
    Sets the selected bits of the port.

  Description:
    This function performs an 'AND' operation on the value and mask parameters, 
	and then sets the bits in the port channel that were set by the result of the 
	'AND' operation.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    value           - Consists of information about which port bit has to be
                      set and which not
    mask            - Identifies the bits which could be intended for setting

  Returns:
    None.

  Example:
    <code>
    
    // MY_VALUE - 0x1234
    PORTS_DATA_MASK myMask = (PORTS_DATA_MASK)0x00FF;
    
    // Set the PORT C bit positions 2,4 and 5 (0x0034 = b0000 0000 0011 0100)
    PLIB_PORTS_Set(MY_PORTS_INSTANCE, PORT_CHANNEL_C, MY_VALUE, myMask);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsWrite in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_Set( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                     PORTS_DATA_TYPE value,
                     PORTS_DATA_MASK mask );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_Toggle( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                            PORTS_DATA_MASK toggleMask )

  Summary:
    Toggles the selected digital port/latch.

  Description:
    This function toggles the selected digital port/latch.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    toggleMask      - Identifies the bits to be toggled

  Returns:
    None.

  Example:
    <code>
    
    // Toggles the three least significant Port C bits
    PLIB_PORTS_Toggle(PORTS_ID_0, PORT_CHANNEL_C, 0x0007);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsWrite in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_Toggle( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                        PORTS_DATA_MASK toggleMask );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_Clear( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                           PORTS_DATA_MASK clearMask )

  Summary:
    Clears the selected digital port/latch bits.

  Description:
    This function clears the selected digital port/latch bits.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    clearMask       - Identifies the bits to be cleared

  Returns:
    None.

  Example:
    <code>
    
    // Clears the three least significant Port C bits
    PLIB_PORTS_Clear(PORTS_ID_0, PORT_CHANNEL_C, 0x0007);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsWrite in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_Clear( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                       PORTS_DATA_MASK clearMask );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_DirectionInputSet( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                       PORTS_DATA_MASK mask )

  Summary:
    Makes the selected pins direction input.

  Description:
    This function makes the selected pins direction input.
    This function does not perform atomic register access.
    
  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    mask            - Identifies the pins direction that has to be made input

  Returns:
    None.

  Example:
    <code>
    
    // Make RC0, RC1 and RC2 pins as Input
    PLIB_PORTS_DirectionInputSet(PORTS_ID_0, PORT_CHANNEL_C, 0x0007);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsDirection in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_DirectionInputSet( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                   PORTS_DATA_MASK mask );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_DirectionOutputSet( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                        PORTS_DATA_MASK mask )

  Summary:
    Makes the selected pins direction output.

  Description:
    This function makes the selected pins direction output.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    mask            - Identifies the pins direction that has to be made output

  Returns:
    None.

  Example:
    <code>
    
    // Make RC0, RC1 and RC2 pins as Output
    PLIB_PORTS_DirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_C, 0x0007);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsDirection in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_DirectionOutputSet( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                    PORTS_DATA_MASK mask );


// *****************************************************************************
/* Function:
    PORTS_DATA_MASK PLIB_PORTS_DirectionGet( PORTS_MODULE_ID index, PORTS_CHANNEL channel )

  Summary:
    Reads the direction of the selected digital port.

  Description:
    This function reads the direction of the selected digital port.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.

  Returns:
    Direction of the selected port of type PORTS_DATA_MASK

  Example:
    <code>
    
    // Reads the direction of Port C pins
    PORTS_DATA_MASK readDir = PLIB_PORTS_DirectionGet(PORTS_ID_0, PORT_CHANNEL_C);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsDirection in your application to determine whether
	this feature is available.
*/

PORTS_DATA_MASK PLIB_PORTS_DirectionGet( PORTS_MODULE_ID index, PORTS_CHANNEL channel );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_OpenDrainEnable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                     PORTS_DATA_MASK mask )

  Summary:
    Enables the open drain functionality for the selected port pins.

  Description:
    This function enables the open drain functionality for the selected port pins.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    mask            - Identifies the pins for the open drain to be enabled 

  Returns:
    None.

  Example:
    <code>
    
    // Enable Open Drain for RC0, RC1 and RC2 pins
    PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, PORT_CHANNEL_C, 0x0007);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsOpenDrain in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_OpenDrainEnable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                 PORTS_DATA_MASK mask );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_OpenDrainDisable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                      PORTS_DATA_MASK mask )

  Summary:
    Disables the open drain functionality for the selected port.

  Description:
    This function disables the open drain functionality for the selected port.
    This function does not perform atomic register access.
    
  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the Ports channel A, B, C, etc.
    mask            - Identifies the pins for the open drain to be disabled 

  Returns:
    None.

  Example:
    <code>
    
    // Disable Open Drain for RC0, RC1 and RC2 pins
    PLIB_PORTS_OpenDrainDisable(PORTS_ID_0, PORT_CHANNEL_C, 0x0007);
    
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPortsOpenDrain in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_OpenDrainDisable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                  PORTS_DATA_MASK mask );

// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChannelModeSelect
    ( 
        PORTS_MODULE_ID  index,
	    PORTS_CHANNEL channel,
        PORTS_DATA_MASK   modeMask,
        PORTS_PIN_MODE mode
    );

  Summary:
    Enables the selected channel pins as analog or digital.

  Description:
    This function enables the selected channel pins as analog or digital.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    modeMask        - Identifies the pins whose mode has to be modified.
                      Modes of the pins whose corresponding bit is '1' get
                      modified, mode of the other pins remains the same.
    mode            - Possible values of PORTS_PIN_MODE (Analog or Digital)

  Returns:
    None.

  Example:
    <code>
    
    // Make pins RC5, RC8 and RC13 Analog
    PLIB_PORTS_ChannelModeSelect(PORTS_ID_0, PORT_CHANNEL_C, 0x2120, PORTS_PIN_MODE_ANALOG);
                                                                
    </code>

  Remarks:
    This function is only available in devices with PPS. For Non-PPS devices, use the
    PLIB_PORTS_AnPinsModeSelect function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPinModePerPort in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_ChannelModeSelect
     ( 
         PORTS_MODULE_ID  index,
	     PORTS_CHANNEL channel,
         PORTS_DATA_MASK   modeMask,
         PORTS_PIN_MODE mode
     );

// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChannelChangeNoticePullUpEnable
     ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel, 
        PORTS_DATA_MASK  mask 
     );

  Summary:
    Enables Change Notice pull-up for the selected channel pins.

  Description:
    This function enables the Change Notice pull-up for the selected
    channel pins.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    mask            - Identifies the pins of the pull-up to be enabled 

  Returns:
    None.

  Example:
    <code>
    
    // Enable pull-up for RC5, RC8 and RC13 pins
    PLIB_PORTS_ChannelChangeNoticePullUpEnable(PORTS_ID_0, PORT_CHANNEL_C,
                                                                0x2120);
    </code>

  Remarks:
    This function is only available in devices with PPS. For Non-PPS devices, use the
    PLIB_PORTS_CnPinsPullUpEnable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePullUpPerPort in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_ChannelChangeNoticePullUpEnable
     ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel, 
        PORTS_DATA_MASK  mask 
     );

// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChannelChangeNoticePullUpDisable
     ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel, 
        PORTS_DATA_MASK  mask 
     );

  Summary:
    Disables Change Notice pull-up for the selected channel pins.

  Description:
    This function Disables the Change Notice pull-up for the selected
    channel pins.
    PLIB_PORTS_ChannelChangeNoticePullUpDisable

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    mask            - Identifies the pins of the pull-up to be disabled 

  Returns:
    None.

  Example:
    <code>
    
    // Disable pull-up for RC5, RC8 and RC13 pins
    PLIB_PORTS_ChannelChangeNoticePullUpDisable(PORTS_ID_0, PORT_CHANNEL_C,
                                                                0x2120);
    </code>

  Remarks:
    This function is only available in devices with PPS. For Non-PPS devices, use the
    PLIB_PORTS_CnPinsPullUpDisable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePullUpPerPort in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_ChannelChangeNoticePullUpDisable
     ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel, 
        PORTS_DATA_MASK  mask 
     );

// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChannelChangeNoticePullDownEnable
     ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel, 
        PORTS_DATA_MASK  mask 
     );

  Summary:
    Enables Change Notice pull-down for the selected channel pins.

  Description:
    This function enables the Change Notice pull-down for the selected
    channel pins.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    mask            - Identifies the pins for the pull-down to be enabled 

  Returns:
    None.

  Example:
    <code>
    
    // Enable pull-down for RC5, RC8 and RC13 pins
    PLIB_PORTS_ChannelChangeNoticePullDownEnable(PORTS_ID_0, PORT_CHANNEL_C,
                                                                0x2120);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePullDownPerPort in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_ChannelChangeNoticePullDownEnable
     ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel, 
        PORTS_DATA_MASK  mask 
     );

// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChannelChangeNoticePullDownDisable
     ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel, 
        PORTS_DATA_MASK  mask 
     );

  Summary:
    Disables Change Notice pull-down for the selected channel pins.

  Description:
    This function Disables the Change Notice pull-down for the selected
    channel pins.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    mask            - Identifies the pins for the pull-down to be disabled 

  Returns:
    None.

  Example:
    <code>
    
    // Disable pull-down for RC5, RC8 and RC13 pins
    PLIB_PORTS_ChannelChangeNoticePullDownDisable(PORTS_ID_0, PORT_CHANNEL_C,
                                                                0x2120);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePullDownPerPort in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_ChannelChangeNoticePullDownDisable
     ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel, 
        PORTS_DATA_MASK  mask 
     );
 
// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChannelChangeNoticeEnable
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel,
        PORTS_DATA_MASK  mask 
    );

  Summary:
    Enables CN interrupt for the selected pins of a channel.

  Description:
    This function enables Change Notice interrupt for the selected port
    pins of a channel.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    mask            - Identifies the pins for which change notification is
                      to be enabled 

  Returns:
    None.

  Example:
    <code>
    
    // Enable CN interrupt for RC5, RC8 and RC13 pins
    PLIB_PORTS_ChannelChangeNoticeEnable(PORTS_ID_0, PORT_CHANNEL_C, 0x2120);
    
    </code>

  Remarks:
    This function is only available in devices with PPS. For Non-PPS devices, use the
    PLIB_PORTS_CnPinsEnable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticeIntPerPort in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_ChannelChangeNoticeEnable
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel,
        PORTS_DATA_MASK  mask 
    );

// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChannelChangeNoticeDisable
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel,
        PORTS_DATA_MASK  mask 
    );

  Summary:
    Disables CN interrupt for the selected pins of a channel.

  Description:
    This function Disables Change Notice interrupt for the selected port
    pins of a channel.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    mask            - Identifies the pins for which change notification is
                      to be disabled 

  Returns:
    None.

  Example:
    <code>
    
    // Disable CN interrupt for RC5, RC8 and RC13 pins
    PLIB_PORTS_ChannelChangeNoticeDisable(PORTS_ID_0, PORT_CHANNEL_C, 0x2120);
    
    </code>

  Remarks:
	This function is only available in devices with PPS. For Non-PPS devices, use the
    PLIB_PORTS_CnPinsDisable function.
    
    This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticeIntPerPort in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_ChannelChangeNoticeDisable
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel,
        PORTS_DATA_MASK  mask 
    );

// *****************************************************************************
/* Function:
    void PLIB_PORTS_AnPinsModeSelect
     ( 
         PORTS_MODULE_ID  index,
         PORTS_AN_PIN   anPins,
         PORTS_PIN_MODE mode
     );

  Summary:
    Enables the selected AN pins as analog or digital.

  Description:
    This function enables the selected AN pins as analog or digital.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    anPins          - AN pins whose mode is to be changed. Multiple AN pins
                      can be ORed. 
    mode            - Possible values of PORTS_PIN_MODE (Analog or Digital)

  Returns:
    None.

  Example:
    <code>
    
    // Make pins AN5, AN8 and AN13 Analog
    PLIB_PORTS_AnPinsModeSelect(PORTS_ID_0, PORTS_AN_PIN_5 | 
	                            PORTS_AN_PIN_8 | 
								PORTS_AN_PIN_13, 
								PORTS_PIN_MODE_ANALOG);
                                                                
    </code>

  Remarks:
    This function is only available in devices without PPS feature. For PPS
    devices, use the PLIB_PORTS_ChannelModeSelect function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsAnPinsMode in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_AnPinsModeSelect
     ( 
         PORTS_MODULE_ID  index,
         PORTS_AN_PIN   anPins,
         PORTS_PIN_MODE mode
     );

// *****************************************************************************
/* Function:
    void PLIB_PORTS_CnPinsPullUpEnable
     ( 
        PORTS_MODULE_ID  index, 
        PORTS_CN_PIN   cnPins
     );

  Summary:
    Enables Change Notice pull-up for the selected channel pins.

  Description:
    This function enables the Change Notice pull-up for the selected
    channel pins.
    This function performs atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    cnPins          - CN pins whose pull-up is to be enabled. Multiple CN pins
                      can be ORed.

  Returns:
    None.

  Example:
    <code>
    
    // Enable pull-up for CN5, CN8 and CN13 pins
    PLIB_PORTS_CnPinsPullUpEnable(PORTS_ID_0, CHANGE_NOTICE_PIN_5 | 
	                                          CHANGE_NOTICE_PIN_8 | 
											  CHANGE_NOTICE_PIN_13);
    
    </code>

  Remarks:
    This function is only available in devices without PPS feature. For PPS
    devices, use the PLIB_PORTS_ChannelChangeNoticePullUpEnable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePullUp in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_CnPinsPullUpEnable
     ( 
        PORTS_MODULE_ID  index, 
        PORTS_CN_PIN   cnPins
     );

// *****************************************************************************
/* Function:
    void PLIB_PORTS_CnPinsPullUpDisable
    ( 
        PORTS_MODULE_ID  index, 
        PORTS_CN_PIN   cnPins
    );

  Summary:
    Disables Change Notice pull-up for the selected channel pins.

  Description:
    This function Disables the Change Notice pull-up for the selected
    channel pins.
    This function performs atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    cnPins          - CN pins whose pull-up is to be disabled. Multiple CN pins
                      can be ORed.

  Returns:
    None.

  Example:
    <code>
    
    // Disable pull-up for CN5, CN8 and CN13 pins
    PLIB_PORTS_CnPinsPullUpDisable(PORTS_ID_0, 
	                               CHANGE_NOTICE_PIN_5 | 
								   CHANGE_NOTICE_PIN_8 | 
								   CHANGE_NOTICE_PIN_13);
    
    </code>

  Remarks:
    This function is only available in devices without PPS feature. For PPS
    devices, use the PLIB_PORTS_ChannelChangeNoticePullUpDisable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePullUp in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_CnPinsPullUpDisable
     ( 
        PORTS_MODULE_ID  index, 
        PORTS_CN_PIN   cnPins
     );

// *****************************************************************************
/* Function:
    void PLIB_PORTS_CnPinsEnable
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CN_PIN   cnPins
    );

  Summary:
    Enables CN interrupt for the selected pins of a channel.

  Description:
    This function enables Change Notice interrupt for the selected port
    pins of a channel.
    This function performs atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    cnPins          - CN pins to be enabled. Multiple CN pins can be ORed.

  Returns:
    None.

  Example:
    <code>
    
    // Enable CN interrupt for CN5, CN8 and CN13 pins
    PLIB_PORTS_CnPinsEnable(PORTS_ID_0, 
	                        CHANGE_NOTICE_PIN_5 | 
							CHANGE_NOTICE_PIN_8 | 
							CHANGE_NOTICE_PIN_13);
    
    </code>

  Remarks:
    This function is only available in devices without PPS feature. For PPS
    devices, use the PLIB_PORTS_ChannelChangeNoticeEnable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPinChangeNotice in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_CnPinsEnable
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CN_PIN   cnPins
    );

// *****************************************************************************
/* Function:
    void PLIB_PORTS_CnPinsDisable
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CN_PIN   cnPins
    );

  Summary:
    Disables CN interrupt for the selected pins of a channel.

  Description:
    This function Disables Change Notice interrupt for the selected port
    pins of a channel.
    This function performs atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    cnPins          - CN pins to be disabled. Multiple CN pins can be ORed.

  Returns:
    None.

  Example:
    <code>
    
    // Disable CN interrupt for CN5, CN8 and CN13 pins
    PLIB_PORTS_CnPinsDisable(PORTS_ID_0, 
	                         CHANGE_NOTICE_PIN_5 | 
							 CHANGE_NOTICE_PIN_8 | 
							 CHANGE_NOTICE_PIN_13);
    
    </code>

  Remarks:
    This function is only available in devices without PPS feature. For PPS
    devices, use the PLIB_PORTS_ChannelChangeNoticeDisable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPinChangeNotice in your application to
    determine whether this feature is available.
*/

void PLIB_PORTS_CnPinsDisable
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CN_PIN   cnPins 
    );
    
// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChangeNoticeEnable( PORTS_MODULE_ID index )

  Summary:
    Global Change Notice enable.

  Description:
    This function enables the global Change Notice feature.
    This function performs atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    
    // Enable Change Notification
    PLIB_PORTS_ChangeNoticeEnable(PORTS_ID_0);
    
    </code>

  Remarks:
    This function is only available in devices without PPS. For PPS devices, use the
    PLIB_PORTS_ChangeNoticePerPortTurnOn function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNotice in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_ChangeNoticeEnable( PORTS_MODULE_ID index );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChangeNoticeDisable( PORTS_MODULE_ID index )

  Summary:
    Global Change Notice disable.

  Description:
    This function disables the global Change Notice feature.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    
    // Disable Change Notification
    PLIB_PORTS_ChangeNoticeDisable(PORTS_ID_0);
    
    </code>

  Remarks:
    This function is only available in devices without PPS. For PPS devices, use the
    PLIB_PORTS_ChangeNoticePerPortTurnOff function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNotice in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_ChangeNoticeDisable( PORTS_MODULE_ID index );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_PinChangeNoticeEnable( PORTS_MODULE_ID         index,
                                           PORTS_CHANGE_NOTICE_PIN pinNum )

  Summary:
    Port pin Change Notice interrupt enable.

  Description:
    This function enables the port pin Change Notice feature.
    This function performs atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    pinNum          - Possible values of PORTS_CHANGE_NOTICE_PIN

  Returns:
    None.

  Example:
    <code>
    
    // Enable Change Notice interrupt for pin CN13
    PLIB_PORTS_PinChangeNoticeEnable(PORTS_ID_0, CN13);
    
    </code>

  Remarks:
    This function is only available in devices without PPS. For PPS devices, use the
    PLIB_PORTS_PinChangeNoticePerPortEnable function.
    
 	This feature may not be available on all devices. Please refer to the
 	specific device data sheet to determine availability or use
 	PLIB_PORTS_ExistsPinChangeNotice in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_PinChangeNoticeEnable( PORTS_MODULE_ID         index,
                                       PORTS_CHANGE_NOTICE_PIN pinNum );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_PinChangeNoticeDisable( PORTS_MODULE_ID         index,
                                            PORTS_CHANGE_NOTICE_PIN pinNum )

  Summary:
    Port pin Change Notice disable.

  Description:
    This function disables the port pin Change Notice feature.
    This function performs atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    pinNum          - Possible values of PORTS_CHANGE_NOTICE_PIN

  Returns:
    None.

  Example:
    <code>
    
    // Disable Change Notice interrupt for pin CN13
    PLIB_PORTS_PinChangeNoticeDisable(PORTS_ID_0, CN13);
    
    </code>

  Remarks:
    This function is only available in devices without PPS. For PPS devices, use the
    PLIB_PORTS_PinChangeNoticePerPortDisable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsPinChangeNotice in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_PinChangeNoticeDisable( PORTS_MODULE_ID         index,
                                        PORTS_CHANGE_NOTICE_PIN pinNum );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChangeNoticeInIdleEnable( PORTS_MODULE_ID index )

  Summary:
    CPU Idle mode does not affect Change Notice operation.

  Description:
    This function makes sure that Change Notice feature continues working in
    Idle mode.    
    This function performs atomic register access.
    
  Precondition:
    None.

  Parameters:
    index    - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    
    // Change notification feature will be working even when CPU goes to
    // Idle mode
    PLIB_PORTS_ChangeNoticeInIdleEnable(PORTS_ID_0);
    
    </code>

  Remarks:
    This function is only available in devices without PPS. For PPS devices, use the
    PLIB_PORTS_ChangeNoticeInIdlePerPortEnable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticeInIdle in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_ChangeNoticeInIdleEnable( PORTS_MODULE_ID index );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChangeNoticeInIdleDisable( PORTS_MODULE_ID index )

  Summary:
    CPU Idle halts the Change Notice operation.

  Description:
    This function halts the Change Notice operation when the CPU enters
    Idle mode.
    This function performs atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>

    // Halts the Change notification operation when CPU enters Idle mode
    PLIB_PORTS_ChangeNoticeInIdleDisable(PORTS_ID_0);
    
    </code>

  Remarks:
    This function is only available in devices without PPS. For PPS devices, use the
    PLIB_PORTS_ChangeNoticeInIdlePerPortDisable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticeInIdle in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_ChangeNoticeInIdleDisable( PORTS_MODULE_ID index );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChangeNoticePullUpEnable( PORTS_MODULE_ID         index,
                                              PORTS_CHANGE_NOTICE_PIN pinNum )

  Summary:
    Enable pull-up on input change.

  Description:
    This function enables pull-up on selected input change notification pin.
    This function performs atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    pinNum          - Possible values of PORTS_CHANGE_NOTICE_PIN

  Returns:
    None.

  Example:
    <code>
    
    // Enable pull-up on pin CN13
    PLIB_PORTS_ChangeNoticePullUpEnable(PORTS_ID_0, CN13);
    
    </code>

  Remarks:
    This function is only available in devices without PPS. For PPS devices, use the
    PLIB_PORTS_ChangeNoticePullUpPerPortEnable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePullUp in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_ChangeNoticePullUpEnable( PORTS_MODULE_ID         index,
                                          PORTS_CHANGE_NOTICE_PIN pinNum );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChangeNoticePullUpDisable( PORTS_MODULE_ID         index,
                                               PORTS_CHANGE_NOTICE_PIN pinNum )

  Summary:
    Disable pull-up on input change.

  Description:
    This function disables pull-up on selected input change notification pin.
    This function performs atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    pinNum          - Possible values of PORTS_CHANGE_NOTICE_PIN

  Returns:
    None.

  Example:
    <code>
    
    // Disable pull-up on pin CN13
    PLIB_PORTS_ChangeNoticePullUpDisable(PORTS_ID_0, CN13);
    
    </code>

  Remarks:
    This function is only available in devices without PPS. For PPS devices, use the
    PLIB_PORTS_ChangeNoticePullUpPerPortDisable function.
    
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticePullUp in your application to determine whether
	this feature is available.
*/

void PLIB_PORTS_ChangeNoticePullUpDisable( PORTS_MODULE_ID         index,
                                           PORTS_CHANGE_NOTICE_PIN pinNum );


// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChannelSlewRateSelect
    ( 
        PORTS_MODULE_ID  index,
	    PORTS_CHANNEL channel,
        PORTS_DATA_MASK   channelMask,
        PORTS_PIN_SLEW_RATE slewRate
    );

  Summary:
    Selects the slew rate for selected channel pins.

  Description:
    This function selects the slew rate for selected channel pins.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    channelMask     - Identifies the pins for which slew rate has to be modified.
                      Slew rate of the pins which corresponding bit is "1" get
                      modified, slew rate of the other pins remains the same.
    slewRate        - One of the possible values of PORTS_PIN_SLEW_RATE.

  Returns:
    None.

  Example:
    <code>
    
    // Make slew rate of pins RC5, RC8 and RC13 slowest
    PLIB_PORTS_ChannelSlewRateSelect(PORTS_ID_0, 
	                                 PORT_CHANNEL_C, 
									 0x2120, 
									 PORTS_PIN_SLEW_RATE_SLOWEST);
                                                                
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsSlewRateControl in your application to determine
    whether this feature is available.
*/

void PLIB_PORTS_ChannelSlewRateSelect
    ( 
        PORTS_MODULE_ID  index,
	    PORTS_CHANNEL channel,
        PORTS_DATA_MASK   channelMask,
        PORTS_PIN_SLEW_RATE slewRate
    );
 
// *****************************************************************************
/* Function:
PORTS_PIN_SLEW_RATE PLIB_PORTS_PinSlewRateGet
    ( 
        PORTS_MODULE_ID  index,
	    PORTS_CHANNEL channel,
        PORTS_BIT_POS bitPos
    );

  Summary:
    Gets the slew rate for selected port pin.

  Description:
    This function gets the slew rate of selected port pin.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    bitPos          - One of the possible values of PORTS_BIT_POS.

  Returns:
    One of the possible values of PORTS_PIN_SLEW_RATE. 

  Example:
    <code>
    PORTS_PIN_SLEW_RATE slewRate;
    
    // Get the slew rate of pin RC1
    slewRate = PLIB_PORTS_PinSlewRateGet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_1);
                                                                
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsSlewRateControl in your application to determine
    whether this feature is available.
*/

PORTS_PIN_SLEW_RATE PLIB_PORTS_PinSlewRateGet
    ( 
        PORTS_MODULE_ID  index,
	    PORTS_CHANNEL channel,
        PORTS_BIT_POS bitPos
    );

// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChannelChangeNoticeMethodSelect
    ( 
        PORTS_MODULE_ID  index,
	    PORTS_CHANNEL channel,
        PORTS_CHANGE_NOTICE_METHOD changeNoticeMethod
    );

  Summary:
    Selects the Change Notice style for selected port channel.

  Description:
    This function selects the Change Notice style (or method) for selected port
    channel. It allows user to select whether the Change Notice detection will
    happen based on edge transition or level transition on all the CN pins of a
    particular channel.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index                - Identifier for the device instance to be configured
    channel              - Port pin channel
    changeNoticeMethod   - One of the possible values of PORTS_CHANGE_NOTICE_METHOD.

  Returns:
    None.

  Example:
    <code> 
    PLIB_PORTS_ChannelChangeNoticeMethodSelect(PORTS_ID_0, PORT_CHANNEL_C, 
                                            PORTS_CHANGE_NOTICE_METHOD_EDGE_DETECT);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChannelChangeNoticeMethod in your application to determine
    whether this feature is available.
*/

void PLIB_PORTS_ChannelChangeNoticeMethodSelect
    ( 
        PORTS_MODULE_ID  index,
	    PORTS_CHANNEL channel,
        PORTS_CHANGE_NOTICE_METHOD changeNoticeMethod
    );
 
// *****************************************************************************
/* Function:
    PORTS_CHANGE_NOTICE_METHOD PLIB_PORTS_ChannelChangeNoticeMethodGet
    ( 
        PORTS_MODULE_ID  index,
	    PORTS_CHANNEL channel
    );

  Summary:
    Gets the Change Notice style for the selected port channel.

  Description:
    This function gets the Change Notice style (or method) for the selected port
    channel.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel

  Returns:
    One of the possible values of PORTS_CHANGE_NOTICE_METHOD.

  Example:
    <code>
    PORTS_CHANGE_NOTICE_METHOD changeNoticeMethod;
    changeNoticeMethod = PLIB_PORTS_ChannelChangeNoticeMethodGet(PORTS_ID_0, PORT_CHANNEL_C);                                                            
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChannelChangeNoticeMethod in your application to determine
    whether this feature is available.
*/

PORTS_CHANGE_NOTICE_METHOD PLIB_PORTS_ChannelChangeNoticeMethodGet
    ( 
        PORTS_MODULE_ID  index,
	    PORTS_CHANNEL channel
    );
 
// *****************************************************************************
/* Function:
	void PLIB_PORTS_ChannelChangeNoticeEdgeEnable 
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel,
        PORTS_DATA_MASK   edgeRisingMask,
        PORTS_DATA_MASK   edgeFallingMask 
    );

  Summary:
    Enables selected type of edge for selected CN pins.

  Description:
    This function Enables selected type of edge (falling or rising) for
    selected CN pins of a port channel.
    This function does not perform atomic register access.

  Precondition:
    Change Notice method should be selected as
    "PORTS_CHANGE_NOTICE_METHOD_EDGE_DETECT" using
    PLIB_PORTS_ChannelChangeNoticeMethodSelect before using this function.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    edgeRisingMask  - Identifies the pins for which Change Notice has to be
                      enabled for rising edge. Change Notice interrupt at rising
                      edge is enabled for the pins which corresponding bit is
                      '1', for the other pins it remains the same.
    edgeFallingMask - Identifies the pins for which Change Notice has to be
                      enabled for falling edge. Change Notice interrupt at
                      falling edge is enabled for the pins which corresponding
                      bit is '1', for the other pins it remains the same.

  Returns:
    None.

  Example:
    <code>
    // Enable Change Notice at rising edge for RC1 pin and at falling edge for
    // RC1 & RC5 pins.
    PLIB_PORTS_ChannelChangeNoticeEdgeEnable(PORTS_ID_0, PORT_CHANNEL_C, 0x0002, 0x0022);
    </code>

  Remarks:
	This feature is not available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticeEdgeControl in your application to determine
    whether this feature is available.
*/

void PLIB_PORTS_ChannelChangeNoticeEdgeEnable 
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel,
        PORTS_DATA_MASK   edgeRisingMask,
        PORTS_DATA_MASK   edgeFallingMask 
    );   

// *****************************************************************************
/* Function:
    void PLIB_PORTS_ChannelChangeNoticeEdgeDisable 
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel,
        PORTS_DATA_MASK   edgeRisingMask,
        PORTS_DATA_MASK   edgeFallingMask 
    );

  Summary:
    Disables selected type of edge for selected CN pins.

  Description:
    This function Disables selected type of edge (falling or rising) for
    selected CN pins of a port channel.
    This function does not perform atomic register access.

  Precondition:
    Change Notice method should be selected as
    "PORTS_CHANGE_NOTICE_METHOD_EDGE_DETECT" using
    PLIB_PORTS_ChannelChangeNoticeMethodSelect before using this function.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    edgeRisingMask  - Identifies the pins for which Change Notice has to be
                      enabled for rising edge. Change notice interrupt at rising
                      edge is enabled for the pins which corresponding bit is
                      '1', for the other pins it remains the same.
    edgeFallingMask - Identifies the pins for which Change Notice has to be
                      enabled for falling edge. Change notice interrupt at
                      falling edge is enabled for the pins which corresponding
                      bit is '1', for the other pins it remains the same.

  Returns:
    None.

  Example:
    <code>
    // Disable Change Notice at rising edge for RC1 pin and at falling edge for
    // RC1 & RC5 pins.
    PLIB_PORTS_ChannelChangeNoticeEdgeDisable(PORTS_ID_0, PORT_CHANNEL_C, 0x0002, 0x0022);
    </code>

  Remarks:
	This feature is not available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticeEdgeControl in your application to determine
    whether this feature is available.
*/

void PLIB_PORTS_ChannelChangeNoticeEdgeDisable 
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel,
        PORTS_DATA_MASK   edgeRisingMask,
        PORTS_DATA_MASK   edgeFallingMask 
    );

// *****************************************************************************
/* Function:
    bool PLIB_PORTS_PinChangeNoticeEdgeIsEnabled 
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel,
        PORTS_BIT_POS bitPos,
        PORTS_CHANGE_NOTICE_EDGE  cnEdgeType
    );

  Summary:
    Check if Change Notice edge is enabled or not.
    This function does not perform atomic register access.

  Description:
    This function checks if selected type of Change Notice edge is enabled on a
    particular port pin or not.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    bitPos          - One of the possible values of PORTS_BIT_POS.
    cnEdgeType      - Type of the edge which has to be checked.
    
  Returns:
    - true  - Selected type of Change Notice Edge is enabled.
    - false - Selected type of Change Notice Edge is not enabled.
     
  Example:
    <code>
    // Check if Change Notice at rising edge is enabled or not for pin RC1.
    if (PLIB_PORTS_PinChangeNoticeEdgeIsEnabled(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_1, PORTS_CHANGE_NOTICE_EDGE_RISING))
    {
        // do something
    }
    </code>

  Remarks:
	This feature is not available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticeEdgeControl in your application to determine
    whether this feature is available.
*/

bool PLIB_PORTS_PinChangeNoticeEdgeIsEnabled 
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel,
        PORTS_BIT_POS bitPos,
        PORTS_CHANGE_NOTICE_EDGE  cnEdgeType
    );
 
// *****************************************************************************
/* Function:
    bool PLIB_PORTS_PinChangeNoticeEdgeHasOccurred 
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel,
        PORTS_BIT_POS bitPos
    );

  Summary:
    Check Change Notice edge status.

  Description:
    This function checks whether or no a Change Notice edge transition has occurred 
    on the selected port pin.
    This function does not perform atomic register access.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Port pin channel
    bitPos          - One of the possible values of PORTS_BIT_POS
    
  Returns:
    - true  - Change Notice edge transition has occurred
    - false - Change Notice edge transition has not occurred
     
  Example:
    <code>
    // Check if Change Notice edge transition has occurred for pin RC1.
    if (PLIB_PORTS_PinChangeNoticeEdgeHasOccurred(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_1))
    {
        // do something
    }
    </code>

  Remarks:
	This feature is not available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_PORTS_ExistsChangeNoticeEdgeStatus in your application to determine
    whether this feature is available.
*/

bool PLIB_PORTS_PinChangeNoticeEdgeHasOccurred
    ( 
        PORTS_MODULE_ID  index,
        PORTS_CHANNEL channel,
        PORTS_BIT_POS bitPos
    );
    
// *****************************************************************************
// *****************************************************************************
// Section: Ports Peripheral Library Exists Functions
// *****************************************************************************
// *****************************************************************************
/* The functions below indicate the existence of the features on the device. 
*/

//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsRemapInput( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the RemapInput feature exists on the Ports module. 

  Description:
    This function identifies whether the RemapInput feature is available on the 
	Ports module.
    When this function returns true, this function is supported on the device: 
    - PLIB_PORTS_RemapInput

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The RemapInput feature is supported on the device
    - false  - The RemapInput feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsRemapInput( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsRemapOutput( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the RemapOutput feature exists on the Ports module. 

  Description:
    This function identifies whether the RemapOutput feature is available on the 
	Ports module.
    When this function returns true, this function is supported on the device: 
    - PLIB_PORTS_RemapOutput

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The RemapOutput feature is supported on the device
    - false  - The RemapOutput feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsRemapOutput( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsPinMode( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the PinMode feature exists on the Ports module. 

  Description:
    This function identifies whether the PinMode (Analog Pin or Digital Pin)
    feature is available on the Ports module.
    When this function returns true, this function is supported on the device: 
    - PLIB_PORTS_PinModeSelect

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The PinMode feature is supported on the device
    - false  - The PinMode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsPinMode( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsAnPinsMode( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the AnPinsMode feature exists on the Ports module. 

  Description:
    This function identifies whether the AnPinsMode feature is available on the 
	Ports module.
    When this function returns true, this function is supported on the device: 
    - PLIB_PORTS_AnPinsModeSelect

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The AnPinsMode feature is supported on the device
    - false  - The AnPinsMode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsAnPinsMode( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsPortsRead( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the PortsRead feature exists on the Ports module. 

  Description:
    This function identifies whether the PortsRead feature is available on the 
	Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_PinGet
    - PLIB_PORTS_Read

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The PortsRead feature is supported on the device
    - false  - The PortsRead feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsPortsRead( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsLatchRead( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the LatchRead feature exists on the Ports module. 

  Description:
    This function identifies whether the LatchRead feature is available on the 
	Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_PinGetLatched
    - PLIB_PORTS_ReadLatched

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The LatchRead feature is supported on the device
    - false  - The LatchRead feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsLatchRead( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsPortsWrite( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the PortsWrite feature exists on the Ports module. 

  Description:
    This function identifies whether the PortsWrite feature is available on the 
	Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_PinWrite
    - PLIB_PORTS_PinSet
    - PLIB_PORTS_PinClear
    - PLIB_PORTS_PinToggle
    - PLIB_PORTS_Write
    - PLIB_PORTS_Set
    - PLIB_PORTS_Toggle
    - PLIB_PORTS_Clear

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The PortsWrite feature is supported on the device
    - false  - The PortsWrite feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsPortsWrite( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsPortsDirection( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the PortsDirection feature exists on the Ports module. 

  Description:
    This function identifies whether the PortsDirection feature is available on the 
	Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_PinDirectionInputSet
    - PLIB_PORTS_PinDirectionOutputSet
    - PLIB_PORTS_DirectionInputSet
    - PLIB_PORTS_DirectionOutputSet
    - PLIB_PORTS_DirectionGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The PortsDirection feature is supported on the device
    - false  - The PortsDirection feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsPortsDirection( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsPortsOpenDrain( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the PortsOpenDrain feature exists on the Ports module. 

  Description:
    This function identifies whether the PortsOpenDrain feature is available on 
	the Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_PinOpenDrainEnable
    - PLIB_PORTS_PinOpenDrainDisable
    - PLIB_PORTS_OpenDrainEnable
    - PLIB_PORTS_OpenDrainDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The PortsOpenDrain feature is supported on the device
    - false  - The PortsOpenDrain feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsPortsOpenDrain( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsChangeNotice( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the ChangeNotice feature exists on the Ports module. 

  Description:
    This function identifies whether the ChangeNotice feature is available on the 
	Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_ChangeNoticeEnable
    - PLIB_PORTS_ChangeNoticeDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChangeNotice feature is supported on the device
    - false  - The ChangeNotice feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsChangeNotice( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsPinChangeNotice( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the PinChangeNotice feature exists on the Ports module. 

  Description:
    This function identifies whether the PinChangeNotice feature is available on 
	the Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_PinChangeNoticeEnable
    - PLIB_PORTS_PinChangeNoticeDisable
    - PLIB_PORTS_CnPinsEnable
    - PLIB_PORTS_CnPinsDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The PinChangeNotice feature is supported on the device
    - false  - The PinChangeNotice feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsPinChangeNotice( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsChangeNoticeInIdle( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the ChangeNoticeInIdle feature exists on the Ports module. 

  Description:
    This function identifies whether the ChangeNoticeInIdle feature is available 
	on the Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_ChangeNoticeInIdleEnable
    - PLIB_PORTS_ChangeNoticeInIdleDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChangeNoticeInIdle feature is supported on the device
    - false  - The ChangeNoticeInIdle feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsChangeNoticeInIdle( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsChangeNoticePullUp( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the ChangeNoticePullup feature exists on the Ports module. 

  Description:
    This function identifies whether the ChangeNoticePullup feature is available 
	on the Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_ChangeNoticePullUpEnable
    - PLIB_PORTS_ChangeNoticePullUpDisable
    - PLIB_PORTS_CnPinsPullUpEnable
    - PLIB_PORTS_CnPinsPullUpDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChangeNoticePullup feature is supported on the device
    - false  - The ChangeNoticePullup feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsChangeNoticePullUp( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsPinModePerPort( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the PinModePerPort feature exists on the Ports module. 

  Description:
    This function identifies whether the PinModePerPort (Analog Pin or Digital Pin)
    feature is available on the Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_PinModePerPortSelect
    - PLIB_PORTS_ChannelModeSelect

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The PinModePerPort feature is supported on the device
    - false  - The PinModePerPort feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsPinModePerPort( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsChangeNoticePullDownPerPort( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the ChangeNoticePullDownPerPort feature exists on the 
	Ports module. 

  Description:
    This function identifies whether the ChangeNoticePullDownPerPort feature is 
	available on the Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_ChangeNoticePullDownPerPortEnable
    - PLIB_PORTS_ChangeNoticePullDownPerPortDisable
    - PLIB_PORTS_ChannelChangeNoticePullDownEnable
    - PLIB_PORTS_ChannelChangeNoticePullDownDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChangeNoticePullDownPerPort feature is supported on the device
    - false  - The ChangeNoticePullDownPerPort feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsChangeNoticePullDownPerPort( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsChangeNoticePullUpPerPort( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the ChangeNoticePullUpPerPort feature exists on the Ports 
	module. 

  Description:
    This function identifies whether the ChangeNoticePullUpPerPort feature is available 
	on the Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_ChangeNoticePullUpPerPortEnable
    - PLIB_PORTS_ChangeNoticePullUpPerPortDisable
    - PLIB_PORTS_ChannelChangeNoticePullUpEnable
    - PLIB_PORTS_ChannelChangeNoticePullUpDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChangeNoticePullUpPerPort feature is supported on the device
    - false  - The ChangeNoticePullUpPerPort feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsChangeNoticePullUpPerPort( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsPinChangeNoticePerPort( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the PinChangeNoticePerPort feature exists on the Ports 
	module. 

  Description:
    This function identifies whether the PinChangeNoticePerPort feature is 
	available on the Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_PinChangeNoticePerPortEnable
    - PLIB_PORTS_PinChangeNoticePerPortDisable
    - PLIB_PORTS_ChannelChangeNoticeEnable
    - PLIB_PORTS_ChannelChangeNoticeDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The PinChangeNoticePerPort feature is supported on the device
    - false  - The PinChangeNoticePerPort feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsPinChangeNoticePerPort( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsChangeNoticePerPortTurnOn( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the ChangeNoticePerPortTurnOn feature exists on the Ports 
	module. 

  Description:
    This function identifies whether the ChangeNoticePerPortTurnOn feature is 
	available on the Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_ChangeNoticePerPortTurnOn
    - PLIB_PORTS_ChangeNoticePerPortTurnOff

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChangeNoticePerPortTurnOn feature is supported on the device
    - false  - The ChangeNoticePerPortTurnOn feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsChangeNoticePerPortTurnOn( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsChangeNoticePerPortInIdle( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the ChangeNoticeInIdlePerPort feature exists on the Ports 
	module.

  Description:
    This function identifies whether the ChangeNoticeInIdlePerPort feature is 
	available on the Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_ChangeNoticeInIdlePerPortEnable
    - PLIB_PORTS_ChangeNoticeInIdlePerPortDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChangeNoticeInIdlePerPort feature is supported on the device
    - false  - The ChangeNoticeInIdlePerPort feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsChangeNoticePerPortInIdle( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsChangeNoticePerPortStatus( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the ChangeNoticePerPortStatus feature exists on the Ports 
	module. 

  Description:
    This function identifies whether the ChangeNoticePerPortStatus feature is 
	available on the Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_ChangeNoticePerPortHasOccured
    - PLIB_PORTS_ChangeNoticePerPortHasOccurred

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChangeNoticePerPortStatus feature is supported on the device
    - false  - The ChangeNoticePerPortStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsChangeNoticePerPortStatus( PORTS_MODULE_ID index );

//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsSlewRateControl( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the SlewRateControl feature exists on the Ports module. 

  Description:
    This function identifies whether the SlewRateControl feature is available 
	on the Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_ChannelSlewRateSelect
    - PLIB_PORTS_PinSlewRateGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The SlewRateControl feature is supported on the device
    - false  - The SlewRateControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsSlewRateControl( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsChannelChangeNoticeMethod( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the ChannelChangeNoticeMethod feature exists on the 
	Ports module. 

  Description:
    This function identifies whether the ChannelChangeNoticeMethod feature is 
	available on the Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_ChannelChangeNoticeMethodSelect
    - PLIB_PORTS_ChannelChangeNoticeMethodGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelChangeNoticeMethod feature is supported on the device
    - false  - The ChannelChangeNoticeMethod feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsChannelChangeNoticeMethod( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsChangeNoticeEdgeControl( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the ChangeNoticeEdgeControl feature exists on the Ports 
	module. 

  Description:
    This function identifies whether the ChangeNoticeEdgeControl feature is 
	available on the Ports module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_PORTS_ChannelChangeNoticeEdgeEnable
    - PLIB_PORTS_ChannelChangeNoticeEdgeDisable
    - PLIB_PORTS_PinChangeNoticeEdgeIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChangeNoticeEdgeControl feature is supported on the device
    - false  - The ChangeNoticeEdgeControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsChangeNoticeEdgeControl( PORTS_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_PORTS_ExistsChangeNoticeEdgeStatus( PORTS_MODULE_ID index )

  Summary:
    Identifies whether the ChangeNoticeEdgeStatus feature exists on the Ports 
	module. 

  Description:
    This function identifies whether the ChangeNoticeEdgeStatus feature is available 
	on the Ports module.
    When this function returns true, this function is supported on the device: 
    - PLIB_PORTS_PinChangeNoticeEdgeHasOccurred

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChangeNoticeEdgeStatus feature is supported on the device
    - false  - The ChangeNoticeEdgeStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_PORTS_ExistsChangeNoticeEdgeStatus( PORTS_MODULE_ID index );

#include "peripheral/ports/plib_ports_compatibility.h"

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _PLIB_PORTS_H
/*******************************************************************************
 End of File
*/