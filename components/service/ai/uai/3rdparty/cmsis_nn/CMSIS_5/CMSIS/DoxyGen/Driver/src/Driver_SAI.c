/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2014 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * $Date:        9. Dec 2014
 * $Revision:    V1.00
 *
 * Project:      SAI (Serial Audio Interface) Driver definitions
 * -------------------------------------------------------------------------- */

/**
\defgroup sai_interface_gr SAI Interface
\brief   Driver API for Serial Audio Interface (%Driver_SAI.h)
\details 
The <b>Serial Audio Interface</b> (SAI) implements a synchronous serial bus interface for connecting digital audio devices. 
It is by far the most common mechanism used to transfer two channels of audio data between devices within a system. \b SAI
can transfer digital audio using various protocols:
- \ref Driver_SAI_I2S
- \ref Driver_SAI_MSB
- \ref Driver_SAI_LSB
- \ref Driver_SAI_PCM
- \ref Driver_SAI_AC97
- \ref Driver_SAI_User


<b>Block Diagram</b>

<p>&nbsp;</p>
\image html SAI_Schematics.png "Simplified SAI Schematic"
<p>&nbsp;</p>


<b>SAI API</b>

The following header files define the Application Programming Interface (API) for the SAI interface:
  - \b %Driver_SAI.h : Driver API for Serial Audio Interface

The driver implementation is a typical part of the
<a class="el" href="../../Pack/html/index.html" target="_blank">Device Family Pack (DFP)</a> that supports the peripherals of
the microcontroller family.


<b>Driver Functions</b>

The driver functions are published in the access struct as explained in \ref DriverFunctions
  - \ref ARM_DRIVER_SAI : access struct for SAI driver functions
  
\section Driver_SAI_I2S I2S
<a href="https://en.wikipedia.org/wiki/I%C2%B2S" target="_blank">Integrated Interchip Sound</a> (\b I2S) is a serial bus
interface that connects digital audio devices together. It was introduced by Philips (now
<a href="http://www.nxp.com" target="_blank">NXP</a>) in the late 80's and last revised 1996. It uses pulse code modulation
to exchange the audio data between the devices. The following timing diagram explains the operation:
 
\image html driver_sai_i2s.png

I2S separates the clock (\b SCK) from the serial data (\b SD), resulting in a lower jitter. A complete audio data frame
consists of two slots, one for the left channel and one for the right. The slot size equals the data size.
The word select (\b WS) line lets the device know whether the left channel (WS is low) or the right channel (WS is high) is
currently being transmitted. WS has a 50% duty-cycle signal that has the same frequency as the sample frequency. It is an
early signal, meaning that the WS line changes one clock cycle before the actual data (SD) is transmitted (left or right).
The data on SD is always transmitted MSB first and can have a data size of 8 up to 32 bits.

In terms of the CMSIS-Driver for SAI, the I2S protocol can be described as follows:
- Data Size: 8..32 (MSB first)
- Clock Polarity: Drive on falling edge, Capture on rising edge
- Frame Length: 2 * Data Size = 2 * Slot Size
- Frame Sync Width: Frame Length / 2
- Frame Sync Polarity: Active Low
- Frame Sync Early
- Slot Count: 2 (L R)
- Slot Size: Data Size
- Slot Offset: 0


\section Driver_SAI_MSB MSB Justified
\b MSB \b Justified is much like \ref Driver_SAI_I2S, with a few differences:

\image html driver_sai_msb.png

Unlike I2S, in MSB Justified the word select (\b WS) signals the left channel when it is active high and the right channel,
when it is active low. The signal changes when the first actual \b SD data is available. It might happen that a frame (left
or right) is not fully filled with data. In this case, all data after the LSB is forced to zero.

In terms of the CMSIS-Driver for SAI, the MSB Justified protocol can be described as follows:
- Data Size: 8..32 (MSB first)
- Clock Polarity: Drive on falling edge, Capture on rising edge
- Frame Length: 2 * Slot Size
- Frame Sync Width: Frame Length / 2
- Frame Sync Polarity: Active High
- Slot Count: 2 (L R)
- Slot Size: Data Size or higher (16/32)
- Slot Offset: 0 (Zero padding after Data: Slot Size - Data Size)

\section Driver_SAI_LSB LSB Justified
\b LSB \b Justified is much like \ref Driver_SAI_MSB, with the single difference that the padding 0's are sent before the
first actual data (MSB on \b SD):

\image html driver_sai_lsb.png

In terms of the CMSIS-Driver for SAI, the LSB Justified protocol can be described as follows:
- Data Size: 8..32 (MSB first)
- Clock Polarity: Drive on falling edge, Capture on rising edge
- Frame Length: 2*Slot Size
- Frame Sync Width: Frame Length / 2
- Frame Sync Polarity: Active High
- Slot Count: 2
- Slot Size: Data Size or higher (16/32)
- Slot Offset: Slot Size - Data Size (Zero padding before Data: Slot Size - Data Size)

\section Driver_SAI_PCM PCM
<a href="https://en.wikipedia.org/wiki/Pulse-code_modulation" target="_blank">Pulse Code Modulation</a> (\b PCM) differs to
the previous protocols in a few ways:

\image html driver_sai_pcm.png

- Only one channel is transferred.

- There are two types of synchronization modes available:
  - In \b short \b frame sync mode, the falling edge of \b Frame \b Sync indicates the start of the serial data \b SD. \b Frame \b Sync is always
    one clock cycle long.
  - In \b long \b frame sync mode, the rising edge of \b Frame \b Sync indicates the start of the serial data \b SD. \b Frame \b Sync stays active
    high for 13 clock cycles.

In terms of the CMSIS-Driver for SAI, the PCM protocol can be described as follows:\n
\b PCM \b Short \b Frame
- Data Size: 8..32 (MSB first)
- Clock Polarity: Drive on falling edge, Capture on rising edge
- Frame Length: Slot Size
- Frame Sync Width: 1
- Frame Sync Polarity: Active High
- Frame Sync Early
- Slot Count: 1
- Slot Size: Data Size or higher (16/32)
- Slot Offset: 0

\b PCM \b Long \b Frame
- Data Size: 16..32 (MSB first)
- Clock Polarity: Drive on falling edge, Capture on rising edge
- Frame Length: Slot Size
- Frame Sync Width: 13
- Frame Sync Polarity: Active High
- Slot Count: 1
- Slot Size: Data Size or higher (32)
- Slot Offset: 0

\section Driver_SAI_AC97 AC'97
<a href="https://en.wikipedia.org/wiki/AC'97" target="_blank">Audio Codec '97</a> was developed by
<a href="http://www.intel.com" target="_blank">Intel</a>. It is composed of five wires: the clock (12.288 MHz), a sync
signal, a reset signal, and two data wires: sdata_out (contains the AC97 output) and sdata_in (contains the CODEC output).
For more information, consult the
<a href="http://www-inst.eecs.berkeley.edu/~cs150/Documents/ac97_r23.pdf" target="_blank">standard documentation</a>.

\section Driver_SAI_User User Defined Protocol
Using the control structs of the CMSIS-Driver SAI, it is possible to create support for nearly all serial audio protocols
that are available today. 

\image html driver_sai_user.png

The following properties can be configured for a user protocol:
- Data Size in bits (8..32)
- Data Bit Order: MSB first (default) or LSB first
- Clock Polarity:
  - Driver on falling edge, Capture on rising edge (default)
  - Driver on rising edge, Capture on falling edge
- Frame Length in bits
- Frame Sync Width in bits (default=1)
- Frame Sync Polarity: active high (default) or low
- Frame Sync Early: Sync signal one bit before the first bit of frame
- Slot Count: number of slots in frame (default=1)
- Slot Size: equal to data size (default) or 16 or 32-bit
- Slot Offset: offset of first data bit in slot (default=0)

For more information, refer to \ref ARM_SAI_Control that explains the different configuration options in more detail.

@{
*/


/** 
\struct     ARM_DRIVER_SAI
\details
The functions of the SAI driver are accessed by function pointers exposed by this structure.
Refer to \ref DriverFunctions for overview information.

Each instance of an SAI interface provides such an access structure. 
The instance is identified by a postfix number in the symbol name of the access structure, for example:
 - \b Driver_SAI0 is the name of the access struct of the first instance (no. 0).
 - \b Driver_SAI1 is the name of the access struct of the second instance (no. 1).

A middleware configuration setting allows connecting the middleware to a specific driver instance \b %Driver_SAI<i>n</i>.
The default is \token{0}, which connects a middleware to the first instance of a driver.
*****************************************************************************************************************/

/**
\struct     ARM_SAI_CAPABILITIES 
\details
An SAI driver can be implemented with different capabilities (for example protocol support). The data fields of this
structure encode the capabilities implemented by this driver. If a certain hardware peripheral is not able to handle one
of the protocols directly (not advertised using ARM_SAI_CAPABILITIES), then it might be possible to implement it using
the \ref Driver_SAI_User (if supported).

<b>Returned by:</b>
  - \ref ARM_SAI_GetCapabilities
*****************************************************************************************************************/

/**
\struct ARM_SAI_STATUS 
\details
Structure with information about the status of the SAI. The data fields encode busy flags and error flags.

<b>Returned by:</b>
  - \ref ARM_SAI_GetStatus
*****************************************************************************************************************/

/**
\typedef    ARM_SAI_SignalEvent_t
\details
Provides the typedef for the callback function \ref ARM_SAI_SignalEvent.

<b>Parameter for:</b>
  - \ref ARM_SAI_Initialize
*******************************************************************************************************************/


/****** SAI specific error codes *****/
/**
\defgroup sai_execution_status Status Error Codes
\ingroup common_drv_gr
\brief Negative values indicate errors (SAI has specific codes in addition to common \ref execution_status). 
\details 
The SAI driver has additional status error codes that are listed below.
\note
- In case multiple errors exist, only the first encountered error will be reported.
- errors ARM_SAI_ERROR_BIT_ORDER, ARM_SAI_ERROR_FRAME_SYNC_xxx, ARM_SAI_ERROR_SLOT_xxx will only be reported in \ref Driver_SAI_User mode.
- The SAI driver also returns the common \ref execution_status.
  
@{
\def ARM_SAI_ERROR_SYNCHRONIZATION       
The \b synchronization requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_PROTOCOL
The \b protocol requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_DATA_SIZE     
The <b>data size</b> requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_BIT_ORDER
The <b>bit order</b> requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_MONO_MODE            
The <b>mono mode</b> requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_COMPANDING   
The <b>companding</b> requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_CLOCK_POLARITY 
The <b>clock polarity</b> requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_AUDIO_FREQ
The <b>audio frequency</b> requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_MCLK_PIN
The <b>MCLK pin</b> setting requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_MCLK_PRESCALER       
The <b>MCLK prescaler</b> requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_FRAME_LENGHT
The <b>frame length</b> requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_FRAME_SYNC_WIDTH 
The <b>frame sync width</b> requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_FRAME_SYNC_POLARITY
The <b>frame sync polarity</b> requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_FRAME_SYNC_EARLY      
The <b>frame sync early</b> requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_SLOT_COUNT 
The <b>slot count</b> requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_SLOT_SIZE
The <b>slot size</b> requested with the function \ref ARM_SAI_Control is not supported.

\def ARM_SAI_ERROR_SLOT_OFFESET
The <b>slot offset</b> requested with the function \ref ARM_SAI_Control is not supported.
@}
*/


/****** SAI Event *****/
/**
\defgroup SAI_events SAI Events
\ingroup sai_interface_gr
\brief The SAI driver generates call back events that are notified via the function \ref ARM_SAI_SignalEvent.
\details 
This section provides the event values for the \ref ARM_SAI_SignalEvent callback function.

The following call back notification events are generated:
@{
\def ARM_SAI_EVENT_SEND_COMPLETE    
\def ARM_SAI_EVENT_RECEIVE_COMPLETE 
\def ARM_SAI_EVENT_TX_UNDERFLOW     
\def ARM_SAI_EVENT_RX_OVERFLOW      
\def ARM_SAI_EVENT_FRAME_ERROR      
@}
*/


/**
\defgroup sai_control SAI Control Codes
\ingroup sai_interface_gr
\brief Many parameters of the SAI driver are configured using the \ref ARM_SAI_Control function.
\details
@{
The various SAI control codes define:
  - \ref sai_configure_control specifies SAI configuration
  - \ref sai_controls specifies SAI controls

Refer to the \ref ARM_SAI_Control function for further details.
*/


/**
\defgroup sai_configure_control SAI Configuration
\ingroup sai_control
\brief Specify Transmitter/Receiver configuration.
\details
@{
Configuration is specified by ORing \b ARM_SAI_CONFIGURE_<i>x</i> with the following parameters:
 - \ref sai_mode_control
 - \ref sai_sync_control
 - \ref sai_protocol_control
 - \ref sai_data_bits_control
 - \ref sai_bit_order_control
 - \ref sai_mono_control
 - \ref sai_clock_pol_control
 - \ref sai_companding_control
 - \ref sai_mclk_pin_control

Additional configuration specified by \em arg1:
 - \ref sai_frame_control
 - \ref sai_slot_control

Additional configuration specified by \em arg2:
 - <b>Audio Frequency</b> (Master only)
 - \ref sai_mclk_pres_control

\defgroup sai_mode_control SAI Mode
\ingroup sai_configure_control
\brief Defines Transmitter/Receiver mode.
\details
@{
\def ARM_SAI_MODE_MASTER
\def ARM_SAI_MODE_SLAVE
@}

\defgroup sai_sync_control SAI Synchronization
\ingroup sai_configure_control
\brief Defines Transmitter/Receiver synchronization.
\details
@{
\def ARM_SAI_ASYNCHRONOUS
\def ARM_SAI_SYNCHRONOUS
@}

\defgroup sai_protocol_control SAI Protocol
\ingroup sai_configure_control
\brief Defines Transmitter/Receiver protocol.
\details
@{
\def ARM_SAI_PROTOCOL_USER
\def ARM_SAI_PROTOCOL_I2S
\def ARM_SAI_PROTOCOL_MSB_JUSTIFIED
\def ARM_SAI_PROTOCOL_LSB_JUSTIFIED
\def ARM_SAI_PROTOCOL_PCM_SHORT
\def ARM_SAI_PROTOCOL_PCM_LONG
\def ARM_SAI_PROTOCOL_AC97
@}

\defgroup sai_data_bits_control SAI Data Size
\ingroup sai_configure_control
\brief Defines data size in bits (per channel/slot).
\details
@{
\def ARM_SAI_DATA_SIZE(n)
@}

\defgroup sai_bit_order_control SAI Bit Order
\ingroup sai_configure_control
\brief Defines the bit order.
\details
@{
\def ARM_SAI_MSB_FIRST
\def ARM_SAI_LSB_FIRST
@}

\defgroup sai_mono_control SAI Mono Mode
\ingroup sai_configure_control
\brief Defines mono mode.
\details
@{
\def ARM_SAI_MONO_MODE
@}

\defgroup sai_companding_control SAI Companding
\ingroup sai_configure_control
\brief Defines companding.
\details
@{
\def ARM_SAI_COMPANDING_NONE
\def ARM_SAI_COMPANDING_A_LAW
\def ARM_SAI_COMPANDING_U_LAW
@}

\defgroup sai_clock_pol_control SAI Clock Polarity
\ingroup sai_configure_control
\brief Defines clock polarity.
\details
@{
\def ARM_SAI_CLOCK_POLARITY_0
\def ARM_SAI_CLOCK_POLARITY_1
@}

\defgroup sai_frame_control SAI Frame
\ingroup sai_configure_control
\brief Defines frame.
\details
@{
\def ARM_SAI_FRAME_LENGTH(n)
\def ARM_SAI_FRAME_SYNC_WIDTH(n)
\def ARM_SAI_FRAME_SYNC_POLARITY_HIGH
\def ARM_SAI_FRAME_SYNC_POLARITY_LOW
\def ARM_SAI_FRAME_SYNC_EARLY
@}

\defgroup sai_slot_control SAI Slot
\ingroup sai_configure_control
\brief Defines data slots.
\details
@{
\def ARM_SAI_SLOT_COUNT(n)
\def ARM_SAI_SLOT_SIZE_DEFAULT
\def ARM_SAI_SLOT_SIZE_16
\def ARM_SAI_SLOT_SIZE_32
\def ARM_SAI_SLOT_OFFSET(n)
@}

\defgroup sai_mclk_pin_control SAI Master Clock Pin
\ingroup sai_configure_control
\brief Defines MCLK pin.
\details
@{
\def ARM_SAI_MCLK_PIN_INACTIVE
\def ARM_SAI_MCLK_PIN_OUTPUT
\def ARM_SAI_MCLK_PIN_INPUT
@}

\defgroup sai_mclk_pres_control SAI Master Clock Prescaler
\ingroup sai_configure_control
\brief Defines MCLK prescaler.
\details
@{
\def ARM_SAI_MCLK_PRESCALER(n)
@}

@}
*/


/**
\defgroup sai_controls SAI Controls
\ingroup sai_control
\brief Specifies controls.
\details
@{
\def ARM_SAI_CONFIGURE_TX
\sa ARM_SAI_Control
\def ARM_SAI_CONFIGURE_RX
\sa ARM_SAI_Control
\def ARM_SAI_CONTROL_TX
\sa ARM_SAI_Control; ARM_SAI_Send
\def ARM_SAI_CONTROL_RX
\sa ARM_SAI_Control; ARM_SAI_Receive
\def ARM_SAI_MASK_SLOTS_TX
\sa ARM_SAI_Control; ARM_SAI_Send
\def ARM_SAI_MASK_SLOTS_RX
\sa ARM_SAI_Control; ARM_SAI_Receive
\def ARM_SAI_ABORT_SEND
\sa ARM_SAI_Control; ARM_SAI_Send
\def ARM_SAI_ABORT_RECEIVE
\sa ARM_SAI_Control; ARM_SAI_Receive
@}
*/


/**
@}
*/
// end group SAI_control


//
// Function documentation
//

ARM_DRIVER_VERSION ARM_SAI_GetVersion (void)  {
  return { 0, 0 };
}
/**
\fn ARM_DRIVER_VERSION ARM_SAI_GetVersion (void)
\details
The function \b ARM_SAI_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION
 - API version is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version is source code version of the actual driver implementation.

\b Example:
\code
extern ARM_DRIVER_SAI Driver_SAI0;
ARM_DRIVER_SAI *drv_info;
 
void setup_sai (void)  {
  ARM_DRIVER_VERSION  version;
 
  drv_info = &Driver_SAI0;  
  version = drv_info->GetVersion ();
  if (version.api < 0x10A)  {      // requires at minimum API version 1.10 or higher
    // error handling
    return;
  }
}
\endcode
*****************************************************************************************************************/

ARM_SAI_CAPABILITIES ARM_SAI_GetCapabilities (void)  {
  return {0};
}
/**
\details
\fn ARM_SAI_CAPABILITIES ARM_SAI_GetCapabilities (void)
The function \b  ARM_SAI_GetCapabilities retrieves information about the capabilities in this driver implementation.
The data fields of the struct \ref ARM_SAI_CAPABILITIES encode various capabilities, for example
supported protocols, or if a hardware is capable to create signal events using the \ref ARM_SAI_SignalEvent 
callback function.
 
\b Example:
\code
extern ARM_DRIVER_SAI Driver_SAI0;
ARM_DRIVER_SAI *drv_info;
  
void read_capabilities (void)  {
  ARM_SAI_CAPABILITIES drv_capabilities;
 
  drv_info = &Driver_SAI0;  
  drv_capabilities = drv_info->GetCapabilities ();
  // interrogate capabilities
 
}
\endcode
*****************************************************************************************************************/

int32_t ARM_SAI_Initialize (ARM_SAI_SignalEvent_t cb_event)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_SAI_Initialize (ARM_SAI_SignalEvent_t cb_event)
\details
The function \b ARM_SAI_Initialize initializes the SAI interface. It is called when the middleware component starts
operation.

The function performs the following operations:
  - Initializes the required resources of the SAI interface.
  - Registers the \ref ARM_SAI_SignalEvent callback function.

The parameter \em cb_event is a pointer to the \ref ARM_SAI_SignalEvent callback function; use a NULL pointer 
when no callback signals are required.
*****************************************************************************************************************/

int32_t ARM_SAI_Uninitialize (void)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_SAI_Uninitialize (void)
\details
The function \b ARM_SAI_Uninitialize de-initializes the resources of SAI interface.

It is called when the middleware component stops operation and releases the software resources used by the interface.
*****************************************************************************************************************/
  
int32_t ARM_SAI_PowerControl (ARM_POWER_STATE state)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_SAI_PowerControl (ARM_POWER_STATE state)
\details
The function \b ARM_SAI_PowerControl allows you to control the power modes of the SAI interface.  

The parameter \em state sets the operation and can have the following values:
  - \ref ARM_POWER_FULL : set-up peripheral for data transfers, enable interrupts (NVIC) and optionally DMA. 
                          Can be called multiple times. If the peripheral is already in this mode the function performs 
						  no operation and returns with \ref ARM_DRIVER_OK.
  - \ref ARM_POWER_LOW : may use power saving. Returns \ref ARM_DRIVER_ERROR_UNSUPPORTED when not implemented.
  - \ref ARM_POWER_OFF : terminates any pending data transfers, disables peripheral, disables related interrupts and DMA.
      
Refer to \ref CallSequence for more information.
*****************************************************************************************************************/

int32_t ARM_SAI_Send (const void *data, uint32_t num)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_SAI_Send (const void *data, uint32_t num) 
\details
The function \b ARM_SAI_Send sends data to the SAI transmitter.

The function parameters specify the buffer with \a data and the number \a num of items to send.
The item size is defined by the data type which depends on the configured number of data bits.

Data type is:
 - \em uint8_t when configured for \token{8} data bits
 - \em uint16_t when configured for \token{9..16} data bits
 - \em uint32_t when configured for \token{17..32} data bits

Transmitter is enabled by calling \ref ARM_SAI_Control with \ref ARM_SAI_CONTROL_TX as the control parameter and \token{1} as
an argument. This starts the transmit engine which, generates a clock and frame sync signal in master mode and transmits the
data. In slave mode, clock and frame sync are generated by the external master. When mute is active, data is discarded and
zero values are transmitted. 
 
Calling the function <b>ARM_SAI_Send</b> only starts the send operation. The function is non-blocking and returns as soon as
the driver has started the operation (the driver typically configures DMA or the interrupt system for continuous transfer).
During the operation it is not allowed to call this function again. Also, the data buffer must stay allocated and the
contents of unsent data must not be modified. When the send operation is completed (requested number of items have been
sent), the event \ref ARM_SAI_EVENT_SEND_COMPLETE is generated. Progress of the send operation can be monitored by reading
the number of already sent items by calling the function \ref ARM_SAI_GetTxCount. 

The status of the transmitter can also be monitored by calling the function \ref ARM_SAI_GetStatus and checking the \em tx_busy flag,
which indicates if a transmission is still in progress.

If the transmitter is enabled and data is to be sent but the send operation has not been started yet, then the event
\ref ARM_SAI_EVENT_TX_UNDERFLOW is generated.

If an invalid synchronization frame is detected in slave mode, then the event \ref ARM_SAI_EVENT_FRAME_ERROR is generated (if
supported and reported by \em event_frame_error in \ref ARM_SAI_CAPABILITIES). 

The send operation can be aborted by calling the function \ref ARM_SAI_Control with the control parameter \ref ARM_SAI_ABORT_SEND.
*****************************************************************************************************************/

int32_t ARM_SAI_Receive (void *data, uint32_t num)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_SAI_Receive (void *data, uint32_t num)
\details
The function \b ARM_SAI_Receive is used to receive data from the SAI receiver. The function parameters specify the buffer for
\a data and the number \a num of items to receive. The item size is defined by the data type, which depends on the configured
number of data bits.

Data type is:
 - \em uint8_t when configured for \token{8} data bits
 - \em uint16_t when configured for \token{9..16} data bits
 - \em uint32_t when configured for \token{17..32} data bits

The receiver is enabled by calling the function \ref ARM_SAI_Control with the control parameter \ref ARM_SAI_CONTROL_RX and the value \token{1}
for the parameter \em arg1. This starts the receive engine, which generates a clock and frame sync signal in master mode and receives
data. In slave mode, clock and frame sync are generated by the external master. 
 
Calling the function <b>ARM_SAI_Receive</b> only starts the receive operation. The function is non-blocking and returns as
soon as the driver has started the operation (the driver typically configures DMA or the interrupt system for continuous
transfer). During the operation, it is not allowed to call this function again. The data buffer must also stay allocated.
When receive operation is completed (the requested number of items have been received), the
\ref ARM_SAI_EVENT_RECEIVE_COMPLETE event is generated. Progress of the receive operation can also be monitored by reading
the number of items already received by calling the function \ref ARM_SAI_GetRxCount. 

The status of the receiver can also be monitored by calling the function \ref ARM_SAI_GetStatus and checking the \em rx_busy flag, which
indicates whether a reception is still in progress. 

When the receiver is enabled and data is received but the receive operation has not been started yet, then the event
\ref ARM_SAI_EVENT_RX_OVERFLOW is generated.

If an invalid synchronization frame is detected in slave mode, then the event \ref ARM_SAI_EVENT_FRAME_ERROR is generated (if
supported and reported by \em event_frame_error in \ref ARM_SAI_CAPABILITIES). 

The receive operation can be aborted by calling the function \ref ARM_SAI_Control with the control parameter \ref ARM_SAI_ABORT_RECEIVE.
*****************************************************************************************************************/

uint32_t ARM_SAI_GetTxCount (void)  {
  return 0;
}
/**
\fn uint32_t ARM_SAI_GetTxCount (void)
\details
The function \b ARM_SAI_GetTxCount returns the number of the currently transmitted data items during an \ref ARM_SAI_Send
operation.
*****************************************************************************************************************/

uint32_t ARM_SAI_GetRxCount (void)  {
  return 0;
}
/**
\fn uint32_t ARM_SAI_GetRxCount (void)
\details
The function \b ARM_SAI_GetRxCount returns the number of the currently received data items during an \ref ARM_SAI_Receive
operation.
*****************************************************************************************************************/

int32_t ARM_SAI_Control (uint32_t control, uint32_t arg1, uint32_t arg2)  {
  return ARM_DRIVER_OK;
}

/**
\fn int32_t ARM_SAI_Control (uint32_t control, uint32_t arg1, uint32_t arg2)
\details
The function \b ARM_SAI_Control controls the SAI interface and executes various operations.

The parameter \em control specifies the operation. Values are listed in the table <a href="#sai_contrl_tab"><b>Parameter <i>control</i></b></a>.\n
The parameter \em arg1 provides,  depending on the operation,  additional information or sets values. 
Refer to table <a href="#sai_arg1_tab"><b>Parameter <i>arg1</i></b></a>. \n
The parameter \em arg2 provides,  depending on the operation and/or \em arg1,  additional information or sets values. 

The driver provides a receiver/transmitter pair of signals. 
In asynchronous operation mode, they operate completely independent from each other. 
In synchronous operation mode, the synchronous channel uses the Clock (SCK) and Frame Sync (WS) signal from the asynchronous one
(control category <a href="#sai_sync"><b>Synchronization</b></a>).

The clock polarity can be set for every protocol, regardless whether it is already predefined for I2S, MSB/LSB Jusitified
(control category <a href="#sai_clk_polarity"><b>Clock Polarity</b></a>).

A master clock provides a faster clock from which the frame can be derived (usually 256 x faster than the normal frame clock). 
You can use a master clock only in master mode. A slave will always have only one clock 
(control category <a href="#master_clock"><b>Master Clock pin (MCLK)</b></a>).

\anchor sai_contrl_tab		
The table lists the operation values for \em control. Values from different categories can be ORed.
<table class="cmtable" summary="">
<tr><th> Parameter \em control              </th><th>                                       Bit </th><th>             Category      </th>
    <th> Description                        </th></tr>
<tr><td> \ref ARM_SAI_CONFIGURE_TX          </td><td rowspan="9" style="text-align:right"> 0..7 </td><td rowspan="9"> Operation    </td>
    <td> Configure transmitter. \em arg1 (see  <a href="#sai_arg1_tab"><b>Parameter <i>arg1</i></b></a>) and \em arg2 provide additional configuration.  </td></tr>
<tr><td> \ref ARM_SAI_CONFIGURE_TX          </td>    
    <td> Configure transmitter. \em arg1 (see  <a href="#sai_arg1_tab"><b>Parameter <i>arg1</i></b></a>) and \em arg2 provide additional configuration.  </td></tr>
<tr><td> \ref ARM_SAI_CONFIGURE_RX          </td>
    <td> Configure transmitter. \em arg1 and \em arg2 provide additional configuration.  </td></tr>
<tr><td> \ref ARM_SAI_CONTROL_TX                </td>
    <td> Enable or disable transmitter and control mute; 
	\em arg1.0 : \token{0=disable (default); 1=enable;} \em arg1.1 : \token{mute} (see \ref ARM_SAI_Send) </td></tr>
<tr><td> \ref ARM_SAI_CONTROL_RX            </td>
    <td> Enable or disable receiver; \em arg1.0 : \token{0=disable (default); 1=enable} (see \ref ARM_SAI_Receive)  </td></tr>
<tr><td> \ref ARM_SAI_MASK_SLOTS_TX         </td>
    <td> Mask transmitter slots; \em arg1 = \token{mask} (bit: 0=active, 1=inactive); all configured slots are active by default.  </td></tr>
<tr><td> \ref ARM_SAI_MASK_SLOTS_RX         </td>
    <td> Mask receiver slots; \em arg1 = \token{mask} (bit: 0=active, 1=inactive); all configured slots are active by default.  </td></tr>
<tr><td> \ref ARM_SAI_ABORT_SEND            </td>
    <td> Abort send operation (see \ref ARM_SAI_Send).  </td></tr>
<tr><td> \ref ARM_SAI_ABORT_RECEIVE         </td>
    <td> Abort receive operation (see \ref ARM_SAI_Receive).  </td></tr>
<tr><td> \ref ARM_SAI_MODE_MASTER           </td><td rowspan="2" style="text-align:right"> 8    </td><td rowspan="2"> Mode             </td>
    <td> Master mode. \em arg2 specifies the audio frequency in [Hz].  You can also set the <a href="#master_clock"><b>Master Clock pin</b></a>.</td></tr>
<tr><td> \ref ARM_SAI_MODE_SLAVE (default)  </td>
    <td> Slave mode.  </td></tr>
<tr><td> \ref ARM_SAI_ASYNCHRONOUS (default)    \anchor sai_sync </td><td rowspan="2" style="text-align:right">  9  </td><td rowspan="2"> Synchronization </td>
    <td> Asynchronous operation using own clock and sync signal.  </td></tr>
<tr><td> \ref ARM_SAI_SYNCHRONOUS               </td> 
    <td> Synchronous operation using clock and sync signal from other transmitter/receiver.  </td></tr>
<tr><td> \ref ARM_SAI_PROTOCOL_USER  (default)  </td><td rowspan="7" style="text-align:right"> 10..12 </td><td rowspan="7"> Protocol     </td>
    <td> User defined                           </td></tr>
<tr><td> \ref ARM_SAI_PROTOCOL_I2S              </td>
    <td> I2C                                    </td></tr>
<tr><td> \ref ARM_SAI_PROTOCOL_MSB_JUSTIFIED    </td> 
    <td> MSB (left) justified                   </td></tr>
<tr><td> \ref ARM_SAI_PROTOCOL_LSB_JUSTIFIED    </td> 
    <td> LSB (right) justified                  </td></tr>
<tr><td> \ref ARM_SAI_PROTOCOL_PCM_SHORT        </td> 
    <td> PCM with short frame                   </td></tr>
<tr><td> \ref ARM_SAI_PROTOCOL_PCM_LONG         </td> 
    <td> PCM with long frame                    </td></tr>
<tr><td> \ref ARM_SAI_PROTOCOL_AC97             </td> 
    <td> AC'97                                  </td></tr>
<tr><td> \ref ARM_SAI_DATA_SIZE(n)              </td><td style="text-align:right"> 13..17 </td><td> Data Size    </td>     
    <td> Data size in bits; the range for \em n is \token{8..32}. See also: <a href="#frame_slot_size"><b>Frame Slot Size</b></a>.    </td></tr>
<tr><td> \ref ARM_SAI_MSB_FIRST                 </td><td rowspan="2" style="text-align:right"> 18     </td><td rowspan="2"> Bit Order    </td>     
    <td> Data is transferred with MSB first.    </td></tr>
<tr><td> \ref ARM_SAI_LSB_FIRST                 </td>
    <td> Data is transferred with LSB first (User protocol only, ignored otherwise).     </td></tr>
<tr><td> \ref ARM_SAI_MONO_MODE                 </td><td style="text-align:right"> 19     </td><td> Mono Mode</td>      
    <td> Only for I2S, MSB/LSB justified. 
	     When using \ref Driver_SAI_I2S in mono mode, only data for a single channel is sent to and received from the driver.
         Hardware will duplicate the data for the second channel on transmit and ignore the second channel on receive.    </td></tr>
<tr><td> \ref ARM_SAI_COMPANDING_NONE (default) </td><td rowspan="3" style="text-align:right"> 20..22 </td><td rowspan="3"> Companding    </td>
    <td> No companding  </td></tr>
<tr><td> \ref ARM_SAI_COMPANDING_A_LAW          </td>   
    <td> A-Law companding (8-bit data)          </td></tr>
<tr><td> \ref ARM_SAI_COMPANDING_U_LAW          </td>   
    <td> u-Law companding (8-bit data)          </td></tr>
<tr><td> \ref ARM_SAI_CLOCK_POLARITY_0&nbsp;(default)  \anchor sai_clk_polarity > </td><td rowspan="2" style="text-align:right">    23   </td><td rowspan="2"> Clock Polarity </td>
    <td> Drive on falling edge, capture on rising  edge. </td></tr>
<tr><td> \ref ARM_SAI_CLOCK_POLARITY_1         \anchor master_clock </td>
    <td> Drive on rising  edge, capture on falling edge. </td></tr>
<tr><td> \ref ARM_SAI_MCLK_PIN_INACTIVE&nbsp;(default)  </td><td rowspan="3" style="text-align:right"> 24..26 </td><td rowspan="3"> Master Clock pin (MCLK) </td>
    <td> MCLK not used.                         </td></tr>
<tr><td> \ref ARM_SAI_MCLK_PIN_OUTPUT           </td>   
    <td> MCLK is output (Master mode only).     </td></tr>
<tr><td> \ref ARM_SAI_MCLK_PIN_INPUT            </td>   
    <td> MCLK is input (Master mode only).      </td></tr>
</table>

\anchor sai_arg1_tab
The parameter \em arg1 provides frame-specific values depending on the \em control operation. Values from different categories can be ORed.
<table class="cmtable" summary="">
<tr><th nowrap> Parameter \em arg1       </th>
    <th style="text-align:right">   Bit  </th>
	<th> Category                        </th>
    <th> Description                     </th></tr>
<tr><td> \ref ARM_SAI_FRAME_LENGTH(n)    </td>
    <td style="text-align:right">  0..9  </td>
	<td> Frame Length                    </td>
    <td> Frame length in bits; the possible range for \em n is \token{8..1024}; default depends on protocol and data.    </td></tr>
<tr><td> \ref ARM_SAI_FRAME_SYNC_WIDTH(n)</td>
    <td style="text-align:right"> 10..17 </td>
	<td> Frame Sync Width                </td>
    <td> Frame Sync width in bits; the possible range for \em n is \token{1..256}; \token{default=1}; User protocol only, ignored otherwise.  </td></tr>
<tr><td> \ref ARM_SAI_FRAME_SYNC_POLARITY_HIGH   </td>
    <td rowspan="2" style="text-align:right"> 18 </td>
	<td rowspan="2" style="white-spaces:nowrap"> Frame Sync Polarity  </td>
    <td> Frame Sync is active high (default).    </td></tr>
<tr><td> \ref ARM_SAI_FRAME_SYNC_POLARITY_LOW    </td>
    <td> Frame Sync is active low (User protocol only, ignored otherwise).   </td></tr>
<tr><td> \ref ARM_SAI_FRAME_SYNC_EARLY           </td>
    <td style="text-align:right">  19  </td>
	<td> Frame Sync Early              </td>
    <td> Frame Sync one bit before the first bit of the frame (User protocol only, ignored otherwise).                   </td></tr>
<tr><td> \ref ARM_SAI_SLOT_COUNT(n)              </td>
    <td style="text-align:right"> 20..24         </td>
	<td> Frame Sync Count                        </td>
    <td> Number of slots in frame; the possible range for \em n is \token{1..32}; default=\token{1};  User protocol only, ignored otherwise.  </td></tr>
<tr><td> \ref ARM_SAI_SLOT_SIZE_DEFAULT   \anchor frame_slot_size  </td>
    <td rowspan="3" style="text-align:right"> 25..26                </td>
	<td rowspan="3">  Frame Slot Size                               </td>
    <td> Slot size is equal to data size (default).                 </td></tr>
<tr><td> \ref ARM_SAI_SLOT_SIZE_16                                  </td>
    <td> Slot size is \token{16 bits} (User protocol only, ignored otherwise).     </td></tr>
<tr><td> \ref ARM_SAI_SLOT_SIZE_32                                  </td>
    <td> Slot size is \token{32 bits} (User protocol only, ignored otherwise).     </td></tr>
<tr><td> \ref ARM_SAI_SLOT_OFFSET(n)     </td>
    <td style="text-align:right"> 27..31 </td>
	<td> Frame Slot Offset               </td>
    <td> Offset of first data bit in slot; The range for \em n is \token{0..31}; default=\token{0};  User protocol only, ignored otherwise.   </td></tr>
</table>


\anchor mckl_prescaler
Depending on the \em control operation, the parameter \em arg2 specifies the Master Clock (MCLK) prescaler and calculates the audio frequency automatically.

Parameter \em arg2                       | MCLK Prescaler
:----------------------------------------|:--------------------------------------------
\ref ARM_SAI_MCLK_PRESCALER(n)           | MCLK prescaler; Audio frequency = MCLK/n; the range for \em n is \token{1..4096}; default=\token{1}.


\b Example

\code
extern ARM_DRIVER_SAI Driver_SAI0;
 
// configure Transmitter to Asynchronous Master: I2S Protocol, 16-bit data, 16kHz Audio frequency
status = Driver_SAI0.Control(ARM_SAI_CONFIGURE_TX | 
                             ARM_SAI_MODE_MASTER  | 
                             ARM_SAI_ASYNCHRONOUS | 
                             ARM_SAI_PROTOCOL_I2S | 
                             ARM_SAI_DATA_SIZE(16), 0, 16000);
 
// configure Receiver to Asynchronous Master: I2S Protocol, 16-bit data, 16kHz Audio frequency
status = Driver_SAI0.Control(ARM_SAI_CONFIGURE_RX | 
                             ARM_SAI_MODE_MASTER  | 
                             ARM_SAI_ASYNCHRONOUS | 
                             ARM_SAI_PROTOCOL_I2S | 
                             ARM_SAI_DATA_SIZE(16), 0, 16000);
 
// enable Transmitter
status = Driver_SAI0.Control(ARM_SAI_CONTROL_TX, 1, 0);
 
// enable Receiver
status = Driver_SAI0.Control(ARM_SAI_CONTROL_RX, 1, 0);
\endcode

*****************************************************************************************************************/

ARM_SAI_STATUS ARM_SAI_GetStatus (void)  {
  return { 0 };
}
/**
\fn ARM_SAI_STATUS ARM_SAI_GetStatus (void)
\details
The function \b ARM_SAI_GetStatus retrieves the current SAI interface status.
*****************************************************************************************************************/

void ARM_SAI_SignalEvent (uint32_t event)  {
  // function body
}
/**
\fn void ARM_SAI_SignalEvent (uint32_t event)
\details
The function \b ARM_SAI_SignalEvent is a callback function registered by the function \ref ARM_SAI_Initialize. 

The parameter \em event indicates one or more events that occurred during driver operation.
Each event is encoded in a separate bit and therefore it is possible to signal multiple events within the same call. 

The following events can be generated:

Parameter \em event                        | Bit | Description 
------------------------------------------ |:---:|:-----------
\ref ARM_SAI_EVENT_SEND_COMPLETE           |  0  | Occurs after call to \ref ARM_SAI_Send to indicate that all the data has been sent (or queued in transmit buffers). The driver is ready for the next call to \ref ARM_SAI_Send.
\ref ARM_SAI_EVENT_RECEIVE_COMPLETE        |  1  | Occurs after call to \ref ARM_SAI_Receive to indicate that all the data has been received. The driver is ready for the next call to \ref ARM_SAI_Receive.
\ref ARM_SAI_EVENT_TX_UNDERFLOW            |  2  | Occurs when data is to be sent but send operation has not been started. Data field \em tx_underflow = \token{1} of \ref ARM_SAI_STATUS.
\ref ARM_SAI_EVENT_RX_OVERFLOW             |  3  | Occurs when data is received but receive operation has not been started. Data field \em rx_underflow = \token{1} of \ref ARM_SAI_STATUS.
\ref ARM_SAI_EVENT_FRAME_ERROR             |  4  | Occurs in slave mode when invalid synchronization frame is detected. Data field \em  event_frame_error = \token{1} of \ref ARM_SAI_STATUS.
  
*****************************************************************************************************************/

/**
@}
*/ 
// End SAI Interface
