/**
\defgroup eth_phy_interface_gr Ethernet PHY Interface
\ingroup eth_interface_gr
\brief Driver API for Ethernet PHY Peripheral (%Driver_ETH_PHY.h)
\details The following section describes the Ethernet PHY Interface as defined in the %Driver_ETH_PHY.h header file.

The %Driver_ETH_PHY.h contains two \#defines that are used to configure the connection between the PHY and the
microcontroller device:
- \c ETH_PHY_NUM and
- \c ETH_PHY_ADDR

Usually, the Serial Management Interface (\b SMI) (using MDC and MDIO) is used to access the PHY’s internal registers to read
the state of the link (up/down), duplex mode, speed, and to restart auto-negotiation etc. SMI is a serial bus, which allows
to connect up to 32 devices. Devices on the bus are accessed using a 5-bit device address. A default device address is
hardware configurable by pin-strapping on the device (some pins are sampled when a reset is asserted or at power-up).

The device’s internal weak pull-up or pull-down resistors define a default device address. This address can be changed by
connecting strong pull-up or pull-down resistors externally. In this case, the \c ETH_PHY_ADDR needs to be defined by the
user.

If a microcontroller device offers more than one Ethernet PHY driver, the user needs to set the correct \c ETH_PHY_NUM in his
application.
@{
*/

/**
\struct     ARM_DRIVER_ETH_PHY
\details
The functions of the Ethernet PHY are accessed by function pointers exposed by this structure. Refer to \ref DriverFunctions for 
overview information.

Each instance of an Ethernet PHY provides such an access struct. The instance is identified by
a postfix number in the symbol name of the access struct, for example:
 - \b Driver_ETH_PHY0 is the name of the access struct of the first instance (no. 0).
 - \b Driver_ETH_PHY1 is the name of the access struct of the second instance (no. 1).


A configuration setting in the middleware allows connecting the middleware to a specific driver instance <b>Driver_ETH_PHY<i>n</i></b>.
The default is \token{0}, which connects a middleware to the first instance of a driver.
*****************************************************************************************************************/


/**
\typedef    ARM_ETH_PHY_Read_t
\details
Provides the typedef for the register read function \ref ARM_ETH_MAC_PHY_Read.

<b>Parameter for:</b>
  - \ref ARM_ETH_PHY_Initialize
*******************************************************************************************************************/

/**
\typedef    ARM_ETH_PHY_Write_t
\details
Provides the typedef for the register write function \ref ARM_ETH_MAC_PHY_Write.

<b>Parameter for:</b>
  - \ref ARM_ETH_PHY_Initialize
*******************************************************************************************************************/


//
// Functions
//

ARM_DRIVER_VERSION ARM_ETH_PHY_GetVersion (void)  {
  return { 0, 0 };
}
/**
\fn     ARM_DRIVER_VERSION ARM_ETH_PHY_GetVersion (void)
\details
The function \b ARM_ETH_PHY_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION
 - API version is the version of the CMSIS-Driver specification used to implement this driver.
 - Driver version is source code version of the actual driver implementation.

Example:
\code
extern ARM_DRIVER_ETH_PHY Driver_ETH_PHY0;
ARM_DRIVER_ETH_PHY *drv_info;
 
void setup_ethernet_phy (void)  {
  ARM_DRIVER_VERSION  version;
 
  drv_info = &Driver_ETH_PHY0;  
  version = drv_info->GetVersion ();
  if (version.api < 0x10A)   {      // requires at minimum API version 1.10 or higher
    // error handling
    return;
  }
}
\endcode
*****************************************************************************************************************/

int32_t ARM_ETH_PHY_Initialize (ARM_ETH_PHY_Read_t fn_read, ARM_ETH_PHY_Write_t fn_write)  {
  return 0;
}
/**
\fn       int32_t ARM_ETH_PHY_Initialize (ARM_ETH_PHY_Read_t fn_read, ARM_ETH_PHY_Write_t fn_write)
\details
The function \b ARM_ETH_PHY_Initialize initializes the Ethernet PHY interface. 
It is called when the middleware component starts operation.

The \ref ARM_ETH_PHY_Initialize function performs the following operations:
  - Initializes the resources needed for Ethernet PHY peripheral.
  - Registers the \ref ARM_ETH_MAC_PHY_Read register read access function.
  - Registers the \ref ARM_ETH_MAC_PHY_Write register write access function.

\b Example:
 - see \ref eth_interface_gr - Driver Functions

*****************************************************************************************************************/

int32_t ARM_ETH_PHY_Uninitialize (void)  {
  return 0;
}
/**
\fn       int32_t ARM_ETH_PHY_Uninitialize (void)
\details
The function \b ARM_ETH_PHY_Uninitialize de-initializes the resources of Ethernet PHY interface.

It is called when the middleware component stops operation and releases the software resources used by the interface.
*****************************************************************************************************************/

int32_t ARM_ETH_PHY_PowerControl (ARM_POWER_STATE state)  {
  return 0;
}
/**
\fn int32_t ARM_ETH_PHY_PowerControl (ARM_POWER_STATE state)
\details     
The function \b ARM_ETH_PHY_PowerControl operates the power modes of the Ethernet PHY interface.  

The parameter \em state sets the operation and can have the following values:
  - \ref ARM_POWER_FULL : set-up peripheral for data transfers, enable interrupts (NVIC) and optionally DMA. 
                          Can be called multiple times. If the peripheral is already in this mode the function performs 
						  no operation and returns with \ref ARM_DRIVER_OK.
  - \ref ARM_POWER_LOW : may use power saving. Returns \ref ARM_DRIVER_ERROR_UNSUPPORTED when not implemented.
  - \ref ARM_POWER_OFF : terminates any pending data transfers, disables peripheral, disables related interrupts and DMA.
      
Refer to \ref CallSequence for more information.

\b Example:
 - see \ref eth_interface_gr - Driver Functions
*****************************************************************************************************************/

int32_t ARM_ETH_PHY_SetInterface (uint32_t interface)  {
  return 0;
}
/**
\fn int32_t ARM_ETH_PHY_SetInterface (uint32_t interface)

\details
The function \b ARM_ETH_PHY_SetInterface specifies the \ref eth_interface_types1 that links the Ethernet MAC and Ethernet PHY. 
After initialization of the PHY interface, you can set the media type.
The function \ref ARM_ETH_MAC_GetCapabilities retrieves the media interface type encoded in the data field \b media_interface of the structure
\ref ARM_ETH_MAC_CAPABILITIES. 

The parameter \em interface can have the following values:

Parameter \em interface       | Media Type
:-----------------------------|:-------------------------
\ref ARM_ETH_INTERFACE_MII    | Media Independent Interface (MII)
\ref ARM_ETH_INTERFACE_RMII   | Reduced Media Independent Interface (RMII)
\ref ARM_ETH_INTERFACE_SMII   | Serial Media Independent Interface (SMII); 

\note
Some \em interface values may be unsupported by a driver implementation. For example \ref ARM_ETH_INTERFACE_SMII may return \b ARM_DRIVER_ERROR_UNSUPPORTED.

\b Example:
\code
static ARM_ETH_MAC_CAPABILITIES capabilities;
static ARM_DRIVER_ETH_MAC *mac;
static ARM_DRIVER_ETH_PHY *phy;
 
mac = &Driver_ETH_MAC0;
phy = &Driver_ETH_PHY0;
 
// Initialize Media Access Controller
capabilities = mac->GetCapabilities ();
...
status = phy->SetInterface (capabilities.media_interface);
if (status != ARM_DRIVER_OK) ...  // error handling
status = phy->SetMode (ARM_ETH_PHY_AUTO_NEGOTIATE);
if (status != ARM_DRIVER_OK) ...  // error handling
...
\endcode
*****************************************************************************************************************/

int32_t ARM_ETH_PHY_SetMode (uint32_t mode)  {
  return 0;
}
/**
\fn int32_t ARM_ETH_PHY_SetMode (uint32_t mode)
\details
The function \b ARM_ETH_PHY_SetMode sets the operation mode parameters for the Ethernet PHY.


The table below lists the possible values for the parameter \em mode. Values from different categories can be ORed as shown in this example code:

\code
phy->SetMode (ARM_ETH_PHY_SPEED_100M | ARM_ETH_PHY_LOOPBACK | ARM_ETH_PHY_DUPLEX_HALF );
\endcode
\n

<table class="cmtable" summary="">
<tr><th>Parameter \em mode              </th><th> bit              </th><th> Category              </th> <th>Description</th></tr>
<tr><td>\ref ARM_ETH_PHY_SPEED_10M      </td><td rowspan="3"> 0..1 </td><td rowspan="3">Link Speed </td> <td>Set the link speed to \token{10 [Mbps]}     </td></tr>
<tr><td>\ref ARM_ETH_PHY_SPEED_100M     </td>                                                      <td>Set the link speed to \token{100 [Mbps]}          </td></tr>
<tr><td>\ref ARM_ETH_PHY_SPEED_1G       </td>                                                      <td>Set the link speed to \token{1  [Gbps]}           </td></tr>
<tr><td>\ref ARM_ETH_PHY_DUPLEX_HALF    </td><td rowspan="2"> 2    </td><td rowspan="2">Link Mode  </td> <td>Set the link mode to half duplex            </td></tr>
<tr><td>\ref ARM_ETH_PHY_DUPLEX_FULL    </td>                                                      <td>Set the link mode to full duplex                  </td></tr>
<tr><td>\ref ARM_ETH_PHY_AUTO_NEGOTIATE </td><td>           3      </td><td>Autonegotiation        </td> <td>Set the interface to Auto Negotiation mode of transmission parameters</td></tr>
<tr><td>\ref ARM_ETH_PHY_LOOPBACK       </td><td>           4      </td><td>Loopback               </td> <td>Set the interface into a Loop-back test mode      </td></tr>
<tr><td>\ref ARM_ETH_PHY_ISOLATE        </td><td>           5      </td><td>Isolation              </td> <td>Set to indicate electrical isolation of PHY interface from MII/RMII interface</td></tr>
</table>

\note
Some settings may be also taken from configuration pins (example \ref ARM_ETH_PHY_ISOLATE). Check the effect of mode settings in the actual driver implementation.
\note
Some \em mode values may be unsupported by a driver implementation. For example \ref ARM_ETH_PHY_SPEED_1G may return \b ARM_DRIVER_ERROR_UNSUPPORTED.


\b Example:
\code
static ARM_ETH_MAC_CAPABILITIES capabilities;
static ARM_DRIVER_ETH_MAC *mac;
static ARM_DRIVER_ETH_PHY *phy;
 
mac = &Driver_ETH_MAC0;
phy = &Driver_ETH_PHY0;
 
// Initialize Media Access Controller
capabilities = mac->GetCapabilities ();
...
status = phy->SetInterface (capabilities.media_interface);
if (status != ARM_DRIVER_OK) ...  // error handling
status = phy->SetMode (ARM_ETH_PHY_SPEED_100M | ARM_ETH_PHY_DUPLEX_FULL | ARM_ETH_PHY_ISOLATE);
if (status != ARM_DRIVER_OK) ...  // error handling
...
\endcode



*****************************************************************************************************************/

ARM_ETH_LINK_STATE ARM_ETH_PHY_GetLinkState (void)  {
  return 0;
}
/**
\fn ARM_ETH_LINK_STATE ARM_ETH_PHY_GetLinkState (void)
\details
The function \b ARM_ETH_PHY_GetLinkState retrieves the connection status of the physical Ethernet link.

\b Example:
 - see \ref eth_interface_gr - Driver Functions
*****************************************************************************************************************/

ARM_ETH_LINK_INFO ARM_ETH_PHY_GetLinkInfo (void)  {
  return 0;
}
/**
\fn ARM_ETH_LINK_INFO ARM_ETH_PHY_GetLinkInfo (void)
\details
The function \b ARM_ETH_PHY_GetLinkInfo retrieves information about the current established communication
mode (half/full duplex) and communication speed. Information is only valid when link is up (see \ref ARM_ETH_PHY_GetLinkState).

\b Example:
 - see \ref eth_interface_gr - Driver Functions
*****************************************************************************************************************/


/**
@}
*/ 
// End ETH PHY Interface group; below the groups are included with \ingroup


/**
\defgroup eth_phy_mode_ctrls Ethernet PHY Mode
\ingroup eth_phy_interface_gr
\brief Specify operation modes of the Ethernet PHY interface
\details
@{
\def ARM_ETH_PHY_SPEED_10M 
\sa ARM_ETH_PHY_SetMode
\def ARM_ETH_PHY_SPEED_100M
\sa ARM_ETH_PHY_SetMode
\def ARM_ETH_PHY_SPEED_1G  
\sa ARM_ETH_PHY_SetMode
\def ARM_ETH_PHY_DUPLEX_HALF
\sa ARM_ETH_PHY_SetMode
\def ARM_ETH_PHY_DUPLEX_FULL
\sa ARM_ETH_PHY_SetMode
\def ARM_ETH_PHY_AUTO_NEGOTIATE
\sa ARM_ETH_PHY_SetMode
\def ARM_ETH_PHY_LOOPBACK      
\sa ARM_ETH_PHY_SetMode
\def ARM_ETH_PHY_ISOLATE       
\sa ARM_ETH_PHY_SetMode
@}
*/



