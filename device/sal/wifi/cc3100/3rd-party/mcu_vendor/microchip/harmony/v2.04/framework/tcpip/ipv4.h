/**********************************************************************
IPv4 Header File

  Company:
    Microchip Technology Inc.
	
  File Name:
    ipv4.h
	
  Summary:
    IPv4 definitions for the Microchip TCP/IP Stack.
	
  Description:
    IP is the workhorse protocol of the TCP/IP protocol suite. All TCP,
    UDP, and ICMP data gets transmitted as IP datagrams. IP provides an
    unreliable, connectionless datagram delivery service.              
  **********************************************************************/

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

#ifndef __IPV4_H_
#define __IPV4_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  


// *****************************************************************************
// *****************************************************************************
// Section: Public Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************

typedef struct
{
}TCPIP_IPV4_MODULE_CONFIG;

// *****************************************************************************
/* IPv4 supported protocols

  Summary:
    List of supported protocols.

  Description:
    This is the list of the protocols that are supported by this IPv4 implementation.

  Remarks:
    None.
 */
typedef enum
{
    IP_PROT_ICMP = (1u),
    IP_PROT_IGMP = (2u),
    IP_PROT_TCP = (6u),
    IP_PROT_UDP = (17u),
} IPV4_HEADER_TYPE;


// *****************************************************************************
/* IPv4 packet header definition

  Summary:
    Structure of an IPv4 header.

  Description:
    This is the structure of an IPv4 packet header.

  Remarks:
    None.
 */

typedef union
{
    uint16_t        val;
    struct
    {
        uint16_t    fragOffset:     13;     // fragment offset
        uint16_t    MF:             1;      // more fragments
        uint16_t    DF:             1;      // don't fragment
        uint16_t    reserved:       1;
    };
}IPV4_FRAGMENT_INFO;

typedef struct
{
    struct
    {   // option type
        uint8_t     optNumber:  5;      // specifies an option
        uint8_t     optClass:   2;      // option class: 0 - control; 2 - debugging and measurement; 1, 3 - reserved
        uint8_t     optCopied:  1;      // option copied to all fragments
    };
    uint8_t         optLength;          // size of the entire option
    uint16_t        optData[0];         // option specific variable data
}IPV4_OPTION_FIELD;

typedef union
{
    uint8_t     val;
    struct
    {
        uint8_t     reserved:       2;      // not used
        uint8_t     reliability:    1;      // reliability settings: normal/high
        uint8_t     throughput:     1;      // throughput settings: normal/high
        uint8_t     delay:          1;      // delay settings: normal/low
        uint8_t     precedence:     3;      // precedence; one of TCPIP_IPV4_PRECEDENCE_ values
    };
}IPV4_TYPE_OF_SERVICE;

typedef struct
{
    struct
    {
        uint8_t IHL:        4;
        uint8_t Version:    4;
    };
    IPV4_TYPE_OF_SERVICE TypeOfService;
    uint16_t TotalLength;
    uint16_t Identification;
    IPV4_FRAGMENT_INFO FragmentInfo;
    uint8_t TimeToLive;
    uint8_t Protocol;
    uint16_t HeaderChecksum;
    IPV4_ADDR SourceAddress;
    IPV4_ADDR DestAddress;
    uint32_t  options[0];
} IPV4_HEADER;


// *****************************************************************************
/* IPv4 packet structure definition

  Summary:
    IPv4 packet structure.

  Description:
    This is the structure of an IPv4 packet for transmission over the network.

  Remarks:
    None.
 */
typedef struct
{
    TCPIP_MAC_PACKET    macPkt;         // standard MAC packet header
                                        // safe cast to TCPIP_MAC_PACKET

    // additional IPv4 packet data 
    IPV4_ADDR           srcAddress;     // packet source
    IPV4_ADDR           destAddress;    // packet destination
    TCPIP_NET_HANDLE    netIfH;         // packet interface
    IPV4_ADDR           arpTarget;      // ARP resolution target
    uint8_t             optionLen;      // length of the options in the IPV4 packet; usually 0
    uint8_t             optionOffset;   // offset of the current option; when multiple options are supported
    uint16_t            optionMask;     // internal options to be embedded in the packet
} IPV4_PACKET;

// *****************************************************************************
/* IPv4 packet filters

  Summary:
    List of supported IPv4 packet filters.

  Description:
    This is the list of the packet filters that are supported by this IPv4 implementation.
    There are 3 types of IPv4 packets currently supported:
        - unicast
        - broadcast
        - multicast
    An IPV4 packet is accepted if the filter corresponding to the packet type is not set 

  Remarks:
    Multiple filters can be set
    
    If no filter is set, all packets are accepted this is the default case.
 */
typedef enum
{
    TCPIP_IPV4_FILTER_NONE      = 0x00,     // no packet filter active. All packets are accepted
    TCPIP_IPV4_FILTER_UNICAST   = 0x01,     // unicast packets will be filtered out
    TCPIP_IPV4_FILTER_BROADCAST = 0x02,     // broadcast packets will be filtered out
    TCPIP_IPV4_FILTER_MULTICAST = 0x04,     // multicast packets will be filtered out
} TCPIP_IPV4_FILTER_TYPE;


// *****************************************************************************
/* IPv4 options

  Summary:
    List of supported IPv4 options

  Description:
    This is the list of the options that are supported by this IPv4 implementation.

  Remarks:
    Currently only limited options are supported.
    
    Other options will be eventually added, including variable size ones.

 */

typedef enum
{
    TCPIP_IPV4_OPTION_NONE          = 0,     // no option; standard packet
    TCPIP_IPV4_OPTION_ROUTER_ALERT  = 20,    // router alert option for IGMP traffic
} TCPIP_IPV4_OPTION_TYPE;

// *****************************************************************************
/* Descriptor for the IPv4 options

  Summary:
    Structure describing the options for an IPv4 packet

  Description:
    This is a data structure describing options that are part of an IPv4 packet

  Remarks:
    None;
 */

typedef struct
{
    TCPIP_IPV4_OPTION_TYPE  optionType;     // current option type
    int                     optionSize;     // current option type in bytes, including the IPV4_OPTION_FIELD data;
}TCPIP_IPV4_OPTION_DCPT;

// *****************************************************************************
/* IPv4 Type Of Service Flags

  Summary:
    List describing the TOS flags/values for an IPv4 packet

  Description:
    This is a enumeration pf the possible values for the Type Of Service field for an IPv4 packet

  Remarks:
    The type is used to set the IPV4_TYPE_OF_SERVICE in an IPv4 packet.

    Multiple flags can be set to specify the precedence, delay, throughput and reliability.

    Default value is 0x00, i.e. normal precedence, delay, throughput and reliability.
 */

typedef enum __attribute__ ((__packed__))
{
    /* Default values for all fields */
    TCPIP_IPV4_TOS_DEFAULT                = 0x00,

    // precedence types
    TCPIP_IPV4_PRECEDENCE_ROUTINE         = 0x00,
    TCPIP_IPV4_PRECEDENCE_PRIORITY        = 0x01,
    TCPIP_IPV4_PRECEDENCE_IMMEDIATE       = 0x02,
    TCPIP_IPV4_PRECEDENCE_FLASH           = 0x03,
    TCPIP_IPV4_PRECEDENCE_FLASH_OVERRIDE  = 0x04,
    TCPIP_IPV4_PRECEDENCE_CRITIC          = 0x05,
    TCPIP_IPV4_PRECEDENCE_INET_CONTROL    = 0x06,
    TCPIP_IPV4_PRECEDENCE_NET_CONTROL     = 0x07,

    TCPIP_IPV4_PRECEDENCE_MASK            = 0x07,

    // delay flag
    TCPIP_IPV4_DELAY_NORMAL               = 0x00,
    TCPIP_IPV4_DELAY_LOW                  = 0x08,

    // throughput flag
    TCPIP_IPV4_THROUGHPUT_NORMAL          = 0x00,
    TCPIP_IPV4_THROUGHPUT_HIGH            = 0x10,

    // reliability flag
    TCPIP_IPV4_RELIABILITY_NORMAL         = 0x00,
    TCPIP_IPV4_RELIABILITY_HIGH           = 0x20,

}TCPIP_IPV4_TOS_FLAGS;

// *****************************************************************************
/* IPv4 packet parameters

  Summary:
    IPv4 packet formatting parameters

  Description:
    This is the structure describing parameters that can be specified for the formatting of an IPv4 packet for transmission.

  Remarks:
    None.
 */

typedef struct
{
    /* Type Of Service specification */
    TCPIP_IPV4_TOS_FLAGS tosFlags;
    /* Time to Live */
    uint8_t              ttl;
}TCPIP_IPV4_PACKET_PARAMS;

// *****************************************************************************
// *****************************************************************************
// Section: Public Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    TCPIP_IPV4_PacketFormatTx(IPV4_PACKET* pPkt, uint8_t protocol, uint16_t ipLoadLen, TCPIP_IPV4_PACKET_PARAMS* pParams);

  Summary:
    Formats an IPV4 packet and makes it ready for transmission.

  Description:
    The necessary fields are set into the IPv4 packet.

  Precondition:
    Properly allocated pPkt.
    The source and destination addresses should be updated in the packet.
    The network interface should be updated in the IPV4_PACKET

  Parameters:
    pPkt      - the packet to be formatted
    protocol  - the protocol associated with the packet
    ipLoadLen - the IPv4 packet payload length
    pParams   - pointer to a structure containing parameters for this packet
                If NULL, then default values are used.

  Returns:
    None
      
  Remarks:
    The segments should be properly updated with the right number of bytes (segLen).
    The IP payload length (ipLoadLen) is added only to the 1st segment of the packet!
    Other segments (for packets having multiple packets) are not touched.

 */
void    TCPIP_IPV4_PacketFormatTx(IPV4_PACKET* pPkt, uint8_t protocol, uint16_t ipLoadLen, TCPIP_IPV4_PACKET_PARAMS* pParams);


// *****************************************************************************
/*
  Function:
    bool TCPIP_IPV4_PacketTransmit(IPV4_PACKET* pPkt);

  Summary:
    Transmits an IPV4 packet over the network.

  Description:
    The IPv4 packet is sent to the MAC for transmission.

  Precondition:
    pPkt should have been properly formatted with TCPIP_IPV4_PacketFormatTx().
    The packet interface should be updated.
        

  Parameters:
    pPkt     - the packet to be transmitted

  Returns:
    - true  - if the packet was handed to the MAC or is queued for transmission
    - false - the packet cannot be transmitted (wrong interface, etc.) 
      
  Remarks:
    Only single packets can be transmitted.
    Chained packets are not supported for now.
 */
bool    TCPIP_IPV4_PacketTransmit(IPV4_PACKET* pPkt);


// *****************************************************************************
/*
  Function:
    TCPIP_NET_HANDLE TCPIP_IPV4_SelectSourceInterface(TCPIP_NET_HANDLE netH, 
	         IPV4_ADDR* pDestAddress, IPV4_ADDR* pSrcAddress, bool srcSet)

  Summary:
    Selects a source address and an interface based on the IPv4 destination address

  Description:
    Updates the pSrcAddress and returns the needed interface, if successful:
    * if srcSet == 1 and netH != 0, the function will not change anything 
    * if srcSet == 1 and netH == 0, the call will never fail it will use whatever 
	  value in pSrcAddress (even 0) and will try to come up with an appropriate interface
    * if srcSet == 0 and netH == 0, it will use the destination address
    * if srcSet == 0 and netH != 0, it will use the address of that interface

  Precondition:
    netH has to be valid (if non-0).

  Parameters:
    netH            - network interface handle
    pDestAddress    - pointer to destination address
    pSrcAddress     - pointer to source address
    srcSet          - boolean; true if address pointed by pSrcAddress is valid

  Returns:
    - A valid interface - if it succeeds and a valid source interface selected
    - 0 - interface selection failed
      
  Remarks:
    None.
 */
TCPIP_NET_HANDLE   TCPIP_IPV4_SelectSourceInterface(TCPIP_NET_HANDLE netH, 
                 IPV4_ADDR* pDestAddress, IPV4_ADDR* pSrcAddress, bool srcSet);

// *****************************************************************************
/*
  Function:
    const IPV4_ADDR*  TCPIP_IPV4_PacketGetDestAddress(TCPIP_MAC_PACKET* pPkt);

  Summary:
    Returns the IPv4 destination address associated with a TCPIP_MAC_PACKET

  Description:
    The function will return a pointer to where the IPv4 destination address
    is located in the TCPIP_MAC_PACKET.
    The TCPIP_MAC_PACKET is supposed to be a valid IPv4 packet that has
    destination address properly set.

  Precondition:
    pPkt - valid IPv4 packet, pNetLayer filed properly set.
        
  Parameters:
    pPkt - packet to query

  Returns:
    - A valid pointer to an IPV4_ADDR - if it succeeds
    - 0 - if call failed
      
  Remarks:
    This function is primarily meant for RX packets.
 */

static __inline__ const IPV4_ADDR* __attribute__((always_inline)) TCPIP_IPV4_PacketGetDestAddress(TCPIP_MAC_PACKET* pPkt)
{
    return &((IPV4_HEADER*)pPkt->pNetLayer)->DestAddress;
}

// *****************************************************************************
/*
  Function:
    const IPV4_ADDR*  TCPIP_IPV4_PacketGetSourceAddress(TCPIP_MAC_PACKET* pPkt);

  Summary:
    Returns the IPv4 source address associated with a TCPIP_MAC_PACKET

  Description:
    The function will return a pointer to where the IPv4 source address
    is located in the TCPIP_MAC_PACKET.
    The TCPIP_MAC_PACKET is supposed to be a valid IPv4 packet that has
    properly source address set.

  Precondition:
    pPkt - valid IPv4 packet, pNetLayer filed properly set
        

  Parameters:
    pPkt - packet to query


  Returns:
    - A valid pointer to an IPV4_ADDR - if it succeeds
    - 0 - if call failed
      
  Remarks:
    This function is primarily meant for RX packets.
 */

static __inline__ const IPV4_ADDR* __attribute__((always_inline)) TCPIP_IPV4_PacketGetSourceAddress(TCPIP_MAC_PACKET* pPkt)
{
    return &((IPV4_HEADER*)pPkt->pNetLayer)->SourceAddress;
}

// *****************************************************************************
/*
  Function:
    TCPIP_IPV4_FILTER_TYPE    TCPIP_IPV4_PacketFilterSet(TCPIP_IPV4_FILTER_TYPE filtType);

  Summary:
    Sets the IPV4 packet filters

  Description:
    The function will set the IPv4 packet filters.
    The filters that are present in the mask will be set.
    Other filters won't be touched.

  Precondition:
    filtType - valid IPv4 filter
    IPv4 properly initialized
        

  Parameters:
    filtType - packet filter mask to set


  Returns:
    - the current value of the IPV4 packet filters after this mask was applied.
      
  Remarks:
    None.
 */
TCPIP_IPV4_FILTER_TYPE    TCPIP_IPV4_PacketFilterSet(TCPIP_IPV4_FILTER_TYPE filtType);


// *****************************************************************************
/*
  Function:
    TCPIP_IPV4_FILTER_TYPE    TCPIP_IPV4_PacketFilterClear(TCPIP_IPV4_FILTER_TYPE filtType);

  Summary:
    Clears the IPV4 packet filters

  Description:
    The function will clear the IPv4 packet filters.
    The filters that are present in the mask will be cleared.
    Other filters won't be touched.

  Precondition:
    filtType - valid IPv4 filter
    IPv4 properly initialized
        

  Parameters:
    filtType - packet filter mask to clear


  Returns:
    - the current value of the IPV4 packet filters after this mask was applied.
      
  Remarks:
    None.
 */
TCPIP_IPV4_FILTER_TYPE    TCPIP_IPV4_PacketFilterClear(TCPIP_IPV4_FILTER_TYPE filtType);


// *****************************************************************************
/*
  Function:
    bool TCPIP_IPV4_MacPacketTransmit(TCPIP_MAC_PACKET* pPkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR* pDestAddress);

  Summary:
    Transmits a fully constructed MAC packet over the network.

  Description:
    The function is a helper to transmit a fully constructed MAC packet over an interface.
    The MAC packet is supposed to have all the fields properly constructed
    (including the TCPIP_MAC_ETHERNET_HEADER fields).
    The MAC packet does not need ARP resolution since the DestMACAddr is already known. 
    The packet acknowledge function should be properly set (it will be called by the MAC 
    driver once the packet is transmitted).

  Precondition:
    pPkt should have been properly formatted and constructed into a valid
    MAC packet that can go out over an interface
    pDestAddress - valid pointer
        

  Parameters:
    pPkt        - the packet to be transmitted
    hNet        - interface over which the packet is to be transmitted
    pDestAddress- the IPv4 packet destination address
                  It is used only for routing purposes, to decide if the
                  packet is routed internally or sent to the network interface

  Returns:
    - true  - if the packet was handed to the MAC or queued internally
    - false - the packet cannot be transmitted (wrong interface, etc.) 
      
  Remarks:
    Only single packets can be transmitted.
    Chained packets are not supported for now.

    The packet could be composed of multiple fragments.
    If the transmission fails, some fragments could have been transmitted
    while other failed.
    The flag TCPIP_MAC_PKT_FLAG_QUEUED could be used to determine which 
    fragments didn't make it through.

 */
bool    TCPIP_IPV4_MacPacketTransmit(TCPIP_MAC_PACKET* pPkt, TCPIP_NET_HANDLE hNet, IPV4_ADDR* pDestAddress);

// *****************************************************************************
/*
  Function:
    IPV4_PACKET* TCPIP_IPV4_PacketAlloc(uint16_t pktPayload, TCPIP_IPV4_OPTION_DCPT* pOptions, int nOptions,  TCPIP_MAC_PACKET_FLAGS flags);

  Summary:
    Allocates an IPv4 packet for transmission over the network.

  Description:
    The function is a helper to allocate an IPv4 packet that supports options.
    For fixed size options, the size of the options is determined by its type and
    the supplied size is ignored. 
   

  Precondition:
    IPv4 properly initialized
        

  Parameters:
    pktPayload  - the packet payload
    pOptions    - pointer to an array of option descriptors to be part of this packet
    nOptions    - number of supplied options
    flags       - standard MAC packet flags


  Returns:
    - a valid IPV4_PACKET - if the allocation succeeded
    - 0 - the packet cannot be allocated
      
  Remarks:
        If the option type contains a fixed option length for a supported type (TCPIP_IPV4_OPTION_ROUTER_ALERT, for ex.),
        then the size of the the fixed option is calculated internally amd the supplied option size is ignored.

        For not supported or variable size options the supplied optionLen bytes will be added to the 
        allocated packet.


 */
IPV4_PACKET* TCPIP_IPV4_PacketAlloc(uint16_t pktPayload, TCPIP_IPV4_OPTION_DCPT* pOptions, int nOptions,  TCPIP_MAC_PACKET_FLAGS flags);

// *****************************************************************************
/*
  Function:
    IPV4_OPTION_FIELD* TCPIP_IPV4_PacketOptionFieldSet(IPV4_PACKET* pPkt, uint16_t optionLen, bool firstOption);

  Summary:
    Helper to set an IPv4 packet option field.

  Description:
    The function is a helper to allow setting a specific option into an IPv4 packet that supports options.
    It is meant for supporting user controlled and variable size options.

   
  Precondition:
    IPv4 properly initialized
    pPkt properly created with TCPIP_IPV4_PacketAlloc
        

  Parameters:
    pPkt        - a packet allocated with TCPIP_IPV4_PacketAlloc
    optionLen   - size for the option to set, bytes
                  This is the size of the whole IPV4_OPTION_FIELD, the option data included
    firstOption - specifies that this is the first option to be set to the packet


  Returns:
    - a valid IPV4_OPTION_FIELD pointer that the caller can use to set the IPv4 option fields
    - 0 - invalid packet or the option field cannot hold the required option size
      
  Remarks:
    This function should be called for unsupported options or for options that have variable data.
    It doesn't have to be used for standard supported options (TCPIP_IPV4_OPTION_ROUTER_ALERT, for ex.)
    
    The packet should have been allocated with enough option space using TCPIP_IPV4_PacketAlloc.

    Sequential calls can be made using firstOption true for the first call and false for the 
    subsequent calls.

    When setting the option data, optionLen MUST NOT be exceeded!

    If the option doesn't take a multiple of 4 bytes, it should be padded with the NOP option (0x01).

 */
IPV4_OPTION_FIELD* TCPIP_IPV4_PacketOptionFieldSet(IPV4_PACKET* pPkt, uint16_t optionLen, bool firstOption);

// *****************************************************************************
/*
  Function:
    int TCPIP_IPV4_PacketOptionListGet(TCPIP_MAC_PACKET* pRxPkt, TCPIP_IPV4_OPTION_DCPT* pOptions, int nOptions );

  Summary:
    Helper to get the options that a IPv4 receive packet has.

  Description:
    The function is a helper to allow retrieving a list of options that are present in the received packet.

   
  Precondition:
    IPv4 properly initialized
    pRxPkt a proper IPv4 packet received by the IPv4 module
        

  Parameters:
    pRxPkt      - a packet received by the IPv4 module
    pOptions    - pointer to an array of option descriptors to receive the options present in the packet
    nOptions    - number of elements the array can store
                  Could be 0, in which case pOptions is not updated

  Returns:
    - the number of options found in the packet; 
    - 0 - invalid packet or packet without options
      
  Remarks:
    The packet should be a proper IPv4 packet received through the IPv4 module.
    Only minimal consistency check is done.

    On return, the pOptions is updated, up to nOptions;

 */
int TCPIP_IPV4_PacketOptionListGet(TCPIP_MAC_PACKET* pRxPkt, TCPIP_IPV4_OPTION_DCPT* pOptions, int nOptions );


// *****************************************************************************
/*
  Function:
    int TCPIP_IPV4_PacketOptionGet(TCPIP_MAC_PACKET* pRxPkt, TCPIP_IPV4_OPTION_TYPE optType, IPV4_OPTION_FIELD* pOptField, int optSize);

  Summary:
    Helper to get the options that a IPv4 receive packet has.

  Description:
    The function is a helper to allow retrieving a specific option that is present in an IPv4 received packet.

   
  Precondition:
    IPv4 properly initialized
    pRxPkt a proper IPv4 packet received by the IPv4 module
        

  Parameters:
    pRxPkt      - a packet received by the IPv4 module
    optType     - type of options to be searched/retrieved
    pOptField   - address to store the option
                  Could be NULL if not needed.
    optSize     - size of the pOptField structure, including the optData part
                  If 0, the pOptField will not be updated


  Returns:
    - size of the option, if found in the IPv4 packet
    - 0 if such an option does not exit
      
  Remarks:
    The packet should be a proper IPv4 packet received through the IPv4 module.
    Only minimal consistency check is done.

 */
int TCPIP_IPV4_PacketOptionGet(TCPIP_MAC_PACKET* pRxPkt, TCPIP_IPV4_OPTION_TYPE optType, IPV4_OPTION_FIELD* pOptField, int optSize);

// *****************************************************************************
/*
  Function:
    int TCPIP_IPV4_MaxDatagramDataSizeGet(TCPIP_NET_HANDLE netH);

  Summary:
    Helper to get the maximum datagream data size (MDDS) on a specific interface.
   
  Description:
    The function is a helper to allow retrieving the MDDS on the specified interface.
   
  Precondition:
    IPv4 properly initialized
    Interface properly initialized
        

  Parameters:
    netH      - handle of the interface

  Returns:
    - the maximum datagrem data size that can be transported on the selected interface
    - 0 - invalid interface specified
      
  Remarks:
    None

 */
int TCPIP_IPV4_MaxDatagramDataSizeGet(TCPIP_NET_HANDLE netH);

// *****************************************************************************
/*
  Function:
    bool TCPIP_IPV4_IsFragmentationEnabled(void);

  Summary:
    Helper to get the IPv4 fragmentation support.
   
  Description:
    The function is a helper to return the IPv4 fragmentation support.
   
  Precondition:
    IPv4 properly initialized

  Parameters:
    None

  Returns:
    - true  - if the fragmentation is enabled and supported by the IPv4 module 
    - false - fragmentation is not supported
      
  Remarks:
    The IPv4 fragmentation support is a build time option.

 */
bool TCPIP_IPV4_IsFragmentationEnabled(void);


// *****************************************************************************
/*
  Function:
    void  TCPIP_IPV4_Task(void)

  Summary:
    Standard TCP/IP stack module task function.

  Description:
    This function performs IPv4 module tasks in the TCP/IP stack.

  Precondition:
    The IPv4 module should have been initialized.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
*/
void  TCPIP_IPV4_Task(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // __IPV4_H_ 
