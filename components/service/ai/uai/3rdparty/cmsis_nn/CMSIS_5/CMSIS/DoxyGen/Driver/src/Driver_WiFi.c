/**
\defgroup wifi_interface_gr WiFi Interface
\brief Driver API for WiFi (%Driver_WiFi.h)
\details 

Wi-Fi is technology for radio wireless local area networking of devices. Wi-Fi compatible devices typically
connect to the Internet via a WLAN and a wireless access point (AP) also called hotspot.

Wikipedia offers more information about 
the <a href="http://en.wikipedia.org/wiki/Ethernet" target="_blank"><b>WiFi</b></a>.

<b>Driver Block Diagram</b>

\image html WiFi.png  "Block Diagram of the WiFi interface"

<b>WiFi API</b>

The following header files define the Application Programming Interface (API) for the WiFi interface:
  - \b %Driver_WiFi.h : Driver API for WiFi

The CMSIS-Driver WiFi provides access to the following interfaces:

 - \ref wifi_control_gr "Control interface": setup and control the WiFi module.
 - \ref wifi_management_gr "Management interface": allows you to configure and manage the connection
   to the WiFi access point (AP) or configure and manage the access point (AP).
 - \ref wifi_socket_gr "Socket interface": provides the interface to an IP stack that is running
   on the WiFi module. This IP stack handles data communication.
 - \ref wifi_bypass_gr "Bypass interface": is an optional interface and enables the transmission of
   Ethernet frames with the WiFi module. Using this interface requires the IP stack running on the microcontroller.

The WiFi interface usually requires CMSIS-RTOS features (i.e. mutex) and is often implemented
with a peripheral device that is connected to the system using the SPI or UART interface. However,
there are also some microcontroller devices with WiFi interface on the chip.

The implementation of the WiFi CMSIS-Driver is therefore generally provided as a separate software pack.
It is often implemented as wrapper to the SDK (Software Development Kit) of the WiFi chipset.


<b>Driver Functions</b>

The driver functions are published in the access struct as explained in \ref DriverFunctions
  - \ref ARM_DRIVER_WIFI : access struct for WiFi driver functions


<b>Example Code</b>

@{
*/

/**
\struct  ARM_DRIVER_WIFI
\details 
The functions of the WiFi driver are accessed by function pointers exposed by this structure.
Refer to \ref DriverFunctions for overview information.

Each instance of a WiFi interface provides such an access structure. 
The instance is identified by a postfix number in the symbol name of the access structure, for example:
 - \b Driver_WiFi0 is the name of the access struct of the first instance (no. \token{0}).
 - \b Driver_WiFi1 is the name of the access struct of the second instance (no. \token{1}).

A middleware configuration setting allows connecting the middleware to a specific driver instance \b %Driver_WiFi<i>n</i>.
The default is \token{0}, which connects a middleware to the first instance of a driver.
*******************************************************************************************************************/


/**
\defgroup wifi_control_gr WiFi Control
\ingroup wifi_interface_gr
\brief Control functions for the WiFi module
\details  
The \ref wifi_control_gr functions setup and control the WiFi module.
@{
*/

/** 
\struct  ARM_WIFI_CAPABILITIES
\details
A WiFi driver can be implemented with different capabilities.
The data fields of this structure encode the capabilities implemented by this driver.

<b>Returned by:</b>
  - \ref ARM_WIFI_GetCapabilities
*******************************************************************************************************************/

/**
\typedef ARM_WIFI_SignalEvent_t
\details
Provides the typedef for the callback function \ref ARM_WIFI_SignalEvent.

<b>Parameter for:</b>
  - \ref ARM_WIFI_Initialize
*******************************************************************************************************************/

/**
\defgroup wifi_event WiFi Events
\ingroup wifi_control_gr
\brief The WiFi driver generates call back events that are notified via the function \ref ARM_WIFI_SignalEvent.
\details The following call back notification events are generated:
@{
\def ARM_WIFI_EVENT_AP_CONNECT
\def ARM_WIFI_EVENT_AP_DISCONNECT
\def ARM_WIFI_EVENT_ETH_RX_FRAME
@}
*/

ARM_DRIVER_VERSION ARM_WIFI_GetVersion (void) {
  return { 0, 0 };
}
/**
\fn ARM_DRIVER_VERSION ARM_WIFI_GetVersion (void)
\details
The function \b ARM_WIFI_GetVersion returns version information of the driver implementation in \ref ARM_DRIVER_VERSION.

API version is the version of the CMSIS-Driver specification used to implement this driver.
Driver version is source code version of the actual driver implementation.

\b Example:
\code
extern ARM_DRIVER_WIFI Driver_WiFi0;
static ARM_DRIVER_WIFI *wifi;
 
void get_wifi_version (void)  {
  ARM_DRIVER_VERSION version;
 
  wifi= &Driver_WiFi0;  
  version = wifi->GetVersion ();
  if (version.api < 0x100U) {        // requires at minimum API version 1.0 or higher
    // error handling
    return;
  }
}
\endcode
*/

ARM_WIFI_CAPABILITIES ARM_WIFI_GetCapabilities (void) {
  return { 0 };
}
/**
\fn ARM_WIFI_CAPABILITIES ARM_WIFI_GetCapabilities (void)
\details
The function \b ARM_WIFI_GetCapabilities retrieves information about capabilities in this driver implementation.
The data fields of the struct \ref ARM_WIFI_CAPABILITIES encode various capabilities, for example
if a WiFi module supports the Access Point mode or the bypass mode, or is capable to signal events using
the \ref ARM_WIFI_SignalEvent callback function.

\b Example:
\code
extern ARM_DRIVER_WIFI Driver_WiFi0;
static ARM_DRIVER_WIFI *wifi;
  
void get_wifi_capabilities (void)  {
  ARM_WIFI_CAPABILITIES capabilities;
 
  wifi = &Driver_WiFi0;  
  capabilities = wifi->GetCapabilities ();
  // interrogate capabilities
   :
}
\endcode
*/

int32_t ARM_WIFI_Initialize (ARM_WIFI_SignalEvent_t cb_event) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_WIFI_Initialize (ARM_WIFI_SignalEvent_t cb_event)
\details
The function \b ARM_WIFI_Initialize initializes the WiFi module.

It is called when the middleware component starts operation.

The \ref ARM_WIFI_Initialize function performs the following operations:
  - Initializes the resources and peripherals required for the WiFi module.
  - Registers the \ref ARM_WIFI_SignalEvent callback function.

The parameter \em cb_event is a pointer to the \ref ARM_WIFI_SignalEvent callback function;
use a \token{NULL} pointer when no callback signals are required.

\b Example:
\code
extern ARM_DRIVER_WIFI Driver_WiFi0;
static ARM_DRIVER_WIFI *wifi;
static ARM_ETH_MAC_ADDR own_mac_address;
 
void initialize_wifi (void) {
  wifi = &Driver_WiFi0;
 
  // Initialize and Power-on WiFi Module
  wifi->Initialize (NULL);
  wifi->PowerControl (ARM_POWER_FULL);
 
  // Populate own_mac_address with the address to use
  wifi->SetOption(ARM_WIFI_MAC, &own_mac_address, 6U);
}
\endcode
*/

int32_t ARM_WIFI_Uninitialize (void) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_WIFI_Uninitialize (void)
\details
The function \b ARM_WIFI_Uninitialize de-initializes the resources of the WiFi module.

It is called when the middleware component stops operation and releases the software resources 
used by the module.

\b Example:
\code
extern ARM_DRIVER_WIFI Driver_WiFi0;
static ARM_DRIVER_WIFI *wifi;
 
void uninitialize_wifi (void) {
  wifi = &Driver_WiFi0;
 
  // Power off and De-initialize WiFi Module
  wifi->PowerControl (ARM_POWER_OFF);
  wifi->Uninitialize ();
}
\endcode
*/

int32_t ARM_WIFI_PowerControl (ARM_POWER_STATE state) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_WIFI_PowerControl (ARM_POWER_STATE state)
\details
The function \b ARM_WIFI_PowerControl allows you to configure the power modes of the WiFi module.

The parameter \em state specifies the \ref ARM_POWER_STATE.

Low-power mode depends on additional options set by \ref ARM_WIFI_SetOption :
 - Deep-sleep mode is entered when \ref ARM_WIFI_LP_TIMER option is set to a value different than 0
 - Sleep mode is entered otherwise

\b Deep-sleep mode (only for station):
Module turns off the radio and also internal CPU thus reducing power consumption to minimum,
only the timer is running that wakes-up the module after specified time.
When timer expires the module reconnects to the access point.

This mode is used when power consumption is a priority (battery powered devices) and when WiFi
is used in short intervals that do not occur very often
(example: sending a temperature from a sensor to a cloud every 10 seconds).

\b Sleep mode (only for station):
Module reduces power consumption by going into sleep and waking up periodically to listen for beacons.

Delivery Traffic Indication Message (DTIM) interval can be configured with option \ref ARM_WIFI_DTIM
(station and access point) and beacon interval with option \ref ARM_WIFI_BEACON (only for access point).

Default module intervals are used when those options are not explicitly set.

If power \em state specifies an unsupported mode, the function returns \ref ARM_DRIVER_ERROR_UNSUPPORTED as
status information and the previous power state of the peripheral is unchanged. Multiple calls with the same
\em state generate no error.

\b Example:
 - see \ref ARM_WIFI_Initialize
 - see \ref ARM_WIFI_Uninitialize
*/

int32_t ARM_WIFI_GetModuleInfo (char *module_info, uint32_t max_len) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_WIFI_GetModuleInfo (char *module_info, uint32_t max_len)
\details
The function \b ARM_WIFI_GetModuleInfo retrieves string containing information about the WiFi module.

The information might include module name, firmware version, ...

\note Module must be initialized and powered before module information can be retrieved.

\b Example:
\code
extern ARM_DRIVER_WIFI Driver_WiFi0;
static ARM_DRIVER_WIFI *wifi;
 
void initialize_wifi (void) {
  char info[32];
 
  wifi = &Driver_WiFi0;
 
  // Initialize and Power-on WiFi Module
  wifi->Initialize (NULL);
  wifi->PowerControl (ARM_POWER_FULL);
 
  // Retrieve module information
  wifi->GetModuleInfo(&info, sizeof(info));
}
\endcode
*/

void ARM_WIFI_SignalEvent (uint32_t event, void *arg) {
}
/**
\fn void ARM_WIFI_SignalEvent (uint32_t event, void *arg)
\details
The function \b ARM_WIFI_SignalEvent is a callback function registered by the function \ref ARM_WIFI_Initialize.
It is called by the WiFi driver to notify the application about WiFi Events occurred during operation.

The parameter \em event indicates the event that occurred during driver operation.

The parameter \em arg provides additional information about the event.

The following events can be generated:

Parameter \em event                  | Description
:------------------------------------|:------------------------------------------
\ref ARM_WIFI_EVENT_AP_CONNECT       | Occurs in access point mode when a station has connected to the access point.
\ref ARM_WIFI_EVENT_AP_DISCONNECT    | Occurs in access point mode when a station has disconnected from the access point.
\ref ARM_WIFI_EVENT_ETH_RX_FRAME     | Occurs in \ref wifi_bypass_gr when an ethernet frame is received.
*/

/**
@}
*/
// end group wifi_control_gr


/**
\defgroup wifi_management_gr WiFi Management
\ingroup wifi_interface_gr
\brief Configure and manage the connection to a WiFi access point (AP) or configure and manage the access point (AP).
\details The \ref wifi_management_gr functions are used to configure and manage the connection to a WiFi access point (AP)
also called hotspot when in station mode. They are also used to configure and manage the access point (AP) itself
when in access point mode.
@{
*/

/**
\defgroup WiFi_option WiFi Option Codes
\ingroup wifi_management_gr
\brief  WiFi Option Codes for \ref ARM_WIFI_SetOption or \ref ARM_WIFI_GetOption function.
\details 
Many parameters of the WiFi module are configured using the \ref ARM_WIFI_SetOption or \ref ARM_WIFI_GetOption function.
@{
\def ARM_WIFI_BSSID
\details Specifies the BSSID of the access point to connect or the access point itself.
\sa WiFi_option
\def ARM_WIFI_TX_POWER
\details Specifies the transmit power in dBm.
\sa WiFi_option
\def ARM_WIFI_LP_TIMER
\details Specifies the low-power deep-sleep time in seconds for station (disabled when 0 - default).
\sa WiFi_option
\def ARM_WIFI_DTIM
\details Specifies the DTIM interval in number of beacons.
\sa WiFi_option
\def ARM_WIFI_BEACON
\details Specifies the beacon interval in milliseconds for access point.
\sa WiFi_option
\def ARM_WIFI_MAC
\details Specifies the MAC address.
\sa WiFi_option
\def ARM_WIFI_IP
\details Specifies the IP address.
\sa WiFi_option
\def ARM_WIFI_IP_SUBNET_MASK
\details Specifies the subnet mask.
\sa WiFi_option
\def ARM_WIFI_IP_GATEWAY
\details Specifies the gateway IP address.
\sa WiFi_option
\def ARM_WIFI_IP_DNS1
\details Specifies the IP address of the primary DNS server.
\sa WiFi_option
\def ARM_WIFI_IP_DNS2
\details Specifies the IP address of the secondary DNS server.
\sa WiFi_option
\def ARM_WIFI_IP_DHCP
\details Enables or disables the DHCP client for station or DHCP server for access point.
\sa WiFi_option
\def ARM_WIFI_IP_DHCP_POOL_BEGIN
\details Specifies the start IP address for DHCP server (access point).
\sa WiFi_option
\def ARM_WIFI_IP_DHCP_POOL_END
\details Specifies the end IP address for DHCP server (access point).
\sa WiFi_option
\def ARM_WIFI_IP_DHCP_LEASE_TIME
\details Specifies the lease time for DHCP server (access point).
\sa WiFi_option
\def ARM_WIFI_IP6_GLOBAL
\details Specifies the global IPv6 address.
\sa WiFi_option
\def ARM_WIFI_IP6_LINK_LOCAL
\details Specifies the link-local IPv6 address.
\sa WiFi_option
\def ARM_WIFI_IP6_SUBNET_PREFIX_LEN
\details Specifies the address prefix length.
\sa WiFi_option
\def ARM_WIFI_IP6_GATEWAY
\details Specifies the gateway IPv6 address.
\sa WiFi_option
\def ARM_WIFI_IP6_DNS1
\details Specifies the IPv6 address of the primary DNS server.
\sa WiFi_option
\def ARM_WIFI_IP6_DNS2
\details Specifies the IPv6 address of the secondary DNS server.
\sa WiFi_option
\def ARM_WIFI_IP6_DHCP_MODE
\details Specifies the operation mode of the DHCPv6 client.
\sa WiFi_option
@}
*/

/**
\defgroup wifi_sec_type WiFi Security Type
\ingroup wifi_management_gr
\brief Specifies WiFi security type for \ref ARM_WIFI_Activate.
\details
The WiFi security type defines the standard used to protect the wireless network from unauthorized access.
@{
\def ARM_WIFI_SECURITY_OPEN
\details This is an open system which provides \b no security.
\sa wifi_sec_type
\def ARM_WIFI_SECURITY_WEP
\details This security standard provides \b weak level of security.
\sa wifi_sec_type
\def ARM_WIFI_SECURITY_WPA
\details This security standard provides \b medium level of security.
\sa wifi_sec_type
\def ARM_WIFI_SECURITY_WPA2
\details This security standard provides \b strong level of security.
\sa wifi_sec_type
\def ARM_WIFI_SECURITY_UNKNOWN
\details Unknown security standard (reported by \ref ARM_WIFI_Scan).
\sa wifi_sec_type
@}
*/

/**
\defgroup wifi_wps_method WiFi Protected Setup (WPS) Method
\ingroup wifi_management_gr
\brief Specifies WiFi WPS method for \ref ARM_WIFI_Activate.
\details
The WiFi WPS method defines which WPS method is used.
@{
\def ARM_WIFI_WPS_METHOD_NONE
\details WPS not used.
\sa wifi_wps_method
\def ARM_WIFI_WPS_METHOD_PBC
\details WPS with Push Button Configuration.
\sa wifi_wps_method
\def ARM_WIFI_WPS_METHOD_PIN
\details WPS with PIN.
\sa wifi_wps_method
@}
*/

/**
\defgroup wifi_dhcp_v6_mode WiFi DHCPv6 Mode
\ingroup wifi_management_gr
\brief Specifies IPv6 Dynamic Host Configuration Protocol (DHCP) Mode.
\details
The WiFi DHCPv6 mode defines the DHCP mode in IPv6.
@{
\def ARM_WIFI_IP6_DHCP_OFF
\details
In the static host configuration mode, the IPv6 address, the default gateway address,
and the addresses of DNS servers are statically configured from the preset values.
\sa wifi_dhcp_v6_mode
\def ARM_WIFI_IP6_DHCP_STATELESS
\details
In the stateless DHCP configuration mode, the client obtains only extended information
from a DHCPv6 server, such as DNS server addresses. Stateless auto-configuration of
IPv6 allows the client device to self configure it's IPv6 addresses and routing based
on the router advertisements.
\sa wifi_dhcp_v6_mode
\def ARM_WIFI_IP6_DHCP_STATEFULL
\details
In the stateful DHCP configuration mode, the client connects to a DHCPv6 server for
a leased IPv6 address and DNS server addresses.
\sa wifi_dhcp_v6_mode
@}
*/

/**
\struct  ARM_WIFI_CONFIG_t
\details
Provides information needed to connect to the WiFi network for station or how to configure the access point (AP).

<b>Used in:</b>
  - \ref ARM_WIFI_Activate
*******************************************************************************************************************/

/**
\struct  ARM_WIFI_SCAN_INFO_t
\details
Provides information about the wireless networks that were detected when searching for available WiFi networks. The structure
contains the information needed to connect to the WiFi network. Of course, the access password is not included and must
be provided separately.

<b>Used in:</b>
  - \ref ARM_WIFI_Scan
*******************************************************************************************************************/

/**
\struct  ARM_WIFI_NET_INFO_t
\details
Provides information about the network that the station is connected to.

<b>Used in:</b>
  - \ref ARM_WIFI_GetNetInfo
*******************************************************************************************************************/

int32_t ARM_WIFI_SetOption (uint32_t interface, uint32_t option, const void *data, uint32_t len) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_WIFI_SetOption (uint32_t interface, uint32_t option, const void *data, uint32_t len)
\details
The function \b ARM_WIFI_SetOption sets the value of the specified option of the WiFi module.

The argument \em interface specifies the interface (0 = Station, 1 = Access Point).

The argument \em option specifies the option that is to be set (see below).

The argument \em data points to a buffer containing the value of the option to be set
and must be aligned to the data type of the corresponding option.

The argument \em len specifies the length of the buffer \em data and must be equal (or higher)
to the length of the corresponding option.
 
Option                                 | Description                            | Data          | Type/Length
:--------------------------------------|:---------------------------------------|:--------------|:-----------
\ref ARM_WIFI_BSSID                    | BSSID of AP to connect or AP           | bssid         | uint8_t[6]
\ref ARM_WIFI_TX_POWER                 | Transmit power                         | power[dbm]    | uint32_t
\ref ARM_WIFI_LP_TIMER                 | Low-power deep-sleep time              | time[seconds] | uint32_t
\ref ARM_WIFI_DTIM                     | DTIM interval                          | dtim[beacons] | uint32_t
\ref ARM_WIFI_BEACON                   | Beacon interval                        | interval[ms]  | uint32_t
\ref ARM_WIFI_MAC                      | MAC address                            | mac           | uint8_t[6]
\ref ARM_WIFI_IP                       | IPv4 address                           | ip            | uint8_t[4]
\ref ARM_WIFI_IP_SUBNET_MASK           | IPv4 subnet mask                       | mask          | uint8_t[4]
\ref ARM_WIFI_IP_GATEWAY               | IPv4 gateway address                   | ip            | uint8_t[4]
\ref ARM_WIFI_IP_DNS1                  | IPv4 primary DNS server address        | ip            | uint8_t[4]
\ref ARM_WIFI_IP_DNS2                  | IPv4 secondary DNS server address      | ip            | uint8_t[4]
\ref ARM_WIFI_IP_DHCP                  | IPv4 DHCP client/server enable/disable | dhcp (0, 1)   | uint32_t
\ref ARM_WIFI_IP_DHCP_POOL_BEGIN       | IPv4 DHCP server begin address         | ip            | uint8_t[4]
\ref ARM_WIFI_IP_DHCP_POOL_END         | IPv4 DHCP server end address           | ip            | uint8_t[4]
\ref ARM_WIFI_IP_DHCP_LEASE_TIME       | IPv4 DHCP server lease time            | time[seconds] | uint32_t
\ref ARM_WIFI_IP6_GLOBAL               | IPv6 global address                    | ip6           | uint8_t[16]
\ref ARM_WIFI_IP6_LINK_LOCAL           | IPv6 link-local address                | ip6           | uint8_t[16]
\ref ARM_WIFI_IP6_SUBNET_PREFIX_LEN    | IPv6 subnet prefix length              | len (1..127)  | uint32_t
\ref ARM_WIFI_IP6_GATEWAY              | IPv6 gateway address                   | ip6           | uint8_t[16]
\ref ARM_WIFI_IP6_DNS1                 | IPv6 primary DNS server address        | ip6           | uint8_t[16]
\ref ARM_WIFI_IP6_DNS2                 | IPv6 secondary DNS server address      | ip6           | uint8_t[16]
\ref ARM_WIFI_IP6_DHCP_MODE            | IPv6 DHCP client mode                  | mode          | uint32_t

\b Example:
\code
uint8_t ip[4];
 
ip[0] = 192U;
ip[1] = 168U;
ip[2] = 0U;
ip[3] = 1U;
 
// Set IP static address of the Station
wifi->SetOption (0U, ARM_WIFI_IP, &ip, sizeof(ip));
\endcode
*/

int32_t ARM_WIFI_GetOption (uint32_t interface, uint32_t option, void *data, uint32_t *len) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_WIFI_GetOption (uint32_t interface, uint32_t option, void *data, uint32_t *len)
\details
The function \b ARM_WIFI_GetOption retrieves the current value of the specified option of
the WiFi module.

The argument \em interface specifies the interface (0 = Station, 1 = Access Point).

The argument \em option specifies the option that is to be retrieved (see \ref ARM_WIFI_SetOption).

The argument \em data points to a buffer that will be used to store the value of 
the \em option and must be aligned to the data type of the corresponding option.

The argument \em len is a pointer to the length of the buffer at input and returns the length
of the option information on the output.

\b Example:
\code
uint8_t ip[4];          // IP address
uint8_t mask[4];        // Subnet mask
uint8_t gateway[4];     // Gateway address
 
// Get IP address, Subnet mask and Gateway address of the Station
wifi->GetOption (0U, ARM_WIFI_IP, &ip, sizeof(ip));
wifi->GetOption (0U, ARM_WIFI_IP_SUBNET_MASK, &mask, sizeof(mask));
wifi->GetOption (0U, ARM_WIFI_IP_GATEWAY, &gateway, sizeof(gateway));
\endcode
*/

int32_t ARM_WIFI_Scan (ARM_WIFI_SCAN_INFO_t scan_info[], uint32_t max_num) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_WIFI_Scan (ARM_WIFI_SCAN_INFO_t scan_info[], uint32_t max_num)
\details
The function \b ARM_WIFI_Scan searches for available WiFi networks. Using this function,
you can determine which wireless networks are available for the connection. If the network is
secured, you must also know the password to connect.
 
The argument \em scan_info is a pointer to an array of network information structures, where 
the available network information will be returned.

The argument \em max_num specifies maximum number of network information structures,
that can be stored to the \em scan_info.

\b Example:
\code
ARM_WIFI_SCAN_INFO_t scan_info[8];
 
num = wifi->Scan (scan_info, 8U);
 
// Print SSIDs of available WiFi networks
for (i = 0; i < num; i++) {
  printf ("%d. ssid=%s\n", i, scan_info[i].ssid);
}
\endcode
*/

int32_t ARM_WIFI_Activate (uint32_t interface, ARM_WIFI_CONFIG_t *config) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_WIFI_Activate (uint32_t interface, ARM_WIFI_CONFIG_t *config)
\details
The function \b ARM_WIFI_Activate activates the specified interface.

The argument \em interface specifies the interface (0 = Station, 1 = Access Point).

When station interface is specified, the WiFi module connects to a wireless network.

The wireless network trying to connect to must be available,
otherwise the operation will fail after a timeout.

Available wireless networks can be scanned by using the function \ref ARM_WIFI_Scan.

When access point interface is specified, the WiFi module creates a wireless network
by activating the access point.

The argument \em config is a pointer to the configuration \ref ARM_WIFI_CONFIG_t
which provides information needed to connect to a WiFi network for station interface
or information used to configure the access point (AP) for access point interface.

\em ssid specifies the name of the network to connect to or the network to create.

\em pass specifies the password for accessing the wireless network.

\em security specifies the security type which will be used for the connection.

\em ch specifies the WiFi channel which will be used for the connection.
Valid channels for 2.4 GHz frequency are from \token{1} to \token{13}. If the value for \em ch = \token{0},
the system automatically selects the channel.
For station interface the channel of the AP being connected to is used.
For access point interface the module automatically selects the best channel for the WiFi connection.

\note
Optionally BSSID parameter can be also set using \ref ARM_WIFI_SetOption.
It allows connection to specific BSSID when connecting to an access point or specifies
the BSSID of the access point.

\em wps_method specifies if WiFi Protected Setup (WPS) is used and which method.

\em wps_pin specifies the PIN used with WPS (\ref ARM_WIFI_WPS_METHOD_PIN).

With the \b push-button method, you typically press the button, either real or virtual,
both at the access point and the station. No credentials are needed.

With \b PIN method, you must provide the PIN code that you read from the label or screen
on the wireless device.

WPS configuration for station is used when station connects to an access point.
It enables to connect without specifying SSID, Password, Security Type or WiFi Channel.
The actual network information can be retrieved once connected with \ref ARM_WIFI_GetNetInfo.

WPS configuration for access point is used when access point is activated.
Subsequent activate calls re-trigger the WPS procedure.

\note
WPS is typically activated by pressing the WPS button at the access point.
During the discovery mode (usually 2 minutes or less) any wireless device may connect
to the access point (PIN needs to match when PIN method is selected).

\b Example:
\code
ARM_WIFI_CONFIG_t wifi_config;
 
wifi_config.ssid = "GuestAccess";
wifi_config.pass = "guest";
wifi_config.security = ARM_WIFI_SECURITY_WPA2;
wifi_config.ch = 0U;
wifi_config.wps_method = ARM_WIFI_WPS_METHOD_NONE;
 
// Connect to wireless network
status = wifi->Activate (0U, &wifi_config);
if (status != ARM_DRIVER_OK) {
  // error handling
}
\endcode
*/

int32_t ARM_WIFI_Deactivate (uint32_t interface) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_WIFI_Deactivate (uint32_t interface)
\details
The function \b ARM_WIFI_Deactivate deactivates the specified interface.

The argument \em interface specifies the interface (0 = Station, 1 = Access Point).

When station interface is specified, the WiFi module disconnects from the wireless network.

When access point interface is specified, the WiFi module deactivates the access point.

\b Example:
 - see \ref ARM_WIFI_GetNetInfo
*/

uint32_t ARM_WIFI_IsConnected (void) {
  return 0;
}
/**
\fn uint32_t ARM_WIFI_IsConnected (void)
\details
The function \b ARM_WIFI_IsConnected checks if the station is connected to a wireless network
and returns the connection status.

The function returns a \token{non-zero} value, if the station is connected. If the station
is not connected, the function returns \token{0}.

\b Example:
 - see \ref ARM_WIFI_GetNetInfo
*/

int32_t ARM_WIFI_GetNetInfo (ARM_WIFI_NET_INFO_t *net_info) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_WIFI_GetNetInfo (ARM_WIFI_NET_INFO_t *net_info)
\details
The function \b ARM_WIFI_GetNetInfo retrieves wireless network information of a connected station.

It can be used to retrieve network connection information for subsequent connections
after initially connecting using WPS.

\b Example:
\code
ARM_WIFI_CONFIG_t wifi_config;
ARM_WIFI_NET_INFO_t net_info;
 
memset(&wifi_config, 0, sizeof(wifi_config));
 
wifi_config.wps_method = ARM_WIFI_WPS_METHOD_PBC;
 
// Connect to wireless network (WPS)
status = wifi->Activate (0U, &wifi_config);
if (status != ARM_DRIVER_OK) {
  // error handling
}
 
// Retrieve network information
if (wifi->IsConnected ()) {
  status = wifi->GetNetInfo (&net_info);
  if (status != ARM_DRIVER_OK) {
    // error handling
  }
  printf("SSID=%s, Password=%s",net_info.ssid, net_info.pass);
}
 
// Disconnect from wireless network
wifi->Deactivate (0U);
\endcode
*/

/**
@}
*/
// end group wifi_management_gr


/**
\defgroup wifi_bypass_gr WiFi Bypass Mode
\ingroup wifi_interface_gr
\brief Transfer Ethernet frames by WiFi module.
\details The \ref wifi_bypass_gr functions are an optional interface and enable the transmission of
Ethernet frames with WiFi modules. The use of this interface requires that the IP stack is running
on the microcontroller. The internal IP stack of the WiFi module is therefore not used, and this
usually means that the \ref wifi_socket_gr functions can not be used.
@{
*/

int32_t ARM_WIFI_BypassControl (uint32_t interface, uint32_t mode) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_WIFI_BypassControl (uint32_t interface, uint32_t mode)
\details
The function \b ARM_WIFI_BypassControl enables or disables the WiFi bypass mode.

The WiFi Bypass mode can only be enabled, if there is a bypass mode supported in the WiFi driver.
You can check this by checking the driver's capabilities.

\note
Bypass mode is enabled by default if the module does not support the Socket interface.

The argument \em mode specifies the desired state of the WiFi Bypass mode, which is
enabled or disabled.
 
\b Example:
\code
extern ARM_DRIVER_WIFI Driver_WiFi0;
static ARM_DRIVER_WIFI *wifi;
static ARM_ETH_MAC_ADDR own_mac_address;
 
static void wifi_notify (uint32_t event, ,void *arg) {
  switch (event)  {
     :
  }  
}
 
void initialize_wifi_bypass (void) {
  ARM_WIFI_CAPABILITIES capabilities;
 
  wifi = &Driver_WiFi0;
  capabilities = wifi->GetCapabilities ();
  if (capabilities.bypass_mode == 0) {
    // error handling 
  } 
 
  // Initialize and Power-on WiFi Interface
  wifi->Initialize ((capabilities.eth_rx_frame_event) ? wifi_notify : NULL);
  wifi->PowerControl (ARM_POWER_FULL);
 
  // populate own_mac_address with the address to use for station
  wifi->SetOption(0U, ARM_WIFI_MAC, &own_mac_address, 6U);
 
  wifi->BypassControl (0U, 1U); // Enable bypass mode for station
}
\endcode
*/

int32_t ARM_WIFI_EthSendFrame (uint32_t interface, const uint8_t *frame, uint32_t len) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_WIFI_EthSendFrame (uint32_t interface, const uint8_t *frame, uint32_t len)
\details
The function \b ARM_WIFI_EthSendFrame writes an <b>Ethernet frame</b> to the WiFi transmit buffer.

The WiFi bypass mode must be enabled by using the function \ref ARM_WIFI_BypassControl
before a call to this function.

The frame data addressed by \em frame starts with MAC destination and ends with the last
Payload data byte. The frame data is copied into the transmit buffer of the WiFi interface.

The maximum value for \em len is implied by the size restrictions of the Ethernet frame
but is not verified. Using an invalid value for \em len may generate unpredicted results.

\b Example:
\code
status = wifi->EthSendFrame (0U, &frame_data[0], frame_length);
if (status != ARM_DRIVER_OK)  {
  // error handling
}
\endcode
*/

int32_t ARM_WIFI_EthReadFrame (uint32_t interface, uint8_t *frame, uint32_t len) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_WIFI_EthReadFrame (uint32_t interface, uint8_t *frame, uint32_t len)
\details
The function \b ARM_WIFI_EthReadFrame reads an <b>Ethernet frame</b> from the WiFi interface
in the bypass mode.

The \em len of the Ethernet frame can be checked using the function \ref ARM_WIFI_EthGetRxFrameSize.

The frame data addressed by \em frame starts with MAC destination and ends with the last
Payload data byte. The frame data is read from the receive buffer of the WiFi interface and
the number of bytes written into the memory addressed by \em frame is returned.
A negative return value indicates an error whereby the status code is defined with 
driver common return codes.

The function \ref ARM_WIFI_EthReadFrame may be called with \em buf = \token{NULL} and \em len = \token{0}
to discard or release a frame. This is useful when an incorrect frame has been received or
no memory is available to hold the Ethernet frame.
 
\b Example:
\code
size = wifi->EthGetRxFrameSize ();
if ((size < 14) || (size > 1514)) {    // frame excludes CRC
  wifi->EthReadFrame (NULL, 0);        // Frame error, release it
}
len = wifi->ReadFrame (0U, &frame_data[0], size);
if (len < 0)  {
  // error handling
}
\endcode
*/

uint32_t ARM_WIFI_EthGetRxFrameSize (uint32_t interface) {
  return 0;
}
/**
\fn uint32_t ARM_WIFI_EthGetRxFrameSize (uint32_t interface)
\details
The function \b ARM_WIFI_EthGetRxFrameSize returns the size of a received <b>Ethernet frame</b>
in the bypass mode. This function can be called before \ref ARM_WIFI_EthReadFrame and retrieves 
the value \em len.

The frame size includes MAC destination and ends with the last Payload data byte.
Value \token{0} indicates that no Ethernet frame is available in the receive buffer.
Values smaller than minimum size of Ethernet frame or larger than maximum size of Ethernet frame
indicate an invalid frame which needs to be discarded by calling \ref ARM_WIFI_EthReadFrame.
 
\b Example:
 - see \ref ARM_WIFI_EthReadFrame
*/
/**
@}
*/
// end group wifi_bypass_gr


/**
\defgroup wifi_socket_gr WiFi Socket
\ingroup wifi_interface_gr
\brief Socket interface to IP stack running on WiFi module
\details The \ref wifi_socket_gr functions provide the interface to an IP stack that is running
on the WiFi module. This IP stack handles data communication with the network and provides the user
with a communication endpoint called sockets.
@{
*/

/**
\defgroup wifi_addr_family WiFi Socket Address Family definitions
\ingroup wifi_socket_gr
\brief WiFi Socket Address Family definitions.
\details The WiFi Socket Address Family specifies the addressing scheme that an instance of the WiFi socket can use.
@{
\def ARM_SOCKET_AF_INET
\details Internet Address Family version 4.
\def ARM_SOCKET_AF_INET6
\details Internet Address Family version 6.
@}
*/

/**
\defgroup wifi_socket_type WiFi Socket Type definitions
\ingroup wifi_socket_gr
\brief WiFi Socket Type definitions.
\details The WiFi Socket Type specifies the type of the WiFi socket.
@{
\def ARM_SOCKET_SOCK_STREAM
\details Stream Socket is connection-oriented, sequenced and reliable, implemented on top of the TCP protocol.
\def ARM_SOCKET_SOCK_DGRAM
\details Datagram Socket is connectionless, unreliable, using the UDP protocol.
@}
*/

/**
\defgroup wifi_protocol WiFi Socket Protocol definitions
\ingroup WiFi_socket_gr
\brief WiFi Socket Protocol definitions.
\details The WiFi Socket Protocol specifies the Internet Protocol Type that the socket is using.
@{
\def ARM_SOCKET_IPPROTO_TCP
\details Transmission Control Protocol.
\def ARM_SOCKET_IPPROTO_UDP
\details User Datagram Protocol.
@}
*/

/**
\defgroup wifi_soc_opt WiFi Socket Option definitions
\ingroup WiFi_socket_gr
\brief WiFi Socket Option definitions.
\details The WiFi Socket Option specifies the socket option for which the value is to be set or obtained.
@{
\def ARM_SOCKET_IO_FIONBIO
\details Enables or disables the non-blocking mode for the WiFi socket.
\sa wifi_soc_opt
\def ARM_SOCKET_SO_RCVTIMEO
\details Specifies the time limit for receiving in blocking mode. The time limit is in milliseconds.
\sa wifi_soc_opt
\def ARM_SOCKET_SO_SNDTIMEO
\details Specifies the time limit for sending in blocking mode. The time limit is in milliseconds.
\sa wifi_soc_opt
\def ARM_SOCKET_SO_KEEPALIVE
\details Enables or disables the keep-alive mode for the stream socket.
\sa wifi_soc_opt
\def ARM_SOCKET_SO_TYPE
\details Obtains the type of the Wifi socket.
\sa wifi_soc_opt
@}
*/

/**
\defgroup wifi_soc_func WiFi Socket Function return codes
\ingroup WiFi_socket_gr
\brief WiFi Socket Function return codes.
\details This section lists all the return errors the WiFi socket functions will return.
The error codes are negative. This makes it easy to check an error when the return
code is less than \token{0}.
@{
\def ARM_SOCKET_ERROR
\sa wifi_soc_func
\def ARM_SOCKET_ESOCK
\sa wifi_soc_func
\def ARM_SOCKET_EINVAL
\sa wifi_soc_func
\def ARM_SOCKET_ENOTSUP
\sa wifi_soc_func
\def ARM_SOCKET_ENOMEM
\sa wifi_soc_func
\def ARM_SOCKET_EAGAIN
\sa wifi_soc_func
\def ARM_SOCKET_EINPROGRESS
\sa wifi_soc_func
\def ARM_SOCKET_ETIMEDOUT
\sa wifi_soc_func
\def ARM_SOCKET_EISCONN
\sa wifi_soc_func
\def ARM_SOCKET_ENOTCONN
\sa wifi_soc_func
\def ARM_SOCKET_ECONNREFUSED
\sa wifi_soc_func
\def ARM_SOCKET_ECONNRESET
\sa wifi_soc_func
\def ARM_SOCKET_ECONNABORTED
\sa wifi_soc_func
\def ARM_SOCKET_EALREADY
\sa wifi_soc_func
\def ARM_SOCKET_EADDRINUSE
\sa wifi_soc_func
\def ARM_SOCKET_EHOSTNOTFOUND
\sa wifi_soc_func
@}
*/

int32_t ARM_WIFI_SocketCreate (int32_t af, int32_t type, int32_t protocol) {
  return 0;
}
/**
\fn int32_t ARM_WIFI_SocketCreate (int32_t af, int32_t type, int32_t protocol)
\details
The function \b ARM_WIFI_SocketCreate creates a communication endpoint called a socket.

The argument \em af specifies the address family. The following values are supported:
Family                       | Description
:----------------------------|:-------------------------------------------------
\ref ARM_SOCKET_AF_INET      | Address Family Internet
\ref ARM_SOCKET_AF_INET6     | Address Family Internet version 6

The argument \em type specifies the communication semantics. The following are the currently supported types:
Type                         | Description
:----------------------------|:-------------------------------------------------
\ref ARM_SOCKET_SOCK_STREAM  | Provides a reliable connection based data stream that is full-duplex
\ref ARM_SOCKET_SOCK_DGRAM   | Provides connectionless communication that is unreliable

The argument \em protocol specifies the protocol that must be used with the socket type:
Protocol                     | Description
:----------------------------|:-------------------------------------------------
\ref ARM_SOCKET_IPPROTO_TCP  | Must be used with ARM_SOCKET_SOCK_STREAM socket type
\ref ARM_SOCKET_IPPROTO_UDP  | Must be used with ARM_SOCKET_SOCK_DGRAM socket type
 
\b Example:
 - see \ref ARM_WIFI_SocketListen, \ref ARM_WIFI_SocketConnect
*/

int32_t ARM_WIFI_SocketBind (int32_t socket, const uint8_t *ip, uint32_t ip_len, uint16_t port) {
  return 0;
}
/**
\fn int32_t ARM_WIFI_SocketBind (int32_t socket, const uint8_t *ip, uint32_t ip_len, uint16_t port)
\details
The function \b ARM_WIFI_SocketBind assigns a name to an unnamed socket. The name represents the local address
and port of the communication endpoint.

The argument \em socket specifies a socket identification number returned from a previous call
to \ref ARM_WIFI_SocketCreate.

The argument \em ip is a pointer to the buffer containing the IP address octets of the local IP address.

The argument \em ip_len specifies the length of the local IP address. The length is \token{4} bytes
for the IPv4 address and \token{16} bytes for the IPv6 address.

The argument \em port specifies the local port. If the argument \em port is \token{0}, the function returns error,
because this port is reserved.
 
\b Example:
 - see \ref ARM_WIFI_SocketListen
*/

int32_t ARM_WIFI_SocketListen (int32_t socket, int32_t backlog) {
  return 0;
}
/**
\fn int32_t ARM_WIFI_SocketListen (int32_t socket, int32_t backlog)
\details
The function \b ARM_WIFI_SocketListen sets the specified socket to listening mode, that is to the
server mode of operation. Before calling the \b ARM_WIFI_SocketListen function, the \ref ARM_WIFI_SocketBind
function must be called.

The argument \em socket specifies a socket identification number returned from a previous call
to \ref ARM_WIFI_SocketCreate.

The argument \em backlog specifies a maximum number of connection requests that can be queued.
 
\b Example:
\code
extern ARM_DRIVER_WIFI Driver_WiFi0;
static ARM_DRIVER_WIFI *wifi;
 
void Echo_Server_Thread (void *arg) {
  uint8_t ip[4] = { 0U, 0U, 0U, 0U };
  int32_t sock, sd, res;
  char dbuf[120];
 
  while (1) {
    wifi = &Driver_WiFi0;
    sock = wifi->SocketCreate (ARM_SOCKET_AF_INET, ARM_SOCKET_SOCK_STREAM, ARM_SOCKET_IPPROTO_TCP);
 
    wifi->SocketBind (sock, (uint8_t *)ip, sizeof(ip), 7U);
    wifi->SocketListen (sock, 1);
    sd = wifi->SocketAccept (sock, NULL, NULL, NULL);
    wifi->SocketClose (sock);
    sock = sd;
 
    while (1) {
      res = wifi->SocketRecv (sock, dbuf, sizeof(dbuf));
      if (res < 0) {
        break;                                      // Error occurred
      }
      if (res > 0) {
        wifi->SocketSend (sock, dbuf, res);         // Echo the data
      }    
    }
    wifi->SocketClose (sock);
  }
}
\endcode
*/

int32_t ARM_WIFI_SocketAccept (int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port) {
  return 0;
}
/**
\fn int32_t ARM_WIFI_SocketAccept (int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port)
\details
The function \b ARM_WIFI_SocketAccept accepts a connection request queued for a listening socket.
If a connection request is pending, \b ARM_WIFI_SocketAccept removes the request from the queue,
and creates a new socket for the connection. The original listening socket remains open and continues
to queue new connection requests. The \em socket must be a socket of type \b ARM_SOCKET_SOCK_STREAM.

In blocking mode, which is enabled by default, this function waits for a connection request. In
non blocking mode, you must call the \b ARM_WIFI_SocketAccept function again if the error code
\c ARM_SOCKET_EAGAIN is returned.

The argument \em socket specifies a socket identification number returned from a previous call
to \ref ARM_WIFI_SocketCreate.

The argument \em ip is a pointer to the buffer that will receive the IP address of the connection node.
If the \em ip is \token{NULL}, the IP address is not returned.

The argument \em ip_len is a pointer to the IP address length. It should initially contain the amount of
space pointed to by \em ip. On return it contains the actual length of the address returned in bytes.

The argument \em port is a pointer to the buffer, that will receive the port number of the connection node.
If the \em port is \token{NULL}, the port number is not returned.

\b Example:
 - see \ref ARM_WIFI_SocketListen
*/

int32_t ARM_WIFI_SocketConnect (int32_t socket, const uint8_t *ip, uint32_t ip_len, uint16_t port) {
  return 0;
}
/**
\fn int32_t ARM_WIFI_SocketConnect (int32_t socket, const uint8_t *ip, uint32_t ip_len, uint16_t port)
\details
The function \b ARM_WIFI_SocketConnect assigns the address of the peer communication endpoint. The function
behaves differently according to the type of socket:

- \b ARM_SOCKET_SOCK_STREAM: A connection is established between the endpoints.

  In blocking mode, which is enabled by default, this function waits for a connection to be established.

  In non blocking mode, the function returns the error code \c ARM_SOCKET_EINPROGRESS and the connection
  is established asynchronously. Subsequent calls to \b ARM_WIFI_SocketConnect for the same socket,
  before the connection is established, return the error code \c ARM_SOCKET_EALREADY.  When the connection
  is established, the call to \b ARM_WIFI_SocketConnect returns the error code \c ARM_SOCKET_EISCONN. 

- \b ARM_SOCKET_SOCK_DGRAM: An address filter is established between the endpoints.

  The address filter is changed with another \b ARM_WIFI_SocketConnect function call. If the socket
  is not yet bound, the system implicitly binds to a random dynamic port.

The argument \em socket specifies a socket identification number returned from a previous call
to \ref ARM_WIFI_SocketCreate.

The argument \em ip is a pointer to the buffer containing the IP address octets of the endpoint node.

The argument \em ip_len specifies the length of the IP address. The length is \token{4} bytes
for the IPv4 address and \token{16} bytes for the IPv6 address.

The argument \em port specifies the port of the endpoint node. If the argument \em port is \token{0}, 
the function returns error, because this port is reserved.

\b Example:
\code
extern ARM_DRIVER_WIFI Driver_WiFi0;
static ARM_DRIVER_WIFI *wifi;
 
static const char message[] = { "The quick brown fox jumps over the lazy dog." };
  
void Echo_Client_Thread (void *arg) {
  uint8_t ip[4] = { 192U, 168U, 0U, 100U };
  int32_t sock, res;
  char dbuf[120];
 
  while (1) {
    wifi = &Driver_WiFi0;
    sock = wifi->SocketCreate (ARM_SOCKET_AF_INET, ARM_SOCKET_SOCK_STREAM, ARM_SOCKET_IPPROTO_TCP);
 
    res = wifi->SocketConnect (sock, (uint8_t *)ip, sizeof(ip), 7U);
    if (res == 0) {
      wifi->SocketSend (sock, message, sizeof(message));
      res = wifi->SocketRecv (sock, dbuf, sizeof(dbuf));
      if (res < 0) {
        break;               // Error occured
      }
      if (res > 0) {
        if (memcmp (dbuf, message, res) != 0) {
          // error handling, message is not the same as sent
        }
      }    
    }
    wifi->SocketClose (sock);
    osDelay (1000U);
  }
}
\endcode
*/

int32_t ARM_WIFI_SocketRecv (int32_t socket, void *buf, uint32_t len) {
  return 1;
}
/**
\fn int32_t ARM_WIFI_SocketRecv (int32_t socket, void *buf, uint32_t len)
\details
The function \b ARM_WIFI_SocketRecv receives incoming data that has been queued for the socket.
You can use this function with both, the stream and the datagram socket. It reads as much
information as currently available up to the size of the buffer specified.

In blocking mode, which is enabled by default, this function waits for received data. In non
blocking mode, you must call the \b ARM_WIFI_SocketRecv function again if the error code 
\c ARM_SOCKET_EAGAIN is returned.

The argument \em socket specifies a socket identification number returned from a previous call
to \ref ARM_WIFI_SocketCreate.

The argument \em buf is a pointer to the application data buffer for storing the data to.
If the available data is too large to fit in the supplied application buffer \em buf, excess bytes
are discarded in case of a datagram sockets. For stream sockets, the data is buffered internally
so the application can retrieve all data by multiple calls of \b ARM_WIFI_SocketRecv function.

The argument \em len specifies the size of the application data buffer.

\b Example:
 - see \ref ARM_WIFI_SocketListen
*/

int32_t ARM_WIFI_SocketRecvFrom (int32_t socket, void *buf, uint32_t len, uint8_t *ip, uint32_t *ip_len, uint16_t *port) {
  return 1;
}
/**
\fn int32_t ARM_WIFI_SocketRecvFrom (int32_t socket, void *buf, uint32_t len, uint8_t *ip, uint32_t *ip_len, uint16_t *port)
\details
The function \b ARM_WIFI_SocketRecvFrom is used to receive data that has been queued for a socket.
It is normally used to receive messages on datagram sockets, but can also be used to receive a reliable,
ordered stream of data on a connected stream sockets. It reads as much information as currently available
up to the size of the buffer specified.

In blocking mode, which is enabled by default, this function waits for received data. In non
blocking mode, you must call the \b ARM_WIFI_SocketRecv function again if the error code 
\c ARM_SOCKET_EAGAIN is returned.

The argument \em socket specifies a socket identification number returned from a previous call
to \ref ARM_WIFI_SocketCreate.

The argument \em buf is a pointer to the application data buffer for storing the data to.
If the available data is too large to fit in the supplied application buffer \em buf, excess bytes
are discarded in case of a datagram sockets. For stream sockets, the data is buffered internally
so the application can retrieve all data by multiple calls of \b ARM_WIFI_SocketRecv function.

The argument \em len specifies the size of the application data buffer.

The argument \em ip is a pointer to the buffer that will receive the IP address of the sender.
If the \em ip is \token{NULL}, the IP address is not returned.

The argument \em ip_len is a pointer to the IP address length. It should initially contain the amount of
space pointed to by \em ip. On return it contains the actual length of the address returned in bytes.

The argument \em port is a pointer to the buffer, that will receive the port number of the sender.
If the \em port is \token{NULL}, the port number is not returned.
 
\b Example:
\code
extern ARM_DRIVER_WIFI Driver_WiFi0;
static ARM_DRIVER_WIFI *wifi;
 
void Echo_Server_Thread (void *arg) {
  uint8_t ip[4];
  uint16_t port;
  int32_t sock, res;
  uint32_t ip_len;
  char dbuf[120];
 
  while (1) {
    wifi = &Driver_WiFi0;
    sock = wifi->SocketCreate (ARM_SOCKET_AF_INET, ARM_SOCKET_SOCK_DGRAM, ARM_SOCKET_IPPROTO_UDP);
 
    ip[0] = 0U;                 // Unspecified address
    ip[1] = 0U;
    ip[2] = 0U;
    ip[3] = 0U;
    port  = 7U;                 // Standard port for Echo service
 
    wifi->SocketBind (sock, (uint8_t *)ip, sizeof(ip), port);
 
    while (1) {
      ip_len = sizeof(ip);
      res = wifi->SocketRecvFrom (sock, dbuf, sizeof(dbuf), (uint8_t *)ip, &ip_len, &port);
      if (res < 0) {
        break;                  // Error occurred
      }
      if (res > 0) {            // Echo the data
        wifi->SocketSendTo (sock, dbuf, res, (uint8_t *)ip, ip_len, port);
      }    
    }
    wifi->SocketClose (sock);
  }
}
\endcode
*/

int32_t ARM_WIFI_SocketSend (int32_t socket, const void *buf, uint32_t len) {
  return 1;
}
/**
\fn int32_t ARM_WIFI_SocketSend (int32_t socket, const void *buf, uint32_t len)
\details
The function \b ARM_WIFI_SocketSend is used to send data on an already connected socket. This function is
normally used to send a reliable, ordered stream of data bytes on a stream sockets. It can also be used
to send messages on datagram sockets.

The argument \em socket specifies a socket identification number returned from a previous call
to \ref ARM_WIFI_SocketCreate.

The argument \a buf is a pointer to the application data buffer containing data to transmit. The buffer
data length is not limited in size. If the data length is too large for one packet, the \b ARM_WIFI_SocketSend function
will fragment the data and send it in several successive data packets:
- In blocking mode, which is enabled by default, this function returns after the data has been successfully queued for transmission.
- In non blocking mode, the function returns immediately without blocking the system.

The argument \a len specifies the length of data in bytes.

Return value, when positive, represents the number of bytes sent, which can be less than \a len.
 
\b Example:
 - see \ref ARM_WIFI_SocketListen
*/

int32_t ARM_WIFI_SocketSendTo (int32_t socket, const void *buf, uint32_t len, const uint8_t *ip, uint32_t ip_len, uint16_t port) {
  return 1;
}
/**
\fn int32_t ARM_WIFI_SocketSendTo (int32_t socket, const void *buf, uint32_t len, const uint8_t *ip, uint32_t ip_len, uint16_t port)
\details
The function \b ARM_WIFI_SocketSendTo is used to send data. It is normally used to send messages
on a datagram sockets, but can also be used to send data on a connected stream sockets.

If the datagram socket is not yet bound, the system implicitly binds to a random dynamic port.

The argument \em socket specifies a socket identification number returned from a previous call
to \ref ARM_WIFI_SocketCreate.

The argument \a buf is a pointer to the application data buffer containing data to transmit. The buffer
data length is not limited in size. If the data length is too large for one packet, the \b ARM_WIFI_SocketSend function
will fragment the data and send it in several successive data packets:
- In blocking mode, which is enabled by default, this function returns after the data has been successfully queued for transmission.
- In non blocking mode, the function returns immediately without blocking the system.

The argument \a len specifies the length of data in bytes.

The argument \em ip is a pointer to the buffer containing the IP address octets of the endpoint node.

The argument \em ip_len specifies the length of the IP address. The length is \token{4} bytes
for the IPv4 address and \token{16} bytes for the IPv6 address.

The argument \em port specifies the port of the endpoint node. If the argument \em port is \token{0}, 
the function returns error, because this port is reserved.

For the stream sockets, arguments \em ip, \em ip_len and \em port are ignored.

Return value, when positive, represents the number of bytes sent, which can be less than \a len.

\b Example:
 - see \ref ARM_WIFI_SocketRecvFrom
*/

int32_t ARM_WIFI_SocketGetSockName (int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port) {
  return 0;
}
/**
\fn int32_t ARM_WIFI_SocketGetSockName (int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port)
\details
The function \b ARM_WIFI_SocketGetSockName retrieves the local IP address and port for a socket.

The argument \em socket specifies a socket identification number returned from a previous call
to \ref ARM_WIFI_SocketCreate.

The argument \em ip is a pointer to the buffer that will receive the local IP address.
If the \em ip is \token{NULL}, the local IP address is not returned.

The argument \em ip_len is a pointer to the IP address length. It should initially contain the amount of
space pointed to by \em ip. On return it contains the actual length of the address returned in bytes.

The argument \em port is a pointer to the buffer, that will receive the local port number.
If the \em port is \token{NULL}, the local port number is not returned.

\b Example:
\code
static uint8_t  local_ip[4];    // Socket address and port
static uint16_t local_port;
 
static void get_socket_local_info (void) {
  uint32_t ip_len;
 
  ip_len = sizeof(local_ip);
  wifi->SocketGetSockName (sock, (uint8_t *)local_ip, &ip_len, &local_port);
}
\endcode
*/

int32_t ARM_WIFI_SocketGetPeerName (int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port) {
  return 0;
}
/**
\fn int32_t ARM_WIFI_SocketGetPeerName (int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port)
\details
The function \b ARM_WIFI_SocketGetPeerName retrieves the IP address and port of the peer to which
a socket is connected.

The argument \em socket specifies a socket identification number returned from a previous call
to \ref ARM_WIFI_SocketCreate.

The argument \em ip is a pointer to the buffer that will receive the IP address of the peer.
If the \em ip is \token{NULL}, the IP address is not returned.

The argument \em ip_len is a pointer to the IP address length. It should initially contain the amount of
space pointed to by \em ip. On return it contains the actual length of the address returned in bytes.

The argument \em port is a pointer to the buffer, that will receive the port number of the peer.
If the \em port is \token{NULL}, the port number is not returned.

\b Example:
\code
static uint8_t  peer_ip[4];    // Socket address and port
static uint16_t peer_port;
 
static void get_socket_peer_info (void) {
  uint32_t ip_len;
 
  ip_len = sizeof(peer_ip);
  wifi->SocketGetPeerName (sock, (uint8_t *)peer_ip, &ip_len, &peer_port);
}
\endcode
*/

int32_t ARM_WIFI_SocketGetOpt (int32_t socket, int32_t opt_id, void *opt_val, uint32_t *opt_len) {
  return 0;
}
/**
\fn int32_t ARM_WIFI_SocketGetOpt (int32_t socket, int32_t opt_id, void *opt_val, uint32_t *opt_len)
\details
The function \b ARM_WIFI_SocketGetOpt retrieves options for a socket.

The argument \em socket specifies a socket identification number returned from a previous call
to \ref ARM_WIFI_SocketCreate.

The argument \em opt_id is the socket option for which the value is to be retrieved. The following
socket options are supported:
Option                       | Description
:----------------------------|:-------------------------------------------------
\ref ARM_SOCKET_SO_RCVTIMEO  | Timeout for receiving in blocking mode
\ref ARM_SOCKET_SO_SNDTIMEO  | Timeout for sending in blocking mode
\ref ARM_SOCKET_SO_KEEPALIVE | Keep-alive mode for the stream socket
\ref ARM_SOCKET_SO_TYPE      | Type of the socket (stream or datagram)

The argument \em opt_val points to the buffer that will receive the value of the \em opt_id.

The argument \em opt_len contains the length of the buffer at the input and returns the length
of the option information on the output.

\b Example:
\code
uint32_t type;
 
wifi->SocketGetOpt (sock, ARM_SOCKET_SO_TYPE, &type, sizeof(type));
if (type == ARM_SOCKET_SOCK_STREAM) {
  // Stream socket
}
if (type == ARM_SOCKET_SOCK_DGRAM) {
  // Datagram socket
}
\endcode
*/

int32_t ARM_WIFI_SocketSetOpt (int32_t socket, int32_t opt_id, const void *opt_val, uint32_t opt_len) {
  return 0;
}
/**
\fn int32_t ARM_WIFI_SocketSetOpt (int32_t socket, int32_t opt_id, const void *opt_val, uint32_t opt_len)
\details
The function \b ARM_WIFI_SocketSetOpt sets options for a socket.

The argument \em socket specifies a socket identification number returned from a previous call
to \ref ARM_WIFI_SocketCreate.

The argument \em opt_id is the socket option for which the value is to be set. The following
socket options are supported:
Option                       | Description
:----------------------------|:-------------------------------------------------
\ref ARM_SOCKET_IO_FIONBIO   | Non-blocking mode for the socket
\ref ARM_SOCKET_SO_RCVTIMEO  | Timeout for receiving in blocking mode
\ref ARM_SOCKET_SO_SNDTIMEO  | Timeout for sending in blocking mode
\ref ARM_SOCKET_SO_KEEPALIVE | Keep-alive mode for the stream socket

The argument \em opt_val points to the buffer containing the value of the \em opt_id.

The argument \em opt_len tells the exact length of the option.
 
\b Example:
\code
uint32_t nonblocking = 0U;    // Blocking mode
uint32_t timeout = 10000U;    // Timeout 10 seconds
 
wifi->SocketSetOpt (sock, ARM_SOCKET_IO_FIONBIO, &nonblocking, sizeof(nonblocking));
wifi->SocketSetOpt (sock, ARM_SOCKET_SO_RCVTIMEO, &timeout, sizeof(timeout)); 
wifi->SocketSetOpt (sock, ARM_SOCKET_SO_SNDTIMEO, &timeout, sizeof(timeout));
\endcode
*/

int32_t ARM_WIFI_SocketClose (int32_t socket) {
  return 0;
}
/**
\fn int32_t ARM_WIFI_SocketClose (int32_t socket)
\details
The function \b ARM_WIFI_SocketClose closes an existing socket and releases the socket descriptor.
Further references to \em socket fail with \c ARM_SOCKET_EINVAL error code.

The argument \em socket specifies a socket identification number returned from a previous call
to \ref ARM_WIFI_SocketCreate.

In blocking mode, which is enabled by default, this function will wait until a socket is closed.
In non blocking mode, you must call the \b ARM_WIFI_SocketClose function again if the error code
\c ARM_SOCKET_EAGAIN is returned.

\b Example:
 - see \ref ARM_WIFI_SocketListen
*/

int32_t ARM_WIFI_SocketGetHostByName (const char *name, int32_t af, uint8_t *ip, uint32_t *ip_len) {
  return 0;
}
/**
\fn int32_t ARM_WIFI_SocketGetHostByName (const char *name, int32_t af, uint8_t *ip, uint32_t *ip_len)
\details
The function \b ARM_WIFI_SocketGetHostByName retrieves host information corresponding to
a host name from a host database. It does this by sending DNS requests to the DNS server.
The IP address of the DNS server is specified in the network interface configuration or can be
obtained from the DHCP server for the local area network.

The argument \a name is a pointer to the \token{null}-terminated name of the host to resolve.

The argument \em af specifies the address family, that is, which type of IP address you want
to resolve. The following values are supported:
Family                       | Description
:----------------------------|:-------------------------------------------------
\ref ARM_SOCKET_AF_INET      | Resolve the IPv4 address
\ref ARM_SOCKET_AF_INET6     | Resolve the IPv6 address

The argument \em ip is a pointer to the buffer that will receive the resolved IP address of the host.
If the argument \em ip is \token{NULL}, the function returns error.

The argument \em ip_len is a pointer to the IP address length. It should initially contain the amount of
space pointed to by \em ip. On return it contains the actual length of the address returned in bytes.

\b Example:
\code
extern ARM_DRIVER_WIFI Driver_WiFi0;
static ARM_DRIVER_WIFI *wifi;
 
void ping_arm_com (void) {
  uint8_t ip[4];
  uint32_t ip_len;
  int32_t res;
 
  wifi = &Driver_WiFi0;
  ip_len = sizeof(ip);
  res = wifi->SocketGetHostByName ("www.arm.com", ARM_SOCKET_AF_INET, (uint8_t *)ip, &ip_len);
  if (res == ARM_DRIVER_OK) {
    res = wifi->Ping ((uint8_t *)ip, sizeof(ip));
    if (res == ARM_DRIVER_OK) {
      // "www.arm.com" responded to ping  
    }
  }
  else {
    // "www.arm.com" not resolved
  }
}
\endcode
*/

int32_t ARM_WIFI_Ping (const uint8_t *ip, uint32_t ip_len) {
  return ARM_DRIVER_OK;
}
/**
\fn int32_t ARM_WIFI_Ping (const uint8_t *ip, uint32_t ip_len)
\details
The function \b ARM_WIFI_Ping checks if the remote host is reachable. It does this by sending 
an echo request and waiting for an echo response. The function then returns the result
of the operation. Check the \ref ARM_WIFI_CAPABILITIES of the driver, if this function
is supported in the driver implementation.

The argument \em ip is a pointer to the buffer containing the IP address octets of the host to ping.

The argument \em ip_len specifies the length of the IP address. The length is \token{4} bytes
for the IPv4 address and \token{16} bytes for the IPv6 address.
 
\note
The host availability check fails, if the remote host does not respond to echo requests,
or intermediate routers do not forward the echo requests or echo responses.

\b Example:
\code
extern ARM_DRIVER_WIFI Driver_WiFi0;
static ARM_DRIVER_WIFI *wifi;
 
void ping_host (void) {
  uint8_t ip[4] = { 192U, 168U, 0U, 100U };
  int32_t res;
 
  wifi = &Driver_WiFi0;
  res = wifi->Ping ((uint8_t *)ip, sizeof(ip));
  if (res == ARM_DRIVER_OK) {
    // Host responded
  }
}
\endcode
*/
/**
@}
*/
// end group wifi_socket_gr


/**
@}
*/
// End WiFi Interface
