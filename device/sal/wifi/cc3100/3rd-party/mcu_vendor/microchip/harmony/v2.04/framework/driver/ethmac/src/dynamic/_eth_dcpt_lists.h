/*********************************************************************
 *
 *                  Ethernet helper header file
 *
 *********************************************************************
 * File Name:       _eth_dcpt_lists.h
 * Dependencies:
 * Processor:       PIC32
 *
 * Compiler:        MPLAB XC32
 *                  MPLAB IDE
 * Company:         Microchip Technology Inc.
 *
 * Software License Agreement
 * Microchip Audio Library – PIC32 Software.
 * Copyright © 2008 Microchip Technology Inc.  All rights reserved.
 *
 * Microchip licenses the Software for your use with Microchip microcontrollers
 * and Microchip digital signal controllers pursuant to the terms of the
 * Non-Exclusive Software License Agreement accompanying this Software.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY
 * OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION,
 * ANY WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS
 * FOR A PARTICULAR PURPOSE.
 * MICROCHIP AND ITS LICENSORS ASSUME NO RESPONSIBILITY FOR THE ACCURACY,
 * RELIABILITY OR APPLICATION OF THE SOFTWARE AND DOCUMENTATION.
 * IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED
 * UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH
 * OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL,
 * SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS
 * OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY,
 * SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED
 * TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *
 *$Id: $
 ********************************************************************/


#ifndef __ETH_DCPT_LISTS_H_
#define __ETH_DCPT_LISTS_H_

#include <sys/kmem.h>

// *****************************************************************************
/*  Packet Descriptor

  Summary:
    Descriptor of a packet accepted by the TX/RX Ethernet engine.

  Description:
    A packet handled by the Ethernet TX/RX engine is a list of buffer
    descriptors.  A packet consists of multiple buffers and each buffer needs a
    descriptor.  Although the number of buffers per packet is not limited, note
    that the hardware overhead is higher when many buffers have to be handled
    for one packet.  The list ends when the next field is NULL or when the pBuff
    is NULL.
*/

typedef struct _tag_DRV_ETHMAC_PKT_DCPT
{
    // Next descriptor in chain. NULL to end
    struct _tag_DRV_ETHMAC_PKT_DCPT  *next;

    // Buffer to be transmitted
    void                   *pBuff;

    // Number of bytes in the buffer 0-2047 allowed
    unsigned short int      nBytes;

} /*DOM-IGNORE-BEGIN*/ __attribute__ ((__packed__)) /*DOM-IGNORE-END*/ DRV_ETHMAC_PKT_DCPT;


// *****************************************************************************
/*  Transmitted Packet Status

  Summary:
    Status of a transmitted packet.

  Description:
    This structure contains the status of a transmitted packet.

  Notes:
    Status is always two "words" (64-bits) long.
*/

typedef union
{
    struct
    {
        // Total bytes transmitted
        unsigned        totTxBytes   :16;

        // Control frame transmitted
        unsigned         txCtrl      : 1;

        // Pause control frame transmitted
        unsigned         txPause     : 1;

        // Transmit backpressure applied
        unsigned         txBPres     : 1;

        // Transmit VLAN tagged frame
        unsigned         txVLAN      : 1;

        unsigned                     : 12;

        // Transmit bytes count
        unsigned         bCount      :16;

        // Transmit collision count
        unsigned        collCount   : 4;

        // Transmit CRC error
        unsigned         crcError    : 1;

        // Transmit length check error
        unsigned         lenError    : 1;

        // Transmit length out of range
        unsigned         lenRange    : 1;

        // Transmit done
        unsigned         txDone      : 1;

        // Transmit multicast
        unsigned         mcast       : 1;

        // Transmit broadcast
        unsigned         bcast       : 1;

        // Transmit packet defer
        unsigned         defer       : 1;

        // Transmit excessive packet defer
        unsigned         excDefer    : 1;

        // Transmit maximum collision
        unsigned         maxColl     : 1;

        // Transmit late collision
        unsigned         lateColl    : 1;

        // Transmit giant frame (set when pktSz>MaxFrameSz && HugeFrameEn==0)
        unsigned         giant       : 1;

        // Transmit underrun
        unsigned         underrun    : 1;

    } __attribute__ ((__packed__));

    // Status is 2 words always
    unsigned long long  w;

} DRV_ETHMAC_PKT_STAT_TX;


// *****************************************************************************
/*  Received Packet Status

  Summary:
    Status of a received packet.

  Description:
    This structure contains the status of a received packet.

  Notes:
    Status is always two "words" (64-bits) long.
*/

typedef union
{
    struct
    {
        // Packet payload checksum
        unsigned        pktChecksum     :16;

        unsigned                        : 8;

        // Runt packet received
        unsigned        runtPkt         : 1;

        // Unicast, not me packet,
        unsigned        notMeUcast      : 1;

        // Hash table match
        unsigned        htMatch         : 1;

        // Magic packet match
        unsigned        magicMatch      : 1;

        // Pattern match match
        unsigned        pmMatch         : 1;

        // Unicast match
        unsigned        uMatch          : 1;

        // Broadcast match
        unsigned        bMatch          : 1;

        // Multicast match
        unsigned        mMatch          : 1;

        // Received bytes
        unsigned        rxBytes         :16;

        // Packet previously ignored
        unsigned        prevIgnore      : 1;

        // RX data valid event previously seen
        unsigned        prevDV          : 1;

        // Carrier event previously seen
        unsigned        prevCarrier     : 1;

        // RX code violation
        unsigned        rxCodeViol      : 1;

        // CRC error in packet
        unsigned        crcError        : 1;

        // Receive length check error
        unsigned        lenError        : 1;

        // Receive length out of range
        unsigned        lenRange        : 1;

        // Receive OK
        unsigned        rxOk            : 1;

        // Multicast packet
        unsigned        mcast           : 1;

        // Broadcast packet
        unsigned        bcast           : 1;

        // Dribble nibble
        unsigned        dribble         : 1;

        // Control frame received
        unsigned        rxCtrl          : 1;

        // Pause control frame received
        unsigned        rxPause         : 1;

        // Received unsupported code
        unsigned        rxCodeErr       : 1;

        // Received VLAN tagged frame
        unsigned        rxVLAN          : 1;

        unsigned                        : 1;

    }__attribute__ ((__packed__));

    // Status is 2 words always
    unsigned long long  w;

} DRV_ETHMAC_PKT_STAT_RX;


// descriptors

typedef union
{
    struct
    {
        unsigned        : 7;
        unsigned EOWN   : 1;
        unsigned NPV    : 1;
        unsigned sticky : 1;    // a receive buffer is sticky
        unsigned kv0    : 1;    // belongs to k0. else k1
        unsigned rx_wack: 1;    // RX buffer waiting for acknowledge
        unsigned rx_nack: 1;    // RX descriptor is not acknowledged
        unsigned        : 3;
        unsigned bCount : 11;
        unsigned        : 3;
        unsigned EOP    : 1;
        unsigned SOP    : 1;
    };
    unsigned int    w;
}DRV_ETHMAC_DCPT_HDR;  // descriptor header

#define _SDCPT_HDR_EOWN_MASK_       0x00000080
#define _SDCPT_HDR_NPV_MASK_        0x00000100
#define _SDCPT_HDR_STICKY_MASK_     0x00000200
#define _SDCPT_HDR_KV0_MASK_        0x00000400
#define _SDCPT_HDR_RX_WACK_MASK_    0x00000800
#define _SDCPT_HDR_BCOUNT_MASK_     0x07ff0000
#define _SDCPT_HDR_BCOUNT_POS_      16
#define _SDCPT_HDR_EOP_MASK_        0x40000000
#define _SDCPT_HDR_SOP_MASK_        0x80000000


typedef struct
{
    volatile DRV_ETHMAC_DCPT_HDR       hdr;        // header
    unsigned char*          pEDBuff;    // data buffer address
    volatile unsigned long long stat;       // TX/RX packet status
    unsigned int            next_ed;    // next descriptor (hdr.NPV==1);
}__attribute__ ((__packed__)) DRV_ETHMAC_HW_DCPT;   // hardware TX/RX descriptor (linked).


typedef struct __attribute__ ((__packed__))
{
    volatile DRV_ETHMAC_DCPT_HDR       hdr;        // header
    unsigned char*          pEDBuff;    // data buffer address
    volatile DRV_ETHMAC_PKT_STAT_TX  stat;       // transmit packet status
    unsigned int            next_ed;    // next descriptor (hdr.NPV==1);
} DRV_ETHMAC_HW_DCPT_TX; // hardware TX descriptor (linked).


typedef struct __attribute__ ((__packed__))
{
    volatile DRV_ETHMAC_DCPT_HDR       hdr;        // header
    unsigned char*          pEDBuff;    // data buffer address
    volatile DRV_ETHMAC_PKT_STAT_RX  stat;       // transmit packet status
    unsigned int            next_ed;    // next descriptor (hdr.NPV==1);
} DRV_ETHMAC_HW_DCPT_RX; // hardware Rx descriptor (linked).



typedef struct _tag_DRV_ETHMAC_DCPT_NODE
{
    struct _tag_DRV_ETHMAC_DCPT_NODE*      next;       // next pointer in virtual space
    DRV_ETHMAC_HW_DCPT          hwDcpt;     // the associated hardware descriptor
}DRV_ETHMAC_DCPT_NODE; // Ethernet descriptor node: generic linked descriptor for both TX/RX.


typedef struct
{
    struct _tag_DRV_ETHMAC_DCPT_NODE*      next;       // next pointer in virtual space
    DRV_ETHMAC_HW_DCPT_TX            hwDcpt;     // the associated hardware descriptor
}DRV_ETHMAC_DCPT_NODE_TX;   // TX linked descriptor


typedef struct
{
    struct _tag_DRV_ETHMAC_DCPT_NODE*      next;       // next pointer in virtual space
    DRV_ETHMAC_HW_DCPT_RX            hwDcpt;     // the associated hardware descriptor
}DRV_ETHMAC_DCPT_NODE_RX;   // Rx linked descriptor

// although there could be unlinked TX and RX descriptors (hdr.NPV==0), we don't use them in this implementation


#if defined(__PIC32MZ__)
// for PIC32MZ these lists have to be aligned on a cache line
#define DRV_ETHMAC_DCPT_LIST_ALIGN      16
typedef struct __attribute__((aligned(16)))
{
    uint64_t    pad;
    DRV_ETHMAC_DCPT_NODE*  head;   // list head
    DRV_ETHMAC_DCPT_NODE*  tail;
}DRV_ETHMAC_DCPT_LIST;  // single linked list
#else
// for PIC32MX alignment is 1 byte
#define DRV_ETHMAC_DCPT_LIST_ALIGN      1
typedef struct
{
    DRV_ETHMAC_DCPT_NODE*  head;   // list head
    DRV_ETHMAC_DCPT_NODE*  tail;
}DRV_ETHMAC_DCPT_LIST;  // single linked list
#endif  // defined(__PIC32MZ__)

/////  single linked lists manipulation ///////////
//
#define     DRV_ETHMAC_LIB_ListIsEmpty(pL)       ((pL)->head==0)

extern __inline__ int __attribute__((always_inline)) DRV_ETHMAC_LIB_ListCount(DRV_ETHMAC_DCPT_LIST* pL)
{

    DRV_ETHMAC_DCPT_NODE*  pN;
    int     nNodes=0;
    for(pN=pL->head; pN!=0; pN=pN->next)
    {
        nNodes++;
    }
    return nNodes;
}


extern __inline__ void __attribute__((always_inline)) DRV_ETHMAC_LIB_ListAddHead(DRV_ETHMAC_DCPT_LIST* pL, DRV_ETHMAC_DCPT_NODE* pN)
{
    pN->next=pL->head;
    pL->head=pN;
    pN->hwDcpt.next_ed=KVA_TO_PA(&pL->head->hwDcpt);
    if(pL->tail==0)
    {  // empty list
        pL->tail=pN;
    }

}


extern __inline__ void __attribute__((always_inline)) DRV_ETHMAC_LIB_ListAddTail(DRV_ETHMAC_DCPT_LIST* pL, DRV_ETHMAC_DCPT_NODE* pN)
{
    pN->next=0;
    pN->hwDcpt.next_ed = 0;
    if(pL->tail==0)
    {
        pL->head=pL->tail=pN;
    }
    else
    {
        pL->tail->next=pN;
        pL->tail->hwDcpt.next_ed=KVA_TO_PA(&pN->hwDcpt);
        pL->tail=pN;
    }

}


// insertion in the middle, not head or tail
#define     SlAddMid(pL, pN, after) do{ (pN)->next=(after)->next; (pN)->next_ed=(after)->next_ed; \
                            (after)->next=(pN); (after)->next_ed=KVA_TO_PA(&pN->hwDcpt); \
                        }while(0)


extern __inline__ DRV_ETHMAC_DCPT_NODE* __attribute__((always_inline)) DRV_ETHMAC_LIB_ListRemoveHead(DRV_ETHMAC_DCPT_LIST* pL)
{
    DRV_ETHMAC_DCPT_NODE* pN=pL->head;
    if(pL->head==pL->tail)
    {
        pL->head=pL->tail=0;
    }
    else
    {
        pL->head=pN->next;
    }

    return pN;
}


extern __inline__ void __attribute__((always_inline)) DRV_ETHMAC_LIB_ListAppendTail(DRV_ETHMAC_DCPT_LIST* pL, DRV_ETHMAC_DCPT_LIST* pAList)
{
    DRV_ETHMAC_DCPT_NODE* pN;
    while((pN=DRV_ETHMAC_LIB_ListRemoveHead(pAList)))
    {
        DRV_ETHMAC_LIB_ListAddTail(pL, pN);
    }
}


#if defined(__PIC32MZ__)
// flushes a data cache line/address
extern __inline__ void __attribute__((always_inline)) DRV_ETHMAC_LIB_DataLineFlush(void* address)
{   // issue a hit-writeback invalidate order
    __asm__ __volatile__ ("cache 0x15, 0(%0)" ::"r"(address));
    __asm__ __volatile__ ("sync");
}
#endif  // defined(__PIC32MZ__)


extern __inline__ DRV_ETHMAC_DCPT_LIST* __attribute__((always_inline)) DRV_ETHMAC_LIB_ListInit(DRV_ETHMAC_DCPT_LIST* pL)
{
#if defined(__PIC32MZ__)
    DRV_ETHMAC_LIB_DataLineFlush(pL);
    DRV_ETHMAC_DCPT_LIST* pNL = KVA0_TO_KVA1(pL);
#else
    DRV_ETHMAC_DCPT_LIST* pNL = pL;
#endif

    pNL->head = pNL->tail = 0;
    return pNL;
}

/////  generic single linked lists manipulation ///////////
//
//
typedef struct _TAG_DRV_ETHMAC_SGL_LIST_NODE
{
	struct _TAG_DRV_ETHMAC_SGL_LIST_NODE*	next;		// next node in list
    void*                                   data[0];    // generic payload    
}DRV_ETHMAC_SGL_LIST_NODE;	// generic linked list node definition


typedef struct
{
	DRV_ETHMAC_SGL_LIST_NODE*	head;	// list head
	DRV_ETHMAC_SGL_LIST_NODE*	tail;
    int             nNodes; // number of nodes in the list

}DRV_ETHMAC_SGL_LIST;	// single linked list

extern __inline__ void  __attribute__((always_inline)) DRV_ETHMAC_SingleListInitialize(DRV_ETHMAC_SGL_LIST* pL)
{
    pL->head = pL->tail = 0;
    pL->nNodes = 0;
}



extern __inline__ int __attribute__((always_inline)) DRV_ETHMAC_SingleListCount(DRV_ETHMAC_SGL_LIST* pL)
{
    return pL->nNodes;
}

// removes the head node
DRV_ETHMAC_SGL_LIST_NODE*  DRV_ETHMAC_SingleListHeadRemove(DRV_ETHMAC_SGL_LIST* pL);

void  DRV_ETHMAC_SingleListTailAdd(DRV_ETHMAC_SGL_LIST* pL, DRV_ETHMAC_SGL_LIST_NODE* pN);

#endif //  __ETH_DCPT_LISTS_H_
