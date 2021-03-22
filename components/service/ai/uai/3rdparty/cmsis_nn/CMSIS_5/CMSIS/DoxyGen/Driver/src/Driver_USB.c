/**
\defgroup usb_interface_gr USB Interface
\brief   USB common definitions (%Driver_USB.h)
\details
The <b>Universal Serial Bus</b> (USB) implements a serial bus for data exchange. It is a host controlled, plug-and-play interface
between a USB host and USB devices using a tiered star topology. 
In microcontroller (MCU) applications, the interface is often used to connect a device to a host for data exchange or control purposes.

 - Wikipedia offers more information about the <a href="http://en.wikipedia.org/wiki/Universal_Serial_Bus" target="_blank"><b>Universal Serial Bus</b></a>.
 - The USB Implementers Forum provides detailed documentation under <a href="http://www.usb.org"><b>www.usb.org</b></a>.


<b>Block Diagram</b>

Typically only one USB Device is connected to a USB Host. If several USB devices must be connected to the same USB host, then
the connection must be done via a USB hub.

<p>
\image html USB_Schematics.png "Simplified USB Schema"
</p>


<b>USB API</b>

The following header files define the Application Programming Interface (API) for the USB interface:
  - \b %Driver_USB.h :  Common definitions of the USBD and USBH interface
  - \b %Driver_USBD.h : Driver API for USB Device Peripheral
  - \b %Driver_USBH.h : Driver API for USB Host Peripheral

The driver implementation is a typical part of the Device Family Pack (DFP) that supports the 
peripherals of the microcontroller family.


<b>Driver Functions</b>

The driver functions are published in the access struct as explained in \ref DriverFunctions
  - \ref ARM_DRIVER_USBD : access struct for USBD driver functions
  - \ref ARM_DRIVER_USBH : access struct for USBH driver functions

<hr>
*/


/**
\addtogroup usbd_interface_gr 
\ingroup usb_interface_gr
\details

<b>USB Device API</b>

The header file \b Driver_USBD.h defines the API for the <b>USB Device Driver</b> interface used by middleware components.
The driver implementation itself is a typical part of the Device Family Pack, which provides entry points to the interface 
as function pointers in the struct \ref ARM_DRIVER_USBD. This structure can be available several times in each interface to control multiple USBD interfaces.

Header file \b Driver_USBD.h also defines callback routines that can be categorized as 
<b>device event callbacks</b> and <b>endpoint event callbacks</b>.
Callbacks are called by the driver, in interrupt context when an appropriate event occurs, to signal device related events (\ref USBD_dev_events)
and endpoint related events (\ref USBD_ep_events).


<b>USB Device Function Call Sequence</b>

To use the USBD driver invoke the API functions in the following order:

\msc
 a [label="", textcolor="indigo", linecolor="indigo", arclinecolor="red"],
 b [label="", textcolor="blue", linecolor="blue", arclinecolor="blue"];

 a rbox a [label="Middleware", linecolor="indigo"],
 b rbox b [label="USBD Driver", linecolor="blue"];
 a=>b [label="ARM_USBD_Initialize", URL="\ref ARM_USBD_Initialize"];
 a=>b [label="ARM_USBD_PowerControl (ARM_POWER_FULL)", URL="\ref ARM_USBD_Initialize"];
 a=>b [label="ARM_USBD_DeviceConnect", URL="\ref ARM_USBD_DeviceConnect"];
 a<=b [label="ARM_USBD_SignalDeviceEvent (ARM_USBD_EVENT_RESET)", URL="\ref ARM_USBD_SignalDeviceEvent", linecolor="orange"];
 a=>b [label="ARM_USBD_DeviceGetState", URL="\ref ARM_USBD_DeviceGetState"];
 a=>b [label="ARM_USBD_EndpointConfigure", URL="\ref ARM_USBD_EndpointConfigure", linecolor="green"];
 --- [label="Repeat and use as needed"];
 a=>b [label="ARM_USBD_EndpointTransfer", URL="\ref ARM_USBD_EndpointTransfer", linecolor="green"];
 a<=b [label="ARM_USBD_SignalEndpointEvent", URL="\ref ARM_USBD_SignalEndpointEvent", linecolor="orange"];
 a=>b [label="ARM_USBD_EndpointTransferGetResult", URL="\ref ARM_USBD_EndpointTransferGetResult", linecolor="green"];
 --- [label="Repeat End"];
 a=>b [label="ARM_USBD_DeviceDisconnect", URL="\ref ARM_USBD_DeviceDisconnect"];
 a=>b [label="ARM_USBD_PowerControl (ARM_POWER_OFF)", URL="\ref ARM_USBD_Initialize"];
 a=>b [label="ARM_USBD_Uninitialize", URL="\ref ARM_USBD_Uninitialize"];
\endmsc

*/

/**
\addtogroup usbh_interface_gr
\ingroup usb_interface_gr
\details
<b>USB Host API</b>

The header file \b Driver_USBH.h defines the API for the <b>USB Host Driver</b> interface used by middleware components.
The driver implementation itself is a typical part of the Device Family Pack, which provides entry points to the interface 
as function pointers in the struct \ref ARM_DRIVER_USBH. This structure can be available several times in each interface to control multiple USBH interfaces.

\b Driver_USBH.h also defines callback routines, which are categorized in 
<b>port event callbacks</b> and <b>pipe event callbacks</b>.
Callbacks are called by the driver, in interrupt context when an appropriate event occurs, to signal port related events (\ref ARM_USBH_SignalPortEvent)
and pipe related events (\ref ARM_USBH_SignalPipeEvent). 

\cond

<b>USB Host Function Call Sequence</b>

To use the USBH driver invoke the API functions in the following order:

\msc
 a [label="", textcolor="indigo", linecolor="indigo", arclinecolor="red"],
 b [label="", textcolor="blue", linecolor="blue", arclinecolor="blue"];

 a rbox a [label="Middleware", linecolor="indigo"],
 b rbox b [label="USBH Driver", linecolor="blue"];
 a=>b [label="ARM_USBH_Initialize", URL="\ref ARM_USBD_Initialize"];
 --- [label="Repeat and use as needed"];
 --- [label="Repeat End"];
 a=>b [label="ARM_USBH_Uninitialize", URL="\ref ARM_USBH_Uninitialize"];
\endmsc

 <hr>

\endcond
*/


/**
\defgroup USB_speed USB Speed
\ingroup usb_interface_gr
\brief USB Speed definitions
\details
The following USB speed values are defined:
@{
\def  ARM_USB_SPEED_LOW
\def  ARM_USB_SPEED_FULL
\def  ARM_USB_SPEED_HIGH
@}
*/

/**
\defgroup USB_endpoint_type USB Endpoint Type
\ingroup usb_interface_gr
\brief USB Endpoint Type definitions
\details
The following USB Endpoint Type values are defined:
@{
\def  ARM_USB_ENDPOINT_CONTROL
\def  ARM_USB_ENDPOINT_ISOCHRONOUS
\def  ARM_USB_ENDPOINT_BULK
\def  ARM_USB_ENDPOINT_INTERRUPT
@}
*/
