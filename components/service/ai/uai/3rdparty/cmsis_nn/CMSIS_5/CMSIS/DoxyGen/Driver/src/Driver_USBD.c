/**
\defgroup   usbd_interface_gr USB Device Interface
\ingroup    usb_interface_gr
\brief      Driver API for USB Device Peripheral (%Driver_USBD.h)
@{
*/

/** 
\struct     ARM_DRIVER_USBD
\details
The functions of the USB Device driver are accessed by function pointers. Refer to \ref DriverFunctions for 
overview information.

Each instance of an USBD provides such an access struct. The instance is indicated by
a postfix in the symbol name of the access struct, for example:
 - \b Driver_USBD0 is the name of the access struct of the first instance (no. 0).
 - \b Driver_USBD1 is the name of the access struct of the second instance (no. 1).


A configuration setting in the middleware allows connecting the middleware to a specific driver instance <b>Driver_USBD<i>n</i></b>.
The default is \token{0}, which connects a middleware to the first instance of a driver.

\note    The struct must remain unchanged.
*****************************************************************************************************************/

/**
\struct     ARM_USBD_CAPABILITIES 
\details
A USB Device driver can be implemented with different capabilities.
The data fields of this structure encode the capabilities implemented by this driver.

<b>Returned by:</b>
  - \ref ARM_USBD_GetCapabilities

\note    The struct must remain unchanged.
*****************************************************************************************************************/

/**
\struct     ARM_USBD_STATE 
\details
This structure stores information about the state of the USB Device. The data fields encode the established speed,
whether the device is powered and active.

<b>Returned by:</b>
  - \ref ARM_USBD_DeviceGetState
*****************************************************************************************************************/

/**
\typedef    ARM_USBD_SignalDeviceEvent_t
\details
Provides the typedef for the callback function \ref ARM_USBD_SignalDeviceEvent.

<b>Parameter for:</b>
  - \ref ARM_USBD_Initialize
*******************************************************************************************************************/

/**
\typedef    ARM_USBD_SignalEndpointEvent_t
\details
Provides the typedef for the callback function \ref ARM_USBD_SignalEndpointEvent.

<b>Parameter for:</b>
  - \ref ARM_USBD_Initialize
*******************************************************************************************************************/

/**
\defgroup USBD_dev_events USBD Device Events
\ingroup usbd_interface_gr
\brief The USB Device driver generates Device call back events that are notified via the function \ref ARM_USBD_SignalDeviceEvent.
\details 
This section provides the event values for the \ref ARM_USBD_SignalDeviceEvent callback function.

The following call back notification events are generated:
@{
\def  ARM_USBD_EVENT_VBUS_ON
\def  ARM_USBD_EVENT_VBUS_OFF
\def  ARM_USBD_EVENT_RESET
\def  ARM_USBD_EVENT_HIGH_SPEED
\def  ARM_USBD_EVENT_SUSPEND
\def  ARM_USBD_EVENT_RESUME
@}
*/

/**
\defgroup USBD_ep_events USBD Endpoint Events
\ingroup usbd_interface_gr
\brief The USB Device driver generates Endpoint call back events that are notified via the function \ref ARM_USBD_SignalEndpointEvent.
\details 
This section provides the event values for the \ref ARM_USBD_SignalEndpointEvent callback function.

The following call back notification events are generated:
@{
\def  ARM_USBD_EVENT_SETUP
\def  ARM_USBD_EVENT_OUT
\def  ARM_USBD_EVENT_IN
@}
*/


//
// Functions
//

ARM_DRIVER_VERSION ARM_USBD_GetVersion (void)  {
  return { 0, 0 };
}
/**
\fn     ARM_DRIVER_VERSION ARM_USBD_GetVersion (void)
\details
The function \b ARM_USBD_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION
 - API version is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version is source code version of the actual driver implementation.

Example:
\code
extern ARM_DRIVER_USBD Driver_USBD0;
ARM_DRIVER_USBD *drv_info;
 
void setup_usbd (void)  {
  ARM_DRIVER_VERSION  version;
 
  drv_info = &Driver_USBD0;  
  version = drv_info->GetVersion ();
  if (version.api < 0x10A)   {      // requires at minimum API version 1.10 or higher
    // error handling
    return;
  }
}
\endcode
*****************************************************************************************************************/

ARM_USBD_CAPABILITIES ARM_USBD_GetCapabilities (void)  {
  return { 0 };
}
/**
\fn       ARM_USBD_CAPABILITIES ARM_USBD_GetCapabilities (void)
\details
The function \b ARM_USBD_GetCapabilities returns information about capabilities in this driver implementation.
The data fields of the structure \ref ARM_USBD_CAPABILITIES encode various capabilities, for example
if the hardware can create signal events using the \ref ARM_USBD_SignalDeviceEvent callback function.
 
Example:
\code
extern ARM_DRIVER_USBD Driver_USBD0;
ARM_DRIVER_USBD *drv_info;
  
void read_capabilities (void)  {
  ARM_USBD_CAPABILITIES drv_capabilities;
 
  drv_info = &Driver_USBD0;  
  drv_capabilities = drv_info->GetCapabilities ();
  // interrogate capabilities
 
}
\endcode
*****************************************************************************************************************/

int32_t ARM_USBD_Initialize (ARM_USBD_SignalDeviceEvent_t   cb_device_event,
                             ARM_USBD_SignalEndpointEvent_t cb_endpoint_event)  {
  return ARM_DRIVER_OK;
}
/**
\fn       int32_t ARM_USBD_Initialize (ARM_USBD_SignalDeviceEvent_t cb_device_event, ARM_USBD_SignalEndpointEvent_t cb_endpoint_event)
\details
The function \b ARM_USBD_Initialize initializes the USB Device interface. 
It is called when the middleware component starts operation.

The function performs the following operations:
  - Initializes the resources needed for the USBD interface.
  - Registers the \ref ARM_USBD_SignalDeviceEvent callback function.
  - Registers the \ref ARM_USBD_SignalEndpointEvent callback function.

The parameter \em cb_device_event is a pointer to the \ref ARM_USBD_SignalDeviceEvent callback function; use a NULL pointer 
when no device callback signals are required. \n
The parameter \em cb_endpoint_event is a pointer to the \ref ARM_USBD_SignalEndpointEvent callback function.

\b Example:
 - see \ref usbd_interface_gr - Driver Functions

*****************************************************************************************************************/

int32_t ARM_USBD_Uninitialize (void)  {
  return ARM_DRIVER_OK;
}
/**
\fn       int32_t ARM_USBD_Uninitialize (void)
\details
The function \b ARM_USBD_Uninitialize de-initializes the resources of USBD interface.

It is called when the middleware component stops operation and releases the software resources used by the interface.
*****************************************************************************************************************/

int32_t ARM_USBD_PowerControl (ARM_POWER_STATE state)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBD_PowerControl (ARM_POWER_STATE state)
\details
The function \b ARM_USBD_PowerControl operates the power modes of the USB Device interface.  

The parameter \em state sets the operation and can have the following values:
  - \ref ARM_POWER_FULL : set-up peripheral for data transfers, enable interrupts (NVIC) and optionally DMA. 
                          Can be called multiple times. If the peripheral is already in this mode the function performs 
						  no operation and returns with \ref ARM_DRIVER_OK.
  - \ref ARM_POWER_LOW : may use power saving. Returns \ref ARM_DRIVER_ERROR_UNSUPPORTED when not implemented.
  - \ref ARM_POWER_OFF : terminates any pending data transfers, disables peripheral, disables related interrupts and DMA.
      
Refer to \ref CallSequence for more information.
*****************************************************************************************************************/

int32_t ARM_USBD_DeviceConnect (void)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBD_DeviceConnect (void)
\details
The function \b ARM_USBD_DeviceConnect signals to the host that the device is connected.
*****************************************************************************************************************/

int32_t ARM_USBD_DeviceDisconnect (void)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBD_DeviceDisconnect (void)
\details
The function \b ARM_USBD_DeviceDisconnect signals to the host that the device is disconnected.
*****************************************************************************************************************/

ARM_USBD_STATE ARM_USBD_DeviceGetState (void)  {
  return ARM_DRIVER_OK;
}
/**
\fn ARM_USBD_STATE ARM_USBD_DeviceGetState (void)
\details
Retrieves the current USB device state.
*****************************************************************************************************************/

int32_t ARM_USBD_DeviceRemoteWakeup (void)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBD_DeviceRemoteWakeup (void)
\details
The function \b ARM_USBD_DeviceRemoteWakeup signals remote wakeup to the host.
*****************************************************************************************************************/

int32_t ARM_USBD_DeviceSetAddress (uint8_t dev_addr)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBD_DeviceSetAddress (uint8_t dev_addr)
\details
Assigns an address to the device.
*****************************************************************************************************************/

int32_t ARM_USBD_ReadSetupPacket (uint8_t *setup)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBD_ReadSetupPacket (uint8_t *setup)
\details
The function \b ARM_USBD_ReadSetupPacket reads the last SETUP packet (8 bytes) that was received over Control Endpoint (Endpoint 0)
which is indicated by \ref ARM_USBD_EVENT_SETUP event.

<b>See also:</b>
 - \ref ARM_USBD_SignalEndpointEvent
*****************************************************************************************************************/

int32_t ARM_USBD_EndpointConfigure (uint8_t  ep_addr,
                                    uint8_t  ep_type,
                                    uint16_t ep_max_packet_size)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBD_EndpointConfigure (uint8_t ep_addr, uint8_t ep_type, uint16_t ep_max_packet_size)
\details       
The function \b ARM_USBD_EndpointConfigure configures an endpoint for transfers.


*****************************************************************************************************************/

int32_t ARM_USBD_EndpointUnconfigure (uint8_t ep_addr)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBD_EndpointUnconfigure (uint8_t ep_addr)
\details
The function \b ARM_USBD_EndpointUnconfigure de-configures the specified endpoint.

The parameter \em ep_addr specifies the endpoint address. 
*****************************************************************************************************************/

int32_t ARM_USBD_EndpointStall (uint8_t ep_addr, bool stall)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBD_EndpointStall (uint8_t ep_addr, bool stall)
\details
The function \b ARM_USBD_EndpointStall sets or clears stall condition for the specified endpoint.

The parameter \em ep_addr specifies the endpoint address. \n
The parameter \em stall is a boolean parameter.
*****************************************************************************************************************/

int32_t ARM_USBD_EndpointTransfer (uint8_t ep_addr, uint8_t *data, uint32_t num)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBD_EndpointTransfer (uint8_t ep_addr, uint8_t *data, uint32_t num)
\details

The function \b ARM_USBD_EndpointTransfer reads from or writes data to an USB Endpoint.

The parameter \em ep_addr specifies the endpoint address. \n
The parameter \em data is a buffer for data to read or data to write. \n
The parameter \em num is the number of bytes to transfer (must be multiple of endpoint maximum packet size for Read transfers).

The function is non-blocking and returns as soon as the driver starts the operation on the specified endpoint. 
During the operation it is not allowed to call this function again on the same endpoint. 
Also the data buffer must stay allocated and the contents of data must not be modified.

Direction in the endpoint address specifies the type of transfer:
- Endpoint Read for OUT endpoint (direction = 0)
- Endpoint Write for IN endpoint (direction = 1)

Endpoint Read is finished when the requested number of data bytes have been received or when a short packet or ZLP (Zero-Length Packet) has been received.
Completion of operation is indicated by \ref ARM_USBD_EVENT_OUT event. Number of successfully received data bytes can be retrieved 
by calling \ref ARM_USBD_EndpointTransferGetResult.

Endpoint Write is finished when the requested number of data bytes have been sent.
Completion of operation is indicated by \ref ARM_USBD_EVENT_IN event. Number of successfully sent data bytes can be retrieved 
by calling \ref ARM_USBD_EndpointTransferGetResult.

Transfer operation can be aborted by calling \ref ARM_USBD_EndpointTransferAbort.
*****************************************************************************************************************/

uint32_t ARM_USBD_EndpointTransferGetResult (uint8_t ep_addr)  {
  return 0;
}
/**
\fn uint32_t ARM_USBD_EndpointTransferGetResult (uint8_t ep_addr)
\details
The function \b ARM_USBD_EndpointTransferGetResult returns the number of successfully transferred data bytes started by \ref ARM_USBD_EndpointTransfer.

The parameter \em ep_addr specifies the endpoint address.
*****************************************************************************************************************/

int32_t ARM_USBD_EndpointTransferAbort (uint8_t ep_addr)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_USBD_EndpointTransferAbort (uint8_t ep_addr)
\details
The function \b ARM_USBD_EndpointTransferAbort aborts the transfer to an endpoint started by \ref ARM_USBD_EndpointTransfer.

The parameter \em ep_addr specifies the endpoint address.
*****************************************************************************************************************/

uint16_t ARM_USBD_GetFrameNumber (void)  {
  return 0;
}
/**
\fn uint16_t ARM_USBD_GetFrameNumber (void)
\details
Retrieves the sequential 11-bit frame number of the last Start of Frame (SOF) packet.
*****************************************************************************************************************/

void ARM_USBD_SignalDeviceEvent (uint32_t event)  {
  // function body
}
/**
\fn void ARM_USBD_SignalDeviceEvent (uint32_t event)
\details
The function \b ARM_USBD_SignalDeviceEvent is a callback function registered by the function \ref ARM_USBD_Initialize. 

The parameter \em event indicates one or more events that occurred during driver operation.
Each event is encoded in a separate bit and therefore it is possible to signal multiple events within the same call. 

Not every event is necessarily generated by the driver. This depends on the implemented capabilities stored in the 
data fields of the structure \ref ARM_USBD_CAPABILITIES, which can be retrieved with the function \ref ARM_USBD_GetCapabilities.

The following events can be generated:

Event                           | Bit| Description                                        | supported when \ref ARM_USBD_CAPABILITIES
:-------------------------------|---:|:---------------------------------------------------|----------------------------------------------
\ref ARM_USBD_EVENT_VBUS_ON     | 0  | Occurs when valid VBUS voltage is detected.        | data field \em  event_vbus_on = \token{1}
\ref ARM_USBD_EVENT_VBUS_OFF    | 1  | Occurs when VBUS voltage is turned off.            | data field \em  event_vbus_off = \token{1}
\ref ARM_USBD_EVENT_RESET       | 2  | Occurs when USB Reset is detected.                 | <i>always supported</i>
\ref ARM_USBD_EVENT_HIGH_SPEED  | 3  | Occurs when USB Device is switched to High-speed.  | <i>always supported</i>
\ref ARM_USBD_EVENT_SUSPEND     | 4  | Occurs when USB Suspend is detected.               | <i>always supported</i>
\ref ARM_USBD_EVENT_RESUME      | 5  | Occurs when USB Resume is detected.                | <i>always supported</i>
*****************************************************************************************************************/

void ARM_USBD_SignalEndpointEvent (uint8_t ep_addr, uint32_t ep_event)  {
  // function body
}
/**
\fn void ARM_USBD_SignalEndpointEvent (uint8_t ep_addr, uint32_t event)
\details
The function \b ARM_USBD_SignalEndpointEvent is a callback function registered by the function \ref ARM_USBD_Initialize. 

The argument \a ep_addr specifies the endpoint. \n
The parameter \em event indicates one or more events that occurred during driver operation.
Each event is encoded in a separate bit and therefore it is possible to signal multiple events within the same call. 

The following events can be generated:

Event                                    | Bit | Description 
:----------------------------------------|----:|:-----------
\ref ARM_USBD_EVENT_SETUP                |  0  | Occurs when SETUP packet is received over Control Endpoint.
\ref ARM_USBD_EVENT_OUT                  |  1  | Occurs when data is received over OUT Endpoint.
\ref ARM_USBD_EVENT_IN                   |  2  | Occurs when data is sent over IN Endpoint.
*****************************************************************************************************************/

/**
@}
*/ 
// End USBD Interface
