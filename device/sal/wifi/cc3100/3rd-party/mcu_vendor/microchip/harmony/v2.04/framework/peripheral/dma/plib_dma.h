/*******************************************************************************
  DMA Peripheral Library Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    plib_dma.h

  Summary:
    Defines the DMA Peripheral Library interface functions.

  Description:
    This header file contains the function prototypes and definitions of
    the data types and constants that make up the interface to the Direct Memory
    Access (DMA) Peripheral Library for Microchip microcontrollers.  The
    definitions in this file are for the DMA module.
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

#ifndef _PLIB_DMA_H
#define _PLIB_DMA_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Includes
// *****************************************************************************
// *****************************************************************************
/* See Bottom of file for implementation header include files.
*/

#include "peripheral/dma/processor/dma_processor.h"

// *****************************************************************************
// *****************************************************************************
// Section: Peripheral Library Interface Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: DMA Channel Status Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    bool PLIB_DMA_ChannelXBufferedDataIsWritten (   DMA_MODULE_ID index,
                                                    DMA_CHANNEL channel )

  Summary:
    Returns the buffered data write status for the specified channel.

  Description:
    This function returns the buffered data write status for the specified channel.

  Precondition:
    None.

  Parameters:
    channel	- One of the existing DMA channels listed by DMA_CHANNEL

  Returns:
    - true    - The content of the DMA buffer has not been written to the location
                specified in the destination/source address or in Null Write mode
    - false   - The content of the DMA buffer has been written to the location
                specified in the destination/source address or in Null Write mode

  Example:
    <code>
	bool chBuffWriteStatus;
    chBuffWriteStatus = PLIB_DMA_ChannelXBufferedDataIsWritten( DMA_ID_0,
                                                                DMA_CHANNEL_3 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific device
    data sheet to determine availability.
*/

    bool PLIB_DMA_ChannelXBufferedDataIsWritten (   DMA_MODULE_ID index,
                                                    DMA_CHANNEL channel );


/*******************************************************************************
  Function:
    bool PLIB_DMA_ChannelXCollisionStatus ( DMA_MODULE_ID index,
                                            DMA_CHANNEL channel,
                                            DMA_CHANNEL_COLLISION collisonType )

  Summary:
    Returns the status of the specified collision type for the specified channel.

  Description:
    This function returns the status of the specified collision type for the
	specified channel.

  Precondition:
    None.

  Parameters:
    channel         - One of the existing DMA channels listed by DMA_CHANNEL
    collisonType    - Collision type listed by DMA_CHANNEL_COLLISION

  Returns:
    - true    - A collision specified by collisonType was detected
    - false   - No collision of type collisonType was detected

  Example:
    <code>
        bool memWriteCollisionStatus;
        memWriteCollisionStatus = PLIB_DMA_ChannelXMemoryWriteCollisionStatus(
                                                DMA_ID_0,
                                                DMA_CHANNEL_3,
                                                DMA_CHANNEL_COLLISION_MEMORY );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

bool PLIB_DMA_ChannelXCollisionStatus ( DMA_MODULE_ID index,
                                        DMA_CHANNEL channel,
                                        DMA_CHANNEL_COLLISION collisonType );


//******************************************************************************
/* Function:
    DMA_PING_PONG_MODE PLIB_DMA_ChannelXPingPongModeGet(    DMA_MODULE_ID index,
                                                            DMA_CHANNEL channel )

  Summary:
    Returns the Ping-Pong mode status for the specified channel.

  Description:
    This function returns the Ping-Pong mode status for the specified channel.

  Precondition:
    None.

  Parameters:
    channel	- One of the existing DMA channels listed by DMA_CHANNEL

  Returns:
    mode        - One of the possible Ping-Pong modes

  Example:
    <code>
	DMA_PING_PONG_MODE chPingPongStatus;
    chPingPongStatus = PLIB_DMA_ChannelXPingPongModeGet(DMA_ID_0,
                                                        DMA_CHANNEL_3 );
    if (DMA_PING_PONG_SECONDARY == chPingPongStatus)
        {
            \\Application
        }
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

DMA_PING_PONG_MODE PLIB_DMA_ChannelXPingPongModeGet (   DMA_MODULE_ID index,
                                                        DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    bool PLIB_DMA_ChannelXEventIsDetected ( DMA_MODULE_ID index,
                                            DMA_CHANNEL channel )

  Summary:
    Returns the event status on the specified channel.

  Description:
    This function returns the event status on the specified channel.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - true	- An event was detected
    - false	- No events were detected

  Example:
    <code>
	bool channeleventStatus;
    channeleventStatus = PLIB_DMA_ChannelXEventIsDetected(  DMA_ID_0,
                                                            DMA_CHANNEL_2 );
    </code>

  Remarks:
	This function implements an operation of the ChannelXEvent feature.
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or include the
	PLIB_DMA_ExistsChannelXEvent function in your application to determine
	whether this feature is available.
*/

bool PLIB_DMA_ChannelXEventIsDetected ( DMA_MODULE_ID index,
                                        DMA_CHANNEL channel );


// *****************************************************************************
// *****************************************************************************
// Section: DMA Channel Configuration Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXPrioritySelect (  DMA_MODULE_ID index,
                                            DMA_CHANNEL channel,
                                            DMA_CHANNEL_PRIORITY channelPriority )

  Summary:
    Sets the priority of the specified channel.

  Description:
    This function sets the priority of the specified channel.

  Precondition:
    None.

  Parameters:
    channel		    - One of the existing DMA channels listed by DMA_CHANNEL
    channelPriority	- One of the supported priorities listed by DMA_CHANNEL_PRIORITY

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL channel = DMA_CHANNEL_0;
	DMA_CHANNEL_PRIORITY channelPriority = DMA_CHANNEL_PRIORITY_3;
    PLIB_DMA_ChannelXPrioritySelect( DMA_ID_0, channel, channelPriority );
    </code>

  Remarks:
	This function implements an operation of the ChannelXPriority feature.
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXPriority function in your application to determine
	whether this feature is available.
*/

void PLIB_DMA_ChannelXPrioritySelect (  DMA_MODULE_ID index,
                                        DMA_CHANNEL channel,
                                        DMA_CHANNEL_PRIORITY channelPriority );


//******************************************************************************
/* Function:
    DMA_CHANNEL_PRIORITY PLIB_DMA_ChannelXPriorityGet ( DMA_MODULE_ID index,
                                                        DMA_CHANNEL channel )

  Summary:
    Gets the priority of the specified channel.

  Description:
    This function gets the priority of the specified channel.

  Precondition:
    None.

  Parameters:
    channel			- One of the existing DMA channels listed by DMA_CHANNEL

  Returns:
	- channelPriority	- One of the supported priorities listed by DMA_CHANNEL_PRIORITY

  Example:
    <code>
	DMA_CHANNEL channel = DMA_CHANNEL_0;
	DMA_CHANNEL_PRIORITY channelPriority;
    channelPriority = PLIB_DMA_ChannelXPriorityGet( DMA_ID_0, channel );
    </code>

  Remarks:
	This function implements an operation of the ChannelXPriority feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXPriority function in your application to determine
	whether this feature is available.
*/

DMA_CHANNEL_PRIORITY PLIB_DMA_ChannelXPriorityGet ( DMA_MODULE_ID index,
                                                    DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelPrioritySelect (   DMA_MODULE_ID index,
                                            DMA_CHANNEL_PRIORITY channelPriority )

  Summary:
    Sets the priority scheme of the DMA channels.

  Description:
    This function sets the priority scheme of the DMA channels at the global level.
    This function is used in devices that do not have the per channel priority feature.

  Precondition:
    None.

  Parameters:
	channelPriority	- One of the supported priorities listed by DMA_CHANNEL_PRIORITY

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL_PRIORITY channelPriority = DMA_CHANNEL_ROUND_ROBIN;
    PLIB_DMA_ChannelPrioritySelect( DMA_ID_0, channelPriority );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

void PLIB_DMA_ChannelPrioritySelect (   DMA_MODULE_ID index,
                                        DMA_CHANNEL_PRIORITY channelPriority );


//******************************************************************************
/* Function:
    DMA_CHANNEL_PRIORITY PLIB_DMA_ChannelPriorityGet ( DMA_MODULE_ID index )

  Summary:
    Gets the priority scheme of the DMA channels.

  Description:
    This function gets the priority scheme of the DMA channels at the global level.
    This function is used in devices that do not have the per channel priority feature.

  Precondition:
    None.

  Parameters:
	None.

  Returns:
	- channelPriority	- One of the supported priorities listed by DMA_CHANNEL_PRIORITY

  Example:
    <code>
	DMA_CHANNEL_PRIORITY channelPriority;
    channelPriority = PLIB_DMA_ChannelPriorityGet( DMA_ID_0 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

DMA_CHANNEL_PRIORITY PLIB_DMA_ChannelPriorityGet ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXAutoEnable (  DMA_MODULE_ID index,
                                        DMA_CHANNEL channel )

  Summary:
    Channel is continuously enabled.

  Description:
    This function enables the channel continuously. The channel is not automatically
	disabled after a block transfer is complete.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXAutoEnable( DMA_ID_0, DMA_CHANNEL_2 );
    </code>

  Remarks:
	This function implements an operation of the ChannelXAuto feature.
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXAuto function in your application to determine
	whether this feature is available.
*/

void PLIB_DMA_ChannelXAutoEnable (  DMA_MODULE_ID index,
                                    DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    bool PLIB_DMA_ChannelXAutoIsEnabled (   DMA_MODULE_ID index,
                                            DMA_CHANNEL channel )

  Summary:
    Returns the channel automatic enable status.

  Description:
    This function returns the channel automatic enable status.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - true	- Channel automatic enable is on
    - false	- Channel automatic enable is off

  Example:
    <code>
	bool ChAutoEnableStatus;
    ChAutoEnableStatus = PLIB_DMA_ChannelXAutoIsEnabled(DMA_ID_0, DMA_CHANNEL_2 );
    </code>

  Remarks:
	This function implements an operation of the ChannelXAuto feature.
	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXAuto function in your application to determine
	whether this feature is available.
*/

bool PLIB_DMA_ChannelXAutoIsEnabled (   DMA_MODULE_ID index,
                                        DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXAutoDisable ( DMA_MODULE_ID index,
                                        DMA_CHANNEL channel )

  Summary:
    Channel is disabled after a block transfer is complete.

  Description:
    This function disables a channel after a block transfer is complete.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXAutoDisable( DMA_ID_0, DMA_CHANNEL_2 );
    </code>

  Remarks:
	This function implements an operation of the ChannelXAuto feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXAuto function in your application to determine
	whether this feature is available.
*/

void PLIB_DMA_ChannelXAutoDisable ( DMA_MODULE_ID index,
                                    DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXChainEnable ( DMA_MODULE_ID index,
                                        DMA_CHANNEL channel )

  Summary:
    Channel chain feature is enabled.

  Description:
    This function enables the channel chain feature.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXChainEnable( DMA_ID_0, DMA_CHANNEL_2 );
    </code>

  Remarks:
	This function implements an operation of the ChannelXChainEnbl feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXChainEnbl function in your application to determine
	whether this feature is available.
*/

void PLIB_DMA_ChannelXChainEnable ( DMA_MODULE_ID index,
                                    DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    bool PLIB_DMA_ChannelXChainIsEnabled (  DMA_MODULE_ID index,
                                            DMA_CHANNEL channel )

  Summary:
    Returns the chain status of the specified channel.

  Description:
    This function returns the chain status of the specified channel.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - true	- The channel chain is on for this channel
    - false	- The channel chain is off for this channel

  Example:
    <code>
	bool ChchainStatus;
    ChchainStatus = PLIB_DMA_ChannelXChainIsEnabled( DMA_ID_0, DMA_CHANNEL_2 );
    </code>

  Remarks:
	This function implements an operation of the ChannelXChainEnbl feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXChainEnbl function in your application to determine
	whether this feature is available.
*/

bool PLIB_DMA_ChannelXChainIsEnabled (  DMA_MODULE_ID index,
                                        DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXChainDisable( DMA_MODULE_ID index,
                                        DMA_CHANNEL channel )

  Summary:
    Disables the channel chaining for the specified DMA channel.

  Description:
    This function disables the channel chaining for the specified DMA channel.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXChainDisable( DMA_ID_0, DMA_CHANNEL_2 );
    </code>

  Remarks:
	This function implements an operation of the ChannelXChainEnbl feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXChainEnbl function in your application to determine
	whether this feature is available.
*/

void PLIB_DMA_ChannelXChainDisable( DMA_MODULE_ID index,
                                    DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXDisabledEnablesEvents (  DMA_MODULE_ID index,
                                                    DMA_CHANNEL channel )



  Summary:
    Channel start/abort events will be registered even if the channel is
	disabled.

  Description:
    This function will allow the channel register start/abort events even if the
    channel is disabled.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXDisabledEnablesEvents ( DMA_ID_0, DMA_CHANNEL_2 );
    </code>

  Remarks:
	This function implements an operation of the ChannelXDisabled feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXDisabled function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXDisabledEnablesEvents (  DMA_MODULE_ID index,
                                                DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXDisabledDisablesEvents ( DMA_MODULE_ID index,
                                                    DMA_CHANNEL channel )

  Summary:
    Channel start/abort events will be ignored even if the channel is
	disabled.

  Description:
    This function will allow the channel start/abort events to be ignored even if
    the channel is disabled.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXDisabledDisablesEvents ( DMA_ID_0, DMA_CHANNEL_2 );
    </code>

  Remarks:
	This function implements an operation of the ChannelXDisabled feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXDisabled function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXDisabledDisablesEvents ( DMA_MODULE_ID index,
                                                DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXEnable (  DMA_MODULE_ID index,
                                    DMA_CHANNEL channel )

  Summary:
    Enable the specified channel.

  Description:
    This function will enable the specified channel.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXEnable ( DMA_ID_0, DMA_CHANNEL_2 );
    </code>

  Remarks:
	This function implements an operation of the ChannelX feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelX function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXEnable ( DMA_MODULE_ID index,
                               DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    bool PLIB_DMA_ChannelXIsEnabled (   DMA_MODULE_ID index,
                                        DMA_CHANNEL channel )

  Summary:
    Return the enable status of the specified channel.

  Description:
    This function will return the enable status of the specified channel.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - true	- The specified DMA channel is enabled
    - false	- The specified DMA channel is disabled

  Example:
    <code>
	bool chEnableStatus;
    chEnableStatus = PLIB_DMA_ChannelXIsEnabled ( DMA_ID_0, DMA_CHANNEL_2 );
    </code>

  Remarks:
	This function implements an operation of the ChannelX feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelX function in your application to automatically
	determine whether this feature is available.
*/

bool PLIB_DMA_ChannelXIsEnabled (   DMA_MODULE_ID index,
                                    DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXDisable ( DMA_MODULE_ID index,
                                    DMA_CHANNEL channel )

  Summary:
    Disable the specified channel.

  Description:
    This function will disable the specified channel.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXDisable ( DMA_ID_0, DMA_CHANNEL_2 );
    </code>

  Remarks:
	This function implements an operation of the ChannelX feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelX function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXDisable ( DMA_MODULE_ID index,
                                DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXChainToLower( DMA_MODULE_ID index,
                                        DMA_CHANNEL channel )

  Summary:
    Chains the specified channel to a channel lower in natural priority.

  Description:
    This function will chain the specified channel to a channel lower in
	natural priority. CH3 will be enabled by a CH4 transfer complete.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXChainToLower ( DMA_ID_0, DMA_CHANNEL_4 );
    </code>

  Remarks:
	This function implements an operation of the ChannelXChain feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXChain function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXChainToLower( DMA_MODULE_ID index,
                                    DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXChainToHigher (   DMA_MODULE_ID index,
                                            DMA_CHANNEL channel )

  Summary:
    Chains the specified channel to a channel higher in natural priority.

  Description:
    This function will chain the specified channel to a channel higher in
	natural priority. CH5 will be enabled by a CH4 transfer complete.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXChainToHigher ( DMA_ID_0, DMA_CHANNEL_4 );
    </code>

  Remarks:
	This function implements an operation of the ChannelXChain feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXChain function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXChainToHigher (   DMA_MODULE_ID index,
                                        DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXBusyActiveSet (   DMA_MODULE_ID index,
                                            DMA_CHANNEL channel )

  Summary:
    Sets the Busy bit to active.

  Description:
    This function sets the Busy bit to active, indicating the channel is active
	or has been enabled.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXBusyActiveSet ( DMA_ID_0, DMA_CHANNEL_4 );
    </code>

  Remarks:
	This function implements an operation of the ChannelXBusy feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXBusy function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXBusyActiveSet (   DMA_MODULE_ID index,
                                        DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXBusyInActiveSet ( DMA_MODULE_ID index,
                                            DMA_CHANNEL channel )

  Summary:
    Sets the Busy bit to inactive.

  Description:
    This function sets the Busy bit to inactive, indicating the channel is inactive
	or has been disabled.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXBusyInActiveSet ( DMA_ID_0, DMA_CHANNEL_4 );
    </code>

  Remarks:
	This function implements an operation of the ChannelXBusy feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXBusy function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXBusyInActiveSet ( DMA_MODULE_ID index,
                                        DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    bool PLIB_DMA_ChannelXBusyIsBusy (  DMA_MODULE_ID index,
                                    DMA_CHANNEL channel )

  Summary:
    Returns the busy status of the specified channel.

  Description:
    This function returns the busy status of the specified channel.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - true	- The channel is active or has been enabled
    - false	- The channel is inactive or has been disabled

  Example:
    <code>
	bool chBusyStatus;
    chBusyStatus = PLIB_DMA_ChannelXBusyIsBusy ( DMA_ID_0, DMA_CHANNEL_4 );
    </code>

  Remarks:
	This function implements an operation of the ChannelXBusy feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXBusy function in your application to automatically
	determine whether this feature is available.
*/

bool PLIB_DMA_ChannelXBusyIsBusy (  DMA_MODULE_ID index,
                                DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXTransferDirectionSelect ( DMA_MODULE_ID index,
                                                    DMA_CHANNEL channel,
                                                    DMA_CHANNEL_TRANSFER_DIRECTION chTransferDirection	)

  Summary:
    Selects the data transfer direction of the specified channel.

  Description:
    This function selects the data transfer direction of the specified channel.

  Precondition:
    None.

  Parameters:
	channel			    - One of the possible DMA channels listed by DMA_CHANNEL
	chTransferDirection	- The transfer direction indicated by DMA_CHANNEL_TRANSFER_DIRECTION

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXTransferDirectionSelect (
                                DMA_ID_0,
                                DMA_CHANNEL_4,
                                DMA_READ_FROM_MEMORY_WRITE_TO_PERIPHERAL );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

void PLIB_DMA_ChannelXTransferDirectionSelect ( DMA_MODULE_ID index,
                                                DMA_CHANNEL channel,
                                                DMA_CHANNEL_TRANSFER_DIRECTION chTransferDirection	);


//******************************************************************************
/* Function:
    DMA_CHANNEL_TRANSFER_DIRECTION PLIB_DMA_ChannelXTransferDirectionGet (
                                                    DMA_MODULE_ID index,
                                                    DMA_CHANNEL channel)

  Summary:
    Returns the data transfer direction of the specified channel.

  Description:
    This function returns the data transfer direction of the specified channel.

  Precondition:
    None.

  Parameters:
	channel - One of the possible DMA channels listed by DMA_CHANNEL


  Returns:
    - DMA_CHANNEL_TRANSFER_DIRECTION  - The transfer direction indicated by the
                                        DMA_CHANNEL_TRANSFER_DIRECTION

  Example:
    <code>
	DMA_CHANNEL_TRANSFER_DIRECTION chTransferDirection;
    chTransferDirection = PLIB_DMA_ChannelXTransferDirectionGet (
                                                                DMA_ID_0,
                                                                DMA_CHANNEL_4 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

DMA_CHANNEL_TRANSFER_DIRECTION PLIB_DMA_ChannelXTransferDirectionGet (
                                                        DMA_MODULE_ID index,
                                                        DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXStartAddressOffsetSet(    DMA_MODULE_ID index,
                                                    DMA_CHANNEL channel ,
                                                    uint16_t address,
                                                    DMA_ADDRESS_OFFSET_TYPE offset )

  Summary:
    Sets the primary/secondary start address (DPSRAM) offset to the value
    specified depending on the offset type specified for the specified channel.

  Description:
    This function sets the primary/secondary start address (DPSRAM) offset to the value
    specified depending on the offset type specified for the specified channel.

  Precondition:
    None.

  Parameters:
	channel - One of the possible DMA channels listed by DMA_CHANNEL
	address - The primary/secondary DPSRAM start address offset
	offset  - The type of the address offset (primary/secondary)

  Returns:
    None.

  Example:
    <code>
	uint16_t address = 0x100;
    PLIB_DMA_ChannelXStartAddressOffsetSet (    DMA_ID_0,
                                                DMA_CHANNEL_4,
                                                address,
                                                DMA_ADDRESS_OFFSET_PRIMARY );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

void PLIB_DMA_ChannelXStartAddressOffsetSet (   DMA_MODULE_ID index,
                                                DMA_CHANNEL channel ,
                                                uint16_t address,
                                                DMA_ADDRESS_OFFSET_TYPE offset );


//******************************************************************************
/* Function:
    uint16_t PLIB_DMA_ChannelXStartAddressOffsetGet (   DMA_MODULE_ID index,
                                                        DMA_CHANNEL channel,
                                                        DMA_ADDRESS_OFFSET_TYPE offset)

  Summary:
    Gets the primary/secondary start address (DPSRAM) offset.

  Description:
    This function gets the primary/secondary start address (DPSRAM) offset.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL
    offset  - The type of the address offset (primary/secondary)

  Returns:
    - uint16_t	- The primary/secondary DPSRAM start address offset

  Example:
    <code>
	uint16_t addressOffsetA;
    addressOffsetA = PLIB_DMA_ChannelXStartAddressOffsetGet (
                                                    DMA_ID_0,
                                                    DMA_CHANNEL_4,
                                                    address,
                                                    DMA_ADDRESS_OFFSET_PRIMARY );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

uint16_t PLIB_DMA_ChannelXStartAddressOffsetGet (   DMA_MODULE_ID index,
                                                    DMA_CHANNEL channel,
                                                    DMA_ADDRESS_OFFSET_TYPE offset);


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXPeripheralAddressSet( DMA_MODULE_ID index,
                                                DMA_CHANNEL channel ,
	                                            uint16_t peripheraladdress )

  Summary:
    Sets the peripheral address for the specified channel.

  Description:
    This function sets the peripheral address for the specified channel.

  Precondition:
    None.

  Parameters:
	channel			- One of the possible DMA channels listed by DMA_CHANNEL
	peripheraladdress	- The peripheral address for the specified channel

  Returns:
    None.

  Example:
    <code>
	uint16_t peripheraladdress = 0x100;
    PLIB_DMA_ChannelXPeripheralAddressSet ( DMA_ID_0,
                                            DMA_CHANNEL_4,
                                            peripheraladdress );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

void PLIB_DMA_ChannelXPeripheralAddressSet( DMA_MODULE_ID index,
                                            DMA_CHANNEL channel ,
	                                        uint16_t peripheraladdress );


//******************************************************************************
/* Function:
    uint16_t PLIB_DMA_ChannelXPeripheralAddressGet( DMA_MODULE_ID index,
                                                    DMA_CHANNEL channel )

  Summary:
    Gets the peripheral address configured for the specified channel.

  Description:
    This function gets the peripheral address configured for the specified channel.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - uint16_t	- The peripheral address configured for the specified channel

  Example:
    <code>
	uint16_t peripheraladdress;
    peripheraladdress = PLIB_DMA_ChannelXPeripheralAddressGet ( DMA_ID_0,
                                                                DMA_CHANNEL_4 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

uint16_t PLIB_DMA_ChannelXPeripheralAddressGet( DMA_MODULE_ID index,
                                                DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXTransferCountSet( DMA_MODULE_ID index,
                                            DMA_CHANNEL channel ,
	                                        uint16_t transferCount )

  Summary:
    Sets the DMA data transfer count for the specified channel.

  Description:
    This function sets the DMA data transfer count for the specified channel.

  Precondition:
    None.

  Parameters:
	channel		- One of the possible DMA channels listed by DMA_CHANNEL
	transferCount	- The DMA transfer count for the channel

  Returns:
    None.

  Example:
    <code>
	uint16_t transferCount = 0x10;
    PLIB_DMA_ChannelXTransferCountSet ( DMA_ID_0,
                                        DMA_CHANNEL_4,
                                        transferCount );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

void PLIB_DMA_ChannelXTransferCountSet( DMA_MODULE_ID index,
                                        DMA_CHANNEL channel ,
	                                    uint16_t transferCount );


//******************************************************************************
/* Function:
    uint16_t PLIB_DMA_ChannelXTransferCountGet( DMA_MODULE_ID index,
                                                DMA_CHANNEL channel )

  Summary:
    Gets the DMA data transfer count that is programmed for the specified channel.

  Description:
    This function gets the DMA data transfer count that is programmed for the specified channel.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - uint16_t	- The DMA transfer count for the channel

  Example:
    <code>
	uint16_t transferCount;
    transferCount = PLIB_DMA_ChannelXTransferCountGet ( DMA_ID_0,
                                                        DMA_CHANNEL_4 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

uint16_t PLIB_DMA_ChannelXTransferCountGet( DMA_MODULE_ID index,
                                            DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXSourceAddressModeSelect (
                                DMA_MODULE_ID index,
                                DMA_CHANNEL channel,
                                DMA_SOURCE_ADDRESSING_MODE sourceAddressMode )

  Summary:
    Sets the source address mode of the specified channel.

  Description:
    This function sets the source address mode of the specified channel.

  Precondition:
    None.

  Parameters:
	channel			- One of the possible DMA channels listed by DMA_CHANNEL
	sourceAddressMode	- One of the possible source addressing modes listed by
	                      DMA_SOURCE_ADDRESSING_MODE

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXSourceAddressModeSelect (
                                DMA_ID_0,
                                DMA_CHANNEL_4,
                                DMA_ADDRESSING_SOURCE_INCREMENT_BASED_ON_SIZE );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

void PLIB_DMA_ChannelXSourceAddressModeSelect (
                                DMA_MODULE_ID index,
                                DMA_CHANNEL channel,
                                DMA_SOURCE_ADDRESSING_MODE sourceAddressMode );


//******************************************************************************
/* Function:
    DMA_SOURCE_ADDRESSING_MODE PLIB_DMA_ChannelXSourceAddressModeGet (
                                                            DMA_MODULE_ID index,
                                                            DMA_CHANNEL channel )

  Summary:
    Gets the source address mode of the specified channel.

  Description:
    This function gets the source address mode of the specified channel.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - DMA_SOURCE_ADDRESSING_MODE	- One of the possible source addressing modes
                                      listed by DMA_SOURCE_ADDRESSING_MODE

  Example:
    <code>
	DMA_SOURCE_ADDRESSING_MODE sourceAddressMode;
    sourceAddressMode = PLIB_DMA_ChannelXSourceAddressModeGet ( DMA_ID_0,
                                                                DMA_CHANNEL_4 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

DMA_SOURCE_ADDRESSING_MODE PLIB_DMA_ChannelXSourceAddressModeGet (
                                                            DMA_MODULE_ID index,
                                                            DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXDestinationAddressModeSelect (
                                DMA_MODULE_ID index,
                                DMA_CHANNEL channel,
                                DMA_DESTINATION_ADDRESSING_MODE destinationAddressMode )

  Summary:
    Sets the source address mode of the specified channel.

  Description:
    This function Sets the source address mode of the specified channel.

  Precondition:
    None.

  Parameters:
	channel			- One of the possible DMA channels listed by DMA_CHANNEL
	destinationAddressMode	- One of the possible source addressing modes listed by
	                          DMA_DESTINATION_ADDRESSING_MODE

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXDestinationAddressModeSelect (
                            DMA_ID_0,
		            DMA_CHANNEL_4,
                            DMA_ADDRESSING_DESTINATION_INCREMENT_BASED_ON_SIZE );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

void PLIB_DMA_ChannelXDestinationAddressModeSelect (
                                DMA_MODULE_ID index,
                                DMA_CHANNEL channel,
                                DMA_DESTINATION_ADDRESSING_MODE destinationAddressMode );


//******************************************************************************
/* Function:
    DMA_DESTINATION_ADDRESSING_MODE PLIB_DMA_ChannelXDestinationAddressModeGet (
                                                            DMA_MODULE_ID index,
                                                            DMA_CHANNEL channel )

  Summary:
    Gets the source address mode of the specified channel.

  Description:
    This function gets the source address mode of the specified channel.

  Precondition:
    None.

  Parameters:
    channel - One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - destinationAddressMode - One of the possible source addressing modes listed by
                               DMA_DESTINATION_ADDRESSING_MODE

  Example:
    <code>
	DMA_SOURCE_ADDRESSING_MODE destinationAddressMode;
    destinationAddressMode = PLIB_DMA_ChannelXDestinationAddressModeGet (
                                                                DMA_ID_0,
                                                                DMA_CHANNEL_4 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

 DMA_DESTINATION_ADDRESSING_MODE PLIB_DMA_ChannelXDestinationAddressModeGet (
                                                            DMA_MODULE_ID index,
                                                            DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXAddressModeSelect (
                                DMA_MODULE_ID index,
                                DMA_CHANNEL channel,
                                DMA_CHANNEL_ADDRESSING_MODE channelAddressMode )

  Summary:
    Sets the channel address mode.

  Description:
    This function sets the channel address mode.

  Precondition:
    None.

  Parameters:
	channel			- One of the possible DMA channels listed by DMA_CHANNEL
	channelAddressMode	- One of the possible channel addressing modes listed by
	                      DMA_CHANNEL_ADDRESSING_MODE

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXAddressModeSelect (
                        DMA_ID_0,
                        DMA_CHANNEL_4,
                        DMA_ADDRESSING_REGISTER_INDIRECT_WITH_POST_INCREMENT );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

void PLIB_DMA_ChannelXAddressModeSelect (
                                DMA_MODULE_ID index,
                                DMA_CHANNEL channel,
                                DMA_CHANNEL_ADDRESSING_MODE channelAddressMode );


//******************************************************************************
/* Function:
    DMA_CHANNEL_ADDRESSING_MODE PLIB_DMA_ChannelXAddressModeGet (
                                                            DMA_MODULE_ID index,
                                                            DMA_CHANNEL channel )

  Summary:
    Gets the channel address mode.

  Description:
    This function gets the channel address mode.

  Precondition:
    None.

  Parameters:
    channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - channelAddressMode - One of the possible source addressing modes listed by
                           DMA_CHANNEL_ADDRESSING_MODE

  Example:
    <code>
	DMA_CHANNEL_ADDRESSING_MODE channelAddressMode;
    channelAddressMode = PLIB_DMA_ChannelXAddressModeGet (  DMA_ID_0,
                                                            DMA_CHANNEL_4 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

DMA_CHANNEL_ADDRESSING_MODE PLIB_DMA_ChannelXAddressModeGet (
                                                            DMA_MODULE_ID index,
                                                            DMA_CHANNEL channel );


// *****************************************************************************
// *****************************************************************************
// Section: DMA Channel Event Configuration functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXTriggerEnable (   DMA_MODULE_ID index,
                                            DMA_CHANNEL channel,
                                            DMA_CHANNEL_TRIGGER_TYPE trigger )

  Summary:
    Enables the specified DMA channel trigger.

  Description:
    This function enables the specified DMA channel trigger.

  Precondition:
    None.

  Parameters:
    channel  - One of the possible DMA channels listed by DMA_CHANNEL
    trigger - Type of trigger (transfer start/abort/pattern match abort)

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXTriggerEnable (    DMA_ID_0,
                                        DMA_CHANNEL_4,
                                        DMA_CHANNEL_TRIGGER_TRANSFER_ABORT );
    </code>

  Remarks:
	This function implements an operation of the ChannelXTrigger feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXTrigger function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXTriggerEnable (   DMA_MODULE_ID index,
                                        DMA_CHANNEL channel,
                                        DMA_CHANNEL_TRIGGER_TYPE trigger );


//******************************************************************************
/* Function:
    bool PLIB_DMA_ChannelXTriggerIsEnabled( DMA_MODULE_ID index,
                                            DMA_CHANNEL channel,
                                            DMA_CHANNEL_TRIGGER_TYPE trigger )

  Summary:
    Returns the enable status of the specified DMA transfer/abort trigger.

  Description:
    This function returns the enable status of the specified DMA transfer/abort trigger.

  Precondition:
    None.

  Parameters:
    channel  - One of the possible DMA channels listed by DMA_CHANNEL
    trigger  - Type of trigger (transfer start/abort/pattern match abort)

  Returns:
    - true	- The specified trigger is enabled
    - false	- The specified trigger is disabled

  Example:
    <code>
	bool startTriggerstatus;
    startTriggerstatus = PLIB_DMA_ChannelXTriggerIsEnabled (
                                            DMA_ID_0,
                                            DMA_CHANNEL_4,
                                            DMA_CHANNEL_TRIGGER_TRANSFER_START );
    </code>

  Remarks:
	This function implements an operation of the ChannelXTrigger feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXTrigger function in your application to automatically
	determine whether this feature is available.
*/

bool PLIB_DMA_ChannelXTriggerIsEnabled( DMA_MODULE_ID index,
                                        DMA_CHANNEL channel,
                                        DMA_CHANNEL_TRIGGER_TYPE trigger );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXTriggerDisable (  DMA_MODULE_ID index,
                                            DMA_CHANNEL channel,
                                            DMA_CHANNEL_TRIGGER_TYPE trigger )

  Summary:
    Disables the DMA transfer abort via a matching interrupt (specified by the IRQ).

  Description:
    This function disables the DMA transfer abort via a matching interrupt
	(specified by the IRQ). The interrupt number IRQ is ignored and does not terminate
	a transfer.

  Precondition:
    None.

  Parameters:
    channel - One of the possible DMA channels listed by DMA_CHANNEL
    trigger - Type of trigger (transfer start/abort/pattern match abort)

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXTriggerDisable (   DMA_ID_0,
                                        DMA_CHANNEL_4,
                                        DMA_CHANNEL_TRIGGER_PATTERN_MATCH_ABORT );
    </code>

  Remarks:
	This function implements an operation of the ChannelXTrigger feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXTrigger function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXTriggerDisable (  DMA_MODULE_ID index,
                                        DMA_CHANNEL channel,
                                        DMA_CHANNEL_TRIGGER_TYPE trigger );

//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXTriggerSourceNumberGet (  DMA_MODULE_ID index,
						    DMA_CHANNEL channel)

  Summary:
    Gets the source number for the DMA channel interrupts.

  Description:
    This function returns the interrupt source number for the specified
	DMA channel.

  Precondition:
    None.

  Parameters:
    channel - One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXTriggerSourceNumberGet (   DMA_ID_0,
						DMA_CHANNEL_4);
    </code>

  Remarks:
	This function implements an operation of the ChannelXTrigger feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXTrigger function in your application to automatically
	determine whether this feature is available.
*/
DMA_CHANNEL_INT_SOURCE PLIB_DMA_ChannelXTriggerSourceNumberGet( DMA_MODULE_ID index,
								DMA_CHANNEL channel);


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXStartIRQSet ( DMA_MODULE_ID index,
                                        DMA_CHANNEL channel,
                                        DMA_TRIGGER_SOURCE IRQnum )

  Summary:
    Sets the IRQ to initiate the DMA transfer on the specified channel.

  Description:
    This function sets the IRQ to initiate the DMA transfer on the specified channel.
	(IRQ to start the channel transfer.)

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL
	IRQnum	- The IRQ number of the trigger source of type DMA_TRIGGER_SOURCE

  Returns:
    None.

  Example:
    <code>
	DMA_TRIGGER_SOURCE irq = DMA_TRIGGER_OUTPUT_COMPARE_1;
    PLIB_DMA_ChannelXStartIRQSet (  DMA_ID_0,
                                    DMA_CHANNEL_4,
                                    irq );
    </code>

  Remarks:
	This function implements an operation of the ChannelXStartIRQ feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXStartIRQ function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXStartIRQSet ( DMA_MODULE_ID index,
                                    DMA_CHANNEL channel,
                                    DMA_TRIGGER_SOURCE IRQnum );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXAbortIRQSet ( DMA_MODULE_ID index,
                                        DMA_CHANNEL channel,
                                        DMA_TRIGGER_SOURCE IRQ )

  Summary:
    Sets the IRQ to abort the DMA transfer on the specified channel.

  Description:
    This function sets the IRQ to abort the DMA transfer on the specified channel.
	(IRQ to start the channel transfer.)

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL
	IRQnum	- The IRQ number of the trigger source of type DMA_TRIGGER_SOURCE

  Returns:
    None.

  Example:
    <code>
	DMA_TRIGGER_SOURCE irq = DMA_TRIGGER_TIMER_CORE;
    PLIB_DMA_ChannelXAbortIRQSet (  DMA_ID_0,
                                    DMA_CHANNEL_4,
                                    irq );
    </code>

  Remarks:
	This function implements an operation of the ChannelXAbortIRQ feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXAbortIRQ function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXAbortIRQSet ( DMA_MODULE_ID index,
                                    DMA_CHANNEL channel,
                                    DMA_TRIGGER_SOURCE IRQ );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXDataSizeSelect (  DMA_MODULE_ID index,
                                            DMA_CHANNEL channel,
                                            DMA_CHANNEL_DATA_SIZE channelDataSize )

  Summary:
    Selects the data size for the specified channel.

  Description:
    This function selects the data size for the specified channel.

  Precondition:
    None.

  Parameters:
	channel		- One of the possible DMA channels listed by DMA_CHANNEL
	channelDataSize	- One of the possible data sizes listed by DMA_CHANNEL_DATA_SIZE

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL_DATA_SIZE channelDataSize = DMA_CHANNEL_DATA_8;
    PLIB_DMA_ChannelXDataSizeSelect (   DMA_ID_0,
                                        DMA_CHANNEL_4,
                                        channelDataSize );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

void PLIB_DMA_ChannelXDataSizeSelect (  DMA_MODULE_ID index,
                                        DMA_CHANNEL channel,
                                        DMA_CHANNEL_DATA_SIZE channelDataSize );


//******************************************************************************
/* Function:
    DMA_CHANNEL_DATA_SIZE PLIB_DMA_ChannelXDataSizeGet (    DMA_MODULE_ID index,
                                                            DMA_CHANNEL channel )

  Summary:
    Returns the current data size for the specified channel.

  Description:
    This function returns the current data size for the specified channel.

  Precondition:
    None.

  Parameters:
	channel		- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
	- channelDataSize	- One of the possible data sizes listed by
	                      DMA_CHANNEL_DATA_SIZE

  Example:
    <code>
	DMA_CHANNEL_DATA_SIZE channelDataSize;
    channelDataSize = PLIB_DMA_ChannelXDataSizeGet( DMA_ID_0,
                                                    DMA_CHANNEL_4 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

DMA_CHANNEL_DATA_SIZE PLIB_DMA_ChannelXDataSizeGet (    DMA_MODULE_ID index,
                                                        DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXOperatingTransferModeSelect (
                                        DMA_MODULE_ID index,
                                        DMA_CHANNEL channel,
                                        DMA_TRANSFER_MODE channeltransferMode )

  Summary:
    Selects the transfer/operating mode for the specified channel.

  Description:
    This function selects the transfer/operating mode for the specified channel.
	(Transfer mode and operating mode are used interchangeably.)

  Precondition:
    None.

  Parameters:
	channel			    - One of the possible DMA channels listed by DMA_CHANNEL
	channeltransferMode	- One of the possible operating/transfer modes listed by
                          DMA_TRANSFER_MODE

  Returns:
    None.

  Example:
    <code>
	DMA_TRANSFER_MODE channeltransferMode = DMA_MODE_REPEATED_CONTINUOUS;
    PLIB_DMA_ChannelXOperatingTransferModeSelect (  DMA_ID_0,
                                                    DMA_CHANNEL_4,
                                                    channeltransferMode );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

void PLIB_DMA_ChannelXOperatingTransferModeSelect (
                                        DMA_MODULE_ID index,
                                        DMA_CHANNEL channel,
                                        DMA_TRANSFER_MODE channeltransferMode );


//******************************************************************************
/* Function:
    DMA_TRANSFER_MODE PLIB_DMA_ChannelXOperatingTransferModeGet (
                                                            DMA_MODULE_ID index,
                                                            DMA_CHANNEL channel )

  Summary:
    Returns the current transfer/operating mode for the specified channel.

  Description:
    This function returns the current transfer/operating mode for the specified channel.
	(Transfer mode and operating mode are used interchangeably.)

  Precondition:
    None.

  Parameters:
	channel		- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - channeltransferMode	- One of the possible operating/transfer modes listed
                              by DMA_TRANSFER_MODE

  Example:
    <code>
	DMA_TRANSFER_MODE channeltransferMode;
    channeltransferMode = PLIB_DMA_ChannelXOperatingTransferModeGet (
                                                                DMA_ID_0,
                                                                DMA_CHANNEL_4 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

DMA_TRANSFER_MODE PLIB_DMA_ChannelXOperatingTransferModeGet (
                                                            DMA_MODULE_ID index,
                                                            DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXReloadEnable( DMA_MODULE_ID index,
                                        DMA_CHANNEL channel )

  Summary:
    Enables reloading of the address and count registers.

  Description:
    This function enables reloading of the address and count registers. The source,
	destination address, and the DMA count registers are reloaded to their previous
	values upon the start of the next operation.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXReloadEnable ( DMA_ID_0,
                                    DMA_CHANNEL_4 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

void PLIB_DMA_ChannelXReloadEnable( DMA_MODULE_ID index,
                                    DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    bool PLIB_DMA_ChannelXReloadIsEnabled ( DMA_MODULE_ID index,
                                            DMA_CHANNEL channel )

  Summary:
    Returns the address and count registers reload enable status.

  Description:
    This function returns the address and count registers reload enable status.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - true	- The address and count registers reload is enabled
    - false	- The address and count registers reload is disabled

  Example:
    <code>
	bool chAddressCountReloadStatus;
    chAddressCountReloadStatus = PLIB_DMA_ChannelXReloadIsEnabled (
                                                                DMA_ID_0,
                                                                DMA_CHANNEL_4 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

bool PLIB_DMA_ChannelXReloadIsEnabled ( DMA_MODULE_ID index,
                                        DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXReloadDisable (   DMA_MODULE_ID index,
                                            DMA_CHANNEL channel )

  Summary:
    Disables reloading of the address and count registers.

  Description:
    This function disables reloading of the address and count registers. The source, destination
	address, and the DMA count registers are not reloaded to their previous values upon
	the start of the next operation.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXReloadDisable (    DMA_ID_0,
                                        DMA_CHANNEL_4 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

void PLIB_DMA_ChannelXReloadDisable (   DMA_MODULE_ID index,
                                        DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXNullWriteModeEnable ( DMA_MODULE_ID index,
                                                DMA_CHANNEL channel )

  Summary:
    Enables the Null Write mode.

  Description:
    This function enables the Null Write mode. A dummy write is initiated to the
	source address for every write to the destination address.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXNullWriteModeEnable (  DMA_ID_0,
                                            DMA_CHANNEL_4 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

void PLIB_DMA_ChannelXNullWriteModeEnable ( DMA_MODULE_ID index,
                                            DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    bool PLIB_DMA_ChannelXNullWriteModeIsEnabled (  DMA_MODULE_ID index,
                                                    DMA_CHANNEL channel )

  Summary:
    Returns the enable status of the Null Write mode for the specified channel.

  Description:
    This function returns the enable status of the Null Write mode for the specified channel.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - true	- Null write mode is enabled for this channel
    - false	- Null write mode is disabled for this channel

  Example:
    <code>
	bool chNullWriteStatus;
    chNullWriteStatus = PLIB_DMA_ChannelXNullWriteModeIsEnabled (
                                                                DMA_ID_0,
                                                                DMA_CHANNEL_4 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

bool PLIB_DMA_ChannelXNullWriteModeIsEnabled (  DMA_MODULE_ID index,
                                                DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXNullWriteModeDisable( DMA_MODULE_ID index,
                                                DMA_CHANNEL channel )

  Summary:
    Disables the Null Write mode.

  Description:
    This function disables the Null Write mode. No dummy write is initiated.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_ChannelXNullWriteModeDisable ( DMA_ID_0,
                                            DMA_CHANNEL_4 );
    </code>

  Remarks:
    This feature is not available on all devices. Please refer to the specific
    device data sheet to determine availability.
*/

void PLIB_DMA_ChannelXNullWriteModeDisable( DMA_MODULE_ID index,
                                            DMA_CHANNEL channel );


// *****************************************************************************
// *****************************************************************************
// Section: DMA start/end functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void PLIB_DMA_StartTransferSet (   DMA_MODULE_ID index,
                                    DMA_CHANNEL channel )

  Summary:
    Initiates transfer on the specified channel.

  Description:
    This function initiates transfer on the specified channel. This is a forced transfer
	controlled via software.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_StartTransferSet (    DMA_ID_0,
                                DMA_CHANNEL_4 );
    </code>

  Remarks:
	This function implements an operation of the StartTransfer feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsStartTransfer function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_StartTransferSet (   DMA_MODULE_ID index,
                                DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    void PLIB_DMA_AbortTransferSet (   DMA_MODULE_ID index,
                                    DMA_CHANNEL channel )

  Summary:
    Aborts transfer on the specified channel.

  Description:
    This function aborts transfer on the specified channel. This is a forced abort
	controlled via software.

  Precondition:
    None.

  Parameters:
	channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_AbortTransferSet (    DMA_ID_0,
                                DMA_CHANNEL_4 );
    </code>

  Remarks:
	This function implements an operation of the AbortTransfer feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsAbortTransfer function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_AbortTransferSet (   DMA_MODULE_ID index,
                                DMA_CHANNEL channel );



// *****************************************************************************
// *****************************************************************************
// Section: DMA General Configuration functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void PLIB_DMA_BusyActiveSet ( DMA_MODULE_ID index )

  Summary:
    Sets the BUSY bit of the DMA controller.

  Description:
    This function sets the BUSY bit of the DMA controller. The DMA module is active.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_BusyActiveSet( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the Busy feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsBusy function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_BusyActiveSet ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_BusyActiveReset ( DMA_MODULE_ID index )

  Summary:
    Resets the BUSY bit of the DMA controller.

  Description:
    This function resets the BUSY bit of the DMA controller. The DMA module is
    disabled and is not actively transferring data.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_BusyActiveReset( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the Busy feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsBusy function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_BusyActiveReset (   DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_DMA_IsBusy ( DMA_MODULE_ID index )

  Summary:
    Gets the BUSY bit of the DMA controller.

  Description:
    This function gets the BUSY bit of the DMA controller.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    - true	- DMA module is active
    - false	- DMA module is disabled and is not actively transferring data

  Example:
    <code>
	bool dmaBusyStatus;
    dmaBusyStatus = PLIB_DMA_IsBusy( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the Busy feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsBusy function in your application to automatically
	determine whether this feature is available.
*/

bool PLIB_DMA_IsBusy ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_SuspendEnable ( DMA_MODULE_ID index )

  Summary:
    DMA transfers are suspended to allow uninterrupted access by the CPU to the data bus.

  Description:
    This function suspends the DMA transfers to allow uninterrupted access
	by the CPU to the data bus.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_SuspendEnable( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the Suspend feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsSuspend function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_SuspendEnable ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_SuspendDisable ( DMA_MODULE_ID index )

  Summary:
    DMA suspend is disabled and the DMA module operates normally.

  Description:
    This function disables the DMA suspend. The DMA module continues to operate
	normally.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_SuspendDisable( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the Suspend feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsSuspend function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_SuspendDisable ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_DMA_SuspendIsEnabled ( DMA_MODULE_ID index )

  Summary:
    Returns the DMA suspend status.

  Description:
    This function returns the DMA suspend status.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    - true	- The DMA transfers are suspended
    - false	- The DMA operates normally

  Example:
    <code>
	bool dmaSuspendStatus;
    dmaSuspendStatus = PLIB_DMA_SuspendIsEnabled( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the Suspend feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsSuspend function in your application to automatically
	determine whether this feature is available.
*/

bool PLIB_DMA_SuspendIsEnabled ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_StopInIdleEnable ( DMA_MODULE_ID index )

  Summary:
    DMA transfers are halted during Idle mode.

  Description:
    This function halts DMA transfers during Idle mode.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_StopInIdleEnable( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the StopInIdle feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsStopInIdle function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_StopInIdleEnable ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_StopInIdleDisable ( DMA_MODULE_ID index )

  Summary:
    DMA transfers continue during Idle mode.

  Description:
    This function causes DMA transfers to continue during Idle mode.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_StopInIdleDisable( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the StopInIdle feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsStopInIdle function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_StopInIdleDisable ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_Enable ( DMA_MODULE_ID index )

  Summary:
    DMA module is enabled.

  Description:
    This function enables the DMA module.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_Enable( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the EnableControl feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsEnableControl function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_Enable ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_DMA_IsEnabled ( DMA_MODULE_ID index )

  Summary:
    Returns the DMA module enable status.

  Description:
    This function returns the DMA module enable status.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    - true	- The DMA is enabled
    - false	- The DMA is disabled

  Example:
    <code>
    PLIB_DMA_IsEnabled( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the EnableControl feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsEnableControl function in your application to automatically
	determine whether this feature is available.
*/

bool PLIB_DMA_IsEnabled ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_Disable ( DMA_MODULE_ID index )

  Summary:
    DMA module is disabled.

  Description:
    This function disables the DMA module.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_Disable( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the EnableControl feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsEnableControl function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_Disable ( DMA_MODULE_ID index );


// *****************************************************************************
// *****************************************************************************
// Section: DMA status functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    uint8_t PLIB_DMA_ChannelBitsGet ( DMA_MODULE_ID index )

  Summary:
    Returns the DMA channel bits.

  Description:
    This function returns the channel bits.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    - uint8_t	- DMA channel bits

  Example:
    <code>
	uint8_t dmaChBits;
    dmaChBits = PLIB_DMA_ChannelBitsGet( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the ChannelBits feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelBits function in your application to automatically
	determine whether this feature is available.
*/

uint8_t PLIB_DMA_ChannelBitsGet ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_DMA_LastBusAccessIsRead ( DMA_MODULE_ID index )

  Summary:
    Returns true if the last DMA bus access was a read.

  Description:
    This function returns true if the last DMA bus access was a read.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    - true	- The last bus access was a read
    - false	- The last bus access was not a read

  Example:
    <code>
	bool dmaLastBusAccessType;
    dmaLastBusAccessType = PLIB_DMA_LastBusAccessIsRead( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the LastBusAccess feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsLastBusAccess function in your application to automatically
	determine whether this feature is available.
*/

bool PLIB_DMA_LastBusAccessIsRead ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_DMA_LastBusAccessIsWrite ( DMA_MODULE_ID index )

  Summary:
    Returns true if the last DMA bus access was a write.

  Description:
    This function returns true if the last DMA bus access was a write operation.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    - true	- The last bus access was a write operation
    - false	- The last bus access was not a write operation

  Example:
    <code>
	bool dmaLastBusAccessType;
    dmaLastBusAccessType = PLIB_DMA_LastBusAccessIsWrite( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the LastBusAccess feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsLastBusAccess function in your application to automatically
	determine whether this feature is available.
*/

bool PLIB_DMA_LastBusAccessIsWrite ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    uint32_t PLIB_DMA_RecentAddressAccessed ( DMA_MODULE_ID index )

  Summary:
    Returns the address of the most recent DMA access.

  Description:
    This function returns the address of the most recent DMA access.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    - uint32_t	- The most recent address accessed by the DMA

  Example:
    <code>
	uint32_t dmaLastAddressAccessed;
    dmaLastAddressAccessed = PLIB_DMA_RecentAddressAccessed( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the RecentAddress feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsRecentAddress function in your application to automatically
	determine whether this feature is available.
*/

uint32_t PLIB_DMA_RecentAddressAccessed ( DMA_MODULE_ID index );


// *****************************************************************************
// *****************************************************************************
// Section: DMA Module CRC Feature Interface Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void PLIB_DMA_CRCChannelSelect( DMA_MODULE_ID index,
                                    DMA_CHANNEL channel )

  Summary:
    Assigns the CRC to the specified DMA channel.

  Description:
    This function assigns the CRC feature to the specified channel.

  Precondition:
    None.

  Parameters:
    channel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_CRCChannelSelect(  DMA_ID_0,
                                DMA_CHANNEL_5 );
    </code>

  Remarks:
	This function implements an operation of the CRCChannel feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCChannel function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_CRCChannelSelect( DMA_MODULE_ID index,
                                DMA_CHANNEL channel );


//******************************************************************************
/* Function:
    DMA_CHANNEL PLIB_DMA_CRCChannelGet ( DMA_MODULE_ID index )

  Summary:
    Returns the current DMA channel to which the CRC is assigned.

  Description:
    This function returns the current DMA channel to which the CRC is assigned.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    crcChannel	- One of the possible DMA channels listed by DMA_CHANNEL

  Example:
    <code>
	DMA_CHANNEL crcChannel;
    crcChannel = PLIB_DMA_CRCChannelGet( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the CRCChannel feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCChannel function in your application to automatically
	determine whether this feature is available.
*/

DMA_CHANNEL PLIB_DMA_CRCChannelGet ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_CRCTypeSet (  DMA_MODULE_ID index,
                                DMA_CRC_TYPE CRCType )

  Summary:
    Selects the DMA module CRC feature type.

  Description:
    This function selects the DMA module CRC feature type. The CRC feature will
	compute either the IP header checksum or the Linear Shift Feedback Register (LFSR)
	checksum.

  Precondition:
    None.

  Parameters:
    CRCType	- One of the possible CRC checksums listed by DMA_CRC_TYPE

  Returns:
    None.

  Example:
    <code>
    PLIB_DMA_CRCTypeSet(DMA_ID_0,
                        DMA_CRC_IP_HEADER );
    </code>

  Remarks:
	This function implements an operation of the CRCType feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCType function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_CRCTypeSet (  DMA_MODULE_ID index,
							DMA_CRC_TYPE CRCType );


//******************************************************************************
/* Function:
    DMA_CRC_TYPE PLIB_DMA_CRCTypeGet ( DMA_MODULE_ID index )

  Summary:
    Gets the current DMA module CRC feature type.

  Description:
    This function gets the DMA module CRC feature type. The CRC feature will
	compute either the IP header checksum or the Linear Shift Feedback Register (LFSR)
	checksum.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    - CRCType	- One of the possible CRC checksums listed by DMA_CRC_TYPE.

  Example:
    <code>
	DMA_CRC_TYPE CRCType;
    CRCType = PLIB_DMA_CRCTypeGet( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the CRCType feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCType function in your application to automatically
	determine whether this feature is available.
*/

DMA_CRC_TYPE PLIB_DMA_CRCTypeGet ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_CRCAppendModeEnable ( DMA_MODULE_ID index )

  Summary:
    Enables the CRC append mode.

  Description:
    This function enables the CRC append mode. The DMA transfers data from the source
	into the CRC, but not to the destination. When a block transfer completes, the
	DMA writes the calculated CRC value to the location specified by the CHxDSA register.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
	PLIB_DMA_CRCAppendModeEnable( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the CRCAppendMode feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCAppendMode function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_CRCAppendModeEnable ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_DMA_CRCAppendModeIsEnabled ( DMA_MODULE_ID index )

  Summary:
    Gets the enable status of the CRC append mode.

  Description:
    This function gets the enable status of the CRC append mode.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    - true	- CRC append mode is enabled
    - false	- CRC append mode is disabled

  Example:
    <code>
	bool DMAcrcAppendMode;
	DMAcrcAppendMode = PLIB_DMA_CRCAppendModeIsEnabled( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the CRCAppendMode feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCAppendMode function in your application to automatically
	determine whether this feature is available.
*/

bool PLIB_DMA_CRCAppendModeIsEnabled ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_CRCAppendModeDisable ( DMA_MODULE_ID index )

  Summary:
    Disables the CRC append mode.

  Description:
    This function disables the CRC append mode. The DMA transfers data from the
	source through the CRC obeying WBO (DMA_MODULE_ID index, write byte order)
    as it writes the data to the destination.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
	PLIB_DMA_CRCAppendModeDisable( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the CRCAppendMode feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCAppendMode function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_CRCAppendModeDisable ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_CRCEnable ( DMA_MODULE_ID index )

  Summary:
    Enables the DMA module CRC feature.

  Description:
    This function enables the DMA module CRC feature. The channel transfers are
	routed through the CRC.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
	PLIB_DMA_CRCEnable( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the CRC feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRC function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_CRCEnable ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_DMA_CRCIsEnabled ( DMA_MODULE_ID index )

  Summary:
    Gets the enable status of the CRC feature.

  Description:
    This function gets the enable status of the CRC feature.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    - true	- The CRC feature is enabled
    - false	- The CRC feature is disabled

  Example:
    <code>
	bool DMAcrcStatus;
	DMAcrcStatus = PLIB_DMA_CRCIsEnabled( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the CRC feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRC function in your application to automatically
	determine whether this feature is available.
*/

bool PLIB_DMA_CRCIsEnabled ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_CRCDisable ( DMA_MODULE_ID index )

  Summary:
    Disables the DMA module CRC feature.

  Description:
    This function disables the DMA module CRC feature. The channel transfers proceed
	normally.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
	PLIB_DMA_CRCDisable( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the CRC feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRC function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_CRCDisable ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_CRCPolynomialLengthSet (  DMA_MODULE_ID index,
                                            uint8_t polyLength )

  Summary:
    Selects the polynomial length.

  Description:
    This function Selects the polynomial length.

  Precondition:
    None.

  Parameters:
    polyLength	- Polynomial length

  Returns:
    None.

  Example:
    <code>
	uint8_t polyLength = 0x2;
    PLIB_DMA_CRCPolynomialLengthSet( DMA_ID_0, polyLength );
    </code>

  Remarks:
	This function implements an operation of the CRCPolynomialLength feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCPolynomialLength function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_CRCPolynomialLengthSet (  DMA_MODULE_ID index,
                                        uint8_t polyLength );


//******************************************************************************
/* Function:
    uint8_t PLIB_DMA_CRCPolynomialLengthGet ( DMA_MODULE_ID index )

  Summary:
    Gets the current polynomial length.

  Description:
    This function gets the current polynomial length.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    - uint8_t	- Polynomial length

  Example:
    <code>
	uint8_t polyLength;
    polyLength = PLIB_DMA_CRCPolynomialLengthGet( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the CRCPolynomialLength feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCPolynomialLength function in your application to automatically
	determine whether this feature is available.
*/

uint8_t PLIB_DMA_CRCPolynomialLengthGet( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_CRCBitOrderSelect (   DMA_MODULE_ID index,
                                        DMA_CRC_BIT_ORDER bitOrder )

  Summary:
    Selects the bit order for checksum calculation.

  Description:
    This function selects the bit order for checksum calculation.

  Precondition:
    None.

  Parameters:
    bitOrder - Specifies the bit order for CRC calculation

  Returns:
    None.

  Example:
    <code>
	PLIB_DMA_CRCBitOrderSelect ( DMA_ID_0,DMA_CRC_BIT_ORDER_LSB );
    </code>

  Remarks:
	This function implements an operation of the CRCBitOrder feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCBitOrder function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_CRCBitOrderSelect (   DMA_MODULE_ID index,
                                    DMA_CRC_BIT_ORDER bitOrder );


//******************************************************************************
/* Function:
    void PLIB_DMA_CRCWriteByteOrderAlter ( DMA_MODULE_ID index )

  Summary:
    The source data is written to the destination reordered as defined by the
	BYTO<1:0> bits.

  Description:
    This function enables byte order alteration as specified by the BYTO<1:0> bits.
	The source data is written to the destination reordered as defined by the
	BYTO<1:0> bits.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
	PLIB_DMA_CRCWriteByteOrderAlter ( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the CRCWriteByteOrder feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCWriteByteOrder function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_CRCWriteByteOrderAlter ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_CRCWriteByteOrderMaintain ( DMA_MODULE_ID index )

  Summary:
    The source data is written to the destination unaltered.

  Description:
    This function disables byte order alteration. The source data is written
	to the destination unaltered.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
	PLIB_DMA_CRCWriteByteOrderMaintain ( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the CRCWriteByteOrder feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCWriteByteOrder function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_CRCWriteByteOrderMaintain ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_CRCByteOrderSelect (  DMA_MODULE_ID index,
                                        DMA_CRC_BYTE_ORDER byteOrder )

  Summary:
    Selects the byte order.

  Description:
    This function selects the byte order.

  Precondition:
    The WBO bit must be set to use this function.

  Parameters:
    byteOrder	- One of the possible byte orders specified by DMA_CRC_BYTE_ORDER

  Returns:
    None.

  Example:
    <code>
	PLIB_DMA_CRCByteOrderSelect (   DMA_ID_0,
                                    DMA_CRC_SWAP_HALF_WORD_ON_WORD_BOUNDARY );
    </code>

  Remarks:
	This function implements an operation of the CRCByteOrder feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCByteOrder function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_CRCByteOrderSelect (  DMA_MODULE_ID index,
                                    DMA_CRC_BYTE_ORDER byteOrder );


//******************************************************************************
/* Function:
    DMA_CRC_BYTE_ORDER PLIB_DMA_CRCByteOrderGet ( DMA_MODULE_ID index )

  Summary:
    Gets the current byte order selected by the DMA module CRC feature.

  Description:
    This function gets the current byte order selected by the DMA module CRC feature.

  Precondition:
    The WBO bit must be set to use this function.

  Parameters:
    None.

  Returns:
    - byteOrder	- One of the possible byte orders specified by DMA_CRC_BYTE_ORDER

  Example:
    <code>
	DMA_CRC_BYTE_ORDER byteOrder;
	byteOrder = PLIB_DMA_CRCByteOrderGet ( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the CRCByteOrder feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCByteOrder function in your application to automatically
	determine whether this feature is available.
*/

DMA_CRC_BYTE_ORDER PLIB_DMA_CRCByteOrderGet ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    uint32_t PLIB_DMA_CRCDataRead ( DMA_MODULE_ID index )

  Summary:
    Reads the contents of the DMA CRC data register.

  Description:
    This function reads the contents of the DMA CRC data register.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    - uint32_t	- 32-bit CRC data

    The CRC data (16-bit, 32-bit) is device-specific. Please refer to the specific
    device data sheet to determine availability.

  Example:
    <code>
	uint32_t DMACRCdata;
	DMACRCdata = PLIB_DMA_CRCDataRead ( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the CRCData feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCData function in your application to automatically
	determine whether this feature is available.
*/

uint32_t PLIB_DMA_CRCDataRead ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_CRCDataWrite (    DMA_MODULE_ID index,
                                    uint32_t DMACRCdata )

  Summary:
    Writes the contents of the DMA CRC data register with the specified data.

  Description:
    This function writes the contents of the DMA CRC data register.

  Precondition:
    None.

  Parameters:
    - DMACRCdata	- 32-bit CRC data

    The CRC data (16-bit, 32-bit) is device-specific. Please refer to the specific
    device data sheet to determine availability.

  Returns:
    None.

  Example:
    <code>
	uint32_t DMACRCdata = 0x0E0E0E;
	PLIB_DMA_CRCDataWrite ( DMA_ID_0, DMACRCdata );
    </code>

  Remarks:
	This function implements an operation of the CRCData feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCData function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_CRCDataWrite (    DMA_MODULE_ID index,
                                uint32_t DMACRCdata );


//******************************************************************************
/* Function:
    uint32_t PLIB_DMA_CRCXOREnableGet ( DMA_MODULE_ID index )

  Summary:
    Reads the CRC XOR register.

  Description:
    This function reads the CRC XOR register.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    - uint32_t	- 32-bit CRC XOR enable mask data

    The CRC data (16-bit, 32-bit) is device-specific. Please refer to the specific
    device data sheet to determine availability.

  Example:
    <code>
	uint32_t DMACRCXORdata;
	DMACRCXORdata = PLIB_DMA_CRCXOREnableGet ( DMA_ID_0 );
    </code>

  Remarks:
	This function implements an operation of the CRCXOREnable feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCXOREnable function in your application to automatically
	determine whether this feature is available.
*/

uint32_t PLIB_DMA_CRCXOREnableGet ( DMA_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_DMA_CRCXOREnableSet ( DMA_MODULE_ID index,
                                    uint32_t DMACRCXOREnableMask )

  Summary:
    Writes to the CRC XOR enable register as per the specified enable mask.

  Description:
    This function writes to the CRC XOR enable register as per the specified
	enable mask. Each enabled bit will be taken as input to the shift register.

  Precondition:
    None.

  Parameters:
    - DMACRCXOREnableMask	- 32-bit CRC XOR enable mask data

    The CRC data (16-bit, 32-bit) is device-specific. Please refer to the specific
    device data sheet to determine availability.

  Returns:
    None.

  Example:
    <code>
	uint32_t DMACRCXOREnableMask = 0x05EFFFFF;
	PLIB_DMA_CRCXOREnableSet ( DMA_ID_0, DMACRCXOREnableMask );
    </code>

  Remarks:
	This function implements an operation of the CRCXOREnable feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsCRCXOREnable function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_CRCXOREnableSet ( DMA_MODULE_ID index,
                                uint32_t DMACRCXOREnableMask );

// *****************************************************************************
// *****************************************************************************
// Section: DMA Address control Interface functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    uint32_t PLIB_DMA_ChannelXSourceStartAddressGet (   DMA_MODULE_ID index,
                                                        DMA_CHANNEL dmaChannel )

  Summary:
    Reads the source start address configured for the specified channel.

  Description:
    This function reads the source start address configured for the specified
	channel.

  Precondition:
    None.

  Parameters:
    dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - uint32_t	- The source start address configured for this channel

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint32_t SourceStartAddress;
	SourceStartAddress = PLIB_DMA_ChannelXSourceStartAddressGet(DMA_ID_0,
                                                                spiDMAChannel );
    </code>

  Remarks:
	This function implements an operation of the ChannelXSourceStartAddress feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXSourceStartAddress function in your application to automatically
	determine whether this feature is available.
*/

uint32_t PLIB_DMA_ChannelXSourceStartAddressGet (   DMA_MODULE_ID index,
                                                    DMA_CHANNEL dmaChannel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXSourceStartAddressSet (   DMA_MODULE_ID index,
                                                    DMA_CHANNEL dmaChannel,
													uint32_t sourceStartAddress)

  Summary:
    Writes the specified source start address into the register
	corresponding to the specified channel.

  Description:
    This function writes the specified Source start address into the register
    corresponding to the specified channel.

  Precondition:
    None.

  Parameters:
    dmaChannel		    - One of the possible DMA channels listed by DMA_CHANNEL
	sourceStartAddress	- The source start address

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint32_t sourceStartAddress = 0x00FDEA00;
	PLIB_DMA_ChannelXSourceStartAddressSet (    DMA_ID_0,
                                                spiDMAChannel,
                                                sourceStartAddress );
    </code>

  Remarks:
	This function implements an operation of the ChannelXSourceStartAddress feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXSourceStartAddress function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXSourceStartAddressSet (   DMA_MODULE_ID index,
                                                DMA_CHANNEL dmaChannel,
												uint32_t sourceStartAddress);


//******************************************************************************
/* Function:
    uint32_t PLIB_DMA_ChannelXDestinationStartAddressGet (  DMA_MODULE_ID index,
                                                            DMA_CHANNEL dmaChannel )

  Summary:
    Reads the destination start address configured for the specified channel.

  Description:
    This function reads the destination start address configured for the specified
	channel.

  Precondition:
    None.

  Parameters:
    dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - uint32_t	- The destination start address configured for this channel

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint32_t DestinationStartAddress;
	DestinationStartAddress = PLIB_DMA_ChannelXDestinationStartAddressGet (
                                                                DMA_ID_0,
                                                                spiDMAChannel );
    </code>

  Remarks:
	This function implements an operation of the ChannelXDestinationStartAddress feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXDestinationStartAddress function in your application to automatically
	determine whether this feature is available.
*/

uint32_t PLIB_DMA_ChannelXDestinationStartAddressGet (  DMA_MODULE_ID index,
                                                        DMA_CHANNEL dmaChannel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXDestinationStartAddressSet (
                                            DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel,
											uint32_t destinationStartAddress)

  Summary:
    Writes the specified destination start address into the register corresponding to
    the specified channel.

  Description:
    This function writes the specified destination start address into the register
    corresponding to the specified channel.

  Precondition:
    None.

  Parameters:
    dmaChannel	               - One of the possible DMA channels listed by DMA_CHANNEL
	destinationStartAddress	   - The destination start address

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint32_t destinationStartAddress = 0x00FDEA00;
	PLIB_DMA_ChannelXDestinationStartAddressSet(    DMA_ID_0,
                                                    spiDMAChannel,
                                                    destinationStartAddress );
    </code>

  Remarks:
	This function implements an operation of the ChannelXDestinationStartAddress feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXDestinationStartAddress function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXDestinationStartAddressSet (
                                            DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel,
											uint32_t destinationStartAddress);


// *****************************************************************************
// *****************************************************************************
// Section: DMA Data control Interface functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    uint16_t PLIB_DMA_ChannelXSourceSizeGet (   DMA_MODULE_ID index,
                                                DMA_CHANNEL dmaChannel )

  Summary:
    Reads the source size configured for the specified channel.

  Description:
    This function reads the source size configured for the specified
	channel.

  Precondition:
    None.

  Parameters:
    dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - uint16_t	- The Source size configured (in bytes) for this channel.

    The source size (8-bit, 16-bit) is device-specific. Please refer to the specific
    device data sheet to determine availability.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint16_t sourceSize;
	sourceSize = PLIB_DMA_ChannelXSourceSizeGet (   DMA_ID_0,
                                                    spiDMAChannel );
    </code>

  Remarks:
	This function implements an operation of the ChannelXSourceSize feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXSourceSize function in your application to automatically
	determine whether this feature is available.
*/

uint16_t PLIB_DMA_ChannelXSourceSizeGet (   DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXSourceSizeSet (   DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel,
										    uint16_t sourceSize)

  Summary:
    Writes the specified source size into the register corresponding
	to the specified channel.

  Description:
    This function writes the specified source size into the register
	corresponding to the specified channel.

  Precondition:
    None.

  Parameters:
    dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL
	sourceSize	- The source size. The source size (8-bit, 16-bit) is
	              device-specific. Please refer to the specific device data
				  sheet to determine availability.

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint16_t sourceSize = 0xA00;
	PLIB_DMA_ChannelXSourceSizeSet (    DMA_ID_0,
                                        spiDMAChannel,
                                        sourceSize );
    </code>

  Remarks:
	This function implements an operation of the ChannelXSourceSize feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXSourceSize function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXSourceSizeSet (   DMA_MODULE_ID index,
                                        DMA_CHANNEL dmaChannel,
										uint16_t sourceSize);


//******************************************************************************
/* Function:
    uint16_t PLIB_DMA_ChannelXDestinationSizeGet (  DMA_MODULE_ID index,
                                                    DMA_CHANNEL dmaChannel )

  Summary:
    Reads the destination size configured for the specified channel.

  Description:
    This function reads the destination size configured for the specified
	channel.

  Precondition:
    None.

  Parameters:
    dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - uint16_t	- The destination size configured (in bytes) for this channel.

    The destination size (8-bit, 16-bit) is device-specific. Please refer to the specific
    device data sheet to determine availability.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint16_t DestinationSize;
	DestinationSize = PLIB_DMA_ChannelXDestinationSizeGet ( DMA_ID_0,
                                                            spiDMAChannel );
    </code>

  Remarks:
	This function implements an operation of the ChannelXDestinationSize feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXDestinationSize function in your application to automatically
	determine whether this feature is available.
*/

uint16_t PLIB_DMA_ChannelXDestinationSizeGet (  DMA_MODULE_ID index,
                                                DMA_CHANNEL dmaChannel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXDestinationSizeSet (  DMA_MODULE_ID index,
                                                DMA_CHANNEL dmaChannel,
                                                uint16_t destinationSize)

  Summary:
    Writes the specified destination size into the register corresponding
	to the specified channel.

  Description:
    This function writes the specified destination size into the register
	corresponding to the specified channel.

  Precondition:
    None.

  Parameters:
    dmaChannel	    - One of the possible DMA channels listed by DMA_CHANNEL
	destinationSize	- The destination size. The destination size (8-bit, 16-bit)
                      is device-specific. Please refer to the specific device
					  data sheet to determine availability.)

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint16_t destinationSize = 0xA00;
	PLIB_DMA_ChannelXDestinationSizeSet( DMA_ID_0, spiDMAChannel, destinationSize );
    </code>

  Remarks:
	This function implements an operation of the ChannelXDestinationSize feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXDestinationSize function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXDestinationSizeSet (  DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel,
                                            uint16_t destinationSize);


//******************************************************************************
/* Function:
    uint16_t PLIB_DMA_ChannelXSourcePointerGet (   DMA_MODULE_ID index,
                                                DMA_CHANNEL dmaChannel )

  Summary:
    Reads the current byte of the source being pointed to for the specified channel.

  Description:
    This function reads the current byte of the source being pointed to for the
	specified channel.

  Precondition:
    None.

  Parameters:
    dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - uint16_t	- The source byte being pointed to for this channel.

    The source pointer (8-bit, 16-bit) is device-specific. Please refer to the specific
    device data sheet to determine availability.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint16_t sourcebyte;
	sourcebyte = PLIB_DMA_ChannelXSourcePointerGet ( DMA_ID_0, spiDMAChannel );
    </code>

  Remarks:
	This function implements an operation of the ChannelXSourcePointer feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXSourcePointer function in your application to automatically
	determine whether this feature is available.
*/

uint16_t PLIB_DMA_ChannelXSourcePointerGet (   DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel );


//******************************************************************************
/* Function:
    uint16_t PLIB_DMA_ChannelXDestinationPointerGet (  DMA_MODULE_ID index,
                                                    DMA_CHANNEL dmaChannel )

  Summary:
    Reads the current byte of the destination being pointed to for the specified channel.

  Description:
    This function reads the current byte of the destination being pointed to for the
	specified channel.

  Precondition:
    None.

  Parameters:
    dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - uint16_t	- The destination byte being pointed to for this channel.

    The destination pointer (8-bit, 16-bit) is device-specific. Please refer to the specific
    device data sheet to determine availability.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint16_t destinationbyte;
	destinationbyte = PLIB_DMA_ChannelXDestinationPointerGet ( DMA_ID_0,
                                                            spiDMAChannel );
    </code>

  Remarks:
	This function implements an operation of the ChannelXDestinationPointer feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXDestinationPointer function in your application to automatically
	determine whether this feature is available.
*/

uint16_t PLIB_DMA_ChannelXDestinationPointerGet (  DMA_MODULE_ID index,
                                                DMA_CHANNEL dmaChannel );


//******************************************************************************
/* Function:
    uint16_t PLIB_DMA_ChannelXCellSizeGet ( DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel )

  Summary:
    Reads the cell size (in bytes) configured for the specified channel.

  Description:
    This function reads the cell size (in bytes) configured for the specified
	channel.

  Precondition:
    None.

  Parameters:
    dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - uint16_t	- The cell size configured (in bytes) for this channel

    The cell size (8-bit, 16-bit) is device-specific. Please refer to the specific
    device data sheet to determine availability.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint16_t cellSize;
	cellSize = PLIB_DMA_ChannelXCellSizeGet (   DMA_ID_0,
                                                spiDMAChannel );
    </code>

  Remarks:
	This function implements an operation of the ChannelXCellSize feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXCellSize function in your application to automatically
	determine whether this feature is available.
*/

uint16_t PLIB_DMA_ChannelXCellSizeGet ( DMA_MODULE_ID index,
                                        DMA_CHANNEL dmaChannel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXCellSizeSet ( DMA_MODULE_ID index,
                                        DMA_CHANNEL dmaChannel,
										uint16_t cellSize)

  Summary:
    Writes the specified cell size into the register corresponding
	to the specified channel.

  Description:
    This function writes the specified cell size into the register
	corresponding to the specified channel.

  Precondition:
    None.

  Parameters:
    dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL
    cellSize	- The cell size in bytes. The cell size (8-bit, 16-bit) is
	              device-specific. Please refer to the specific device data
				  sheet to determine availability.)

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint16_t cellSize = 0x10;
	PLIB_DMA_ChannelXCellSizeSet ( DMA_ID_0, spiDMAChannel, cellSize );
    </code>

  Remarks:
	This function implements an operation of the ChannelXCellSize feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXCellSize function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXCellSizeSet ( DMA_MODULE_ID index,
                                    DMA_CHANNEL dmaChannel,
									uint16_t CellSize);


//******************************************************************************
/* Function:
    uint16_t PLIB_DMA_ChannelXCellProgressPointerGet ( DMA_MODULE_ID index,
                                                    DMA_CHANNEL dmaChannel )

  Summary:
    Returns the number of bytes transferred since the last event.

  Description:
    This function returns the number of bytes transferred since the last event.

  Precondition:
    None.

  Parameters:
    dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - uint16_t	- The number of bytes transferred since the last event.

    The cell progress pointer (8-bit, 16-bit) is device-specific. Please refer to
	the specific device data sheet to determine availability.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint16_t CellProgress;
	CellProgress = PLIB_DMA_ChannelXCellProgressPointerGet (   DMA_ID_0,
                                                            spiDMAChannel );
    </code>

  Remarks:
	This function implements an operation of the ChannelXCellProgressPointer feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXCellProgressPointer function in your application to automatically
	determine whether this feature is available.
*/

uint16_t PLIB_DMA_ChannelXCellProgressPointerGet ( DMA_MODULE_ID index,
                                                DMA_CHANNEL dmaChannel );


//******************************************************************************
/* Function:
    uint16_t PLIB_DMA_ChannelXPatternDataGet (   DMA_MODULE_ID index,
                                                DMA_CHANNEL dmaChannel )

  Summary:
    Returns the pattern matching (for DMA abort) data programmed for the specified
	channel.

  Description:
    This function returns pattern matching (for DMA abort) data programmed for
	the specified channel.
    (The size of pattern matching data(8-bit, 16-bit) is device-specific. Please refer to
	the specific device data sheet to determine availability.)

  Precondition:
    None.

  Parameters:
    dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - uint16_t	- The pattern matching data programmed for the current channel.

    The size of pattern matching data(8-bit, 16-bit) is device-specific. Please refer to
	the specific device data sheet to determine availability.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint16_t patternData;
	patternData = PLIB_DMA_ChannelXPatternDataGet ( DMA_ID_0, spiDMAChannel );
    </code>

  Remarks:
	This function implements an operation of the ChannelXPatternData feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXPatternData function in your application to automatically
	determine whether this feature is available.
*/

uint16_t PLIB_DMA_ChannelXPatternDataGet (   DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXPatternDataSet (  DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel,
										    uint16_t patternData)

  Summary:
    Writes the specified pattern matching data (for DMA abort) into the register
	corresponding to the specified channel.

  Description:
    This function writes the specified pattern matching data (for DMA abort) into the
	register corresponding to the specified channel.
    (The size of pattern matching data(8-bit, 16-bit) is device-specific. Please refer to
	the specific device data sheet to determine availability.)

  Precondition:
    None.

  Parameters:
    dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL
    patternData	- The pattern matching DATA programmed for the current channel
    (The size of pattern matching data(8-bit, 16-bit) is device-specific. Please refer to
	the specific device data sheet to determine availability.)

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	uint16_t patternData = '\0';
	PLIB_DMA_ChannelXPatternDataSet ( DMA_ID_0, spiDMAChannel, patternData );
    </code>

  Remarks:
	This function implements an operation of the ChannelXPatternData feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXPatternData function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXPatternDataSet (  DMA_MODULE_ID index,
                                        DMA_CHANNEL dmaChannel,
                                        uint16_t patternData);

// *****************************************************************************
// *****************************************************************************
// Section: DMA Interrupt Control Interface functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    bool PLIB_DMA_ChannelXINTSourceFlagGet (    DMA_MODULE_ID index,
                                                DMA_CHANNEL dmaChannel,
                                                DMA_INT_TYPE dmaINTSource )

  Summary:
    Returns the status of the interrupt flag of the specified DMA interrupt source
	for the specified channel.

  Description:
    This function returns the status of the interrupt flag of the specified DMA interrupt
	source for the specified channel.

  Precondition:
    None.

  Parameters:
	dmaChannel	  - One of the possible DMA channels listed by DMA_CHANNEL
	dmaINTSource  - One of the DMA interrupt sources specified by DMA_INT_TYPE

  Returns:
    - true	- The interrupt flag is set
    - false	- The interrupt flag is not set

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	bool AddressErrorINTStatus;
	AddressErrorINTStatus = PLIB_DMA_ChannelXINTSourceFlagGet (
                                                        DMA_ID_0,
                                                        spiDMAChannel,
                                                        DMA_INT_ADDRESS_ERROR );
    </code>

  Remarks:
	This function implements an operation of the ChannelXINTSourceFlag feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXINTSourceFlag function in your application to automatically
	determine whether this feature is available.
*/

bool PLIB_DMA_ChannelXINTSourceFlagGet (    DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel,
                                            DMA_INT_TYPE dmaINTSource );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXINTSourceFlagSet (    DMA_MODULE_ID index,
                                                DMA_CHANNEL dmaChannel,
                                                DMA_INT_TYPE dmaINTSource )

  Summary:
    Sets the interrupt flag of the specified DMA interrupt source for the specified channel.

  Description:
    This function sets the interrupt flag of the specified DMA interrupt source for
	the specified channel.

  Precondition:
    None.

  Parameters:
	dmaChannel	 - One of the possible DMA channels listed by DMA_CHANNEL
	dmaINTSource - One of the DMA interrupt sources specified by DMA_INT_TYPE

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	PLIB_DMA_ChannelXINTSourceFlagSet ( DMA_ID_0,
                                        spiDMAChannel,
                                        DMA_INT_ADDRESS_ERROR );
    </code>

  Remarks:
	This function implements an operation of the ChannelXINTSourceFlag feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXINTSourceFlag function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXINTSourceFlagSet (    DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel,
                                            DMA_INT_TYPE dmaINTSource );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXINTSourceFlagClear (  DMA_MODULE_ID index,
                                                DMA_CHANNEL dmaChannel,
                                                DMA_INT_TYPE dmaINTSource )

  Summary:
    Clears the interrupt flag of the specified DMA interrupt source
	for the specified channel.

  Description:
    This function clears the interrupt flag of the specified DMA interrupt source
	for the specified channel.

  Precondition:
    None.

  Parameters:
    dmaChannel		- One of the possible DMA channels listed by DMA_CHANNEL
    dmaINTSource	- One of the DMA interrupt sources specified by DMA_INT_TYPE

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	PLIB_DMA_ChannelXINTSourceFlagClear (   DMA_ID_0,
                                            spiDMAChannel,
                                            DMA_INT_ADDRESS_ERROR );
    </code>

  Remarks:
	This function implements an operation of the ChannelXINTSourceFlag feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXINTSourceFlag function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXINTSourceFlagClear (  DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel,
                                            DMA_INT_TYPE dmaINTSource );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXINTSourceEnable ( DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel,
											DMA_INT_TYPE dmaINTSource )

  Summary:
    Enables the specified interrupt source for the specified channel.

  Description:
    This function enables the specified interrupt source for the specified channel.

  Precondition:
    None.

  Parameters:
    dmaChannel      - One of the possible DMA channels listed by DMA_CHANNEL
    dmaINTSource    - One of the DMA interrupt sources specified by DMA_INT_TYPE

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	PLIB_DMA_ChannelXINTSourceEnable (  DMA_ID_0,
                                        spiDMAChannel,
                                        DMA_INT_ADDRESS_ERROR );
    </code>

  Remarks:
	This function implements an operation of the ChannelXINTSource feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXINTSource function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXINTSourceEnable ( DMA_MODULE_ID index,
                                        DMA_CHANNEL dmaChannel,
									    DMA_INT_TYPE dmaINTSource );


//******************************************************************************
/* Function:
    void PLIB_DMA_ChannelXINTSourceDisable (    DMA_MODULE_ID index,
                                                DMA_CHANNEL dmaChannel,
                                                DMA_INT_TYPE dmaINTSource )

  Summary:
    Disables the specified interrupt source for the specified channel.

  Description:
    This function disables the specified interrupt source for the specified channel.

  Precondition:
    None.

  Parameters:
	dmaChannel	 - One of the possible DMA channels listed by DMA_CHANNEL
	dmaINTSource - One of the DMA interrupt sources specified by DMA_INT_TYPE

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	PLIB_DMA_ChannelXINTSourceDisable ( DMA_ID_0,
                                        spiDMAChannel,
                                        DMA_INT_ADDRESS_ERROR );
    </code>

  Remarks:
	This function implements an operation of the ChannelXINTSource feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXINTSource function in your application to automatically
	determine whether this feature is available.
*/

void PLIB_DMA_ChannelXINTSourceDisable (    DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel,
                                            DMA_INT_TYPE dmaINTSource );


//******************************************************************************
/* Function:
    bool PLIB_DMA_ChannelXINTSourceIsEnabled (  DMA_MODULE_ID index,
                                                DMA_CHANNEL dmaChannel,
                                                DMA_INT_TYPE dmaINTSource )

  Summary:
    Returns the enable status of the specified interrupt source for the specified channel.

  Description:
    This function returns the enable status of the specified interrupt source for the
	specified channel.

  Precondition:
    None.

  Parameters:
    dmaChannel		- One of the possible DMA channels listed by DMA_CHANNEL
	dmaINTSource	- One of the DMA interrupt sources specified by DMA_INT_TYPE

  Returns:
    - true	- The interrupt is enabled
    - false	- The interrupt is not enabled

  Example:
    <code>
	DMA_CHANNEL spiDMAChannel = DMA_CHANNEL_2;
	bool dmaINTSourceEnableStatus;
	dmaINTSourceEnableStatus = PLIB_DMA_ChannelXINTSourceIsEnabled (
                                                        DMA_ID_0,
                                                        spiDMAChannel,
                                                        DMA_INT_ADDRESS_ERROR );
    </code>

  Remarks:
	This function implements an operation of the ChannelXINTSource feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXINTSource function in your application to automatically
	determine whether this feature is available.
*/

bool PLIB_DMA_ChannelXINTSourceIsEnabled (  DMA_MODULE_ID index,
                                            DMA_CHANNEL dmaChannel,
                                            DMA_INT_TYPE dmaINTSource );


//******************************************************************************
/* Function:
	void PLIB_DMA_ChannelXPatternLengthSet(  DMA_MODULE_ID index,
						DMA_CHANNEL dmaChannel,DMA_PATTERN_LENGTH patternLen )

  Summary:
    Sets the pattern match length.

  Description:
    This function sets the length of the pattern match ignore to 1 or 2 bytes.

  Precondition:
    None.

  Parameters:
	dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL
	patternLen	- Length of pattern match (either 1 or 2)

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL dmaChannel = DMA_CHANNEL_2;
	DMA_PATTERN_LENGTH patternLen;
	patternLen = DMA_PATTERN_MATCH_LENGTH_1BYTE;
	PLIB_DMA_ChannelXPatternLengthSet(DMA_ID_0, dmaChannel, patternLen);
    </code>

  Remarks:
	This function implements an operation of the ChannelXPatternLength feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXPatternLength function in your application to automatically
	determine whether this feature is available.
*/
void PLIB_DMA_ChannelXPatternLengthSet(  DMA_MODULE_ID index,
										 DMA_CHANNEL dmaChannel,
   										 DMA_PATTERN_LENGTH patternLen );



//******************************************************************************
/* Function:
	DMA_PATTERN_LENGTH PLIB_DMA_ChannelXPatternLengthGet(  DMA_MODULE_ID index,
													DMA_CHANNEL dmaChannel);

  Summary:
    Returns the pattern match length.

  Description:
    This function returns the length of the byte matching the CHPIGN bits during a
    pattern match that may be ignored during the pattern match determination when
    the CHPIGNEN bit is set.

  Precondition:
    None.

  Parameters:
	dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
	- patternLen	- Length of pattern match (either 1 or 2)

  Example:
    <code>
	DMA_CHANNEL dmaChannel = DMA_CHANNEL_2;
	DMA_PATTERN_LENGTH patternLen;
	patternLen = PLIB_DMA_ChannelXPatternLengthGet(DMA_ID_0, dmaChannel);
    </code>

  Remarks:
	This function implements an operation of the ChannelXPatternLength feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXPatternLength function in your application to automatically
	determine whether this feature is available.
*/
DMA_PATTERN_LENGTH PLIB_DMA_ChannelXPatternLengthGet(  DMA_MODULE_ID index,
													DMA_CHANNEL dmaChannel);


//******************************************************************************
/* Function:
	void PLIB_DMA_ChannelXPatternIgnoreByteEnable (  DMA_MODULE_ID index,
														DMA_CHANNEL channel );

  Summary:
    Enables the pattern match ignore byte.

  Description:
    This function enables the pattern match ignore byte.

  Precondition:
    None.

  Parameters:
    dmaChannel		- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL dmaChannel = DMA_CHANNEL_2;
	PLIB_DMA_ChannelXPatternIgnoreByteEnable(DMA_ID_0, dmaChannel);
    </code>

  Remarks:
	This function implements an operation of the ChannelXPatternIgnoreByte feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXPatternIgnoreByte function in your application to automatically
	determine whether this feature is available.
*/
void PLIB_DMA_ChannelXPatternIgnoreByteEnable (  DMA_MODULE_ID index,  DMA_CHANNEL channel );



//******************************************************************************
/* Function:
	bool PLIB_DMA_ChannelXPatternIgnoreByteIsEnabled  (  DMA_MODULE_ID index,
														DMA_CHANNEL channel );

  Summary:
    Returns the state of the pattern match ignore byte.

  Description:
    This function returns the state (enabled or disabled) of the pattern match
    ignore byte.

  Precondition:
    None.

  Parameters:
    dmaChannel		- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - true   - The pattern match ignore byte is enabled
    - false  - The pattern match ignore byte is disabled

  Example:
    <code>
	bool patternIsEnabled;
	DMA_CHANNEL dmaChannel = DMA_CHANNEL_2;
	patternIsEnabled = PLIB_DMA_ChannelXPatternIgnoreByteIsEnabled(DMA_ID_0, dmaChannel);
    </code>

  Remarks:
	This function implements an operation of the ChannelXPatternIgnoreByte feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXPatternIgnoreByte function in your application to automatically
	determine whether this feature is available.
*/
bool PLIB_DMA_ChannelXPatternIgnoreByteIsEnabled (   DMA_MODULE_ID index,  DMA_CHANNEL channel );


//******************************************************************************
/* Function:
	void PLIB_DMA_ChannelXPatternIgnoreByteDisable (  DMA_MODULE_ID index,
														DMA_CHANNEL channel );

  Summary:
    Disables the pattern match ignore byte.

  Description:
    This function disables the pattern match ignore byte.

  Precondition:
    None.

  Parameters:
    dmaChannel		- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    None.

  Example:
    <code>
	DMA_CHANNEL dmaChannel = DMA_CHANNEL_2;
	PLIB_DMA_ChannelXPatternIgnoreByteDisable(DMA_ID_0, dmaChannel);
    </code>

  Remarks:
	This function implements an operation of the ChannelXPatternIgnoreByte feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXPatternIgnoreByte function in your application to automatically
	determine whether this feature is available.
*/
void PLIB_DMA_ChannelXPatternIgnoreByteDisable ( DMA_MODULE_ID index,
													DMA_CHANNEL channel );



//******************************************************************************
/* Function:
	void PLIB_DMA_ChannelXPatternIgnoreSet (DMA_MODULE_ID index,
											DMA_CHANNEL channel, uint8_t pattern );

  Summary:
    Sets the pattern match ignore value.

  Description:
    This function sets the value of the pattern match ignore.

  Precondition:
    None.

  Parameters:
    dmaChannel		- One of the possible DMA channels listed by DMA_CHANNEL
    pattern		- Pattern match ignore value

  Returns:
    None.

  Example:
    <code>
	uint8_t patternMatch = 0x8;
	DMA_CHANNEL dmaChannel = DMA_CHANNEL_2;
	PLIB_DMA_ChannelXPatternIgnoreSet ( DMA_ID_0, dmaChannel,patternMatch);
    </code>

  Remarks:
	This function implements an operation of the ChannelXPatternIgnore feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXPatternIgnore function in your application to automatically
	determine whether this feature is available.
*/
void PLIB_DMA_ChannelXPatternIgnoreSet (DMA_MODULE_ID index,
											DMA_CHANNEL channel, uint8_t pattern );


//******************************************************************************
/* Function:
	uint8_t PLIB_DMA_ChannelXPatternIgnoreGet( DMA_MODULE_ID index,
												DMA_CHANNEL channel );

  Summary:
    Returns the pattern match ignore value.

  Description:
    This function returns the value of the pattern match ignore.

  Precondition:
    None.

  Parameters:
    dmaChannel	- One of the possible DMA channels listed by DMA_CHANNEL

  Returns:
    - uint8_t	- Pattern match ignore value

  Example:
    <code>
	uint8_t patternMatch;
	DMA_CHANNEL dmaChannel = DMA_CHANNEL_2;
	patternMatch = PLIB_DMA_ChannelXPatternIgnoreGet ( DMA_ID_0, dmaChannel);
    </code>

  Remarks:
	This function implements an operation of the ChannelXPatternIgnore feature.
	This feature may not be available on all devices.  Please refer to the
	specific device data sheet to determine availability or use the
	PLIB_DMA_ExistsChannelXPatternIgnore function in your application to automatically
	determine whether this feature is available.
*/
uint8_t PLIB_DMA_ChannelXPatternIgnoreGet( DMA_MODULE_ID index,  DMA_CHANNEL channel );

// *****************************************************************************
// *****************************************************************************
// Section: DMA Peripheral Library Exists API Routines
// *****************************************************************************
// *****************************************************************************
/* The following functions indicate the existence of the features on the device.
*/

//******************************************************************************
/* Function :  PLIB_DMA_ExistsBusy( DMA_MODULE_ID index )

  Summary:
    Identifies whether the Busy feature exists on the DMA module.

  Description:
    This function identifies whether the Busy feature is available on the
	DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_BusyActiveSet
    - PLIB_DMA_BusyActiveReset
    - PLIB_DMA_IsBusy

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Busy feature is supported on the device
    - false  - The Busy feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsBusy( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsSuspend( DMA_MODULE_ID index )

  Summary:
    Identifies whether the Suspend feature exists on the DMA module.

  Description:
    This function identifies whether the Suspend feature is available on the
	DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_SuspendEnable
    - PLIB_DMA_SuspendDisable
    - PLIB_DMA_SuspendIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Suspend feature is supported on the device
    - false  - The Suspend feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsSuspend( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsStopInIdle( DMA_MODULE_ID index )

  Summary:
    Identifies whether the StopInIdle feature exists on the DMA module.

  Description:
    This function identifies whether the StopInIdle feature is available on the
	DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_StopInIdleEnable
    - PLIB_DMA_StopInIdleDisable

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

bool PLIB_DMA_ExistsStopInIdle( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsEnableControl( DMA_MODULE_ID index )

  Summary:
    Identifies whether the EnableControl feature exists on the DMA module.

  Description:
    This function identifies whether the EnableControl feature is available on
	the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_Enable
    - PLIB_DMA_Disable
    - PLIB_DMA_IsEnabled

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

bool PLIB_DMA_ExistsEnableControl( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelBits( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelBits feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelBits feature is available on the
	DMA module.
    When this function returns true, this function is supported on the device:
    - PLIB_DMA_ChannelBitsGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelBits feature is supported on the device
    - false  - The ChannelBits feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelBits( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsLastBusAccess( DMA_MODULE_ID index )

  Summary:
    Identifies whether the LastBusAccess feature exists on the DMA module.

  Description:
    This function identifies whether the LastBusAccess feature is available on the
	DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_LastBusAccessIsRead
    - PLIB_DMA_LastBusAccessIsWrite

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The LastBusAccess feature is supported on the device
    - false  - The LastBusAccess feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsLastBusAccess( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsRecentAddress( DMA_MODULE_ID index )

  Summary:
    Identifies whether the RecentAddress feature exists on the DMA module.

  Description:
    This function identifies whether the RecentAddress feature is available on
	the DMA module.
    When this function returns true, this function is supported on the device:
    - PLIB_DMA_RecentAddressAccessed

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The RecentAddress feature is supported on the device
    - false  - The RecentAddress feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsRecentAddress( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsCRCChannel( DMA_MODULE_ID index )

  Summary:
    Identifies whether the CRCChannel feature exists on the DMA module.

  Description:
    This function identifies whether the CRCChannel feature is available on the
	DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_CRCChannelSelect
    - PLIB_DMA_CRCChannelGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CRCChannel feature is supported on the device
    - false  - The CRCChannel feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsCRCChannel( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsCRCType( DMA_MODULE_ID index )

  Summary:
    Identifies whether the CRCType feature exists on the DMA module.

  Description:
    This function identifies whether the CRCType feature is available on the
	DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_CRCTypeGet
    - PLIB_DMA_CRCTypeSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CRCType feature is supported on the device
    - false  - The CRCType feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsCRCType( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsCRCAppendMode( DMA_MODULE_ID index )

  Summary:
    Identifies whether the CRCAppendMode feature exists on the DMA module.

  Description:
    This function identifies whether the CRCAppendMode feature is available on
	the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_CRCAppendModeEnable
    - PLIB_DMA_CRCAppendModeDisable
    - PLIB_DMA_CRCAppendModeIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CRCAppendMode feature is supported on the device
    - false  - The CRCAppendMode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsCRCAppendMode( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsCRC( DMA_MODULE_ID index )

  Summary:
    Identifies whether the CRC feature exists on the DMA module.

  Description:
    This function identifies whether the CRC feature is available on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_CRCEnable
    - PLIB_DMA_CRCDisable
    - PLIB_DMA_CRCIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CRC feature is supported on the device
    - false  - The CRC feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsCRC( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsCRCPolynomialLength( DMA_MODULE_ID index )

  Summary:
    Identifies whether the CRCPolynomialLength feature exists on the DMA module.

  Description:
    This function identifies whether the CRCPolynomialLength feature is available
	on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_CRCPolynomialLengthSet
    - PLIB_DMA_CRCPolynomialLengthGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CRCPolynomialLength feature is supported on the device
    - false  - The CRCPolynomialLength feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsCRCPolynomialLength( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsCRCBitOrder( DMA_MODULE_ID index )

  Summary:
    Identifies whether the CRCBitOrder feature exists on the DMA module.

  Description:
    This function identifies whether the CRCBitOrder feature is available on the
	DMA module.
    When this function returns true, this function is supported on the device:
    - PLIB_DMA_CRCBitOrderSelect

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CRCBitOrder feature is supported on the device
    - false  - The CRCBitOrder feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsCRCBitOrder( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsCRCWriteByteOrder( DMA_MODULE_ID index )

  Summary:
    Identifies whether the CRCWriteByteOrder feature exists on the DMA module.

  Description:
    This function identifies whether the CRCWriteByteOrder feature is available
	on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_CRCWriteByteOrderAlter
    - PLIB_DMA_CRCWriteByteOrderMaintain

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CRCWriteByteOrder feature is supported on the device
    - false  - The CRCWriteByteOrder feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsCRCWriteByteOrder( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsCRCByteOrder( DMA_MODULE_ID index )

  Summary:
    Identifies whether the CRCByteOrder feature exists on the DMA module.

  Description:
    This function identifies whether the CRCByteOrder feature is available on
	the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_CRCByteOrderSelect
    - PLIB_DMA_CRCByteOrderGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CRCByteOrder feature is supported on the device
    - false  - The CRCByteOrder feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsCRCByteOrder( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsCRCData( DMA_MODULE_ID index )

  Summary:
    Identifies whether the CRCData feature exists on the DMA module.

  Description:
    This function identifies whether the CRCData feature is available on the
	DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_CRCDataRead
    - PLIB_DMA_CRCDataWrite

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CRCData feature is supported on the device
    - false  - The CRCData feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsCRCData( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsCRCXOREnable( DMA_MODULE_ID index )

  Summary:
    Identifies whether the CRCXOREnable feature exists on the DMA module.

  Description:
    This function identifies whether the CRCXOREnable feature is available on the
	DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_CRCXOREnableSet
    - PLIB_DMA_CRCXOREnableGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CRCXOREnable feature is supported on the device
    - false  - The CRCXOREnable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsCRCXOREnable( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXPriority( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXPriority feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXPriority feature is available on
	the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXPrioritySelect
    - PLIB_DMA_ChannelXPriorityGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXPriority feature is supported on the device
    - false  - The ChannelXPriority feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXPriority( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXEvent( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXEvent feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXEvent feature is available on
	the DMA module.
    When this function returns true, this function is supported on the device:
    - PLIB_DMA_ChannelXEventIsDetected

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXEvent feature is supported on the device
    - false  - The ChannelXEvent feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXEvent( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXAuto( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXAuto feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXAuto feature is available on
	the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXAutoEnable
    - PLIB_DMA_ChannelXAutoDisable
    - PLIB_DMA_ChannelXAutoIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXAuto feature is supported on the device
    - false  - The ChannelXAuto feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXAuto( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXChainEnbl( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXChainEnbl feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXChainEnbl feature is available
	on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXChainEnable
    - PLIB_DMA_ChannelXChainDisable
    - PLIB_DMA_ChannelXChainIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXChainEnbl feature is supported on the device
    - false  - The ChannelXChainEnbl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXChainEnbl( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXDisabled( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXDisabled feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXDisabled feature is available
	on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXDisabledEnablesEvents
    - PLIB_DMA_ChannelXDisabledDisablesEvents

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXDisabled feature is supported on the device
    - false  - The ChannelXDisabled feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXDisabled( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelX( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelX feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelX feature is available on the
	DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXEnable
    - PLIB_DMA_ChannelXIsEnabled
    - PLIB_DMA_ChannelXDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelX feature is supported on the device
    - false  - The ChannelX feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelX( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXChain( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXChain feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXChain feature is available on the
	DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXChainToLower
    - PLIB_DMA_ChannelXChainToHigher

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXChain feature is supported on the device
    - false  - The ChannelXChain feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXChain( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXPatternLength( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXPatternLength feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXPatternLength feature is available
	on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXPatternLengthSet
    - PLIB_DMA_ChannelXPatternLengthGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXPatternLength feature is supported on the device
    - false  - The ChannelXPatternLength feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXPatternLength( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXPatternIgnoreByte( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXPatternIgnoreByte feature exists on the DMA
	module.

  Description:
    This function identifies whether the ChannelXPatternIgnoreByte feature is
	available on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXPatternIgnoreByteEnable
    - PLIB_DMA_ChannelXPatternIgnoreByteIsEnabled
    - PLIB_DMA_ChannelXPatternIgnoreByteDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXPatternIgnoreByte feature is supported on the device
    - false  - The ChannelXPatternIgnoreByte feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXPatternIgnoreByte( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXBusy( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXBusy feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXBusy feature is available on the
	DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXBusyActiveSet
    - PLIB_DMA_ChannelXBusyInActiveSet
    - PLIB_DMA_ChannelXBusyIsBusy

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXBusy feature is supported on the device
    - false  - The ChannelXBusy feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXBusy( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXPatternIgnore( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXPatternIgnore feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXPatternIgnore feature is available
	on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXPatternIgnoreSet
    - PLIB_DMA_ChannelXPatternIgnoreGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXPatternIgnore feature is supported on the device
    - false  - The ChannelXPatternIgnore feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXPatternIgnore( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXTrigger( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXTrigger feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXTrigger feature is available on
	the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXTriggerEnable
    - PLIB_DMA_ChannelXTriggerIsEnabled
    - PLIB_DMA_ChannelXTriggerDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXTrigger feature is supported on the device
    - false  - The ChannelXTrigger feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXTrigger( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsAbortTransfer( DMA_MODULE_ID index )

  Summary:
    Identifies whether the AbortTransfer feature exists on the DMA module.

  Description:
    This function identifies whether the AbortTransfer feature is available on
	the DMA module.
    When this function returns true, this function is supported on the device:
    - PLIB_DMA_AbortTransferSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The AbortTransfer feature is supported on the device
    - false  - The AbortTransfer feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsAbortTransfer( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsStartTransfer( DMA_MODULE_ID index )

  Summary:
    Identifies whether the StartTransfer feature exists on the DMA module.

  Description:
    This function identifies whether the StartTransfer feature is available on
	the DMA module.
    When this function returns true, this function is supported on the device:
    - PLIB_DMA_StartTransferSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The StartTransfer feature is supported on the device
    - false  - The StartTransfer feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsStartTransfer( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXStartIRQ( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXStartIRQ feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXStartIRQ feature is available
	on the DMA module.
    When this function returns true, this function is supported on the device:
    - PLIB_DMA_ChannelXStartIRQSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXStartIRQ feature is supported on the device
    - false  - The ChannelXStartIRQ feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXStartIRQ( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXAbortIRQ( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXAbortIRQ feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXAbortIRQ feature is available
	on the DMA module.
    When this function returns true, this function is supported on the device:
    - PLIB_DMA_ChannelXAbortIRQSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXAbortIRQ feature is supported on the device
    - false  - The ChannelXAbortIRQ feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXAbortIRQ( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXINTSourceFlag( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXINTSourceFlag feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXINTSourceFlag feature is available
	on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXINTSourceFlagGet
    - PLIB_DMA_ChannelXINTSourceFlagSet
    - PLIB_DMA_ChannelXINTSourceFlagClear

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXINTSourceFlag feature is supported on the device
    - false  - The ChannelXINTSourceFlag feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXINTSourceFlag( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXINTSource( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXINTSource feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXINTSource feature is available
	on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXINTSourceEnable
    - PLIB_DMA_ChannelXINTSourceDisable
    - PLIB_DMA_ChannelXINTSourceIsEnabled

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXINTSource feature is supported on the device
    - false  - The ChannelXINTSource feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXINTSource( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXSourceStartAddress( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXSourceStartAddress feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXSourceStartAddress feature is available
	on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXSourceStartAddressGet
    - PLIB_DMA_ChannelXSourceStartAddressSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXSourceStartAddress feature is supported on the device
    - false  - The ChannelXSourceStartAddress feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXSourceStartAddress( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXDestinationStartAddress( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXDestinationStartAddress feature exists on
	the DMA module.

  Description:
    This function identifies whether the ChannelXDestinationStartAddress feature
	is available on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXDestinationStartAddressGet
    - PLIB_DMA_ChannelXDestinationStartAddressSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXDestinationStartAddress feature is supported on the device
    - false  - The ChannelXDestinationStartAddress feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXDestinationStartAddress( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXSourceSize( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXSourceSize feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXSourceSize feature is available
	on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXSourceSizeGet
    - PLIB_DMA_ChannelXSourceSizeSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXSourceSize feature is supported on the device
    - false  - The ChannelXSourceSize feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXSourceSize( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXDestinationSize( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXDestinationSize feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXDestinationSize feature is available
	on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXDestinationSizeGet
    - PLIB_DMA_ChannelXDestinationSizeSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXDestinationSize feature is supported on the device
    - false  - The ChannelXDestinationSize feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXDestinationSize( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXSourcePointer( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXSourcePointer feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXSourcePointer feature is available
	on the DMA module.
    When this function returns true, this function is supported on the device:
    - PLIB_DMA_ChannelXSourcePointerGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXSourcePointer feature is supported on the device
    - false  - The ChannelXSourcePointer feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXSourcePointer( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXDestinationPointer( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXDestinationPointer feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXDestinationPointer feature is available
	on the DMA module.
    When this function returns true, this function is supported on the device:
    - PLIB_DMA_ChannelXDestinationPointerGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXDestinationPointer feature is supported on the device
    - false  - The ChannelXDestinationPointer feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXDestinationPointer( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXCellSize( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXCellSize feature exists on the DMA module.

  Description:
    This function identifies whether the ChannelXCellSize feature is available on
	the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXCellSizeGet
    - PLIB_DMA_ChannelXCellSizeSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXCellSize feature is supported on the device
    - false  - The ChannelXCellSize feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXCellSize( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXCellProgressPointer( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXCellProgressPointer feature exists on the
	DMA module.

  Description:
    This function identifies whether the ChannelXCellProgressPointer feature is
	available on the DMA module.
    When this function returns true, this function is supported on the device:
    - PLIB_DMA_ChannelXCellProgressPointerGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXCellProgressPointer feature is supported on the device
    - false  - The ChannelXCellProgressPointer feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXCellProgressPointer( DMA_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_DMA_ExistsChannelXPatternData( DMA_MODULE_ID index )

  Summary:
    Identifies whether the ChannelXPatternData feature exists on the DMA module

  Description:
    This function identifies whether the ChannelXPatternData feature is available
	on the DMA module.
    When this function returns true, these functions are supported on the device:
    - PLIB_DMA_ChannelXPatternDataGet
    - PLIB_DMA_ChannelXPatternDataSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ChannelXPatternData feature is supported on the device
    - false  - The ChannelXPatternData feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_DMA_ExistsChannelXPatternData( DMA_MODULE_ID index );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _PLIB_DMA_H

/*******************************************************************************
 End of File
*/
