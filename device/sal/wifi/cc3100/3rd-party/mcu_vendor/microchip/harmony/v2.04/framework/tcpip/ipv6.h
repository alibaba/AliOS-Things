/*******************************************************************************
  IPv6 Definitions for the Microchip TCP/IP Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    ipv6.h

  Summary:
    IPv6 is the next generation TCP/IP protocol suite.

  Description:
    Like IPv4 , IPv6 (Internet Protocol Version 6) is the another version of IP protocol.
	This is the next-generation protocol  which works parallel with IPv4. Microchip TCP/IP 
	stack is a dual stack architecture where both Ipv4 and Ipv6 works simultaneously.
	(RFC - 2460,3484)
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
//DOM-IGNORE-END

#ifndef __IPV6_H_
#define __IPV6_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: public definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* 
  Enumeration:
	IPV6_NEXT_HEADER_TYPE

  Summary:
    Defines a list of IPv6 next header types.

  Description:
    Identifies the type of the next header immediately follows IPv6 header.

  Remarks:
    None
 */
typedef enum
{
    IPV6_PROT_HOP_BY_HOP_OPTIONS_HEADER = (0u), // IPv6 Hop-by-Hop Opt. Header
    IPV6_PROT_ICMP = (1u), // ICMPv4 Header
    IPV6_PROT_TCP = (6u), // TCP protocol Header
    IPV6_PROT_UDP = (17u), // UDP protocol Header
    IPV6_PROT_IPV6 = (41u), // IPv6 Protocol
    IPV6_PROT_ROUTING_HEADER = (43u), // IPv6 Routing Header
    IPV6_PROT_FRAGMENTATION_HEADER = (44u), // IPv6 Fragmentation Header
    IPV6_PROT_ENCAPSULATING_SECURITY_PAYLOAD_HEADER = (50u), // Encapsulating Security Payload Header
    IPV6_PROT_AUTHENTICATION_HEADER = (51u), // Authentication Header
    IPV6_PROT_ICMPV6 = (58u), // ICMPv6 Protocol
    IPV6_PROT_NONE = (59u), // No next header
    IPV6_PROT_DESTINATION_OPTIONS_HEADER = (60u) // Destination Options Header
} IPV6_NEXT_HEADER_TYPE;

// IP Version details
#define IP_VERSION_6    (1u) // Using IPv6 
#define IP_VERSION_4    (0u) // Using IPv4

// *****************************************************************************
/* 
  Enumeration:
	IPV6_ACTION 

  Summary:
    Provides a list of possible IPv6 actions.

  Description:
    Different actions need to be taken depending on the result of our header processing.

  Remarks:
    None.
 */
typedef enum {
    IPV6_ACTION_NONE = 0, // No action. Skip the option 
    IPV6_ACTION_DISCARD_SILENT, // Discard the packet silently
    IPV6_ACTION_DISCARD_PP_0, // Discard the packet and send an ICMP parameter problem message with code value 0
    IPV6_ACTION_DISCARD_PP_2, // Discard the packet and send an ICMP parameter problem message with code value 2
    IPV6_ACTION_DISCARD_PP_2_NOT_MC, // Discard the packet and send an ICMP parameter problem message with code value 3
    IPV6_ACTION_BEGIN_EX_HEADER_PROCESSING // Begin extension processing
} IPV6_ACTION;

/* IPv6 Type-length-value type code for the Pad 1 option */
#define IPV6_TLV_PAD_1                          0u
/* IPv6 Type-length-value type code for the Pad N option */
#define IPV6_TLV_PAD_N                          1u
/* IPv6 Type-length-value type code for the Hop-by-hop "Jumbo-gram Payload" option */
#define IPV6_TLV_HBHO_PAYLOAD_JUMBOGRAM         0xC2u
/* IPv6 Type-length-value type code for the Hop-by-hop "Router Alert" option */
#define IPV6_TLV_HBHO_ROUTER_ALERT              0x05u

/* IPv6 action code for the unrecognized option reaction to skip the option */
#define IPV6_TLV_UNREC_OPT_SKIP_OPTION          0b00
/* IPv6 action code for the unrecognized option reaction to discard the packet silently */
#define IPV6_TLV_UNREC_OPT_DISCARD_SILENT       0b01
/* IPv6 action code for the unrecognized option reaction to discard the packet and send 
an ICMP parameter problem message 
*/
#define IPV6_TLV_UNREC_OPT_DISCARD_PP           0b10
/* IPv6 action code for the unrecognized option reaction to discard the packet and send
 an ICMP parameter problem message is the destination address isn't a multicast address
*/
#define IPV6_TLV_UNREC_OPT_DISCARD_PP_NOT_MC    0b11

/* Header offset for payload length */
#define IPV6_HEADER_OFFSET_PAYLOAD_LENGTH       (0x04u) 
/* Header offset for next header */
#define IPV6_HEADER_OFFSET_NEXT_HEADER          (0x06u) 
/* Header offset for source address */
#define IPV6_HEADER_OFFSET_SOURCE_ADDR          (0x08u)
/* Header offset for destination address */
#define IPV6_HEADER_OFFSET_DEST_ADDR            (0x08u + sizeof (IPV6_ADDR)) 


// *****************************************************************************
/* 
  Enumeration: 
	IPV6_ADDRESS_PREFERENCE 

  Summary:
    Provides selection of public versus temporary addresses.

  Description:
    None

  Remarks:
    None.
 */
typedef enum
{
    IPV6_PREFER_PUBLIC_ADDRESSES = 0,
    IPV6_PREFER_TEMPORARY_ADDRESSES
} IPV6_ADDRESS_PREFERENCE;


// *****************************************************************************
/* 
  Enumeration: 
  IPV6_SEGMENT_TYPE 

  Summary:
    Provides an enumeration of IPv6 segment types.

  Description:
    IPv6 extended header.

  Remarks:
    None.
 */
typedef enum
{
    TYPE_IPV6_HEADER = 1u,
    TYPE_IPV6_EX_HEADER_HOP_BY_HOP_OPTIONS,
    TYPE_IPV6_EX_HEADER_DESTINATION_OPTIONS_1,
    TYPE_IPV6_EX_HEADER_ROUTING,
    TYPE_IPV6_EX_HEADER_FRAGMENT,
    TYPE_IPV6_EX_HEADER_AUTHENTICATION_HEADER,
    TYPE_IPV6_EX_HEADER_ENCAPSULATING_SECURITY_PAYLOAD,
    TYPE_IPV6_EX_HEADER_DESTINATION_OPTIONS_2,
    TYPE_IPV6_UPPER_LAYER_HEADER,
    TYPE_IPV6_UPPER_LAYER_PAYLOAD,
    TYPE_IPV6_BEGINNING_OF_WRITABLE_PART,
    TYPE_IPV6_END_OF_LIST
} IPV6_SEGMENT_TYPE;

//DOM-IGNORE-BEGIN
extern const IPV6_ADDR IPV6_FIXED_ADDR_UNSPECIFIED;
extern const IPV6_ADDR IPV6_FIXED_ADDR_ALL_NODES_MULTICAST;
extern const IPV6_ADDR IPV6_FIXED_ADDR_ALL_ROUTER_MULTICAST;
extern const IPV6_ADDR IPV6_SOLICITED_NODE_MULTICAST;
extern const TCPIP_MAC_ADDR  IPV6_MULTICAST_MAC_ADDRESS;
//DOM-IGNORE-END

// *****************************************************************************
/* 
  Type:
	IPV6_TLV_OPTION_TYPE

  Summary:
    Data structure for IPv6 TLV options.

  Description:
    This type defines the data structure for IPv6 TLV options.

  Remarks:
    None.
 */
typedef union
{
    unsigned char b;

    struct
    {
        unsigned option : 6;
        unsigned unrecognizedAction : 2;
    } bits;
} IPV6_TLV_OPTION_TYPE;


// *****************************************************************************
/* 
  Type:
	IPV6_ADDRESS_TYPE

  Summary:
    Data structure for IPv6 address types.

  Description:
    This type defines the data structure for IPv6 address types.

  Remarks:
    None.
 */
typedef union
{
    unsigned char byte;

    struct
    {
        unsigned scope : 4;
        unsigned type : 2;
    } bits;
} IPV6_ADDRESS_TYPE;


// *****************************************************************************
/* 
  Type:
	IPV6_ADDRESS_POLICY

  Summary:
	Data structure for IPv6 address policy.

  Description:
	The policy table is the longest matching prefix lookup table and is 
	used to select the destination IPv6 Address.
	
	If Precedence(A) > Precedence(B), address A has higher precedence than address B.
	The label value Label(A) allows for policies that prefer a particular
    source address prefix for use with a destination address prefix.
	Default Policy table -
	Prefix			Precedence	Label
	::1/128				50		0
	::/0				40		1
	2002::/16			30		2
	::/96				20		3
	::ffff:0.0.0.0/96	10		4
	
  Example -
	const IPV6_ADDRESS_POLICY gPolicyTable[] = {
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}}, 128, 50,  0},          // Loopback address
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},   0, 40,  1},            // Unspecified address
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00}},  96, 35,  4},           // IPv4-mapped address
    {{{0x20, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  16, 30,  2},           // 2002::/15 - 6to4
    {{{0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},  32,  5,  5},            // 2001::/32 - Teredo tunneling
    {{{0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},   7,  3, 13},            // ULA
	};
  
  Remarks:
    None.
 */
typedef struct
{
    IPV6_ADDR address; // IPv6 address
    unsigned char prefixLength; // IPv6 prefix length
    unsigned char precedence; // IPv6 address precedence
    unsigned char label; // IPv6 label type
} IPV6_ADDRESS_POLICY;


// *****************************************************************************
/* 
  Structure :
	IPV6_DATA_SEGMENT_HEADER

  Summary:
	Data structure for IPv6 Data Segment header.

  Description:
	Data structure is used to allocate a data segment header and an optional payload.

  Remarks:
    None.
 */
typedef struct _IPV6_DATA_SEGMENT_HEADER
{
    uint8_t* dataLocation; // Location of the data to transmit
    unsigned short segmentSize; // Size of this data segment
    unsigned short segmentLen; // Number of bytes of data in this segment
    unsigned char memory; // Type: IPV6_DATA_NONE, IPV6_DATA_DYNAMIC_BUFFER, IPV6_DATA_NETWORK_FIFO, IPV6_DATA_PIC_RAM
    unsigned char segmentType; // Type of segment contents
    struct _IPV6_DATA_SEGMENT_HEADER * nextSegment; // Pointer to the next data segment
    void * data[0]; // Optional buffer space
} IPV6_DATA_SEGMENT_HEADER;



// *****************************************************************************
/* 
  Structure: 
	IPV6_RX_FRAGMENT_BUFFER

  Summary:
	Data structure for IPv6 Received fragmented packet.

  Description:
	Each fragment is composed of unfragmentable Part and  fragmentable part.
	Allocate  memory for the fragmented packet w.r.t to TCPIP_IPV6_RX_FRAGMENTED_BUFFER_SIZE.
	The RFC specifies that the fragments must be reassembled in one minute or less.secondsRemaining
	is the times remaining for reassemble.  

  Remarks:
    None.
 */
typedef struct _IPV6_RX_FRAGMENT_BUFFER {
    struct _IPV6_RX_FRAGMENT_BUFFER * next; // Next fragmented packet    
    uint8_t *ptrPacket; // Packet information
    uint32_t identification; // Fragment id
    uint16_t bytesInPacket; // Number of bytes written to packet
    uint16_t packetSize; // Packet size (packet is complete when this matches bytesInPacket)
    uint16_t firstFragmentLength; // Length of the first fragment
    uint8_t secondsRemaining; // Number of seconds remaining during which the fragment can be reassembled
} IPV6_RX_FRAGMENT_BUFFER;


// *****************************************************************************
/* 
  Structure:
	IPV6_FRAGMENT_HEADER

  Summary:
	Data structure for IPv6 fragment header.

  Description:
	The Fragment header is used by an IPv6 source to send a packet larger
    than would fit in the path MTU to its destination.
	The Fragment header is identified by a Next Header value of 44 in the
    immediately preceding header.

  Remarks:
    None.
 */
typedef struct __attribute__((__packed__))
{
    uint8_t nextHeader;
    uint8_t reserved;

    union {

        struct __attribute__((__packed__))
        {
            uint16_t m : 1;
            uint16_t reserved2 : 2;
            uint16_t fragmentOffset : 13;
        }
        bits;
        uint16_t w;
    } offsetM;
    uint32_t identification;
} IPV6_FRAGMENT_HEADER;

#define IPV6_DATA_NONE                (0x0u)        // The data segment is unused
#define IPV6_DATA_DYNAMIC_BUFFER      (0x1u)        // Data to transmit is allocated in dynamically allocated RAM
#define IPV6_DATA_NETWORK_FIFO        (0x2u)        // Data to transmit is stored in the Network Controller's FIFOs
#define IPV6_DATA_PIC_RAM             (0x3u)        // Data to transmit is stored in PIC RAM

#define IPV6_NO_UPPER_LAYER_CHECKSUM  (0xFFFFu) 	// Value flag for no upper layer checksum


// *****************************************************************************
/* 
  Structure:
	IPV6_HEADER
	
  Summary:
    IPv6 packet header definition.

  Description:
    IPv6 packet header definition.

  Remarks:
    Any extension headers present are considered part of the payload length.
 */
typedef struct
{
    unsigned long V_T_F; // Version , Traffic class and Flow Label 
    unsigned short PayloadLength; // Length of IPv6 payload, i.e. the rest of packet following this IPv6 header in octets 
    unsigned char NextHeader; // Identifies the type of header immediately following IPv6 header
    unsigned char HopLimit; // Decremented by 1 by each node that forwards the packet
    IPV6_ADDR SourceAddress; // 128 bit address of originator
    IPV6_ADDR DestAddress; // 128 bit address of intended recipient
} IPV6_HEADER;


// *****************************************************************************
/* 
  Enumeration:
	IPV6_EVENT_TYPE 

  Summary:
    This enumeration is used to notify IPv6 client applications.

  Description:
    These events are used while notifying to the registered applications.

  Remarks:
    None.
 */
typedef enum {
    IPV6_EVENT_ADDRESS_ADDED = 1,  // Event is generated when IPv6 added to the list
    IPV6_EVENT_ADDRESS_REMOVED, // Event is generated when IPv6 address removed from the list
    //
    IPV6_EVENT_ULA_ADDRESS_GENERATED, // Event is generated when IPv6 unique local address included to the list
    IPV6_EVENT_ULA_ADDRESS_FAILED, // Event is generated when IPv6 unique local address is failed  

} IPV6_EVENT_TYPE;

// *****************************************************************************
/* Type:
	IPV6_HANDLE

  Summary:
    Notification handler that can be called for successful address addition and removed.

  Description:
    The format of a notification handler registered with the IPv6 module.
	Once an IPV6_EVENT_TYPE occurs the IPv6 module will be called for the registered handler.
	
  Remarks:
    If pNetIf == 0, the notification is called for events on any interface.
*/
typedef const void * IPV6_HANDLE; // Pointer to IPv6 object


// *****************************************************************************
/* 
  Structure:
	TCPIP_IPV6_MODULE_CONFIG

  Summary:
    Provides a place holder for IPv6 configuration.

  Description:
    IPv6 module runtime configuration and initialization parameters.

  Remarks:
    None.
 */
typedef struct
{
    uint32_t        rxfragmentBufSize;  // RX fragmented buffer size
    uint32_t        fragmentPktRxTimeout;  // fragmented packet time out value
}TCPIP_IPV6_MODULE_CONFIG;


// *****************************************************************************
/* 
  Type:
	IPV6_EVENT_HANDLER

  Summary:
    Clients can register a handler with the IPv6 service.

  Description:
    Once an IPv6 event occurs the IPv6 service will call the registered handler
    The handler has to be short and fast.
    It is meant for setting an event flag, not for lengthy processing!

    evParam is a parameter that's associated to an IPv6 event
    - For an address event (IPV6_EVENT_ADDRESS_ADDED, IPV6_EVENT_ADDRESS_REMOVED) it should typecast
      to (const IPV6_ADDR_STRUCT*)
    - For an IPV6_EVENT_ULA_ADDRESS_GENERATED ULA event it should typecast
      to (const IPV6_ADDR*)
    - For an IPV6_EVENT_ULA_ADDRESS_FAILED ULA event the evParam is an IPV6_ULA_RESULT error code

    The evParam is invalid outside of the IPV6_EVENT_HANDLER context call and should not be stored
    by the caller. Info that's needed has to be copied into caller's own context.

    usrParam is a user-supplied parameter

  Remarks:
    For address related events the passed (const IPV6_ADDR_STRUCT*) parameter
    is invalid after the notification call.

 */
typedef void (*IPV6_EVENT_HANDLER)(TCPIP_NET_HANDLE hNet, IPV6_EVENT_TYPE evType, const void* evParam, const void* usrParam);


// *****************************************************************************
/* 
  Type:
	IPV6_PACKET_ACK_FNC
	
  Summary:
    Packet allocation and deallocation acknowledgment callback function.

  Description:
    Packet allocation and deallocation acknowledgment callback function.
	 
  Parameters:
    void * - Pointer to the packet that was transmitted
    bool - True if the packet was sent, false otherwise
    const void* - 0
	
  Returns:
    -true  - if the packet needs the queuing flags removed (it is not deleted 
	         and still in use)
    -false - if the packet does not need the queuing flags removed (either no 
	         longer exists or the flags updated)

  Remarks:
    None.
*/
typedef bool (*IPV6_PACKET_ACK_FNC)(void*, bool, const void*);

//*****************************************************************************
/* 
  Structure:
	IPV6_PACKET
	
  Summary:
    Packet structure/state tracking for IPv6 packets.

  Description:
    IPv6 packets are queued for future transmission.queuedPacketTimeout is used to 
	time-out IPv6 queued packets.
	 
  Remarks:
    For IPv6 queuing the time out has to be 0!
    The queue is processed separately by the NDP.IPV6_PACKET_ACK_FNC is called after the 
	successful removal and inclusion of the packet.
*/
typedef struct _IPV6_PACKET
{
    struct _IPV6_PACKET * next;                 // Next packet in a queue
    unsigned short payloadLen;                  // Amount of data in payload buffer
    unsigned short headerLen;                   // Total header length (IP header + IPv6 Extension headers)
    unsigned short upperLayerHeaderLen;         // Total length of the upper layer header
    unsigned short upperLayerChecksumOffset;    // Offset of the upper layer checksum
    unsigned char upperLayerHeaderType;         // Type definition for the upper-layer header type
    union {
        struct {
            unsigned char reserved :         3;
            unsigned char useUnspecAddr :    1; // This packet should use the unspecified address
            unsigned char sourceSpecified :  1; // The upper layer or application layer specified a source address
            unsigned char queued :           1; // Packet has been queued
            unsigned char addressType :      2; // IP_ADDRESS_TYPE_IPV6 or IP_ADDRESS_TYPE_IPV4
        };
        unsigned char val;
    } flags;
    TCPIP_MAC_ADDR remoteMACAddr;               // The packet's remote MAC address
    IPV6_PACKET_ACK_FNC ackFnc;                 // function to be called when IPv6 done with the packet
    TCPIP_MAC_PACKET_ACK_FUNC macAckFnc;        // function to be called when MAC done with a TX packet
    void* ackParam;                             // parameter to be used
    void* clientData;                           // optional packet client data
    void * neighbor;                            // The neighbor that the message was received from    
    unsigned short offsetInSegment;             // Offset used for storing fragment transmission information
    uint32_t queuedPacketTimeout;				// Time out for IPv6 packets which are queued
    IPV6_DATA_SEGMENT_HEADER payload;			// IPv6 data segment payload
    TCPIP_NET_HANDLE netIfH;                    // packet network interface
    IPV6_HEADER ipv6Header;						// IPv6 header definition
} IPV6_PACKET;

// *****************************************************************************
/* 
  Enumeration:
	IPV6_ULA_FLAGS 

  Summary:
    Provides a list of possible ULA action flags.

  Description:
    This enumeration provides a list of possible flags for the Unique Local Address 
	(ULA) generation.

  Remarks:
    None.
 */
typedef enum
{
    IPV6_ULA_FLAG_NTPV4         = 0x01,     // use an IPv4 NTP server access in Unique Local Address generation
                                            // default is an IPv6 server
    IPV6_ULA_FLAG_GENERATE_ONLY = 0x02,     // generate an address only, don't add it to the interface addresses
    IPV6_ULA_FLAG_SKIP_DAD      = 0x04,     // when adding the address to the interface, skip the Duplicate Address Detection

} IPV6_ULA_FLAGS;

// *****************************************************************************
/* 
  Enumeration:
	IPV6_ULA_RESULT

  Summary:
    Provides a list of possible ULA results.

  Description:
    This enumeration provides a list of possible results for the Unique Local Address 
	(ULA) generation.

  Remarks:
    None.
 */
typedef enum
{
    // success codes
    IPV6_ULA_RES_OK,                // the address generation was started successfully

    // error codes
    IPV6_ULA_RES_BUSY               = -1,   // address generation module is busy
    IPV6_ULA_RES_IF_ERR             = -2,   // interface non existent
    IPV6_ULA_RES_NTP_ACCESS_ERR     = -3,   // NTP module could not be accessed
    IPV6_ULA_RES_NTP_TSTAMP_ERR     = -4,   // wrong NTP time stamp received

} IPV6_ULA_RESULT;


// *****************************************************************************
// *****************************************************************************
// Section: IPv6 Routines
// *****************************************************************************
// *****************************************************************************

//*****************************************************************************
/*
  Function:
    bool TCPIP_IPV6_InterfaceIsReady (TCPIP_NET_HANDLE netH)

  Summary:
    Determines if an interface is ready for IPv6 transactions.

  Description:
    Returns the current state of the IPv6 interface (i.e., determines 
	if an interface is ready for IPv6 transactions).

  Precondition:
    None.

  Parameters:
    netH - The interface to check

  Returns:
    - true  - if the interface has IPv6 functionality available
	- false - if the interface does not have IPv6 functionality available

  Remarks:
    None.
  */
bool TCPIP_IPV6_InterfaceIsReady(TCPIP_NET_HANDLE netH);


//*****************************************************************************
/*
  Function:
    IPV6_ADDR_STRUCT * TCPIP_IPV6_DASSourceAddressSelect (TCPIP_NET_HANDLE hNetIf, 
	                               IPV6_ADDR * dest, IPV6_ADDR * requestedSource)

  Summary:
    Determines the appropriate source address for a given destination
    address.

  Description:
    The IPv6 policy table is used to select the destination address. The destination 
	address selection algorithm takes a list of IPv6 addresses (gPolicyTable) and 
	sorts the linked list. There are eight sorting rules. Starting with the last rule 
	and working to the most important, using a stable sorting algorithm, will produce 
	a sorted list most  efficiently.  The best average run time we'll get with a stable 
	sort with O(1) memory usage is O(n^2), so we'll use an insertion sort.  This will 
	usually be most efficient for small lists (which should be the typical case).
	If a rule determines a result, then the remaining rules are not relevant and should be
    ignored.

  Precondition:
    TCPIP_IPV6_Initialize is required.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    hNetIf - The given interface
    dest - The destination address
    requestedSource - A specified source

  Returns:
	- On Success - Pointer to the selected source address
	- On Failure - NULL
	
  Remarks:
    None.
  */
IPV6_ADDR_STRUCT * TCPIP_IPV6_DASSourceAddressSelect(TCPIP_NET_HANDLE hNetIf, 
                       const IPV6_ADDR * dest, IPV6_ADDR * requestedSource);


//*****************************************************************************
/*
  Function:
    IPV6_PACKET * TCPIP_IPV6_TxPacketAllocate (TCPIP_NET_HANDLE netH, 
	                             IPV6_PACKET_ACK_FNC ackFnc, void* ackParam);


  Summary:
    Dynamically allocates a packet for transmitting IP protocol data.

  Description:
    This function dynamically allocates a packet for transmitting IP protocol data and 
	sets the packet IPv6 protocol for a TX packet.

  Precondition:
    TCPIP_IPV6_Initialize is required.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    netH        - Interface of the outgoing packet.
    ackFnc      - function to be called when IP is done with the TX packet
                  (finished transmitting)
    ackParam    - parameter to be used for this callback
                  This has meaning only for the caller of the
                  TCPIP_IPV6_TxPacketAllocate

  Returns:
    - On Success - Pointer to the allocated packet
	- On Failure - NULL

  Remarks:
    None.
 */
IPV6_PACKET * TCPIP_IPV6_TxPacketAllocate(TCPIP_NET_HANDLE netH, IPV6_PACKET_ACK_FNC ackFnc, void* ackParam);


//*****************************************************************************
/*
  Function:
    unsigned short TCPIP_IPV6_TxIsPutReady (IPV6_PACKET * ptrPacket,
        unsigned short count)

  Summary:
    Determines whether a TX packet can be written to.

  Description:
    This function determines whether a TX packet can be written to.  This function 
	will allocate additional space to the packet to accommodate the user.

  Precondition:
    TCPIP_IPV6_Initialize is required.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    ptrPacket - The packet to check
    count - The amount of writable space to check for

  Returns:
    - On Success - The amount of space available
	- On Failure - 0
  
  Remarks:
    None.
 */
unsigned short TCPIP_IPV6_TxIsPutReady(IPV6_PACKET * pkt, unsigned short count);


//*****************************************************************************
/*
  Function:
    void TCPIP_IPV6_PacketFree (IPV6_PACKET * ptrPacket)

  Summary:
    Frees a TCP/IP Packet structure from dynamic memory.

  Description:
    This function frees a TCP/IP Packet structure from dynamic memory.

  Precondition:
    TCPIP_IPV6_Initialize is required.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    ptrPacket - The packet to free.

  Returns:
    None.

  Remarks:
    None.
 */
void TCPIP_IPV6_PacketFree(IPV6_PACKET * pkt);


//*****************************************************************************
/*
  Function:
    bool TCPIP_IPV6_Put (IPV6_PACKET * pkt, unsigned char v)

  Summary:
    Writes a character of data to a packet.

  Description:
    This function writes a character of data to a packet.

  Precondition:
    TCPIP_IPV6_Initialize is required.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    pkt - The packet
    v - The character

  Returns:
    - true  - if the character was written
	- false - if the character was not written

  Remarks:
    None.
 */
bool TCPIP_IPV6_Put(IPV6_PACKET * pkt, unsigned char v);


//*****************************************************************************
/*
  Function:
    unsigned short TCPIP_IPV6_ArrayPutHelper (IPV6_PACKET * ptrPacket, 
	         const void * dataSource, uint8_t dataType, unsigned short len)

  Summary:
    Helper function to write data to a packet.

  Description:
    This is a helper function for writing data to a packet.

  Precondition:
    TCPIP_IPV6_Initialize is required.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    ptrPacket - The packet
    dataSource - The address of the data on its medium
    dataType - Descriptor of the data type (dynamic memory on PIC, in a
                    network FIFO, in static PIC RAM)
    len - Length of the data

  Returns:
    unsigned short - The number of bytes of data written.

  Remarks:
    None.
  */
unsigned short TCPIP_IPV6_ArrayPutHelper(IPV6_PACKET * pkt, const void * dataSource, uint8_t dataType, unsigned short len);


//******************************************************************************
/*
  Function:
        unsigned short TCPIP_IPV6_PutArray (IPV6_PACKET * ptrPacket, 
		                          const void * dataSource, unsigned short len)

  Summary:
    Writes data to a packet

  Description:
    This function writes data to an outgoing packet.

  Precondition:
	TCPIP_IPV6_Initialize is required.
	TCPIP_IPV6_InterfaceIsReady should be true.
    The TCPIP_IPV6_TxIsPutReady function must have returned a value greater
    than or equal to 'len'.

  Parameters:
    ptrPacket - The packet
    dataSource - Pointer to the data to copy to the packet
    len - Length of the data

  Returns:
    unsigned short - The number of bytes of data written.

  Remarks:
    None.
 */
#define TCPIP_IPV6_PutArray(pkt,data,len)  TCPIP_IPV6_ArrayPutHelper(pkt, data, IPV6_DATA_PIC_RAM, len)


//*****************************************************************************
/*
  Function:
    unsigned short TCPIP_IPV6_PayloadSet (IPV6_PACKET * ptrPacket, uint8_t* payload, 
	                                      unsigned short len)

  Summary:
    Allocates a segment on the end of a packet segment chain and uses it to
    address prebuffered data.

  Description:
    This function will allocate a data segment header and append it to the
    end of a chain of segments in a TX packet.  It will set the data pointer in
    the packet segment to a pre-existing buffer of data.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    ptrPacket - The packet
    payload - Address of the data payload
    len - Length of the data payload

  Returns:
    - On Success - The amount of data added to the packet length
	- On Failure - 0

  Remarks:
    This function is useful for adding payloads to outgoing packets without
    copying them if the data is in another pre-existing buffer (i.e., TCP).
*/
unsigned short TCPIP_IPV6_PayloadSet(IPV6_PACKET * pkt, uint8_t* payload, unsigned short len);


//*****************************************************************************
/*
  Function:
    int TCPIP_IPV6_Flush (IPV6_PACKET * ptrPacket)

  Summary:
    Flushes a IP TX packet.

  Description:
    This function flushes a IP TX packet. Determines the link-layer address, 
	if necessary and calculates the upper-layer checksum, if necessary.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    ptrPacket - The packet to flush

  Returns:
    - 1   - if the packet has been transmitted
    - 0   - if the packet has been queued
    - <0 - if the packet has been discarded for some error

  Remarks:
    None.
*/
int TCPIP_IPV6_Flush(IPV6_PACKET * pkt);


//******************************************************************************
/*
  Function:
    uint8_t TCPIP_IPV6_Get (TCPIP_MAC_PACKET* pRxPkt, uint8_t* pData)

  Summary:
    Reads the next byte of data from the specified MAC.

  Description:
    This function is used to read the next byte of data from a packet.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    pRxPkt - The MAC RX packet to read data from
	pData -  Byte data to be read

  Returns:
    - 1 - On Successful read
	- 0 - No byte is available to read


  Remarks:
    None.
 */
uint8_t TCPIP_IPV6_Get (TCPIP_MAC_PACKET* pRxPkt, uint8_t* pData);


//******************************************************************************
/*
  Function:
    uint8_t TCPIP_IPV6_ArrayGet (TCPIP_MAC_PACKET* pRxPkt, uint8_t *val,
        uint16_t len);

  Summary:
    Reads the next byte of data from the specified MAC.

  Description:
    Reads a character of data from a packet.

  Precondition:
    TCPIP_IPV6_Initialize is required.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    pRxPkt - The MAC RX packet to read data from
    val - The buffer to store the data
    len - The amount of data to read

  Returns:
    - > 0 - The number of bytes read
	- 0 - No byte is available to read

  Remarks:
    None
 */
uint8_t TCPIP_IPV6_ArrayGet (TCPIP_MAC_PACKET* pRxPkt, uint8_t *val, uint16_t len);


//*****************************************************************************
/*
  Function:
    void  TCPIP_IPV6_DestAddressSet(IPV6_PACKET * p, const IPV6_ADDR * addr)

  Summary:
    Sets the destination address for a IPv6 packet.

  Description:
    This API is used to configure the destination address for a IPv6 packet.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    p - pointer to IPv6 packet
    addr - destination address

  Returns:
    None.

  Remarks:
    None.
 */
void  TCPIP_IPV6_DestAddressSet(IPV6_PACKET * p, const IPV6_ADDR * addr);


//*****************************************************************************
/*
  Function:
    IPV6_ADDR *  TCPIP_IPV6_DestAddressGet(IPV6_PACKET * p)

  Summary:
    Gets the destination address for a IPv6 packet.

  Description:
    This function is used to get the destination address for a IPv6 packet.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    p - pointer to IPv6 packet

  Returns:
    IPV6_ADDR * 
	- On Success - Get a valid IPv6 Destination address
	- On Failure - NULL
  Remarks:
    None.
*/
IPV6_ADDR *  TCPIP_IPV6_DestAddressGet(IPV6_PACKET * p);


//*****************************************************************************
/*
  Function:
    void  TCPIP_IPV6_SourceAddressSet(IPV6_PACKET * p, const IPV6_ADDR * addr)

  Summary:
    Sets the source address for a IPv6 packet.

  Description:
    This function is used to configure the source address for a IPv6 packet.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    p - pointer to IPv6 packet
    addr - source address

  Returns:
    None.

  Remarks:
    None.
 */
void  TCPIP_IPV6_SourceAddressSet(IPV6_PACKET * p, const IPV6_ADDR * addr);


//*****************************************************************************
/*
  Function:
    IPV6_ADDR *  TCPIP_IPV6_SourceAddressGet(IPV6_PACKET * p)

  Summary:
    Gets the source address for an IPv6 packet.

  Description:
    This API is used to get the source address for an IPv6 packet.

  Precondition:
	TCPIP_IPV6_Initialize should be called.
	TCPIP_IPV6_InterfaceIsReady should be true.
	
  Parameters:
    p - pointer to IPv6 packet

  Returns:
	IPV6_ADDR *
    - On Success - Get a valid IPv6 Source address
	- On Failure - NULL

  Remarks:
    None.
*/
IPV6_ADDR *  TCPIP_IPV6_SourceAddressGet(IPV6_PACKET * p);


//*****************************************************************************
/*
  Function:
    IPV6_ADDR_STRUCT * TCPIP_IPV6_UnicastAddressAdd (TCPIP_NET_HANDLE netH, 
				IPV6_ADDR * address, int prefixLen, uint8_t skipProcessing)

  Summary:
    Adds a unicast address to a specified interface

  Description:
    Adds a unicast address to a specified interface.  Starts duplicate address
    detection if necessary.

  Precondition:
    TCPIP_IPV6_Initialize is required.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    netH            - The interface to which the the address is to be addded
    address         - The address to add
    prefixLen       - The prefix length associated to this static address
                      (providing the subnet prefix length). If 0, the default value 
					  of 64 will be used
    skipProcessing  - true to skip Duplicate address detection; otherwise, false

  Returns:
    IPV6_ADDR_STRUCT * 
	- On Success - Pointer to the structure of the newly allocated address
	- On Failure - NULL

  Remarks:
    The RFC (4291) requires the interface ID for all unicast addresses,
    (except those that start with the binary value 000)
    to be 64 bits long and to be constructed in Modified EUI-64 format.
    Therefore the prefixLen parameter should probably always be 64.

*/
IPV6_ADDR_STRUCT * TCPIP_IPV6_UnicastAddressAdd(TCPIP_NET_HANDLE netH, 
            IPV6_ADDR * address, int prefixLen, uint8_t skipProcessing);


//*****************************************************************************
/*
  Function:
    void TCPIP_IPV6_AddressUnicastRemove(TCPIP_NET_HANDLE netH, IPV6_ADDR * address)

  Summary:
    Removed a configured unicast address from an interface.

  Description:
    This function is used to remove a configured unicast address from an interface.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    netH    - The interface to remove the address
    address - The address

  Returns:
    None.

  Remarks:
    None.
*/
void TCPIP_IPV6_AddressUnicastRemove(TCPIP_NET_HANDLE netH, IPV6_ADDR * address);


//*****************************************************************************
/*
  Function:
    IPV6_ADDR_STRUCT * TCPIP_IPV6_MulticastListenerAdd(TCPIP_NET_HANDLE hNet, IPV6_ADDR * address)

  Summary:
    Adds a multicast listener to an interface.

  Description:
    This function is used to add the IPv6 multicast address to an interface.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    hNet - The interface to add the address
    address - The new listener

  Returns:
    IPV6_ADDR_STRUCT * 
	- On Success - Pointer to the new listener
	- On Failure - NULL

  Remarks:
    None.
*/
IPV6_ADDR_STRUCT * TCPIP_IPV6_MulticastListenerAdd(TCPIP_NET_HANDLE hNet, IPV6_ADDR * address);


//*****************************************************************************
/*
  Function:
    void TCPIP_IPV6_MulticastListenerRemove (TCPIP_NET_HANDLE netH, IPV6_ADDR * address)

  Summary:
    Removes a multicast listener from a given interface.

  Description:
    This function removes a multicast listener from a given interface.

  Precondition:
    TCPIP_IPV6_Initialize should be called.
	TCPIP_IPV6_InterfaceIsReady should be true.

  Parameters:
    netH  - The interface
    address - The address

  Returns:
    None.

  Remarks:
    None.
*/
void TCPIP_IPV6_MulticastListenerRemove(TCPIP_NET_HANDLE netH, IPV6_ADDR * address);


//****************************************************************************
/*
  Function:
    IPV6_ULA_RESULT TCPIP_IPV6_UniqueLocalUnicastAddressAdd (TCPIP_NET_HANDLE netH, 
	      uint16_t subnetID, IPV6_ULA_FLAGS genFlags, IP_MULTI_ADDRESS* ntpAddress)
    
  Summary:
    Adds a Unique Local Unicast Address (ULA) to a specified interface.
	
  Description:
    This function starts the process of adding an ULA address to the
    specified interface. The segments of the generated address are as
    follows:
    - FC00::/7     - ULA prefix
    - L            - 1 bit set to 1, locally assigned
    - Global ID    - 40 bit random generated identifier
    - subnet ID    - 16 bit subnet identifier
    - Interface ID - 64 bit interface identifier generated as a EUI64 from the 
                     specified interface MAC
    
    The randomness of the "Global ID" prefix of the generated IPv6 address
    is obtained by using an NTP server. The supplied NTP server will be
    contacted to obtain an NTP time stamp. This time stamp together with the
    EUI64 identifier obtained from the interface MAC are passed through a
    160 bits hash algorithm (SHA1) and the least significant 40 bits are
    used as the GlobalID of the interface.
	
  Preconditions:
    The IPv6 stack is initialized and the interface is up and configured.
	
  Input:
    netH -        The interface to add the address to.
    subnetID -    The subnet ID to be used.
    genFlags -    Address generation flags:
                  * IPV6_ULA_FLAG_NTPV4 - if set, the ntpAddress is an IPv4 
				    address and the NTP server will be contacted over an IPv4 
					connection. Otherwise, a default IPv6 connection will be 
					attempted
                  * IPV6_ULA_FLAG_GENERATE_ONLY - if set, the address will not 
				    be added to the list of the addresses for the specified interface
                  * IPV6_ULA_FLAG_SKIP_DAD - if set, the DAD processing will be skipped
    ntpAddress -  The NTP server address - it is an IPv4/IPv6 address as
                  selected by the IPV6_ULA_FLAG_NTPV4 flag (the IP address
                  could be obtained with an DNS call into the stack)
  Return:
	- IPV6_ULA_RES_BUSY - address generation module is busy
	- IPV6_ULA_RES_IF_ERR - IPv6 interface is not up
    - IPV6_ULA_RES_OK - if the call succeeded and the generation process was
						started
  Remarks:
    Only one address generation at a time is supported for now. Before
    attempting a new address generation the previous operation has to be
    completed, otherwise the call will fail.
    
    This function requires that the NTP client is enabled in the stack. If
    not, the call will fail.
    
    The caller will be notified by the outcome of the operation by the
    stack using the standard IPv6 notification handler (registered by
    TCPIP_IPV6_HandlerRegister call).                                                                                                                        
*/
IPV6_ULA_RESULT    TCPIP_IPV6_UniqueLocalUnicastAddressAdd (TCPIP_NET_HANDLE netH, 
         uint16_t subnetID, IPV6_ULA_FLAGS genFlags, IP_MULTI_ADDRESS* ntpAddress);


//*****************************************************************************
/*
  Function:
    bool    TCPIP_IPV6_RouterAddressAdd(TCPIP_NET_HANDLE netH, IPV6_ADDR * rAddress, 
	                                         unsigned long validTime, int flags);

  Summary:
    Adds a new router address to a specified interface.

  Description:
    This function adds a unicast address as a routing address to a specified interface.

  Precondition:
    rAddress pointer to a valid IPv6 router address
    IPv6 stack initialized, interface up and configured

  Parameters:
    netH            - The interface to which the address is to be added
    rAddress        - The router address to add
    validTime       - The time this valid will be valid, seconds.
                      If 0, the entry will be valid forever.
    flags           - Creation flags (not used for now, should be 0)

  Returns:
    - true  - operation succeeded
    - false - operation failed (interface not valid, interface still configuring, no memory, etc.)

  Remarks:
    The validTime parameter is relevant for an existent router on the network.
    If such router does not exist the stack will eventually discard the entry automatically.

*/
bool    TCPIP_IPV6_RouterAddressAdd(TCPIP_NET_HANDLE netH, IPV6_ADDR * rAddress, 
                                    unsigned long validTime, int flags);

//*****************************************************************************
/*
  Function:
    const IPV6_ADDR*    TCPIP_IPV6_DefaultRouterGet(TCPIP_NET_HANDLE hNet);

  Summary:
    Returns the current router address for a specified interface.

  Description:
    This function returns the current router address for a specified interface.

  Precondition:
    The IPv6 stack is initialized and the interface is up and configured.

  Parameters:
    netH            - The interface for which to return the router address.

  Returns:
    - valid IPV6_ADDR pointer  - if the interface exists and a valid router address exists
    - 0 - if the interface does not exist and a valid router address does not exist

  Remarks:
    None.

*/
const IPV6_ADDR*    TCPIP_IPV6_DefaultRouterGet(TCPIP_NET_HANDLE netH);

//*****************************************************************************
/*
  Function:
    void    TCPIP_IPV6_DefaultRouterDelete(TCPIP_NET_HANDLE netH);

  Summary:
    Deletes the current router list for a specified interface

  Description:
    This function deletes all default routers on a given interface

  Precondition:
    The IPv6 stack is initialized and the interface is up and configured.

  Parameters:
    netH            - The interface for which to delete the router address.

  Returns:
    None.

  Remarks:
    None.

*/
void    TCPIP_IPV6_DefaultRouterDelete(TCPIP_NET_HANDLE netH);

//*****************************************************************************
/*
  Function:
    IPV6_HANDLE TCPIP_IPV6_HandlerRegister(TCPIP_NET_HANDLE hNet, 
	                          IPV6_EVENT_HANDLER handler, const void* hParam)

  Summary:
    Registers an IPv6 event handler callback function.

  Description:
    This function is used to register a notification handler with the IPv6 module.

  Precondition:
    The IPv6 stack is initialized and the interface is up and configured.

  Parameters:
    netH  - Specifies interface to register on.
    handler - Handler to be called for event.
    hParam - The hParam is passed by the client and will be used by the 
			IPv6 when the notification is made. It is used for per-thread 
			content or if more modules, for example, share the same handler
			and need a way to differentiate the callback.

  Returns:
    Handle to registered callback.
	- On Success - Returns a valid handle
    - On Failure - null handle
	
  Remarks:
    None.
 */
IPV6_HANDLE TCPIP_IPV6_HandlerRegister(TCPIP_NET_HANDLE hNet, 
                              IPV6_EVENT_HANDLER handler, const void* hParam);


//*****************************************************************************
/*
  Function:
    bool TCPIP_IPV6_HandlerDeregister(IPV6_HANDLE hIpv6)

  Summary:
    Deregisters an IPv6 event handler callback function.

  Description:
    This function deregisters a previously registered IPv6 event handler callback function.

  Precondition:
    The IPv6 stack is initialized and the interface is up and configured.

  Parameters:
    hIpv6 - Handle to registered callback

  Returns:
    - true  - if deregister successful
    - false - if deregister is not successful

  Remarks:
    None.
  */
bool TCPIP_IPV6_HandlerDeregister(IPV6_HANDLE hIpv6);

// *****************************************************************************
/*
  Function:
    int TCPIP_IPV6_MaxDatagramDataSizeGet(TCPIP_NET_HANDLE netH);

  Summary:
    Helper to get the maximum datagream data size (MDDS) on a specific interface.
   
  Description:
    The function is a helper to allow retrieving the MDDS on the specified interface.
   
  Precondition:
    IPv6 properly initialized
    Interface properly initialized
        

  Parameters:
    netH      - handle of the interface

  Returns:
    - the maximum datagrem data size that can be transported on the selected interface
    - 0 - invalid interface specified
      
  Remarks:
    None

 */
int TCPIP_IPV6_MaxDatagramDataSizeGet(TCPIP_NET_HANDLE netH);

// *****************************************************************************
/*
  Function:
    bool TCPIP_IPV6_IsFragmentationEnabled(void);

  Summary:
    Helper to get the IPv6 fragmentation support.
   
  Description:
    The function is a helper to return the IPv6 fragmentation support.
   
  Precondition:
    IPv6 properly initialized

  Parameters:
    None

  Returns:
    - true  - if the fragmentation is enabled and supported by the IPv6 module 
    - false - fragmentation is not supported
      
  Remarks:
    The IPv6 fragmentation support is always enabled.

 */
static __inline__ bool __attribute__((always_inline)) TCPIP_IPV6_IsFragmentationEnabled(void)
{
    return true;
}


// *****************************************************************************
/*
  Function:
    void  TCPIP_IPV6_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs IPv6 module tasks in the TCP/IP stack.

  Precondition:
    The IPv6 stack is initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_IPV6_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // __IPV6_H_
