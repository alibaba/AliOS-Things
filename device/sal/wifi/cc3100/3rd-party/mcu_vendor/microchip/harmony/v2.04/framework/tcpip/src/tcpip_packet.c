/*******************************************************************************
  TCPIP network packet manager implementation

  Summary:
    
  Description:
*******************************************************************************/

/*******************************************************************************
File Name: tcpip_packet.c 
Copyright © 2012 released Microchip Technology Inc.  All rights
reserved.

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
#if !defined(__PIC32C__)
    #include <sys/kmem.h>
#else
//kmem.h dummy definitions
/* translate between KSEG0 and KSEG1 virtual addresses */
#define KVA0_TO_KVA1(v)	(v)
#define KVA1_TO_KVA0(v)	(v)

/* Test for KSEGS */
#define IS_KVA(v)	(true)
#define IS_KVA0(v)	(true)
#endif

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MANAGER

#include "tcpip_private.h"
#include "tcpip_packet.h"


/*  TCPIP MAC Frame Offset

  Summary:
    An offset from a 4 byte aligned address where the MAC frame should start

  Description:
    Offset used on 32 bit machines that allows alignment of the network layer data.
    This allows improved efficiency for checksum calculations, etc.
  
  Remarks:
    Usual value is 2.

    See notes for the TCPIP_MAC_DATA_SEGMENT.segLoadOffset member.

    It makes sense to have this value as a #define (or global variable) instead of a parameter to the packet allocation function.
    In a system whith different MAC drivers the value may be different from MAC to MAC and then the stack would be forced
    to keep track of what packet could be forwarded on what interface.

*/

#if 0
#if defined(TCPIP_IF_PIC32WK)
    #define TCPIP_MAC_FRAME_OFFSET      (34+4)  // bytes to store the packet pointer
#else // including interfaces such as MRF24WN, WINC1500 and WILC1000
    #define TCPIP_MAC_FRAME_OFFSET      2
#endif
#endif
#define TCPIP_MAC_FRAME_OFFSET      10  // this should match FreeRTOSIP ipconfigBUFFER_PADDING!

static TCPIP_STACK_HEAP_HANDLE    pktMemH = 0;
static bool                 pktK0Heap = 0;

#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
static TCPIP_PKT_TRACE_ENTRY    _pktTraceTbl[TCPIP_PKT_TRACE_SIZE];


static TCPIP_PKT_TRACE_INFO _pktTraceInfo;  // global counters  


static TCPIP_PKT_TRACE_ENTRY*   _TCPIP_PKT_TraceFindEntry(int moduleId, bool addNewSlot);
static void                     _TCPIP_PKT_TraceAddToEntry(int moduleId, TCPIP_MAC_PACKET* pPkt);
static void                     _TCPIP_PKT_TraceFreeEntry(int moduleId, TCPIP_MAC_PACKET* pPkt);
static void                     _TCPIP_PKT_TraceAckEntry(int moduleId, TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes);
static uint32_t                 _TCPIP_PKT_TracePktSize(TCPIP_MAC_PACKET* pPkt);


static /*__inline__*/ void /*__attribute__((always_inline))*/ _TCPIP_PKT_TraceFail(void)
{
    _pktTraceInfo.traceFails++;
}

static TCPIP_MAC_PACKET* _TCPIP_PKT_PacketAllocInt(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags, int moduleId);
static void _TCPIP_PKT_PacketFreeInt(TCPIP_MAC_PACKET* pPkt, int moduleId);
static TCPIP_MAC_PACKET*  _TCPIP_PKT_SocketAllocInt(uint16_t pktLen, uint16_t transpHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags, int moduleId);
static TCPIP_MAC_DATA_SEGMENT* _TCPIP_PKT_SegmentAllocInt(uint16_t loadLen, uint16_t loadOffset, TCPIP_MAC_SEGMENT_FLAGS flags, int moduleId);
static void _TCPIP_PKT_SegmentFreeInt(TCPIP_MAC_DATA_SEGMENT* pSeg, int moduleId);
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

#if (TCPIP_PACKET_LOG_ENABLE)

// size of the packet log
// The log entries will be overwritten when no available free slot is found!

static TCPIP_PKT_LOG_ENTRY  _pktLogTbl[TCPIP_PKT_LOG_SIZE];

static TCPIP_PKT_LOG_INFO   _pktLogInfo;  // global log counters  

static int                  _pktOverwriteIx;    // simple LRU displacement pointer
#endif  // (TCPIP_PACKET_LOG_ENABLE)



// API

bool TCPIP_PKT_Initialize(TCPIP_STACK_HEAP_HANDLE heapH, const TCPIP_NETWORK_CONFIG* pNetConf, int nNets)
{
    pktMemH = 0;

    while(heapH != 0)
    {
        TCPIP_MAC_PACKET* allocPtr;

        allocPtr = (TCPIP_MAC_PACKET*)TCPIP_HEAP_Malloc(heapH, sizeof(TCPIP_MAC_PACKET));

        if(allocPtr == 0)
        {
            break;
        }

        TCPIP_HEAP_Free(heapH, allocPtr);
        if(!IS_KVA(allocPtr))
        {   // only kernel space buffers accepted
            break;
        }
        // success
        pktK0Heap = IS_KVA0(allocPtr);
        pktMemH = heapH;

#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
        memset(_pktTraceTbl, 0, sizeof(_pktTraceTbl));
        memset(&_pktTraceInfo, 0, sizeof(_pktTraceInfo));
        _pktTraceInfo.nEntries = sizeof(_pktTraceTbl)/sizeof(*_pktTraceTbl);
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)

#if (TCPIP_PACKET_LOG_ENABLE)
        memset(_pktLogTbl, 0, sizeof(_pktLogTbl));
        memset(&_pktLogInfo, 0, sizeof(_pktLogInfo));
        _pktLogInfo.nEntries = sizeof(_pktLogTbl) / sizeof(*_pktLogTbl);
        _pktOverwriteIx = 0;
        // construct the start netLogMask
        int ix;
        for(ix = 0; ix < nNets; ix++, pNetConf++)
        {
            if((pNetConf->startFlags & TCPIP_NETWORK_CONFIG_PKT_LOG_ON) != 0) 
            {
                _pktLogInfo.netLogMask |= 1 << ix;
            }
        }

#endif  // (TCPIP_PACKET_LOG_ENABLE)

        break;
    }


    return pktMemH != 0;
    
}

void TCPIP_PKT_Deinitialize(void)
{
    pktMemH = 0;
}


// acknowledges a packet
void _TCPIP_PKT_PacketAcknowledge(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, TCPIP_STACK_MODULE moduleId)
{
    if(ackRes != TCPIP_MAC_PKT_ACK_NONE)
    {
        pPkt->ackRes = ackRes;
    }

    if(pPkt->ackFunc)
    {
       TCPIP_PKT_FlightLogAcknowledge(pPkt, moduleId, ackRes);
       if((*pPkt->ackFunc)(pPkt, pPkt->ackParam))
       {
           pPkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
       }
    }
    else
    {
        SYS_ERROR(SYS_ERROR_WARNING, "Packet Ack: orphan packet! \r\n");
    }
}

void TCPIP_PKT_SegmentAppend(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_DATA_SEGMENT* pSeg)
{
    TCPIP_MAC_DATA_SEGMENT  *pN, *prev;

    if((pN = pPkt->pDSeg) == 0)
    {   // insert as root
        pPkt->pDSeg = pSeg;
    }
    else
    {   // traverse the list
        for(prev = 0; pN != 0; prev = pN, pN = pN->next);
        prev->next = pSeg;
    }

}

// helpers

bool TCPIP_PKT_PacketMACFormat(TCPIP_MAC_PACKET* pPkt, const TCPIP_MAC_ADDR* dstAddr, const TCPIP_MAC_ADDR* srcAddr, uint16_t pktType)
{
    if(srcAddr)
    {
        TCPIP_MAC_ETHERNET_HEADER* macHdr;
        TCPIP_MAC_ADDR    *destHdrAdd, *srcHdrAdd;

        macHdr = (TCPIP_MAC_ETHERNET_HEADER*)pPkt->pMacLayer;
        srcHdrAdd = &macHdr->SourceMACAddr;

        if(dstAddr)
        {
            destHdrAdd = &macHdr->DestMACAddr;
            memcpy(destHdrAdd, dstAddr, sizeof(*destHdrAdd));
        }

        memcpy(srcHdrAdd, srcAddr, sizeof(*srcHdrAdd));
        // set the MAC frame type
        macHdr->Type = TCPIP_Helper_htons(pktType);

        // update the frame length
        pPkt->pDSeg->segLen += sizeof(TCPIP_MAC_ETHERNET_HEADER);
        return true;
    }

    return false;
}

// returns the segment to which dataAddress belongs
// 0 if not in this packet
TCPIP_MAC_DATA_SEGMENT* TCPIP_PKT_DataSegmentGet(TCPIP_MAC_PACKET* pPkt, const uint8_t* dataAddress, bool srchTransport)
{
    TCPIP_MAC_DATA_SEGMENT  *pStartSeg, *pSeg;

    pStartSeg = 0;

    if(srchTransport)
    {   // search the segment containing the transport data
        for(pSeg = pPkt->pDSeg; pSeg != 0; pSeg = pSeg->next)
        {
            if(pSeg->segLoad <= pPkt->pTransportLayer && pPkt->pTransportLayer <= pSeg->segLoad + pSeg->segSize)
            {   // found segment containing the beg of the transport
                if(pPkt->pTransportLayer <= dataAddress && dataAddress <= pSeg->segLoad + pSeg->segSize)
                {
                    return pSeg;
                }

                pStartSeg = pSeg->next;
                break;
            }
        }
    }
    else
    {
        pStartSeg = pPkt->pDSeg;
    }


    for(pSeg = pStartSeg; pSeg != 0; pSeg = pSeg->next)
    {
        if(pSeg->segLoad <= dataAddress && dataAddress <= pSeg->segLoad + pSeg->segSize)
        {
            return pSeg;
        }
    }

    return 0;
}

uint16_t TCPIP_PKT_PayloadLen(TCPIP_MAC_PACKET* pPkt)
{
    uint32_t payloadSize = 0;

    if(pPkt)
    {
        TCPIP_MAC_DATA_SEGMENT* pSeg = pPkt->pDSeg;

        while(pSeg != 0)
        {
            payloadSize += pSeg->segLen;
            pSeg = pSeg->next;
        }
    }

    return payloadSize;
}

// repeated debug versions; they store the original moduleId
#if defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)
static __inline__ TCPIP_MAC_PACKET* __attribute__((always_inline)) _TCPIP_PKT_PacketAllocInt(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags, int moduleId)
{
    TCPIP_MAC_PACKET* pPkt;
    TCPIP_MAC_DATA_SEGMENT  *pSeg;
    uint16_t        pktUpLen, allocLen;

    if(pktLen < sizeof(TCPIP_MAC_PACKET))
    {
        pktLen = sizeof(TCPIP_MAC_PACKET);
    }

    pktUpLen = (((pktLen + 3) >> 2) << 2);     // 32 bits round up

    allocLen = pktUpLen + sizeof(*pSeg) + segLoadLen + sizeof(TCPIP_MAC_ETHERNET_HEADER) + TCPIP_MAC_FRAME_OFFSET;

    
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    pPkt = (TCPIP_MAC_PACKET*)TCPIP_HEAP_MallocDebug(pktMemH, allocLen, moduleId, __LINE__);
#else
    pPkt = (TCPIP_MAC_PACKET*)TCPIP_HEAP_Malloc(pktMemH, allocLen);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

    if(pPkt)
    {   
        // clear the TCPIP_MAC_PACKET and 1st segment fields
        // populate the 1st segment
        memset(pPkt, 0, pktUpLen + sizeof(*pSeg));
        pSeg = (TCPIP_MAC_DATA_SEGMENT*)((uint8_t*)pPkt + pktUpLen);

        pSeg->segSize = segLoadLen + sizeof(TCPIP_MAC_ETHERNET_HEADER);
        pSeg->segLoadOffset = TCPIP_MAC_FRAME_OFFSET;
        pSeg->segLoad = (uint8_t*)(pSeg + 1) + TCPIP_MAC_FRAME_OFFSET;
        pSeg->segFlags = TCPIP_MAC_SEG_FLAG_STATIC; // embedded in TCPIP_MAC_PACKET itself
        pPkt->pDSeg = pSeg;

        pPkt->pMacLayer = pSeg->segLoad;
        pPkt->pktFlags = flags & (~TCPIP_MAC_PKT_FLAG_STATIC);  // this packet is dynamically allocated
        if(segLoadLen)
        {
            pPkt->pNetLayer = pPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);
        }

        if(pktK0Heap)
        {
            pPkt = (TCPIP_MAC_PACKET*)KVA0_TO_KVA1(pPkt);
        }
    }

    return pPkt;
}

static __inline__ void __attribute__((always_inline)) _TCPIP_PKT_PacketFreeInt(TCPIP_MAC_PACKET* pPkt, int moduleId)
{
    if((pPkt->pktFlags & TCPIP_MAC_PKT_FLAG_STATIC) == 0)
    {   // we don't deallocate static packets
        TCPIP_MAC_DATA_SEGMENT  *pSeg, *pNSeg;

        for(pSeg = pPkt->pDSeg; pSeg != 0 ; )
        {
            pNSeg = pSeg->next;
            if((pSeg->segFlags & TCPIP_MAC_SEG_FLAG_STATIC) == 0)
            {
                if(pktK0Heap)
                {
                    pSeg = (TCPIP_MAC_DATA_SEGMENT*)KVA1_TO_KVA0(pSeg);
                }
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
                TCPIP_HEAP_FreeDebug(pktMemH, pSeg, moduleId);
#else
                TCPIP_HEAP_Free(pktMemH, pSeg);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
            }
            pSeg = pNSeg;
        }

        if(pktK0Heap)
        {
            pPkt = (TCPIP_MAC_PACKET*)KVA1_TO_KVA0(pPkt);
        }
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
        TCPIP_HEAP_FreeDebug(pktMemH, pPkt, moduleId);
#else
        TCPIP_HEAP_Free(pktMemH, pPkt);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    }
}

static __inline__ TCPIP_MAC_PACKET* __attribute__((always_inline)) _TCPIP_PKT_SocketAllocInt(uint16_t pktLen, uint16_t transpHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags, int moduleId)
{
    uint16_t    netHdrLen, totHdrLen;
    TCPIP_MAC_PACKET* pPkt;

    if((flags & TCPIP_MAC_PKT_FLAG_IPV6) != 0)
    {
        netHdrLen = sizeof(IPV6_HEADER);
    }
    else
    {
        netHdrLen = sizeof(IPV4_HEADER);
    }


    totHdrLen = netHdrLen + transpHdrLen;

    pPkt = _TCPIP_PKT_PacketAllocInt(pktLen, totHdrLen +  payloadLen, flags, moduleId);

    if(pPkt)
    {   // set the layer pointers in place
        if(transpHdrLen)
        {
            pPkt->pTransportLayer = pPkt->pNetLayer + netHdrLen;
        }
    }

    return pPkt;
}

static __inline__ TCPIP_MAC_DATA_SEGMENT* __attribute__((always_inline)) _TCPIP_PKT_SegmentAllocInt(uint16_t loadLen, uint16_t loadOffset, TCPIP_MAC_SEGMENT_FLAGS flags, int moduleId)
{
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    uint16_t allocSize;

    if(loadLen != 0)
    {
        allocSize = sizeof(*pSeg) + loadLen + loadOffset;
    }
    else
    {
        allocSize = sizeof(*pSeg);
    }

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    pSeg = (TCPIP_MAC_DATA_SEGMENT*)TCPIP_HEAP_MallocDebug(pktMemH, allocSize, moduleId, __LINE__);
#else
    pSeg = (TCPIP_MAC_DATA_SEGMENT*)TCPIP_HEAP_Malloc(pktMemH, allocSize);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 


    if(pSeg)
    {
        memset(pSeg, 0, sizeof(*pSeg));

        pSeg->segFlags = flags & (~TCPIP_MAC_SEG_FLAG_STATIC);
        if(loadLen != 0)
        {
            pSeg->segSize = loadLen;
            pSeg->segLoadOffset = loadOffset;
            pSeg->segLoad = (uint8_t*)(pSeg + 1) + loadOffset;
        }

        if(pktK0Heap)
        {
            pSeg = (TCPIP_MAC_DATA_SEGMENT*)KVA0_TO_KVA1(pSeg);
        }
        
    }

    return pSeg;
}

static __inline__ void __attribute__((always_inline)) _TCPIP_PKT_SegmentFreeInt(TCPIP_MAC_DATA_SEGMENT* pSeg, int moduleId)
{
    if( (pSeg->segFlags & TCPIP_MAC_SEG_FLAG_STATIC) == 0)
    {
        if(pktK0Heap)
        {
            pSeg = (TCPIP_MAC_DATA_SEGMENT*)KVA1_TO_KVA0(pSeg);
        }
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
        TCPIP_HEAP_FreeDebug(pktMemH, pSeg, moduleId);
#else
        TCPIP_HEAP_Free(pktMemH, pSeg);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    }

}

TCPIP_MAC_PACKET* _TCPIP_PKT_SocketAllocDebug(uint16_t pktLen, uint16_t tHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags, int moduleId)
{
    TCPIP_MAC_PACKET* pPkt = _TCPIP_PKT_SocketAllocInt(pktLen, tHdrLen, payloadLen, flags, moduleId);
    _TCPIP_PKT_TraceAddToEntry(moduleId, pPkt);
    return pPkt;

}

TCPIP_MAC_PACKET* _TCPIP_PKT_PacketAllocDebug(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags, int moduleId)
{
    TCPIP_MAC_PACKET* pPkt = _TCPIP_PKT_PacketAllocInt(pktLen, segLoadLen, flags, moduleId);
    _TCPIP_PKT_TraceAddToEntry(moduleId, pPkt);
    return pPkt;

}


void _TCPIP_PKT_PacketFreeDebug(TCPIP_MAC_PACKET* pPkt, int moduleId)
{
    _TCPIP_PKT_TraceFreeEntry(moduleId, pPkt);
    _TCPIP_PKT_PacketFreeInt(pPkt, moduleId);
}

TCPIP_MAC_DATA_SEGMENT* _TCPIP_PKT_SegmentAllocDebug(uint16_t loadLen, uint16_t loadOffset, TCPIP_MAC_SEGMENT_FLAGS flags, int moduleId)
{
    return _TCPIP_PKT_SegmentAllocInt(loadLen, loadOffset, flags, moduleId); 
}

void _TCPIP_PKT_SegmentFreeDebug(TCPIP_MAC_DATA_SEGMENT* pSeg, int moduleId)
{
    _TCPIP_PKT_SegmentFreeInt(pSeg, moduleId);
}


void _TCPIP_PKT_PacketAcknowledgeDebug(TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes, int moduleId)
{
    _TCPIP_PKT_PacketAcknowledge(pPkt, ackRes, moduleId);
    _TCPIP_PKT_TraceAckEntry(moduleId, pPkt, ackRes);
}

int TCPIP_PKT_TraceGetEntriesNo(TCPIP_PKT_TRACE_INFO* pTraceInfo)
{
    TCPIP_PKT_TRACE_ENTRY *pEntry;
    int ix;
    int nUsed = 0;


    for(ix = 0, pEntry = _pktTraceTbl; ix < sizeof(_pktTraceTbl)/sizeof(*_pktTraceTbl); ix++, pEntry++)
    {
        if(pEntry->moduleId > 0)
        {
            nUsed++;
        }
    }

    _pktTraceInfo.nUsed = nUsed;
    if(pTraceInfo)
    {
        *pTraceInfo = _pktTraceInfo;
    }


    return nUsed;
}


// populates a trace entry with data for a index
bool TCPIP_PKT_TraceGetEntry(int entryIx, TCPIP_PKT_TRACE_ENTRY* tEntry)
{
    TCPIP_PKT_TRACE_ENTRY *pEntry;

    if(entryIx < sizeof(_pktTraceTbl)/sizeof(*_pktTraceTbl))
    {   // valid index
        pEntry = _pktTraceTbl + entryIx;
        if(pEntry->moduleId > 0)
        {
            *tEntry = *pEntry;
            return true;
        }
    }

    return false;
}

static TCPIP_PKT_TRACE_ENTRY* _TCPIP_PKT_TraceFindEntry(int moduleId, bool addNewSlot)
{
    int ix;
    TCPIP_PKT_TRACE_ENTRY    *freeEntry,*pEntry;

    freeEntry = 0;
    for(ix = 0, pEntry = _pktTraceTbl; ix < sizeof(_pktTraceTbl)/sizeof(*_pktTraceTbl); ix++, pEntry++)
    {
        if(pEntry->moduleId == moduleId)
        {
            return pEntry;
        }
        else if(addNewSlot && freeEntry == 0 && pEntry->moduleId == 0)
        {
            freeEntry = pEntry;
        }
    }

    if(freeEntry)
    {
        memset(freeEntry, 0x0, sizeof(*freeEntry));
        freeEntry->moduleId = moduleId;
    }

    return freeEntry;
}

static uint32_t _TCPIP_PKT_TracePktSize(TCPIP_MAC_PACKET* pPkt)
{
    TCPIP_MAC_DATA_SEGMENT* pSeg = pPkt->pDSeg;
    uint32_t pktSize = ((uint8_t*)pSeg - (uint8_t*)pPkt) + TCPIP_MAC_FRAME_OFFSET + sizeof(*pSeg) + pSeg->segSize;

    while((pSeg = pSeg->next) != 0)
    {
        if((pSeg->segFlags & TCPIP_MAC_SEG_FLAG_STATIC) == 0)
        {
            pktSize += sizeof(*pSeg) + pSeg->segSize;
        }
    }

    return pktSize;

}
    
static void _TCPIP_PKT_TraceAddToEntry(int moduleId, TCPIP_MAC_PACKET* pPkt)
{
    TCPIP_PKT_TRACE_ENTRY* pEntry = _TCPIP_PKT_TraceFindEntry(moduleId, true);

    if(pEntry)
    {
        if(pPkt)
        {
            pEntry->totAllocated++;
            pEntry->currAllocated++;
            pEntry->currSize += _TCPIP_PKT_TracePktSize(pPkt);
        }
        else
        {
            pEntry->totFailed++;
        }
    }
    else
    {
        _TCPIP_PKT_TraceFail();
    }

}



static void _TCPIP_PKT_TraceFreeEntry(int moduleId, TCPIP_MAC_PACKET* pPkt)
{
    TCPIP_PKT_TRACE_ENTRY* pEntry = _TCPIP_PKT_TraceFindEntry(moduleId, false);

    if(pEntry)
    {
        pEntry->currAllocated--;
        pEntry->currSize -= _TCPIP_PKT_TracePktSize(pPkt);
    }
    else
    {
        _TCPIP_PKT_TraceFail();
    }

}

static void _TCPIP_PKT_TraceAckEntry(int moduleId, TCPIP_MAC_PACKET* pPkt, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_PKT_TRACE_ENTRY* pEntry = _TCPIP_PKT_TraceFindEntry(moduleId, false);

    if(pEntry)
    {
        pEntry->nAcks++;
        if(ackRes < 0)
        {
            _pktTraceInfo.traceAckErrors++;
        }
    }
    else
    {
        _pktTraceInfo.traceAckOwnerFails++;
    }

}

#else
// regular allocation functions, non-debug
// they don't take a module Id parameter; otherwise exactly the same functions
TCPIP_MAC_PACKET* _TCPIP_PKT_PacketAlloc(uint16_t pktLen, uint16_t segLoadLen, TCPIP_MAC_PACKET_FLAGS flags)
{
    TCPIP_MAC_PACKET* pPkt;
    TCPIP_MAC_DATA_SEGMENT  *pSeg;
    uint16_t        pktUpLen, allocLen;

    if(pktLen < sizeof(TCPIP_MAC_PACKET))
    {
        pktLen = sizeof(TCPIP_MAC_PACKET);
    }

    pktUpLen = (((pktLen + 3) >> 2) << 2);     // 32 bits round up

    allocLen = pktUpLen + sizeof(*pSeg) + segLoadLen + sizeof(TCPIP_MAC_ETHERNET_HEADER) + TCPIP_MAC_FRAME_OFFSET;
    
    pPkt = (TCPIP_MAC_PACKET*)TCPIP_HEAP_Malloc(pktMemH, allocLen);

    if(pPkt)
    {   
        // clear the TCPIP_MAC_PACKET and 1st segment fields
        // populate the 1st segment
        memset(pPkt, 0, pktUpLen + sizeof(*pSeg));
        pSeg = (TCPIP_MAC_DATA_SEGMENT*)((uint8_t*)pPkt + pktUpLen);

        pSeg->segSize = segLoadLen + sizeof(TCPIP_MAC_ETHERNET_HEADER);
        pSeg->segLoadOffset = TCPIP_MAC_FRAME_OFFSET;
        pSeg->segLoad = (uint8_t*)(pSeg + 1) + TCPIP_MAC_FRAME_OFFSET;
        pSeg->segFlags = TCPIP_MAC_SEG_FLAG_STATIC; // embedded in TCPIP_MAC_PACKET itself
        pPkt->pDSeg = pSeg;

        pPkt->pMacLayer = pSeg->segLoad;
        pPkt->pktFlags = flags & (~TCPIP_MAC_PKT_FLAG_STATIC);  // this packet is dynamically allocated
        if(segLoadLen)
        {
            pPkt->pNetLayer = pPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);
        }

        if(pktK0Heap)
        {
            pPkt = (TCPIP_MAC_PACKET*)KVA0_TO_KVA1(pPkt);
        }
    }

    return pPkt;
}

// allocates a socket packet
TCPIP_MAC_PACKET*  _TCPIP_PKT_SocketAlloc(uint16_t pktLen, uint16_t transpHdrLen, uint16_t payloadLen, TCPIP_MAC_PACKET_FLAGS flags)
{
    uint16_t    netHdrLen, totHdrLen;
    TCPIP_MAC_PACKET* pPkt;

    if((flags & TCPIP_MAC_PKT_FLAG_IPV6) != 0)
    {
        netHdrLen = sizeof(IPV6_HEADER);
    }
    else
    {
        netHdrLen = sizeof(IPV4_HEADER);
    }

    totHdrLen = netHdrLen + transpHdrLen;

    pPkt = _TCPIP_PKT_PacketAlloc(pktLen, totHdrLen +  payloadLen, flags );

    if(pPkt)
    {   // set the layer pointers in place
        if(transpHdrLen)
        {
            pPkt->pTransportLayer = pPkt->pNetLayer + netHdrLen;
        }
    }

    return pPkt;
}

// frees a previously allocated packet
void _TCPIP_PKT_PacketFree(TCPIP_MAC_PACKET* pPkt)
{
    if((pPkt->pktFlags & TCPIP_MAC_PKT_FLAG_STATIC) == 0)
    {   // we don't deallocate static packets
        TCPIP_MAC_DATA_SEGMENT  *pSeg, *pNSeg;

        for(pSeg = pPkt->pDSeg; pSeg != 0 ; )
        {
            pNSeg = pSeg->next;
            if((pSeg->segFlags & TCPIP_MAC_SEG_FLAG_STATIC) == 0)
            {
                if(pktK0Heap)
                {
                    pSeg = (TCPIP_MAC_DATA_SEGMENT*)KVA1_TO_KVA0(pSeg);
                }
                TCPIP_HEAP_Free(pktMemH, pSeg);
            }
            pSeg = pNSeg;
        }

        if(pktK0Heap)
        {
            pPkt = (TCPIP_MAC_PACKET*)KVA1_TO_KVA0(pPkt);
        }
        TCPIP_HEAP_Free(pktMemH, pPkt);
    }
}

TCPIP_MAC_DATA_SEGMENT* _TCPIP_PKT_SegmentAlloc(uint16_t loadLen, uint16_t loadOffset, TCPIP_MAC_SEGMENT_FLAGS flags)
{
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    uint16_t allocSize;

    if(loadLen != 0)
    {
        allocSize = sizeof(*pSeg) + loadLen + loadOffset;
    }
    else
    {
        allocSize = sizeof(*pSeg);
    }

    pSeg = (TCPIP_MAC_DATA_SEGMENT*)TCPIP_HEAP_Malloc(pktMemH, allocSize);

    if(pSeg)
    {
        memset(pSeg, 0, sizeof(*pSeg));

        pSeg->segFlags = flags & (~TCPIP_MAC_SEG_FLAG_STATIC);
        if(loadLen != 0)
        {
            pSeg->segSize = loadLen;
            pSeg->segLoadOffset = loadOffset;
            pSeg->segLoad = (uint8_t*)(pSeg + 1) + loadOffset;
        }

        if(pktK0Heap)
        {
            pSeg = (TCPIP_MAC_DATA_SEGMENT*)KVA0_TO_KVA1(pSeg);
        }
        
    }

    return pSeg;
}

void _TCPIP_PKT_SegmentFree(TCPIP_MAC_DATA_SEGMENT* pSeg)
{
    if( (pSeg->segFlags & TCPIP_MAC_SEG_FLAG_STATIC) == 0)
    {
        if(pktK0Heap)
        {
            pSeg = (TCPIP_MAC_DATA_SEGMENT*)KVA1_TO_KVA0(pSeg);
        }
        TCPIP_HEAP_Free(pktMemH, pSeg);
    }
}
#endif  // defined(TCPIP_PACKET_ALLOCATION_TRACE_ENABLE)


#if (TCPIP_PACKET_LOG_ENABLE)


// discards an used log entry
static __inline__ void __attribute__((always_inline)) _TCPIP_PKT_LogDiscardEntry(TCPIP_PKT_LOG_ENTRY* pEntry)
{
    if(pEntry->pPkt != 0)
    {
        _pktLogInfo.nUsed--;
        if((pEntry->logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) != 0)
        {
            _pktLogInfo.nPersistent--;
        }
    }
    pEntry->pPkt = 0;

}

// finds a log entry that matches the pPkt
// if itExists == true, then only an existing packet is searched for
static TCPIP_PKT_LOG_ENTRY* _TCPIP_PKT_LogFindEntry(TCPIP_MAC_PACKET* pPkt, int moduleId, bool itExists)
{
    int ix, lowOvrIx, hiOvrIx;
    TCPIP_PKT_LOG_ENTRY *freeEntry, *pEntry;
    TCPIP_PKT_LOG_ENTRY *pLowOvr, *pHiOvr;
    TCPIP_PKT_LOG_FLAGS logFlags;

    freeEntry = pLowOvr = pHiOvr = 0;
    lowOvrIx = hiOvrIx = 0;
    for(ix = 0, pEntry = _pktLogTbl; ix < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl); ix++, pEntry++)
    {
        if(pEntry->pPkt == 0)
        {   // empty slot
            if(freeEntry == 0)
            {
                freeEntry = pEntry;
            }
            continue;
        }

        if((pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0 && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0)
        {   // can overwrite
            if(ix <= _pktOverwriteIx)
            {
                pLowOvr = pEntry;
                lowOvrIx = ix;
            }
            else if(ix > _pktOverwriteIx && pHiOvr == 0)
            {
                pHiOvr = pEntry;
                hiOvrIx = ix;
            }
        }


        if(pEntry->pPkt == pPkt && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) == 0)
        {   // packet match. this is it
            pEntry->logFlags &= ~(TCPIP_PKT_LOG_FLAG_NEW | TCPIP_PKT_LOG_FLAG_REPLACE);
            return pEntry;
        }

    }

    // check if we have a freeEntry or overwrite to setup
    if(itExists)
    {   
        return 0;
    }

    if(freeEntry)
    {
        pEntry = freeEntry;
        logFlags = TCPIP_PKT_LOG_FLAG_NEW;
    }
    else if(pHiOvr != 0)
    {
        pEntry = pHiOvr;
        logFlags = TCPIP_PKT_LOG_FLAG_REPLACE;
        _pktOverwriteIx = hiOvrIx;
    }
    else if(pLowOvr != 0)
    {
        pEntry = pLowOvr;
        logFlags = TCPIP_PKT_LOG_FLAG_REPLACE;
        _pktOverwriteIx = lowOvrIx;
    }
    else
    {
        pEntry = 0;
        logFlags = TCPIP_PKT_LOG_FLAG_NONE;
    }

    if(pEntry)
    {
        memset(pEntry, 0x0, sizeof(*pEntry));
        pEntry->pPkt = pPkt;
        pEntry->logFlags = logFlags;
    }
    // else we're out of slots...

    return pEntry;
}


static void _TCPIP_PKT_LogCallHandler(TCPIP_PKT_LOG_ENTRY* pLogEntry, TCPIP_STACK_MODULE moduleId)
{
    // check if we need to report this log
    while(_pktLogInfo.logHandler != 0)
    {
        if((_pktLogInfo.logType & TCPIP_PKT_LOG_TYPE_HANDLER_ALL) == 0)
        {
            if((_pktLogInfo.logType & TCPIP_PKT_LOG_TYPE_RX_ONLY) != 0)
            {
                if((pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_RX) == 0)
                {   // don't report
                    break;
                }
            }
            else if((_pktLogInfo.logType & TCPIP_PKT_LOG_TYPE_TX_ONLY) != 0)
            {
                if((pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_TX) == 0)
                {   // don't report
                    break;
                }
            }
            else if((_pktLogInfo.logType & TCPIP_PKT_LOG_TYPE_SKT_ONLY) != 0)
            {
                if((pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_SKT_PARAM) == 0)
                {   // don't report
                    break;
                }
            }

            if((pLogEntry->moduleLog & ~_pktLogInfo.discardMask) == 0)
            {   // don't report
                break;
            }
        }

        // report it
        (*_pktLogInfo.logHandler)(moduleId, pLogEntry);
        break;
    }
}

static TCPIP_PKT_LOG_ENTRY* _TCPIP_PKT_FlightLog(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, TCPIP_PKT_LOG_FLAGS logFlags)
{

    uint16_t netMask = 1 << TCPIP_STACK_NetIxGet((TCPIP_NET_IF*)pPkt->pktIf);

    if((_pktLogInfo.netLogMask & netMask) == 0)
    {   // not logging this
        return 0;
    }

    TCPIP_PKT_LOG_ENTRY* pLogEntry = _TCPIP_PKT_LogFindEntry(pPkt, moduleId, false);

    if(pLogEntry == 0)
    {
        _pktLogInfo.nFailed++;
        return 0;
    }


    if((pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_NEW) != 0)
    {
        _pktLogInfo.nUsed++;
    }
    // else probably a replacement

    if((pLogEntry->logFlags & (TCPIP_PKT_LOG_FLAG_NEW | TCPIP_PKT_LOG_FLAG_REPLACE)) != 0)
    {
        pLogEntry->pktOwner = moduleId;
        pLogEntry->netMask = netMask;
    }

    if(moduleId >= TCPIP_MODULE_MAC_START)
    {
        pLogEntry->macId = moduleId;
    }
    else
    {   // some module can log the same packet multiple times if routed internally
        pLogEntry->moduleLog |= 1 << moduleId;
    }

    pLogEntry->logFlags |= logFlags;


    return pLogEntry;


}

void TCPIP_PKT_FlightLogTx(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId)
{
    TCPIP_PKT_LOG_ENTRY* pLogEntry = _TCPIP_PKT_FlightLog(pPkt, moduleId, TCPIP_PKT_LOG_FLAG_TX);

    if(pLogEntry)
    {
        _TCPIP_PKT_LogCallHandler(pLogEntry, moduleId);
    }
}

void TCPIP_PKT_FlightLogRx(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId)
{
    TCPIP_PKT_LOG_ENTRY* pLogEntry =  _TCPIP_PKT_FlightLog(pPkt, moduleId, TCPIP_PKT_LOG_FLAG_RX);

    if(pLogEntry)
    {
        _TCPIP_PKT_LogCallHandler(pLogEntry, moduleId);
    }
}

void TCPIP_PKT_FlightLogTxSkt(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, uint32_t lclRemPort, uint16_t sktNo )
{
    TCPIP_PKT_LOG_ENTRY* pLogEntry = _TCPIP_PKT_FlightLog(pPkt, moduleId, TCPIP_PKT_LOG_FLAG_TX);

    if(pLogEntry)
    {
        pLogEntry->logFlags |= TCPIP_PKT_LOG_FLAG_SKT_PARAM;
        pLogEntry->sktNo =  sktNo;
        pLogEntry->lclPort =  (uint16_t)(lclRemPort >> 16);
        pLogEntry->remPort =  (uint16_t)(lclRemPort);
        _TCPIP_PKT_LogCallHandler(pLogEntry, moduleId);
    }
}

void TCPIP_PKT_FlightLogRxSkt(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, uint32_t lclRemPort, uint16_t sktNo )
{
    TCPIP_PKT_LOG_ENTRY* pLogEntry = _TCPIP_PKT_FlightLog(pPkt, moduleId, TCPIP_PKT_LOG_FLAG_RX);

    if(pLogEntry)
    {
        pLogEntry->logFlags |= TCPIP_PKT_LOG_FLAG_SKT_PARAM;
        pLogEntry->sktNo =  sktNo;
        pLogEntry->lclPort =  (uint16_t)(lclRemPort >> 16);
        pLogEntry->remPort =  (uint16_t)(lclRemPort);
        _TCPIP_PKT_LogCallHandler(pLogEntry, moduleId);
    }
}


void TCPIP_PKT_FlightLogAcknowledge(TCPIP_MAC_PACKET* pPkt, TCPIP_STACK_MODULE moduleId, TCPIP_MAC_PKT_ACK_RES ackRes)
{

    TCPIP_PKT_LOG_ENTRY* pLogEntry = _TCPIP_PKT_LogFindEntry(pPkt, moduleId, true);

    if(pLogEntry != 0)
    {
        pLogEntry->logFlags |= TCPIP_PKT_LOG_FLAG_DONE;
        pLogEntry->ackRes = ackRes;
        pLogEntry->pktAcker = moduleId;
        // store the module to log it
        pLogEntry->moduleLog |= 1 << moduleId; 

        bool discardPkt = false;

        // check if we keep this log
        while(true)
        {
            if((_pktLogInfo.logType & TCPIP_PKT_LOG_TYPE_RX_ONLY) != 0)
            {
                if((pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_RX) == 0)
                {   // discard it
                    discardPkt = true;
                    break;
                }
            }
            else if((_pktLogInfo.logType & TCPIP_PKT_LOG_TYPE_TX_ONLY) != 0)
            {
                if((pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_TX) == 0)
                {   // discard it
                    discardPkt = true;
                    break;
                }
            }
            else if((_pktLogInfo.logType & TCPIP_PKT_LOG_TYPE_SKT_ONLY) != 0 && (pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_SKT_PARAM) == 0)
            {   // discard it
                discardPkt = true;
                break;
            }
            else if((pLogEntry->logFlags & TCPIP_PKT_LOG_FLAG_SKT_PARAM) != 0)
            {   // a socket entry; check against skt discard mask
                if(((1 << pLogEntry->sktNo) & ~_pktLogInfo.sktDiscardMask) == 0)
                {   // discard it
                    discardPkt = true;
                    break;
                }
            }

            // see if this is to be made persistent
            if((pLogEntry->moduleLog & _pktLogInfo.persistMask) != 0)
            {
                pLogEntry->logFlags |= TCPIP_PKT_LOG_FLAG_PERSISTENT;
                _pktLogInfo.nPersistent++;
            }
            else if((pLogEntry->moduleLog & ~_pktLogInfo.discardMask) == 0)
            {   // discard it
                discardPkt = true;
            }
            // else keep it

            break;
        }

        if(discardPkt)
        {
            _TCPIP_PKT_LogDiscardEntry(pLogEntry);
        }
    }
    // else it can happen if the service was stopped!
}

bool  TCPIP_PKT_FlightLogGetInfo(TCPIP_PKT_LOG_INFO* pLogInfo)
{
    if(pLogInfo)
    {
        *pLogInfo = _pktLogInfo;
    }

    return true;
}

// populates a log entry with data for a index
bool TCPIP_PKT_FlightLogGetEntry(int entryIx, TCPIP_PKT_LOG_ENTRY* pLEntry)
{
    TCPIP_PKT_LOG_ENTRY* pEntry;

    if(entryIx < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl))
    {
        pEntry = _pktLogTbl + entryIx;
        if(pEntry->pPkt != 0)
        {
            if(pLEntry != 0)
            {
                *pLEntry = *pEntry;
            }
            return true;
        }
    }

    return false;
}

bool TCPIP_PKT_FlightLogRegister(TCPIP_PKT_LOG_HANDLER logHandler, bool logAll)
{
    _pktLogInfo.logHandler = logHandler;
    if(logAll)
    {
        _pktLogInfo.logType |= TCPIP_PKT_LOG_TYPE_HANDLER_ALL; 
    }
    else
    {
        _pktLogInfo.logType &= ~TCPIP_PKT_LOG_TYPE_HANDLER_ALL; 
    }
    return true;
}

bool TCPIP_PKT_FlightLogTypeSet(TCPIP_PKT_LOG_TYPE logType, bool clrPersist)
{
    TCPIP_PKT_LOG_FLAGS searchRxTx;
    bool                sktsOnly = false;
    TCPIP_PKT_LOG_TYPE oldType = _pktLogInfo.logType & (TCPIP_PKT_LOG_TYPE_RX_ONLY | TCPIP_PKT_LOG_TYPE_TX_ONLY | TCPIP_PKT_LOG_TYPE_SKT_ONLY);

    logType &= (TCPIP_PKT_LOG_TYPE_RX_ONLY | TCPIP_PKT_LOG_TYPE_TX_ONLY | TCPIP_PKT_LOG_TYPE_SKT_ONLY);
    // don't allow unknown types/filters
    if((logType & (TCPIP_PKT_LOG_TYPE_RX_ONLY | TCPIP_PKT_LOG_TYPE_TX_ONLY)) == (TCPIP_PKT_LOG_TYPE_RX_ONLY | TCPIP_PKT_LOG_TYPE_TX_ONLY)) 
    {   // these 2 are exclusive
       return false;
    } 

    if((logType & TCPIP_PKT_LOG_TYPE_RX_ONLY) != 0) 
    {   
        searchRxTx = TCPIP_PKT_LOG_FLAG_RX;
    }
    else if((logType & TCPIP_PKT_LOG_TYPE_TX_ONLY) != 0) 
    {   
        searchRxTx = TCPIP_PKT_LOG_FLAG_TX;
    }
    else
    {
        logType = TCPIP_PKT_LOG_TYPE_RX_TX;
        searchRxTx = 0;
        sktsOnly = (logType & TCPIP_PKT_LOG_TYPE_SKT_ONLY) != 0;
    }

    if(oldType != logType)
    {
        _pktLogInfo.logType = logType;

        if(logType != TCPIP_PKT_LOG_TYPE_RX_TX)
        {   // only RX or TX needed; clean up the log
            int ix;
            TCPIP_PKT_LOG_ENTRY *pEntry;

            for(ix = 0, pEntry = _pktLogTbl; ix < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl); ix++, pEntry++)
            {
                if(pEntry->pPkt != 0 && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0)
                {   // non empty completed slot
                    if(clrPersist || (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0)
                    {   // can check if needs to be discarded 
                        if((pEntry->logFlags & searchRxTx) == 0 || (sktsOnly && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_SKT_PARAM) == 0))
                        {
                            _TCPIP_PKT_LogDiscardEntry(pEntry);
                        }
                    }
                }
            }
        }
    }

    return true;
}

void TCPIP_PKT_FlightLogSetDiscardMask(uint32_t moduleMask, TCPIP_PKT_LOG_MASK_OP discardOp, bool clrPersist)
{
    int ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;


    switch(discardOp)
    {
        case TCPIP_PKT_LOG_MASK_AND:
            _pktLogInfo.discardMask &= moduleMask;
            break;

        case TCPIP_PKT_LOG_MASK_NOR:
            _pktLogInfo.discardMask |= ~moduleMask;
            break;

        case TCPIP_PKT_LOG_MASK_NAND:
            _pktLogInfo.discardMask &= ~moduleMask;
            break;

        default:    // TCPIP_PKT_LOG_MASK_OR
            _pktLogInfo.discardMask |= moduleMask;
            break;
    }

    // only up to and including TCPIP_MODULE_LAYER3 modules are logged
    _pktLogInfo.discardMask &= (1 << (TCPIP_MODULE_LAYER3 + 1)) - 1;


    // apply discard mask it to all completed logs...
    for(ix = 0, pEntry = _pktLogTbl; ix < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl); ix++, pEntry++)
    {
        if(pEntry->pPkt != 0 && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0)
        {   // non empty completed slot
            if(clrPersist || (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0)
            {
                if((pEntry->moduleLog & ~_pktLogInfo.discardMask) == 0)
                {
                    _TCPIP_PKT_LogDiscardEntry(pEntry);
                }
            }
        }
    }
}

void TCPIP_PKT_FlightLogSetPersistMask(uint32_t moduleMask, TCPIP_PKT_LOG_MASK_OP persistOp, bool clrNonPersist)
{
    int ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;

    switch(persistOp)
    {
        case TCPIP_PKT_LOG_MASK_AND:
            _pktLogInfo.persistMask &= moduleMask;
            break;

        case TCPIP_PKT_LOG_MASK_NOR:
            _pktLogInfo.persistMask |= ~moduleMask;
            break;

        case TCPIP_PKT_LOG_MASK_NAND:
            _pktLogInfo.persistMask &= ~moduleMask;
            break;

        default:    // TCPIP_PKT_LOG_MASK_OR
            _pktLogInfo.persistMask |= moduleMask;
            break;
    }

    // only up to and including TCPIP_MODULE_LAYER3 modules are logged
    _pktLogInfo.persistMask &= (1 << (TCPIP_MODULE_LAYER3 + 1)) - 1;

    // apply it to all completed logs...
    for(ix = 0, pEntry = _pktLogTbl; ix < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl); ix++, pEntry++)
    {
        if(pEntry->pPkt != 0 && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0)
        {   // non empty completed slot
            if((pEntry->moduleLog & _pktLogInfo.persistMask) != 0)
            {
                pEntry->logFlags |= TCPIP_PKT_LOG_FLAG_PERSISTENT;
            }
            else if(clrNonPersist)
            {
                _TCPIP_PKT_LogDiscardEntry(pEntry);
            }
            else
            {   // keep it but mark it non persistent
                pEntry->logFlags &= ~TCPIP_PKT_LOG_FLAG_PERSISTENT;
            }
        }
    }
}

void TCPIP_PKT_FlightLogSetNetMask(uint32_t netMask, TCPIP_PKT_LOG_MASK_OP netOp, bool clrPersist)
{
    int ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;

    switch(netOp)
    {
        case TCPIP_PKT_LOG_MASK_AND:
            _pktLogInfo.netLogMask &= netMask;
            break;

        case TCPIP_PKT_LOG_MASK_NOR:
            _pktLogInfo.netLogMask |= ~netMask;
            break;

        case TCPIP_PKT_LOG_MASK_NAND:
            _pktLogInfo.netLogMask &= ~netMask;
            break;

        default:    // TCPIP_PKT_LOG_MASK_OR
            _pktLogInfo.netLogMask |= netMask;
            break;
    }

    // apply it to all completed logs...
    for(ix = 0, pEntry = _pktLogTbl; ix < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl); ix++, pEntry++)
    {
        if(pEntry->pPkt != 0 && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0)
        {   // non empty completed slot
            if(clrPersist || (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0)
            {
                if((pEntry->netMask & _pktLogInfo.netLogMask) == 0)
                {
                    _TCPIP_PKT_LogDiscardEntry(pEntry);
                }
            }
        }
    }

}  

void TCPIP_PKT_FlightLogSetSocketDiscardMask(uint32_t sktMask, TCPIP_PKT_LOG_MASK_OP netOp, bool clrPersist)
{
    int ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;

    switch(netOp)
    {
        case TCPIP_PKT_LOG_MASK_AND:
            _pktLogInfo.sktDiscardMask &= sktMask;
            break;

        case TCPIP_PKT_LOG_MASK_NOR:
            _pktLogInfo.sktDiscardMask |= ~sktMask;
            break;

        case TCPIP_PKT_LOG_MASK_NAND:
            _pktLogInfo.sktDiscardMask &= ~sktMask;
            break;

        default:    // TCPIP_PKT_LOG_MASK_OR
            _pktLogInfo.sktDiscardMask |= sktMask;
            break;
    }

    // apply it to all completed logs...
    for(ix = 0, pEntry = _pktLogTbl; ix < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl); ix++, pEntry++)
    {
        if(pEntry->pPkt != 0 && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0)
        {   // non empty completed slot
            if(clrPersist || (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0)
            {
                if((pEntry->logFlags & TCPIP_PKT_LOG_FLAG_SKT_PARAM) != 0)
                {
                    if(((1 << pEntry->sktNo) & ~_pktLogInfo.sktDiscardMask) == 0)
                    {
                        _TCPIP_PKT_LogDiscardEntry(pEntry);
                    }
                }
            }
        }
    }

}  
void TCPIP_PKT_FlightLogClear(bool clrPersist)
{
    int ix;
    TCPIP_PKT_LOG_ENTRY *pEntry;

    // discard all completed logs...
    for(ix = 0, pEntry = _pktLogTbl; ix < sizeof(_pktLogTbl) / sizeof(*_pktLogTbl); ix++, pEntry++)
    {
        if(pEntry->pPkt != 0 && (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_DONE) != 0)
        {   // non empty completed slot
            if(clrPersist || (pEntry->logFlags & TCPIP_PKT_LOG_FLAG_PERSISTENT) == 0)
            {
                _TCPIP_PKT_LogDiscardEntry(pEntry);
            }
        }
    }
}


#endif  //  (TCPIP_PACKET_LOG_ENABLE)



