/*******************************************************************************
  Ports System Service Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_ports.h

  Summary:
    Ports System Service interface definitions

  Description:
    This file contains the interface definition for the Ports system
    service.  It provides a way to interact with the Ports subsystem to
    manage the timing requests supported by the system
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

#ifndef _SYS_PORTS_H
#define _SYS_PORTS_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#if defined(__PIC32C__)
#include "system/ports/sys_ports_definitions_pic32c.h"
#else
#include "system/ports/sys_ports_definitions.h"
#endif

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: SYS PorTS Module Initialization Routine
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SYS_PORTS_Initialize()

  Summary:
    Initializes PORT Pins/Channels.
	<p><b>Implementation:</b> Static/Dynamic</p>

  Description:
    This function initializes different port pins/channels to the desired state.
    It also remaps the pins to the desired specific function.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
	
	SYS_PORTS_Initialize();
	
    </code>

  Remarks:
    This API must be be called at the time of system initialization to
	initialize the ports pins. 
*/

void SYS_PORTS_Initialize();

// *****************************************************************************
// *****************************************************************************
// Section: SYS PORTS CONTROL Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    PORTS_DATA_TYPE SYS_PORTS_Read( PORTS_MODULE_ID index, PORTS_CHANNEL channel )

  Summary:
    Reads the data from the I/O port.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function reads the data from the I/O port.

  Preconditions:
    The direction of the port to be set as input.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.

  Returns:
    Returns the data read from the port.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    PORTS_DATA_TYPE readData;
    readData = SYS_PORTS_Read( MY_PORTS_INSTANCE, MY_CHANNEL );
	</code>

  Remarks:
    None.
*/

PORTS_DATA_TYPE SYS_PORTS_Read( PORTS_MODULE_ID index, PORTS_CHANNEL channel );

// *****************************************************************************
/* Function:
    void SYS_PORTS_Write( PORTS_MODULE_ID index, 
                          PORTS_CHANNEL channel,
                          PORTS_DATA_TYPE value )

  Summary:
    Writes the data to the I/O port.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function writes the data to the I/O port.

  Preconditions:
    The direction of the port to be set as output.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    value           - Value to be written into a port of width PORTS_DATA_TYPE

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
	PORTS_DATA_TYPE writeData;
	SYS_PORTS_Write( MY_PORTS_INSTANCE, MY_CHANNEL, MY_VALUE );
	</code>

  Remarks:
    None.
*/

void SYS_PORTS_Write( PORTS_MODULE_ID index,
                      PORTS_CHANNEL channel,
                      PORTS_DATA_TYPE value );
     

// *****************************************************************************
/* Function:
    PORTS_DATA_TYPE SYS_PORTS_LatchedGet( PORTS_MODULE_ID index, PORTS_CHANNEL channel )

  Summary:
    Reads the data driven on the I/O port.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function reads the data driven on the I/O port.

  Preconditions:
    The direction of the port to be set as output.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.

  Returns:
    Returns the data driven on the port.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    PORTS_DATA_TYPE drivenData;
    drivenData = SYS_PORTS_LatchedGet( MY_PORTS_INSTANCE, MY_CHANNEL );
	</code>

  Remarks:
    None.
*/

PORTS_DATA_TYPE SYS_PORTS_LatchedGet( PORTS_MODULE_ID index, PORTS_CHANNEL channel );

     
// *****************************************************************************
/* Function:
    void SYS_PORTS_Set( PORTS_MODULE_ID index, PORTS_CHANNEL channel, 
                         PORTS_DATA_TYPE value,
                         PORTS_DATA_MASK mask )

  Summary:
    Sets the selected digital port/latch based on the mask.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function Sets the selected digital port/latch relative to the mask.
    
    This function "AND" value and mask parameters and then set the bits
    in the port channel that were set in the result of the ANDing operation.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    value           - Consists of information about which port bit has to be set
    mask            - Identifies the bits which could be intended for setting

  Returns:
    None.

  Example:
    <code>
    
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_VALUE - 0x1234
    PORTS_DATA_MASK myMask = (PORTS_DATA_MASK)0x00FF;
    
    // Set the MY_CHANNEL bit positions 2,4 and 5 (0x0034 = b0000 0000 0011 0100)
    SYS_PORTS_Set(MY_PORTS_INSTANCE, MY_CHANNEL, MY_VALUE, myMask);
    
    </code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_Set( PORTS_MODULE_ID index, PORTS_CHANNEL channel, 
                    PORTS_DATA_TYPE value,
                    PORTS_DATA_MASK mask );

// *****************************************************************************
/* Function:
    void SYS_PORTS_Clear ( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                           PORTS_DATA_MASK clearMask )

  Summary:
    Clears the selected digital port.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function clears the selected digital port.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    clearMask       - Identifies the bits to be cleared

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    PORTS_DATA_MASK clearMask = (PORTS_DATA_MASK)0x00FF;
    SYS_PORTS_Clear( MY_PORTS_INSTANCE, MY_CHANNEL, clearMask );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_Clear ( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                           PORTS_DATA_MASK clearMask );

// *****************************************************************************
/* Function:
    void SYS_PORTS_DirectionSelect( PORTS_MODULE_ID index, 
                                 SYS_PORTS_PIN_DIRECTION pinDir,
                                 PORTS_CHANNEL channel, 
                                 PORTS_DATA_MASK mask )

  Summary:
    Enables the direction for the selected port.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables the direction for the selected port.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    pinDir          - Pin direction
    channel         - Identifier for the PORT channel: A, B, C, etc.
    mask            - Mask for the direction of width PORTS_DATA_MASK

  Returns:
    None

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    SYS_PORTS_PIN_DIRECTION pinDir;
    pinDir = SYS_PORTS_DIRECTION_INPUT;
    PORTS_DATA_MASK myMask = (PORTS_DATA_MASK)0x00FF;
    SYS_PORTS_DirectionSelect(MY_PORTS_INSTANCE, pinDir, MY_CHANNEL, myMask );
	</code>

  Remarks:
    None.
*/

void SYS_PORTS_DirectionSelect( PORTS_MODULE_ID index, 
                             SYS_PORTS_PIN_DIRECTION pinDir, 
                             PORTS_CHANNEL channel, 
                             PORTS_DATA_MASK mask );

// *****************************************************************************
/* Function:
    PORTS_DATA_MASK SYS_PORTS_DirectionGet( PORTS_MODULE_ID index, PORTS_CHANNEL channel ) 

  Summary:
    Reads the direction for the selected port.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function reads the direction for the selected port.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.

  Returns:
    Direction of the port.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    PORTS_DATA_MASK value;
    value = SYS_PORTS_DirectionGet( MY_PORTS_INSTANCE, MY_CHANNEL );
	</code>

  Remarks:
    None.
*/

PORTS_DATA_MASK SYS_PORTS_DirectionGet( PORTS_MODULE_ID index, PORTS_CHANNEL channel );

// *****************************************************************************
/* Function:
    void SYS_PORTS_Toggle( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                           PORTS_DATA_MASK toggleMask )

  Summary:
    Toggles the selected digital port pins.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function toggles the selected digital port pins.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    toggleMask      - Identifies the bits to be toggled

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    PORTS_DATA_MASK toggleMask = (PORTS_DATA_MASK)0x00FF;	
    SYS_PORTS_Toggle( MY_PORTS_INSTANCE, MY_CHANNEL, toggleMask );
	</code>

  Remarks:
    None.
*/

void SYS_PORTS_Toggle( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                       PORTS_DATA_MASK toggleMask );

// *****************************************************************************
/* Function:
    void SYS_PORTS_OpenDrainEnable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                     PORTS_DATA_MASK mask )

  Summary:
    Enables the open-drain functionality for the selected port.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables the open-drain functionality for the selected port.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    mask            - Mask of type PORTS_DATA_MASK

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    SYS_PORTS_OpenDrainEnable( MY_PORTS_INSTANCE, MY_CHANNEL, (PORTS_DATA_MASK)0x00FF );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_OpenDrainEnable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                PORTS_DATA_MASK mask );

// *****************************************************************************
/* Function:
    void SYS_PORTS_OpenDrainDisable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                     PORTS_DATA_MASK mask )

  Summary:
    Disables the open-drain functionality for the selected port.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function disables the open-drain functionality for the selected port.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    mask            - Mask of type PORTS_DATA_MASK

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    SYS_PORTS_PeripheralOpenDrainDisable( MY_PORTS_INSTANCE, MY_CHANNEL, (PORTS_DATA_MASK)0x00FF );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_OpenDrainDisable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                 PORTS_DATA_MASK mask );

// *****************************************************************************
/* Function:
    PORTS_DATA_TYPE SYS_PORTS_InterruptStatusGet
    ( 
        PORTS_MODULE_ID index, 
        PORTS_CHANNEL channel 
    )

  Summary:
    Reads the interrupt status from the I/O port.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function reads the interrupt status from the I/O port.

  Preconditions:
    The interrupts should have been configured.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.

  Returns:
    Returns the interrupt status of the port channel.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    PORTS_DATA_TYPE channelStatus;
    channelStatus = SYS_PORTS_InterruptStatusGet( MY_PORTS_INSTANCE, MY_CHANNEL );
	</code>

  Remarks:
    None.
*/

PORTS_DATA_TYPE SYS_PORTS_InterruptStatusGet
( 
    PORTS_MODULE_ID index, 
    PORTS_CHANNEL channel 
);


// *****************************************************************************
// *****************************************************************************
// Section: SYS PORT PINS REMAPING Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SYS_PORTS_RemapInput( PORTS_MODULE_ID      index,
							  PORTS_REMAP_INPUT_FUNCTION function,
							  PORTS_REMAP_INPUT_PIN      remapPin )

  Summary:
    Input/Output (I/O) function remapping.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function controls the I/O function remapping.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    function        - One of the possible values of PORTS_REMAP_INPUT_FUNCTION
    remapPin        - One of the possible values of PORTS_REMAP_INPUT_PIN

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // Remapping input function 'Input Capture 1' to the Remappable pin 'RPD2'
    SYS_PORTS_RemapInput(MY_PORTS_INSTANCE, INPUT_FUNC_IC1, INPUT_PIN_RPD2 );
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	SYS_PORTS_ExistsRemapInputOutput in your application to determine whether
	this feature is available.
*/

void SYS_PORTS_RemapInput( PORTS_MODULE_ID      index,
							  PORTS_REMAP_INPUT_FUNCTION function,
							  PORTS_REMAP_INPUT_PIN      remapPin );


// *****************************************************************************
/* Function:
    void SYS_PORTS_RemapOutput( PORTS_MODULE_ID      index,
                                      PORTS_REMAP_OUTPUT_FUNCTION function,
                                      PORTS_REMAP_OUTPUT_PIN      remapPin )

  Summary:
    Input/Output (I/O) function remapping.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function controls the I/O function remapping.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    function        - One of the possible values of PORTS_REMAP_OUTPUT_FUNCTION
    remapPin        - One of the possible values of PORTS_REMAP_OUTPUT_PIN

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // Remapping output function 'UART3 Transmit' to the Remappable pin 'RPA14'
    SYS_PORTS_RemapInputOutput(MY_PORTS_INSTANCE, OUTPUT_FUNC_U3TX, OUTPUT_PIN_RPA14);
    </code>

  Remarks:
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	SYS_PORTS_ExistsRemapInputOutput in your application to determine whether
	this feature is available.
*/

void SYS_PORTS_RemapOutput( PORTS_MODULE_ID      index,
                                  PORTS_REMAP_OUTPUT_FUNCTION function,
                                  PORTS_REMAP_OUTPUT_PIN      remapPin );


// *****************************************************************************
// *****************************************************************************
// Section: SYS Change Notification Pins Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SYS_PORTS_ChangeNotificationGlobalEnable( PORTS_MODULE_ID index )

  Summary:
    Globally enables the change notification for the selected port.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function globally enables the change notification for the selected port.

  Preconditions:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    SYS_PORTS_ChangeNotificationGlobalEnable( MY_PORTS_INSTANCE );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_ChangeNotificationGlobalEnable( PORTS_MODULE_ID index );

// *****************************************************************************
/* Function:
    void SYS_PORTS_ChangeNotificationGlobalDisable( PORTS_MODULE_ID index );

  Summary:
    Globally disables the change notification for the selected port.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function globally disables the change notification for the selected port.

  Preconditions:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    SYS_PORTS_ChangeNotificationGlobalDisable( MY_PORTS_INSTANCE);
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_ChangeNotificationGlobalDisable( PORTS_MODULE_ID index );

// *****************************************************************************
/* Function:
    void SYS_PORTS_ChangeNotificationEnable( PORTS_MODULE_ID index, 
                                             PORTS_CHANGE_NOTICE_PIN pinNum, 
                                             SYS_PORTS_PULLUP_PULLDOWN_STATUS value )

  Summary:
    Enables the change notification for the selected port.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables the change notification for the selected port.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured  
    value           - Pull-up enable or disable value
    pinNum          - Possible values of PORTS_CHANGE_NOTICE_PIN

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    SYS_PORTS_PULLUP_PULLDOWN_STATUS value;
    PORTS_CHANGE_NOTICE_PIN pinNum;
    SYS_PORTS_ChangeNotificationEnable( index, pinNum, value );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_ChangeNotificationEnable( PORTS_MODULE_ID index, 
                                         PORTS_CHANGE_NOTICE_PIN pinNum, 
                                         SYS_PORTS_PULLUP_PULLDOWN_STATUS value );

// *****************************************************************************
/* Function:
    void SYS_PORTS_ChangeNotificationDisable( PORTS_MODULE_ID index, 
                                              PORTS_CHANGE_NOTICE_PIN pinNum )

  Summary:
    Disables the change notification for the selected port.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function disables the change notification for the selected port.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured  
    pinNum          - Possible values of PORTS_CHANGE_NOTICE_PIN

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    PORTS_CHANGE_NOTICE_PIN pinNum;
    SYS_PORTS_ChangeNotificationDisable( index, pinNum );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_ChangeNotificationDisable( PORTS_MODULE_ID index, 
                                          PORTS_CHANGE_NOTICE_PIN pinNum );

// *****************************************************************************
/* Function:
    void SYS_PORTS_ChangeNotificationInIdleModeEnable( PORTS_MODULE_ID index );

  Summary:
    Enables the change notification for the selected port in Sleep or Idle mode.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables the change notification for the selected port in Sleep
    or Idle mode.

  Preconditions:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    SYS_PORTS_ChangeNotificationInIdleModeEnable( MY_PORTS_INSTANCE );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_ChangeNotificationInIdleModeEnable( PORTS_MODULE_ID index );

// *****************************************************************************
/* Function:
    void SYS_PORTS_ChangeNotificationInIdleModeDisable( PORTS_MODULE_ID index);

  Summary:
    Disables the change notification for the selected port in Sleep or Idle mode.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function disables the change notification for the selected port in Sleep
    or Idle mode.

  Preconditions:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    SYS_PORTS_ChangeNotificationInIdleModeDisable( MY_PORTS_INSTANCE );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_ChangeNotificationInIdleModeDisable( PORTS_MODULE_ID index ); 

// *****************************************************************************
/* Function:
    void SYS_PORTS_ChangeNotificationPullUpEnable ( PORTS_MODULE_ID         index,
                                                    PORTS_CHANGE_NOTICE_PIN pinNum )

  Summary:
    Enables a weak pull-up on the change notification pin.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables a weak pull-up on the change notification pin.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    pinNum          - Possible values of PORTS_CHANGE_NOTICE_PIN

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_CHANGE_NOTICE_PIN_10
    SYS_PORTS_ChangeNotificationPullUpEnable( MY_PORTS_INSTANCE, MY_PINNUM );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_ChangeNotificationPullUpEnable ( PORTS_MODULE_ID         index,
                                                PORTS_CHANGE_NOTICE_PIN pinNum );

// *****************************************************************************
/* Function:
    void SYS_PORTS_ChangeNotificationPullUpDisable ( PORTS_MODULE_ID         index,
                                                    PORTS_CHANGE_NOTICE_PIN pinNum )

  Summary:
    Disables a weak pull-up on the change notification pin.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function Disables a weak pull-up on the change notification pin.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    pinNum          - Possible values of PORTS_CHANGE_NOTICE_PIN

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_CHANGE_NOTICE_PIN_10
    SYS_PORTS_ChangeNotificationPullUpDisable( MY_PORTS_INSTANCE, MY_PINNUM );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_ChangeNotificationPullUpDisable ( PORTS_MODULE_ID         index,
                                                PORTS_CHANGE_NOTICE_PIN pinNum );
                                                
// *****************************************************************************
// *****************************************************************************
// Section: SYS PORT PINS Control Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SYS_PORTS_PinModeSelect ( PORTS_MODULE_ID index, 
	                               PORTS_ANALOG_PIN pin, 
								   PORTS_PIN_MODE mode)

  Summary:
    Enables the selected pin as analog or digital.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables the selected pin as analog or digital.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    pin             - Possible values of PORTS_ANALOG_PIN
    mode            - Possible values of PORTS_PIN_MODE	

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PIN       - PORTS_ANALOG_PIN_AN0
    // MY_PIN_MODE  - PORTS_PIN_MODE_ANALOG
    SYS_PORTS_PinModSYS_PORTS_PinModeSelecteSelect( MY_PORTS_INSTANCE, MY_PIN, MY_PIN_MODE );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinModeSelect ( PORTS_MODULE_ID index, 
                               PORTS_ANALOG_PIN pin, 
							   PORTS_PIN_MODE mode);

// *****************************************************************************
/* Function:
    void SYS_PORTS_PinWrite ( PORTS_MODULE_ID index, 
                              PORTS_CHANNEL channel,
                              PORTS_BIT_POS bitPos
                              bool value )

  Summary:
    Writes the selected digital pin.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function writes the selected digital pin.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS
    value           - Value to be written to the specific pin/latch:
                      - true  - Sets the bit
                      - false - Clears the bit

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_PIN_10
    SYS_PORTS_PinWrite(MY_PORTS_INSTANCE, MY_CHANNEL, MY_PINNUM, value);
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinWrite ( PORTS_MODULE_ID index, 
                          PORTS_CHANNEL channel,
                          PORTS_BIT_POS bitPos,
                          bool value );


// *****************************************************************************
/* Function:
    bool SYS_PORTS_PinLatchedGet ( PORTS_MODULE_ID index, 
                             PORTS_CHANNEL channel,
                             PORTS_BIT_POS bitPos )

  Summary:
    Reads the data driven on the selected digital pin.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function reads the data driven on the selected 
    digital output pin.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    The status of the data driven on the port pin.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_PIN_10
    status = SYS_PORTS_PinLatchedGet(MY_PORTS_INSTANCE, MY_CHANNEL, MY_PINNUM);
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

bool SYS_PORTS_PinLatchedGet ( PORTS_MODULE_ID index, 
                         PORTS_CHANNEL channel,
                         PORTS_BIT_POS bitPos );
                         
                         
// *****************************************************************************
/* Function:
    bool SYS_PORTS_PinRead ( PORTS_MODULE_ID index, 
                             PORTS_CHANNEL channel,
                             PORTS_BIT_POS bitPos )

  Summary:
    Reads the selected digital pin.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function reads the selected digital pin, not the Latch.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    The status of the port pin.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_PIN_10
    bool bitStatus = SYS_PORTS_PinRead(MY_PORTS_INSTANCE, MY_CHANNEL, MY_PINNUM);
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

bool SYS_PORTS_PinRead ( PORTS_MODULE_ID index, 
                         PORTS_CHANNEL channel,
                         PORTS_BIT_POS bitPos );

// *****************************************************************************
/* Function:
    void SYS_PORTS_PinToggle ( PORTS_MODULE_ID index, 
                               PORTS_CHANNEL channel,
                               PORTS_BIT_POS bitPos )

  Summary:
    Toggles the selected digital pin.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function toggles the selected digital pin.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_PIN_10
    SYS_PORTS_PinToggle( MY_PORTS_INSTANCE, MY_CHANNEL, MY_PINNUM );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinToggle ( PORTS_MODULE_ID index, 
                           PORTS_CHANNEL channel,
                           PORTS_BIT_POS bitPos );

// *****************************************************************************
/* Function:
    void SYS_PORTS_PinSet( PORTS_MODULE_ID index, 
                           PORTS_CHANNEL channel, 
                           PORTS_BIT_POS bitPos )

  Summary:
    Sets the selected digital pin/latch.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the selected digital pin/latch.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_PIN_10
    SYS_PORTS_PinSet(MY_PORTS_INSTANCE, MY_CHANNEL, MY_PINNUM);
    </code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinSet( PORTS_MODULE_ID index, 
                       PORTS_CHANNEL channel, 
                       PORTS_BIT_POS bitPos );

// *****************************************************************************
/* Function:
    void SYS_PORTS_PinClear ( PORTS_MODULE_ID index, 
                              PORTS_CHANNEL channel,
                              PORTS_BIT_POS bitPos )

  Summary:
    Clears the selected digital pin.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function clears the selected digital pin.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_IO_PIN_10
    SYS_PORTS_PinClear( MY_PORTS_INSTANCE, MY_CHANNEL, MY_PINNUM );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinClear ( PORTS_MODULE_ID index, 
                          PORTS_CHANNEL channel,
                          PORTS_BIT_POS bitPos );

// *****************************************************************************
/* Function:
    void SYS_PORTS_PinDirectionSelect ( PORTS_MODULE_ID index,
                                     SYS_PORTS_PIN_DIRECTION pinDir,
                                     PORTS_CHANNEL channel,
                                     PORTS_BIT_POS bitPos )
  Summary:
    Enables the direction for the selected pin.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables the direction for the selected pin.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    pinDir          - Pin direction
    channel         - Identifier for the PORT channel: A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_PIN_10
    SYS_PORTS_PIN_DIRECTION pinDir;
    pinDir = SYS_PORTS_DIRECTION_INPUT;
    SYS_PORTS_PinDirectionSelect(MY_PORTS_INSTANCE, pinDir, MY_CHANNEL, MY_PINNUM);
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinDirectionSelect ( PORTS_MODULE_ID index,
                                 SYS_PORTS_PIN_DIRECTION pinDir,
                                 PORTS_CHANNEL channel,
                                 PORTS_BIT_POS bitPos );

// *****************************************************************************
/* Function:
    void SYS_PORTS_PinOpenDrainEnable ( PORTS_MODULE_ID index, 
                                        PORTS_CHANNEL channel,
                                        PORTS_BIT_POS bitPos )

  Summary:
    Enables the open-drain functionality for the selected pin.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables the open-drain functionality for the selected pin.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_PIN_10
    SYS_PORTS_PinOpenDrainEnable( MY_PORTS_INSTANCE, MY_CHANNEL, MY_PINNUM );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinOpenDrainEnable ( PORTS_MODULE_ID index, 
                                    PORTS_CHANNEL channel,
                                    PORTS_BIT_POS bitPos );


// *****************************************************************************
/* Function:
    void SYS_PORTS_PinOpenDrainDisable ( PORTS_MODULE_ID index, 
                                         PORTS_CHANNEL channel,
                                         PORTS_BIT_POS bitPos )

  Summary:
    Disables the open-drain functionality for the selected pin.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function disables the open-drain functionality for the selected pin.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_PIN_10
    SYS_PORTS_PinOpenDrainDisable(MY_PORTS_INSTANCE, MY_PINNUM);
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinOpenDrainDisable ( PORTS_MODULE_ID index, 
                                     PORTS_CHANNEL channel,
                                     PORTS_BIT_POS bitPos );

        
// *****************************************************************************
/* Function:
    void SYS_PORTS_PinPullUpEnable ( PORTS_MODULE_ID index, 
                                        PORTS_CHANNEL channel,
                                        PORTS_BIT_POS bitPos )

  Summary:
    Enables the pull-up functionality for the selected pin.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables the pull-up functionality for the selected pin.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_PIN_10
    SYS_PORTS_PinPullUpEnable( MY_PORTS_INSTANCE, MY_CHANNEL, MY_PINNUM );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinPullUpEnable ( PORTS_MODULE_ID index, 
                                    PORTS_CHANNEL channel,
                                    PORTS_BIT_POS bitPos );


// *****************************************************************************
/* Function:
    void SYS_PORTS_PinPullUpDisable ( PORTS_MODULE_ID index, 
                                         PORTS_CHANNEL channel,
                                         PORTS_BIT_POS bitPos )

  Summary:
    Disables the pull-up functionality for the selected pin.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function disables the pull-up functionality for the selected pin.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_PIN_10
    SYS_PORTS_PinPullUpDisable(MY_PORTS_INSTANCE, MY_PINNUM);
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinPullUpDisable ( PORTS_MODULE_ID index, 
                                     PORTS_CHANNEL channel,
                                     PORTS_BIT_POS bitPos );

// *****************************************************************************
/* Function:
    void SYS_PORTS_PinPullDownEnable ( PORTS_MODULE_ID index, 
                                        PORTS_CHANNEL channel,
                                        PORTS_BIT_POS bitPos )

  Summary:
    Enables the pull-down functionality for the selected pin.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables the pull-down functionality for the selected pin.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_PIN_10
    SYS_PORTS_PinPullDownEnable( MY_PORTS_INSTANCE, MY_CHANNEL, MY_PINNUM );
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinPullDownEnable ( PORTS_MODULE_ID index, 
                                    PORTS_CHANNEL channel,
                                    PORTS_BIT_POS bitPos );


// *****************************************************************************
/* Function:
    void SYS_PORTS_PinPullDownDisable ( PORTS_MODULE_ID index, 
                                         PORTS_CHANNEL channel,
                                         PORTS_BIT_POS bitPos )

  Summary:
    Disables the pull-down functionality for the selected pin.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function disables the pull-down functionality for the selected pin.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    channel         - Identifier for the PORT channel: A, B, C, etc.
    bitPos          - Possible values of PORTS_BIT_POS

  Returns:
    None.

  Example:
    <code>
    // Where MY_PORTS_INSTANCE, is the ports instance selected for use by the
    // application developer.
    // MY_PINNUM - PORTS_PIN_10
    SYS_PORTS_PinPullDownDisable(MY_PORTS_INSTANCE, MY_PINNUM);
	</code>

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinPullDownDisable ( PORTS_MODULE_ID index, 
                                     PORTS_CHANNEL channel,
                                     PORTS_BIT_POS bitPos );

                                     
//******************************************************************************
/* Function:
    void SYS_PORTS_InterruptEnable
    (
        PORTS_MODULE_ID index,
        PORTS_CHANNEL channel,
        PORTS_BIT_POS bitPos,
        PORTS_PIN_INTERRUPT_TYPE pinInterruptType
    )

  Summary:
    Enables the selected interrupt for the selected port pin.

  Description:
    This function enables the selected interrupt for the selected port pin.

  Preconditions:
    Select the pull-up or pull-down as required.

  Parameters:
    index            - Identifier for the device instance to be configured
    channel          - Identifier for the PORT channel: A, B, C, etc.
    bitPos           - Possible values of PORTS_BIT_POS
    pinInterruptType - Interrupt type from PORTS_PIN_INTERRUPT_TYPE set 

  Returns:
    None.

  Example:
    <code>
    SYS_PORTS_InterruptEnable(MY_PORTS_INSTANCE, MY_PORTS_CHANNEL, MY_PORT_BIT_POS, PORTS_PIN_INTERRUPT_BOTH_EDGE );
	</code>
    
  Remarks:
    None.
*/
void SYS_PORTS_InterruptEnable
(
    PORTS_MODULE_ID index,
    PORTS_CHANNEL channel,
    PORTS_BIT_POS bitPos,
    PORTS_PIN_INTERRUPT_TYPE pinInterruptType
);
        
        
//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif //SYS_PORTS_H

/*******************************************************************************
 End of File
*/