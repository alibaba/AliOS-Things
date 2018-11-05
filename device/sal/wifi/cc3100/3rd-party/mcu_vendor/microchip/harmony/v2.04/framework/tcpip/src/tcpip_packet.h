/*******************************************************************************
  TCPIP network packet manager - private stack API

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_packet.h

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

#ifndef __TCPIP_PACKET_H_
#define __TCPIP_PACKET_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "tcpip/tcpip_mac.h"

// enables packet trace
// This is used to trace packet allocation, by module
//#define TCPIP_PACKET_ALLOCATION_TRACE_ENABLE

// global trace info
typedef struct
{
    int nEntries;           // all trace entries
    int nUsed;              // used ones
    int traceFails;         // failed to allocate/find a packet entry
    int traceAckErrors;     // packets acknowledged with an error code
    int traceAckOwnerFails; // packets acknowledged but no module owner found
}TCPIP_PKT_TRACE_INFO;

// packet trace
// the moduleId is the one from tcpip.h::TCPIP_STACK_MODULE
// only if TCPIP_PACKET_ALLOCATION_TRACE_ENABLE defined
typedef struct
{
    int         moduleId;           // info belonging to this module; <0 means slot free
    uint32_t    totAllocated;       // total number of packets allocated successfully by this module
    uint32_t    currAllocated;      // number of packets still allocated by this module
    uint32_t    currSize;           // number of bytes currently allocated
    uint32_t    totFailed;          // total number of packets that failed for this module
    uint32_t    nAcks;              // number of acknowledgments
}TCPIP_PKT_TRACE_ENTRY;

// number of trace slots
// each module that allocates packets should have its slot.
// currently: tcp, udp, icmp, arp, ipv6
#define TCPIP_PKT_TRACE_SIZE        8

// module and packet logging flags
// only if TCPIP_PACKET_LOG_ENABLE is enabled
//

// global attributes that can be applied to logging
// Note: TCPIP_PKT_LOG_TYPE_RX_ONLY and TCPIP_PKT_LOG_TYPE_TX_ONLY are mutually exclusive and
// cannot be both set!
typedef enum
{
    TCPIP_PKT_LOG_TYPE_RX_TX        = 0x0000,             // log both RX and TX traffic - default
    TCPIP_PKT_LOG_TYPE_RX_ONLY      = 0x0001,             // log only RX traffic
    TCPIP_PKT_LOG_TYPE_TX_ONLY      = 0x0002,             // log only TX traffic
    TCPIP_PKT_LOG_TYPE_SKT_ONLY     = 0x0004,             // log only socket traffic

    //
    //
    TCPIP_PKT_LOG_TYPE_HANDLER_ALL  = 0x1000,             // forward to the external handler all logs,
                                                          // regardless of the discard mask

}TCPIP_PKT_LOG_TYPE;


// operations with log module masks
typedef enum
{
    TCPIP_PKT_LOG_MASK_OR,          // mask |= value; default operation
    TCPIP_PKT_LOG_MASK_AND,         // mask &= value
    TCPIP_PKT_LOG_MASK_NOR,         // mask |= ~value
    TCPIP_PKT_LOG_MASK_NAND,        // mask &= ~value
}TCPIP_PKT_LOG_MASK_OP;


typedef enum
{
    // flags that are stored as part of the entry
    TCPIP_PKT_LOG_FLAG_NONE         = 0x0000,   // no flag set
    TCPIP_PKT_LOG_FLAG_RX           = 0x0001,   // This is a RX packet
    TCPIP_PKT_LOG_FLAG_TX           = 0x0002,   // This is a TX packet
    TCPIP_PKT_LOG_FLAG_SKT_PARAM    = 0x0004,   // This is log containing socket info
    TCPIP_PKT_LOG_FLAG_PERSISTENT   = 0x0008,   // the log will be kept even after the packet is acknowledged
                                                // by default, once the packet is acknowledged, the packet will be discarded from the log
                                                // to make room for other packets

    // internal flags, not used in a log call: 0x1000 - 0x8000
    TCPIP_PKT_LOG_FLAG_DONE         = 0x1000,   // the log for this packet is completed, no other module will log it
                                                // used when packet is acknowledged
    TCPIP_PKT_LOG_FLAG_NEW          = 0x2000,   // a new log was created for this packet
    TCPIP_PKT_LOG_FLAG_REPLACE      = 0x4000,   // an existing log entry was used for this packet


    TCPIP_PKT_LOG_FLAG_INT_MASK     = 0xf000,   // internal flags mask
}TCPIP_PKT_LOG_FLAGS;

// structure describing the log of a packet
typedef struct
{
    TCPIP_MAC_PACKET*   pPkt;               // packet that's logged
    uint16_t            moduleLog;          // 32 bit mask of modules that processed the packet
                                            // each module that processes the packet sets its corresponding bit (1 << moduleId) in this mask
                                            // only the basic modules log their packets
                                            // TCPIP_MODULE_LAYER3 will show up as the module ID for a packet handed to a higher layer
    uint16_t            pktOwner;           // TCPIP_STACK_MODULE: module that owns the packet/allocated it, etc.
                                            // usually this is the 1st module that calls the log
    uint16_t            macId;              // TCPIP_STACK_MODULE: the ID of the MAC involved in the packet processing
                                            // These IDs are too big to be represented as a bit in the moduleLog
    uint16_t            netMask;            // corresponding network mask
    uint16_t            pktAcker;           // TCPIP_STACK_MODULE: module that acknowledges the packet.
    uint16_t            sktNo;              // for UDP and TCP sockets, the socket number
    uint16_t            lclPort;            // for TCP and UDP sockets, the local port number
    uint16_t            remPort;            // for TCP and UDP sockets, the remote port number

    int16_t             ackRes;             // acknowledge result; TCPIP_MAC_PKT_ACK_RES
    uint16_t            logFlags;           // log flags;     

}TCPIP_PKT_LOG_ENTRY;


// function that can receive notification when a new log is done for a packet
// the moduleId is the module making the log in the pLogEntry
// if pLogEntry == 0, then no entry could be found for that log (all used)
typedef void    (*TCPIP_PKT_LOG_HANDLER)(TCPIP_STACK_MODULE moduleId, const TCPIP_PKT_LOG_ENTRY* pLogEntry);


// global logging info
typedef struct
{
    int16_t                 nEntries;           // all log entries
    int16_t                 nUsed;              // used ones
    int16_t                 nPersistent;        // non deletable ones
    int16_t                 nFailed;            // failed to log because all the slots were taken...
    uint32_t                persistMask;        // current persistent mask
    uint32_t                discardMask;        // current discard mask
    uint32_t                netLogMask;         // current mask of networks the logging is enabled on
    uint32_t                sktDiscardMask;     // current mask of sockets to be discarded
    TCPIP_PKT_LOG_TYPE      logType;            // current log type
    TCPIP_PKT_LOG_HANDLER   logHandler;         // current log handler

}TCPIP_PKT_LOG_INFO;

// Extra TX/RX packet flags
// NOTE: // 16 bits only packet flags!

#define    TCPIP_MAC_PKT_FLAG_ARP           (TCPIP_MAC_PKT_FLAG_USER << 0)  // ARP packet data
                                                                            // set when the packet is ARP
                                                                            //
#define    TCPIP_MAC_PKT_FLAG_NET_TYPE      (TCPIP_MAC_PKT_FLAG_USER << 1)  // the network type: IPv4/IPv6 packet
#define    TCPIP_MAC_PKT_FLAG_IPV4          (0)                             // IPv4 packet data; cleared when the packet is IPV4
#define    TCPIP_MAC_PKT_FLAG_IPV6          (TCPIP_MAC_PKT_FLAG_USER << 1)  // IPv6 packet data; set when the packet is IPV6

#define    TCPIP_MAC_PKT_FLAG_LLDP          (TCPIP_MAC_PKT_FLAG_USER << 2)  // LLDP packet data; set when the packet is LLDP

#define    TCPIP_MAC_PKT_FLAG_ICMP_TYPE     (TCPIP_MAC_PKT_FLAG_USER << 3)  // ICMP packet type
#define    TCPIP_MAC_PKT_FLAG_ICMPV4        (0)                             // ICMPv4 packet data; cleared when the packet is ICMPv4
#define    TCPIP_MAC_PKT_FLAG_ICMPV6        (TCPIP_MAC_PKT_FLAG_USER << 3)  // ICMPv6 packet data; set when the packet is ICMPv6

#define    TCPIP_MAC_PKT_FLAG_NDP           (TCPIP_MAC_PKT_FLAG_USER << 4)  // NDP packet data; set when the packet is NDP

#define    TCPIP_MAC_PKT_FLAG_TRANSP_TYPE   (TCPIP_MAC_PKT_FLAG_USER << 5)  // UDP/TCP packet type
#define    TCPIP_MAC_PKT_FLAG_UDP           (0)                             // UDP packet data; set when the packet is UDP
#define    TCPIP_MAC_PKT_FLAG_TCP           (TCPIP_MAC_PKT_FLAG_USER << 5)  // TCP packet data; set when the packet is TCP

#define    TCPIP_MAC_PKT_FLAG_IGMP          (TCPIP_MAC_PKT_FLAG_USER << 6)  // IGMP packet


                                            // packet type extraction mask
#define    TCPIP_MAC_PKT_FLAG_TYPE_MASK     (TCPIP_MAC_PKT_FLAG_ARP | TCPIP_MAC_PKT_FLAG_NET_TYPE | TCPIP_MAC_PKT_FLAG_LLDP | TCPIP_MAC_PKT_FLAG_ICMP_TYPE | TCPIP_MAC_PKT_FLAG_NDP | TCPIP_MAC_PKT_FLAG_TRANSP_TYPE)

#define    TCPIP_MAC_PKT_FLAG_CONFIG        (TCPIP_MAC_PKT_FLAG_USER << 7)  // packet needs to be transmitted even when the stack
                                                                            // is not properly configured
                                                                            // probably a stack configuration packet


// initialization API

// sets the heap handle to be used for packet allocations
bool            TCPIP_PKT_Initialize(TCPIP_STACK_HEAP_HANDLE heapH, const TCPIP_NETWORK_CONFIG* pNetConf, int nNets);

void            TCPIP_PKT_Deinitialize(void);


// packet allocation API


// allocates a socket/transport IPv4/IPv6 packet
// The MAC, IPv4/IPv6 and transport headers (tHdrLen != 0) are all located in the 1st segment
// if payloadLen != 0 then the 1st segment will contain the/some payload too
// if needed, extra segments could be eventually added to the packet
// pktLen - size of the packet structure (at least TCPIP_MAC_PACKET will be allocated)
// tHdrLen - optional transport header length to be part of the 1st data segment
// payloadLen - optional transport payload to be part of the 1st data segment
// flags     - packet + 1st segment flags
TCPIP_MAC_PACKET*  TCPIP_PKT_SocketAlloc(uint16_t pktLen, uint16_t tHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags);


// allocates a TCPIP_MAC_PACKET packet (TCPIP_MAC_ETHERNET_HEADER always added);
// pktLen - the size of the packet (it will be 32 bits rounded up)
// segLoadLen - the payload size for the segment associated to this packet; Payload is always 32 bit aligned
//              if 0 no segment is created
// flags are attached to the 1st segment too 
TCPIP_MAC_PACKET* TCPIP_PKT_PacketAlloc(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags);


// forces freeing a previously allocated packet
// note that neither the packet nor segments marked
// with TCPIP_MAC_PKT_FLAG_STATIC are not freed
// Also note that this function does not free explicitly the segment payload.
// A payload that was created contiguously when the segment was created
// will be automatically freed by this function.
void            TCPIP_PKT_PacketFree(TCPIP_MAC_PACKET* pPkt);

static __inline__ void __attribute__((always_inline)) TCPIP_PKT_PacketAcknowledgeSet(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PACKET_ACK_FUNC ackFunc, const void* ackParam)
{
    pPkt->ackFunc = ackFunc;
    pPkt->ackParam = ackParam;
}

// acknowledges a packet
// clears the TCPIP_MAC_PKT_FLAG_QUEUED flag!
// a packet should always have an acknowledgment function
// packet's ackRes is updated only if the parameter ackRes != TCPIP_MAC_PKT_ACK_NONE.
void            TCPIP_PKT_PacketAcknowledge(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes);


//  simple segment allocation/manipulation

// allocates a segment with payload following immediately the segment header 
// this segment can be added to a packet using TCPIP_PKT_SegmentAppend
// loadLen specifies the segment allocated payload (could be 0)
// The segment payload is always allocated to be 32-bit aligned.
// The segment payload pointer will point loadOffset bytes after this address 
// 
TCPIP_MAC_DATA_SEGMENT* TCPIP_PKT_SegmentAlloc(uint16_t loadLen, uint16_t loadOffset, TCPIP_MAC_SEGMENT_FLAGS flags);

// adds a segment to the tail of segments of a packet
// segment should be fully constructed, with flags updated
void            TCPIP_PKT_SegmentAppend(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_DATA_SEGMENT* pSeg);           

// frees a created segment
void            TCPIP_PKT_SegmentFree(TCPIP_MAC_DATA_SEGMENT* pSeg);


// packet helpers
//

// sets the proper source, destination and type for a packet
// it also updated the packet length to include the MAC header size
// dstAddr can be 0 if not known
// packet should have been properly allocated and pMacLayer set
// returns false if the srcAddress is 0 though
// (intended for checking that a network interface is down)
bool             TCPIP_PKT_PacketMACFormat(TCPIP_MAC_PACKET* pPkt, const TCPIP_MAC_ADDR* dstAddr, const TCPIP_MAC_ADDR* srcAddr, uint16_t pktType);


// returns the segment to which dataAddress belongs
// the search occurs in every segment of the packet
// if srchTransport is set, the search starts with the transport data, i.e.
//      startPoint = pTransportLayer,
//      endPoint = startPoint + totTransportLen
// otherwise
//      startPoint = segLoad
//      endPoint = startPoint + segSize
// 0 if not in this packet
TCPIP_MAC_DATA_SEGMENT* TCPIP_PKT_DataSegmentGet(TCPIP_MAC_PACKET* pPkt, const uint8_t* dataAddress, bool srchTransport);


// simple helper to calculate the payload length of a packet
uint16_t        TCPIP_PKT_PayloadLen(TCPIP_MAC_PACKET* pPkt);


// debugging, tracing, logging
//

// returns the number of entries in the trace
int     TCPIP_PKT_TraceGetEntriesNo(TCPIP_PKT_TRACE_INFO* pTraceInfo);


// populates a trace entry with data for a index
// returns true if the entry is active - has valid data
bool    TCPIP_PKT_TraceGetEntry(int entryIx, TCPIP_PKT_TRACE_ENTRY* tEntry);


// logs a TX packet info
void    TCPIP_PKT_FlightLogTx(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId);
// logs a RX packet info;
void    TCPIP_PKT_FlightLogRx(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId);

// logs a TX packet info for a socket
// lclRemPort contains both ports: (lcl << 16 | remPort)
void    TCPIP_PKT_FlightLogTxSkt(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, uint32_t lclRemPort, uint16_t sktNo );

// logs a RX packet info for a socket; on RX the pkt IF is already known 
// lclRemPort contains both ports: (lcl << 16 | remPort)
void    TCPIP_PKT_FlightLogRxSkt(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, uint32_t lclRemPort, uint16_t sktNo );

// acknowledges a packet when a module is done with it
// this should be the last log call for this packet
void    TCPIP_PKT_FlightLogAcknowledge(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, TCPIP_MAC_PKT_ACK_RES ackRes);

// gets logger info
// returns true if there's log info available, false otherwise
bool     TCPIP_PKT_FlightLogGetInfo(TCPIP_PKT_LOG_INFO* pLogInfo);

// populates a log entry with data for a index
// returns true if the entry is active - has valid data
bool    TCPIP_PKT_FlightLogGetEntry(int entryIx, TCPIP_PKT_LOG_ENTRY* pLEntry);

// registers a log handler
// there is only one handler in the system, no protection
// call with 0 to deregister
// if logAll is true, then the log type and the discard mask are ignored
// and all packets are reported as they are reported
// returns true if success, false otherwise
bool    TCPIP_PKT_FlightLogRegister(TCPIP_PKT_LOG_HANDLER logHandler, bool logAll);


// applies/removes the persistence attribute/mask to all logs based on the module mask
// any log that has recorded modules according to this mask
// will have its persistent flag set/cleared
// The mask is constructed as the moduleLog: i.e. 1 << moduleId
// The persistence mask will be updated
// If clrNonPersist is true, the log entries that are made non persistent will be discarded
// The module mask should be limited only to the basic modules: <= TCPIP_MODULE_LAYER3
// Note: the call will search and apply the new attribute in all stored log entries
// so it can be expensive!
// Note: the persistent mask takes precedence over the discard one
//
void    TCPIP_PKT_FlightLogSetPersistMask(uint32_t moduleMask, TCPIP_PKT_LOG_MASK_OP persistOp, bool clrNonPersist);

// applies the discard attribute/mask to all logs based on the module mask
// any completed log that has recorded modules according to this mask
// will be discarded
// By default the completed logs are kept until replaced by a new log
// (unless the log is persistent)
// The mask is constructed as the moduleLog: i.e. 1 << moduleId
// The discard mask will be updated
// If clrPersist is true, the matching log entries will be discarded even if they are persistent
// Else only non persistent entries will be deleted.
// The module mask should be limited only to the basic modules: <= TCPIP_MODULE_LAYER3
// Note: the call will search and discard logs in all stored log entries
// so it can be expensive!
// Note: the persistent mask takes precedence over the discard one
void    TCPIP_PKT_FlightLogSetDiscardMask(uint32_t moduleMask, TCPIP_PKT_LOG_MASK_OP discardOp, bool clrPersist);

// applies the mask of logging interfaces to the logging service
// The mask is constructed as the 1 << netIx
// where the network index is the number of that specific interface
// The netLogMask mask will be updated
// If clrPersist is true, the matching log entries will be discarded even if they are persistent
// Else only non persistent entries will be deleted.
// When the stack starts the logger will be enabled 
// on a specific interface as directed by
// TCPIP_NETWORK_CONFIG_FLAGS::TCPIP_NETWORK_CONFIG_PKT_LOG_ON flag
void    TCPIP_PKT_FlightLogSetNetMask(uint32_t netMask, TCPIP_PKT_LOG_MASK_OP netOp, bool clrPersist);

// applies the mask of discarding sockets to the logging service
// The mask is constructed as the 1 << sktIx
// where the socket index is the number of that specific socket
// The sktDiscardMask mask will be updated
// If clrPersist is true, the matching log entries will be discarded even if they are persistent
// Else only non persistent entries will be deleted.
// When the stack starts the sktDiscardMask is cleared.
void    TCPIP_PKT_FlightLogSetSocketDiscardMask(uint32_t sktMask, TCPIP_PKT_LOG_MASK_OP netOp, bool clrPersist);

// applies the attributes to all logs
// - 
// - TCPIP_PKT_LOG_TYPE_RX_TX is global and is applied/removed to all logs:
//   both RX and TX packets will be logged - this is the default
//
// - TCPIP_PKT_LOG_TYPE_RX_ONLY is global and is applied/removed to all logs:
//   only RX packets will be logged
//   and the log will be cleared of TX packets
//
// - TCPIP_PKT_LOG_TYPE_TX_ONLY is global and is applied/removed to all logs:
//   only TX packets will be logged
//   and the log will be cleared of RX packets
//
// - TCPIP_PKT_LOG_TYPE_SKT_ONLY is global and is applied/removed to all logs:
//   only socket packets will be logged
//   and the log will be cleared of nonsocket packets
//
// Returns:
//      true if the call succeeded
//      false otherwise (wrong parameter)
//
// Notes:
//      - The RX_ONLY and TX_ONLY  are global.
//        They are applied before TCPIP_PKT_LOG_TYPE_SKT_ONLY;
//
//        These filters are applied before the PERSIST and DISCARD.
//
//      - Setting a type to RX or TX only will search and discard logs in all stored log entries
//        so it can be expensive!
//        If clrPersist is true the persistent entries will be deleted if they do not match the type!
//
//      - TCPIP_PKT_LOG_TYPE_RX_ONLY and TCPIP_PKT_LOG_TYPE_TX_ONLY are mutually exclusive and
//        cannot be both set!
//
bool    TCPIP_PKT_FlightLogTypeSet(TCPIP_PKT_LOG_TYPE logType, bool clrPersist);


// clears all log
// if clrPersist is true, it clears persistent entries too
void    TCPIP_PKT_FlightLogClear(bool clrPersist);

#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

// proto
// packet allocation
TCPIP_MAC_PACKET*   _TCPIP_PKT_SocketAllocDebug(uint16_t pktLen, uint16_t tHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags, int moduleId);
TCPIP_MAC_PACKET*   _TCPIP_PKT_PacketAllocDebug(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags, int moduleId);
void                _TCPIP_PKT_PacketFreeDebug(TCPIP_MAC_PACKET* pPkt, int moduleId);
TCPIP_MAC_DATA_SEGMENT* _TCPIP_PKT_SegmentAllocDebug(uint16_t loadLen, uint16_t loadOffset, TCPIP_MAC_SEGMENT_FLAGS flags, int moduleId);
void                _TCPIP_PKT_SegmentFreeDebug(TCPIP_MAC_DATA_SEGMENT* pSeg, int moduleId);

// packet acknowledgement
void                _TCPIP_PKT_PacketAcknowledgeDebug(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, int moduleId);

// direct calls
#define TCPIP_PKT_SocketAlloc(pktLen, tHdrLen, payloadLen, flags)   _TCPIP_PKT_SocketAllocDebug(pktLen, tHdrLen, payloadLen, flags, TCPIP_THIS_MODULE_ID)
#define TCPIP_PKT_PacketAlloc(pktLen, segLoadLen, flags)            _TCPIP_PKT_PacketAllocDebug(pktLen, segLoadLen, flags, TCPIP_THIS_MODULE_ID)
#define TCPIP_PKT_PacketFree(pPkt)                                  _TCPIP_PKT_PacketFreeDebug(pPkt, TCPIP_THIS_MODULE_ID)
#define TCPIP_PKT_SegmentAlloc(loadLen, loadOffset, flags)          _TCPIP_PKT_SegmentAllocDebug(loadLen, loadOffset, flags, TCPIP_THIS_MODULE_ID)
#define TCPIP_PKT_SegmentFree(pSeg)                                 _TCPIP_PKT_SegmentFreeDebug(TCPIP_MAC_DATA_SEGMENT* pSeg, int moduleId);


#define TCPIP_PKT_PacketAcknowledge(pPkt, ackRes)                   _TCPIP_PKT_PacketAcknowledgeDebug(pPkt, ackRes, TCPIP_THIS_MODULE_ID)

#define     _TCPIP_PKT_ALLOC_FNC        _TCPIP_PKT_PacketAllocDebug
#define     _TCPIP_PKT_FREE_FNC         _TCPIP_PKT_PacketFreeDebug
#define     _TCPIP_PKT_ACK_FNC          _TCPIP_PKT_PacketAcknowledgeDebug

#else

// proto
TCPIP_MAC_PACKET*   _TCPIP_PKT_SocketAlloc(uint16_t pktLen, uint16_t tHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags);
TCPIP_MAC_PACKET*   _TCPIP_PKT_PacketAlloc(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags);
void                _TCPIP_PKT_PacketFree(TCPIP_MAC_PACKET* pPkt);
TCPIP_MAC_DATA_SEGMENT* _TCPIP_PKT_SegmentAlloc(uint16_t loadLen, uint16_t loadOffset, TCPIP_MAC_SEGMENT_FLAGS flags);
void                _TCPIP_PKT_SegmentFree(TCPIP_MAC_DATA_SEGMENT* pSeg);

void                _TCPIP_PKT_PacketAcknowledge(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, TCPIP_STACK_MODULE moduleId);


// direct calls
#define TCPIP_PKT_SocketAlloc(pktLen, tHdrLen, payloadLen, flags)   _TCPIP_PKT_SocketAlloc(pktLen, tHdrLen, payloadLen, flags)
#define TCPIP_PKT_PacketAlloc(pktLen, segLoadLen, flags)            _TCPIP_PKT_PacketAlloc(pktLen, segLoadLen, flags)
#define TCPIP_PKT_PacketFree(pPkt)                                  _TCPIP_PKT_PacketFree(pPkt)
#define TCPIP_PKT_SegmentAlloc(loadLen, loadOffset, flags)          _TCPIP_PKT_SegmentAlloc(loadLen, loadOffset, flags)
#define TCPIP_PKT_SegmentFree(pSeg)                                 _TCPIP_PKT_SegmentFree(TCPIP_MAC_DATA_SEGMENT* pSeg);


#define TCPIP_PKT_PacketAcknowledge(pPkt, ackRes)                   _TCPIP_PKT_PacketAcknowledge(pPkt, ackRes, TCPIP_THIS_MODULE_ID)


#define     _TCPIP_PKT_ALLOC_FNC        _TCPIP_PKT_PacketAlloc
#define     _TCPIP_PKT_FREE_FNC         _TCPIP_PKT_PacketFree
#define     _TCPIP_PKT_ACK_FNC          _TCPIP_PKT_PacketAcknowledge



static __inline__ int __attribute__((always_inline)) TCPIP_PKT_TraceGetEntriesNoStatic(TCPIP_PKT_TRACE_INFO* pTraceInfo)
{
    return 0;
}
#define TCPIP_PKT_TraceGetEntriesNo(pTraceInfo) TCPIP_PKT_TraceGetEntriesNoStatic(pTraceInfo)


// populates a trace entry with data for a index
static __inline__ bool __attribute__((always_inline)) TCPIP_PKT_TraceGetEntryStatic(int entryIx, TCPIP_PKT_TRACE_ENTRY* tEntry)
{
    return false;
}
#define TCPIP_PKT_TraceGetEntry(entryIx, tEntry) TCPIP_PKT_TraceGetEntryStatic(entryIx, tEntry)

#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

#if !(TCPIP_PACKET_LOG_ENABLE)

#define TCPIP_PKT_FlightLogTx(pPkt, moduleId)

#define TCPIP_PKT_FlightLogRx(pPkt, moduleId)

#define TCPIP_PKT_FlightLogTxSkt(pPkt, moduleId, lclRemPort, sktNo )

#define TCPIP_PKT_FlightLogRxSkt(pPkt, moduleId, lclRemPort, sktNo )

#define TCPIP_PKT_FlightLogAcknowledge(pPkt, moduleId, ackRes)

#endif


#endif // __TCPIP_PACKET_H_


