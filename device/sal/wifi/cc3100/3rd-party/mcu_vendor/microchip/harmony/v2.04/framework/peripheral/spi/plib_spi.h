/*******************************************************************************
  SPI Peripheral Library Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    plib_spi.h

  Summary:
    SPI Peripheral Library Interface Header for common definitions.

  Description:
    This header file contains the function prototypes and definitions of
    the data types and constants that make up the interface to the SPI
    PLIB.
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

#ifndef _PLIB_SPI_H
#define _PLIB_SPI_H

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
    that are also included.
 */

#include "peripheral/spi/processor/spi_processor.h"

// *****************************************************************************
// *****************************************************************************
// Section: Constants & Data Types
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: SPI Peripheral Library Interface Routines
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void PLIB_SPI_Enable( SPI_MODULE_ID index )

 Summary:
    Enables the SPI module.

 Description:
    This function enables the SPI module.
    This operation is atomic.

 Precondition:
    None.

 Parameters:
    index           - Identifier for the device instance to be configured

 Returns:
    None.

 Example:
   <code>
   // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
   // application developer.
   PLIB_SPI_Enable(MY_SPI_INSTANCE);
   </code>

 Remarks:
    The SCKx, SDOx, SDIx and SSx pins must be assigned to available RPn pins
    before use.

    This function implements an operation of the enable control feature. This
    feature may not be available on all devices.  Please refer to the specific
    device data sheet to determine availability or use "PLIB_SPI_ExistsEnableControl"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_Enable( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_SPI_Disable (SPI_MODULE_ID index)

 Summary:
    Disables the SPI module.

 Description:
    This function disables the SPI module.
    This operation is atomic.

 Precondition:
    None.

 Parameters:
    index           - Identifier for the device instance to be configured

 Returns:
    None.

 Example:
   <code>
   // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
   // application developer.
   PLIB_SPI_Disable(MY_SPI_INSTANCE);
   </code>

 Remarks:
    This function implements an operation of the enable control feature. This
    feature may not be available on all devices.  Please refer to the specific
    device data sheet to determine availability or use "PLIB_SPI_ExistsEnableControl"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_Disable(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
   void PLIB_SPI_StopInIdleEnable (SPI_MODULE_ID index)

 Summary:
   Discontinues module operation when the device enters Idle mode.

 Description:
   This function sets up the SPI module such that module operation is disabled
   when the device enters Idle mode.
    This operation is atomic.

 Precondition:
   None.

 Parameters:
   index           - Identifier for the device instance to be configured

 Returns:
   None.

 Example:
   <code>
   // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
   // application developer.
   PLIB_SPI_StopInIdleEnable(MY_SPI_INSTANCE);
   </code>

 Remarks:
    This function implements an operation of the stop in idle control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use
    "PLIB_SPI_ExistsStopInIdleControl" in your application to automatically
    determine if this feature is available.
 */

void PLIB_SPI_StopInIdleEnable(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
   void PLIB_SPI_StopInIdleDisable (SPI_MODULE_ID index)

 Summary:
   Continues module operation when the device enters Idle mode.

 Description:
   This function sets up the SPI module such that module operation is continued
   when the device enters Idle mode.
    This operation is atomic.

 Precondition:
   None.

 Parameters:
   index           - Identifier for the device instance to be configured

 Returns:
   None.

 Example:
   <code>
   // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
   // application developer.
   PLIB_SPI_StopInIdleDisable(MY_SPI_INSTANCE);
   </code>

 Remarks:
    This function implements an operation of the stop in idle control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use
    "PLIB_SPI_ExistsStopInIdleControl" in your application to determine whether
    this feature is available.
 */

void PLIB_SPI_StopInIdleDisable(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_SPI_ReceiverHasOverflowed (SPI_MODULE_ID index)

  Summary:
    Returns the current status of the SPI receiver overflow.

  Description:
    This function returns the current status of the SPI receiver overflow.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    SPI receiver overflow status:
    - true  - A new byte/word is completely received and discarded.
              The user software has not read the previous data in
              the SPI buffer register.
    - false - No Overflow has occurred

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    bool overflowstate = PLIB_SPI_ReceiverHasOverflowed(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the receiver overflow status feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsReceiverOverflow"
    in your application to automatically determine whether this feature is available.
 */

bool PLIB_SPI_ReceiverHasOverflowed(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_SPI_ReceiverOverflowClear(SPI_MODULE_ID index)

  Summary:
    Clears the SPI receive overflow flag.

  Description:
    This function clears the SPI receive overflow flag.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
   None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_ReceiverOverflowClear(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the receiver overflow status feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsReceiverOverflow"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_ReceiverOverflowClear(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_SPI_TransmitBufferIsFull (SPI_MODULE_ID index)

  Summary:
    Returns the current transmit buffer status of the SPI module.

  Description:
    This function returns the current transmit buffer status of the SPI module.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true  - Transmit not yet started, transmit buffer is full
    - false - Transmit started, transmit buffer is empty/not full

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    bool buffullstate = PLIB_SPI_TransmitBufferIsFull (MY_SPI_INSTANCE);
    </code>

  Remarks:
    In Standard Buffer mode - automatically set in hardware when SPI buffer writes
    occur, loading the transmit buffer. Automatically cleared in hardware when
     the SPI module transfers data from the transmit buffer to the shift register.

    In Enhanced Buffer mode - automatically set in hardware when SPI buffer writes
    occur, loading the last available buffer. Automatically cleared in hardware
    when the buffer is available for writing.

    This function implements an operation of the transmit buffer status feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsTransmitBufferFullStatus"
    in your application to automatically determine whether this feature is available.
 */

bool PLIB_SPI_TransmitBufferIsFull(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_SPI_ReceiverBufferIsFull (SPI_MODULE_ID index)

  Summary:
    Returns the current status of the SPI receive buffer.

  Description:
    This function returns the current status of the SPI receive buffer.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    Receiver Buffer Full Status:
    - true  - Receive complete, receive buffer is full
    - false - Receive is not complete, receive buffer is empty

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    bool receivefullstate = PLIB_SPI_ReceiverBufferIsFull (MY_SPI_INSTANCE);
    </code>

  Remarks:
    In Standard Buffer mode - automatically set in hardware when the SPI module
    transfers data from the shift register to the receive buffer. Automatically
    cleared in hardware when the core reads the SPI buffer, read in the receive buffer.

    In Enhanced Buffer mode - automatically set in hardware when the SPI module
    transfers data from the shift register to the receive buffer, filling the
    last unread buffer. Automatically cleared in hardware when a buffer is available
    for a transfer from the shift register.

    This function implements an operation of the receiver buffer status feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsReceiveBufferStatus"
    in your application to automatically determine whether this feature is available.


 */

bool PLIB_SPI_ReceiverBufferIsFull(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_SPI_PinEnable (SPI_MODULE_ID index,SPI_PIN pin)

  Summary:
    Enables the selected SPI pins.

  Description:
    This function enables the selected SPI pins.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    pin             - One of the SPI_PIN enumeration values as the SPI pin

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_PinEnable(MY_SPI_INSTANCE, SPI_PIN_SLAVE_SELECT);
    </code>

  Remarks:
    This function implements an operation of the pin control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsPinControl"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_PinEnable(SPI_MODULE_ID index, SPI_PIN pin);


//******************************************************************************
/* Function:
    void PLIB_SPI_PinDisable (SPI_MODULE_ID index, SPI_PIN pin)

  Summary:
    Enables the selected SPI pins.

  Description:
    This function enables the selected SPI pins.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    pin             - One of the SPI_PIN enumeration values as the SPI pin

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_PinDisable(MY_SPI_INSTANCE, SPI_PIN_SLAVE_SELECT);
    </code>

  Remarks:
    This function implements an operation of the pin control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsPinControl"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_PinDisable(SPI_MODULE_ID index, SPI_PIN pin);


//******************************************************************************
/* Function:
    void PLIB_SPI_CommunicationWidthSelect (SPI_MODULE_ID index,
                                            SPI_COMMUNICATION_WIDTH width )

  Summary:
    Selects the data width for the SPI communication.

  Description:
    This function selects the data width for the SPI communication.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    width           - One of the SPI_COMMUNICATION_WIDTH enumeration values as the
                      SPI buffer width

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_CommunicationWidthSelect(MY_SPI_INSTANCE, SPI_COMMUNICATION_WIDTH_8BITS);
    </code>

  Remarks:
    This function implements an operation of the communication width feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsCommunicationWidth"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_CommunicationWidthSelect(SPI_MODULE_ID index,
                                       SPI_COMMUNICATION_WIDTH width);


//******************************************************************************
/* Function:
    void PLIB_SPI_AudioCommunicationWidthSelect (SPI_MODULE_ID index,
                                                 SPI_AUDIO_COMMUNICATION_WIDTH width )

  Summary:
    Selects the data width for the SPI audio communication.

  Description:
    This function selects the data width for the SPI audio communication.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    width           - One of the SPI_AUDIO_COMMUNICATION_WIDTH enumeration values as the
                      SPI buffer width

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_AudioCommunicationWidthSelect(MY_SPI_INSTANCE,
	                       SPI_AUDIO_COMMUNICATION_32DATA_32FIFO_32CHANNEL);
    </code>

  Remarks:
   This mode is available only when PLIB_SPI_AudioProtocolEnable is enabled.

    This function implements an operation of the audio communication width feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsAudioCommunicationWidth"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_AudioCommunicationWidthSelect(SPI_MODULE_ID index,
                                            SPI_AUDIO_COMMUNICATION_WIDTH mode);


//******************************************************************************
/* Function:
    void PLIB_SPI_InputSamplePhaseSelect(SPI_MODULE_ID index,
                                         SPI_INPUT_SAMPLING_PHASE phase)

  Summary:
    Selects the SPI data input sample phase.

  Description:
    This function selects the input sampling phase in Master mode.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    phase           - One of the SPI_INPUT_SAMPLING_PHASE as the SPI input
                      sampling phase

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_InputSamplePhaseSelect(MY_SPI_INSTANCE, SPI_INPUT_SAMPLING_PHASE_IN_MIDDLE);
    </code>

  Remarks:
    This function implements an operation of the input sample phase feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsInputSamplePhase"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_InputSamplePhaseSelect(SPI_MODULE_ID index,
                                     SPI_INPUT_SAMPLING_PHASE phase);


//******************************************************************************
/* Function:
    void PLIB_SPI_OutputDataPhaseSelect (SPI_MODULE_ID index,
                                         SPI_OUTPUT_DATA_PHASE data)

  Summary:
    Selects serial output data change.

  Description:
    This function selects serial output data change.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    data            - One of the SPI_OUTPUT_DATA_PHASE enumeration values as the
                      SPI serial output data change

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_OutputDataPhaseSelect(MY_SPI_INSTANCE, SPI_OUTPUT_DATA_PHASE_ON_ACTIVE_TO_IDLE_CLOCK);
    </code>

  Remarks:
    This function implements an operation of the output data phase feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsOutputDataPhase"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_OutputDataPhaseSelect(SPI_MODULE_ID index,
                                    SPI_OUTPUT_DATA_PHASE phase);


//******************************************************************************
/* Function:
    void PLIB_SPI_ClockPolaritySelect(SPI_MODULE_ID index,
                                      SPI_CLOCK_POLARITY polarity)

  Summary:
    Enables clock polarity.

  Description:
    This function enables clock polarity.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    polarity        - One of the SPI_CLOCK_POLARITY enumeration values as the SPI clock polarity

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_ClockPolaritySelect(MY_SPI_INSTANCE, SPI_CLOCK_POLARITY_IDLE_HIGH );
    </code>

  Remarks:
    This function implements an operation of the clock polarity feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsClockPolarity"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_ClockPolaritySelect(SPI_MODULE_ID index,
                                  SPI_CLOCK_POLARITY polarity);


//******************************************************************************
/* Function:
    void PLIB_SPI_MasterEnable(SPI_MODULE_ID index)

  Summary:
    Enables the SPI in Master mode.

  Description:
    This function enables the SPI in Master mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_MasterEnable(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the master enable control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsMasterControl"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_MasterEnable(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_SPI_SlaveEnable(SPI_MODULE_ID index)

  Summary:
    Enables the SPI in Slave mode.

  Description:
    This function enables the SPI in Slave mode.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_SlaveEnable(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the master enable control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsMasterControl"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_SlaveEnable(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
   void PLIB_SPI_BaudRateSet(SPI_MODULE_ID index, uint32_t clockFrequency,
                             uint32_t baudRate )

  Summary:
    Sets the baud rate to the desired value.

  Description:
    This function sets the baud rate to the desired value.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    clockFrequency  - Clock frequency
    baudrate        - Baud rate value

  Returns:
    None.

  Example:
    <code>
     // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_BaudRateSet(MY_SPI_INSTANCE, MY_CLOCK_FREQUENCY, 9600);
    </code>

  Remarks:
    Setting a new baud rate value causes the baud rate timer to reset.
    This ensures that the baud rate timer does not have to overflow before
    outputting the new baud rate.

    If the system clock is changed during an active receive operation, a
    receive error or data loss may result. To avoid this issue, verify that
    no receptions are in progress before changing the system clock.

    This function implements an operation of the baud rate set feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsBaudRate"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_BaudRateSet(SPI_MODULE_ID index, uint32_t clockFrequency,
                          uint32_t baudRate);


//******************************************************************************
/* Function:
    bool PLIB_SPI_IsBusy (SPI_MODULE_ID index)

  Summary:
    Returns the current SPI module activity status.

  Description:
    This function returns the current SPI module activity status.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true  - SPI module is currently busy with some transactions
    - false - SPI module is currently idle

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    bool status = PLIB_SPI_IsBusy(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the bus status feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsBusStatus"
    in your application to automatically determine whether this feature is available.
 */

bool PLIB_SPI_IsBusy(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_SPI_ReadDataIsSignExtended(SPI_MODULE_ID index)

  Summary:
    Returns the current status of the receive (RX) FIFO sign-extended data.

  Description:
    This function returns the current status of the receive (RX) FIFO
    sign-extended data.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true  - Data from RX FIFO is sign-extended
    - false - Data from RX FIFO is not sign-extended

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    bool Status = PLIB_SPI_ReadDataIsSignExtended(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the data sign feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsReadDataSignStatus"
    in your application to automatically determine whether this feature is available.
 */

bool PLIB_SPI_ReadDataIsSignExtended(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_SPI_SlaveSelectEnable(SPI_MODULE_ID index)

  Summary:
    Enables Master mode slave select.

  Description:
    This function enables Master mode slave select.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_SlaveSelectEnable(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This feature does not support Framed SPI mode.

    This function implements an operation of the Master mode slave select feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsSlaveSelectControl"
    in your application to automatically determine whether this feature is available.
    
    To enable Slave mode slave select pin (SSEN), PLIB_SPI_PinEnable API can be used.
 */

void PLIB_SPI_SlaveSelectEnable(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_SPI_SlaveSelectDisable(SPI_MODULE_ID index)

  Summary:
    Disables Master mode slave select.

  Description:
    This function disables Master mode slave select.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_SlaveSelectDisable(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This feature does not support Framed SPI mode.

    This function implements an operation of the slave select feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsSlaveSelectControl"
    in your application to automatically determine whether this feature is available.
    
    To disable Slave mode slave select pin (SSEN), PLIB_SPI_PinDisable API can be used.
 */

void PLIB_SPI_SlaveSelectDisable(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_SPI_TransmitUnderRunStatusGet(SPI_MODULE_ID index)

  Summary:
    Returns the current status of the transmit underrun.

  Description:
    This function returns the current status of the transmit underrun.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true  - Transmit buffer has encountered an underrun condition
    - false - Transmit buffer run has not encountered an underrun condition

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    bool Status = PLIB_SPI_TransmitUnderRunStatusGet(MY_SPI_INSTANCE);
    </code>

  Remarks:
    Valid in Framed Sync mode.

    This function implements an operation of the transmit underrun status feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsTransmitUnderRunStatus"
    in your application to automatically determine whether this feature is available.
 */

bool PLIB_SPI_TransmitUnderRunStatusGet(SPI_MODULE_ID index);

//******************************************************************************
/* Function:
    void PLIB_SPI_TransmitUnderRunStatusClear(SPI_MODULE_ID index)

  Summary:
    Clears the SPI transmit underrun flag.

  Description:
    This function clears the SPI transmit underrun flag.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
   None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_TransmitUnderRunStatusClear(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the transmit underrun status feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "ExistsTransmitUnderRunStatus"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_TransmitUnderRunStatusClear(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_SPI_TransmitBufferIsEmpty (SPI_MODULE_ID index)

  Summary:
    Returns the current status of the transmit buffer.

  Description:
    This function returns the current status of the transmit buffer.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true  - Transmit buffer is empty
    - false - Transmit buffer is not empty

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    bool Status = PLIB_SPI_TransmitBufferIsEmpty(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the transmit buffer empty status feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsTransmitBufferEmptyStatus"
    in your application to automatically determine whether this feature is available.
 */

bool PLIB_SPI_TransmitBufferIsEmpty(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_SPI_FIFOEnable (SPI_MODULE_ID index)

  Summary:
    Enables the SPI enhanced buffer.

  Description:
    This function enables the SPI enhanced buffer.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_FIFOEnable(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This enables the enhanced buffer mode.

    This function implements an operation of the FIFO control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsFIFOControl"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_FIFOEnable(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_SPI_FIFODisable (SPI_MODULE_ID index)

  Summary:
    Disables the SPI enhanced buffer.

  Description:
    This function disables the SPI enhanced buffer.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_FIFODisable(MY_SPI_INSTANCE);
    </code>

  Remarks:
    Enables the legacy standard single buffer mode.

    This function implements an operation of the FIFO control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsFIFOControl"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_FIFODisable(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    uint8_t PLIB_SPI_FIFOCountGet (SPI_MODULE_ID index, SPI_FIFO_TYPE type)

  Summary:
    Reads the SPI Buffer Element Count bits for either receive or transmit.

  Description:
    This function reads the number of SPI transfers pending for Master mode
    and the number of unread SPI transfers for Slave mode.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    type            - One of the SPI_FIFO_TYPE enumeration values

  Returns:
    CountValue     - Buffer element count bits

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    uint8_t count = PLIB_SPI_FIFOCountGet(MY_SPI_INSTANCE,SPI_FIFO_TYPE_TRANSMIT);
    </code>

  Remarks:
    Valid in Enhanced Buffer mode.

    This function implements an operation of the FIFO control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsFIFOCount"
    in your application to automatically determine whether this feature is available.
 */

uint8_t PLIB_SPI_FIFOCountGet(SPI_MODULE_ID index, SPI_FIFO_TYPE type);


//******************************************************************************
/* Function:
    bool PLIB_SPI_FIFOShiftRegisterIsEmpty (SPI_MODULE_ID index)

  Summary:
    Returns the current status of the SPI shift register.

  Description:
    This function returns the current status of the SPI shift register.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true  - SPI shift register is empty and ready to send or receive
    - false - SPI shift register is not empty

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    bool Status = PLIB_SPI_FIFOShiftRegisterIsEmpty(MY_SPI_INSTANCE);
    </code>

  Remarks:
    Valid in Enhanced Buffer mode.

    This function implements an operation of the FIFO status feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsFIFOShiftRegisterEmptyStatus"
    in your application to automatically determine whether this feature is available.
 */

bool PLIB_SPI_FIFOShiftRegisterIsEmpty(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    bool PLIB_SPI_ReceiverFIFOIsEmpty (SPI_MODULE_ID index)

  Summary:
    Returns the current status of the SPI receive FIFO.

  Description:
    This function returns the current status of the SPI receive FIFO.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true  - Receive FIFO is empty
    - false - Receive FIFO is not empty

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    bool fifostate = PLIB_SPI_ReceiverFIFOIsEmpty (MY_SPI_INSTANCE);
    </code>

  Remarks:
    Valid in Enhanced Buffer mode.

    This function implements an operation of the FIFO status feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsReceiveFIFOStatus"
    in your application to automatically determine whether this feature is available.
 */

bool PLIB_SPI_ReceiverFIFOIsEmpty (SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_SPI_FIFOInterruptModeSelect (SPI_MODULE_ID index,
                                           SPI_FIFO_INTERRUPT mode)

  Summary:
    Selects the SPI buffer interrupt mode.

  Description:
    This function selects the SPI buffer interrupt mode from SPI_FIFO_INTERRUPT.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    mode            - One of the SPI_FIFO_INTERRUPT enumeration values as the SPI buffer
                      interrupt mode

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_FIFOInterruptModeSelect(MY_SPI_INSTANCE,
                    SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_NOT_FULL);
    </code>

  Remarks:
    Valid in Enhanced Buffer mode.

    This function implements an operation of the FIFO interrupt feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsFIFOInterruptMode"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_FIFOInterruptModeSelect(SPI_MODULE_ID index,
                                      SPI_FIFO_INTERRUPT mode);


//******************************************************************************
/* Function:
    void PLIB_SPI_FramedCommunicationEnable (SPI_MODULE_ID index)

  Summary:
    Enables framed SPI support.

  Description:
    This function enables framed SPI support.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_FramedCommunicationEnable(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the framed communication feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsFramedCommunication"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_FramedCommunicationEnable(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_SPI_FramedCommunicationDisable (SPI_MODULE_ID index)

  Summary:
    Disables framed SPI support.

  Description:
    This function disables framed SPI support.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_FramedCommunicationDisable(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the framed communication feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsFramedCommunication"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_FramedCommunicationDisable(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_SPI_FrameSyncPulseDirectionSelect (SPI_MODULE_ID index,
                                                 SPI_FRAME_PULSE_DIRECTION direction)

  Summary:
    Selects the frame sync pulse direction.

  Description:
    This function selects the frame sync pulse direction.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    direction       - One of the SPI_FRAME_PULSE_DIRECTION enumeration values
                      as the SPI frame sync pulse polarity

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_FrameSyncPulseDirectionSelect(MY_SPI_INSTANCE, SPI_FRAME_PULSE_DIRECTION_INPUT );
    </code>

  Remarks:
    This function implements an operation of the framed communication feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsFrameSyncPulseDirection"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_FrameSyncPulseDirectionSelect (SPI_MODULE_ID index,
                                             SPI_FRAME_PULSE_DIRECTION direction);


//******************************************************************************
/* Function:
    void PLIB_SPI_FrameSyncPulsePolaritySelect (SPI_MODULE_ID index,
                                                SPI_FRAME_PULSE_POLARITY polarity)

  Summary:
    Selects the frame sync pulse polarity.

  Description:
    This function selects the frame sync pulse polarity.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    polarity        - One of the SPI_FRAME_PULSE_POLARITY enumeration values as the SPI frame
        	      sync pulse polarity

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_FrameSyncPulsePolaritySelect(MY_SPI_INSTANCE, SPI_FRAME_PULSE_POLARITY_ACTIVE_HIGH );
    </code>

  Remarks:
    Available only for Frame mode.

    This function implements an operation of the framed communication feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsFrameSyncPulsePolarity"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_FrameSyncPulsePolaritySelect (SPI_MODULE_ID index,
                                            SPI_FRAME_PULSE_POLARITY polarity);


//******************************************************************************
/* Function:
    void PLIB_SPI_FrameSyncPulseEdgeSelect (SPI_MODULE_ID index,
                                            SPI_FRAME_PULSE_EDGE edge)

  Summary:
    Selects the frame sync pulse edge.

  Description:
    This function selects the frame sync pulse edge.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    edge            - One of the SPI_FRAME_PULSE_EDGE enumeration values as the
                      SPI frame sync pulse edge

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_FrameSyncPulseEdgeSelect(MY_SPI_INSTANCE,
	               SPI_FRAME_PULSE_EDGE_COINCIDES_FIRST_BIT_CLOCK);
    </code>

  Remarks:
    This function implements an operation of the framed communication feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsFrameSyncPulseEdge"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_FrameSyncPulseEdgeSelect(SPI_MODULE_ID index,
                                       SPI_FRAME_PULSE_EDGE edge);


//******************************************************************************
/* Function:
    void PLIB_SPI_FrameSyncPulseWidthSelect (SPI_MODULE_ID index,
                                             SPI_FRAME_PULSE_WIDTH width)

  Summary:
    Sets the frame sync pulse width.

  Description:
    This function sets the frame sync pulse width.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    edge            - One of the SPI_FRAME_PULSE_WIDTH enumeration values as the SPI
                      frame sync pulse width.

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_FrameSyncPulseWidthSelect (MY_SPI_INSTANCE,SPI_FRAME_PULSE_WIDTH_ONE_WORD_LENGTH);
    </code>

  Remarks:
    Length of the word is dependent on the communication mode.

    This function implements an operation of the framed communication feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsFrameSyncPulseWidth"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_FrameSyncPulseWidthSelect(SPI_MODULE_ID index,
                                        SPI_FRAME_PULSE_WIDTH width);


//******************************************************************************
/* Function:
    void PLIB_SPI_FrameSyncPulseCounterSelect (SPI_MODULE_ID index,
                                               SPI_FRAME_SYNC_PULSE pulse)

  Summary:
    Selects at which character the SPI frame sync pulse is generated.

  Description:
    This function selects at which character the SPI frame sync pulse is generated.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    pulse           - One of the SPI_FRAME_SYNC_PULSE enumeration values as the SPI
                      frame sync pulse count

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_FrameSyncPulseCounterSelect(MY_SPI_INSTANCE,
                                            SPI_FRAME_SYNC_PULSE_ON_EVERY_32_DATA_CHARACTER );
    </code>

  Remarks:
    This is valid only when PLIB_SPI_FramedCommunicationEnable is enabled.

    This function implements an operation of the framed communication feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsFrameSyncPulseCounter"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_FrameSyncPulseCounterSelect (SPI_MODULE_ID index,
                                           SPI_FRAME_SYNC_PULSE pulse);


//******************************************************************************
/* Function:
    bool PLIB_SPI_FrameErrorStatusGet (SPI_MODULE_ID index)

  Summary:
    Returns the current status of the SPI frame error.

  Description:
    This function returns the current status of the SPI frame error.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    - true  - Frame error detected
    - false - No frame error detected

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    bool status = PLIB_SPI_FrameErrorStatusGet(MY_SPI_INSTANCE);
    </code>

  Remarks:
	Valid only if Frame mode is enabled.

    This function implements an operation of the framed communication feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsFrameErrorStatus"
    in your application to automatically determine whether this feature is available.
 */

bool PLIB_SPI_FrameErrorStatusGet(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_SPI_FrameErrorStatusClear(SPI_MODULE_ID index)

  Summary:
    Clears the SPI frame error flag.

  Description:
    This function clears the SPI frame error flag.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
   None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_FrameErrorStatusClear(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the frame error status feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "ExistsFrameErrorStatus"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_FrameErrorStatusClear(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_SPI_BufferClear (SPI_MODULE_ID index)

  Summary:
    Clears the SPI receive buffer.

  Description:
    This function clears the SPI receive buffer.
    In Enhanced Buffer mode, this functions clears the full RX FIFO.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
     // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    //This call will flush the receive buffer.
    PLIB_SPI_BufferClear(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the buffer control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsBuffer"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_BufferClear(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    uint8_t PLIB_SPI_BufferRead (SPI_MODULE_ID index)

  Summary:
    Returns the SPI buffer value.

  Description:
    This function returns the SPI buffer value.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
     Reads the SPI buffer.

  Example:
    <code>
     // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    uint8_t bufferValue = PLIB_SPI_BufferRead(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the buffer control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsBuffer"
    in your application to automatically determine whether this feature is available.
 */

uint8_t PLIB_SPI_BufferRead(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    uint16_t PLIB_SPI_BufferRead16bit ( SPI_MODULE_ID index )

  Summary:
    Returns 16-bit SPI buffer value.

  Description:
    This function returns 16-bit SPI buffer value.
    This operation is atomic.

  Precondition:
    SPI 16-bit wide communication must be selected using PLIB_SPI_CommunicationWidthSelect.

  Parameters:
    index   - Identifier for the device instance to be configured

  Returns:
    Returns the SPI 16-bit buffer value.

  Example:
    <code>
	#define MY_SPI_INSTANCE SPI_ID_1
	
    uint16_t bufferValue = PLIB_SPI_BufferRead16bit( MY_SPI_INSTANCE );
    </code>

  Remarks:
    This function implements an operation of the buffer control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_Exists16bitBuffer"
    in your application to automatically determine whether this feature is available.
 */

uint16_t PLIB_SPI_BufferRead16bit ( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    uint32_t PLIB_SPI_BufferRead32bit ( SPI_MODULE_ID index )

  Summary:
    Returns 32-bit SPI buffer value.

  Description:
    This function returns 32-bit SPI buffer value.
    This operation is atomic.

  Precondition:
    SPI 32-bit wide communication must be selected using PLIB_SPI_CommunicationWidthSelect.

  Parameters:
    index   - Identifier for the device instance to be configured

  Returns:
    Returns the SPI 32-bit buffer value.

  Example:
    <code>
	#define MY_SPI_INSTANCE SPI_ID_1
	
    uint32_t bufferValue = PLIB_SPI_BufferRead32bit( MY_SPI_INSTANCE );
    </code>

  Remarks:
    This function implements an operation of the buffer control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_Exists32bitBuffer"
    in your application to automatically determine whether this feature is available.
 */

uint32_t PLIB_SPI_BufferRead32bit ( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_SPI_BufferWrite ( SPI_MODULE_ID index , uint8_t data )

  Summary:
    Write the data to the SPI buffer.

  Description:
    This function writes data to the SPI buffer.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    data            - Data to written to the SPI buffer

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_BufferWrite ( MY_SPI_INSTANCE, 0xFF );
    </code>

  Remarks:
    This function implements an operation of the buffer control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsBuffer"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_BufferWrite ( SPI_MODULE_ID index, uint8_t data );


//******************************************************************************
/* Function:
    void PLIB_SPI_BufferWrite16bit ( SPI_MODULE_ID index , uint16_t data )

  Summary:
    Writes 16-bit data to the SPI buffer.

  Description:
    This function writes 16-bit data to the SPI buffer.
    This operation is atomic.

  Precondition:
    SPI 16-bit wide communication must be selected using PLIB_SPI_CommunicationWidthSelect.

  Parameters:
    index           - Identifier for the device instance to be configured
    data            - 16-bit data to be written to the SPI buffer

  Returns:
    None.

  Example:
    <code>
	#define MY_SPI_INSTANCE	SPI_ID_1
	
    PLIB_SPI_BufferWrite16bit ( MY_SPI_INSTANCE, 0x55AA );
    </code>

  Remarks:
    This function implements an operation of the buffer control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_Exists16bitBuffer"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_BufferWrite16bit ( SPI_MODULE_ID index, uint16_t data );


//******************************************************************************
/* Function:
    void PLIB_SPI_BufferWrite32bit ( SPI_MODULE_ID index , uint32_t data )

  Summary:
    Write 32-bit data to the SPI buffer.

  Description:
    This function writes 32-bit data to the SPI buffer.
    This operation is atomic.

  Precondition:
    SPI 32-bit wide communication must be selected using PLIB_SPI_CommunicationWidthSelect.

  Parameters:
    index           - Identifier for the device instance to be configured
    data            - 32-bit data to be written to the SPI buffer

  Returns:
    None.

  Example:
    <code>
	#define MY_SPI_INSTANCE	SPI_ID_1
	
    PLIB_SPI_BufferWrite ( MY_SPI_INSTANCE, 0x55AA55AA );
    </code>

  Remarks:
    This function implements an operation of the buffer control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_Exists32bitBuffer"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_BufferWrite32bit ( SPI_MODULE_ID index, uint32_t data );


//******************************************************************************
/* Function :  PLIB_SPI_BufferAddressGet( SPI_MODULE_ID index )

  Summary:
    Returns the address of the SPIxBUF (Transmit(SPIxTXB) and Receive (SPIxRXB)) 
	register.

  Description:
    This function returns the address of the SPIxBUF (Transmit(SPIxTXB) and Receive
    (SPIxRXB)) register.
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    The address of the SPIxBUF register

  Remarks:
    None.
*/
void* PLIB_SPI_BufferAddressGet ( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_SPI_BaudRateClockSelect (SPI_MODULE_ID index,
                                       SPI_BAUD_RATE_CLOCK type)

  Summary:
    Selects the type of clock is used by the Baud Rate Generator.

  Description:
    This function selects the type of clock is used by the Baud Rate Generator.

  Precondition:
    None.

  Parameters:
    index     - Identifier for the device instance to be configured
    type      - One of the SPI_BAUD_RATE_CLOCK enumeration values as the SPI baud clock

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_BaudRateClockSelect (MY_SPI_INSTANCE, SPI_BAUD_RATE_MCLK_CLOCK);
    </code>

 Remarks:
    This function implements an operation of the baud rate clock control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsBaudRateClock"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_BaudRateClockSelect (SPI_MODULE_ID index,
                                   SPI_BAUD_RATE_CLOCK type);


//******************************************************************************
/* Function:
    void PLIB_SPI_ErrorInterruptEnable (SPI_MODULE_ID index,
                                        SPI_ERROR_INTERRUPT error)

  Summary:
    Enables SPI error interrupts

  Description:
    This function enables SPI error interrupts.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    type            - One of the SPI_ERROR_INTERRUPT enumeration values as the
                      SPI interrupt error

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_ErrorInterruptEnable (MY_SPI_INSTANCE, SPI_ERROR_INTERRUPT_FRAME_ERROR_OVERFLOW);
    </code>

  Remarks:
    This function implements an operation of the error interrupt control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsErrorInterruptControl"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_ErrorInterruptEnable (SPI_MODULE_ID index,
                                    SPI_ERROR_INTERRUPT error);


//******************************************************************************
/* Function:
    void PLIB_SPI_ErrorInterruptDisable (SPI_MODULE_ID index,
                                         SPI_ERROR_INTERRUPT error)

  Summary:
    Enables SPI error interrupts.

  Description:
    This function enables SPI error interrupts.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    error           - One of the SPI_ERROR_INTERRUPT enumeration values as the
                      SPI interrupt error

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_ErrorInterruptDisable (MY_SPI_INSTANCE, SPI_ERROR_INTERRUPT_FRAME_ERROR_OVERFLOW);
    </code>

  Remarks:
    This function implements an operation of the error interrupt control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsErrorInterruptControl"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_ErrorInterruptDisable (SPI_MODULE_ID index,
                                     SPI_ERROR_INTERRUPT error);


//******************************************************************************
/* Function:
    void PLIB_SPI_AudioErrorEnable (SPI_MODULE_ID index, SPI_Audio_ERROR error)

  Summary:
    Enables the SPI error.

  Description:
    This function enables the SPI error.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    error           - One of the SPI_AUDIO_ERROR enumeration values as the SPI error

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_AudioErrorEnable (MY_SPI_INSTANCE, SPI_AUDIO_ERROR_RECEIVE_OVERFLOW);
    </code>

  Remarks:
    This function implements an operation of the audio error control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsAudioErrorControl"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_AudioErrorEnable (SPI_MODULE_ID index, SPI_AUDIO_ERROR error);


//******************************************************************************
/* Function:
    void PLIB_SPI_AudioErrorDisable (SPI_MODULE_ID index, SPI_AUDIO_ERROR error)

  Summary:
    Disables the SPI error.

  Description:
    This function disables the SPI error.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    error           - One of the SPI_AUDIO_ERROR enumeration values as the SPI error

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_AudioErrorDisable (MY_SPI_INSTANCE, SPI_AUDIO_ERROR_RECEIVE_OVERFLOW);
    </code>

  Remarks:
    This function implements an operation of the audio error control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsAudioErrorControl"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_AudioErrorDisable (SPI_MODULE_ID index, SPI_AUDIO_ERROR error);


//******************************************************************************
/* Function:
    void PLIB_SPI_AudioProtocolEnable (SPI_MODULE_ID index)

  Summary:
    Audio protocol is enabled.

  Description:
    This function enables the audio protocol.
    This operation is atomic.

  Precondition:
    Disable the SPI module by calling PLIB_SPI_Disable.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_AudioProtocolEnable(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the audio protocol control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsAudioProtocolControl"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_AudioProtocolEnable(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_SPI_AudioProtocolDisable (SPI_MODULE_ID index)

  Summary:
    Audio protocol is disabled.

  Description:
    This function disables the audio protocol.
    This operation is atomic.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_AudioProtocolDisable(MY_SPI_INSTANCE);
    </code>

  Remarks:
    This function implements an operation of the audio protocol control feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsAudioProtocolControl"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_AudioProtocolDisable(SPI_MODULE_ID index);


//******************************************************************************
/* Function:
    void PLIB_SPI_AudioTransmitModeSelect (SPI_MODULE_ID index,
                                           SPI_AUDIO_TRANSMIT_MODE mode)

 Summary:
    Selects the transmit audio data format.

  Description:
    This function selects the transmit audio data format.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    mode            - One of the SPI_TRANSMIT_AUDIO_MODE enumeration values as the
                      transmit audio format

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_AudioTransmitModeSelect (MY_SPI_INSTANCE, SPI_AUDIO_TRANSMIT_MONO);
    </code>

  Remarks:
    This function implements an operation of the audio transmit mode feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsAudioTransmitMode"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_AudioTransmitModeSelect (SPI_MODULE_ID index,
                                       SPI_AUDIO_TRANSMIT_MODE mode);


//******************************************************************************
/* Function:
    void PLIB_SPI_AudioProtocolModeSelect(SPI_MODULE_ID index,
                                          SPI_AUDIO_PROTOCOL mode )

  Summary:
    Selects the Audio Protocol mode.

  Description:
    This function selects the Audio Protocol mode.

  Precondition:
    None.

  Parameters:
    index           - Identifier for the device instance to be configured
    mode            - One of the SPI_AUDIO_PROTOCOL enumeration values as the
                      audio protocol

  Returns:
    None.

  Example:
    <code>
    // Where MY_SPI_INSTANCE, is the SPI instance selected for use by the
    // application developer.
    PLIB_SPI_AudioProtocolModeSelect(MY_SPI_INSTANCE,SPI_AUDIO_PROTOCOL_RIGHT_JUSTIFIED );
    </code>

  Remarks:
    This function implements an operation of the audio protocol mode feature.
    This feature may not be available on all devices.  Please refer to the
    specific device data sheet to determine availability or use "PLIB_SPI_ExistsAudioProtocolMode"
    in your application to automatically determine whether this feature is available.
 */

void PLIB_SPI_AudioProtocolModeSelect(SPI_MODULE_ID index,
                                      SPI_AUDIO_PROTOCOL mode);


// *****************************************************************************
// *****************************************************************************
// Section: SPI Peripheral Library Exists Functions
// *****************************************************************************
// *****************************************************************************
/* The following functions indicate the existence of the features on the device. 
*/

//******************************************************************************
/* Function:
    PLIB_SPI_ExistsEnableControl( SPI_MODULE_ID index )

  Summary:
    Identifies whether the EnableControl feature exists on the SPI module. 

  Description:
    This function identifies whether the EnableControl feature is available on 
	the SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_Enable
    - PLIB_SPI_Disable
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

bool PLIB_SPI_ExistsEnableControl( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsStopInIdleControl( SPI_MODULE_ID index )

  Summary:
    Identifies whether the StopInIdle feature exists on the SPI module. 

  Description:
    This function identifies whether the StopInIdle feature is available on the 
	SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_StopInIdleEnable
    - PLIB_SPI_StopInIdleDisable
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

bool PLIB_SPI_ExistsStopInIdleControl( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsReceiverOverflow( SPI_MODULE_ID index )

  Summary:
    Identifies whether the ReceiverOverflow feature exists on the SPI module. 

  Description:
    This function identifies whether the ReceiverOverflow feature is available 
	on the SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_ReceiverHasOverflowed
    - PLIB_SPI_ReceiverOverflowClear
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiverOverflow feature is supported on the device
    - false  - The ReceiverOverflow feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsReceiverOverflow( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsTransmitBufferFullStatus( SPI_MODULE_ID index )

  Summary:
    Identifies whether the TransmitBufferFullStatus feature exists on the SPI module. 

  Description:
    This function identifies whether the TransmitBufferFullStatus feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_TransmitBufferIsFull
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The TransmitBufferFullStatus feature is supported on the device
    - false  - The TransmitBufferFullStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsTransmitBufferFullStatus( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsTransmitBufferEmptyStatus( SPI_MODULE_ID index )

  Summary:
    Identifies whether the TransmitBufferEmptyStatus feature exists on the SPI module. 

  Description:
    This function identifies whether the TransmitBufferEmptyStatus feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_TransmitBufferIsEmpty
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The TransmitBufferEmptyStatus feature is supported on the device
    - false  - The TransmitBufferEmptyStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsTransmitBufferEmptyStatus( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsReceiveBufferStatus( SPI_MODULE_ID index )

  Summary:
    Identifies whether the ReceiveBufferStatus feature exists on the SPI module. 

  Description:
    This function identifies whether the ReceiveBufferStatus feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_ReceiverBufferIsFull
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiveBufferStatus feature is supported on the device
    - false  - The ReceiveBufferStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsReceiveBufferStatus( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsPinControl( SPI_MODULE_ID index )

  Summary:
    Identifies whether the PinControl feature exists on the SPI module. 

  Description:
    This function identifies whether the PinControl feature is available on the 
	SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_PinEnable
    - PLIB_SPI_PinDisable
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The PinControl feature is supported on the device
    - false  - The PinControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsPinControl( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsCommunicationWidth( SPI_MODULE_ID index )

  Summary:
    Identifies whether the CommunicationWidth feature exists on the SPI module. 

  Description:
    This function identifies whether the CommunicationWidth feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_CommunicationWidthSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The CommunicationWidth feature is supported on the device
    - false  - The CommunicationWidth feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsCommunicationWidth( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsAudioCommunicationWidth( SPI_MODULE_ID index )

  Summary:
    Identifies whether the AudioCommunicationWidth feature exists on the SPI module. 

  Description:
    This function identifies whether the AudioCommunicationWidth feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_AudioCommunicationWidthSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The AudioCommunicationWidth feature is supported on the device
    - false  - The AudioCommunicationWidth feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsAudioCommunicationWidth( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsInputSamplePhase( SPI_MODULE_ID index )

  Summary:
    Identifies whether the InputSamplePhase feature exists on the SPI module. 

  Description:
    This function identifies whether the InputSamplePhase feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_InputSamplePhaseSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The InputSamplePhase feature is supported on the device
    - false  - The InputSamplePhase feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsInputSamplePhase( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsOutputDataPhase( SPI_MODULE_ID index )

  Summary:
    Identifies whether the OutputDataPhase feature exists on the SPI module. 

  Description:
    This function identifies whether the OutputDataPhase feature is available on 
	the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_OutputDataPhaseSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The OutputDataPhase feature is supported on the device
    - false  - The OutputDataPhase feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsOutputDataPhase( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsClockPolarity( SPI_MODULE_ID index )

  Summary:
    Identifies whether the ClockPolarity feature exists on the SPI module. 

  Description:
    This function identifies whether the ClockPolarity feature is available on 
	the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_ClockPolaritySelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ClockPolarity feature is supported on the device
    - false  - The ClockPolarity feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsClockPolarity( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsMasterControl( SPI_MODULE_ID index )

  Summary:
    Identifies whether the MasterControl feature exists on the SPI module. 

  Description:
    This function identifies whether the MasterControl feature is available on 
	the SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_MasterEnable
    - PLIB_SPI_SlaveEnable
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The MasterControl feature is supported on the device
    - false  - The MasterControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsMasterControl( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsBaudRate( SPI_MODULE_ID index )

  Summary:
    Identifies whether the BaudRate feature exists on the SPI module. 

  Description:
    This function identifies whether the BaudRate feature is available on the SPI 
	module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_BaudRateSet
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

bool PLIB_SPI_ExistsBaudRate( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsBusStatus( SPI_MODULE_ID index )

  Summary:
    Identifies whether the BusStatus feature exists on the SPI module. 

  Description:
    This function identifies whether the BusStatus feature is available on the 
	SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_IsBusy
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BusStatus feature is supported on the device
    - false  - The BusStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsBusStatus( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsReadDataSignStatus( SPI_MODULE_ID index )

  Summary:
    Identifies whether the ReadDataSignStatus feature exists on the SPI module. 

  Description:
    This function identifies whether the ReadDataSignStatus feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_ReadDataIsSignExtended
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReadDataSignStatus feature is supported on the device
    - false  - The ReadDataSignStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsReadDataSignStatus( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsSlaveSelectControl( SPI_MODULE_ID index )

  Summary:
    Identifies whether the SlaveSelectControl feature exists on the SPI module. 

  Description:
    This function identifies whether the SlaveSelectControl feature is available 
	on the SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_SlaveSelectEnable
    - PLIB_SPI_SlaveSelectDisable
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The SlaveSelectControl feature is supported on the device
    - false  - The SlaveSelectControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsSlaveSelectControl( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsTransmitUnderRunStatus( SPI_MODULE_ID index )

  Summary:
    Identifies whether the TransmitUnderRunStatus feature exists on the SPI module. 

  Description:
    This function identifies whether the TransmitUnderRunStatus feature is available 
	on the SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_TransmitUnderRunStatusGet
    - PLIB_SPI_TransmitUnderRunStatusClear
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The TransmitUnderRunStatus feature is supported on the device
    - false  - The TransmitUnderRunStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsTransmitUnderRunStatus( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsFIFOControl( SPI_MODULE_ID index )

  Summary:
    Identifies whether the FIFOControl feature exists on the SPI module. 

  Description:
    This function identifies whether the FIFOControl feature is available on the 
	SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_FIFOEnable
    - PLIB_SPI_FIFODisable
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FIFOControl feature is supported on the device
    - false  - The FIFOControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsFIFOControl( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsFIFOCount( SPI_MODULE_ID index )

  Summary:
    Identifies whether the FIFOCount feature exists on the SPI module. 

  Description:
    This function identifies whether the FIFOCount feature is available on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_FIFOCountGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FIFOCount feature is supported on the device
    - false  - The FIFOCount feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsFIFOCount( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsReceiveFIFOStatus( SPI_MODULE_ID index )

  Summary:
    Identifies whether the ReceiveFIFOStatus feature exists on the SPI module. 

  Description:
    This function identifies whether the ReceiveFIFOStatus feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_ReceiverFIFOIsEmpty
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ReceiveFIFOStatus feature is supported on the device
    - false  - The ReceiveFIFOStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsReceiveFIFOStatus( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsFIFOShiftRegisterEmptyStatus( SPI_MODULE_ID index )

  Summary:
    Identifies whether the FIFOShiftRegisterEmptyStatus feature exists on the SPI module. 

  Description:
    This function identifies whether the FIFOShiftRegisterEmptyStatus feature is 
	available on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_FIFOShiftRegisterIsEmpty
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FIFOShiftRegisterEmptyStatus feature is supported on the device
    - false  - The FIFOShiftRegisterEmptyStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsFIFOShiftRegisterEmptyStatus( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsFIFOInterruptMode( SPI_MODULE_ID index )

  Summary:
    Identifies whether the FIFOInterruptMode feature exists on the SPI module. 

  Description:
    This function identifies whether the FIFOInterruptMode feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_FIFOInterruptModeSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FIFOInterruptMode feature is supported on the device
    - false  - The FIFOInterruptMode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsFIFOInterruptMode( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsFramedCommunication( SPI_MODULE_ID index )

  Summary:
    Identifies whether the FramedCommunication feature exists on the SPI module. 

  Description:
    This function identifies whether the FramedCommunication feature is available 
	on the SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_FramedCommunicationEnable
    - PLIB_SPI_FramedCommunicationDisable
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FramedCommunication feature is supported on the device
    - false  - The FramedCommunication feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsFramedCommunication( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsFrameSyncPulseDirection( SPI_MODULE_ID index )

  Summary:
    Identifies whether the FrameSyncPulseDirection feature exists on the SPI module. 

  Description:
    This function identifies whether the FrameSyncPulseDirection feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_FrameSyncPulseDirectionSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FrameSyncPulseDirection feature is supported on the device
    - false  - The FrameSyncPulseDirection feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsFrameSyncPulseDirection( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsFrameSyncPulsePolarity( SPI_MODULE_ID index )

  Summary:
    Identifies whether the FrameSyncPulsePolarity feature exists on the SPI module. 

  Description:
    This function identifies whether the FrameSyncPulsePolarity feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_FrameSyncPulsePolaritySelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FrameSyncPulsePolarity feature is supported on the device
    - false  - The FrameSyncPulsePolarity feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsFrameSyncPulsePolarity( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsFrameSyncPulseEdge( SPI_MODULE_ID index )

  Summary:
    Identifies whether the FrameSyncPulseEdge feature exists on the SPI module. 

  Description:
    This function identifies whether the FrameSyncPulseEdge feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_FrameSyncPulseEdgeSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FrameSyncPulseEdge feature is supported on the device
    - false  - The FrameSyncPulseEdge feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsFrameSyncPulseEdge( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsFrameSyncPulseWidth( SPI_MODULE_ID index )

  Summary:
    Identifies whether the FrameSyncPulseWidth feature exists on the SPI module. 

  Description:
    This function identifies whether the FrameSyncPulseWidth feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_FrameSyncPulseWidthSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FrameSyncPulseWidth feature is supported on the device
    - false  - The FrameSyncPulseWidth feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsFrameSyncPulseWidth( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsFrameSyncPulseCounter( SPI_MODULE_ID index )

  Summary:
    Identifies whether the FrameSyncPulseCounter feature exists on the SPI module. 

  Description:
    This function identifies whether the FrameSyncPulseCounter feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_FrameSyncPulseCounterSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FrameSyncPulseCounter feature is supported on the device
    - false  - The FrameSyncPulseCounter feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsFrameSyncPulseCounter( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsFrameErrorStatus( SPI_MODULE_ID index )

  Summary:
    Identifies whether the FrameErrorStatus feature exists on the SPI module. 

  Description:
    This function identifies whether the FrameErrorStatus feature is available on 
	the SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_FrameErrorStatusGet
    - PLIB_SPI_FrameErrorStatusClear
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The FrameErrorStatus feature is supported on the device
    - false  - The FrameErrorStatus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsFrameErrorStatus( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsBuffer( SPI_MODULE_ID index )

  Summary:
    Identifies whether the Buffer feature exists on the SPI module. 

  Description:
    This function identifies whether the Buffer feature is available on the SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_BufferClear
    - PLIB_SPI_BufferRead
    - PLIB_SPI_BufferWrite
    - PLIB_SPI_BufferAddressGet
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Buffer feature is supported on the device
    - false  - The Buffer feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsBuffer( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsBaudRateClock( SPI_MODULE_ID index )

  Summary:
    Identifies whether the BaudRateClock feature exists on the SPI module. 

  Description:
    This function identifies whether the BaudRateClock feature is available on 
	the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_BaudRateClockSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The BaudRateClock feature is supported on the device
    - false  - The BaudRateClock feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsBaudRateClock( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsErrorInterruptControl( SPI_MODULE_ID index )

  Summary:
    Identifies whether the ErrorInterruptControl feature exists on the SPI module. 

  Description:
    This function identifies whether the ErrorInterruptControl feature is available 
	on the SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_ErrorInterruptEnable
    - PLIB_SPI_ErrorInterruptDisable
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The ErrorInterruptControl feature is supported on the device
    - false  - The ErrorInterruptControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsErrorInterruptControl( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsAudioErrorControl( SPI_MODULE_ID index )

  Summary:
    Identifies whether the AudioErrorControl feature exists on the SPI module. 

  Description:
    This function identifies whether the AudioErrorControl feature is available 
	on the SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_AudioErrorEnable
    - PLIB_SPI_AudioErrorDisable
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The AudioErrorControl feature is supported on the device
    - false  - The AudioErrorControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsAudioErrorControl( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsAudioProtocolControl( SPI_MODULE_ID index )

  Summary:
    Identifies whether the AudioProtocolControl feature exists on the SPI module. 

  Description:
    This function identifies whether the AudioProtocolControl feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_AudioProtocolEnable
    - PLIB_SPI_AudioProtocolDisable
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The AudioProtocolControl feature is supported on the device
    - false  - The AudioProtocolControl feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsAudioProtocolControl( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsAudioTransmitMode( SPI_MODULE_ID index )

  Summary:
    Identifies whether the AudioTransmitMode feature exists on the SPI module. 

  Description:
    This function identifies whether the AudioTransmitMode feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_AudioTransmitModeSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The AudioTransmitMode feature is supported on the device
    - false  - The AudioTransmitMode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsAudioTransmitMode( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_ExistsAudioProtocolMode( SPI_MODULE_ID index )

  Summary:
    Identifies whether the AudioProtocolMode feature exists on the SPI module. 

  Description:
    This function identifies whether the AudioProtocolMode feature is available 
	on the SPI module.
    When this function returns true, this function is supported on the device: 
    - PLIB_SPI_AudioProtocolModeSelect
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The AudioProtocolMode feature is supported on the device
    - false  - The AudioProtocolMode feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_ExistsAudioProtocolMode( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_Exists32bitBuffer( SPI_MODULE_ID index )

  Summary:
    Identifies whether the Buffer32bit feature exists on the SPI module. 

  Description:
    This function identifies whether the Buffer32bit feature is available on the 
	SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_BufferRead32bit
    - PLIB_SPI_BufferWrite32bit
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Buffer32bit feature is supported on the device
    - false  - The Buffer32bit feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_Exists32bitBuffer( SPI_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_SPI_Exists16bitBuffer( SPI_MODULE_ID index )

  Summary:
    Identifies whether the Buffer16bit feature exists on the SPI module. 

  Description:
    This function identifies whether the Buffer16bit feature is available on the
	SPI module.
    When this function returns true, these functions are supported on the device: 
    - PLIB_SPI_BufferRead16bit
    - PLIB_SPI_BufferWrite16bit
    This operation is atomic.

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true   - The Buffer16bit feature is supported on the device
    - false  - The Buffer16bit feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_SPI_Exists16bitBuffer( SPI_MODULE_ID index );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef _PLIB_SPI_H
/*******************************************************************************
 End of File
 */
