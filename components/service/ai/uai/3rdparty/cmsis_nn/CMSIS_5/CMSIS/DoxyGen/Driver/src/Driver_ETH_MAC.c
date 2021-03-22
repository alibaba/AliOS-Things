/**
\defgroup   eth_mac_interface_gr Ethernet MAC Interface
\ingroup    eth_interface_gr
\brief      Driver API for Ethernet MAC Peripheral (%Driver_ETH_MAC.h)
\details 
The following section describes the Ethernet MAC Interface as defined in the %Driver_ETH_MAC.h header file.
@{
*/

/**
\struct     ARM_ETH_MAC_CAPABILITIES
\details
An Ethernet MAC driver can be implemented with different capabilities.  
The data fields of this struct encode the capabilities implemented by this driver.

<b>Returned by:</b>
  - \ref ARM_ETH_MAC_GetCapabilities
*/

/**
\struct     ARM_DRIVER_ETH_MAC
\details
The functions of the Ethernet MAC are accessed by function pointers. Refer to \ref DriverFunctions for 
overview information.

Each instance of an Ethernet MAC provides such an access struct. The instance is indicated by
a postfix in the symbol name of the access struct, for example:
 - \b Driver_ETH_MAC0 is the name of the access struct of the first instance (no. 0).
 - \b Driver_ETH_MAC1 is the name of the access struct of the second instance (no. 1).

A configuration setting in the middleware allows connecting the middleware to a specific driver instance <b>Driver_ETH_MAC<i>n</i></b>.
The default is \token{0}, which connects a middleware to the first instance of a driver.
*/

/**
\struct     ARM_ETH_MAC_TIME
\details
The two members of this struct provide fields to encode time values in the order \token{Nano seconds} and \token{seconds}.

The member \em ns is also used as a correction factor for \ref ARM_ETH_MAC_TIMER_ADJUST_CLOCK.

<b>Used in:</b>
  - \ref ARM_ETH_MAC_GetRxFrameTime
  - \ref ARM_ETH_MAC_GetTxFrameTime
  - \ref ARM_ETH_MAC_ControlTimer

*******************************************************************************************************************/

/**
\typedef    ARM_ETH_MAC_SignalEvent_t
\details
Provides the typedef for the callback function \ref ARM_ETH_MAC_SignalEvent.

<b>Parameter for:</b>
  - \ref ARM_ETH_MAC_Initialize
*******************************************************************************************************************/

/**
\defgroup ETH_MAC_events Ethernet MAC Events
\ingroup eth_mac_interface_gr
\brief The Ethernet MAC driver generates call back events that are notified via the function \ref ARM_ETH_MAC_SignalEvent.
\details 
This section provides the event values for the \ref ARM_ETH_MAC_SignalEvent callback function.

The following call back notification events are generated:
@{
\def ARM_ETH_MAC_EVENT_RX_FRAME
\def ARM_ETH_MAC_EVENT_TX_FRAME
\def ARM_ETH_MAC_EVENT_WAKEUP
\def ARM_ETH_MAC_EVENT_TIMER_ALARM
@}
*/

//
// Function documentation
//

ARM_DRIVER_VERSION ARM_ETH_MAC_GetVersion (void)  {
  ;
}
/**
\fn       ARM_DRIVER_VERSION ARM_ETH_MAC_GetVersion (void);
\details
The function \b ARM_ETH_MAC_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION
 - API version is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version is source code version of the actual driver implementation.

Example:
\code
extern ARM_DRIVER_ETH_MAC Driver_ETH_MAC0;
ARM_DRIVER_ETH_MAC *mac;
 
void setup_ethernet (void)  {
  ARM_DRIVER_VERSION  version;
 
  mac = &Driver_ETH_MAC0;  
  version = mac->GetVersion ();
  if (version.api < 0x10A)   {      // requires at minimum API version 1.10 or higher
    // error handling
    return;
  }
}
\endcode
*******************************************************************************************************************/

ARM_ETH_MAC_CAPABILITIES ARM_ETH_MAC_GetCapabilities (void)  {
  ;
}
/**
\fn       ARM_ETH_MAC_CAPABILITIES ARM_ETH_MAC_GetCapabilities (void)
\details
The function \b ARM_ETH_MAC_GetCapabilities retrieves information about capabilities in this driver implementation.
The data fields of the struct \ref ARM_ETH_MAC_CAPABILITIES encode various capabilities, for example
if a hardware is capable to create checksums in hardware or signal events using the \ref ARM_ETH_MAC_SignalEvent 
callback function.
 
Example:
\code
extern ARM_DRIVER_ETH_MAC Driver_ETH_MAC0;
ARM_DRIVER_ETH_MAC *mac;
  
void read_capabilities (void)  {
  ARM_ETH_MAC_CAPABILITIES mac_capabilities;
 
  mac = &Driver_ETH_MAC0;  
  mac_capabilities = mac->GetCapabilities ();
  // interrogate capabilities
 
}
\endcode
*******************************************************************************************************************/

int32_t ARM_ETH_MAC_Initialize (ARM_ETH_MAC_SignalEvent_t cb_event)  {
  
}
/**
\fn     int32_t ARM_ETH_MAC_Initialize (ARM_ETH_MAC_SignalEvent_t cb_event)
\details 
The function \b ARM_ETH_MAC_Initialize initializes the Ethernet MAC interface. 
It is called when the middleware component starts operation.

The \ref ARM_ETH_MAC_Initialize function performs the following operations:
  - Initializes the resources needed for the Ethernet MAC peripheral.
  - Registers the \ref ARM_ETH_MAC_SignalEvent callback function.

The parameter \em cb_event is a pointer to the \ref ARM_ETH_MAC_SignalEvent callback function; use a NULL pointer 
when no callback signals are required.

\b Example:
 - see \ref eth_interface_gr - Driver Functions
*******************************************************************************************************************/

int32_t ARM_ETH_MAC_Uninitialize (void)  {
  
}
/**
\fn       int32_t ARM_ETH_MAC_Uninitialize (void)
\details
The function \b ARM_ETH_MAC_Uninitialize de-initializes the resources of Ethernet MAC interface.

It is called when the middleware component stops operation and releases the software resources 
used by the interface.
*******************************************************************************************************************/

int32_t ARM_ETH_MAC_PowerControl (ARM_POWER_STATE state)  {
  
}
/**
\fn int32_t ARM_ETH_MAC_PowerControl (ARM_POWER_STATE state)
\details     
The function \b ARM_ETH_MAC_PowerControl allows you to configure the power modes of the Ethernet MAC interface.  

The parameter \em state can be:
 - ARM_POWER_OFF: Ethernet MAC peripheral is turned off.
 - ARM_POWER_FULL: Ethernet MAC peripheral is turned on and fully operational.

If power \em state specifies an unsupported mode, the function returns \ref ARM_DRIVER_ERROR_UNSUPPORTED as status information
and the previous power state of the peripheral is unchanged. Multiple calls with the same \em state generate no
error.

\b Example:
 - see \ref eth_interface_gr - Driver Functions
*******************************************************************************************************************/

int32_t ARM_ETH_MAC_GetMacAddress (ARM_ETH_MAC_ADDR *ptr_addr)  {
  
}
/**
\fn int32_t ARM_ETH_MAC_GetMacAddress (ARM_ETH_MAC_ADDR *ptr_addr)
\details
The function \b ARM_ETH_MAC_GetMacAddress retrieves the Ethernet MAC own address from the driver.
*******************************************************************************************************************/

int32_t ARM_ETH_MAC_SetMacAddress (const ARM_ETH_MAC_ADDR *ptr_addr)  {
  
}
/**
\fn int32_t ARM_ETH_MAC_SetMacAddress (const ARM_ETH_MAC_ADDR *ptr_addr)
\details
The function \b ARM_ETH_MAC_SetMacAddress configures Ethernet MAC own address.
The Ethernet MAC accepts packets <a href="http://en.wikipedia.org/wiki/Ethernet_frame" target="_blank"><b>Ethernet frames</b></a> which contains 
a MAC destination address that matches the address specified with \em ptr_addr. 

The Ethernet MAC receiver will accept also packets with addresses configured by \ref ARM_ETH_MAC_SetAddressFilter function.

MAC receiver can be configured to accept also packets with broadcast address, any multicast address or even all packets regardless of address (Promiscuity Mode). 
This is configured by function \ref ARM_ETH_MAC_Control with \ref ARM_ETH_MAC_CONFIGURE as control parameter.
*******************************************************************************************************************/

int32_t ARM_ETH_MAC_SetAddressFilter (const ARM_ETH_MAC_ADDR *ptr_addr, uint32_t num_addr)  {
  
}
/**
\fn int32_t ARM_ETH_MAC_SetAddressFilter (const ARM_ETH_MAC_ADDR *ptr_addr, uint32_t num_addr)
\details
The function \b ARM_ETH_MAC_SetAddressFilter configures Ethernet MAC receiver address filtering.
The Ethernet MAC accepts packets <a href="http://en.wikipedia.org/wiki/Ethernet_frame" target="_blank"><b>Ethernet frames</b></a> which contains 
a MAC destination address of the list supplied with \em ptr_addr.  The parameter \em ptr_addr provides and array of Ethernet MAC addresses.  The number of addresses
is supplied by \em num_addr. Specifying \em num_adr = 0 disables address filtering previously set with this function.

The Ethernet MAC receiver will accept packets addressed to its own address and packets with addresses configured by this function.

MAC receiver can be configured to accept also packets with broadcast address, any multicast address or even all packets regardless of address (Promiscuity Mode). 
This is configured by function \ref ARM_ETH_MAC_Control with \ref ARM_ETH_MAC_CONFIGURE as control parameter.
*******************************************************************************************************************/

int32_t ARM_ETH_MAC_SendFrame (const uint8_t *frame, uint32_t len, uint32_t flags)  {
  
}
/**
\fn int32_t ARM_ETH_MAC_SendFrame (const uint8_t *frame, uint32_t len, uint32_t flags)
\details
The function \b ARM_ETH_MAC_SendFrame writes an <a href="http://en.wikipedia.org/wiki/Ethernet_frame" target="_blank"><b>Ethernet frame</b></a> to the Ethernet MAC transmit buffer.

The Ethernet MAC transmit engine must be enabled by using the function \ref ARM_ETH_MAC_Control (ARM_ETH_MAC_CONTROL_TX, 1) before a call to this function.

The frame data addressed by \em buf starts with MAC destination and ends with the last Payload data byte. The frame data is copied into 
the transmit buffer of the Ethernet MAC interface. The function does not wait until the transmission over the Ethernet is complete, 
however the memory addressed by \em buf is available for the next Ethernet frame after return.

The maximum value for \em len is implied by the size restrictions of the Ethernet frame but is not verified.
Using an invalid value for \em len may generate unpredicted results.

The parameter \em flags specifies additional attributes for the function as shown in the following table. Multiple flags can be combined, for example:
ARM_ETH_MAC_TX_FRAME_EVENT | ARM_ETH_MAC_TX_FRAME_TIMESTAMP.

Flag bit                               | Description
:--------------------------------------|:-----------------------------------------
\ref ARM_ETH_MAC_TX_FRAME_FRAGMENT     | Indicates that it is a fragment of the frame. allows you to collect multiple fragments before the frame is sent.
\ref ARM_ETH_MAC_TX_FRAME_EVENT        | \ref ARM_ETH_MAC_SignalEvent with \em event bit \ref ARM_ETH_MAC_EVENT_TX_FRAME set will be called when frame send is complete.
\ref ARM_ETH_MAC_TX_FRAME_TIMESTAMP    | Capture the time stamp of the frame. The time stamp can be obtained using the function \ref ARM_ETH_MAC_GetTxFrameTime.


\b Example:
\code
  status = mac->SendFrame (&frame->data[0], frame->length, 0);
  if (status != ARM_DRIVER_OK)  {
    // error handling
  }
\endcode

*******************************************************************************************************************/

int32_t ARM_ETH_MAC_ReadFrame (uint8_t *frame, uint32_t len)  {
  
}
/**
\fn int32_t ARM_ETH_MAC_ReadFrame (uint8_t *frame, uint32_t len)
\details
The function \b ARM_ETH_MAC_ReadFrame reads an <a href="http://en.wikipedia.org/wiki/Ethernet_frame" target="_blank"><b>Ethernet frame</b></a> from the Ethernet MAC receive buffer.

The Ethernet MAC receive engine must be enabled using the function \ref ARM_ETH_MAC_Control (ARM_ETH_MAC_CONTROL_RX , 1) before a call to this function.
The \em len of the Ethernet frame can be checked using the function \ref ARM_ETH_MAC_GetRxFrameSize.

The frame data addressed by \em buf starts with MAC destination and ends with the last Payload data byte. The frame data is read from 
the receive buffer of the Ethernet MAC interface and the number of bytes written into the memory addressed by \em buf is returned.
A negative return value indicates an error whereby the status code is defined with driver common return codes.

The function \ref ARM_ETH_MAC_ReadFrame may be called with \em buf = NULL and \em len = 0 to discard or release an frame. This is useful when an incorrect frame has been received or
no memory is available to hold the Ethernet frame.

\b Example:
\code
  size = mac->GetRxFrameSize ();
  if ((size < 14) || (size > 1514)) {    // frame excludes CRC
    mac->ReadFrame (NULL, 0);            // Frame error, release it
  }
  len = mac->ReadFrame (&frame->data[0], size);
  if (len < 0)  {
    // error handling
  }
\endcode
*******************************************************************************************************************/

uint32_t ARM_ETH_MAC_GetRxFrameSize (void)  {
  
}
/**
\fn uint32_t ARM_ETH_MAC_GetRxFrameSize (void)
\details
The function \b ARM_ETH_MAC_GetRxFrameSize returns the size of a received 
<a href="http://en.wikipedia.org/wiki/Ethernet_frame" target="_blank"><b>Ethernet frame</b></a>.
This function is called before \ref ARM_ETH_MAC_ReadFrame and supplies the value \em len.

The frame size includes MAC destination and ends with the last Payload data byte.
Value \em 0 indicates that no Ethernet frame is available in the receive buffer.
Values smaller than minimum size of Ethernet frame or larger than maximum size of Ethernet frame
indicate an invalid frame which needs to be discarded by calling \ref ARM_ETH_MAC_ReadFrame.

\b Example:
  - see \ref ARM_ETH_MAC_ReadFrame

*******************************************************************************************************************/

int32_t ARM_ETH_MAC_GetRxFrameTime (ARM_ETH_MAC_TIME *time)  {
  
}
/**
\fn int32_t ARM_ETH_MAC_GetRxFrameTime (ARM_ETH_MAC_TIME *time)
\details
Retrieve time stamp of a received <a href="http://en.wikipedia.org/wiki/Ethernet_frame" target="_blank"><b>Ethernet frame</b></a>.
This function must be called before the frame is read using \ref ARM_ETH_MAC_ReadFrame.
*******************************************************************************************************************/

int32_t ARM_ETH_MAC_GetTxFrameTime (ARM_ETH_MAC_TIME *time)  {
  
}
/**
\fn int32_t ARM_ETH_MAC_GetTxFrameTime (ARM_ETH_MAC_TIME *time)
\details
The function \b returns the time stamp of a transmitted <a href="http://en.wikipedia.org/wiki/Ethernet_frame" target="_blank"><b>Ethernet frame</b></a>.
*******************************************************************************************************************/

int32_t ARM_ETH_MAC_Control (uint32_t control, uint32_t arg)  {
  
}
/**
\fn int32_t ARM_ETH_MAC_Control (uint32_t control, uint32_t arg)
\details
The function \b ARM_ETH_MAC_Control controls the Ethernet MAC interface and executes various operations. 
After initialization, the Ethernet transceiver and receiver are disabled.

The parameter \em control specifies an operation as defined in the table <b>Parameter \em control</b>. \n
The parameter \em arg provides, depending on the operation, additional information or values.

The table lists values for the parameter \em control.

Parameter \em control                                | Operation
:----------------------------------------------------|:--------------------------------------------
\ref ARM_ETH_MAC_CONFIGURE                           | Configure the Ethernet MAC interface; For \em arg values, see table <b>Parameter \em arg for CONFIGURE</b>
\ref ARM_ETH_MAC_CONTROL_TX                          | Enable or disable the transmitter; \em arg :  \token{0=disable; 1=enable}
\ref ARM_ETH_MAC_CONTROL_RX                          | Enable or disable the receiver; \em arg :  \token{0=disable; 1=enable}
\ref ARM_ETH_MAC_FLUSH                               | Flush a buffer; \em arg : see table <b>Parameter \em arg for FLUSH</b>
\ref ARM_ETH_MAC_SLEEP                               | Exit/Enter Sleep mode; \em arg : \token{0=exit; 1=enter and wait for Magic packet}
\ref ARM_ETH_MAC_VLAN_FILTER                         | Configure VLAN Filter for received frames;  \em arg : See table <b>Parameter \em arg for VLAN Filter</b>


The table <b>Parameter \em arg for CONFIGURE</b> lists the \em arg values for the \em control \b ARM_ETH_MAC_CONFIGURE.
The values can be ORed in the following way:
\code
    mac->Control(ARM_ETH_MAC_CONFIGURE, ARM_ETH_MAC_SPEED_100M | ARM_ETH_MAC_DUPLEX_FULL | ARM_ETH_MAC_LOOPBACK);
\endcode

<table class="cmtable" summary="">
<tr><th colspan="4">Parameter \em arg CONFIGURE </th></tr>
<tr><th>Parameter \em arg                   </th><th>               Bit    </th><th>            Category     </th><th>Description                             </th></tr>
<tr><td>\ref ARM_ETH_MAC_SPEED_10M          </td><td rowspan="3">   0..1   </td><td rowspan="3">Link Speed   </td><td>Set the link speed to \token{10 [Mbps]} </td></tr>
<tr><td>\ref ARM_ETH_MAC_SPEED_100M         </td>                                                                 <td>Set the link speed to \token{100 [Mbps]}</td></tr>
<tr><td>\ref ARM_ETH_MAC_SPEED_1G           </td>                                                                 <td>Set the link speed to \token{1  [Gbps]} </td></tr>
<tr><td>\ref ARM_ETH_MAC_DUPLEX_HALF        </td><td rowspan="2">   2      </td><td rowspan="2">Link Mode    </td><td>Set the link mode to half duplex        </td></tr>
<tr><td>\ref ARM_ETH_MAC_DUPLEX_FULL        </td>                                                                 <td>Set the link mode to full duplex        </td></tr>
<tr><td>n.a.                                </td><td>               3      </td><td>            n.a.         </td><td>\em reserved                            </td></tr>                      
<tr><td>\ref ARM_ETH_MAC_LOOPBACK           </td><td>               4      </td><td>    Loopback Test Mode   </td><td>Set the interface into a Loop-back test mode</td></tr>
<tr><td>\ref ARM_ETH_MAC_CHECKSUM_OFFLOAD_RX</td><td>               5      </td><td>Receiver Checksum offload</td><td>Enable Receiver Checksum offload        </td></tr>
<tr><td>\ref ARM_ETH_MAC_CHECKSUM_OFFLOAD_TX</td><td>               6      </td><td>Transmitter Checksum offload</td><td>Enable Transmitter Checksum offload  </td></tr>
<tr><td>\ref ARM_ETH_MAC_ADDRESS_BROADCAST  </td><td>               7      </td><td>Broadcast Frame address  </td><td>Accept frames with Broadcast address    </td></tr>
<tr><td>\ref ARM_ETH_MAC_ADDRESS_MULTICAST  </td><td>               8      </td><td>Multicast Frame address  </td><td>Accept frames with any Multicast address</td></tr>
<tr><td>\ref ARM_ETH_MAC_ADDRESS_ALL        </td><td>               9      </td><td>Any Frame address        </td><td>Accept frames with any address (Promiscuous Mode)</td></tr>
</table>

The table <b>Parameter \em arg for FLUSH</b> lists the \em arg values for the \em control \b ARM_ETH_MAC_FLUSH.
The \em arg values can be ORed. 

<table class="cmtable" summary="">
<tr><th colspan="4">Parameter \em arg for FLUSH </th></tr>
<tr><th>Parameter \em arg          </th><th>    Bit    </th><th>     Category     </th><th> Description                  </th></tr>
<tr><td>\ref ARM_ETH_MAC_FLUSH_RX  </td><td>    1      </td><td>  Receive buffer  </td><td> Flush the Receive buffer     </td></tr>
<tr><td>\ref ARM_ETH_MAC_FLUSH_TX  </td><td>    2      </td><td>  Transmit buffer </td><td> Flush the Transmit buffer    </td></tr>
</table>

The table <b>Parameter \em arg for VLAN Filter</b> lists the \em arg values for the \em control \b ARM_ETH_MAC_VLAN_FILTER.
The \em arg values can be ORed. 

<table class="cmtable" summary="">
<tr><th colspan="4">Parameter \em arg for VLAN Filter</th></tr>
<tr><th>Parameter \em arg                     </th><th>              Bit    </th><th>             Category    </th><th> Description                                </th></tr>
<tr><td>\em value                             </td><td>              0..15  </td><td>             VLAN Tag    </td><td> Set VLAN Tag value                         </td></tr>
<tr><td>\token{0}                             </td><td rowspan="2">  16     </td><td rowspan="2"> Use of VLAN </td><td> Compare the complete 16-bit VLAN Tag value </td></tr>
<tr><td>\ref ARM_ETH_MAC_VLAN_FILTER_ID_ONLY  </td>                                                                <td>Compare only the 12-bit VLAN Identifier     </td></tr>
<tr><td>\token{0}                             </td><td>              0..16  </td><td>             Disable     </td><td> Disable the VLAN Filter                    </td></tr>
</table>


\b Example:

\code
...
                                         // start transfer
    mac->Control(ARM_ETH_MAC_CONFIGURE, ARM_ETH_MAC_SPEED_100M | ARM_ETH_MAC_DUPLEX_FULL | ARM_ETH_MAC_ADDRESS_BROADCAST);
    mac->Control(ARM_ETH_MAC_CONTROL_TX, 1);
    mac->Control(ARM_ETH_MAC_CONTROL_RX, 1);
	 
...                                      // stop transfer
    mac->Control(ARM_ETH_MAC_FLUSH, ARM_ETH_MAC_FLUSH_TX | ARM_ETH_MAC_FLUSH_RX);
    mac->Control(ARM_ETH_MAC_CONTROL_TX, 0);
    mac->Control(ARM_ETH_MAC_CONTROL_RX, 0);
  }
}
\endcode

For a complete example, refer to  \ref eth_interface_gr - Driver Functions.

*******************************************************************************************************************/

int32_t ARM_ETH_MAC_ControlTimer (uint32_t control, ARM_ETH_MAC_TIME *time)  {
  
}
/**
\fn int32_t ARM_ETH_MAC_ControlTimer (uint32_t control, ARM_ETH_MAC_TIME *time)
\details
The function \b ARM_ETH_MAC_ControlTimer controls the timer required for PTP (Precision Time Protocol).

The parameter \em control receives \b ARM_ETH_MAC_TIMER_xxx codes to manage the timer for a PTP enabled Ethernet MAC interface. \n
The parameter \em time is pointer to a structure that holds time information.

Mode Parameters: Timer Controls         | Description
:---------------------------------------|:--------------------------------------------
\ref ARM_ETH_MAC_TIMER_GET_TIME         | Retrieve the current time and update the content \ref ARM_ETH_MAC_TIME *time.
\ref ARM_ETH_MAC_TIMER_SET_TIME         | Set the new time using the values provided with \ref ARM_ETH_MAC_TIME *time.
\ref ARM_ETH_MAC_TIMER_INC_TIME         | Increment the current time by using the values provided with \ref ARM_ETH_MAC_TIME *time.
\ref ARM_ETH_MAC_TIMER_DEC_TIME         | Decrement the current time by using the values provided with \ref ARM_ETH_MAC_TIME *time.
\ref ARM_ETH_MAC_TIMER_SET_ALARM        | Set the alarm time to the values provided with \ref ARM_ETH_MAC_TIME *time.  
\ref ARM_ETH_MAC_TIMER_ADJUST_CLOCK     | Set the clock frequency; the value in time->ns is the <b>correction factor</b> in fractional format q31.

*******************************************************************************************************************/

int32_t ARM_ETH_MAC_PHY_Read (uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)  {
  
}
/**
\fn int32_t ARM_ETH_MAC_PHY_Read (uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
\details

Read Ethernet PHY Register through the Management Interface. The function is passed to \ref ARM_ETH_PHY_Initialize.
The Ethernet PHY driver uses this function to read the value of PHY registers.

\b Example:
 - see \ref eth_interface_gr - Driver Functions
*******************************************************************************************************************/

int32_t ARM_ETH_MAC_PHY_Write (uint8_t phy_addr, uint8_t reg_addr, uint16_t data)  {
  
}
/**
\fn int32_t ARM_ETH_MAC_PHY_Write (uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
\details
The function \b ARM_ETH_MAC_PHY_Write writes to a Ethernet PHY register through the Management Interface.  The function is passed to \ref ARM_ETH_PHY_Initialize.
The Ethernet PHY driver uses this function to write data to PHY registers.

\b Example:
 - see \ref eth_interface_gr - Driver Functions

*******************************************************************************************************************/

void ARM_ETH_MAC_SignalEvent (uint32_t event)  {
  ;
}
/**
\fn void ARM_ETH_MAC_SignalEvent (uint32_t event)
\details

The function \b ARM_ETH_MAC_SignalEvent is a callback function registered by the function
\ref ARM_ETH_MAC_Initialize. This function is typically called from interrupt service routines (ISR) to indicate that
a frame is processed or a special event occurred.

The parameter \a event indicates one or more events that occurred during driver operation.
Each event is encoded in a separate bit and therefore it is possible to signal multiple events within the same call. 

Not every event is necessarily generated by the driver. This depends on the implemented capabilities stored in the 
data fields of the structure \ref ARM_ETH_MAC_CAPABILITIES, which can be retrieved with the function \ref ARM_ETH_MAC_GetCapabilities.

The following events can be generated:

Parameter \em event                      | Bit | Description 
:----------------------------------------|:---:|:----------------------------------------
\ref ARM_ETH_MAC_EVENT_RX_FRAME          |  0  | Occurs after a frame is received. Frame can be read by calling \ref ARM_ETH_MAC_ReadFrame.
\ref ARM_ETH_MAC_EVENT_TX_FRAME          |  1  | Occurs after call to \ref ARM_ETH_MAC_SendFrame to indicate that the frame is transmitted.
\ref ARM_ETH_MAC_EVENT_WAKEUP            |  2  | Indicates that a Magic Packet is received while the driver is in Sleep mode (set by \ref ARM_ETH_MAC_SLEEP using \ref ARM_ETH_MAC_Control).
\ref ARM_ETH_MAC_EVENT_TIMER_ALARM       |  3  | Indicates that a Timer Alarm occurred that was set with \ref ARM_ETH_MAC_TIMER_SET_ALARM using ARM_ETH_MAC_ControlTimer.

*******************************************************************************************************************/


/**
\defgroup eth_mac_control Ethernet MAC Control Codes
\ingroup eth_mac_interface_gr
\brief Configure and control the Ethernet MAC using the \ref ARM_ETH_MAC_Control.
\details 
@{
Many parameters of the Ethernet MAC driver are configured using the \ref ARM_ETH_MAC_Control function.

The various Ethernet MAC control codes define:
  
  - \ref eth_mac_ctrls                configures and controls the Ethernet MAC interface
  - \ref eth_mac_configuration_ctrls  specifies speed mode, link mode, checksum, and frame filtering modes
  - \ref eth_mac_flush_flag_ctrls     specify controls to flush a buffer
  - \ref eth_mac_vlan_filter_ctrls    specifies whether to compare only the VLAN Identifier

Refer to the \ref ARM_ETH_MAC_Control function for further details.
*/

/**
\defgroup eth_mac_ctrls Ethernet MAC Controls
\brief Configure and control the Ethernet MAC interface.
\details
@{
\def ARM_ETH_MAC_CONFIGURE          
\sa ARM_ETH_MAC_Control
\def ARM_ETH_MAC_CONTROL_TX         
\sa ARM_ETH_MAC_Control
\def ARM_ETH_MAC_CONTROL_RX         
\sa ARM_ETH_MAC_Control
\def ARM_ETH_MAC_FLUSH              
\sa ARM_ETH_MAC_Control
\def ARM_ETH_MAC_SLEEP              
\sa ARM_ETH_MAC_Control
\def ARM_ETH_MAC_VLAN_FILTER        
\sa ARM_ETH_MAC_Control
@}
*/


/**
\defgroup eth_mac_configuration_ctrls Ethernet MAC Configuration
\brief Specifies speed mode, link mode, checksum, and frame filtering modes.
\details
@{
The function \ref ARM_ETH_MAC_Control with \em control = \ref ARM_ETH_MAC_CONFIGURE configures the Ethernet MAC interface
as specified with \em arg listed bellow.

\def ARM_ETH_MAC_SPEED_10M           
\def ARM_ETH_MAC_SPEED_100M          
\def ARM_ETH_MAC_SPEED_1G            
\def ARM_ETH_MAC_DUPLEX_HALF         
\def ARM_ETH_MAC_DUPLEX_FULL         
\def ARM_ETH_MAC_LOOPBACK            
\def ARM_ETH_MAC_CHECKSUM_OFFLOAD_RX 
\def ARM_ETH_MAC_CHECKSUM_OFFLOAD_TX 
\def ARM_ETH_MAC_ADDRESS_BROADCAST   
\def ARM_ETH_MAC_ADDRESS_MULTICAST   
\def ARM_ETH_MAC_ADDRESS_ALL         
@}
*/

/**
\defgroup eth_mac_flush_flag_ctrls  Ethernet MAC Flush Flags
\brief Specify controls to flush a buffer
\details
@{
The function \ref ARM_ETH_MAC_Control with \em control = \ref ARM_ETH_MAC_FLUSH flushes the buffer
which is specified with \em arg listed bellow.

\def ARM_ETH_MAC_FLUSH_RX           
\def ARM_ETH_MAC_FLUSH_TX           
@}
*/


/**
\defgroup eth_mac_vlan_filter_ctrls   Ethernet MAC VLAN Filter Flag
\brief Specify whether to compare only the VLAN Identifier
\details
@{
The function \ref ARM_ETH_MAC_Control with \em control = \ref ARM_ETH_MAC_VLAN_FILTER configures the VLAN Filter for received frames as specified with \em arg.

By default the complete VLAN Tag (16-bit) is compared. When \ref ARM_ETH_MAC_VLAN_FILTER_ID_ONLY is specified then only the VLAN Identifier (12-bit) is compared.

Specifying \em arg=0 disables the VLAN Filter. 

\def ARM_ETH_MAC_VLAN_FILTER_ID_ONLY 
@}
*/


/**
@} */  // end group eth_mac_control 


/**
\defgroup eth_mac_time_control Ethernet MAC Timer Control Codes
\ingroup eth_mac_interface_gr
\brief Control codes for \ref ARM_ETH_MAC_ControlTimer function.
\details 
The following timer controls are used as parameter \em control for the \ref ARM_ETH_MAC_ControlTimer function:
@{
\def ARM_ETH_MAC_TIMER_GET_TIME     
\sa ARM_ETH_MAC_ControlTimer
\def ARM_ETH_MAC_TIMER_SET_TIME     
\sa ARM_ETH_MAC_ControlTimer
\def ARM_ETH_MAC_TIMER_INC_TIME     
\sa ARM_ETH_MAC_ControlTimer
\def ARM_ETH_MAC_TIMER_DEC_TIME     
\sa ARM_ETH_MAC_ControlTimer
\def ARM_ETH_MAC_TIMER_SET_ALARM    
\sa ARM_ETH_MAC_ControlTimer
\def ARM_ETH_MAC_TIMER_ADJUST_CLOCK 
\sa ARM_ETH_MAC_ControlTimer
@}
*/


/**
\defgroup eth_mac_frame_transmit_ctrls Ethernet MAC Frame Transmit Flags
\brief Specify frame transmit flags
\details
@{
\def ARM_ETH_MAC_TX_FRAME_FRAGMENT  
\sa ARM_ETH_MAC_SendFrame
\def ARM_ETH_MAC_TX_FRAME_EVENT     
\sa ARM_ETH_MAC_SendFrame
\def ARM_ETH_MAC_TX_FRAME_TIMESTAMP 
\sa ARM_ETH_MAC_SendFrame
@}
*/



/**
@}
*/ 
// End ETH MAC Interface
