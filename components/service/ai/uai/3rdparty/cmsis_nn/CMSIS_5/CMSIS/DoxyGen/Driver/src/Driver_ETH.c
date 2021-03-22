/**
\defgroup eth_interface_gr Ethernet Interface
\brief    Ethernet common definitions (%Driver_ETH.h)

\details
<b>Ethernet</b> is a networking technology for exchanging data packages between computer systems. Several microcontrollers integrate 
an Ethernet MAC (Media Access Control) data-link layer that interfaces to an Ethernet PHY (Physical Interface Transceiver).

Wikipedia offers more information about 
the <a href="http://en.wikipedia.org/wiki/Ethernet" target="_blank"><b>Ethernet</b></a>.


<b>Block Diagram</b>

The Ethernet PHY connects typically to the Ethernet MAC using an MII (Media Independent Interface) or RMII (Reduced Media Independent Interface).

\n
\image html EthernetSchematic.png  "Block Diagram of a typical Ethernet Interface"


<b>Ethernet API</b>

The following header files define the Application Programming Interface (API) for the <b>Ethernet</b> interface:
 - \b %Driver_ETH.h : Common definitions of the Ethernet PHY and MAC part
 - \b %Driver_ETH_MAC.h : API for the Ethernet MAC
 - \b %Driver_ETH_PHY.h : API for the Ethernet PHY
 
The driver implementation of the Ethernet MAC is a typical part of a Device Family Pack (DFP) that supports the peripherals of the microcontroller family.
The driver implementation of the Ethernet PHY is a typical part of a \b Network Software Pack, since PHY is typically not integrated into the microcontroller.

\note
For parameters, the value marked with (default) is the setting after the driver initialization.


<b>Driver Functions</b>

The driver functions are published in the access struct as explained in \ref DriverFunctions
 - \ref ARM_DRIVER_ETH_MAC : access struct for <b>Ethernet MAC</b> driver functions.
 - \ref ARM_DRIVER_ETH_PHY : access struct for <b>Ethernet PHY</b> driver functions.

Both drivers are used in combination and usually the Ethernet MAC provides a media interface to the Ethernet PHY.
A typical setup sequence for the drivers is shown below:

<b>Example Code</b>

The following example code shows the usage of the Ethernet interface.

\code
extern ARM_DRIVER_ETH_MAC Driver_ETH_MAC0;
extern ARM_DRIVER_ETH_PHY Driver_ETH_PHY0;
 
static ARM_DRIVER_ETH_MAC *mac;
static ARM_DRIVER_ETH_PHY *phy;
static ARM_ETH_MAC_ADDR          own_mac_address;
static ARM_ETH_MAC_CAPABILITIES  capabilities;
 
void ethernet_mac_notify (uint32_t event)  {
  switch (event)  {
     :
  }  
}
 
 
void initialize_ethernet_interface (void) {
  mac = &Driver_ETH_MAC0;
  phy = &Driver_ETH_PHY0;
 
  // Initialize Media Access Controller
  capabilities = mac->GetCapabilities ();
   
  mac->Initialize (ethernet_mac_notify);
  mac->PowerControl (ARM_POWER_FULL);
 
  if (capabilities.mac_address == 0)  {
    // populate own_mac_address with the address to use
    mac->SetMacAddress(&own_mac_address);
  }
  else {
    mac->GetMacAddress(&own_mac_address);
  }
 
  // Initialize Physical Media Interface
  if (phy->Initialize (mac->PHY_Read, mac->PHY_Write) == ARM_DRIVER_OK) {
    phy->PowerControl (ARM_POWER_FULL);
    phy->SetInterface (capabilities.media_interface);
    phy->SetMode (ARM_ETH_PHY_AUTO_NEGOTIATE);
  }
    :
    :
}
 
 
static ARM_ETH_LINK_STATE ethernet_link;   // current link status
 
void ethernet_check_link_status (void) {
  ARM_ETH_LINK_STATE link;
 
  link = phy->GetLinkState ();
  if (link == ethernet_link) {    
    return;                                // link state unchanged
  }
                                           // link state changed
  ethernet_link = link;   
  if (link == ARM_ETH_LINK_UP) {      // start transfer
    ARM_ETH_LINK_INFO info = phy->GetLinkInfo ();
    mac->Control(ARM_ETH_MAC_CONFIGURE,
                 info.speed  << ARM_ETH_MAC_SPEED_Pos  |
                 info.duplex << ARM_ETH_MAC_DUPLEX_Pos |
                 ARM_ETH_MAC_ADDRESS_BROADCAST);
    mac->Control(ARM_ETH_MAC_CONTROL_TX, 1);
    mac->Control(ARM_ETH_MAC_CONTROL_RX, 1);
  }
  else {                                   // stop transfer
    mac->Control(ARM_ETH_MAC_FLUSH, ARM_ETH_MAC_FLUSH_TX | ARM_ETH_MAC_FLUSH_RX);
    mac->Control(ARM_ETH_MAC_CONTROL_TX, 0);
    mac->Control(ARM_ETH_MAC_CONTROL_RX, 0);
  }
}

\endcode
*/

/**
\defgroup eth_interface_gr Ethernet Interface
@{
*/

/**
\cond
*/

/**
\enum ARM_ETH_INTERFACE
\details
Encodes the supported media interface between Ethernet MAC and Ethernet PHY. 

The function \ref ARM_ETH_MAC_GetCapabilities retrieves the media interface type encoded in the data field \b media_interface of the struct
 \ref ARM_ETH_MAC_CAPABILITIES. 

<b>Parameter for:</b>
- \ref ARM_ETH_PHY_SetInterface
*/

/**
\enum ARM_ETH_DUPLEX
\details 
Lists the supported duplex operating types for MAC.

<b>Parameter for:</b>
  - \ref ARM_ETH_MAC_SetMode
*/


/**
\typedef ARM_ETH_SPEED
\details 
Lists the supported operating speeds for MAC.

<b>Parameter for:</b>
  - \ref ARM_ETH_MAC_SetMode
*/

/**
\endcond
*/


/**
\typedef ARM_ETH_LINK_STATE
\details 
The Ethernet Link status shows if the communication is currently established (up) or interrupted (down).

<b>Returned by:</b>
  - \ref ARM_ETH_PHY_GetLinkState
*/


/**
\struct ARM_ETH_LINK_INFO
\details
The Ethernet Link information provides parameters about the current established communication.

<b>Returned by:</b>
  - \ref ARM_ETH_PHY_GetLinkInfo
*/


/**
\struct ARM_ETH_MAC_ADDR
\details
Stores the MAC Address of the Ethernet interface as defined by IEEE 802. Wikipedia offers more information about 
the <a href="http://en.wikipedia.org/wiki/MAC_address" target="_blank"><b>MAC Address</b></a>.

<b>Parameter for:</b>
  - \ref ARM_ETH_MAC_GetMacAddress, \ref ARM_ETH_MAC_SetMacAddress, \ref ARM_ETH_MAC_SetAddressFilter
*/

/**
@}
*/ 
// End ETH Interface


/**
\defgroup eth_interface_types1 Media Interface Types
\ingroup eth_interface_gr
\brief Ethernet Media Interface type
\details
Encodes the supported media interface between Ethernet MAC and Ethernet PHY. 
The function \ref ARM_ETH_MAC_GetCapabilities retrieves the media interface type encoded in the data field \b media_interface of the struct
 \ref ARM_ETH_MAC_CAPABILITIES. 

<b>Parameter for:</b>
- \ref ARM_ETH_PHY_SetInterface

@{
\def ARM_ETH_INTERFACE_MII 
\sa ARM_ETH_PHY_SetInterface
\def ARM_ETH_INTERFACE_RMII
\sa ARM_ETH_PHY_SetInterface
\def ARM_ETH_INTERFACE_SMII
\sa ARM_ETH_PHY_SetInterface
@}
*/
