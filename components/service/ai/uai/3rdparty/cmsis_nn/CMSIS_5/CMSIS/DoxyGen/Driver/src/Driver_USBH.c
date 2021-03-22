/**
\defgroup usbh_interface_gr USB Host Interface
\ingroup usb_interface_gr
\brief  Driver API for USB Host Peripheral (%Driver_USBH.h)
*/

/**
\defgroup usbh_host_gr USB Host
\ingroup usbh_interface_gr
\brief  Driver API for USB Host

@{
*/

/** 
\struct ARM_DRIVER_USBH
\details 
The functions of the USB Host driver are accessed by function pointers. Refer to \ref DriverFunctions for 
overview information.

Each instance of an USBH provides such an access struct. The instance is indicated by
a postfix in the symbol name of the access struct, for example:
 - \b Driver_USBH0 is the name of the access struct of the first instance (no. 0).
 - \b Driver_USBH1 is the name of the access struct of the second instance (no. 1).


A configuration setting in the middleware allows connecting the middleware to a specific driver instance <b>Driver_USBH<i>n</i></b>.
The default is \token{0}, which connects a middleware to the first instance of a driver.

\note    The struct must remain unchanged.
*****************************************************************************************************************/

/**
\struct ARM_USBH_CAPABILITIES 
\details  
A USB Host driver can be implemented with different capabilities.
The data fields of this structure encode the capabilities implemented by this driver.

<b>Returned by:</b>
  - \ref ARM_USBH_GetCapabilities

\note    The struct must remain unchanged.
*****************************************************************************************************************/

/**
\struct     ARM_USBH_PORT_STATE 
\details  
This structure stores information about the state of the USB Host Port. The data fields encode whether a device 
is connected to the port, if port overcurrent is detected, and the port speed.

<b>Returned by:</b>
  - \ref ARM_USBH_PortGetState
*****************************************************************************************************************/

/**
\typedef    uint32_t ARM_USBH_PIPE_HANDLE
\details 
Each pipe is identified through a unique number, which is created by the function \ref ARM_USBH_PipeCreate.

<b>Parameter for:</b>
 - \ref ARM_USBH_PipeModify,
   \ref ARM_USBH_PipeDelete,
   \ref ARM_USBH_PipeReset,
   \ref ARM_USBH_PipeTransfer,
   \ref ARM_USBH_PipeTransferGetResult,
   \ref ARM_USBH_PipeTransferAbort,
   \ref ARM_USBH_SignalPipeEvent

<b>Retruned by:</b>
 - \ref ARM_USBH_PipeCreate
*****************************************************************************************************************/

/**
\typedef    ARM_USBH_SignalPortEvent_t
\details
Provides the typedef for the callback function \ref ARM_USBH_SignalPortEvent.

<b>Parameter for:</b>
  - \ref ARM_USBH_Initialize
*******************************************************************************************************************/

/**
\typedef    ARM_USBH_SignalPipeEvent_t
\details
Provides the typedef for the callback function \ref ARM_USBH_SignalPipeEvent.

<b>Parameter for:</b>
  - \ref ARM_USBH_Initialize
*******************************************************************************************************************/

/**
\defgroup USBH_port_events USBH Port Events
\ingroup usbh_host_gr
\brief The USB Host driver generates Port call back events that are notified via the function \ref ARM_USBH_SignalPortEvent.
\details 
This section provides the event values for the \ref ARM_USBH_SignalPortEvent callback function.

The following call back notification events are generated:
@{
\def  ARM_USBH_EVENT_CONNECT
\def  ARM_USBH_EVENT_DISCONNECT
\def  ARM_USBH_EVENT_OVERCURRENT
\def  ARM_USBH_EVENT_RESET
\def  ARM_USBH_EVENT_SUSPEND
\def  ARM_USBH_EVENT_RESUME
\def  ARM_USBH_EVENT_REMOTE_WAKEUP
@}
*/

/**
\defgroup USBH_pipe_events USBH Pipe Events
\ingroup usbh_host_gr
\brief The USB Host driver generates Pipe call back events that are notified via the function \ref ARM_USBH_SignalPipeEvent.
\details 
This section provides the event values for the \ref ARM_USBH_SignalPipeEvent callback function.

The following call back notification events are generated:
@{
\def  ARM_USBH_EVENT_TRANSFER_COMPLETE
\def  ARM_USBH_EVENT_HANDSHAKE_NAK
\def  ARM_USBH_EVENT_HANDSHAKE_NYET
\def  ARM_USBH_EVENT_HANDSHAKE_MDATA
\def  ARM_USBH_EVENT_HANDSHAKE_STALL
\def  ARM_USBH_EVENT_HANDSHAKE_ERR
\def  ARM_USBH_EVENT_BUS_ERROR
@}
*/

/**
\defgroup USBH_packets USBH Packet Information
\ingroup usbh_host_gr
\brief Specify USB packet information used by the function \ref ARM_USBH_PipeTransfer
\details 
This section provides the packet information values (parameter \em packet) for the \ref ARM_USBH_PipeTransfer function.

The following values are defined:
@{
\def  ARM_USBH_PACKET_SETUP
Generate SETUP transaction.
\def  ARM_USBH_PACKET_OUT
Generate OUT transaction.
\def  ARM_USBH_PACKET_IN
Generate IN transaction.
\def  ARM_USBH_PACKET_PING
Generate PING transaction (no data packet).
\def  ARM_USBH_PACKET_DATA0
Force DATA0 PID (Packet Identifier) for the initial data packet. When not specified than the driver provides the initial value according to the current state.
\def  ARM_USBH_PACKET_DATA1
Force DATA1 PID (Packet Identifier) for the initial data packet. When not specified than the driver provides the initial value according to the current state.
\def  ARM_USBH_PACKET_SSPLIT
Used when driver does not support automatic handling of SPLIT packets and indicates Start-Split packet.
For isochronous OUT it indicates that the High-speed data is in the middle of the Full-speed data payload.
\def  ARM_USBH_PACKET_SSPLIT_S
Used when driver does not support automatic handling of SPLIT packets and indicates Start-Split packet.
Valid only for isochronous OUT and indicates that the High-speed data is the start of the Full-speed data payload.
\def  ARM_USBH_PACKET_SSPLIT_E
Used when driver does not support automatic handling of SPLIT packets and indicates Start-Split packet.
Valid only for isochronous OUT and indicates that the High-speed data is the end of the Full-speed data payload.
\def  ARM_USBH_PACKET_SSPLIT_S_E
Used when driver does not support automatic handling of SPLIT packets and indicates Start-Split packet.
Valid only for isochronous OUT and indicates that the High-speed data is all of the Full-speed data payload.
\def  ARM_USBH_PACKET_CSPLIT
Used when driver does not support automatic handling of SPLIT packets and indicates Complete-Split packet.
\def  ARM_USBH_PACKET_PRE
Generate PRE (Preamble) for low-speed devices within a full/low-speed signaling environment. 
@}
*/


//
// Functions
//

ARM_DRIVER_VERSION ARM_USBH_GetVersion (void)  {
  return { 0, 0 };
}
/**
\fn       ARM_DRIVER_VERSION ARM_USBH_GetVersion (void)
\details
The function \b ARM_USBH_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION
 - API version is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version is source code version of the actual driver implementation.

Example:
\code
extern ARM_DRIVER_USBH Driver_USBH0;
ARM_DRIVER_USBH *drv_info;
 
void setup_usbh (void)  {
  ARM_DRIVER_VERSION  version;
 
  drv_info = &Driver_USBH0;  
  version = drv_info->GetVersion ();
  if (version.api < 0x10A)   {      // requires at minimum API version 1.10 or higher
    // error handling
    return;
  }
}
\endcode
*****************************************************************************************************************/

ARM_USBH_CAPABILITIES ARM_USBH_GetCapabilities (void)  {
  return { 0 };
}
/**
\fn       ARM_USBH_CAPABILITIES ARM_USBH_GetCapabilities (void)
\details
The function \b ARM_USBH_GetCapabilities returns information about capabilities in this driver implementation.
The data fields of the structure \ref ARM_USBH_CAPABILITIES encode various capabilities, for example
available HUB ports or if the hardware can generate signal events using the \ref ARM_USBH_SignalPortEvent 
callback function.
 
Example:
\code
extern ARM_DRIVER_USBH Driver_USBH0;
ARM_DRIVER_USBH *drv_info;
  
void read_capabilities (void)  {
  ARM_USBH_CAPABILITIES drv_capabilities;
 
  drv_info = &Driver_USBH0;  
  drv_capabilities = drv_info->GetCapabilities ();
  // interrogate capabilities
 
}
\endcode
*****************************************************************************************************************/

int32_t ARM_USBH_Initialize (ARM_USBH_SignalPortEvent_t cb_port_event,
                             ARM_USBH_SignalPipeEvent_t cb_pipe_event)  {
  return ARM_DRIVER_OK;
}
/**
\fn       int32_t ARM_USBH_Initialize (ARM_USBH_SignalPortEvent_t cb_port_event, ARM_USBH_SignalPipeEvent_t cb_pipe_event)
\details
The function \b ARM_USBH_Initialize initializes the USB Host interface. 
It is called when the middleware component starts operation.

The function performs the following operations:
  - Initializes the resources needed for the USBH interface.
  - Registers the \ref ARM_USBH_SignalPortEvent callback function.
  - Registers the \ref ARM_USBH_SignalPipeEvent callback function.

The parameter \em cb_port_event is a pointer to the \ref ARM_USBH_SignalPortEvent callback function; use a NULL pointer 
when no port callback signals are required.

The parameter \em cb_pipe_event is a pointer to the \ref ARM_USBH_SignalPipeEvent callback function.

\b Example:
 - see \ref usbh_interface_gr - Driver Functions

*****************************************************************************************************************/

int32_t ARM_USBH_Uninitialize (void) {
  return ARM_DRIVER_OK;
}
/**
\fn       int32_t ARM_USBH_Uninitialize (void)
\details
The function \b ARM_USBH_Uninitialize de-initializes the resources of USB Host interface.

It is called when the middleware component stops operation and releases the software resources used by the interface.
*****************************************************************************************************************/

int32_t ARM_USBH_PowerControl (ARM_POWER_STATE state)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBH_PowerControl (ARM_POWER_STATE state)
\details
The function \b ARM_USBH_PowerControl operates the power modes of the USB Host interface. 

The parameter \em state sets the operation and can have the following values:
  - \ref ARM_POWER_FULL : set-up peripheral for data transfers, enable interrupts (NVIC) and optionally DMA. 
                          Can be called multiple times. If the peripheral is already in this mode the function performs 
						  no operation and returns with \ref ARM_DRIVER_OK.
  - \ref ARM_POWER_LOW : may use power saving. Returns \ref ARM_DRIVER_ERROR_UNSUPPORTED when not implemented.
  - \ref ARM_POWER_OFF : terminates any pending data transfers, disables peripheral, disables related interrupts and DMA.
      
Refer to \ref CallSequence for more information.
*****************************************************************************************************************/

int32_t ARM_USBH_PortVbusOnOff (uint8_t port, bool vbus)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBH_PortVbusOnOff (uint8_t port, bool vbus)
\details
The function \b ARM_USBH_PortVbusOnOff controls the VBUS signal of the specified port.  
*****************************************************************************************************************/

int32_t ARM_USBH_PortReset (uint8_t port)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBH_PortReset (uint8_t port)
\details
Executes reset signalling on the specified port.
*****************************************************************************************************************/

int32_t ARM_USBH_PortSuspend (uint8_t port)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBH_PortSuspend (uint8_t port)
\details
The function \b ARM_USBH_PortSuspend auspends USB signaling on the specified port.
*****************************************************************************************************************/

int32_t ARM_USBH_PortResume (uint8_t port)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBH_PortResume (uint8_t port)
\details
The function \b ARM_USBH_PortResume resumes USB signaling on the specified port.
*****************************************************************************************************************/

ARM_USBH_PORT_STATE ARM_USBH_PortGetState (uint8_t port)  {
  return 0;
}
/**
\fn ARM_USBH_PORT_STATE ARM_USBH_PortGetState (uint8_t port)
\details
The function \b ARM_USBH_PortGetState returns the current state of the specified port.
*****************************************************************************************************************/

ARM_USBH_PIPE_HANDLE ARM_USBH_PipeCreate (uint8_t  dev_addr,
                                          uint8_t  dev_speed,
                                          uint8_t  hub_addr,
                                          uint8_t  hub_port,
                                          uint8_t  ep_addr,
                                          uint8_t  ep_type,
                                          uint16_t ep_max_packet_size,
                                          uint8_t  ep_interval)  {
  return 0;
}
/**
\fn ARM_USBH_PIPE_HANDLE ARM_USBH_PipeCreate (uint8_t dev_addr, uint8_t dev_speed, uint8_t hub_addr, uint8_t hub_port, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_max_packet_size, uint8_t ep_interval)
\details
The function \b ARM_USBH_PipeCreate creates a pipe for transfers (allocates required resources and configures the pipe).

The parameters specify pipe information (connection between host and device endpoint):
 - device: address and speed
 - hub (optional): hub address and number of the hub port to which the device is connected 
 - endpoint: address, type, maximum packet size and polling interval

The function returns an pipe handle that is used for all subsequent operations on that pipe.
In case of errors an invalid handle (\em NULL) is returned.   
*****************************************************************************************************************/

int32_t ARM_USBH_PipeModify (ARM_USBH_PIPE_HANDLE pipe_hndl,
                             uint8_t              dev_addr,
                             uint8_t              dev_speed,
                             uint8_t              hub_addr,
                             uint8_t              hub_port,
                             uint16_t             ep_max_packet_size)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBH_PipeModify (ARM_USBH_PIPE_HANDLE pipe_hndl, uint8_t dev_addr, uint8_t dev_speed, uint8_t hub_addr, uint8_t hub_port, uint16_t ep_max_packet_size)
\details
The function \b ARM_USBH_PipeModify modifies a pipe configuration that was created with \ref ARM_USBH_PipeCreate.
*****************************************************************************************************************/

int32_t ARM_USBH_PipeDelete (ARM_USBH_PIPE_HANDLE pipe_hndl)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBH_PipeDelete (ARM_USBH_PIPE_HANDLE pipe_hndl)
\details
The function \b ARM_USBH_PipeDelete deletes a pipe that was created with \ref ARM_USBH_PipeCreate (deactivates the pipe and releases used resources).
*****************************************************************************************************************/

int32_t ARM_USBH_PipeReset (ARM_USBH_PIPE_HANDLE pipe_hndl)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBH_PipeReset (ARM_USBH_PIPE_HANDLE pipe_hndl)
\details
The function \b ARM_USBH_PipeReset clears Halt condition and resets data toggle on the specified pipe.
*****************************************************************************************************************/

int32_t ARM_USBH_PipeTransfer (ARM_USBH_PIPE_HANDLE pipe_hndl,
                               uint32_t packet,       
                               uint8_t *data,
                               uint32_t num)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBH_PipeTransfer (ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t packet, uint8_t *data, uint32_t num)
\details
The function \b ARM_USBH_PipeTransfer generates packets for sending or receiving data from an USB Endpoint.

The function specifies the buffer with data to send or for data to receive and the number of bytes to transfer (must be multiple of device endpoint maximum packet size for receive).
It also specifies \ref USBH_packets with parameter \em packet.

The function is non-blocking and returns as soon as the driver starts the operation on the specified pipe. During the operation it is not allowed to call this function again on the same pipe. Also the data buffer must stay allocated and the contents of data must not be modified.

Operation is completed when the the requested number of data bytes have been transferred and is indicated with \ref ARM_USBH_EVENT_TRANSFER_COMPLETE event.
It can also finish earlier on reception of different handshake tokens which are also indicated through \ref USBH_pipe_events.
 
Transfer operation can be aborted by calling \ref ARM_USBH_PipeTransferAbort.
*****************************************************************************************************************/

uint32_t ARM_USBH_PipeTransferGetResult (ARM_USBH_PIPE_HANDLE pipe_hndl)  {
  return 0;
}
/**
\fn uint32_t ARM_USBH_PipeTransferGetResult (ARM_USBH_PIPE_HANDLE pipe_hndl)
\details       
The function \b ARM_USBH_PipeTransferGetResult returns the number of successfully transferred data bytes started by \ref ARM_USBH_PipeTransfer operation.
*****************************************************************************************************************/

int32_t ARM_USBH_PipeTransferAbort (ARM_USBH_PIPE_HANDLE pipe_hndl)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBH_PipeTransferAbort (ARM_USBH_PIPE_HANDLE pipe_hndl)
\details

The function \b ARM_USBH_PipeTransferAbort aborts an active pipe transfer started by \ref ARM_USBH_PipeTransfer.
*****************************************************************************************************************/

uint16_t ARM_USBH_GetFrameNumber (void)  {
  return 0;
}
/**
\fn uint16_t ARM_USBH_GetFrameNumber (void)
\details
The function \b ARM_USBH_GetFrameNumber returns the sequential 11-bit frame number of the last Start of Frame (SOF) packet.
*****************************************************************************************************************/

void ARM_USBH_SignalPortEvent (uint8_t port, uint32_t event)  {
  // function body
}
/**
\fn void ARM_USBH_SignalPortEvent (uint8_t port, uint32_t event)
\details
The function \b ARM_USBH_SignalPortEvent is a callback function registered by the function \ref ARM_USBH_Initialize. 

The parameter \em port specifies the root hub port number. \n
The parameter \em event indicates one or more events that occurred during driver operation.
Each event is encoded in a separate bit and therefore it is possible to signal multiple events within the same call. 

Not every event is necessarily generated by the driver. This depends on the implemented capabilities stored in the 
data fields of the structure \ref ARM_USBH_CAPABILITIES, which can be retrieved with the function \ref ARM_USBH_GetCapabilities.

The following events can be generated:

Parameter \em event               | Bit | Description                                                                | supported when ARM_USBH_CAPABILITIES
:---------------------------------|:---:|:---------------------------------------------------------------------------|---------------------------------------
\ref ARM_USBH_EVENT_CONNECT       |  0  | Occurs when USB Device connects to the Host.                               | data field \em event_connect=\token{1}
\ref ARM_USBH_EVENT_DISCONNECT    |  1  | Occurs when USB Device disconnects from the Host.                          | data field \em event_disconnect=\token{1}
\ref ARM_USBH_EVENT_OVERCURRENT   |  2  | Occurs when USB Overcurrent it detected.                                   | data field \em event_overcurrent=\token{1}
\ref ARM_USBH_EVENT_RESET         |  3  | Occurs when USB Reset is completed after calling \ref ARM_USBH_PortReset.  | <i>always supported</i>
\ref ARM_USBH_EVENT_SUSPEND       |  4  | Occurs when USB Suspend is detected.                                       | <i>always supported</i>
\ref ARM_USBH_EVENT_RESUME        |  5  | Occurs when USB Resume is detected.                                        | <i>always supported</i>
\ref ARM_USBH_EVENT_REMOTE_WAKEUP |  6  | Occurs when USB Remote wakeup is detected.                                 | <i>always supported</i>
*****************************************************************************************************************/

void ARM_USBH_SignalPipeEvent (ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t event)  {
  // function body
}
/**
\fn void ARM_USBH_SignalPipeEvent (ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t event)
\details
The function \b ARM_USBH_SignalPipeEvent is a callback function registered by the function \ref ARM_USBH_Initialize. 

The parameter \em pipe_hndl specifies the pipe handle. \n
The parameter \em event indicates one or more events that occurred during driver operation.
Each event is encoded in a separate bit and therefore it is possible to signal multiple events within the same call. 

The following events can be generated:

Parameter \em event                       | Bit| Description 
:-----------------------------------------|---:|:-----------
\ref ARM_USBH_EVENT_TRANSFER_COMPLETE     | 0  | Occurs after all the data has been transferred without errors.
\ref ARM_USBH_EVENT_HANDSHAKE_NAK         | 1  | Occurs when NAK Handshake is received before all the data is transferred.
\ref ARM_USBH_EVENT_HANDSHAKE_NYET        | 2  | Occurs when NYET Handshake is received before all the data is transferred.
\ref ARM_USBH_EVENT_HANDSHAKE_MDATA       | 3  | Occurs when MDATA Handshake is received before all the data is transferred.
\ref ARM_USBH_EVENT_HANDSHAKE_STALL       | 4  | Occurs when STALL Handshake is received before all the data is transferred.
\ref ARM_USBH_EVENT_HANDSHAKE_ERR         | 5  | Occurs when ERR Handshake is received before all the data is transferred.
\ref ARM_USBH_EVENT_BUS_ERROR             | 6  | Occurs when bus error is detected before all the data is transferred.

<b>See also:</b>
 - ARM_USBH_PipeCreate
*****************************************************************************************************************/

/**
@}
*/ 



/**
\defgroup usbh_hci_gr USB OHCI/EHCI
\ingroup usbh_interface_gr
\brief  Driver API for USB OHCI/EHCI
\details
OHCI and EHCI compliant interfaces have memory mapped registers that are used to control the USB host.

Only certain functionalities (interrupts, VBUS control, power control) require device specific interface which is provided through functions
of the struct \ref ARM_DRIVER_USBH_HCI (functionality accessed with the struct \ref ARM_DRIVER_USBH is not needed). 
@{
*/

/** 
\struct ARM_DRIVER_USBH_HCI
\details
The functions of the USB Host HCI (OHCI/EHCI) driver are accessed by function pointers. Refer to \ref DriverFunctions for 
overview information.

Each instance of an USBH provides such an access struct. The instance is indicated by
a postfix in the symbol name of the access struct, for example:
 - \b Driver_USBH0_HCI is the name of the access struct of the first instance (no. 0).
 - \b Driver_USBH1_HCI is the name of the access struct of the second instance (no. 1).


A configuration setting in the middleware allows connecting the middleware to a specific driver instance <b>Driver_USBH<i>n</i>_HCI</b>.
The default is \token{0}, which connects a middleware to the first instance of a driver.

\note    The struct must remain unchanged.
*/


/**
\struct ARM_USBH_HCI_CAPABILITIES
\details
A USB Host HCI (OHCI/EHCI) driver can be implemented with different capabilities.
The data fields of this structure encode the capabilities implemented by this driver.

<b>Returned by:</b>
  - \ref ARM_USBH_HCI_GetCapabilities

\note    The struct must remain unchanged.
*****************************************************************************************************************/

/**
\typedef    ARM_USBH_HCI_Interrupt_t
\details
Provides the typedef for the interrupt handler \ref ARM_USBH_HCI_Interrupt.

<b>Parameter for:</b>
  - \ref ARM_USBH_HCI_Initialize
*******************************************************************************************************************/


//
// Functions
//

ARM_DRIVER_VERSION ARM_USBH_HCI_GetVersion (void)  {
  return { 0, 0 };
}
/**
\fn       ARM_DRIVER_VERSION ARM_USBH_HCI_GetVersion (void)
\details
The function \b ARM_USBH_HCI_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION
 - API version is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version is source code version of the actual driver implementation.

Example:
\code
extern ARM_DRIVER_USBH Driver_USBH0_HCI;
ARM_DRIVER_USBH *drv_info;
 
void setup_usbh (void)  {
  ARM_DRIVER_VERSION  version;
 
  drv_info = &Driver_USBH0_HCI;  
  version = drv_info->GetVersion ();
  if (version.api < 0x10A)   {      // requires at minimum API version 1.10 or higher
    // error handling
    return;
  }
}
\endcode
*****************************************************************************************************************/

ARM_USBH_HCI_CAPABILITIES ARM_USBH_HCI_GetCapabilities (void)  {
  return { 0 };
}
/**
\fn       ARM_USBH_HCI_CAPABILITIES ARM_USBH_HCI_GetCapabilities (void)
\details
The function \b ARM_USBH_HCI_GetCapabilities returns information about capabilities in this driver implementation.
The data fields of the structure \ref ARM_USBH_HCI_CAPABILITIES encode various capabilities, for example
available HUB ports.
 
Example:
\code
extern ARM_DRIVER_USBH_HCI Driver_USBH0_HCI;
ARM_DRIVER_USBH_HCI *drv_info;
  
void read_capabilities (void)  {
  ARM_USBH_HCI_CAPABILITIES drv_capabilities;
 
  drv_info = &Driver_USBH0_HCI;  
  drv_capabilities = drv_info->GetCapabilities ();
  // interrogate capabilities
 
}
\endcode
*****************************************************************************************************************/

int32_t ARM_USBH_HCI_Initialize (ARM_USBH_HCI_Interrupt_t *cb_interrupt)  {
  return ARM_DRIVER_OK;
}
/**
\fn       int32_t ARM_USBH_HCI_Initialize (ARM_USBH_HCI_Interrupt_t *cb_interrupt)
\details
The function \b ARM_USBH_HCI_Initialize initializes the USB Host HCI (OHCI/EHCI) interface. 
It is called when the middleware component starts operation.

The function performs the following operations:
  - Initializes the resources needed for the USBH interface.
  - Registers the \ref ARM_USBH_HCI_Interrupt interrupt handler.

The parameter \em cb_interrupt is a pointer to the interrupt routine of the OHCI/EHCI peripheral
that needs to be registered. This function is called as ECHI Interrupt Service Handler.

\b Example:
 - see \ref usbh_interface_gr - Driver Functions

*****************************************************************************************************************/

int32_t ARM_USBH_HCI_Uninitialize (void) {
  return ARM_DRIVER_OK;
}
/**
\fn       int32_t ARM_USBH_HCI_Uninitialize (void)
\details
The function \ref ARM_USBH_HCI_Uninitialize de-initializes the resources of USB Host HCI (OHCI/EHCI) interface.

It is called when the middleware component stops operation and releases the software resources used by the interface.
*****************************************************************************************************************/

int32_t ARM_USBH_HCI_PowerControl (ARM_POWER_STATE state)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBH_HCI_PowerControl (ARM_POWER_STATE state)
\details
The function \b ARM_USBH_HCI_PowerControl operates the power modes of the USB Host HCI (OHCI/EHCI) interface.  

The parameter \em state sets the operation and can have the following values:
  - \ref ARM_POWER_FULL : set-up peripheral for data transfers, enable interrupts (NVIC) and optionally DMA. 
                          Can be called multiple times. If the peripheral is already in this mode the function performs 
						  no operation and returns with \ref ARM_DRIVER_OK.
  - \ref ARM_POWER_LOW : may use power saving. Returns \ref ARM_DRIVER_ERROR_UNSUPPORTED when not implemented.
  - \ref ARM_POWER_OFF : terminates any pending data transfers, disables peripheral, disables related interrupts and DMA.
      
Refer to \ref CallSequence for more information.
*****************************************************************************************************************/

int32_t ARM_USBH_HCI_PortVbusOnOff (uint8_t port, bool vbus)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBH_HCI_PortVbusOnOff (uint8_t port, bool vbus)
\details
The function \b  ARM_USBH_HCI_PortVbusOnOff controls the VBUS signal of the specified port.
Most HCI complained USB Host controllers do not require this optional function.
It is only required when a external VBUS interface (for example via I/O pin) is required.

*****************************************************************************************************************/

void ARM_USBH_HCI_Interrupt (void)  {
  // function body
}
/**
\fn void ARM_USBH_HCI_Interrupt (void)
\details
The function \b ARM_USBH_HCI_Interrupt is called from the USBH HCI Interrupt Handler.
*****************************************************************************************************************/

/**
@}
*/ 
// End USBH Interface
