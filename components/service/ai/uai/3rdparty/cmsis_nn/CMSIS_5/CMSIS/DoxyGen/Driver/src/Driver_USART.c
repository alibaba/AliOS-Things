/**
\defgroup usart_interface_gr USART Interface
\brief   Driver API for Universal Synchronous Asynchronous Receiver/Transmitter (%Driver_USART.h)
\details 
The <b>Universal Synchronous Asynchronous Receiver/Transmitter</b> (USART) implements a synchronous and asynchronous serial bus for exchanging data. 
When only asynchronous mode is supported it is called Universal Asynchronous Receiver/Transmitter (UART).  
Almost all microcontrollers have a serial interface (UART/USART peripheral). A UART is a simple device to send data to a PC
via a terminal emulation program (Hyperterm, TeraTerm) or to another microcontroller.
A UART takes bytes of data and transmits the individual bits in a sequential mode. At the destination, 
a second UART reassembles the bits into complete bytes. Each UART contains a shift register for converting between serial and parallel transmission forms. 
Wikipedia offers more information about  
the <a href="http://en.wikipedia.org/wiki/Universal_asynchronous_receiver/transmitter" target="_blank"><b>Universal asynchronous receiver/transmitter</b></a>.


<b>USART API</b>

The following header files define the Application Programming Interface (API) for the USART interface:
  - \b %Driver_USART.h : Driver API for Universal Synchronous Asynchronous Receiver/Transmitter

The driver implementation is a typical part of the Device Family Pack (DFP) that supports the 
peripherals of the microcontroller family.


<b>Driver Functions</b>

The driver functions are published in the access struct as explained in \ref DriverFunctions
  - \ref ARM_DRIVER_USART : access struct for USART driver functions

  
<b>Example Code</b>

The following example code shows the usage of the USART interface for asynchronous communication.

\include USART_Demo.c

@{
*/

/** 
\struct     ARM_DRIVER_USART
\details
The functions of the USART driver are accessed by function pointers exposed by this structure.
Refer to \ref DriverFunctions for overview information.

Each instance of an USART interface provides such an access structure. 
The instance is identified by a postfix number in the symbol name of the access structure, for example:
 - \b Driver_USART0 is the name of the access struct of the first instance (no. 0).
 - \b Driver_USART1 is the name of the access struct of the second instance (no. 1).

A middleware configuration setting allows connecting the middleware to a specific driver instance \b %Driver_USART<i>n</i>.
The default is \token{0}, which connects a middleware to the first instance of a driver.
*****************************************************************************************************************/

/**
\struct     ARM_USART_CAPABILITIES 
\details
An USART driver can be implemented with different capabilities.
The data fields of this structure encode the capabilities implemented by this driver.

<b>Returned by:</b>
  - \ref ARM_USART_GetCapabilities
*****************************************************************************************************************/

/**
\struct     ARM_USART_STATUS
\details
Structure with information about the status of the USART. The data fields encode busy flags and error flags.

<b>Returned by:</b>
  - \ref ARM_USART_GetStatus
*****************************************************************************************************************/

/**
\enum     ARM_USART_MODEM_CONTROL
\details
Specifies values for controlling the modem control lines.

<b>Parameter for:</b>
  - \ref ARM_USART_SetModemControl
*****************************************************************************************************************/

/**
\struct     ARM_USART_MODEM_STATUS 
\details
Structure with information about the status of modem lines. The data fields encode states of modem status lines.

<b>Returned by:</b>
  - \ref ARM_USART_GetModemStatus
*****************************************************************************************************************/


/**
\typedef    ARM_USART_SignalEvent_t
\details
Provides the typedef for the callback function \ref ARM_USART_SignalEvent.

<b>Parameter for:</b>
  - \ref ARM_USART_Initialize
*******************************************************************************************************************/


/**
\defgroup usart_execution_status Status Error Codes
\ingroup common_drv_gr
\brief Negative values indicate errors (USART has specific codes in addition to common \ref execution_status). 
\details 
The USART driver has additional status error codes that are listed below.
Note that the USART driver also returns the common \ref execution_status. 
  
@{
\def ARM_USART_ERROR_MODE
The \b mode requested with the function \ref ARM_USART_Control is not supported by this driver.

\def ARM_USART_ERROR_BAUDRATE
The <b>baude rate</b> requested with the function \ref ARM_USART_Control is not supported by this driver.

\def ARM_USART_ERROR_DATA_BITS
The number of <b>data bits</b> requested with the function \ref ARM_USART_Control is not supported by this driver.

\def ARM_USART_ERROR_PARITY
The <b>parity bit</b> requested with the function \ref ARM_USART_Control is not supported by this driver.

\def ARM_USART_ERROR_STOP_BITS
The <b>stop bit</b> requested with the function \ref ARM_USART_Control is not supported by this driver.

\def ARM_USART_ERROR_FLOW_CONTROL
The <b>flow control</b> requested with the function \ref ARM_USART_Control is not supported by this driver.

\def ARM_USART_ERROR_CPOL
The <b>clock polarity</b> requested with the function \ref ARM_USART_Control is not supported by this driver.

\def ARM_USART_ERROR_CPHA
The <b>clock phase</b> requested with the function \ref ARM_USART_Control is not supported by this driver.
@}
*/


/**
\defgroup USART_events USART Events
\ingroup usart_interface_gr
\brief The USART driver generates call back events that are notified via the function \ref ARM_USART_SignalEvent.
\details 
This section provides the event values for the \ref ARM_USART_SignalEvent callback function.

The following call back notification events are generated:
@{
\def ARM_USART_EVENT_SEND_COMPLETE
\def ARM_USART_EVENT_RECEIVE_COMPLETE
\def ARM_USART_EVENT_TRANSFER_COMPLETE
\def ARM_USART_EVENT_TX_COMPLETE
\def ARM_USART_EVENT_TX_UNDERFLOW
\def ARM_USART_EVENT_RX_OVERFLOW
\def ARM_USART_EVENT_RX_TIMEOUT
\def ARM_USART_EVENT_RX_BREAK
\def ARM_USART_EVENT_RX_FRAMING_ERROR
\def ARM_USART_EVENT_RX_PARITY_ERROR
\def ARM_USART_EVENT_CTS
\def ARM_USART_EVENT_DSR
\def ARM_USART_EVENT_DCD
\def ARM_USART_EVENT_RI
@}
*/


/**
\defgroup USART_control USART Control Codes
\ingroup usart_interface_gr
\brief Many parameters of the USART driver are configured using the \ref ARM_USART_Control function.
\details
@{
The various USART control codes define:
  - \ref usart_mode_control specifies USART mode
  - \ref usart_data_bits defines the number of data bits
  - \ref usart_parity_bit defines the parity bit
  - \ref usart_stop_bits defines the number of stop bits
  - \ref usart_flow_control specifies RTS/CTS flow control
  - \ref usart_clock_polarity defines the clock polarity for the synchronous mode
  - \ref usart_clock_phase defines the clock phase for the synchronous mode
  - \ref usart_misc_control specifies additional miscellaneous controls

Refer to the \ref ARM_USART_Control function for further details.

*/


/**
\defgroup usart_mode_control USART Mode Control
\ingroup USART_control
\brief Specify USART mode.
\details
@{
\def ARM_USART_MODE_ASYNCHRONOUS
\sa ARM_USART_Control
\def ARM_USART_MODE_SYNCHRONOUS_MASTER
\sa ARM_USART_Control
\def ARM_USART_MODE_SYNCHRONOUS_SLAVE
\sa ARM_USART_Control
\def ARM_USART_MODE_SINGLE_WIRE
\sa ARM_USART_Control
\def ARM_USART_MODE_IRDA
\sa ARM_USART_Control
\def ARM_USART_MODE_SMART_CARD
\sa ARM_USART_Control
@}
*/


/**
\defgroup usart_misc_control USART Miscellaneous Control
\ingroup USART_control
\brief Specifies additional miscellaneous controls.
\details
@{
\def ARM_USART_SET_DEFAULT_TX_VALUE
\sa ARM_USART_Control;  ARM_USART_Receive;
\def ARM_USART_SET_IRDA_PULSE
\sa ARM_USART_Control
\def ARM_USART_SET_SMART_CARD_GUARD_TIME
\sa ARM_USART_Control
\def ARM_USART_SET_SMART_CARD_CLOCK
\sa ARM_USART_Control
\def ARM_USART_CONTROL_SMART_CARD_NACK
\sa ARM_USART_Control
\def ARM_USART_CONTROL_TX
\sa ARM_USART_Control;  ARM_USART_Send;  ARM_USART_Transfer
\def ARM_USART_CONTROL_RX
\sa ARM_USART_Control;  ARM_USART_Receive; ARM_USART_Transfer; 
\def ARM_USART_CONTROL_BREAK
\sa ARM_USART_Control
\def ARM_USART_ABORT_SEND
\sa ARM_USART_Control;
\def ARM_USART_ABORT_RECEIVE
\sa ARM_USART_Control;
\def ARM_USART_ABORT_TRANSFER
\sa ARM_USART_Control;
@}
*/

/**
\defgroup usart_data_bits  USART Data Bits
\ingroup USART_control
\brief Defines the number of data bits.
\details
@{
\def ARM_USART_DATA_BITS_5
\sa ARM_USART_Control
\def ARM_USART_DATA_BITS_6
\sa ARM_USART_Control
\def ARM_USART_DATA_BITS_7
\sa ARM_USART_Control
\def ARM_USART_DATA_BITS_8
\sa ARM_USART_Control
\def ARM_USART_DATA_BITS_9
\sa ARM_USART_Control
@}
*/

/**
\defgroup usart_parity_bit  USART Parity Bit
\ingroup USART_control
\brief Defines the parity bit.
\details
@{
\def ARM_USART_PARITY_NONE
\sa ARM_USART_Control
\def ARM_USART_PARITY_EVEN
\sa ARM_USART_Control
\def ARM_USART_PARITY_ODD
\sa ARM_USART_Control
@}
*/

/**
\defgroup usart_stop_bits  USART Stop Bits
\ingroup USART_control
\brief Defines the number of stop bits.
\details
@{
\sa ARM_USART_Control
\def ARM_USART_STOP_BITS_1
\sa ARM_USART_Control
\def ARM_USART_STOP_BITS_2
\sa ARM_USART_Control
\def ARM_USART_STOP_BITS_1_5
\sa ARM_USART_Control
\def ARM_USART_STOP_BITS_0_5
\sa ARM_USART_Control
@}
*/

/**
\defgroup usart_flow_control USART Flow Control
\ingroup USART_control
\brief Specifies RTS/CTS flow control.
\details
@{
\def ARM_USART_FLOW_CONTROL_NONE
\sa ARM_USART_Control
\def ARM_USART_FLOW_CONTROL_RTS
\sa ARM_USART_Control
\def ARM_USART_FLOW_CONTROL_CTS
\sa ARM_USART_Control
\def ARM_USART_FLOW_CONTROL_RTS_CTS
\sa ARM_USART_Control
@}
*/

/**
\defgroup usart_clock_polarity USART Clock Polarity
\ingroup USART_control
\brief Defines the clock polarity for the synchronous mode.
\details
@{
\def ARM_USART_CPOL0
\sa ARM_USART_Control; ARM_USART_Receive; ARM_USART_Send;  ARM_USART_Transfer
\def ARM_USART_CPOL1
\sa ARM_USART_Control; ARM_USART_Receive; ARM_USART_Send;  ARM_USART_Transfer
@}
*/

/**
\defgroup usart_clock_phase USART Clock Phase 
\ingroup USART_control
\brief Defines the clock phase for the synchronous mode.
\details
@{
\def ARM_USART_CPHA0
\sa ARM_USART_Control; ARM_USART_Receive; ARM_USART_Send;  ARM_USART_Transfer
\def ARM_USART_CPHA1
\sa ARM_USART_Control; ARM_USART_Receive; ARM_USART_Send;  ARM_USART_Transfer
@}
*/

/**
@}
*/
// end group USART_control


//
//   Functions
//

ARM_DRIVER_VERSION ARM_USART_GetVersion (void)  {
  return { 0, 0 };
}
/**
\fn     ARM_DRIVER_VERSION ARM_USART_GetVersion (void)
\details
The function \b ARM_USART_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION
 - API version is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version is source code version of the actual driver implementation.

Example:
\code
extern ARM_DRIVER_USART Driver_USART0;
ARM_DRIVER_USART *drv_info;
 
void setup_usart (void)  {
  ARM_DRIVER_VERSION  version;
 
  drv_info = &Driver_USART0;  
  version = drv_info->GetVersion ();
  if (version.api < 0x10A)  {      // requires at minimum API version 1.10 or higher
    // error handling
    return;
  }
}
\endcode
*****************************************************************************************************************/

ARM_USART_CAPABILITIES ARM_USART_GetCapabilities (void)  {
  return  { 0 } ;
}
/**
\fn       ARM_USART_CAPABILITIES ARM_USART_GetCapabilities (void)
\details
The function \b ARM_USART_GetCapabilities returns information about capabilities in this driver implementation.
The data fields of the structure \ref ARM_USART_CAPABILITIES encode various capabilities, for example
supported modes, if a hardware is capable to create signal events using the \ref ARM_USART_SignalEvent 
callback function ...
 
Example:
\code
extern ARM_DRIVER_USART Driver_USART0;
ARM_DRIVER_USART *drv_info;
  
void read_capabilities (void)  {
  ARM_USART_CAPABILITIES drv_capabilities;
 
  drv_info = &Driver_USART0;  
  drv_capabilities = drv_info->GetCapabilities ();
  // interrogate capabilities
 
}
\endcode
*****************************************************************************************************************/

int32_t ARM_USART_Initialize (ARM_USART_SignalEvent_t cb_event)  {
  return ARM_DRIVER_OK;
}
/**
\fn       int32_t ARM_USART_Initialize (ARM_USART_SignalEvent_t cb_event)
\details
The function \b ARM_USART_Initialize initializes the USART interface. 
It is called when the middleware component starts operation.

The function performs the following operations:
  - Initializes the resources needed for the USART interface.
  - Registers the \ref ARM_USART_SignalEvent callback function.

The parameter \em cb_event is a pointer to the \ref ARM_USART_SignalEvent callback function; use a NULL pointer 
when no callback signals are required.

\b Example:
 - see \ref usart_interface_gr - Driver Functions

*****************************************************************************************************************/

int32_t ARM_USART_Uninitialize (void)  {
  return ARM_DRIVER_OK;
}
/**
\fn       int32_t ARM_USART_Uninitialize (void)
\details
The function \b ARM_USART_Uninitialize de-initializes the resources of USART interface.

It is called when the middleware component stops operation and releases the software resources used by the interface.
*****************************************************************************************************************/

int32_t ARM_USART_PowerControl (ARM_POWER_STATE state)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USART_PowerControl (ARM_POWER_STATE state)
\details
The function \b ARM_USART_PowerControl operates the power modes of the USART interface.  

The parameter \em state sets the operation and can have the following values:
  - \ref ARM_POWER_FULL : set-up peripheral for data transfers, enable interrupts (NVIC) and optionally DMA. 
                          Can be called multiple times. If the peripheral is already in this mode the function performs 
						  no operation and returns with \ref ARM_DRIVER_OK.
  - \ref ARM_POWER_LOW : may use power saving. Returns \ref ARM_DRIVER_ERROR_UNSUPPORTED when not implemented.
  - \ref ARM_POWER_OFF : terminates any pending data transfers, disables peripheral, disables related interrupts and DMA.
      
Refer to \ref CallSequence for more information.

*****************************************************************************************************************/

int32_t ARM_USART_Send (const void *data, uint32_t num)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USART_Send (const void *data, uint32_t num) 
\details
This functions \b ARM_USART_Send is used in asynchronous mode to send data to the USART transmitter.
It can also be used in synchronous mode when sending data only (received data is ignored). 

Transmitter needs to be enabled by calling \ref ARM_USART_Control with \ref ARM_USART_CONTROL_TX as the control parameter and \token{1} as argument. 

The function parameters specify the buffer with data and the number of items to send.
The item size is defined by the data type which depends on the configured number of data bits.

Data type is:
 - \em uint8_t when configured for 5..8 data bits
 - \em uint16_t when configured for 9 data bits
 
Calling the function <b>ARM_USART_Send</b> only starts the send operation.
The function is non-blocking and returns as soon as the driver has started the operation (driver typically configures DMA or the interrupt system for continuous transfer).
When in synchronous slave mode the operation is only registered and started when the master starts the transfer.
During the operation it is not allowed to call this function again or any other data transfer function when in synchronous mode. Also the data buffer must stay allocated and the contents of unsent data must not be modified.
When send operation is completed (requested number of items sent) the \ref ARM_USART_EVENT_SEND_COMPLETE event is generated.
Progress of send operation can also be monitored by reading the number of items already sent by calling \ref ARM_USART_GetTxCount. 

After send operation has completed there might still be some data left in the driver's hardware buffer which is still being transmitted.
When all data has been physically transmitted the \ref ARM_USART_EVENT_TX_COMPLETE event is generated (if supported and reported by \em event_tx_complete in \ref ARM_USART_CAPABILITIES).
At that point also the \em tx_busy data field in \ref ARM_USART_STATUS is cleared.

Status of the transmitter can be monitored by calling the \ref ARM_USART_GetStatus and checking the \em tx_busy flag
which indicates if transmission is still in progress. 

When in synchronous slave mode and transmitter is enabled but send/receive/transfer operation is not started and data is requested by the master then the \ref ARM_USART_EVENT_TX_UNDERFLOW event is generated. 

Send operation can be aborted by calling \ref ARM_USART_Control with \ref ARM_USART_ABORT_SEND as the control parameter.
*****************************************************************************************************************/

int32_t ARM_USART_Receive (void *data, uint32_t num)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USART_Receive (void *data, uint32_t num)
\details
This functions \b ARM_USART_Receive is used in asynchronous mode to receive data from the USART receiver.
It can also be used in synchronous mode when receiving data only (transmits the default value as specified by \ref ARM_USART_Control with \ref ARM_USART_SET_DEFAULT_TX_VALUE as control parameter). 

Receiver needs to be enabled by calling \ref ARM_USART_Control with \ref ARM_USART_CONTROL_RX as the control parameter and \token{1} as argument. 

The function parameters specify the buffer for data and the number of items to receive.
The item size is defined by the data type which depends on the configured number of data bits.

Data type is:
 - \em uint8_t when configured for 5..8 data bits
 - \em uint16_t when configured for 9 data bits
 
Calling the function <b>ARM_USART_Receive</b> only starts the receive operation.
The function is non-blocking and returns as soon as the driver has started the operation (driver typically configures DMA or the interrupt system for continuous transfer).
When in synchronous slave mode the operation is only registered and started when the master starts the transfer.
During the operation it is not allowed to call this function again or any other data transfer function when in synchronous mode. Also the data buffer must stay allocated.
When receive operation is completed (requested number of items received) the \ref ARM_USART_EVENT_RECEIVE_COMPLETE event is generated.
Progress of receive operation can also be monitored by reading the number of items already received by calling \ref ARM_USART_GetRxCount. 

Status of the receiver can be monitored by calling the \ref ARM_USART_GetStatus and checking the \em rx_busy flag
which indicates if reception is still in progress. 

During reception the following events can be generated (in asynchronous mode):
 - \ref ARM_USART_EVENT_RX_TIMEOUT : Receive timeout between consecutive characters detected (optional)
 - \ref ARM_USART_EVENT_RX_BREAK : Break detected (Framing error is not generated for Break condition)
 - \ref ARM_USART_EVENT_RX_FRAMING_ERROR : Framing error detected
 - \ref ARM_USART_EVENT_RX_PARITY_ERROR : Parity error detected
 - \ref ARM_USART_EVENT_RX_OVERFLOW : Data overflow detected (also in synchronous slave mode)

\ref ARM_USART_EVENT_RX_OVERFLOW event is also generated when receiver is enabled but data is lost because 
receive operation in asynchronous mode or receive/send/transfer operation in synchronous slave mode has not been started.

Receive operation can be aborted by calling \ref ARM_USART_Control with \ref ARM_USART_ABORT_RECEIVE as the control parameter.
*****************************************************************************************************************/

int32_t ARM_USART_Transfer (const void *data_out, void *data_in, uint32_t num) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USART_Transfer (const void *data_out, void *data_in, uint32_t num) 
\details
This functions \b ARM_USART_Transfer is used in synchronous mode to transfer data via USART. It synchronously sends data to the USART transmitter and receives data from the USART receiver. 

Transmitter needs to be enabled by calling \ref ARM_USART_Control with \ref ARM_USART_CONTROL_TX as the control parameter and \token{1} as argument. 
Receiver needs to be enabled by calling \ref ARM_USART_Control with \ref ARM_USART_CONTROL_RX as the control parameter and \token{1} as argument. 

The function parameters specify the buffer with data to send, the buffer for data to receive and the number of items to transfer.
The item size is defined by the data type which depends on the configured number of data bits.

Data type is:
 - \em uint8_t when configured for 5..8 data bits
 - \em uint16_t when configured for 9 data bits
 
Calling the function <b>ARM_USART_Transfer</b> only starts the transfer operation.
The function is non-blocking and returns as soon as the driver has started the operation (driver typically configures DMA or the interrupt system for continuous transfer).
When in synchronous slave mode the operation is only registered and started when the master starts the transfer.
During the operation it is not allowed to call this function or any other data transfer function again. Also the data buffers must stay allocated and the contents of unsent data must not be modified.
When transfer operation is completed (requested number of items transferred) the \ref ARM_USART_EVENT_TRANSFER_COMPLETE event is generated.
Progress of transfer operation can also be monitored by reading the number of items already transferred by calling \ref ARM_USART_GetTxCount or \ref ARM_USART_GetRxCount. 

Status of the transmitter or receiver can be monitored by calling the \ref ARM_USART_GetStatus and checking the \em tx_busy or \em rx_busy flag. 

When in synchronous slave mode also the following events can be generated:
 - \ref ARM_USART_EVENT_TX_UNDERFLOW : transmitter is enabled but transfer operation is not started and data is requested by the master
 - \ref ARM_USART_EVENT_RX_OVERFLOW : data lost during transfer or because receiver is enabled but transfer operation has not been started

Transfer operation can also be aborted by calling \ref ARM_USART_Control with \ref ARM_USART_ABORT_TRANSFER as the control parameter.
*****************************************************************************************************************/

uint32_t ARM_USART_GetTxCount (void)  {
  return 0;
}
/**
\fn uint32_t ARM_USART_GetTxCount (void)
\details
The function \b ARM_USART_GetTxCount returns the number of the currently transmitted data items during \ref ARM_USART_Send and \ref ARM_USART_Transfer operation.
*****************************************************************************************************************/

uint32_t ARM_USART_GetRxCount (void)  {
  return 0;
}
/**
\fn uint32_t ARM_USART_GetRxCount (void)
\details
The function \b  ARM_USART_GetRxCount returns the number of the currently received data items during \ref ARM_USART_Receive and \ref ARM_USART_Transfer operation.
*****************************************************************************************************************/

int32_t ARM_USART_Control (uint32_t control, uint32_t arg)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USART_Control (uint32_t control, uint32_t arg)
\details
The function \b ARM_USART_Control control the USART interface settings and execute various operations.

The parameter \em control sets the operation and is explained in the table below.
Values from different categories can be ORed with the exception of \ref usart_misc_tab "Miscellaneous Operations".

The parameter \em arg provides, depending on the operation, additional information, for example the baudrate.

The table lists the available \em control operations.

<TABLE class="cmtable" summary="">
<TR><TH>Parameter \em control           </TH>  <TH style="text-align:right"> Bit                 </TH>     <TH> Category </TH> <TH>Description </TH></TR>
<TR><TD>\ref ARM_USART_MODE_ASYNCHRONOUS</TD>         <td rowspan="6" style="text-align:right"> 0..7    </td>     <td rowspan="6"> Operation Mode  </td><TD>Set to asynchronous UART mode. \em arg specifies baudrate.</TD></TR>
<TR><TD>\ref ARM_USART_MODE_SYNCHRONOUS_MASTER</TD>   <TD>Set to synchronous master mode with clock signal generation. \em arg specifies baudrate.</TD></TR>
<TR><TD>\ref ARM_USART_MODE_SYNCHRONOUS_SLAVE</TD>    <TD>Set to synchronous slave mode with external clock signal.</TD></TR>
<TR><TD>\ref ARM_USART_MODE_SINGLE_WIRE</TD>          <TD>Set to single-wire (half-duplex) mode. \em arg specifies baudrate.</TD></TR>
<TR><TD>\ref ARM_USART_MODE_IRDA</TD>                 <TD>Set to Infra-red data mode. \em arg specifies baudrate.</TD></TR>
<TR><TD>\ref ARM_USART_MODE_SMART_CARD</TD>           <TD>Set to Smart Card mode. \em arg specifies baudrate.</TD></TR>
<TR><TD>\ref ARM_USART_DATA_BITS_5</TD>        <td rowspan="5" style="text-align:right"> 8..11    </td>     <td rowspan="5"> Data Bits  </td><TD>Set to \token{5} data bits</TD></TR>
<TR><TD>\ref ARM_USART_DATA_BITS_6</TD>           <TD>Set to \token{6} data bits</TD></TR>
<TR><TD>\ref ARM_USART_DATA_BITS_7</TD>           <TD>Set to \token{7} data bits</TD></TR>
<TR><TD>\ref ARM_USART_DATA_BITS_8</TD>           <TD>Set to \token{8} data bits (default)</TD></TR>
<TR><TD>\ref ARM_USART_DATA_BITS_9</TD>           <TD>Set to \token{9} data bits</TD></TR>
<TR><TD>\ref ARM_USART_PARITY_EVEN</TD>        <td rowspan="3" style="text-align:right"> 12..13 </td>  <td rowspan="3"> Parity Bit  </td><TD>Set to Even Parity</TD></TR>
<TR><TD>\ref ARM_USART_PARITY_NONE</TD>           <TD>Set to No Parity (default)</TD></TR>
<TR><TD>\ref ARM_USART_PARITY_ODD</TD>            <TD>Set to Odd Parity</TD></TR>
<TR><TD>\ref ARM_USART_STOP_BITS_1</TD>        <td rowspan="4" style="text-align:right"> 14..15 </td>  <td rowspan="4"> Stop Bit  </td><TD>Set to \token{1} Stop bit (default)</TD></TR>
<TR><TD>\ref ARM_USART_STOP_BITS_2</TD>           <TD>Set to \token{2} Stop bits</TD></TR>
<TR><TD>\ref ARM_USART_STOP_BITS_1_5</TD>         <TD>Set to \token{1.5} Stop bits</TD></TR>
<TR><TD>\ref ARM_USART_STOP_BITS_0_5</TD>         <TD>Set to \token{0.5} Stop bits</TD></TR>
<TR><TD>\ref ARM_USART_FLOW_CONTROL_NONE</TD>  <td rowspan="4" style="text-align:right"> 16..17 </td>  <td rowspan="4"> Flow Control  </td><TD>No flow control signal (default)</TD></TR>
<TR><TD>\ref ARM_USART_FLOW_CONTROL_CTS</TD>      <TD>Set to use the CTS flow control signal</TD></TR> 
<TR><TD>\ref ARM_USART_FLOW_CONTROL_RTS</TD>      <TD>Set to use the RTS flow control signal</TD></TR>
<TR><TD>\ref ARM_USART_FLOW_CONTROL_RTS_CTS</TD>  <TD>Set to use the RTS and CTS flow control signal</TD></TR> 
<TR><TD>\ref ARM_USART_CPOL0</TD>              <td rowspan="2" style="text-align:right"> 18     </td>  <td rowspan="2"> Clock Polarity  </td><TD>CPOL=\token{0} (default) : data are captured on rising edge (low->high transition)</TD></TR> 
<TR><TD>\ref ARM_USART_CPOL1</TD>                 <TD>CPOL=\token{1} : data are captured on falling edge (high->lowh transition)</TD></TR> 
<TR><TD>\ref ARM_USART_CPHA0</TD>              <td rowspan="2" style="text-align:right"> 19     </td>  <td rowspan="2"> Clock Phase  </td><TD>CPHA=\token{0} (default) : sample on first (leading) edge</TD></TR>
<TR><TD>\ref ARM_USART_CPHA1</TD>                 <TD>CPHA=\token{1} : sample on second (trailing) edge</TD></TR>
<TR><TD>\ref ARM_USART_ABORT_RECEIVE</TD>      <td rowspan="11" style="text-align:right"> 0..19  </td>  <td rowspan="11"> \anchor usart_misc_tab Miscellaneous Operations <br>(cannot be ORed)  </td><TD>Abort receive operation (see also: \ref ARM_USART_Receive)</TD></TR>
<TR> <TD>\ref ARM_USART_ABORT_SEND</TD>                 <TD>Abort  send operation   (see also: \ref ARM_USART_Send)</TD></TR>
<TR> <TD>\ref ARM_USART_ABORT_TRANSFER</TD>             <TD>Abort transfer operation  (see also: \ref ARM_USART_Transfer)</TD></TR>
<TR> <TD>\ref ARM_USART_CONTROL_BREAK</TD>              <TD>Enable or disable continuous Break transmission; \em arg : \token{0=disabled; 1=enabled}</TD></TR> 
<TR> <TD>\ref ARM_USART_CONTROL_RX</TD>                 <TD>Enable or disable receiver; \em arg : \token{0=disabled; 1=enabled} (see also: \ref ARM_USART_Receive; \ref ARM_USART_Transfer)</TD></TR>
<TR> <TD>\ref ARM_USART_CONTROL_SMART_CARD_NACK</TD>    <TD>Enable or disable Smart Card NACK generation; \em arg : \token{0=disabled; 1=enabled}</TD></TR> 
<TR> <TD>\ref ARM_USART_CONTROL_TX</TD>                 <TD>Enable or disable transmitter; \em arg : \token{0=disabled; 1=enabled} (see also: \ref ARM_USART_Send; \ref ARM_USART_Transfer)</TD></TR>
<TR> <TD>\ref ARM_USART_SET_DEFAULT_TX_VALUE</TD>       <TD>Set the default transmit value (synchronous receive only); \em arg specifies the value. (see also: \ref ARM_USART_Receive)</TD></TR>
<TR> <TD>\ref ARM_USART_SET_IRDA_PULSE</TD>             <TD>Set the IrDA pulse value in \token{ns}; \em arg : \token{0=3/16 of bit period}</TD></TR>
<TR> <TD>\ref ARM_USART_SET_SMART_CARD_CLOCK</TD>       <TD>Set the Smart Card Clock in \token{Hz}; \em arg : \token{0=Clock not set}</TD></TR>
<TR> <TD>\ref ARM_USART_SET_SMART_CARD_GUARD_TIME</TD>  <TD>Set the Smart Card guard time; \em arg = number of bit periods</TD></TR>
</TABLE>

\b Example

\code
  extern ARM_DRIVER_USART Driver_USART0;
  
  // configure to UART mode: 8 bits, no parity, 1 stop bit, no flow control, 9600 bps
  status = Driver_USART0.Control(ARM_USART_MODE_ASYNCHRONOUS | 
                                 ARM_USART_DATA_BITS_8 | 
                                 ARM_USART_PARITY_NONE | 
                                 ARM_USART_STOP_BITS_1 | 
                                 ARM_USART_FLOW_CONTROL_NONE, 9600);
 
  // identical with above settings (default settings removed)
  // configure to UART mode: 8 bits, no parity, 1 stop bit, flow control, 9600 bps
  status = Driver_USART0.Control(ARM_USART_MODE_ASYNCHRONOUS, 9600);
 
  // enable TX output
  status = Driver_USART0.Control(ARM_USART_CONTROL_TX, 1);
 
  // disable RX output
  status = Driver_USART0.Control(ARM_USART_CONTROL_RX, 0);
\endcode
*****************************************************************************************************************/

ARM_USART_STATUS ARM_USART_GetStatus (void)  {
  return { 0 };
}
/**
\fn ARM_USART_STATUS ARM_USART_GetStatus (void)
\details
The function \b ARM_USART_GetStatus retrieves the current USART interface status.

*****************************************************************************************************************/

int32_t ARM_USART_SetModemControl (ARM_USART_MODEM_CONTROL control)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USART_SetModemControl (ARM_USART_MODEM_CONTROL control)
\details
The function \b ARM_USART_SetModemControl activates or deactivates the selected USART modem control line. 

The function \ref ARM_USART_GetModemStatus returns information about status of the modem lines.

*****************************************************************************************************************/

ARM_USART_MODEM_STATUS ARM_USART_GetModemStatus (void)  {
  return { 0 };
}
/**
\fn ARM_USART_MODEM_STATUS ARM_USART_GetModemStatus (void)
\details
The function \b ARM_USART_GetModemStatus returns the current USART Modem Status lines state.

The function \ref ARM_USART_SetModemControl sets the modem control lines of the USART.

*****************************************************************************************************************/

void ARM_USART_SignalEvent (uint32_t event)  {
  // function body
}
/**
\fn void ARM_USART_SignalEvent (uint32_t event)
\details
The function \b ARM_USART_SignalEvent is a callback function registered by the function \ref ARM_USART_Initialize. 

The parameter \em event indicates one or more events that occurred during driver operation.
Each event is encoded in a separate bit and therefore it is possible to signal multiple events within the same call. 

Not every event is necessarily generated by the driver. This depends on the implemented capabilities stored in the 
data fields of the structure \ref ARM_USART_CAPABILITIES, which can be retrieved with the function \ref ARM_USART_GetCapabilities.

The following events can be generated:

<table class="cmtable" summary="">
<tr>
  <th> Parameter \em event                      </th><th> Bit </th><th> Description </th>                                              
  <th> supported when ARM_USART_CAPABILITIES    </th>
</tr>
<tr>
  <td> \ref ARM_USART_EVENT_SEND_COMPLETE       </td><td>  0  </td><td> Occurs after call to \ref ARM_USART_Send to indicate that all the data to be sent 
                                                                        was processed by the driver. All the data might have been already transmitted 
																		or parts of it are still queued in transmit buffers. The driver is ready for the next 
																		call to \ref ARM_USART_Send; however USART may still transmit data. </td>     
  <td> <i>always supported</i> </td>
</tr>
<tr>
  <td> \ref ARM_USART_EVENT_RECEIVE_COMPLETE    </td><td>  1  </td><td> Occurs after call to \ref ARM_USART_Receive to indicate that all the data has been 
                                                                        received. The driver is ready for the next call to \ref ARM_USART_Receive. </td>     
  <td> <i>always supported</i> </td>
</tr>
<tr>
  <td> \ref ARM_USART_EVENT_TRANSFER_COMPLETE   </td><td>  2  </td><td> Occurs after call to \ref ARM_USART_Transfer to indicate that all the data has been 
                                                                        transferred. The driver is ready for the next call to \ref ARM_USART_Transfer. </td>     
  <td> <i>always supported</i> </td>
</tr>
<tr>
  <td> \ref ARM_USART_EVENT_TX_COMPLETE         </td><td>  3  </td><td> Occurs after call to \ref ARM_USART_Transfer to indicate that all the data has been 
                                                                        transferred. The driver is ready for the next call to \ref ARM_USART_Transfer. </td>     
  <td> data field \em event_tx_complete = \token{1} </td>
</tr>
<tr>
  <td> \ref ARM_USART_EVENT_TX_UNDERFLOW        </td><td>  4  </td><td> Occurs in synchronous slave mode when data is requested by the master but 
                                                                        send/receive/transfer operation has not been started. 
                                                                        Data field \em  rx_underflow = \token{1} of \ref ARM_USART_STATUS. </td>																		
  <td> <i>always supported</i> </td>
</tr>
<tr>
  <td> \ref ARM_USART_EVENT_RX_OVERFLOW         </td><td>  5  </td><td> Occurs when data is lost during receive/transfer operation or when data is lost 
                                                                        because receive operation in asynchronous mode or receive/send/transfer operation in 
																		synchronous slave mode has not been started.
                                                                        Data field \em  rx_overflow = \token{1} of \ref ARM_USART_STATUS. </td>
  <td> <i>always supported</i> </td>
</tr>
<tr>
  <td> ARM_USART_EVENT_RX_TIMEOUT               </td><td>  6  </td><td> Occurs during receive when idle time is detected between consecutive characters 
                                                                        (idle time is hardware dependent).</td>     
  <td> data field \em event_rx_timeout = \token{1}  </td>
</tr>
<tr>
  <td> \ref ARM_USART_EVENT_RX_BREAK            </td><td>  7  </td><td> Occurs when break is detected during receive. 
                                                                        Data field \em  rx_break = \token{1} of \ref ARM_USART_STATUS. </td>     
  <td> <i>always supported</i> </td>
</tr>
<tr>
  <td> \ref ARM_USART_EVENT_RX_FRAMING_ERROR    </td><td>  8  </td><td> Occurs when framing error is detected during receive. 
                                                                        Data field \em  rx_framing_error = \token{1} of \ref ARM_USART_STATUS. </td>     
  <td> <i>always supported</i> </td>
</tr>
<tr>
  <td> \ref ARM_USART_EVENT_RX_PARITY_ERROR     </td><td>  9  </td><td> Occurs when parity error is detected during receive.
                                                                        Data field \em  rx_parity_error = \token{1} of \ref ARM_USART_STATUS. </td>     
  <td> <i>always supported</i> </td>
</tr>
<tr>
  <td> ARM_USART_EVENT_CTS                      </td><td>  10 </td><td> Indicates that CTS modem line state has changed.
                                                                        Data field \em  cts = \token{1} of \ref ARM_USART_MODEM_STATUS. </td>     
  <td> data field \em event_cts = \token{1} and <br>
       data field \em cts = \token{1}           </td>
</tr>
<tr>
  <td> ARM_USART_EVENT_CTS                      </td><td>  11 </td><td> Indicates that DSR modem line state has changed.
                                                                        Data field \em  dsr = \token{1} of \ref ARM_USART_MODEM_STATUS. </td>     
  <td> data field \em event_dsr = \token{1} and <br>
       data field \em dsr = \token{1}           </td>
</tr>
<tr>
  <td> ARM_USART_EVENT_DCD                      </td><td>  12 </td><td> Indicates that DCD modem line state has changed.
                                                                        Data field \em  dcd = \token{1} of \ref ARM_USART_MODEM_STATUS. </td>     
  <td> data field \em event_dcd = \token{1} and <br>
       data field \em dcd = \token{1}           </td>
</tr>
<tr>
  <td> ARM_USART_EVENT_RI                       </td><td>  13 </td><td> Indicates that RI modem line state has changed.
                                                                        Data field \em  ri = \token{1} of \ref ARM_USART_MODEM_STATUS. </td>     
  <td> data field \em event_ri = \token{1} and  <br>
       data field \em ri = \token{1}            </td>
</tr>
</table>
*****************************************************************************************************************/

/**
@}
*/ 
// End USART Interface
