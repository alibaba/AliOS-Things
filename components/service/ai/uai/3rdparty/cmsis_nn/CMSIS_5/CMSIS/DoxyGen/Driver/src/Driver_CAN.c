/**
\defgroup can_interface_gr CAN Interface
\brief Driver API for CAN Bus Peripheral (%Driver_CAN.h)
\details

The <b>Controller Area Network</b> Interface Bus (CAN) implements a multi-master serial bus for connecting
microcontrollers and devices, also known as nodes, to communicate with each other in applications without a host computer.
CAN is a message-based protocol, designed originally for automotive applications, but meanwhile used also in many other surroundings.
The complexity of the node can range from a simple I/O device up to an embedded computer with a CAN interface and sophisticated software.
The node may also be a gateway allowing a standard computer to communicate over a USB or Ethernet port to the devices on a CAN network.
Devices are connected to the bus through a host processor, a CAN controller, and a CAN transceiver.


The CAN Driver API allows to implement CAN Interfaces that conform to the  
<a href="http://www.bosch-semiconductors.com/" target="_blank">
CAN specifications available from BOSCH</a>:
  - CAN 2.0B: CAN Specification 2.0B (released Sep. 1991) which is now superseded by ISO 11898-1.
  - CAN FD: CAN with Flexible Data Rate introduced in 2012 (released April 17th, 2012).

Wikipedia offers more information about the <a href="http://en.wikipedia.org/wiki/CAN_bus" target="_blank"><b>CAN Bus</b></a>.

**CAN 2.0B**
Every CAN CMSIS-Driver supports the CAN 2.0B standard

CAN 2.0B supports:
  - message can contain up to 8 data bytes
  - bitrates of up to 1Mbits/s
  - \ref Remote_Frame requests
  
\anchor CAN_FD
<b>CAN FD</b>

Support for CAN FD depends on the hardware.  
A CMSIS-Driver that supports CAN FD has the capability \ref ARM_CAN_CAPABILITIES data field \b fd_mode = \token{1}, which can be
retrieved with the function \ref ARM_CAN_GetCapabilities. 

CAN FD supports:
   - message can contain up to 64 data bytes
   - faster data transfers with faster bitrate used during the data phase 

CAN FD does not support \ref Remote_Frame requests.

<b>Block Diagram</b>

The CAN Driver API defines a <b>CAN</b> interface for middleware components. The CAN Driver supports multiple
nodes, which are able to send and receive messages, but not simultaneously.

\image html CAN_Node.png  "CAN Node Schematic"

CAN API
-------

The following header files define the Application Programming Interface (API) for the CAN interface:
  - \b %Driver_CAN.h : Driver API for CAN Bus Peripheral

The driver implementation is a typical part of the Device Family Pack (DFP) that supports the
peripherals of the microcontroller family.


<b>Driver Functions</b>

The driver functions are published in the access struct as explained in \ref DriverFunctions
  - \ref ARM_DRIVER_CAN : access struct for CAN driver functions

<b>Example Code</b>

The following example code shows the usage of the CAN interface.

\code

#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
 
#include "Driver_CAN.h"
 
// CAN Driver Controller selector
#define  CAN_CONTROLLER         1       // CAN Controller number
 
#define _CAN_Driver_(n)         Driver_CAN##n
#define  CAN_Driver_(n)        _CAN_Driver_(n)
extern   ARM_DRIVER_CAN         CAN_Driver_(CAN_CONTROLLER);
#define  ptrCAN               (&CAN_Driver_(CAN_CONTROLLER))
 
uint32_t                        rx_obj_idx  = 0xFFFFFFFFU;
uint8_t                         rx_data[8];
ARM_CAN_MSG_INFO                rx_msg_info;
uint32_t                        tx_obj_idx  = 0xFFFFFFFFU;
uint8_t                         tx_data[8];
ARM_CAN_MSG_INFO                tx_msg_info;
 
static void Error_Handler (void) { while (1); }
 
void CAN_SignalUnitEvent (uint32_t event) {}
 
void CAN_SignalObjectEvent (uint32_t obj_idx, uint32_t event) {
 
  if (obj_idx == rx_obj_idx) {                  // If receive object event
    if (event == ARM_CAN_EVENT_RECEIVE) {       // If message was received successfully
      if (ptrCAN->MessageRead(rx_obj_idx, &rx_msg_info, rx_data, 8U) > 0U) {
                                                // Read received message
        // process received message ...
      }
    }
  }
  if (obj_idx == tx_obj_idx) {                  // If transmit object event
    if (event == ARM_CAN_EVENT_SEND_COMPLETE) { // If message was sent successfully
      // acknowledge sent message ...
    }
  }
}
 
int main (void) {
  ARM_CAN_CAPABILITIES     can_cap;
  ARM_CAN_OBJ_CAPABILITIES can_obj_cap;
  int32_t                  status;
  uint32_t                 i, num_objects;
 
  can_cap = ptrCAN->GetCapabilities (); // Get CAN driver capabilities
  num_objects = can_cap.num_objects;    // Number of receive/transmit objects
 
  status = ptrCAN->Initialize    (CAN_SignalUnitEvent, CAN_SignalObjectEvent);  // Initialize CAN driver
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
 
  status = ptrCAN->PowerControl  (ARM_POWER_FULL);                              // Power-up CAN controller
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
 
  status = ptrCAN->SetMode       (ARM_CAN_MODE_INITIALIZATION);                 // Activate initialization mode
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
 
  status = ptrCAN->SetBitrate    (ARM_CAN_BITRATE_NOMINAL,              // Set nominal bitrate
                                  100000U,                              // Set bitrate to 100 kbit/s
                                  ARM_CAN_BIT_PROP_SEG(5U)   |          // Set propagation segment to 5 time quanta
                                  ARM_CAN_BIT_PHASE_SEG1(1U) |          // Set phase segment 1 to 1 time quantum (sample point at 87.5% of bit time)
                                  ARM_CAN_BIT_PHASE_SEG2(1U) |          // Set phase segment 2 to 1 time quantum (total bit is 8 time quanta long)
                                  ARM_CAN_BIT_SJW(1U));                 // Resynchronization jump width is same as phase segment 2
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
 
  for (i = 0U; i < num_objects; i++) {                                          // Find first available object for receive and transmit
    can_obj_cap = ptrCAN->ObjectGetCapabilities (i);                            // Get object capabilities
    if      ((rx_obj_idx == 0xFFFFFFFFU) && (can_obj_cap.rx == 1U)) { rx_obj_idx = i; }
    else if ((tx_obj_idx == 0xFFFFFFFFU) && (can_obj_cap.tx == 1U)) { tx_obj_idx = i; break; }
  }
  if ((rx_obj_idx == 0xFFFFFFFFU) || (tx_obj_idx == 0xFFFFFFFFU)) { Error_Handler(); }
 
  // Set filter to receive messages with extended ID 0x12345678 to receive object
  status = ptrCAN->ObjectSetFilter(rx_obj_idx, ARM_CAN_FILTER_ID_EXACT_ADD, ARM_CAN_EXTENDED_ID(0x12345678U), 0U);
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
 
  status = ptrCAN->ObjectConfigure(tx_obj_idx, ARM_CAN_OBJ_TX);                 // Configure transmit object
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
 
  status = ptrCAN->ObjectConfigure(rx_obj_idx, ARM_CAN_OBJ_RX);                 // Configure receive object
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
  
  status = ptrCAN->SetMode (ARM_CAN_MODE_NORMAL);                               // Activate normal operation mode
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
  
  memset(&tx_msg_info, 0U, sizeof(ARM_CAN_MSG_INFO));                           // Clear message info structure
  tx_msg_info.id = ARM_CAN_EXTENDED_ID(0x12345678U);                            // Set extended ID for transmit message
  tx_data[0]     = 0xFFU;                                                       // Initialize transmit data
  while (1) {
    tx_data[0]++;                                                               // Increment transmit data
    status = ptrCAN->MessageSend(tx_obj_idx, &tx_msg_info, tx_data, 1U);        // Send data message with 1 data byte
    if (status != 1U) { Error_Handler(); }
    for (i = 0U; i < 1000000U; i++) { __nop(); }                                // Wait a little while
  }
}
\endcode


\section can_objects CAN Message Objects

The CMSIS-Driver for the CAN interface provides multiple CAN message objects, which can be seen as individual communication channels.
The number of available CAN message objects depends on the CAN peripheral. The function \ref ARM_CAN_GetCapabilities returns 
the maximum number of available CAN message objects. The number is encoded in the structure \ref ARM_CAN_CAPABILITIES in the data field \em num_objects.
CAN message objects are addressed with the functions listed below, whereby the parameter \em obj_idx addresses an individual object.
The valid range for \em obj_idx is \token{[0 .. (\em num_objects - 1)]}.

Function                           | Description
:----------------------------------|:--------------------------------------------
\ref ARM_CAN_ObjectGetCapabilities | Retrieves message object capabilities such as receive, transmit, \ref Remote_Frame automatic handling and \ref can_filtering.
\ref ARM_CAN_ObjectSetFilter       | Allows to set-up CAN ID filtering for the message object.
\ref ARM_CAN_ObjectConfigure       | Allows to configure the message object for receive, transmit or \ref Remote_Frame automatic handling.
\ref ARM_CAN_MessageRead           | Read received message from the message object.
\ref ARM_CAN_MessageSend           | Send CAN message or send \ref Remote_Frame or set CAN message to be sent automatically on reception of matching \ref Remote_Frame on the message object.
\ref ARM_CAN_SignalObjectEvent     | Callback function that signals a message transfer or a received message overrun.

Each CAN message object may have different capabilities. Before using a CAN message object, call the 
function \ref ARM_CAN_ObjectGetCapabilities to verify the available features.


\section can_filtering CAN Message Filtering

The CMSIS-Driver for the CAN interface supports ID filtering for the receiving message objects. The receiving CAN node examines the identifier 
to decide if it was relevant. This filtering is done by the CAN peripheral according the settings configured with the function \ref ARM_CAN_ObjectSetFilter.

The function \ref ARM_CAN_ObjectGetCapabilities retrieves the filter capabilities of the CAN message objects stored in \ref ARM_CAN_OBJ_CAPABILITIES.

Data Fields                | CAN Messages Object can be filtered with ...
:--------------------------|:--------------------------------------------
\em exact_filtering        | an exact ID value set by using the function \ref ARM_CAN_ObjectSetFilter with \em control = \ref ARM_CAN_FILTER_ID_EXACT_ADD.
\em range_filtering        | a range ID value set by using the function \ref ARM_CAN_ObjectSetFilter with \em control = \ref ARM_CAN_FILTER_ID_RANGE_ADD.
\em mask_filtering         | a mask ID value set by as using the function \ref ARM_CAN_ObjectSetFilter with \em control = \ref ARM_CAN_FILTER_ID_MASKABLE_ADD.
\em multiple_filters       | ... several filters to capture multiple ID values, or ID value ranges.

<b>CAN message filtering using an exact ID</b>

Example: accept in message object #1 only frames with extended ID = 0x1567.
\code
  status = ptrCAN->ObjectSetFilter (1, ARM_CAN_FILTER_ID_EXACT_ADD, ARM_CAN_EXTENDED_ID(0x1567), 0);
  if (status != ARM_DRIVER_OK) ... // error handling
\endcode

Example: accept in message object #2 frames with extended ID = 0x3167 and extended ID = 0x42123.
\code
  status = ptrCAN->ObjectSetFilter (2, ARM_CAN_FILTER_ID_EXACT_ADD, ARM_CAN_EXTENDED_ID(0x3167), 0);
  if (status != ARM_DRIVER_OK) ... // error handling
  status = ptrCAN->ObjectSetFilter (2, ARM_CAN_FILTER_ID_EXACT_ADD, ARM_CAN_EXTENDED_ID(0x42123), 0);
  if (status != ARM_DRIVER_OK) ... // error handling
\endcode

<b>CAN message filtering using a range ID</b>

Example: accept in message object #3 only frames with extended ID >= 0x1567 and extended ID <= 0x1577.
\code
  status = ptrCAN->ObjectSetFilter (3, ARM_CAN_FILTER_ID_RANGE_ADD, ARM_CAN_EXTENDED_ID(0x1567), ARM_CAN_EXTENDED_ID(0x1577));
  if (status != ARM_DRIVER_OK) ... // error handling
\endcode


<b>CAN message filtering using a mask ID</b>

Using the function \ref ARM_CAN_ObjectSetFilter with \em control = \ref ARM_CAN_FILTER_ID_MASKABLE_ADD allows to specify with \em arg a mask value.
 - if a mask bit is \token{0}, the corresponding \em ID bit will be accepted, regardless of the value.
 - if a mask bit is \token{1}, the corresponding \em ID bit will be compared with the value of the ID filter bit; if they match the message will be accepted otherwise the frame is rejected.

Example: accept in message object #0 only frames with extended IDs 0x1560 to 0x156F.
\code
  status = ptrCAN->ObjectSetFilter (0, ARM_CAN_FILTER_ID_MASKABLE_ADD, ARM_CAN_EXTENDED_ID(0x1560), 0x1FFFFFF0);
  if (status != ARM_DRIVER_OK) ... // error handling
\endcode

Example: accept in message object #2 only frames with extended IDs 0x35603, 0x35613, 0x35623, and 0x35633.
\code
  status = ptrCAN->ObjectSetFilter (2, ARM_CAN_FILTER_ID_MASKABLE_ADD, ARM_CAN_EXTENDED_ID(0x35603), 0x1FFFFFCF);
  if (status != ARM_DRIVER_OK) ... // error handling
\endcode

Example: accept any message in object #4 regardless of the ID.
\code
  status = ptrCAN->ObjectSetFilter (4, ARM_CAN_FILTER_ID_MASKABLE_ADD, ARM_CAN_EXTENDED_ID(0), 0);
  if (status != ARM_DRIVER_OK) ... // error handling
\endcode

\section Remote_Frame Remote Frame

In general, data transmission is performed on an autonomous basis with the data source node sending out Data Frames.

However, sending a <b>Remote Frame</b> allows a destination node to request the data from the source node.
The examples below shows the data exchange using a <b>Remote Transmission Request (RTR)</b>.

<b>Example for automatic Data Message response on RTR</b>

For automatic data message response on an RTR, the object is configured with the function \ref ARM_CAN_ObjectConfigure \em obj_cfg = \ref ARM_CAN_OBJ_RX_RTR_TX_DATA.

In this case, the function \ref ARM_CAN_MessageSend sets a data message that is transmitted when an RTR with a matching CAN ID is received.
If  \ref ARM_CAN_MessageSend was not called before the RTR is received, the response is hardware dependent (either last data message is repeated 
or no data message is sent until \ref ARM_CAN_MessageSend is called).

After data transmission is completed, the driver calls a callback function \ref ARM_CAN_SignalObjectEvent with \em event = \ref ARM_CAN_EVENT_SEND_COMPLETE 
and the related \em obj_idx.

<b>Example:</b>
\code
  status = ptrCAN->ObjectSetFilter(0, ARM_CAN_FILTER_ID_EXACT_ADD, ARM_CAN_EXTENDED_ID(0x12345678U), 0U);
  if (status != ARM_DRIVER_OK) ... // error handling
  status = trCAN->ObjectConfigure(0, ARM_CAN_OBJ_RX_RTR_TX_DATA);
  if (status != ARM_DRIVER_OK) ... // error handling

  memset(&tx_msg_info, 0, sizeof(ARM_CAN_MSG_INFO));            // Clear transmit message structure
  tx_msg_info.id  = ARM_CAN_EXTENDED_ID(0x12345678U);           // Set ID of message
  data_buf[0] = '1';  data_buf[1] = '2';                        // Prepare data to transmit
  data_buf[2] = '3';  data_buf[3] = '4';
  data_buf[4] = '5';  data_buf[5] = '6';
  data_buf[6] = '7';  data_buf[7] = '8';
  ptrCAN->MessageSend(0, &tx_msg_info, data_buf, 8);            // Start send message that will be triggered on RTR reception
\endcode

 
<b>Example for automatic Data Message reception using RTR</b>

For automatic data message reception on an RTR, the object is configured with the function \ref ARM_CAN_ObjectConfigure \em obj_cfg = \ref ARM_CAN_OBJ_TX_RTR_RX_DATA. 

The receiver or consumer requests data with transmission of an RTR with the \ref ARM_CAN_MessageSend. This RTR requests from the transmitter or producer to send the data message.
Once the data message is received, the driver calls a callback function \ref ARM_CAN_SignalObjectEvent with \em event = \ref ARM_CAN_EVENT_RECEIVE
and the related \em obj_idx. The received data message can then be read with the function \ref ARM_CAN_MessageRead. 

<b>Example:</b>
\code
  status = ptrCAN->ObjectSetFilter(0, ARM_CAN_FILTER_ID_EXACT_ADD, ARM_CAN_EXTENDED_ID(0x12345678U), 0U);
  if (status != ARM_DRIVER_OK) ... // error handling
  status = ptrCAN->ObjectConfigure(0, ARM_CAN_OBJ_TX_RTR_RX_DATA);
  if (status != ARM_DRIVER_OK) ... // error handling
  memset(&tx_msg_info, 0, sizeof(ARM_CAN_MSG_INFO));            // Clear transmit message structure
  tx_msg_info.id  = ARM_CAN_EXTENDED_ID(0x12345678U);           // Set ID of message
  tx_msg_info.rtr = 1;                                          // Set RTR flag of message to send RTR
  tx_msg_info.dlc = 1;                                          // Set data length code of message to 1 to request 1 data byte
  ptrCAN->MesageSend(0, &tx_msg_info, 0, 0);                    // Send RTR

  // Wait for ARM_CAN_EVENT_RECEIVE
  ptrCAN->MessageRead(0, &rx_msg_info, data_buf, 8);            // Read received message
\endcode


@{
*****************************************************************************************************************/

/**
\struct     ARM_DRIVER_CAN
\details
The functions of the CAN are accessed by function pointers exposed by this structure. Refer to \ref DriverFunctions for overview information.

Each instance of a CAN provides such an access structure.
The instance is identified by a postfix number in the symbol name of the access structure, for example:
 - \b Driver_CAN0 is the name of the access struct of the first instance (no. 0).
 - \b Driver_CAN1 is the name of the access struct of the second instance (no. 1).

A configuration setting in the middleware allows you to connect the middleware to a specific driver instance <b>Driver_CAN<i>n</i></b>.
*******************************************************************************************************************/

/**
\struct     ARM_CAN_CAPABILITIES
\details
A CAN driver can be implemented with different capabilities encoded in the data fields of this structure.

<b>Returned by:</b>
  - \ref ARM_CAN_GetCapabilities

\sa \ref ARM_CAN_OBJ_CAPABILITIES for information about CAN objects.
*******************************************************************************************************************/

/**
\struct     ARM_CAN_STATUS
\details
Structure with information about the status of the CAN unit state and errors.
The data fields encode the unit bus state, last error code, transmitter error count, and receiver error count.

<b>Returned by:</b>
  - \ref ARM_CAN_GetStatus
*****************************************************************************************************************/

/**
\struct     ARM_CAN_MSG_INFO
\brief      CAN Message Information
\details
Structure with information about the CAN message.

In CAN mode, the following \ref ARM_CAN_MSG_INFO data fields are ignored: \em edl, \em brs, \em esi. \n
In CAN FD mode, the following \ref ARM_CAN_MSG_INFO data field is ignored: \em rtr.

<b>Parameter for:</b>
  - \ref ARM_CAN_MessageSend
  - \ref ARM_CAN_MessageRead

\sa \ref can_filtering
\sa \ref Remote_Frame
*****************************************************************************************************************/
 
/**
\typedef    ARM_CAN_SignalUnitEvent_t
\details
Provides the typedef for the callback function \ref ARM_CAN_SignalUnitEvent.

<b>Parameter for:</b>
  - \ref ARM_CAN_Initialize
*******************************************************************************************************************/

/**
\typedef    ARM_CAN_SignalObjectEvent_t
\details
Provides the typedef for the callback function \ref ARM_CAN_SignalObjectEvent.

<b>Parameter for:</b>
  - \ref ARM_CAN_Initialize
*******************************************************************************************************************/

/**
\defgroup can_status_code_ctrls Status Error Codes
\ingroup can_interface_gr
\brief Status codes of the CAN driver.
\details

The following callback notification unit events are generated:
@{
\def ARM_CAN_UNIT_STATE_INACTIVE
\def ARM_CAN_UNIT_STATE_ACTIVE
\def ARM_CAN_UNIT_STATE_PASSIVE
\def ARM_CAN_UNIT_STATE_BUS_OFF
\def ARM_CAN_LEC_NO_ERROR
\def ARM_CAN_LEC_BIT_ERROR
\def ARM_CAN_LEC_STUFF_ERROR
\def ARM_CAN_LEC_CRC_ERROR
\def ARM_CAN_LEC_FORM_ERROR
\def ARM_CAN_LEC_ACK_ERROR
@}
*******************************************************************************************************************/

/**
\defgroup CAN_unit_events CAN Unit Events
\ingroup can_interface_gr
\brief Callback unit events notified via \ref ARM_CAN_SignalUnitEvent.
\details
The CAN driver generates callback unit events that are notified via the function \ref ARM_CAN_SignalUnitEvent.

The following callback notification unit events are generated:
@{
\def ARM_CAN_EVENT_UNIT_INACTIVE
\sa \ref ARM_CAN_SignalUnitEvent
\def ARM_CAN_EVENT_UNIT_ACTIVE
\sa \ref ARM_CAN_SignalUnitEvent
\def ARM_CAN_EVENT_UNIT_WARNING
\sa \ref ARM_CAN_SignalUnitEvent
\def ARM_CAN_EVENT_UNIT_PASSIVE
\sa \ref ARM_CAN_SignalUnitEvent
\def ARM_CAN_EVENT_UNIT_BUS_OFF
\sa \ref ARM_CAN_SignalUnitEvent
@}
*******************************************************************************************************************/

/**
\defgroup CAN_events CAN Object Events
\brief Callback objects events notified via \ref ARM_CAN_SignalObjectEvent.
\details
The CAN driver generates callback objects events that are notified via the function \ref ARM_CAN_SignalObjectEvent.

The following callback notification object events are generated:
@{
\def ARM_CAN_EVENT_SEND_COMPLETE
\sa \ref ARM_CAN_SignalObjectEvent
\def ARM_CAN_EVENT_RECEIVE
\sa \ref ARM_CAN_SignalObjectEvent
\def ARM_CAN_EVENT_RECEIVE_OVERRUN
\sa \ref ARM_CAN_SignalObjectEvent
@}
*******************************************************************************************************************/

/**
\defgroup can_control CAN Control Codes
\ingroup can_interface_gr
\brief   Codes to configure the CAN driver.
\details
@{
The various CAN control codes define:

  - \ref can_identifer_ctrls          specify CAN identifier. Refer to \ref ARM_CAN_ObjectConfigure.
  - \ref can_mode_ctrls               control CAN interface operation. Refer to \ref ARM_CAN_Control.
  - \ref can_timeseg_ctrls            specify CAN bit rate and timing. Refer to \ref ARM_CAN_SetBitrate.
  - \ref can_bus_mode_ctrls           specify CAN bus operating mode. Refer to \ref ARM_CAN_SetMode.
  - \ref can_filter_operation_ctrls   specify CAN filter operations.  Refer to \ref ARM_CAN_ObjectSetFilter.
  - \ref can_obj_config_ctrls         specify CAN object configuration modes. Refer to \ref ARM_CAN_ObjectConfigure.
*****************************************************************************************************************/

/**
\defgroup can_identifer_ctrls CAN Identifier
\brief Set object to standard or extended.
\details

@{
\def ARM_CAN_STANDARD_ID(id)
\sa \ref ARM_CAN_ObjectConfigure
\def ARM_CAN_EXTENDED_ID(id)
\sa \ref ARM_CAN_ObjectConfigure
@}
*******************************************************************************************************************/

/**
\defgroup can_mode_ctrls CAN Operation Codes
\brief Set CAN operation modes.
\details

These controls set the CAN operation using the function \ref ARM_CAN_Control.

@{
\def ARM_CAN_SET_FD_MODE
\sa \ref ARM_CAN_Control
\def ARM_CAN_ABORT_MESSAGE_SEND
\sa \ref ARM_CAN_Control
\def ARM_CAN_ABORT_MESSAGE_SEND
\sa \ref ARM_CAN_Control
\def ARM_CAN_CONTROL_RETRANSMISSION
\sa \ref ARM_CAN_Control
\def ARM_CAN_SET_TRANSCEIVER_DELAY
\sa \ref ARM_CAN_Control

@}
*****************************************************************************************************************/

/**
\defgroup can_bus_mode_ctrls CAN Bus Communication Mode
@{
\brief Set or initialize the CAN bus
\enum ARM_CAN_MODE 
\details
The enumerations below initialize and set the bus communication mode.

<b>Parameter for:</b>
  - \ref ARM_CAN_SetMode
@}
*/

/**
\defgroup can_timeseg_ctrls CAN Bit Timing Codes
@{
\brief Set bit timing
\details
The following codes are used with the function \ref ARM_CAN_SetBitrate.

\def ARM_CAN_BIT_PROP_SEG(x)
\sa \ref ARM_CAN_SetBitrate
\def ARM_CAN_BIT_PHASE_SEG1(x)
\sa \ref ARM_CAN_SetBitrate
\def ARM_CAN_BIT_PHASE_SEG2(x)
\sa \ref ARM_CAN_SetBitrate
\def ARM_CAN_BIT_SJW(x)
\sa \ref ARM_CAN_SetBitrate

*******************************************************************************************************************/

/**
\enum ARM_CAN_BITRATE_SELECT 
\brief Set the bit rate.
\details 
Provides the typedef for setting the bit rate. 

<b>Parameter for:</b>
  - \ref ARM_CAN_SetBitrate
*******************************************************************************************************************/
/**
@}   
*/

/**
\defgroup can_filter_operation_ctrls CAN Filter Operation Codes
@{
\brief Set CAN filter manipulation codes.

\enum ARM_CAN_FILTER_OPERATION
\details

\b ARM_CAN_FILTER_OPERATION provides the controls for setting the filter type.
Refer to \ref can_filtering for details.

<b>Parameter for:</b>
 - \ref ARM_CAN_ObjectSetFilter
@}
*****************************************************************************************************************/

/**
\defgroup can_obj_config_ctrls CAN Object Configuration Codes
@{
\brief CAN Object Configuration codes
\enum ARM_CAN_OBJ_CONFIG 
\details 
Provides defined values for the configuration of CAN objects.

<b>Parameter for:</b>
  - \ref ARM_CAN_ObjectConfigure
@}
**************************************************************************************************************************/

/**
@}
*/   /* End Control Code */

/**
\struct ARM_CAN_OBJ_CAPABILITIES
@{
\details
A CAN object can be implemented with different capabilities encoded in the 
data fields of this structure.

<b>Returned by</b>:
 - \ref ARM_CAN_ObjectGetCapabilities

\sa \ref ARM_CAN_ObjectConfigure
\sa \ref ARM_CAN_MessageSend
\sa \ref ARM_CAN_MessageRead
\sa \ref ARM_CAN_MSG_INFO
\sa \ref can_filtering
@}
*****************************************************************************************************************/


//
//   Functions
//
ARM_DRIVER_VERSION ARM_CAN_GetVersion (void)  {
  return { 0, 0 };
}
/**
\fn       ARM_DRIVER_VERSION ARM_CAN_GetVersion (void)
\details
The function \b ARM_CAN_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION
 - API version is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version is source code version of the actual driver implementation.

Example:
\code
extern ARM_DRIVER_CAN Driver_CAN0;
ARM_DRIVER_CAN *drv_info;

void setup_can (void)  {
  ARM_DRIVER_VERSION  version;

  drv_info = &Driver_CAN0;
  version = drv_info->GetVersion ();
  if (version.api < 0x10A)   {      // requires at minimum API version 1.10 or higher
    // error handling
    return;
  }
}
\endcode
*******************************************************************************************************************/

ARM_CAN_CAPABILITIES ARM_CAN_GetCapabilities (void)  {
  return { 0 };
}
/**
\fn       ARM_CAN_CAPABILITIES ARM_CAN_GetCapabilities (void)
\details
The function \b ARM_CAN_GetCapabilities returns information about the capabilities in this driver implementation.
The data fields of the structure \ref ARM_CAN_CAPABILITIES encode various capabilities.

Example:
\code
extern ARM_DRIVER_CAN Driver_CAN0;
ARM_DRIVER_CAN *drv_info;

void read_capabilities (void)  {
  ARM_CAN_CAPABILITIES drv_capabilities;

  drv_info = &Driver_CAN0;
  drv_capabilities = drv_info->GetCapabilities ();
  // interrogate capabilities

}
\endcode
*******************************************************************************************************************/


int32_t ARM_CAN_Initialize (ARM_CAN_SignalUnitEvent_t cb_unit_event, ARM_CAN_SignalObjectEvent_t cb_object_event)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_CAN_Initialize (ARM_CAN_SignalUnitEvent_t cb_unit_event, ARM_CAN_SignalObjectEvent_t cb_object_event)
\details
The function \b initializes the CAN interface.

The function performs the following operations:
  - Initializes the resources needed for the CAN interface, for example dynamic memory allocation, RTOS object allocation, and possibly hardware pin configuration.
  - Registers the \ref ARM_CAN_SignalUnitEvent callback function.
  - Registers the \ref ARM_CAN_SignalObjectEvent callback function.

The parameter \em cb_unit_event is a pointer to the \ref ARM_CAN_SignalUnitEvent callback function; use a NULL pointer
when no callback signals are required.

The parameter \em cb_object_event is a pointer to the \ref ARM_CAN_SignalObjectEvent callback function; use a NULL pointer
when no callback signals are required.

\b Example:
 - see \ref can_interface_gr

**************************************************************************************************************************/


int32_t ARM_CAN_Uninitialize (void)  {
  return ARM_DRIVER_OK;
}
/**
\fn       int32_t ARM_CAN_Uninitialize (void)
\details
The function \b ARM_CAN_Uninitialize de-initializes the resources of the CAN interface.
It is called to release the software resources used by the interface such as deallocate any RTOS objects, dynamic memory and pin de-configuration.
*******************************************************************************************************************/

int32_t ARM_CAN_PowerControl (ARM_POWER_STATE state)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_CAN_PowerControl (ARM_POWER_STATE state)
\details
The function \b ARM_CAN_PowerControl controls the power modes of the CAN interface.

The parameter \em state can be:
    - ARM_POWER_FULL: Activate clocks and driver functionality as if peripheral was reset.
    - ARM_POWER_OFF:  Unconditionally put peripheral into non-functional (reset) state.
    - ARM_POWER_LOW:  Put peripheral into low power consumption state ready to wake up on bus event.

**************************************************************************************************************************/

uint32_t ARM_CAN_GetClock (void)  {
  return ARM_DRIVER_OK;  
}
/**
\fn          uint32_t ARM_CAN_GetClock (void)
\details
The function \b ARM_CAN_GetClock returns the CAN base clock frequency in \token{[Hz]}.
This value may be used to validate the \em bitrate for the function \ref ARM_CAN_SetBitrate.

<b>Example</b>:
\code
  CAN_clock = ARM_CAN_GetClock();  // CAN base clock frequency
\endcode

**************************************************************************************************************************/

int32_t ARM_CAN_SetBitrate (ARM_CAN_BITRATE_SELECT select, uint32_t bitrate, uint32_t bit_segments)  {
  return ARM_DRIVER_OK;
}
/**
\fn          int32_t ARM_CAN_SetBitrate (ARM_CAN_BITRATE_SELECT select, uint32_t bitrate, uint32_t bit_segments)
\details  
The function \b ARM_CAN_SetBitrate sets the CAN communication bit rate.

The parameter \em select selects the bit rate affected by function call as defined in \ref ARM_CAN_BITRATE_SELECT and listed in the table below.

Parameter \em select                         | CAN Mode Bit Rate
:--------------------------------------------|:------------------------------
\ref ARM_CAN_BITRATE_NOMINAL                 | Select nominal (flexible data-rate arbitration) bitrate (CAN 2.0B)
\ref ARM_CAN_BITRATE_FD_DATA                 | Select flexible data-rate data bitrate (\ref CAN_FD)

The parameter \em bitrate is the bit rate for the selected CAN mode.

The parameter \em bit_segments is used to setup the time quanta for sampling (see picture below).
The values listed in the table below are ORed and specify the various sampling segments.
The CAN controller samples each bit on the bus at the <i>Sample Point</i>.

<table class="cmtable" summary="">
<tr>
  <th>Parameter \em bit_segments</th>
  <th>Bit</th>  
  <th> for \em select = \ref ARM_CAN_BITRATE_NOMINAL \n (CAN specification)</th>
  <th> for \em select = \ref ARM_CAN_BITRATE_NOMINAL \n (CAN FD specification)</th>
  <th> for \em select = \ref ARM_CAN_BITRATE_FD_DATA \n (CAN FD specification)</th>
</tr>
<tr>
  <td>\ref ARM_CAN_BIT_PROP_SEG(<i>x</i>) \n
      Propagation Time Segment \n (PROP_SEG)
  </td>
  <td>0..7 </td>
  <td>\em x = \token{[1..8]}</td>
  <td>\em x = \token{[1..32] or more}</td>  
  <td>\em x = \token{[0..8]}</td>  
</tr>
<tr>
  <td>\ref ARM_CAN_BIT_PHASE_SEG1(<i>x</i>) \n
       Phase Buffer Segment 1 \n (PHASE_SEG1)
  </td>
  <td>8..15 </td>
  <td>\em x = \token{[1..8]}</td>
  <td>\em x = \token{[1..32] or more}</td>  
  <td>\em x = \token{[1..8]}</td>  
</tr>
<tr>
  <td rowspan="2">\ref ARM_CAN_BIT_PHASE_SEG2(<i>x</i>) \n 
      Phase Buffer Segment 2 \n (PHASE_SEG2)
  </td>
  <td rowspan="2">16..23 </td>
  <td>\em x = \token{[1..8]} </td>
  <td>\em x = \token{[1..32] or more}</td>  
  <td>\em x = \token{[1..8]}</td>  
<tr>
  <td colspan="3">The maximum allowed value is \token{x = MAX (PHASE_SEG1, IPT)}. 
                  IPT = Information Processing Time. Usually, IPT = \token{2}. 
				  Exceptions apply. Read the specifications of your CAN controller.</td>
</tr>
<tr>
  <td rowspan="2">\ref ARM_CAN_BIT_SJW(<i>x</i>) \n
      (Re-)Synchronization Jump Width \n (SJW).
  </td>
  <td rowspan="2">24..31 </td>
  <td>\em x = \token{[1..4]}</td>
  <td>\em x = \token{[1..4]}</td>  
  <td>\em x = \token{[1..4]}</td>  
<tr>
  <td colspan="3">The maximum allowed value is \token{x = MIN (MIN (PHASE_SEG1, PHASE_SEG2), 4)}. 
                  SJW is not allowed to be greater than either PHASE segment.
  </td>
</tr>
</table>

<p>
The picture shows a Nominal Bit Time with 10 time quanta.
\image html CAN_Bit_Timing.png  "CAN Bit Timing"
</p>

The time quanta (N) per bit is:
\code
  N = 1 + PROP_SEG + PHASE_SEG1 + PHASE_SEG2; // note SYNC_SEG is always 1
\endcode

The driver uses this value and the CAN clock to calculate a suitable prescaler value (P).
If the driver cannot achieve the requested \em bitrate it returns with \ref ARM_CAN_INVALID_BITRATE.
The formula for the \em bitrate is:
\code
  bitrate = (CAN_Clock / P) / N;
\endcode

<b>Example</b>:
\code
status = ptrCAN->SetBitrate    (ARM_CAN_BITRATE_NOMINAL,              // Set nominal bitrate
                                125000U,                              // Set bitrate to 125 kbit/s
                                ARM_CAN_BIT_PROP_SEG(5U)   |          // Set propagation segment to 5 time quanta
                                ARM_CAN_BIT_PHASE_SEG1(1U) |          // Set phase segment 1 to 1 time quantum (sample point at 87.5% of bit time)
                                ARM_CAN_BIT_PHASE_SEG2(1U) |          // Set phase segment 2 to 1 time quantum (total bit is 8 time quanta long)
                                ARM_CAN_BIT_SJW(1U));                 // Resynchronization jump width is same as phase segment 2
\endcode

In this example, N = 8 and with a CAN_Clock = 8MHz the prescaler (P) is calculated by the driver to 8.
**************************************************************************************************************************/

int32_t ARM_CAN_SetMode (ARM_CAN_MODE mode)  {
  return ARM_DRIVER_OK;
}
/**
\fn  int32_t ARM_CAN_SetMode (ARM_CAN_MODE mode)
\details
The function \b ARM_CAN_SetMode sets the CAN bus communication mode using the parameter \em mode.

The table lists the values for \em mode.

<table class="cmtable" summary="">
    <tr><th>Parameter \em mode</th>                    
        <th>Bus Communication Mode</th>
        <th>supported when \ref ARM_CAN_OBJ_CAPABILITIES data field</th>
    </tr>
    <tr><td>\ref ARM_CAN_MODE_INITIALIZATION</td>    
        <td>Initialization mode; Used to setup communication parameters for the reception 
    	    objects and global filtering, while peripheral is not active on the bus.
    	    Refer to \ref can_filtering for details.</td>
    	<td><i>always supported</i></td>
    </tr>
    <tr><td>\ref ARM_CAN_MODE_NORMAL</td>
        <td>Normal operation mode. Used when peripheral is in active mode to 
            receive, transmit, and acknowledge messages on the bus. Depending on the current unit state, 
    	    it can generate error or overload messages. Verify the unit state with \ref ARM_CAN_GetStatus.
    	<td><i>always supported</i></td>
    </tr>
    <tr><td>\ref ARM_CAN_MODE_RESTRICTED</td>
        <td>Restricted operation mode. Used for monitoring the bus communication non-intrusively 
            without transmitting.</td>
    	<td>\em restricted_mode = \token{1}</td>
    </tr>
    <tr><td>\ref ARM_CAN_MODE_MONITOR</td>
        <td>Bus monitoring mode.</td>
    	<td>\em monitor_mode = \token{1}</td>
    </tr>
    <tr><td>\ref ARM_CAN_MODE_LOOPBACK_INTERNAL</td>
        <td>Test mode; loopback of CAN transmission to its receiver. No transmission visible on CAN bus.</td>
    	<td>\em internal_loopback = \token{1}</td>
    </tr>
    <tr><td>\ref ARM_CAN_MODE_LOOPBACK_EXTERNAL</td> 
        <td>Test mode; loopback of CAN transmission to its receiver. Transmission is visible on CAN bus.</td>
    	<td>\em external_loopback = \token{1}</td>
    </tr>
</table>
**************************************************************************************************************************/

ARM_CAN_OBJ_CAPABILITIES ARM_CAN_ObjectGetCapabilities (uint32_t obj_idx)  {
   // your code
   // return type ARM_CAN_OBJ_CAPABILITIES;
}
/**
\fn          ARM_CAN_OBJ_CAPABILITIES ARM_CAN_ObjectGetCapabilities (uint32_t obj_idx)
\details
The function \b  ARM_CAN_ObjectGetCapabilities retrieves the capabilities of a CAN object. 
The structure \ref ARM_CAN_OBJ_CAPABILITIES stores the values.

The parameter \em obj_idx is the message object index.

\sa ARM_CAN_ObjectConfigure
\sa ARM_CAN_ObjectSetFilter
**************************************************************************************************************************/

int32_t ARM_CAN_ObjectSetFilter (uint32_t obj_idx, ARM_CAN_FILTER_OPERATION operation, uint32_t id, uint32_t arg)  {
  return ARM_DRIVER_OK;
}
/**
\fn          int32_t ARM_CAN_ObjectSetFilter (uint32_t obj_idx, ARM_CAN_FILTER_OPERATION operation, uint32_t id, uint32_t arg)
\details
The function \b ARM_CAN_ObjectSetFilter sets or removes the filter for message reception. Refer to \ref can_filtering for details on filtering. 

The parameter \em obj_idx is the message object index. \n
The parameter \em operation is the operation on the filter as listed in the table below and 
which are defined in the structure \ref ARM_CAN_FILTER_OPERATION.

Parameter \em operation                 |  Operation on Filter          | supported when \ref ARM_CAN_OBJ_CAPABILITIES data field
:---------------------------------------|:------------------------------|:------------------------------------------
\ref ARM_CAN_FILTER_ID_EXACT_ADD        | Add    exact ID filter        | \em exact_filtering = \token{1}
\ref ARM_CAN_FILTER_ID_EXACT_REMOVE     | Remove exact ID filter        | \em exact_filtering = \token{1}
\ref ARM_CAN_FILTER_ID_RANGE_ADD        | Add    range ID filter        | \em range_filtering = \token{1}
\ref ARM_CAN_FILTER_ID_RANGE_REMOVE     | Remove range ID filter        | \em range_filtering = \token{1}
\ref ARM_CAN_FILTER_ID_MASKABLE_ADD     | Add    maskable ID filter     | \em mask_filtering = \token{1}
\ref ARM_CAN_FILTER_ID_MASKABLE_REMOVE  | Remove maskable ID filter     | \em mask_filtering = \token{1}

The parameter \em id is the identifier of the filter or defines the start of the filter range (depends on the filter operation). \n
The parameter \em arg is the mask of the filter or defines the end of the filter range (depends on the filter operation).

\sa ARM_CAN_ObjectConfigure 
**************************************************************************************************************************/

int32_t ARM_CAN_ObjectConfigure (uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg)  {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_CAN_ObjectConfigure (uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg)
\details
The function \b ARM_CAN_ObjectConfigure configures the message object, which can be a mailbox or FIFO.
Refer to \ref can_filtering for details.

The parameter \em obj_idx specifies the message object index. \n
The parameter \em obj_cfg configures the \b object with values as shown in the following table.

<table class="cmtable" summary="">
<tr>
  <th>Parameter \em obj_cfg</th>
  <th>Object Configuration</th>
  <th>supported when \ref ARM_CAN_OBJ_CAPABILITIES data field</th>
</tr>
<tr>
   <td>\ref ARM_CAN_OBJ_INACTIVE</td>
   <td>Deactivate object (default after \ref ARM_CAN_Initialize)
   </td>
   <td><i>always supported</i></td>
</tr>
<tr>
   <td>\ref ARM_CAN_OBJ_RX</td>
   <td>Receive object; read received message with \ref ARM_CAN_MessageRead.
   </td>
   <td>\em rx = \token{1}</td>
</tr>
<tr>
   <td>\ref ARM_CAN_OBJ_TX </td>
   <td>Transmit object; send message with \ref ARM_CAN_MessageSend.
   </td>
   <td>\em tx = \token{1}</td>
</tr>
<tr>
   <td>\ref ARM_CAN_OBJ_RX_RTR_TX_DATA</td>
   <td>\ref Remote_Frame Receive; when \b RTR is received data message is transmitted; set data message with \ref ARM_CAN_MessageSend.
   </td>
   <td>\em  rx_rtr_tx_data = \token{1}</td>
</tr>
<tr>
   <td>\ref ARM_CAN_OBJ_TX_RTR_RX_DATA</td>
   <td>\ref Remote_Frame Transmit; a \b RTR is sent with \ref ARM_CAN_MessageSend to trigger object reception; read received data message with \ref ARM_CAN_MessageRead.
   </td>
   <td>\em  tx_rtr_rx_data = \token{1}</td>
</tr>
</table>

When the \b object is deactivated, it is not used for data communication.

\sa ARM_CAN_ObjectSetFilter
**************************************************************************************************************************/

int32_t ARM_CAN_MessageSend (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, const uint8_t *data, uint8_t size)  {
  return ARM_DRIVER_OK;
}
/**
\fn          int32_t ARM_CAN_MessageSend (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, const uint8_t *data, uint8_t size)
\details
The function \b ARM_CAN_MessageSend sends a CAN message on the CAN bus, or sets data message that will be automatically returned upon RTR reception with matching CAN ID.

Only one message can be sent with a call to this function (for CAN up to \token{8} bytes; for CAN FD up to \token{64} bytes of data).
A message transmission can be terminated with a call to the function \ref ARM_CAN_Control with \em control = \ref ARM_CAN_ABORT_MESSAGE_SEND.

The parameter \em obj_idx specifies the message object index. 

The parameter \em msg_info is a pointer to the structure \ref ARM_CAN_MSG_INFO, which contains the following relevant data fields for sending message:
    - \em id:  Identifier of the message; bit \token{31} specifies if this is an \token{11-bit} or \token{29-bit} identifier.
    - \em rtr: Specifies if Remote Transmission Request should be sent (\em dlc is used for number of requested bytes), otherwise the data message will be sent. Refer to \ref Remote_Frame for details.
    - \em edl: Specifies if Extended Data Length is used; for CAN FD, message can contain up to \token{64} data bytes.
    - \em brs: Specifies if Bit Rate Switching is to be used; for CAN FD, the bit rate can be increased during data phase.
    - \em dlc: Data Length Code of requested data bytes when sending Remote Transmission Request.

The parameter \em data is a pointer to the data buffer.\n
The parameter \em size is the number of data bytes to send.\n

The function returns the number of bytes accepted to be sent or \ref ARM_DRIVER_ERROR_BUSY if the hardware is not
ready to accept a new message for transmission.

When the message is sent, the callback function \ref ARM_CAN_SignalObjectEvent is called signalling \ref ARM_CAN_EVENT_SEND_COMPLETE
on specified object.

\sa \ref can_filtering

<b>Example:</b>

\code
  status = ptrCAN->ObjectConfigure(0, ARM_CAN_OBJ_TX);
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
 
  memset(&tx_msg_info, 0, sizeof(ARM_CAN_MSG_INFO));            // Clear transmit message structure
  tx_msg_info.id  = ARM_CAN_EXTENDED_ID(0x12345678U);           // Set ID of message
  data_buf[0] = '1';  data_buf[1] = '2';                        // Prepare data to transmit
  data_buf[2] = '3';  data_buf[3] = '4';
  data_buf[4] = '5';  data_buf[5] = '6';
  data_buf[6] = '7';  data_buf[7] = '8';
  status = ptrCAN->MesageSend(0, &tx_msg_info, data_buf, 8);    // Send message
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
\endcode
**************************************************************************************************************************/

int32_t ARM_CAN_MessageRead (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, uint8_t *data, uint8_t size)  {
  return ARM_DRIVER_OK;
}
/**
\fn          int32_t ARM_CAN_MessageRead (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, uint8_t *data, uint8_t size)
\details
The function \b ARM_CAN_MessageRead reads the message received on the CAN bus, if \em obj_idx was configured for reception or 
for automatic Data Message reception using RTR and the callback function \ref ARM_CAN_SignalObjectEvent was called 
signalling \ref ARM_CAN_EVENT_RECEIVE.
If the message was overrun by another received message, then the callback function \ref ARM_CAN_SignalObjectEvent
will be called signalling \ref ARM_CAN_EVENT_RECEIVE_OVERRUN.

The function can read a maximum of \token{8} data bytes for CAN and \token{64} bytes for CAN FD.

The parameter \em obj_idx specifies the message object index. \n
The parameter \em msg_info is a pointer to the CAN information structure. \n
The parameter \em data is a pointer to the data buffer for reading data. \n
The parameter \em size is data buffer size in bytes and indicates the maximum number of bytes that can be read.

The function returns the number of read data in bytes or the \ref execution_status.

All data fields of the structure \ref ARM_CAN_MSG_INFO are updated as described below:
    - id:  Identifier of the message that was received, bit \token{31} specifies if it is a \token{11-bit} identifier or \token{29-bit} identifier.
    - rtr: \token{1} = Remote Frame Request was received (\em dlc is number of requested bytes).  \token{0} = data message
    - edl: \token{1} = CAN FD Extended Data Length message was received.  \token{0} = not Extended Data Length message.
    - brs: \token{1} = CAN FD Bit Rate Switching was used for message transfer. \token{0} = no Bit Rate Switching was used.
    - esi: \token{1} = CAN FD Error State Indicator is active for received message.  \token{0} = Error State Indicator is not active.
    - dlc: Data Length Code is the number of data bytes in the received message or number of data bytes requested by RTR.

Message reception can be disabled by de-configuring the receive object with the function \ref ARM_CAN_ObjectConfigure.
**************************************************************************************************************************/

int32_t ARM_CAN_Control (uint32_t control, uint32_t arg)  {
  return ARM_DRIVER_OK;
}
/**
\fn   int32_t ARM_CAN_Control (uint32_t control, uint32_t arg)
\details
The function \b ARM_CAN_Control controls the CAN interface settings and executes various operations.

The parameter \em control specifies various operations that are listed in the table below.

The parameters \em arg provides, depending on the \em control value, additional information or set values.

Parameter \em control                        | Operation
:--------------------------------------------|:------------------------------
\ref ARM_CAN_SET_FD_MODE                     | Select <a href="#CAN_FD"><b>CAN FD</b></a> mode; \em arg : \token{0} = CAN 2.0B; \token{1} = CAN FD.
\ref ARM_CAN_ABORT_MESSAGE_SEND              | Abort sending of CAN message;      \em arg : object index
\ref ARM_CAN_CONTROL_RETRANSMISSION          | Enable/disable automatic retransmission; \em arg : \token{0 = disable, 1 = enable (default state)}
\ref ARM_CAN_SET_TRANSCEIVER_DELAY           | Set transceiver delay; \em arg : delay in time quanta 

Verify the CAN interface capabilities with \ref ARM_CAN_GetCapabilities.
*******************************************************************************************************************/

ARM_CAN_STATUS ARM_CAN_GetStatus (void)  {
  return ARM_DRIVER_OK;
}
/**
\fn            ARM_CAN_STATUS ARM_CAN_GetStatus (void)
\details
The function \b ARM_CAN_GetStatus retrieves runtime information on CAN bus and CAN unit state.

The following defines give information about the current unit involvement in bus communication:

Unit State                             | Description
:--------------------------------------|:------------
\ref ARM_CAN_UNIT_STATE_INACTIVE       | Unit state: Not active on the bus. Unit is in initialization state.
\ref ARM_CAN_UNIT_STATE_ACTIVE         | Unit state: Active on the bus. Unit can generate active error frames.
\ref ARM_CAN_UNIT_STATE_PASSIVE        | Unit state: Error passive. Unit is interacting on the bus but does not send active error frames.
\ref ARM_CAN_UNIT_STATE_BUS_OFF        | Unit state: Bus-off. Unit does not participate on the bus but monitors it and can recover to active state.

The following defines are error codes of the last error that happened on the bus:

Last Error Code                        | Description
:--------------------------------------|:------------
\ref ARM_CAN_LEC_NO_ERROR              | No error. There was no error since last read of status or last successful transmit or receive.
\ref ARM_CAN_LEC_BIT_ERROR             | Bit error. The bit monitored is different than the bit sent (except during arbitration phase).
\ref ARM_CAN_LEC_STUFF_ERROR           | Bit stuffing error. There were 6 consecutive same bit levels on the bus.
\ref ARM_CAN_LEC_CRC_ERROR             | CRC error. CRC of received data is not as expected.
\ref ARM_CAN_LEC_FORM_ERROR            | Illegal fixed-form bit. Error in fixed form bits.
\ref ARM_CAN_LEC_ACK_ERROR             | Acknowledgment error. Message was not acknowledged by any receiver on the bus.

*******************************************************************************************************************/

void ARM_CAN_SignalUnitEvent (uint32_t event)  {
 // function body
}
/**
\fn      void ARM_CAN_SignalUnitEvent (uint32_t event)
\details
The function \b ARM_CAN_SignalUnitEvent is a callback function registered by the function \ref ARM_CAN_Initialize. 

The parameter \em event indicates unit event that occurred during driver operation.

The following callback notifications are generated:

Parameter \em event                | Value |Description
:----------------------------------|:-----:|:-------------------------------------------------
\ref ARM_CAN_EVENT_UNIT_INACTIVE   |   0   | Unit entered Inactive state.
\ref ARM_CAN_EVENT_UNIT_ACTIVE     |   1   | Unit entered Error Active state.
\ref ARM_CAN_EVENT_UNIT_WARNING    |   2   | Unit entered Error Warning state (one or both error counters >= \token{96}).
\ref ARM_CAN_EVENT_UNIT_PASSIVE    |   3   | Unit entered Error Passive state.
\ref ARM_CAN_EVENT_UNIT_BUS_OFF    |   4   | Unit entered Bus-off state.

\sa \ref ARM_CAN_GetStatus 
*******************************************************************************************************************/

void ARM_CAN_SignalObjectEvent (uint32_t obj_idx, uint32_t event)  {
  // function body
}
/**
\fn          void ARM_CAN_SignalObjectEvent (uint32_t obj_idx, uint32_t event)
\details
The function \b ARM_CAN_SignalObjectEvent is a callback function registered by the function \ref ARM_CAN_Initialize and 
signals a CAN message object event. 

The parameter \em obj_idx  is the index of the message object. \n
The parameter \em event indicates object event that occurred during driver operation. 

The following events can be generated:

 Parameter \em event                 | Bit | Description
:------------------------------------|:---:|:-------------------------------------------------------------------------
 \ref ARM_CAN_EVENT_SEND_COMPLETE    |  0  | Message was sent successfully by the \em obj_idx object.
 \ref ARM_CAN_EVENT_RECEIVE          |  1  | Message was received successfully by the \em obj_idx object.
 \ref ARM_CAN_EVENT_RECEIVE_OVERRUN  |  2  | Message was overwritten before it was read on the \em obj_idx object.

\sa \ref ARM_CAN_MessageSend
\sa \ref ARM_CAN_MessageRead
\sa \ref ARM_CAN_ObjectConfigure
*******************************************************************************************************************/

/**
@}
*/
// End CAN Interface
