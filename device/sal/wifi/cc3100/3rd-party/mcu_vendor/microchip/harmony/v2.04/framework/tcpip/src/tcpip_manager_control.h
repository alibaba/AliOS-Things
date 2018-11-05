/*******************************************************************************
  Microchip TCP/IP Stack Definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_manager_control.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
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
// DOM-IGNORE-END

#ifndef _TCPIP_MANAGER_CONTROL_H_
#define _TCPIP_MANAGER_CONTROL_H_

#include "tcpip/tcpip_mac.h"

// supported ETH frame types that are processed by the stack
//

#define TCPIP_ETHER_TYPE_IPV4      	(0x0800u)
#define TCPIP_ETHER_TYPE_IPV6       (0x86DDu)
#define TCPIP_ETHER_TYPE_ARP     	(0x0806u)
#define TCPIP_ETHER_TYPE_LLDP     	(0x88CCu)
#define TCPIP_ETHER_TYPE_UNKNOWN    (0xFFFFu)


// module signal/timeout/asynchronous event handler
// the stack manager calls it when there's an signal/tmo/asynchronous event pending
// it should clear the pending status
typedef void    (*tcpipModuleSignalHandler)(void);

typedef enum
{
    TCPIP_MODULE_FLAG_NONE              = 0x0000,       // no flag set
                                                        //


}TCPIP_MODULE_FLAGS;

// types of packets/frames processed by this stack
typedef struct
{
    uint16_t        frameType;      // one of valid TCPIP_ETHER_TYPE_ values 
    uint16_t        pktTypeFlags;   // user packet flags to set for this type
    uint16_t        moduleId;       // TCPIP_STACK_MODULE: corresponding module handling this

}TCPIP_FRAME_PROCESS_ENTRY;


// handle for the registration of a signal handler
typedef const void* tcpipSignalHandle;



typedef enum
{
    TCPIP_STACK_ADDRESS_SERVICE_NONE    = 0,    // no service
    TCPIP_STACK_ADDRESS_SERVICE_DHCPC,          // DHCP client
    TCPIP_STACK_ADDRESS_SERVICE_ZCLL,           // ZCLL
    TCPIP_STACK_ADDRESS_SERVICE_DHCPS,          // DHCP server
    //
    TCPIP_STACK_ADDRESS_SERVICE_MASK    = 0x7,     // mask of address services
                                                // hast to match the position in the TCPIP_STACK_NET_IF_FLAGS!!!
}TCPIP_STACK_ADDRESS_SERVICE_TYPE;

typedef enum
{
    TCPIP_STACK_DNS_SERVICE_NONE    = 0,        // no service
    TCPIP_STACK_DNS_SERVICE_CLIENT,             // DNS Client
    TCPIP_STACK_DNS_SERVICE_SERVER,             // DNS Server
    TCPIP_STACK_DNS_SERVICE_MASK    = 0x18,     // mask of DNS services
                                                // hast to match the position in the TCPIP_STACK_NET_IF_FLAGS!!!
}TCPIP_STACK_DNS_SERVICE_TYPE;

typedef enum
{
    TCPIP_STACK_ADDRESS_SERVICE_EVENT_NONE    = 0,      // no event
    TCPIP_STACK_ADDRESS_SERVICE_EVENT_RUN_FAIL,         // run time failure event
    TCPIP_STACK_ADDRESS_SERVICE_EVENT_CONN_LOST,        // conn lost event
    TCPIP_STACK_ADDRESS_SERVICE_EVENT_USER_STOP,        // user stop event
    TCPIP_STACK_ADDRESS_SERVICE_EVENT_RUN_RESTORE,      // run time restore event event
                                                        // DHCPc uses this
}TCPIP_STACK_ADDRESS_SERVICE_EVENT;

typedef union
{
    struct
    {
        // stack configuration dynamic flags
        uint16_t bIsDHCPEnabled         : 1;    // init: controls the DHCP enable/disable on the interface
                                                // runtime: mirror bit set by the DHCP to reflect the current/last status
        uint16_t bIsZcllEnabled         : 1;    // init: controls the ZCLL service enable/disable on the interface
                                                // runtime: mirror bit set by the ZCLL Server to reflect the current/last status
        uint16_t bIsDHCPSrvEnabled      : 1;    // init: controls the DHCP Server enable/disable on the interface
                                                // runtime: mirror bit set by the DHCP Server to reflect the current/last status
                                                // TCPIP_STACK_ADDRESS_SERVICE_MASK has to match!!!
		uint16_t bIsDnsClientEnabled    : 1;    // DNS client  enable/disable  .
        uint16_t bIsDnsServerEnabled    : 1;    // DNS server Enable and Disable
        uint16_t bIsDNSServerAuto       : 1;    // DNS Server auto enable/disable on this interface
        uint16_t bInterfaceEnabled      : 1;    // 0 when TCPIP_MAC_POWER_DOWN/TCPIP_MAC_POWER_LOW 
        uint16_t bIPv6Enabled           : 1;
        uint16_t bIPv6InConfig          : 1;
volatile uint16_t bNewTcpipEventAvlbl   : 1;    // event available flag
        uint16_t powerMode              : 2;    // current power mode
        uint16_t bInConfig              : 1;    // stack is configuring itself
        uint16_t bMacProcessOnEvent     : 1;    // TCPIP_MAC_Process function needed on event
        uint16_t bMacInitialize         : 1;    // MAC is initializing
        uint16_t bMacInitDone           : 1;    // MAC initializing done
    };
    uint16_t v;
}TCPIP_STACK_NET_IF_FLAGS;      // flags per interface

// network interface descriptor
// 4 bytes aligned!
typedef struct
{
    uint16_t        size;                   // structure size; used in the configuration save/restore
    uint16_t        macId;                  // corresponding MAC id
	IPV4_ADDR		netIPAddr;              // IP address; currently only one IP add per interface
	IPV4_ADDR		netMask;                // Subnet mask
	IPV4_ADDR		netGateway;             // Gateway
	IPV4_ADDR		dnsServer[2];           // Primary + Secondary DNS Servers
	IPV4_ADDR		DefaultIPAddr;          // Default IP address
	IPV4_ADDR		DefaultMask;            // Default subnet mask
	IPV4_ADDR		DefaultGateway;         // Default Gateway
	IPV4_ADDR		DefaultDNSServer[2];    // Default DNS Servers; primary and secondary
	uint8_t		    NetBIOSName[16];        // NetBIOS name
	TCPIP_MAC_ADDR	netMACAddr;             // MAC address
    TCPIP_STACK_NET_IF_FLAGS Flags;
    const TCPIP_MAC_OBJECT*  pMacObj;   // MAC object associated with this interface
#if defined(TCPIP_STACK_USE_IPV6)
    uint16_t        startFlags;  // TCPIP_NETWORK_CONFIG_FLAGS: flags for interface start up
    uint16_t        ipv6PrefixLen;  // IPv6 subnet ID
    IPV6_ADDR       netIPv6Addr;    // static IPv6 address
    IPV6_ADDR       netIPv6Gateway; // default IPv6 gateway
#else
    uint32_t        startFlags;  // TCPIP_NETWORK_CONFIG_FLAGS: flags for interface start up
#endif
}TCPIP_STACK_NET_IF_DCPT;



#include "tcpip/src/tcpip_helpers_private.h"

// TCPIP network structure containing interface information
//  
typedef struct _tag_TCPIP_NET_IF
{
    struct  // TCPIP_STACK_NET_IF_DCPT!!!; Anonymous for now;
    // KEEP IN SYNC! ALIGNED! ALWAYS AT BEGINNING!
    {
        uint16_t        size;                   // structure size; used in the configuration save/restore
        uint16_t        macId;                  // corresponding MAC id
        IPV4_ADDR		netIPAddr;              // IP address; currently only one IP add per interface
        IPV4_ADDR		netMask;                // Subnet mask
        IPV4_ADDR		netGateway;             // Gateway
        IPV4_ADDR		dnsServer[2];           // Primary + Secondary DNS Servers
        IPV4_ADDR		DefaultIPAddr;          // Default IP address
        IPV4_ADDR		DefaultMask;            // Default subnet mask
        IPV4_ADDR		DefaultGateway;         // Default Gateway
        IPV4_ADDR		DefaultDNSServer[2];    // Default DNS Servers; primary and secondary
        uint8_t		    NetBIOSName[16];        // NetBIOS name
        TCPIP_MAC_ADDR	netMACAddr;             // MAC address
        TCPIP_STACK_NET_IF_FLAGS Flags;
        const TCPIP_MAC_OBJECT*  pMacObj;   // MAC object associated with this interface
#if defined(TCPIP_STACK_USE_IPV6)
        uint16_t        startFlags;  // TCPIP_NETWORK_CONFIG_FLAGS: flags for interface start up
        uint16_t        ipv6PrefixLen;  // IPv6 subnet ID
        IPV6_ADDR       netIPv6Addr;    // static IPv6 address
        IPV6_ADDR       netIPv6Gateway; // default IPv6 gateway
#else
        uint32_t        startFlags;  // TCPIP_NETWORK_CONFIG_FLAGS: flags for interface start up
#endif
    };

    // NOTE the alignmment!
    SYS_MODULE_OBJ      macObjHandle;           // MAC system wide handle
    TCPIP_MAC_HANDLE    hIfMac;                 // quick reference to which MAC this interface belongs to
    const void*         pMacConfig;             // MAC configuration data save for a restart
#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
    struct _tag_TCPIP_NET_IF* pPriIf;           // pointer to the primary interface
                                                // pPriIf == pIf, means primary
                                                // else alias  
    struct _tag_TCPIP_NET_IF* pAlias;           // linked list of alias interfaces belonging to this primary
#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
                                                // ((__aligned__)) !!!
#if defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)
    PROTECTED_SINGLE_LIST         registeredClients;      // stack notification clients
                                                // ((__aligned__)) !!!
#endif  // defined(TCPIP_STACK_USE_EVENT_NOTIFICATION) && (TCPIP_STACK_USER_NOTIFICATION != 0)
                                                // ((__aligned__)) !!!
    uint16_t            netIfIx;                // index of this entry in the tcpipNetIf table.
                                                // netIfIx = (this - &tcpipNetIf[0])/sizeof(TCPIP_NET_IF)
                                                // stored to avoid the run time division
    uint16_t            activeEvents;           // TCPIP_MAC_EVENT accumulated event available
                                                // TCPIP_MAC_EVENT fits within 16 bits!
                                                // link up/down events are not 
                                                // necessarily triggered by hardware
    uint16_t            currEvents;             // current TCPIP_MAC_EVENT processed event
    uint16_t            linkMtu;                // current interface link MTU
    union
    {
        struct
        {
            uint16_t linkPrev       : 1;        // previous status of the link
            uint16_t connEvent      : 1;        // when set indicates a connection event
            uint16_t connEventType  : 1;        // 0: TCPIP_MAC_EV_CONN_LOST; 1: TCPIP_MAC_EV_CONN_ESTABLISHED
            uint16_t reserved       : 13;       // available
        };
        uint16_t        v;
    }exFlags;                               // additional extended flags      
    char                ifName[7];          // native interface name + \0
    uint8_t             macType;            // a TCPIP_MAC_TYPE value: ETH, Wi-Fi, etc; 
} TCPIP_NET_IF;


// TCPIP stack event registration

typedef struct  _TAG_TCPIP_LIST_NODE
{
	struct _TAG_TCPIP_LIST_NODE *next;	// next node in list
                                                // safe cast to SGL_LIST_NODE node!!!
    TCPIP_STACK_EVENT_HANDLER   handler;    // handler to be called for event
    const void*                 hParam;     // handler parameter
    TCPIP_EVENT                 evMask;     // event that triggers the notification
    TCPIP_NET_IF*               pNetIf;     // interface it belongs to
}TCPIP_EVENT_LIST_NODE;


// network interface action for initialization/deinitialization
typedef enum
{
    TCPIP_STACK_ACTION_INIT,         // stack is initialized 
    TCPIP_STACK_ACTION_REINIT,       // stack is reinitialized
    TCPIP_STACK_ACTION_DEINIT,       // stack is deinitialized
    TCPIP_STACK_ACTION_IF_UP,        // interface is brought up 
    TCPIP_STACK_ACTION_IF_DOWN,      // interface is brought down
}TCPIP_STACK_ACTION;


// data that's passed as reference to each other module init/deinit, etc.
typedef struct _TCPIP_STACK_MODULE_CTRL
{
    // permanent data; this data is maintained by the stack for one full session
    // i.e., across StackInit() -> StackDeInit() calls
    // 
    //
    // number of the interfaces supported in this session
    int     nIfs;         
    int     nAliases;       // number of alias interfaces in this session         
	// number of the modules enabled in this session
	int 	nModules;
    // allocation parameters
    const void* memH;                   // handle to be used in the TCPIP_HEAP_ calls
    TCPIP_STACK_HEAP_TYPE   heapType;   // type of the heap                

    // transient data; contains info for a specific call
    //
    //
    // pointer to the current interface
    TCPIP_NET_IF* pNetIf;
    // index of the current interface addressed
    int     netIx;
    // current action for the stack
    TCPIP_STACK_ACTION      stackAction;
    // the power mode for this interface to go to
    // valid only if stackAction == init/reinit; ignored for deinit
    TCPIP_MAC_POWER_MODE  powerMode;
    
}TCPIP_STACK_MODULE_CTRL;


TCPIP_NET_IF*         TCPIP_STACK_IPAddToNet(IPV4_ADDR* pIpAddress, bool useDefault);

TCPIP_NET_IF*         _TCPIPStackIpAddFromAnyNet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* pIpAddress);


static __inline__ bool __attribute__((always_inline)) _TCPIPStackIpAddFromLAN(TCPIP_NET_IF* pIf, const IPV4_ADDR* pIpAddress)
{
    return ((pIf->netIPAddr.Val ^ pIpAddress->Val) & pIf->netMask.Val) == 0;
}

int  TCPIP_STACK_NetIxGet(TCPIP_NET_IF* pNetIf);


uint32_t  TCPIP_STACK_NetAddressGet(TCPIP_NET_IF* pNetIf);

const IPV6_ADDR* TCPIP_STACK_NetStaticIPv6AddressGet(TCPIP_NET_IF* pNetIf, int* pPrefixLen);

const IPV6_ADDR* TCPIP_STACK_NetDefaultIPv6GatewayGet(TCPIP_NET_IF* pNetIf);


uint32_t  TCPIP_STACK_NetMaskGet(TCPIP_NET_IF* pNetIf);

void  TCPIP_STACK_GatewayAddressSet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* ipAddress);
void  TCPIP_STACK_PrimaryDNSAddressSet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* ipAddress);

void  TCPIP_STACK_SecondaryDNSAddressSet(TCPIP_NET_IF* pNetIf, IPV4_ADDR* ipAddress);

TCPIP_NET_IF*         TCPIP_STACK_NetByAddress(const IPV4_ADDR* pIpAddress);


bool  TCPIP_STACK_AddressIsOfNetUp( TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd);

// detects net-directed bcast
bool  TCPIP_STACK_NetIsBcastAddress(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd);

// detects limited or net-directed bcast
bool  TCPIP_STACK_IsBcastAddress(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd);


bool  TCPIP_STACK_NetworkIsLinked(TCPIP_NET_IF* pNetIf);

TCPIP_NET_IF*         TCPIP_STACK_MACIdToNet(TCPIP_STACK_MODULE macId);

TCPIP_STACK_MODULE  TCPIP_STACK_NetMACId(TCPIP_NET_IF* pNetIf);

TCPIP_NET_IF*     TCPIP_STACK_MacToNet(TCPIP_MAC_HANDLE hMac);

TCPIP_MAC_HANDLE  TCPIP_STACK_NetToMAC(TCPIP_NET_IF* pNetIf);

int         TCPIP_STACK_MacToNetIndex(TCPIP_MAC_HANDLE hMac);



const uint8_t*  TCPIP_STACK_NetMACAddressGet(TCPIP_NET_IF* pNetIf);

static __inline__ uint32_t  __attribute__((always_inline)) _TCPIPStackNetAddress(TCPIP_NET_IF* pNetIf)
{
    return pNetIf->netIPAddr.Val;
}



static __inline__ bool  __attribute__((always_inline)) TCPIP_STACK_AddressIsOfNet(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pIpAdd)
{
    return (pIpAdd->Val == 0x0100007f || pNetIf->netIPAddr.Val == pIpAdd->Val);
}

static __inline__ TCPIP_NET_IF*  __attribute__((always_inline)) _TCPIPStackHandleToNet(TCPIP_NET_HANDLE hNet)
{
    // do some checking
    // if #debug enabled, etc
    return (TCPIP_NET_IF*)hNet; 
}

static __inline__ bool  __attribute__((always_inline)) TCPIP_STACK_NetworkIsUp(TCPIP_NET_IF* pNetIf)
{
    if(pNetIf && pNetIf->Flags.bInterfaceEnabled)
    {
        return true;
    }
    return false;
}

static __inline__ const uint8_t*  __attribute__((always_inline)) _TCPIPStack_NetMACAddressGet(TCPIP_NET_IF* pNetIf)
{
    return pNetIf ? pNetIf->netMACAddr.v : 0;
}

static __inline__ const char*  __attribute__((always_inline)) _TCPIPStack_NetBIOSName(TCPIP_NET_IF* pNetIf)
{
    return pNetIf ? (char*)pNetIf->NetBIOSName : 0;
}

// checks for valid up interface
static __inline__ TCPIP_NET_IF*  __attribute__((always_inline)) _TCPIPStackHandleToNetUp(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = _TCPIPStackHandleToNet(hNet);
    if(TCPIP_STACK_NetworkIsUp(pNetIf))
    {
        return pNetIf;
    }

    return 0;
}

// checks for valid up and linked interface
TCPIP_NET_IF* _TCPIPStackHandleToNetLinked(TCPIP_NET_HANDLE hNet);

// returns a valid, linked interface, if any
// can start search with the default one
TCPIP_NET_IF* _TCPIPStackAnyNetLinked(bool useDefault);

// converts between an interface name and a MAC (TCPIP_STACK_MODULE) ID 
// TCPIP_MODULE_MAC_NONE - no MAC id could be found
TCPIP_STACK_MODULE    TCPIP_STACK_StringToMACId(const char* str);

// string (interface name) corresponding to the given MAC id.
// NULL if no such MAC id.
const char*     TCPIP_STACK_MACIdToString(TCPIP_STACK_MODULE macId);

// helper to select an address service for interface
TCPIP_STACK_ADDRESS_SERVICE_TYPE TCPIP_STACK_AddressServiceSelect(TCPIP_NET_IF* pNetIf, TCPIP_NETWORK_CONFIG_FLAGS configFlags);
//Helper to select an DNS service  for  a interface
TCPIP_STACK_DNS_SERVICE_TYPE TCPIP_STACK_DNSServiceSelect(TCPIP_NET_IF* pNetIf, TCPIP_NETWORK_CONFIG_FLAGS configFlags);


// helper to decide if an address service can be started
bool    TCPIP_STACK_AddressServiceCanStart(TCPIP_NET_IF* pNetIf, TCPIP_STACK_ADDRESS_SERVICE_TYPE adSvcType);

// helper to decide if an DNS  service can be started
bool TCPIP_STACK_DNSServiceCanStart(TCPIP_NET_IF* pNetIf, TCPIP_STACK_DNS_SERVICE_TYPE dnsSvcType);


// notification helper that an address service has some stop event
void    TCPIP_STACK_AddressServiceEvent(TCPIP_NET_IF* pNetIf, TCPIP_STACK_ADDRESS_SERVICE_TYPE adSvcType, TCPIP_STACK_ADDRESS_SERVICE_EVENT evType);

// address service default status
void    TCPIP_STACK_AddressServiceDefaultSet(TCPIP_NET_IF* pNetIf);

// helper to check if an address service is running
TCPIP_STACK_ADDRESS_SERVICE_TYPE _TCPIPStackAddressServiceIsRunning(TCPIP_NET_IF* pNetIf);

static __inline__ bool __attribute__((always_inline)) _TCPIPStackIsConfig(TCPIP_NET_IF* pNetIf)
{
    return pNetIf->Flags.bInConfig != 0;
}


void  _TCPIPStackSetConfigAddress(TCPIP_NET_IF* pNetIf, IPV4_ADDR* ipAddress, IPV4_ADDR* mask, bool config);

// finds an interface that has the IPv6 address
TCPIP_NET_IF* _TCPIPStackIPv6AddToNet(IPV6_ADDR* pIPv6Address, IPV6_ADDR_TYPE addType, bool useDefault);

// register a signal/timeout handler with the stack manager
// returns 0 if failed, a valid handle otherwise
// the entries are identified by the signalHandler;
// a second call with the same signalHandler will succeed and not create a new signal entry
tcpipSignalHandle _TCPIPStackSignalHandlerRegister(TCPIP_STACK_MODULE modId, tcpipModuleSignalHandler signalHandler, int16_t asyncTmoMs);


// changes the parameters for a registered signal handler
// asyncTmo == 0 could be used to stop the timer without deregistration
bool           _TCPIPStackSignalHandlerSetParams(TCPIP_STACK_MODULE modId, tcpipSignalHandle handle, int16_t asyncTmoMs);


// returns the pending module signals
TCPIP_MODULE_SIGNAL  _TCPIPStackModuleSignalGet(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL clrMask);

// returns the pending module signals + signal parameter
// signalParam should NOT be 0!
// the signal parameter is cleared after this operation, there's is no peek!
TCPIP_MODULE_SIGNAL  _TCPIPStackModuleSignalParamGet(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL clrMask, uint32_t* signalParam);

// a stack module requests setting a signal
// This function is mainly intended for requiring attention
// and the signal to be set is TCPIP_MODULE_SIGNAL_ASYNC!
// There is no need for a module to set other signals if there's really
// no pending RX packets or timeouts.
//
// returns true if a valid module ID requested;
// The TCPIP_MODULE_MANAGER itself is invalid!
//
// NOTE:
//  a TCPIP_MODULE_SIGNAL_ASYNC is monitored by the stack manager so that as long as it is active
//  the module gets special attention.
//  This is done to allow a module to set the TCPIP_MODULE_SIGNAL_ASYNC signal just once
//  instead of continuously request it.
//  When done with the critical section the module should clear the TCPIP_MODULE_SIGNAL_ASYNC signal.
//  However, the stack manager keeps a global count of how many times the modules required TCPIP_MODULE_SIGNAL_ASYNC
//  That means that a module should always call
//  _TCPIPStackModuleSignalRequest/_TCPIPStackModuleSignalGet for TCPIP_MODULE_SIGNAL_ASYNC in pairs!
//
// NOTE:
//  A signal request sends a signal to the module task itself
//  A signal request will also send a signal to the stack task itself, when the TCP/IP modules are executed internally.
//  - unless noMgrAlert is specified
//
bool                _TCPIPStackModuleSignalRequest(TCPIP_STACK_MODULE modId, TCPIP_MODULE_SIGNAL signal, bool noMgrAlert);


// de-registers a previous registered signal handler
void           _TCPIPStackSignalHandlerDeregister(tcpipSignalHandle handle); 


// inserts a RX packet into the interface RX queue
// this has to be a fully formatted TCPIP_MAC_PACKET
void           _TCPIPStackInsertRxPacket(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET* pRxPkt, bool signal);


TCPIP_MAC_RES _TCPIPStackPacketTx(TCPIP_NET_IF* pNetIf, TCPIP_MAC_PACKET * ptrPacket);


// extracts a packet from a module RX queue
// returns 0 if queue is empty
TCPIP_MAC_PACKET*   _TCPIPStackModuleRxExtract(TCPIP_STACK_MODULE modId);

// inserts a packet into a module queue
// and signals if necessary
void _TCPIPStackModuleRxInsert(TCPIP_STACK_MODULE modId, TCPIP_MAC_PACKET* pRxPkt, bool signal);


// purges the packets from a module RX queue
// belonging to the pNetIf
void _TCPIPStackModuleRxPurge(TCPIP_STACK_MODULE modId, TCPIP_NET_IF* pNetIf);

// return the stack heap configuration parameters
const TCPIP_STACK_HEAP_CONFIG* _TCPIPStackHeapConfig(void);
            
static __inline__ uint16_t  __attribute__((always_inline)) _TCPIPStackNetLinkMtu(TCPIP_NET_IF* pNetIf)
{
    return pNetIf ? pNetIf->linkMtu : 0;
}

#if (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)

static __inline__ bool  __attribute__((always_inline)) _TCPIPStackNetIsPrimary(TCPIP_NET_IF* pNetIf)
{
    return pNetIf == pNetIf->pPriIf;
}

static __inline__ TCPIP_NET_IF*  __attribute__((always_inline)) _TCPIPStackNetGetPrimary(TCPIP_NET_IF* pNetIf)
{
    return pNetIf->pPriIf;
}

static __inline__ TCPIP_NET_IF*  __attribute__((always_inline)) _TCPIPStackNetGetAlias(TCPIP_NET_IF* pNetIf)
{
    return pNetIf->pAlias;
}

// maps the proper alias network a packet belongs to
// based on the interface the packet arrived on
// (probably a primary interface) and on the packet IP address
// No check is done if the interface (p or a) is UP!
TCPIP_NET_IF* _TCPIPStackMapAliasInterface(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pDestAddress);

#else
static __inline__ bool  __attribute__((always_inline)) _TCPIPStackNetIsPrimary(TCPIP_NET_IF* pNetIf)
{
    return true;
}

static __inline__ TCPIP_NET_IF*  __attribute__((always_inline)) _TCPIPStackNetGetPrimary(TCPIP_NET_IF* pNetIf)
{
    return pNetIf;
}

static __inline__ TCPIP_NET_IF*  __attribute__((always_inline)) _TCPIPStackNetGetAlias(TCPIP_NET_IF* pNetIf)
{
    return 0;
}

static __inline__ TCPIP_NET_IF*  __attribute__((always_inline)) _TCPIPStackMapAliasInterface(TCPIP_NET_IF* pNetIf, const IPV4_ADDR* pDestAddress)
{
    return pNetIf;
}



#endif  // (_TCPIP_STACK_ALIAS_INTERFACE_SUPPORT)
// debugging, tracing, etc.

// enables the measurement of the CPU time taken by the TCPIP_STACK_Task() processing
// Uses the CP0 count register, SYS_FREQ/2 resolution
//#define TCPIP_STACK_TIME_MEASUREMENT


#if defined(TCPIP_STACK_TIME_MEASUREMENT)

void        TCPIP_STACK_TimeMeasureStart(bool reset);

uint64_t    TCPIP_STACK_TimeMeasureGet(bool stop);    


#endif // defined(TCPIP_STACK_TIME_MEASUREMENT)




#endif  // _TCPIP_MANAGER_CONTROL_H_


