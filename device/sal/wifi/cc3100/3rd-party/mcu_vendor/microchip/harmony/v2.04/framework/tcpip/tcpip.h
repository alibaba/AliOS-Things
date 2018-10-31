/*******************************************************************************
  Microchip TCP/IP Stack Include File

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip.h

  Summary:
    TCP/IP API definitions.
	
  Description:
    This is the global TCP/IP header file that any user of the TCP/IP API should include.
    It contains the basic TCP/IP types and data structures and includes all the 
    of the TCP/IP stack modules.
  
    Simplified version!
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2012-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef __TCPIP_H__
#define __TCPIP_H__


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
        
#include "system_config.h"
#include "system/common/sys_common.h"
#include "system/common/sys_module.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  


// TCP/IP stack version
#define TCPIP_STACK_VERSION_MAJOR         7
#define TCPIP_STACK_VERSION_MINOR         25
#define TCPIP_STACK_VERSION_PATCH         0
// #define TCPIP_STACK_VERSION_STR           "7.27"
#define TCPIP_STACK_VERSION_STR         "MPLAB Harmony trunk"


// *****************************************************************************
// *****************************************************************************
// Section: List of the supported TCP/IP Stack Addresses
//          The following enumeration lists all of the address types
//          supported by the TCP/IP Stack.
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* IPv4 Address type

  Summary:
    Definition to represent an IPv4 address

  Description:
    This type describes the IPv4 address type.

  Remarks:
    None.
*/

typedef union
{
    uint32_t Val;
    uint16_t w[2];
    uint8_t  v[4];
} IPV4_ADDR;

// *****************************************************************************
/* IPv4 Address backward compatible type

  Summary:
    Backward compatible definition to represent an IPv4 address.

  Description:
    This type describes the backward compatible IPv4 address type.

  Remarks:
    None.
*/

typedef IPV4_ADDR   IP_ADDR;


// *****************************************************************************
/* IPv6 Address type

  Summary:
    Definition to represent an IPv6 address.

  Description:
    This type describes the IPv6 address type.

  Remarks:
    None.
*/

typedef union
{
    uint8_t  v[16];
    uint16_t w[8];
    uint32_t d[4];
} IPV6_ADDR;

// *****************************************************************************
/* TCP/IP IP Address type

  Summary:
    Definition of the TCP/IP supported address types.

  Description:
    This type describes the supported IP address types.

  Remarks:
    None.
*/

typedef enum
{
    /* either IPv4 or IPv6, unspecified; */
    IP_ADDRESS_TYPE_ANY /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,
    /* IPv4 address type */
    IP_ADDRESS_TYPE_IPV4,
    /* IPv6 address type */
    IP_ADDRESS_TYPE_IPV6,
}IP_ADDRESS_TYPE;
    

// *****************************************************************************
/* TCP/IP multiple Address type

  Summary:
    Definition to represent multiple IP addresses.

  Description:
    This type describes the TCP/IP multiple address type.

  Remarks:
    None.
*/

typedef union
{
    IPV4_ADDR v4Add;
    IPV6_ADDR v6Add;
}IP_MULTI_ADDRESS;
    
// *****************************************************************************
/* IPv6 Address scope

  Summary:
    Definition to represent the scope of an IPv6 address.

  Description:
    This type describes the IPv6 address scope.

  Remarks:
    None.
*/

typedef enum
{
    IPV6_ADDR_SCOPE_UNKNOWN         /*DOM-IGNORE-BEGIN*/ = 0x00 /*DOM-IGNORE-END*/,
    IPV6_ADDR_SCOPE_INTERFACE_LOCAL /*DOM-IGNORE-BEGIN*/ = 0x01 /*DOM-IGNORE-END*/,
    IPV6_ADDR_SCOPE_LINK_LOCAL      /*DOM-IGNORE-BEGIN*/ = 0x02 /*DOM-IGNORE-END*/,
    IPV6_ADDR_SCOPE_ADMIN_LOCAL     /*DOM-IGNORE-BEGIN*/ = 0x04 /*DOM-IGNORE-END*/,
    IPV6_ADDR_SCOPE_SITE_LOCAL      /*DOM-IGNORE-BEGIN*/ = 0x05 /*DOM-IGNORE-END*/,
    IPV6_ADDR_SCOPE_ORG_LOCAL       /*DOM-IGNORE-BEGIN*/ = 0x08 /*DOM-IGNORE-END*/,
    IPV6_ADDR_SCOPE_GLOBAL          /*DOM-IGNORE-BEGIN*/ = 0x0E /*DOM-IGNORE-END*/,
}IPV6_ADDR_SCOPE;

// *****************************************************************************
/* IPv6 Address type

  Summary:
    Definition to represent the type of an IPv6 address.

  Description:
    This type describes the possible type of an IPv6 address.

  Remarks:
    None.
*/

typedef enum
{
    /* Invalid/unknown address type */
    IPV6_ADDR_TYPE_UNKNOWN                  /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,
    /* Only link-local and global are currently valid for unicast */
    IPV6_ADDR_TYPE_UNICAST                  /*DOM-IGNORE-BEGIN*/ = 0x01 /*DOM-IGNORE-END*/,
    IPV6_ADDR_TYPE_ANYCAST                  /*DOM-IGNORE-BEGIN*/ = 0x02 /*DOM-IGNORE-END*/,
    IPV6_ADDR_TYPE_MULTICAST                /*DOM-IGNORE-BEGIN*/ = 0x03 /*DOM-IGNORE-END*/,
    IPV6_ADDR_TYPE_SOLICITED_NODE_MULTICAST /*DOM-IGNORE-BEGIN*/ = 0x04 /*DOM-IGNORE-END*/,
    IPV6_ADDR_TYPE_UNICAST_TENTATIVE        /*DOM-IGNORE-BEGIN*/ = 0x05 /*DOM-IGNORE-END*/,
}IPV6_ADDR_TYPE;



// *****************************************************************************
/* IPv6 Address Structure

  Summary:
    COmplex type to represent an IPv6 address.

  Description:
    This type defines all the fields of an IPv6 address.

  Remarks:
    None.
*/

typedef struct __attribute__((__packed__)) _IPV6_ADDR_STRUCT
{
    struct _IPV6_ADDR_STRUCT * next;
    struct _IPV6_ADDR_STRUCT * prev;
    IPV6_ADDR address;
    unsigned long validLifetime;
    unsigned long preferredLifetime;
    unsigned long lastTickTime;
    unsigned char prefixLen;
    struct __attribute__((__packed__))
    {
        /* Allow preferences */
        unsigned char precedence;                  
        /* Link-local, site-local, global. */
        unsigned scope                  :4;
        /* Policy label */
        unsigned label                  :4; 
        /* Uni-, Any-, Multi-cast */
        unsigned type                   :2;
        /* Indicates that the address is temporary (not public) */
        unsigned temporary              :1;
    }flags;
} IPV6_ADDR_STRUCT;

// *****************************************************************************
/* IPv6 Address handle

  Summary:
    Definition to represent an IPv6 address.

  Description:
    This type describes an IPv6 address handle.

  Remarks:
    None.
*/

typedef const void*   IPV6_ADDR_HANDLE;


// *****************************************************************************
/* TCP/IP stack supported modules

  Summary:
    List of the TCP/IP stack supported modules.

  Description:
    The following enumeration lists all the modules supported
    by the TCP/IP stack.

  Remarks:
    None.
*/

typedef enum
{
    /* unspecified/unknown module */
    TCPIP_MODULE_NONE              = 0,
    /*DOM-IGNORE-BEGIN*/     // manager: layer 0 module /*DOM-IGNORE-END*/
    TCPIP_MODULE_MANAGER,    /* stack manager + packet allocation manager */ 

    /*DOM-IGNORE-BEGIN*/    TCPIP_MODULE_LAYER1,  // 1st layer modules: 2 - 5 /*DOM-IGNORE-END*/
    TCPIP_MODULE_ARP        /*DOM-IGNORE-BEGIN*/ = TCPIP_MODULE_LAYER1 /*DOM-IGNORE-END*/,
    TCPIP_MODULE_IPV4,
    TCPIP_MODULE_IPV6,
    TCPIP_MODULE_LLDP,              /* LLDP module */

    /*DOM-IGNORE-BEGIN*/    TCPIP_MODULE_LAYER2, // 2nd layer modules: 6 - 11 /*DOM-IGNORE-END*/
    TCPIP_MODULE_ICMP       /*DOM-IGNORE-BEGIN*/ = TCPIP_MODULE_LAYER2 /*DOM-IGNORE-END*/,
    TCPIP_MODULE_ICMPV6,
    TCPIP_MODULE_NDP,
    TCPIP_MODULE_UDP,
    TCPIP_MODULE_TCP,
    TCPIP_MODULE_IGMP,      /* IGMP host module */

    /*DOM-IGNORE-BEGIN*/    TCPIP_MODULE_LAYER3, // 3rd layer modules: 12 - 35 /*DOM-IGNORE-END*/
    TCPIP_MODULE_DHCP_CLIENT  /*DOM-IGNORE-BEGIN*/ = TCPIP_MODULE_LAYER3 /*DOM-IGNORE-END*/,
    TCPIP_MODULE_DHCP_SERVER,
    TCPIP_MODULE_ANNOUNCE,
    TCPIP_MODULE_DNS_CLIENT,
    TCPIP_MODULE_DNS_SERVER,
    TCPIP_MODULE_ZCLL,              /* Zero Config Link Local */
    TCPIP_MODULE_MDNS,              /* Bonjour/mDNS */
    TCPIP_MODULE_NBNS,
    TCPIP_MODULE_SMTP_CLIENT,
    TCPIP_MODULE_SNTP,
    TCPIP_MODULE_FTP_SERVER,
    TCPIP_MODULE_HTTP_SERVER,
    TCPIP_MODULE_HTTP_NET_SERVER,
    TCPIP_MODULE_TELNET_SERVER,
    TCPIP_MODULE_SNMP_SERVER,
    TCPIP_MODULE_SNMPV3_SERVER,
    TCPIP_MODULE_DYNDNS_CLIENT,
    TCPIP_MODULE_BERKELEY,
    TCPIP_MODULE_REBOOT_SERVER,
    TCPIP_MODULE_COMMAND,
    TCPIP_MODULE_IPERF,
    TCPIP_MODULE_TFTP_CLIENT,       /* TFTP client module */
    TCPIP_MODULE_DHCPV6_CLIENT,     /* DHCPV6 client */
    TCPIP_MODULE_SMTPC,             /* SMTP (new) client */

    /* add other modules here */
    //
    /*  */
    TCPIP_MODULES_NUMBER,       /* number of modules in the TCP/IP stack itself */
    /*  */
    /* starting here is list of supported MAC modules */
    /* and are defined in the tcpip_mac.h  */
    TCPIP_MODULE_MAC_START /*DOM-IGNORE-BEGIN*/ = 0x1000 /*DOM-IGNORE-END*/, 

}TCPIP_STACK_MODULE;


// *****************************************************************************
/* TCP/IP stack supported network interfaces

  Summary:
    Defines the TCP/IP stack supported network interfaces.

  Description:
     The following enumeration lists the names of all the interfaces supported
     by the TCP/IP stack and aliases.

  Remarks:
    None.
*/

#define TCPIP_STACK_IF_NAME_NONE            0
#define TCPIP_STACK_IF_NAME_ENCJ60          "ENCJ60"
#define TCPIP_STACK_IF_NAME_ENCJ600         "ENCJ600"
#define TCPIP_STACK_IF_NAME_97J60           "97J60"
#define TCPIP_STACK_IF_NAME_PIC32INT        "PIC32INT"
#define TCPIP_STACK_IF_NAME_MRF24WN         "MRF24WN"
#define TCPIP_STACK_IF_NAME_WINC1500        "WINC1500"
#define TCPIP_STACK_IF_NAME_WILC1000        "WILC1000"

/* alias for unknown interface */
#define TCPIP_STACK_IF_NAME_ALIAS_UNK       "unk"
/* alias for Ethernet interface */
#define TCPIP_STACK_IF_NAME_ALIAS_ETH       "eth"
/* alias for Wi-Fi interface */
#define TCPIP_STACK_IF_NAME_ALIAS_WLAN      "wlan"

// *****************************************************************************
/* Configuration Power Modes

  Summary:
    Supported TCP/IP network configuration power modes.

  Description:
    This describes the list of the supported TCP/IP network configuration power modes.

  Remarks:
    None.
*/

/* unsupported, invalid */
#define TCPIP_STACK_IF_POWER_NONE       0
/* up and running; */
#define TCPIP_STACK_IF_POWER_FULL       "full"  
/* low power mode; not supported now */
#define TCPIP_STACK_IF_POWER_LOW        "low"   
/* powered down, not started */
#define TCPIP_STACK_IF_POWER_DOWN       "down"  

// *****************************************************************************
/* Network Configuration start-up flags

  Summary:
    Definition of network configuration start-up flags.

  Description:
    This enumerated type describes the list of the supported TCP/IP network 
    configuration start-up flags.

  Remarks:
    DHCPc, DHCPs and ZCLL are conflicting and
    cannot be more than one enabled on the same interface!
    The order of priorities is: DHCPc, ZCLL, DHCPs in case of conflict
    
    Only one of either DNS server or client can be enabled per interface.

    Valid values are 0x0001 - 0x8000; 16 bits are maintained only.
*/

typedef enum
{
    /* Start the interface with a static IP address */
    /* No address service is enabled on this interface (DHCPc, ZCLL or DHCPs) */
    TCPIP_NETWORK_CONFIG_IP_STATIC            /*DOM-IGNORE-BEGIN*/ = 0x0000 /*DOM-IGNORE-END*/,   
    /* DHCP client enabled on this interface */
    TCPIP_NETWORK_CONFIG_DHCP_CLIENT_ON       /*DOM-IGNORE-BEGIN*/ = 0x0001 /*DOM-IGNORE-END*/,   
    /* ZeroConf LinkLocal enabled on this interface */
    TCPIP_NETWORK_CONFIG_ZCLL_ON              /*DOM-IGNORE-BEGIN*/ = 0x0002 /*DOM-IGNORE-END*/,   
    /* DHCP server enabled on this interface */
    TCPIP_NETWORK_CONFIG_DHCP_SERVER_ON       /*DOM-IGNORE-BEGIN*/ = 0x0004 /*DOM-IGNORE-END*/,   

    /* DNS CLIENT enabled on this interface */
    TCPIP_NETWORK_CONFIG_DNS_CLIENT_ON     	  /*DOM-IGNORE-BEGIN*/ = 0x0008 /*DOM-IGNORE-END*/,	  
    /* DNS Server Enabled on this Interface */
    TCPIP_NETWORK_CONFIG_DNS_SERVER_ON		  /*DOM-IGNORE-BEGIN*/ = 0x0010 /*DOM-IGNORE-END*/,   
    /* Multicast traffic enabled on this Interface */
    TCPIP_NETWORK_CONFIG_MULTICAST_ON		  /*DOM-IGNORE-BEGIN*/ = 0x0020 /*DOM-IGNORE-END*/,   
    /* Packet logging is enabled on this Interface */
    TCPIP_NETWORK_CONFIG_PKT_LOG_ON		      /*DOM-IGNORE-BEGIN*/ = 0x0040 /*DOM-IGNORE-END*/,   

    /* the network configuration contains an IPv6 static address and subnet prefix length */
    TCPIP_NETWORK_CONFIG_IPV6_ADDRESS         /*DOM-IGNORE-BEGIN*/ = 0x0100 /*DOM-IGNORE-END*/,   

    /* add other configuration flags here */
}TCPIP_NETWORK_CONFIG_FLAGS;

// *****************************************************************************
/* Forward declaration of a TCP/IP MAC basic Object

  Summary:
    Definition of a TCP/IP MAC Object.

  Description:
    This data structure identifies the required basic interface of the TCP/IP MAC driver.
    Any dynamic MAC driver has to export this interface.

  Remarks:
    See the structure definition in "tcpip/tcpip_mac_object.h" for details of its members.

*/
struct TCPIP_MAC_OBJECT_TYPE;

// *****************************************************************************
/* Multi-Homed Hosts Network Addressing Configuration Data

  Summary:
    Defines the data required to initialize the network configuration.

  Description:
    This data type defines the data required to initialize the network configuration
    for a specific interface.

  Remarks:
    IPv4 aliased interfaces can be created by specifying the same MAC object member
    TCPIP_NETWORK_CONFIG.pMacObject. 
    An aliased interface is one that shares the same physical interface
    and MAC object with a primary interface.

    Note that the .macAddr is irrelevant for an IPv4 aliased interface.
    The first interface fully configured will be the primary interface, others will
    be aliases.

    An IPv4 alias interface will allow, for example, 
    having a different static/dynamic IPv4 address on the same physical interface.

    Note that the stack won't allow initialization of multiple interfaces
    with the same static IPv4 address.

    It is also recommended that each interface has a different host name.

    For an IPv4 alias interface .powerMode ==  TCPIP_STACK_IF_POWER_DOWN can be used to prevent
    the alias to be started when the stack is initialized and the primary interfaces go up.
 
    A primary interface currently supports only the TCPIP_STACK_IF_POWER_FULL power mode.

    Alias interfaces are not currently supported on IPv6.

    Currently a broadcast message received (on a primary interface)
    is not duplicated on all aliases but it will appear only on the primary interface. 
*/
typedef struct
{
    /* Pointer to the interface name; could be NULL. */
    /* Note: Usually it's been one of the TCPIP_STACK_IF_NAME_xxx symbols: */
    /*        "ENCJ60", "ENCJ600", "97J60", "PIC32INT", "MRF24WN", "WINC1500", "WILC1000". */
    /*        However, the TCP/IP stack will assign an alias names for each interface */
    /*        (eth0, wlan0, etc. see TCPIP_STACK_IF_NAME_ALIAS_xxx), so this name is maintained */
    /*        for backward compatibility purposes only and will be eventually dropped. */
    /*        The native name of the interface in the form TCPIP_STACK_IF_NAME_xxx is */
    /*        present in the MAC driver for that interface (see the "tcpip/tcpip_mac_object.h" */
    /*        definition of TCPIP_MAC_OBJECT_TYPE). */
    char*     interface;

    /* Valid Host name for this interface to use.  Ex: "MCHPBOARD" */
    char*     hostName;

    /* MAC address to use for this interface. */
    /*   Use "00:04:a3:00:00:00" or 0 for the factory preprogrammed address*/
    char*     macAddr;

    /* Static IP address to use.  Ex: "169.254.1.1"*/
    char*     ipAddr;

    /* Netmask to use. Ex: "255.255.0.0"*/
    char*     ipMask;

    /* Static Gateway to use. Ex: "169.254.1.1"*/
    char*     gateway;

    /* Primary DNS to use.  Ex: "169.254.1.1"*/
    char*     priDNS;

    /* Secondary DNS to use.  Use "0.0.0.0" for none */
    char*     secondDNS;  

    /* Power Mode to use.  Use TCPIP_STACK_IF_POWER_NONE, TCPIP_STACK_IF_POWER_FULL, */
    /* TCPIP_STACK_IF_POWER_LOW, or TCPIP_STACK_IF_POWER_DOWN*/
    char*     powerMode; 

    /* flags for interface start-up */
    TCPIP_NETWORK_CONFIG_FLAGS   startFlags;

    /* Non-volatile pointer to the MAC driver object associated with this network interface */
    /* This is the MAC driver that this interface will use */
    /* Note: This object has to be valid for the whole life of the interface! */
    /*       The TCP/IP stack does not make a private copy of this object */
    const struct TCPIP_MAC_OBJECT_TYPE*    pMacObject;   

    /* static IPv6 address; only if TCPIP_NETWORK_CONFIG_IPV6_ADDRESS specified can be NULL if not needed*/
    char*     ipv6Addr;   

    /* subnet prefix length; only if TCPIP_NETWORK_CONFIG_IPV6_ADDRESS specified
       0 means default value (64)
       should probably always be 64 as requested by the RFC */
    int       ipv6PrefixLen;

    /* default IPv6 gateway address; only if TCPIP_NETWORK_CONFIG_IPV6_ADDRESS specified
       can be NULL if not needed*/
    char*     ipv6Gateway; 
}TCPIP_NETWORK_CONFIG;


// *****************************************************************************
/* TCP/IP module initialization/configuration structure

  Summary:
    Definition to represent a TCP/IP module initialization/configuration structure.

  Description:
    This type describes a TCP/IP module initialization/configuration data type
    that's passed to a TCP/IP module at the stack initialization time.

  Remarks:
    Each stack module will be configured with a user-defined initialization/configuration 
    structure.
*/

typedef struct
{
	TCPIP_STACK_MODULE		moduleId;
	const void * const		configData;
}TCPIP_STACK_MODULE_CONFIG;

// *****************************************************************************
/* TCP/IP stack initialization/configuration structure

  Summary:
    Definition to represent the TCP/IP stack initialization/configuration structure.

  Description:
    This type describes the TCP/IP stack initialization/configuration data type
    that's passed to the TCP/IP stack at the initialization time.

  Remarks:
    This data structure will be passed to the stack initialization function,
    TCPIP_STACK_Initialize.
*/

typedef struct
{
    /* system module initialization     */
    SYS_MODULE_INIT                     moduleInit; 
    /* pointer to array of network configurations */
    const TCPIP_NETWORK_CONFIG*         pNetConf;   
    /* number of networks in the configuration array */
    int                                 nNets;      
    /* pointer to array of module configurations */
    const TCPIP_STACK_MODULE_CONFIG*    pModConfig; 
    /* number of modules in the array  */
    int                                 nModules;   
}TCPIP_STACK_INIT;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#if 0

#include "tcpip/tcpip_common_ports.h"

#include "tcpip/tcpip_mac.h"
#include "tcpip/tcpip_mac_object.h"
#include "tcpip/tcpip_manager.h"


#include "tcpip/tcpip_heap.h"
#include "tcpip/tcpip_helpers.h"

#include "tcpip/ndp.h"
#include "tcpip/ipv4.h"
#include "tcpip/ipv6.h"
#include "tcpip/icmpv6.h"
#include "tcpip/dhcpv6.h"
#include "tcpip/arp.h"

#include "tcpip/udp.h"
#include "tcpip/tcp.h"
#include "tcpip/berkeley_api.h"
#include "tcpip/dhcp.h"
#include "tcpip/dhcps.h"
#include "tcpip/zero_conf_link_local.h"
#include "tcpip/zero_conf_multicast_dns.h"
#include "tcpip/dns.h"
#include "tcpip/dnss.h"
#include "tcpip/ftp.h"
#include "tcpip/icmp.h"
#include "tcpip/nbns.h"
#include "tcpip/ddns.h"
#include "tcpip/telnet.h"
#include "tcpip/smtp.h"
#include "tcpip/sntp.h"

#include "tcpip/http.h"
#include "tcpip/http_net.h"
#include "tcpip/snmp.h"
#include "tcpip/snmpv3.h"
#include "tcpip/tcpip_announce.h"
#include "tcpip/lldp.h"
#include "tcpip/smtpc.h"
#include "tcpip/igmp.h"

#include "tcpip/tftpc.h"
#include "tcpip/tcpip_reboot.h"
#include "tcpip/iperf.h"
#include "tcpip/tcpip_commands.h"

#endif

#include "tcpip/tcpip_mac.h"
#include "tcpip/tcpip_mac_object.h"
#include "tcpip/tcpip_manager.h"

#include "tcpip/tcpip_heap.h"
#include "tcpip/tcpip_helpers.h"

#include "tcpip/ipv4.h"
#include "tcpip/ipv6.h"

#endif  // __TCPIP_H__
