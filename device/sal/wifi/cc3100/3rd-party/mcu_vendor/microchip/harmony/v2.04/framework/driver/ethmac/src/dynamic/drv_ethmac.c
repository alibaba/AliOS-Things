/*******************************************************************************
  MAC Module (Microchip PIC32MX5-7) for Microchip TCP/IP Stack
*******************************************************************************/

/*******************************************************************************
File Name:  eth_pic32_int_mac.c
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

#include "driver/ethmac/src/dynamic/drv_ethmac_lib.h"

/** D E F I N I T I O N S ****************************************************/

#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MAC_PIC32INT

#define ETHER_IP    (0x00u)
#define ETHER_ARP   (0x06u)


// All pause capabilities our MAC supports
#define DRV_ETHMAC_PAUSE_CPBL_MASK     (TCPIP_ETH_PAUSE_TYPE_ALL)

// MAX frame defines
// round up to multiple of 16
#define _TCPIP_EMAC_MAX_FRAME (((TCPIP_EMAC_MAX_FRAME + 15) / 16) * 16)

// MTU defines
#define _TCPIP_EMAC_LINK_MTU_ETH_MIN    576                         // min: ETH default, shouldn't go below this value
#define _TCPIP_EMAC_LINK_MTU_ETH_MAX    1500                        // max: ETH default, shouldn't go above this value

#if (_TCPIP_EMAC_MAX_FRAME - 18) > _TCPIP_EMAC_LINK_MTU_ETH_MAX
#define _TCPIP_EMAC_LINK_MTU_MAX    _TCPIP_EMAC_LINK_MTU_ETH_MAX    // max: the ETH default
#else 
#define _TCPIP_EMAC_LINK_MTU_MAX    (_TCPIP_EMAC_MAX_FRAME - 18) // max dictated by the max frame
#endif


#if TCPIP_EMAC_LINK_MTU == 0
#define _TCPIP_EMAC_LINK_MTU _TCPIP_EMAC_LINK_MTU_MAX           // use maximum value
#elif TCPIP_EMAC_LINK_MTU < _TCPIP_EMAC_LINK_MTU_ETH_MIN
#define _TCPIP_EMAC_LINK_MTU _TCPIP_EMAC_LINK_MTU_ETH_MIN       // use minimum value
#elif TCPIP_EMAC_LINK_MTU > _TCPIP_EMAC_LINK_MTU_MAX
#define _TCPIP_EMAC_LINK_MTU _TCPIP_EMAC_LINK_MTU_MAX           // use maximum value
#else
#define _TCPIP_EMAC_LINK_MTU TCPIP_EMAC_LINK_MTU                // use supplied value
#endif

// Flow control defines
//
#define _TCPIP_EMAC_QUANTA_PAUSE_BYTES      64  // mimimum flow control quanta PTV 

typedef enum
{
    DRV_ETHMAC_ADDBUFF_FLAG_NONE         = 0x0000,       // default value
    DRV_ETHMAC_ADDBUFF_FLAG_STICKY       = 0x0001,       // add sticky buffers
    DRV_ETHMAC_ADDBUFF_FLAG_SETSIZE      = 0x0002,       // set the RX size when adding buffers
    DRV_ETHMAC_ADDBUFF_FLAG_RX_LOCK      = 0x0004,       // request a synch lock when adding RX buffers

}DRV_ETHMAC_ADDBUFF_FLAGS;

/******************************************************************************
 * Prototypes
 ******************************************************************************/
static void*    _MacCallocCallback( size_t nitems, size_t size, void* param );

#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
static void     _MacTxFreeCallback(  void* ptr, void* param );
static void     _MacRxFreeCallback(  void* ptr, void* param );
static void     _MACCleanup(DRV_ETHMAC_INSTANCE_DCPT* pMacD );
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

static void     _MACDeinit(DRV_ETHMAC_INSTANCE_DCPT* pMacD );

static TCPIP_MAC_RES    _MACTxPacket(DRV_ETHMAC_INSTANCE_DCPT* pMacD, TCPIP_MAC_PACKET * ptrPacket);
static void             _MACTxAcknowledgeEth(DRV_ETHMAC_INSTANCE_DCPT* pMacD);
static void             _MACTxPacketAckCallback(void* pPktBuff, int buffIx, void* fParam);
static TCPIP_MAC_RES    _MacTxPendingPackets(DRV_ETHMAC_INSTANCE_DCPT* pMacD);

static void             _MacTxDiscardQueues(DRV_ETHMAC_INSTANCE_DCPT* pMacD, TCPIP_MAC_PKT_ACK_RES ackRes);

static bool             _MacRxPacketAck(TCPIP_MAC_PACKET* pkt,  const void* param);

// MAC interface functions
SYS_MODULE_OBJ          DRV_ETHMAC_PIC32MACInitialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
void                    DRV_ETHMAC_PIC32MACDeinitialize(SYS_MODULE_OBJ object);
void                    DRV_ETHMAC_PIC32MACReinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init);
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
SYS_STATUS              DRV_ETHMAC_PIC32MACStatus( SYS_MODULE_OBJ object );
void                    DRV_ETHMAC_PIC32MACTasks( SYS_MODULE_OBJ object );
DRV_HANDLE              DRV_ETHMAC_PIC32MACOpen(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);
void                    DRV_ETHMAC_PIC32MACClose( DRV_HANDLE hMac );
bool                    DRV_ETHMAC_PIC32MACLinkCheck(DRV_HANDLE hMac);
TCPIP_MAC_RES           DRV_ETHMAC_PIC32MACRxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr);
bool                    DRV_ETHMAC_PIC32MACPowerMode(DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode);
TCPIP_MAC_RES           DRV_ETHMAC_PIC32MACPacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket);
TCPIP_MAC_PACKET*       DRV_ETHMAC_PIC32MACPacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, const TCPIP_MAC_PACKET_RX_STAT** ppPktStat);
TCPIP_MAC_RES           DRV_ETHMAC_PIC32MACProcess(DRV_HANDLE hMac);
TCPIP_MAC_RES           DRV_ETHMAC_PIC32MACStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics);
TCPIP_MAC_RES           DRV_ETHMAC_PIC32MACParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams);
TCPIP_MAC_RES           DRV_ETHMAC_PIC32MACRegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries);
size_t                  DRV_ETHMAC_PIC32MACConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize);


bool                    DRV_ETHMAC_PIC32MACEventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable);
bool                    DRV_ETHMAC_PIC32MACEventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents);
TCPIP_MAC_EVENT         DRV_ETHMAC_PIC32MACEventPendingGet(DRV_HANDLE hMac);

static TCPIP_MAC_RES    DRV_ETHMAC_PIC32MACEventInit(DRV_HANDLE hMac, TCPIP_MAC_EventF eventF, const void* eventParam);
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
static TCPIP_MAC_RES    DRV_ETHMAC_PIC32MACEventDeInit(DRV_HANDLE hMac);
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

#if defined(ETH_PIC32_INT_MAC_ISR_TX)
static void             _MACTxAcknowledgeAckQueue(DRV_ETHMAC_INSTANCE_DCPT* pMacD);
#endif  // defined(ETH_PIC32_INT_MAC_ISR_TX)

static int              _DRV_ETHMAC_AddRxBuffers(DRV_ETHMAC_INSTANCE_DCPT* pMacD, int nBuffs, DRV_ETHMAC_ADDBUFF_FLAGS buffFlags);


typedef void (*_DRV_ETHMAC_LinkStateF)(DRV_ETHMAC_INSTANCE_DCPT* pMacD);

static void _DRV_ETHMAC_LinkStateStartLink(DRV_ETHMAC_INSTANCE_DCPT* pMacD);
static void _DRV_ETHMAC_LinkStateGetLink(DRV_ETHMAC_INSTANCE_DCPT* pMacD);
static void _DRV_ETHMAC_LinkStateWaitLinkUp(DRV_ETHMAC_INSTANCE_DCPT* pMacD);
static void _DRV_ETHMAC_LinkStateNegComplete(DRV_ETHMAC_INSTANCE_DCPT* pMacD);
static void _DRV_ETHMAC_LinkStateNegResult(DRV_ETHMAC_INSTANCE_DCPT* pMacD);


static uint16_t     _DRV_ETHMAC_GetFrmTxOk(ETH_MODULE_ID ethId);
static uint16_t     _DRV_ETHMAC_GetFrmRxOk(ETH_MODULE_ID ethId);
static uint16_t     _DRV_ETHMAC_GetFrmPktCount(ETH_MODULE_ID ethId);
static uint16_t     _DRV_ETHMAC_GetFrmRxOvflow(ETH_MODULE_ID ethId);
static uint16_t     _DRV_ETHMAC_GetFrmFcsError(ETH_MODULE_ID ethId);
static uint16_t     _DRV_ETHMAC_GetAlignError(ETH_MODULE_ID ethId);
static uint16_t     _DRV_ETHMAC_GetSCollisionCount(ETH_MODULE_ID ethId);
static uint16_t     _DRV_ETHMAC_GetMCollisionCount(ETH_MODULE_ID ethId);

static ETH_RX_FILTERS _DRV_ETHMAC_MacToEthFilter(TCPIP_MAC_RX_FILTER_TYPE macFilter);

// external function reference
extern bool SYS_INT_SourceRestore(INT_SOURCE src, int level);

/******************************************************************************
 * PIC32 MAC object implementation
 ******************************************************************************/
// the embedded PIC32 MAC object
/*static*/ const TCPIP_MAC_OBJECT DRV_ETHMAC_PIC32MACObject = 
{
    .macId = TCPIP_MODULE_MAC_PIC32INT,
    .macName = "PIC32INT",
    .TCPIP_MAC_Initialize = DRV_ETHMAC_PIC32MACInitialize,
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
    .TCPIP_MAC_Deinitialize = DRV_ETHMAC_PIC32MACDeinitialize,
    .TCPIP_MAC_Reinitialize = DRV_ETHMAC_PIC32MACReinitialize,
#else
    .TCPIP_MAC_Deinitialize = 0,
    .TCPIP_MAC_Reinitialize = 0,
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
    .TCPIP_MAC_Status = DRV_ETHMAC_PIC32MACStatus,
    .TCPIP_MAC_Tasks = DRV_ETHMAC_PIC32MACTasks,
    .TCPIP_MAC_Open = DRV_ETHMAC_PIC32MACOpen,
    .TCPIP_MAC_Close = DRV_ETHMAC_PIC32MACClose,
    .TCPIP_MAC_LinkCheck = DRV_ETHMAC_PIC32MACLinkCheck,
    .TCPIP_MAC_RxFilterHashTableEntrySet = DRV_ETHMAC_PIC32MACRxFilterHashTableEntrySet,
    .TCPIP_MAC_PowerMode = DRV_ETHMAC_PIC32MACPowerMode,
    .TCPIP_MAC_PacketTx = DRV_ETHMAC_PIC32MACPacketTx,
    .TCPIP_MAC_PacketRx = DRV_ETHMAC_PIC32MACPacketRx,
    .TCPIP_MAC_Process = DRV_ETHMAC_PIC32MACProcess,
    .TCPIP_MAC_StatisticsGet = DRV_ETHMAC_PIC32MACStatisticsGet,
    .TCPIP_MAC_ParametersGet = DRV_ETHMAC_PIC32MACParametersGet,
    .TCPIP_MAC_RegisterStatisticsGet =DRV_ETHMAC_PIC32MACRegisterStatisticsGet,
    .TCPIP_MAC_ConfigGet = DRV_ETHMAC_PIC32MACConfigGet,
    .TCPIP_MAC_EventMaskSet = DRV_ETHMAC_PIC32MACEventMaskSet,
    .TCPIP_MAC_EventAcknowledge = DRV_ETHMAC_PIC32MACEventAcknowledge,
    .TCPIP_MAC_EventPendingGet = DRV_ETHMAC_PIC32MACEventPendingGet,
};

// the embedded PIC32 MAC descriptor
// to support multiple instances
// create an array/list of MAC_DCPT structures
// or allocate dynamically
static DRV_ETHMAC_INSTANCE_DCPT _pic32_emb_mac_dcpt[1] = 
{
    {
        &DRV_ETHMAC_PIC32MACObject,
        // specific PIC32 MAC data 
        {
            0,
        }
    }
};

static const DRV_ETHMAC_HW_REG_DCPT macPIC32INTHwRegDcpt[] = 
{
    {"FRMTXOK ",    _DRV_ETHMAC_GetFrmTxOk},
    {"FRMRXOK ",    _DRV_ETHMAC_GetFrmRxOk},
    {"RXBUFCNT",    _DRV_ETHMAC_GetFrmPktCount},
    {"RXOVFLOW",    _DRV_ETHMAC_GetFrmRxOvflow},
    {"FCSERROR",    _DRV_ETHMAC_GetFrmFcsError},
    {"ALGNERR ",    _DRV_ETHMAC_GetAlignError},
    {"SCOLFRM ",    _DRV_ETHMAC_GetSCollisionCount},
    {"MCOLFRM ",    _DRV_ETHMAC_GetMCollisionCount},
};

// correspondence between a TCPIP_MAC_RX_FILTER_TYPE and a ETH_RX_FILTERS
static const ETH_RX_FILTERS _DRV_ETHMAC_FiltConvTbl[] = 
{
    ETH_FILT_BCAST_ACCEPT,          // TCPIP_MAC_RX_FILTER_TYPE_BCAST_ACCEPT
    ETH_FILT_MCAST_ACCEPT,          // TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT
    ETH_FILT_ME_UCAST_ACCEPT,       // TCPIP_MAC_RX_FILTER_TYPE_UCAST_ACCEPT
    ETH_FILT_NOTME_UCAST_ACCEPT,    // TCPIP_MAC_RX_FILTER_TYPE_UCAST_OTHER_ACCEPT
    ETH_FILT_RUNT_REJECT,           // TCPIP_MAC_RX_FILTER_TYPE_RUNT_REJECT
    ETH_FILT_RUNT_ACCEPT,           // TCPIP_MAC_RX_FILTER_TYPE_RUNT_ACCEPT
    ETH_FILT_CRC_ERR_REJECT,        // TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_REJECT
    ETH_FILT_CRC_ERR_ACCEPT,        // TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_ACCEPT
};


static const _DRV_ETHMAC_LinkStateF _DRV_ETHMAC_LinkStateTbl[] = 
{
    _DRV_ETHMAC_LinkStateStartLink,     // DRV_ETHMAC_LINK_CHECK_START_LINK
    _DRV_ETHMAC_LinkStateGetLink,       // DRV_ETHMAC_LINK_CHECK_GET_LINK
    _DRV_ETHMAC_LinkStateWaitLinkUp,       // DRV_ETHMAC_LINK_CHECK_WAIT_LINK_UP
    _DRV_ETHMAC_LinkStateNegComplete,   // DRV_ETHMAC_LINK_CHECK_NEG_COMPLETE
    _DRV_ETHMAC_LinkStateNegResult,     // DRV_ETHMAC_LINK_CHECK_NEG_RESULT
};


static __inline__ int __attribute__((always_inline)) _PIC32MacIdToIx(TCPIP_MODULE_MAC_ID macId)
{
    int macIx = macId - TCPIP_MODULE_MAC_PIC32INT_0;
    if(macIx >= 0 && macIx < sizeof(_pic32_emb_mac_dcpt)/sizeof(*_pic32_emb_mac_dcpt))
    {
        return macIx;
    }

    return -1;
}

#if defined(__PIC32MZ__)

// invalidates a range of addresses from the cache
static void _DRV_ETHMAC_CacheInvalidate(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    DRV_ETHMAC_LIB_DataLineFlush(&pMacD->mData._EnetTxFreeList);
    DRV_ETHMAC_LIB_DataLineFlush(&pMacD->mData._EnetTxBusyList);
    DRV_ETHMAC_LIB_DataLineFlush(&pMacD->mData._EnetRxFreeList);
    DRV_ETHMAC_LIB_DataLineFlush(&pMacD->mData._EnetRxBusyList);

}
#endif  // defined(__PIC32MZ__)

// RX lock functions
#if defined(DRV_ETHMAC_USE_RX_SEMAPHORE_LOCK)
static __inline__ bool __attribute__((always_inline)) _DRV_ETHMAC_RxCreate(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    return (pMacD->mData._synchF == 0) ? true : (*pMacD->mData._synchF)(&pMacD->mData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_CREATE);
}

static __inline__ void __attribute__((always_inline)) _DRV_ETHMAC_RxDelete(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    if(pMacD->mData._synchF != 0)
    {
        (*pMacD->mData._synchF)(&pMacD->mData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_DELETE);
    }
}

static __inline__ void __attribute__((always_inline)) _DRV_ETHMAC_RxLock(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    if(pMacD->mData._synchF != 0)
    {
        (*pMacD->mData._synchF)(&pMacD->mData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_LOCK);
    }
}

static __inline__ void __attribute__((always_inline)) _DRV_ETHMAC_RxUnlock(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    if(pMacD->mData._synchF != 0)
    {
        (*pMacD->mData._synchF)(&pMacD->mData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_UNLOCK);
    }
}

#else
// use critical sections
static __inline__ bool __attribute__((always_inline)) _DRV_ETHMAC_RxCreate(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    return true;
}

static __inline__ void __attribute__((always_inline)) _DRV_ETHMAC_RxDelete(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
}

static __inline__ void __attribute__((always_inline)) _DRV_ETHMAC_RxLock(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    if(pMacD->mData._synchF != 0)
    {
        (*pMacD->mData._synchF)(&pMacD->mData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_ENTER);
    }
}

static __inline__ void __attribute__((always_inline)) _DRV_ETHMAC_RxUnlock(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    if(pMacD->mData._synchF != 0)
    {
        (*pMacD->mData._synchF)(&pMacD->mData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_LEAVE);
    }
}
#endif  // defined(DRV_ETHMAC_USE_RX_SEMAPHORE_LOCK)


// TX lock functions
#if defined(DRV_ETHMAC_USE_TX_SEMAPHORE_LOCK)
static __inline__ bool __attribute__((always_inline)) _DRV_ETHMAC_TxCreate(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    return (pMacD->mData._synchF == 0) ? true : (*pMacD->mData._synchF)(&pMacD->mData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_CREATE);
}

static __inline__ void __attribute__((always_inline)) _DRV_ETHMAC_TxDelete(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    if(pMacD->mData._synchF != 0)
    {
        (*pMacD->mData._synchF)(&pMacD->mData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_DELETE);
    }
}

static __inline__ void __attribute__((always_inline)) _DRV_ETHMAC_TxLock(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    if(pMacD->mData._synchF != 0)
    {
        (*pMacD->mData._synchF)(&pMacD->mData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_LOCK);
    }
}

static __inline__ void __attribute__((always_inline)) _DRV_ETHMAC_TxUnlock(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    if(pMacD->mData._synchF != 0)
    {
        (*pMacD->mData._synchF)(&pMacD->mData._syncTxH, TCPIP_MAC_SYNCH_REQUEST_OBJ_UNLOCK);
    }
}
#else
// use critical sections
static __inline__ bool __attribute__((always_inline)) _DRV_ETHMAC_TxCreate(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    return true;
}

static __inline__ void __attribute__((always_inline)) _DRV_ETHMAC_TxDelete(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
}

static __inline__ void __attribute__((always_inline)) _DRV_ETHMAC_TxLock(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    if(pMacD->mData._synchF != 0)
    {
        (*pMacD->mData._synchF)(&pMacD->mData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_ENTER);
    }
}

static __inline__ void __attribute__((always_inline)) _DRV_ETHMAC_TxUnlock(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    if(pMacD->mData._synchF != 0)
    {
        (*pMacD->mData._synchF)(&pMacD->mData._syncRxH, TCPIP_MAC_SYNCH_REQUEST_CRIT_LEAVE);
    }
}
#endif  // defined(DRV_ETHMAC_USE_TX_SEMAPHORE_LOCK)

/*
 * interface functions
 *
*/

/****************************************************************************
 * Function:        DRV_ETHMAC_PIC32MACInitialize
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *                
 * Side Effects:    None
 *
 * Overview:        This function initializes the Eth controller, the MAC and the associated PHY.
 *                  It should be called to be able to schedule any Eth transmit or receive operation.
 *
 * Note:            Only one client per MAC supported. 
 *****************************************************************************/
SYS_MODULE_OBJ DRV_ETHMAC_PIC32MACInitialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init)
{
    union
    {
        double      align;              // alignement
        uint8_t		addr[6];            // address itself
    }alignMacAddress;        // aligned MAC address

    int		    nRxBuffs;
	DRV_ETHPHY_RESULT   phyInitRes;
	uint8_t		useFactMACAddr[6] = {0x00, 0x04, 0xa3, 0x00, 0x00, 0x00};		// to check if factory programmed MAC address needed
	uint8_t		unsetMACAddr[6] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};		// not set MAC address

    TCPIP_MAC_RES   initRes;
    int		        macIx, phyIx;
    ETH_MODULE_ID   ethId;
    ETH_RX_FILTERS ethRxFilt;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD;
    SYS_MODULE_OBJ hPhySysObject;
    DRV_HANDLE     hPhyClient;
    uint16_t        nRxDedicatedBuffers, nRxInitBuffers;
    const TCPIP_MAC_MODULE_CTRL* const macControl = ((TCPIP_MAC_INIT*)init)->macControl;
    const TCPIP_MODULE_MAC_PIC32INT_CONFIG* initData = (const TCPIP_MODULE_MAC_PIC32INT_CONFIG*)((TCPIP_MAC_INIT*)init)->moduleData;

    macIx = _PIC32MacIdToIx(index);

    if(macIx < 0 )
    {   
        return SYS_MODULE_OBJ_INVALID;      // no such type supported
    }
   
    pMacD = _pic32_emb_mac_dcpt + macIx;
    
    if(pMacD->mData._macFlags._init != 0)
    {   // already initialized 
        return (SYS_MODULE_OBJ)pMacD;
    }
    
    if(pMacD->mData._macFlags._open != 0)
    { 
        return SYS_MODULE_OBJ_INVALID;     // have a client connected
    }

    if(macControl->memH == 0)
    {
        return SYS_MODULE_OBJ_INVALID;     // not possible without dynamic memory!        
    }
    
    if(initData == 0)
    {
        return SYS_MODULE_OBJ_INVALID;     // not possible without init data!        
    }

    if(initData->pPhyBase == 0 || initData->pPhyInit == 0)
    {
        return SYS_MODULE_OBJ_INVALID;     // not possible without a PHY!        
    }

    // init the MAC object
    memset(&pMacD->mData, 0x0, sizeof(pMacD->mData)); 
    pMacD->mData.hPhyClient = DRV_HANDLE_INVALID;
    pMacD->mData.hPhySysObject = SYS_MODULE_OBJ_INVALID;
    pMacD->mData.sysStat = SYS_STATUS_UNINITIALIZED;

#if defined(__PIC32MZ__)
    _DRV_ETHMAC_CacheInvalidate(pMacD);
#endif  //  defined(__PIC32MZ__)
    pMacD->mData._macIx = macIx;
    pMacD->mData._phyIx = phyIx = macIx;    // use the same index for the associated PHY
    pMacD->mData._macFlags._linkPrev = 0;

    pMacD->mData._macIntSrc = DRV_ETHMAC_INTERRUPT_SOURCE;
	
    // use initialization data
    pMacD->mData._AllocH = macControl->memH;
    pMacD->mData._callocF = macControl->callocF;
    pMacD->mData._freeF = macControl->freeF;


    pMacD->mData.pktAllocF = macControl->pktAllocF;
    pMacD->mData.pktFreeF = macControl->pktFreeF;
    pMacD->mData.pktAckF = macControl->pktAckF;

    pMacD->mData._synchF = macControl->synchF;

    // copy the configuration data
    pMacD->mData.macConfig = *initData; 
    // adjust parameters
    if(pMacD->mData.macConfig.nTxDescriptors < ETH_PIC32_INT_MAC_MIN_TX_DESCRIPTORS)
    {
        pMacD->mData.macConfig.nTxDescriptors = ETH_PIC32_INT_MAC_MIN_TX_DESCRIPTORS;
    }
    if(pMacD->mData.macConfig.rxBuffSize < ETH_PIC32_INT_MAC_MIN_RX_SIZE)
    {
        pMacD->mData.macConfig.rxBuffSize = ETH_PIC32_INT_MAC_MIN_RX_SIZE;  
    }
    else
    {
        pMacD->mData.macConfig.rxBuffSize = ((pMacD->mData.macConfig.rxBuffSize + 15) / 16) * 16;   // adjust RX size
    }

#if (TCPIP_EMAC_RX_FRAGMENTS == 1)
    // there's only one descriptor
    pMacD->mData.rxPktDcpt[0].next = 0;
#endif  // (TCPIP_EMAC_RX_FRAGMENTS == 1)

    if(!_DRV_ETHMAC_RxCreate(pMacD))
    {
        return SYS_MODULE_OBJ_INVALID;     // failed to create synch lock
    }
    if(!_DRV_ETHMAC_TxCreate(pMacD))
    {
        _DRV_ETHMAC_RxDelete(pMacD);
        return SYS_MODULE_OBJ_INVALID;     // failed to create synch lock
    }
    
    initRes = TCPIP_MAC_RES_OK;
    ethId = pMacD->mData.macConfig.ethModuleId;

	while(1)
	{
        DRV_ETHPHY_SETUP phySetup;
        const DRV_ETHPHY_OBJECT_BASE* pPhyBase;   


		// start the initialization sequence	
        SYS_INT_SourceDisable(pMacD->mData._macIntSrc);      // stop Eth ints
		DRV_ETHMAC_LibInit(pMacD);
        SYS_INT_SourceStatusClear(pMacD->mData._macIntSrc); // clear any pending interrupt flag
        PLIB_ETH_MACSetMaxFrame(ethId, _TCPIP_EMAC_MAX_FRAME);

        pPhyBase =  pMacD->mData.macConfig.pPhyBase;

        hPhySysObject = pPhyBase->DRV_ETHPHY_Initialize(phyIx, (SYS_MODULE_INIT *)pMacD->mData.macConfig.pPhyInit);
        if ( hPhySysObject == SYS_MODULE_OBJ_INVALID )
        {
            initRes = TCPIP_MAC_RES_PHY_INIT_FAIL;
            break;
        }

        pMacD->mData.hPhySysObject = hPhySysObject;
        hPhyClient = pPhyBase->DRV_ETHPHY_Open(phyIx, DRV_IO_INTENT_READWRITE);
        if ( DRV_HANDLE_INVALID == hPhyClient )
        {
            initRes = TCPIP_MAC_RES_PHY_INIT_FAIL;
            break;
        }

        pMacD->mData.hPhyClient = hPhyClient;

        phySetup.phyAddress = pMacD->mData.macConfig.pPhyInit->phyAddress;
        phySetup.openFlags = pMacD->mData.macConfig.ethFlags; 
        phySetup.configFlags = pMacD->mData.macConfig.pPhyInit->phyFlags;
        phySetup.macPauseType = DRV_ETHMAC_PAUSE_CPBL_MASK;
        phySetup.resetFunction = pMacD->mData.macConfig.pPhyInit->resetFunction;

        phyInitRes = pPhyBase->DRV_ETHPHY_Setup(hPhyClient, &phySetup, &pMacD->mData._linkResFlags);
        if(phyInitRes < 0)
        {   // some error occurred
            initRes = TCPIP_MAC_RES_PHY_INIT_FAIL;
            break;
        }

        // OK, we can continue initalization
		// let the PHY initialization and auto-negotiation (if any) take place
		// and continue the initialization

        // Set the RX filters
        PLIB_ETH_RxFiltersClr(ethId, ETH_FILT_ALL_FILTERS);
        ethRxFilt = _DRV_ETHMAC_MacToEthFilter(TCPIP_EMAC_RX_FILTERS);
        PLIB_ETH_RxFiltersSet(ethId, ethRxFilt);

#if (TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE) 
        // sanity check for water marks
        uint8_t aaFullWMPkts = (uint8_t)TCPIP_EMAC_FLOW_CONTROL_FULL_WMARK;
        uint8_t aaEmptyWMPkts = (uint8_t)TCPIP_EMAC_FLOW_CONTROL_EMPTY_WMARK;

        if(aaFullWMPkts == 0)
        {
            aaFullWMPkts++;
        }
        if(aaEmptyWMPkts >= aaFullWMPkts)
        {
            aaEmptyWMPkts = aaFullWMPkts - 1;
        }

        // enable the AutoFC
        uint32_t ptValue = (TCPIP_EMAC_FLOW_CONTROL_PAUSE_BYTES + _TCPIP_EMAC_QUANTA_PAUSE_BYTES - 1) / _TCPIP_EMAC_QUANTA_PAUSE_BYTES;
        PLIB_ETH_PauseTimerSet(ethId, (uint16_t)ptValue);

        PLIB_ETH_RxFullWmarkSet(ethId, aaFullWMPkts);
        PLIB_ETH_RxEmptyWmarkSet(ethId, aaEmptyWMPkts);

        PLIB_ETH_TxPauseEnable(ethId);
        PLIB_ETH_RxPauseEnable(ethId);
        PLIB_ETH_AutoFlowControlEnable(ethId);
#endif  // (TCPIP_EMAC_AUTO_FLOW_CONTROL_ENABLE) 
        
		// set the MAC address
        memcpy(alignMacAddress.addr, macControl->ifPhyAddress.v, sizeof(alignMacAddress.addr));
        if(memcmp(alignMacAddress.addr, useFactMACAddr, sizeof(useFactMACAddr)) !=0 && memcmp(alignMacAddress.addr, unsetMACAddr, sizeof(unsetMACAddr)) !=0 )
        {   // use the supplied address
            PLIB_ETH_MACSetAddress(ethId, alignMacAddress.addr);
        }
        // else use the factory programmed address existent in the MAC
				
		if(DRV_ETHMAC_LibDescriptorsPoolAdd(pMacD, pMacD->mData.macConfig.nTxDescriptors, DRV_ETHMAC_DCPT_TYPE_TX, _MacCallocCallback, (void*)pMacD)!=pMacD->mData.macConfig.nTxDescriptors)
		{
            initRes = TCPIP_MAC_RES_ALLOC_ERR;
            break;
		}

		if(DRV_ETHMAC_LibDescriptorsPoolAdd(pMacD, pMacD->mData.macConfig.nRxDescriptors, DRV_ETHMAC_DCPT_TYPE_RX, _MacCallocCallback, (void*)pMacD)!=pMacD->mData.macConfig.nRxDescriptors)
		{
            initRes = TCPIP_MAC_RES_ALLOC_ERR;
            break;
		}

        // add RX dedicated buffers
        if((nRxDedicatedBuffers = pMacD->mData.macConfig.nRxDedicatedBuffers))
        {
            nRxBuffs = _DRV_ETHMAC_AddRxBuffers(pMacD, nRxDedicatedBuffers, DRV_ETHMAC_ADDBUFF_FLAG_STICKY | DRV_ETHMAC_ADDBUFF_FLAG_SETSIZE);
            if(nRxBuffs != nRxDedicatedBuffers)
            {   // failed to allocate all buffers
                initRes = TCPIP_MAC_RES_ALLOC_ERR;
                break;
            }
        }
        // add RX non-dedicated buffers
        if((nRxInitBuffers = pMacD->mData.macConfig.nRxInitBuffers))
        {
            nRxBuffs = _DRV_ETHMAC_AddRxBuffers(pMacD, nRxInitBuffers, nRxDedicatedBuffers ? DRV_ETHMAC_ADDBUFF_FLAG_NONE : DRV_ETHMAC_ADDBUFF_FLAG_SETSIZE);
            if(nRxBuffs != nRxInitBuffers)
            {   // failed to allocate all buffers
                initRes = TCPIP_MAC_RES_ALLOC_ERR;
                break;
            }
        }

        if(DRV_ETHMAC_PIC32MACEventInit((DRV_HANDLE)pMacD, macControl->eventF, macControl->eventParam) != TCPIP_MAC_RES_OK)
        {
            initRes = TCPIP_MAC_RES_EVENT_INIT_FAIL;
            break;
        }
        // end of initialization        
		break;
	}



    if(initRes != TCPIP_MAC_RES_OK)
    {   // failed; release what we've allocated
        _MACDeinit(pMacD);        
        return SYS_MODULE_OBJ_INVALID;
    }


    // success
    // remaining initialization is done by DRV_ETHMAC_PIC32MACTasks
    
    pMacD->mData._macFlags._init = 1;
    pMacD->mData.sysStat = SYS_STATUS_BUSY;
	return (SYS_MODULE_OBJ)pMacD;
}

/****************************************************************************
 * Function:        DRV_ETHMAC_PIC32MACDeinitialize
 *
 * PreCondition:    None
 *
 * Input:           stackData - standard stack initialization structure
 *
 * Output:          TCPIP_MAC_RES
 *                
 * Side Effects:    None
 *
 * Overview:        This function de-initializes the Eth MAC controller.
 *
 * Note:            None 
 *****************************************************************************/
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
void DRV_ETHMAC_PIC32MACDeinitialize(SYS_MODULE_OBJ object)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)object;

    if(pMacD->mData._macFlags._init != 0)
    {
        _MACDeinit(pMacD);
    }
}

void DRV_ETHMAC_PIC32MACReinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init)
{
    // not supported
}
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

SYS_STATUS DRV_ETHMAC_PIC32MACStatus (SYS_MODULE_OBJ object)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)object;

    if(pMacD->mData._macFlags._init != 0)
    {
        return pMacD->mData.sysStat;
    }

    return SYS_STATUS_ERROR;
}

void DRV_ETHMAC_PIC32MACTasks(SYS_MODULE_OBJ object)
{
    TCPIP_ETH_PAUSE_TYPE        pauseType;
    DRV_ETHPHY_CLIENT_STATUS    phyStat;
    DRV_HANDLE                  hPhyClient;
	DRV_ETHPHY_RESULT           phyInitRes;
    const DRV_ETHPHY_OBJECT_BASE* pPhyBase;   
    DRV_ETHMAC_INSTANCE_DCPT*   pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)object;

    if(pMacD->mData._macFlags._init == 0)
    {   // nothing to do
        return;
    }

    pPhyBase =  pMacD->mData.macConfig.pPhyBase;
    pPhyBase->DRV_ETHPHY_Tasks(pMacD->mData.hPhySysObject);
    
    switch(pMacD->mData.sysStat)
    {
        case SYS_STATUS_BUSY:
            // perform the PHY initialization

            hPhyClient = pMacD->mData.hPhyClient;
            phyStat = pPhyBase->DRV_ETHPHY_ClientStatus(hPhyClient);
            if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
            {   // wait some more
                return;
            }

            phyInitRes = pPhyBase->DRV_ETHPHY_ClientOperationResult(hPhyClient);

            if ( phyInitRes != DRV_ETHPHY_RES_OK)
            {
                _MACDeinit(pMacD);
                // keep the error status though
                pMacD->mData._macFlags._init = 1;
                pMacD->mData.sysStat = SYS_STATUS_ERROR; 
                SYS_ERROR_PRINT(SYS_ERROR_ERROR, "DRV PHY init failed: %d\r\n", phyInitRes);
                break;
            }

            // PHY was detected properly
            pMacD->mData._macFlags._linkPresent = 1;
            if((pMacD->mData.macConfig.ethFlags & TCPIP_ETH_OPEN_AUTO) != 0)
            {	// we'll just wait for the negotiation to be done
                pMacD->mData._macFlags._linkNegotiation = 1;	// performing the negotiation
            }
            else
            {	// no need of negotiation results; just update the MAC
                pauseType = (pMacD->mData.macConfig.ethFlags & TCPIP_ETH_OPEN_FDUPLEX) ? DRV_ETHMAC_PAUSE_CPBL_MASK : TCPIP_ETH_PAUSE_TYPE_NONE;
                DRV_ETHMAC_LibMACOpen(pMacD, pMacD->mData._linkResFlags, pauseType);
            }

            pMacD->mData._linkUpTick = SYS_TMR_TickCountGet();     // the initialization time
            pMacD->mData._linkWaitTick = ((SYS_TMR_TickCounterFrequencyGet() * pMacD->mData.macConfig.linkInitDelay) + 999 ) / 1000;
            pMacD->mData._linkCheckState = DRV_ETHMAC_LINK_CHECK_START_LINK;

            pMacD->mData.sysStat = SYS_STATUS_READY; 
            break; 

        default:    // SYS_STATUS_READY
            break;
    }
}


size_t DRV_ETHMAC_PIC32MACConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;

    if(pConfigSize)
    {
        *pConfigSize =  sizeof(TCPIP_MODULE_MAC_PIC32INT_CONFIG);
    }

    if(configBuff && buffSize >= sizeof(TCPIP_MODULE_MAC_PIC32INT_CONFIG))
    {   // can copy the data
        TCPIP_MODULE_MAC_PIC32INT_CONFIG* pMacConfig = (TCPIP_MODULE_MAC_PIC32INT_CONFIG*)configBuff;

        *pMacConfig = pMacD->mData.macConfig;
        return sizeof(TCPIP_MODULE_MAC_PIC32INT_CONFIG);
    }

    return 0;

}

DRV_HANDLE DRV_ETHMAC_PIC32MACOpen(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent)
{
    int                 macIx;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD;
    DRV_HANDLE    hMac = DRV_HANDLE_INVALID;

    macIx = _PIC32MacIdToIx(drvIndex);

    if(macIx >= 0 )
    {   
        pMacD = _pic32_emb_mac_dcpt + macIx;
        if(pMacD->mData._macFlags._init == 1)
        {
            if(pMacD->mData._macFlags._open == 0)
            {   // only one client for now
                pMacD->mData._macFlags._open = 1;
                hMac = (DRV_HANDLE)pMacD;
            }
#if (DRV_ETHMAC_CLIENTS_NUMBER > 1)
            else
            {   // allow multiple clients
                hMac = (DRV_HANDLE)pMacD;
            }
#endif
        }
    }

    return hMac;
}


void DRV_ETHMAC_PIC32MACClose( DRV_HANDLE hMac )
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD;

    pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;

    if(pMacD->mData._macFlags._init == 1)
    {
        pMacD->mData._macFlags._open = 0;
    }
}

/**************************
 * TX functions
 ***********************************************/

TCPIP_MAC_RES DRV_ETHMAC_PIC32MACPacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket)
{
    TCPIP_MAC_RES       macRes;
    TCPIP_MAC_PACKET*   pPkt;
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    uint16_t*           pHdrSpace;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;

    _DRV_ETHMAC_TxLock(pMacD);

    _MACTxAcknowledgeEth(pMacD);

#if defined(ETH_PIC32_INT_MAC_ISR_TX)
    _MACTxAcknowledgeAckQueue(pMacD);
#endif  // defined(ETH_PIC32_INT_MAC_ISR_TX)

    // transmit the pending packets...don't transmit out of order
    macRes = _MacTxPendingPackets(pMacD);

    pPkt = ptrPacket;

    // check that packets are properly formatted and
    // have room to store the packet info
    while(pPkt)
    {
        pSeg = pPkt->pDSeg;
        if(pSeg == 0 || pSeg->segLoadOffset < 2)
        {   // cannot send this packet
            _DRV_ETHMAC_TxUnlock(pMacD);
            return TCPIP_MAC_RES_PACKET_ERR;
        }

        // store packet info
        pHdrSpace = (uint16_t*)pSeg->segLoad - 1;
        *pHdrSpace = (uint8_t*)pSeg->segLoad - (uint8_t*)pPkt;

        pPkt = pPkt->next;
    }

    while(ptrPacket && macRes == TCPIP_MAC_RES_OK)
    {   // can schedule some packets
        // set the queue flag; avoid race condition if MACTx is really fast;
        ptrPacket->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
        macRes = _MACTxPacket(pMacD, ptrPacket);
        
        if(macRes == TCPIP_MAC_RES_PACKET_ERR)
        {   // no longer in our queue
            ptrPacket->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
            _DRV_ETHMAC_TxUnlock(pMacD);
            return TCPIP_MAC_RES_PACKET_ERR;
        }

        if(macRes == TCPIP_MAC_RES_PENDING)
        {   // no more room into the hw queue
            break;
        }
        ptrPacket = ptrPacket->next;
    }

    // queue what's left
    while(ptrPacket)
    {
        DRV_ETHMAC_SingleListTailAdd(&pMacD->mData._TxQueue, (DRV_ETHMAC_SGL_LIST_NODE*)ptrPacket);
        ptrPacket->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
        ptrPacket = ptrPacket->next;
    }

    _DRV_ETHMAC_TxUnlock(pMacD);

    return TCPIP_MAC_RES_OK;
}

/**************************
 * RX functions
 ***********************************************/

// returns a pending RX packet if exists
TCPIP_MAC_PACKET* DRV_ETHMAC_PIC32MACPacketRx (DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, const TCPIP_MAC_PACKET_RX_STAT** ppPktStat)
{
	DRV_ETHMAC_RESULT			        ethRes;
    DRV_ETHMAC_PKT_DCPT            *pRootDcpt, *pLastDcpt;
    TCPIP_MAC_RES           mRes;
    uint16_t                buffOffset;
#if (TCPIP_EMAC_RX_FRAGMENTS > 1)
    TCPIP_MAC_PACKET       *pCurrPkt, *pPrevPkt;
    TCPIP_MAC_DATA_SEGMENT *pCurrDSeg;
    DRV_ETHMAC_PKT_DCPT            *pCurrDcpt;
#endif  // (TCPIP_EMAC_RX_FRAGMENTS > 1)
    TCPIP_MAC_PACKET*       pRxPkt = 0;
	const DRV_ETHMAC_PKT_STAT_RX* pRxPktStat = 0;
    int                     buffsPerRxPkt = 0;
    DRV_ETHMAC_INSTANCE_DCPT     *pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;


    // prepare the receive packet descriptor
    // for receiving a new packet
    pLastDcpt = pRootDcpt = pMacD->mData.rxPktDcpt;
#if (TCPIP_EMAC_RX_FRAGMENTS > 1)
    int ix;
    for(ix = 0; ix < sizeof(pMacD->mData.rxPktDcpt)/sizeof(*pMacD->mData.rxPktDcpt); ix++)
    {
        pLastDcpt->next = pLastDcpt + 1;
        pLastDcpt++;
    }
    // adjust last descriptor
    pLastDcpt--;
    pLastDcpt->next = 0;
#endif  // (TCPIP_EMAC_RX_FRAGMENTS > 1)

    _DRV_ETHMAC_RxLock(pMacD);
    ethRes = DRV_ETHMAC_LibRxGetPacket (pMacD, pRootDcpt, &buffsPerRxPkt, &pRxPktStat );
    _DRV_ETHMAC_RxUnlock(pMacD);

    if(ethRes == DRV_ETHMAC_RES_PACKET_QUEUED || ethRes == DRV_ETHMAC_RES_NO_PACKET)
    {   // done, no more packets
        mRes = TCPIP_MAC_RES_PENDING;
    }
    else if(ethRes == DRV_ETHMAC_RES_OK)
    {   // available packet; minimum check
        if(pRxPktStat->rxOk == 0 || pRxPktStat->runtPkt != 0 || pRxPktStat->crcError != 0)
        {   // corrupted packet; discrd/re-insert it
            mRes = TCPIP_MAC_RES_PACKET_ERR;
            pMacD->mData._rxStat.nRxErrorPackets++;
        }
        else
        {
            mRes = TCPIP_MAC_RES_OK;
            pMacD->mData._rxStat.nRxOkPackets++;
        }
    }
    else
    {   // DRV_ETHMAC_RES_RX_PKT_SPLIT_ERR; too many fragments 
        mRes = TCPIP_MAC_RES_FRAGMENT_ERR;
        pMacD->mData._rxStat.nRxFragmentErrors++;
    }

    if(pRes)
    {
        *pRes = mRes;
    }
    if(ppPktStat)
    {
        *ppPktStat = 0; 
    }

    if(mRes == TCPIP_MAC_RES_OK)
    {   // valid ETH packet;
        // format properly and return the packet pointer
        // restore packet the buffer belongs to        
#if (TCPIP_EMAC_RX_FRAGMENTS > 1)
        for(pPrevPkt = 0, pCurrDcpt = pLastDcpt = pRootDcpt; pCurrDcpt != 0 && pCurrDcpt->pBuff != 0; pCurrDcpt = pCurrDcpt->next)
        {
            buffOffset = *((uint16_t*)pCurrDcpt->pBuff - 1);
            pCurrPkt = (TCPIP_MAC_PACKET*)((uint8_t*)pCurrDcpt->pBuff - buffOffset);
            pCurrDSeg = pCurrPkt->pDSeg;
            pCurrDSeg->segLen = pCurrDcpt->nBytes;
            pCurrDSeg->next = 0;
            if(pPrevPkt == 0)
            {
                pRxPkt = pCurrPkt;
            }
            else
            {
                pPrevPkt->pDSeg->next = pCurrDSeg;
            }
            pPrevPkt = pCurrPkt;
            pLastDcpt = pCurrDcpt;
        }
        // adjust the last segment for FCS size
        pPrevPkt->pDSeg->segLen -= 4;
        pLastDcpt->nBytes -= 4;

        if(buffsPerRxPkt > 1)
        {
            pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_SPLIT;
        } 
#else
        buffOffset = *((uint16_t*)pRootDcpt->pBuff - 1);
        pRxPkt = (TCPIP_MAC_PACKET*)((uint8_t*)pRootDcpt->pBuff - buffOffset);
        pRxPkt->pDSeg->next = 0;
        // adjust the last segment for FCS size
        pLastDcpt->nBytes -= 4;
#endif  // (TCPIP_EMAC_RX_FRAGMENTS > 1)

        // no maintenance of the total packet length
        // could be obtained through ppPktStat 
        pRxPkt->pDSeg->segLen = pRootDcpt->nBytes - sizeof(TCPIP_MAC_ETHERNET_HEADER);
        // Note: re-set pMacLayer and pNetLayer; IPv6 changes these pointers inside the packet!
        pRxPkt->pMacLayer = pRxPkt->pDSeg->segLoad;
        pRxPkt->pNetLayer = pRxPkt->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);

        pRxPkt->tStamp = SYS_TMR_TickCountGet();
        pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;



        pRxPkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_CAST_MASK;
        if(pRxPktStat->bMatch)
        {
            pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_BCAST;
        }
        else if(pRxPktStat->mMatch)
        {
            pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_MCAST;
        }
        else
        {
            pRxPkt->pktFlags |= TCPIP_MAC_PKT_FLAG_UNICAST;
        }

        if(ppPktStat)
        {
            *ppPktStat = (const TCPIP_MAC_PACKET_RX_STAT*)pRxPktStat; 
        }
        // success
        return pRxPkt;
    }

    // failure; acknowledge the packet and re-insert
    if(pRootDcpt->pBuff)
    {   // we have a failed packet available
        _DRV_ETHMAC_RxLock(pMacD);
        DRV_ETHMAC_LibRxAcknowledgePacket(pMacD, pRootDcpt, 0, 0);
        _DRV_ETHMAC_RxUnlock(pMacD);
    }

    return 0;

}


// verify the link status

bool DRV_ETHMAC_PIC32MACLinkCheck(DRV_HANDLE hMac)
{

    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;

    if(pMacD->mData._macFlags._linkPresent == 0)
    {
        return false;
    }


    (*_DRV_ETHMAC_LinkStateTbl[pMacD->mData._linkCheckState])(pMacD);

    return pMacD->mData._macFlags._linkPrev == 1;
}

static void _DRV_ETHMAC_LinkStateDown(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    // link is down; reset state flags, initialization time 
    pMacD->mData._macFlags._linkPrev = 0;
    pMacD->mData._macFlags._linkUpDone = 0;
    pMacD->mData._linkUpTick = SYS_TMR_TickCountGet();
    pMacD->mData._linkCheckState = DRV_ETHMAC_LINK_CHECK_START_LINK;
}

static void _DRV_ETHMAC_LinkStateStartLink(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    DRV_ETHPHY_RESULT phyRes;
    
    
    phyRes = pMacD->mData.macConfig.pPhyBase->DRV_ETHPHY_LinkStatusGet(pMacD->mData.hPhyClient, &pMacD->mData._negResult.linkStatus, false);
    if(phyRes < 0)
    {   // some error occurred
        _DRV_ETHMAC_LinkStateDown(pMacD);
        return;
    }

    pMacD->mData._linkCheckState = DRV_ETHMAC_LINK_CHECK_GET_LINK;

}

static void _DRV_ETHMAC_LinkStateGetLink(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    DRV_ETHPHY_CLIENT_STATUS phyStat;
    DRV_ETHPHY_RESULT phyRes;
    const DRV_ETHPHY_OBJECT_BASE* pPhyBase;   
    bool linkCurrUp;

    
    pPhyBase =  pMacD->mData.macConfig.pPhyBase;
    phyStat = pPhyBase->DRV_ETHPHY_ClientStatus( pMacD->mData.hPhyClient );
    if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
    {   // not done yet
        return;
    }

    phyRes = pPhyBase->DRV_ETHPHY_ClientOperationResult(pMacD->mData.hPhyClient);
    if(phyRes != DRV_ETHPHY_RES_OK)
    {   // some error occurred
        _DRV_ETHMAC_LinkStateDown(pMacD);
        return;
    }

    linkCurrUp = (pMacD->mData._negResult.linkStatus & DRV_ETHPHY_LINK_ST_UP) != 0;

    if(linkCurrUp == false)
    {   
        _DRV_ETHMAC_LinkStateDown(pMacD);
        return;
    }

    // link up
    if(pMacD->mData._macFlags._linkPrev == linkCurrUp)
    {   // PHY state not changed 
        pMacD->mData._linkCheckState = DRV_ETHMAC_LINK_CHECK_START_LINK;
    }
    else
    {
        pMacD->mData._linkCheckState = DRV_ETHMAC_LINK_CHECK_WAIT_LINK_UP;
    }
}

// ok, the link is coming up
static void _DRV_ETHMAC_LinkStateWaitLinkUp(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    DRV_ETHPHY_RESULT   phyRes;

    if(pMacD->mData._macFlags._linkUpDone == 0)
    {   // wait the init tmo before checking the link up
        uint32_t currTick = SYS_TMR_TickCountGet();
        if(currTick - pMacD->mData._linkUpTick < pMacD->mData._linkWaitTick)
        {
            return;
        }
        pMacD->mData._macFlags._linkUpDone = 1;
    }

    // initialization delay passed
    if(pMacD->mData._macFlags._linkNegotiation == 0)
    {   // no auto negotiation taking place; this is the link status
        pMacD->mData._macFlags._linkPrev = true;
        pMacD->mData._linkCheckState = DRV_ETHMAC_LINK_CHECK_START_LINK;
        return;
    }

    // auto negotiation is turned on
    // we have to re-negotiate but the link is actually up only
    // after the re-negotiation is done
    // wait for negotiation complete
    phyRes = pMacD->mData.macConfig.pPhyBase->DRV_ETHPHY_NegotiationIsComplete(pMacD->mData.hPhyClient, false);
    if(phyRes < 0)
    {   // some error occurred
        _DRV_ETHMAC_LinkStateDown(pMacD);
    }
    else
    {   // advance
        pMacD->mData._linkCheckState = DRV_ETHMAC_LINK_CHECK_NEG_COMPLETE;
    }
}

static void _DRV_ETHMAC_LinkStateNegComplete(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    DRV_ETHPHY_CLIENT_STATUS phyStat;
    DRV_ETHPHY_RESULT phyRes;
    const DRV_ETHPHY_OBJECT_BASE* pPhyBase;   

    pPhyBase =  pMacD->mData.macConfig.pPhyBase;
    phyStat = pPhyBase->DRV_ETHPHY_ClientStatus( pMacD->mData.hPhyClient );
    if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
    {   // not done yet
        return;
    }

    phyRes = pPhyBase->DRV_ETHPHY_ClientOperationResult(pMacD->mData.hPhyClient);
    if(phyRes != DRV_ETHPHY_RES_OK)
    {   // some error occurred
        _DRV_ETHMAC_LinkStateDown(pMacD);
        return;
    }

    phyRes = pPhyBase->DRV_ETHPHY_NegotiationResultGet(pMacD->mData.hPhyClient, &pMacD->mData._negResult);

    if(phyRes < 0)
    {   // some error occurred
        _DRV_ETHMAC_LinkStateDown(pMacD);
    }
    else
    {   // advance; get negotiation result
        pMacD->mData._linkCheckState = DRV_ETHMAC_LINK_CHECK_NEG_RESULT;
    }
}


static void _DRV_ETHMAC_LinkStateNegResult(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    DRV_ETHPHY_CLIENT_STATUS phyStat;
    DRV_ETHPHY_RESULT phyRes;
    const DRV_ETHPHY_OBJECT_BASE* pPhyBase;   
    TCPIP_ETH_PAUSE_TYPE        pauseType;
    DRV_ETHPHY_CONFIG_FLAGS phyCfgFlags = 0;

    pPhyBase =  pMacD->mData.macConfig.pPhyBase;
    phyStat = pPhyBase->DRV_ETHPHY_ClientStatus( pMacD->mData.hPhyClient );
    if(phyStat == DRV_ETHPHY_CLIENT_STATUS_BUSY)
    {   // not done yet
        return;
    }

    phyRes = pPhyBase->DRV_ETHPHY_ClientOperationResult(pMacD->mData.hPhyClient);
    if(phyRes != DRV_ETHPHY_RES_OK)
    {   // some error occurred
        _DRV_ETHMAC_LinkStateDown(pMacD);
        return;
    }

    
    if( (pMacD->mData._negResult.linkStatus & DRV_ETHPHY_LINK_ST_UP) != 0 )
    {   // negotiation succeeded; properly update the MAC
        pPhyBase->DRV_ETHPHY_HWConfigFlagsGet(pMacD->mData.hPhyClient, &phyCfgFlags);
        pMacD->mData._negResult.linkFlags |= (phyCfgFlags & DRV_ETHPHY_CFG_RMII) ? TCPIP_ETH_OPEN_RMII : TCPIP_ETH_OPEN_MII ;
        pauseType = pMacD->mData._negResult.pauseType;
        DRV_ETHMAC_LibMACOpen(pMacD, pMacD->mData._negResult.linkFlags, pauseType);

        pMacD->mData._macFlags._linkPrev = 1;
        pMacD->mData._linkCheckState = DRV_ETHMAC_LINK_CHECK_START_LINK;
    }
    else
    {
        _DRV_ETHMAC_LinkStateDown(pMacD);
    }
}

/**************************
 * Filter functions
 ***********************************************/
TCPIP_MAC_RES DRV_ETHMAC_PIC32MACRxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr)
{
      volatile unsigned int*    pHTSet;
      uint8_t                   hVal;
      int                       i, j;
      TCPIP_MAC_ADDR            destAddr;
      uint8_t                   nullMACAddr[6] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
      ETH_MODULE_ID ethId = ((DRV_ETHMAC_INSTANCE_DCPT*)hMac)->mData.macConfig.ethModuleId;

      typedef union
      {
          uint8_t Val;
          struct __attribute__((packed))
          {
              uint8_t b0:1;
              uint8_t b1:1;
              uint8_t b2:1;
              uint8_t b3:1;
              uint8_t b4:1;
              uint8_t b5:1;
              uint8_t b6:1;
              uint8_t b7:1;
          } bits;
      }crc_byte;

      union
      {
          uint32_t Val;
          crc_byte v[4];
          struct __attribute__((packed))
          {
              uint32_t b0_23: 24;

              uint32_t b24:1;
              uint32_t b25:1;
              uint32_t b26:1;
              uint32_t b27:1;
              uint32_t b28:1;
              uint32_t b29:1;
              uint32_t b30:1;
              uint32_t b31:1;
          } bits;
      } crc32_val;

      crc_byte    macByte;

      crc32_val.Val = 0xFFFFFFFF;



      // Clear the Hash Table bits and disable the Hash Table Filter if a special 
      // 00-00-00-00-00-00 destination MAC address is provided.
      if( DestMACAddr == 0 || memcmp(DestMACAddr->v, nullMACAddr, sizeof(nullMACAddr))==0 )
      {
          // Disable the Hash Table receive filter and clear the hash table
          PLIB_ETH_RxFiltersClr(ethId, ETH_FILT_HTBL_ACCEPT);
          PLIB_ETH_RxFiltersHTSet(ethId, 0ull);
          return TCPIP_MAC_RES_OK;
      }

 
      // Calculate a CRC-32 over the 6 byte MAC address 
      // using polynomial 0x4C11DB7
      memcpy(destAddr.v, DestMACAddr->v, sizeof(destAddr));
      for(i = 0; i < sizeof(TCPIP_MAC_ADDR); i++)
      {
            uint8_t  crcnext;
      
            // shift in 8 bits
            for(j = 0; j < 8; j++)
            {
                  crcnext = 0;
                  if( crc32_val.v[3].bits.b7)
                  {
                        crcnext = 1;
                  }
                  macByte.Val = destAddr.v[i];
                  crcnext ^= macByte.bits.b0;
      
                  crc32_val.Val <<= 1;
                  if(crcnext)
                  {
                        crc32_val.Val ^= 0x4C11DB7;
                  }
                  // next bit
                  destAddr.v[i] >>= 1;
            }
      }
      
      // CRC-32 calculated, now extract bits 28:23
      // Bit 28 defines what HT register is affected: ETHHT0 or ETHHT1
      // Bits 27:23 define the bit offset within the ETHHT register
      pHTSet = (crc32_val.bits.b28)? &ETHHT1SET : &ETHHT0SET;
      hVal = (crc32_val.Val >> 23)&0x1f;
      *pHTSet = 1 << hVal;
      
      // Enable that the Hash Table receive filter
      PLIB_ETH_RxFiltersSet(ethId, ETH_FILT_HTBL_ACCEPT);

      return TCPIP_MAC_RES_OK;
      
}

bool DRV_ETHMAC_PIC32MACPowerMode(DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode)
{   // not supported
    return true;
}


// adds buffers to the internal hardware RX queue.
// returns the number of RX allocated/scheduled buffers
static int _DRV_ETHMAC_AddRxBuffers(DRV_ETHMAC_INSTANCE_DCPT* pMacD, int nBuffs, DRV_ETHMAC_ADDBUFF_FLAGS buffFlags)
{
    int ix;
    TCPIP_MAC_PACKET*   pRxPkt;
    TCPIP_MAC_DATA_SEGMENT* pRSeg;
    uint16_t* pHdrSpace;
    DRV_ETHMAC_RESULT ethRes;
    bool    stickyBuff, setRxSize, synchLock;


    stickyBuff = (buffFlags & DRV_ETHMAC_ADDBUFF_FLAG_STICKY) != 0; 
    setRxSize = (buffFlags & DRV_ETHMAC_ADDBUFF_FLAG_SETSIZE) != 0; 
    synchLock = (buffFlags & DRV_ETHMAC_ADDBUFF_FLAG_RX_LOCK) != 0; 

    // allocate the RX buffers
    pRxPkt = 0;
    for(ix=0; ix < nBuffs; ix++)
    {   // the rxBuffSize is viewed as total packet size, including the ETH frame
        // the ETH frame header is added by the packet allocation
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
        pRxPkt = (*(TCPIP_MAC_PKT_AllocFDbg)pMacD->mData.pktAllocF)(sizeof(*pRxPkt), pMacD->mData.macConfig.rxBuffSize - sizeof(TCPIP_MAC_ETHERNET_HEADER), 0, TCPIP_THIS_MODULE_ID);
#else
        pRxPkt = (*pMacD->mData.pktAllocF)(sizeof(*pRxPkt), pMacD->mData.macConfig.rxBuffSize - sizeof(TCPIP_MAC_ETHERNET_HEADER), 0);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)

        if(pRxPkt == 0)
        {   // failed
            break;
        }
        // save packet info
        pRSeg = pRxPkt->pDSeg;
        if(stickyBuff)
        {
            pRSeg->segFlags |= TCPIP_MAC_SEG_FLAG_RX_STICKY;
        }
        pHdrSpace = (uint16_t*)pRxPkt->pDSeg->segLoad - 1;
        *pHdrSpace = (uint8_t*)pRSeg->segLoad - (uint8_t*)pRxPkt;

        // set the packet acknowledgement
        pRxPkt->ackFunc = _MacRxPacketAck;
        pRxPkt->ackParam = pMacD;

        if(setRxSize)
        {
            pMacD->mData.macConfig.rxBuffSize = (pRxPkt->pDSeg->segSize / 16) * 16;   // set actual RX size
            PLIB_ETH_RxSetBufferSize(pMacD->mData.macConfig.ethModuleId, pMacD->mData.macConfig.rxBuffSize);
            setRxSize = false;  // just once
        }

        pRxPkt->next = 0;
        if(synchLock)
        {
            _DRV_ETHMAC_RxLock(pMacD);
        }
        ethRes = DRV_ETHMAC_LibRxBuffersAppend(pMacD, (void*)&pRxPkt->pDSeg->segLoad, 1, stickyBuff ? DRV_ETHMAC_BUFF_FLAG_RX_STICKY : 0);
        if(synchLock)
        {
            _DRV_ETHMAC_RxUnlock(pMacD);
        }
        if(ethRes != DRV_ETHMAC_RES_OK)
        {   // failed; free
            // return it to the packet pool
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
            (*(TCPIP_MAC_PKT_FreeFDbg)pMacD->mData.pktFreeF)(pRxPkt, TCPIP_THIS_MODULE_ID);
#else
            (*pMacD->mData.pktFreeF)(pRxPkt);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)

            break;
        }




    }


    return ix;
}



TCPIP_MAC_RES DRV_ETHMAC_PIC32MACProcess(DRV_HANDLE hMac)
{
    int rxLowThreshold;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;

    _DRV_ETHMAC_TxLock(pMacD);
    _MACTxAcknowledgeEth(pMacD);

#if defined(ETH_PIC32_INT_MAC_ISR_TX)
    _MACTxAcknowledgeAckQueue(pMacD);
#endif  // defined(ETH_PIC32_INT_MAC_ISR_TX)


    _MacTxPendingPackets(pMacD);
    _DRV_ETHMAC_TxUnlock(pMacD);

    // replenish RX buffers
    if((rxLowThreshold = pMacD->mData.macConfig.rxLowThreshold) != 0)
    {
        int nRxScheduled = 0; // buffers currently RX scheduled
        int rxLowFill;      // new buffers to add

        _DRV_ETHMAC_RxLock(pMacD);
        DRV_ETHMAC_LibRxScheduledBuffersGet(pMacD, &nRxScheduled);
        _DRV_ETHMAC_RxUnlock(pMacD);

        if(nRxScheduled <= rxLowThreshold)
        {
            if((rxLowFill = pMacD->mData.macConfig.rxLowFill) == 0)
            {   // gap fill
                rxLowFill = rxLowThreshold - nRxScheduled;
            }

            if(rxLowFill)
            {
                _DRV_ETHMAC_AddRxBuffers(pMacD, rxLowFill, DRV_ETHMAC_ADDBUFF_FLAG_RX_LOCK);
            }
        }
    }

    return TCPIP_MAC_RES_OK;
}

TCPIP_MAC_RES DRV_ETHMAC_PIC32MACStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;

    if(pRxStatistics)
    {
        _DRV_ETHMAC_RxLock(pMacD);
        DRV_ETHMAC_LibRxPendingBuffersGet(pMacD, &pMacD->mData._rxStat.nRxPendBuffers);
        DRV_ETHMAC_LibRxScheduledBuffersGet(pMacD, &pMacD->mData._rxStat.nRxSchedBuffers);
        _DRV_ETHMAC_RxUnlock(pMacD);
        *pRxStatistics = pMacD->mData._rxStat;
    }
    if(pTxStatistics)
    {
        _DRV_ETHMAC_TxLock(pMacD);
        DRV_ETHMAC_LibTxPendingBuffersGet(pMacD, &pMacD->mData._txStat.nTxPendBuffers);
        pMacD->mData._txStat.nTxPendBuffers += DRV_ETHMAC_SingleListCount(&pMacD->mData._TxQueue);
        _DRV_ETHMAC_TxUnlock(pMacD);
        *pTxStatistics = pMacD->mData._txStat;
    }

    return TCPIP_MAC_RES_OK;
}

TCPIP_MAC_RES DRV_ETHMAC_PIC32MACRegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries)
{
    const DRV_ETHMAC_HW_REG_DCPT*   pHwRegDcpt;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;

    int nHwEntries = sizeof(macPIC32INTHwRegDcpt)/sizeof(*macPIC32INTHwRegDcpt);

    if(pHwEntries)
    {
        *pHwEntries = nHwEntries; 
    }

    if(pRegEntries)
    {   // fill data
        unsigned int ix, regLim;

        regLim = nEntries > nHwEntries ? nHwEntries : nEntries;
        pHwRegDcpt = macPIC32INTHwRegDcpt;
        for(ix = 0; ix < regLim; ix++, pRegEntries++, pHwRegDcpt++)
        {
            strncpy(pRegEntries->registerName, pHwRegDcpt->regName, sizeof(pRegEntries->registerName));
            pRegEntries->registerValue = (*pHwRegDcpt->regFunc)(pMacD->mData.macConfig.ethModuleId); 
        }
    }

    return TCPIP_MAC_RES_OK;
}

TCPIP_MAC_RES DRV_ETHMAC_PIC32MACParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams)
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;

    if(pMacD->mData.sysStat == SYS_STATUS_READY)
    { 
        if(pMacParams)
        {
            PLIB_ETH_MACGetAddress(pMacD->mData.macConfig.ethModuleId, pMacParams->ifPhyAddress.v); 
            pMacParams->processFlags = (TCPIP_MAC_PROCESS_FLAG_RX | TCPIP_MAC_PROCESS_FLAG_TX); 
            pMacParams->macType = TCPIP_MAC_TYPE_ETH;
            pMacParams->linkMtu = _TCPIP_EMAC_LINK_MTU;
        }

        return TCPIP_MAC_RES_OK;
    }

    return TCPIP_MAC_RES_IS_BUSY;
}

/**************************
 * local functions and helpers
 ***********************************************/

static TCPIP_MAC_RES _MACTxPacket(DRV_ETHMAC_INSTANCE_DCPT* pMacD, TCPIP_MAC_PACKET * ptrPacket)
{
    DRV_ETHMAC_RESULT ethRes;

    // Note: the TCPIP_MAC_DATA_SEGMENT is defined to be a perfect match for DRV_ETHMAC_PKT_DCPT !!!
#if defined(ETH_PIC32_INT_MAC_ISR_TX)
    // avoid conflict with ETHC since we mess with the
    // TX queues from within the ISR!!!
    int ethILev = SYS_INT_SourceDisable(pMacD->mData._macIntSrc);
#endif  // defined(ETH_PIC32_INT_MAC_ISR_TX)

    ethRes = DRV_ETHMAC_LibTxSendPacket(pMacD, (const DRV_ETHMAC_PKT_DCPT*)ptrPacket->pDSeg);

#if defined(ETH_PIC32_INT_MAC_ISR_TX)
    // restore ETHC ints
    SYS_INT_SourceRestore(pMacD->mData._macIntSrc, ethILev);   // re-enable
#endif  // defined(ETH_PIC32_INT_MAC_ISR_TX)
    
    if(ethRes == DRV_ETHMAC_RES_OK)
    {
        return TCPIP_MAC_RES_OK;
    }
    else if(ethRes == DRV_ETHMAC_RES_NO_DESCRIPTORS)
    {
        pMacD->mData._txStat.nTxQueueFull++;
        return TCPIP_MAC_RES_PENDING;
    }
    
    pMacD->mData._txStat.nTxErrorPackets++;
    return TCPIP_MAC_RES_PACKET_ERR;
}


// acknowledge the ETHC packets
static void _MACTxAcknowledgeEth(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
#if defined(ETH_PIC32_INT_MAC_ISR_TX)
    // avoid conflict with ETHC since we mess with the
    // TX queues from within the ISR!!!
    int ethILev = SYS_INT_SourceDisable(pMacD->mData._macIntSrc);
#endif  // defined(ETH_PIC32_INT_MAC_ISR_TX)

    DRV_ETHMAC_LibTxAcknowledgePacket(pMacD, 0, _MACTxPacketAckCallback, pMacD);

#if defined(ETH_PIC32_INT_MAC_ISR_TX)
    // restore ETHC ints
    SYS_INT_SourceRestore(pMacD->mData._macIntSrc, ethILev);   // re-enable
#endif  // defined(ETH_PIC32_INT_MAC_ISR_TX)
}

static void	_MACTxPacketAckCallback(void* pBuff, int buffIx, void* fParam)
{
    if(buffIx == 0)
    {
       // restore packet the buffer belongs to
        uint16_t buffOffset = *((uint16_t*)pBuff - 1);

        TCPIP_MAC_PACKET* ptrPacket = (TCPIP_MAC_PACKET*)((uint8_t*)pBuff - buffOffset);
        
        // acknowledge the packet
        DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)fParam;
        (*pMacD->mData.pktAckF)(ptrPacket, TCPIP_MAC_PKT_ACK_TX_OK, TCPIP_THIS_MODULE_ID);
        pMacD->mData._txStat.nTxOkPackets++;
    }
}


// transmits pending packets, if any
// if the link is down the TX queued packets are discarded
// returns:
//      - TCPIP_MAC_RES_OK if the queue is empty and another packet can be scheduled for TX
//      - TCPIP_MAC_RES_PENDING if MAC descriptor queue is full and no packet can be scheduled for TX
//
static TCPIP_MAC_RES _MacTxPendingPackets(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    TCPIP_MAC_PACKET* pPkt;
    TCPIP_MAC_RES     pktRes;

    if(pMacD->mData._macFlags._linkPrev == false)
    {   // discard the TX queues
        _MacTxDiscardQueues(pMacD, TCPIP_MAC_PKT_ACK_LINK_DOWN); 
        // no need to try to schedule for TX
        return TCPIP_MAC_RES_PENDING;
    }


    while( (pPkt = (TCPIP_MAC_PACKET*)(pMacD->mData._TxQueue.head)) != 0)
    {
        pktRes = _MACTxPacket(pMacD, pPkt);
        if(pktRes == TCPIP_MAC_RES_PENDING)
        {   // not enough room in the hw queue
            return TCPIP_MAC_RES_PENDING;
        }

        // on way or another we're done with this packet
        DRV_ETHMAC_SingleListHeadRemove(&pMacD->mData._TxQueue);
        if(pktRes != TCPIP_MAC_RES_OK)
        {   // not transmitted
            (*pMacD->mData.pktAckF)(pPkt, TCPIP_MAC_PKT_ACK_BUFFER_ERR, TCPIP_THIS_MODULE_ID);
        }
    }


    return TCPIP_MAC_RES_OK;
}

/*********************************************************************
* Function:        void* _MacCallocCallback( size_t nitems, size_t size, void* param )
 *
 * PreCondition:    None
 * 
 * Input:           nitems - number of items to be allocated
 *                  size   - size of each item
 *                  param  - optional parameter specified when DRV_ETHMAC_PIC32DescriptorsPoolAdd() called 
 * 
 * Output:          pointer to the allocated memory of NULL if allocation failed
 * 
 * Side Effects:    None
 * 
 * Overview:        Memory allocation callback.
 * 
 * Note:            None
 ********************************************************************/
static void* _MacCallocCallback( size_t nitems, size_t size, void* param )
{
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)param;
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
    return (*(TCPIP_MAC_HEAP_CallocFDbg)pMacD->mData._callocF)(pMacD->mData._AllocH, nitems, size, TCPIP_THIS_MODULE_ID, __LINE__);
#else
    return (*pMacD->mData._callocF)(pMacD->mData._AllocH, nitems, size);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)

}



#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
static void _MACDeinit(DRV_ETHMAC_INSTANCE_DCPT* pMacD )
{
    SYS_INT_SourceDisable(pMacD->mData._macIntSrc);      // stop Eth ints
    DRV_ETHMAC_LibClose(pMacD, DRV_ETHMAC_CLOSE_DEFAULT);
    SYS_INT_SourceStatusClear(pMacD->mData._macIntSrc); // clear any pending interrupt flag
    DRV_ETHMAC_PIC32MACEventDeInit((DRV_HANDLE)pMacD);

    const DRV_ETHPHY_OBJECT_BASE* pPhyBase =  pMacD->mData.macConfig.pPhyBase;

    if(pMacD->mData.hPhyClient != DRV_HANDLE_INVALID)
    {
        pPhyBase->DRV_ETHPHY_Close(pMacD->mData.hPhyClient);
        pMacD->mData.hPhyClient = DRV_HANDLE_INVALID;
    }
    if(pMacD->mData.hPhySysObject != SYS_MODULE_OBJ_INVALID)
    {
        pPhyBase->DRV_ETHPHY_Deinitialize(pMacD->mData.hPhySysObject);
        pMacD->mData.hPhySysObject = SYS_MODULE_OBJ_INVALID;
    }
    _MACCleanup(pMacD);
    pMacD->mData.sysStat = SYS_STATUS_UNINITIALIZED; 
    pMacD->mData._macFlags.val = 0;
}

static void _MACCleanup(DRV_ETHMAC_INSTANCE_DCPT* pMacD )
{
    // TX clean up
    DRV_ETHMAC_LibDescriptorsPoolCleanUp(pMacD, DRV_ETHMAC_DCPT_TYPE_TX,  _MacTxFreeCallback, (void*)pMacD);
    _MacTxDiscardQueues(pMacD, TCPIP_MAC_PKT_ACK_NET_DOWN); 

    // RX clean up
    DRV_ETHMAC_LibDescriptorsPoolCleanUp(pMacD, DRV_ETHMAC_DCPT_TYPE_RX,  _MacRxFreeCallback, (void*)pMacD);


    _DRV_ETHMAC_RxDelete(pMacD);
    _DRV_ETHMAC_TxDelete(pMacD);
}

/*********************************************************************
* Function:        void _MacTxFreeCallback(  void* ptr, void* param )
 *
 * PreCondition:    None
 * 
 * Input:           ptr -  pointer to be freed
 *                  param  - optional parameter specified when DRV_ETHMAC_PIC32DescriptorsPoolCleanUp() called 
 * 
 * Output:          None
 * 
 * Side Effects:    None
 * 
 * Overview:        Memory free callback.
 * 
 * Note:            None
 ********************************************************************/
static void _MacTxFreeCallback(  void* ptr, void* param )
{
    uint16_t buffOffset;
    TCPIP_MAC_PACKET* pTxPkt;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)param;

    uint8_t*    pTxBuff = (uint8_t*)DRV_ETHMAC_LibDescriptorGetBuffer(pMacD, ptr);
    if(pTxBuff)
    {
        buffOffset = *((uint16_t*)pTxBuff - 1);
        pTxPkt = (TCPIP_MAC_PACKET*)(pTxBuff - buffOffset);
        (*pMacD->mData.pktAckF)(pTxPkt, TCPIP_MAC_PKT_ACK_NET_DOWN, TCPIP_THIS_MODULE_ID);
    }

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
    (*(TCPIP_MAC_HEAP_FreeFDbg)pMacD->mData._freeF)(pMacD->mData._AllocH, ptr, TCPIP_THIS_MODULE_ID, __LINE__);
#else
    (*pMacD->mData._freeF)(pMacD->mData._AllocH, ptr);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)

}

static void _MacRxFreeCallback(  void* ptr, void* param )
{
    uint16_t buffOffset;
    TCPIP_MAC_PACKET* pRxPkt;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)param;
    
    uint8_t*    pRxBuff = (uint8_t*)DRV_ETHMAC_LibDescriptorGetBuffer(pMacD, ptr);
    if(pRxBuff)
    {
        buffOffset = *((uint16_t*)pRxBuff - 1);
        pRxPkt = (TCPIP_MAC_PACKET*)(pRxBuff - buffOffset);
        pRxPkt->pDSeg->next = 0;     // break the ETH MAC run time chaining
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
        (*(TCPIP_MAC_PKT_FreeFDbg)pMacD->mData.pktFreeF)(pRxPkt, TCPIP_THIS_MODULE_ID);
#else
        (*pMacD->mData.pktFreeF)(pRxPkt);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
    }

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
    (*(TCPIP_MAC_HEAP_FreeFDbg)pMacD->mData._freeF)(pMacD->mData._AllocH, ptr, TCPIP_THIS_MODULE_ID, __LINE__);
#else
    (*pMacD->mData._freeF)(pMacD->mData._AllocH, ptr);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)

}

#else
static void _MACDeinit(DRV_ETHMAC_INSTANCE_DCPT* pMacD )
{
    SYS_INT_SourceDisable(pMacD->mData._macIntSrc);      // stop Eth ints

    const DRV_ETHPHY_OBJECT_BASE* pPhyBase =  pMacD->mData.macConfig.pPhyBase;

    if(pMacD->mData.hPhyClient != DRV_HANDLE_INVALID)
    {
        pPhyBase->DRV_ETHPHY_Close(pMacD->mData.hPhyClient);
        pMacD->mData.hPhyClient = DRV_HANDLE_INVALID;
    }
    pMacD->mData.hPhySysObject = SYS_MODULE_OBJ_INVALID;
    pMacD->mData.sysStat = SYS_STATUS_UNINITIALIZED; 
    pMacD->mData._macFlags.val = 0;
}
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)

static void _MacTxDiscardQueues(DRV_ETHMAC_INSTANCE_DCPT* pMacD, TCPIP_MAC_PKT_ACK_RES ackRes)
{
    TCPIP_MAC_PACKET* pPkt;

#if defined(ETH_PIC32_INT_MAC_ISR_TX)
    while( (pPkt = (TCPIP_MAC_PACKET*)DRV_ETHMAC_SingleListHeadRemove(&pMacD->mData._TxAckQueue)) != 0)
    {   // acknowledge the packet
        (*pMacD->mData.pktAckF)(pPkt, ackRes, TCPIP_THIS_MODULE_ID);
    }
#endif  // defined(ETH_PIC32_INT_MAC_ISR_TX)

    while( (pPkt = (TCPIP_MAC_PACKET*)DRV_ETHMAC_SingleListHeadRemove(&pMacD->mData._TxQueue)) != 0)
    {   // acknowledge the packet
        (*pMacD->mData.pktAckF)(pPkt, ackRes, TCPIP_THIS_MODULE_ID);
    }
}

#if defined(ETH_PIC32_INT_MAC_ISR_TX)
static void _MACTxAcknowledgeAckQueue(DRV_ETHMAC_INSTANCE_DCPT* pMacD)
{
    TCPIP_MAC_PACKET* pPkt;
    while( (pPkt = (TCPIP_MAC_PACKET*)DRV_ETHMAC_SingleListHeadRemove(&pMacD->mData._TxAckQueue)) != 0)
    {   // acknowledge the packet; the ackRes should be set
        (*pMacD->mData.pktAckF)(pPkt, ackRes, TCPIP_THIS_MODULE_ID);
    }

}
#endif  // defined(ETH_PIC32_INT_MAC_ISR_TX)

// a RX packet has been done with 
static bool _MacRxPacketAck(TCPIP_MAC_PACKET* pRxPkt,  const void* param)
{
    uint16_t buffOffset;
    TCPIP_MAC_PACKET* pCurrPkt;
    TCPIP_MAC_DATA_SEGMENT* pSeg, *pNSeg;

    DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)param;

    bool isMacDead = pMacD->mData._macFlags._init == 0; // if we're dead and gone

    for(pSeg = pRxPkt->pDSeg; pSeg != 0; pSeg = pNSeg)
    {
        pNSeg = pSeg->next;
        pSeg->next = 0;     // break the ETH MAC run time chaining
        if(!isMacDead)
        {   // acknowledge the ETHC
            // No further acknowledge/processing needed from MAC
            _DRV_ETHMAC_RxLock(pMacD);
            DRV_ETHMAC_LibRxAcknowledgeBuffer(pMacD, pSeg->segLoad, 0, 0);
            _DRV_ETHMAC_RxUnlock(pMacD);
        }

        // extract packet the segment belongs to
        buffOffset = *((uint16_t*)pSeg->segLoad - 1);
        pCurrPkt = (TCPIP_MAC_PACKET*)((uint8_t*)pSeg->segLoad - buffOffset);
        if(isMacDead || (pSeg->segFlags & TCPIP_MAC_SEG_FLAG_RX_STICKY) == 0)
        {   // free the packet this segment belongs to
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
            (*(TCPIP_MAC_PKT_FreeFDbg)pMacD->mData.pktFreeF)(pCurrPkt, TCPIP_THIS_MODULE_ID);
#else
            (*pMacD->mData.pktFreeF)(pCurrPkt);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE)
        }
        else
        {
            pCurrPkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
        }
    }

    return false;
}


/*************************
 * local data
 *************************/


static DRV_HANDLE         _hEventMac;     // the MAC we belong to


/*********************************
 *  local proto 
 ******************************************/

void DRV_ETHMAC_Tasks_ISR( SYS_MODULE_OBJ object );

#if defined(ETH_PIC32_INT_MAC_ISR_TX)
static void     _MACTxLocalAckCallback(void* pPktBuff, int buffIx, void* fParam);
#endif  // defined(ETH_PIC32_INT_MAC_ISR_TX)

/****************************************************************************
 * Function:        _XtlEventsTcp2Eth
 *
 * PreCondition:    None
 *
 * Input:           tcpEv - the TcpIp events to be translated
 *
 * Output:          Ethernet events
 *
 * Side Effects:    None
 *
 * Overview:        This function translates from  events to Ethernet events
 *
 * Note:            - The translation could be done in a more systematic way
 *                    using a (few) LUT(s).
 *                    For now this direct approach is taken.
 *                  - The CONN_LOST/CONN_ESTABLISHED events do NOT map to Ethernet events! 
 *                    The current translation function assumes no overlap between a CONN event
 *                    and an Ethernet one!
 *                    By this translation the CONN_LOST/CONN_ESTABLISHED event is lost!
 ******************************************************************************/
/*static __inline__*/static  PLIB_ETH_EVENTS /*__attribute__((always_inline))*/ _XtlEventsTcp2Eth(TCPIP_MAC_EVENT tcpEv)
{
    PLIB_ETH_EVENTS  eEvents;

    eEvents =  (tcpEv&(TCPIP_MAC_EV_TX_BUSERR))<<4;
    eEvents |= (tcpEv&(TCPIP_MAC_EV_TX_ABORT))>>7;
    eEvents |= (tcpEv&(TCPIP_MAC_EV_TX_DONE))>>5;
    eEvents |= (tcpEv&(TCPIP_MAC_EV_RX_BUSERR|TCPIP_MAC_EV_RX_PKTPEND))<<6;
    eEvents |= (tcpEv&(TCPIP_MAC_EV_RX_EWMARK|TCPIP_MAC_EV_RX_FWMARK|TCPIP_MAC_EV_RX_DONE))<<3;
    eEvents |= (tcpEv&(TCPIP_MAC_EV_RX_ACT))<<2;
    eEvents |= (tcpEv&(TCPIP_MAC_EV_RX_BUFNA|TCPIP_MAC_EV_RX_OVFLOW))>>1;
    
    return eEvents;
}

/****************************************************************************
 * Function:        _XtlEventsEth2Tcp
 *
 * PreCondition:    None
 *
 * Input:           eEvents - the Eth events to be translated
 *
 * Output:          TcpIp events
 *
 * Side Effects:    None
 *
 * Overview:        This function translates from Ethernet events to TcpIp events
 *
 * Note:            - The translation could be done in a more systematic way
 *                    using a (few) LUT(s).
 *                    For now this direct approach is taken.
 *                  - The CONN_LOST/CONN_ESTABLISHED events do NOT map to Ethernet events! 
 *                    The current translation function assumes no overlap between a CONN event
 *                    and an Ethernet one!
 *                    By this translation a CONN_LOST/CONN_ESTABLISHED event cannot be generated!
 ******************************************************************************/
/*static __inline__*/static  TCPIP_MAC_EVENT /*__attribute__((always_inline))*/ _XtlEventsEth2Tcp(PLIB_ETH_EVENTS eEvents)
{
    TCPIP_MAC_EVENT tcpEv;

    tcpEv =  (eEvents&(_ETHIRQ_TXBUSE_MASK))>>4;
    tcpEv |= (eEvents&(_ETHIRQ_TXABORT_MASK))<<7;
    tcpEv |= (eEvents&(_ETHIRQ_TXDONE_MASK))<<5;
    tcpEv |= (eEvents&(_ETHIRQ_RXBUSE_MASK|_ETHIRQ_PKTPEND_MASK))>>6;
    tcpEv |= (eEvents&(_ETHIRQ_EWMARK_MASK|_ETHIRQ_FWMARK_MASK|_ETHIRQ_RXDONE_MASK))>>3;
    tcpEv |= (eEvents&(_ETHIRQ_RXACT_MASK))>>2;
    tcpEv |= (eEvents&(_ETHIRQ_RXBUFNA_MASK|_ETHIRQ_RXOVFLW_MASK))<<1;
    
    return tcpEv;
}

/*********************************
 *  implementation
 ******************************************/

/*******************************************************************************
  Function:
    TCPIP_MAC_RES DRV_ETHMAC_PIC32MACEventInit(DRV_HANDLE hMac, TCPIP_MAC_EventF eventF, const void* eventParam)

  Summary:
    Initializes the ethernet event notification.

  Description:
     This function initializes the ethernet event notification.
     It performs any resource allocation that may be needed.

  Precondition:
     None.

  Parameters:
    hMac        - parameter identifying the intended MAC  
    eventF      - notification function
    eventParam  - notification parameter
    
  Returns:
    TCPIP_MAC_RES_OK  if initialization succeeded,
    an error code otherwise

  Example:
    <code>
    DRV_ETHMAC_PIC32MACEventInit( hMac, eventF, param);
    </code>

  Remarks:

    Not multi-threaded safe.
*****************************************************************************/
static TCPIP_MAC_RES DRV_ETHMAC_PIC32MACEventInit(DRV_HANDLE hMac, TCPIP_MAC_EventF eventF, const void* eventParam)
{
    DRV_ETHMAC_EVENT_DCPT*    pDcpt; 
    DRV_ETHMAC_INSTANCE_DCPT*     pMacD;

    if(_hEventMac == hMac)
    {   // re-init the same MAC
        return TCPIP_MAC_RES_OK;
    }
    else if(_hEventMac != 0)
    {   // already opened
        return TCPIP_MAC_RES_IS_BUSY;
    }

    pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;
    SYS_INT_SourceDisable(pMacD->mData._macIntSrc);      // stop Eth ints
    SYS_INT_SourceStatusClear(pMacD->mData._macIntSrc);

    pDcpt = &pMacD->mData._pic32_ev_group_dcpt;
    pDcpt->_TcpEnabledEvents = pDcpt->_TcpPendingEvents = TCPIP_MAC_EV_NONE;
    pDcpt->_EthEnabledEvents = pDcpt->_EthPendingEvents = 0;
    pDcpt->_TcpNotifyFnc = eventF;
    pDcpt->_TcpNotifyParam = eventParam;

    _hEventMac = hMac;    
    return TCPIP_MAC_RES_OK;
}

/*******************************************************************************
  Function:
    TCPIP_MAC_RES    DRV_ETHMAC_PIC32MACEventDeInit(DRV_HANDLE hMac )

  Summary:
    De-initializes the ethernet event notification.

  Description:
     This function de-initializes the ethernet event notification.
     It performs any resource clean-up that may be needed.

  Precondition:
     None.

  Parameters:
    hMac      - parameter identifying the intended MAC  
    
  Returns:
    TCPIP_MAC_RES_OK  always

  Example:
    <code>
    DRV_ETHMAC_PIC32MACEventDeInit( hMac );
    </code>

  Remarks:

    Not multi-threaded safe.
*****************************************************************************/
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
static TCPIP_MAC_RES DRV_ETHMAC_PIC32MACEventDeInit(DRV_HANDLE hMac)
{
    DRV_ETHMAC_EVENT_DCPT* pDcpt; 
    DRV_ETHMAC_INSTANCE_DCPT*  pMacD;

    if(_hEventMac != hMac)
    {   // not us?
        return TCPIP_MAC_RES_IS_BUSY;
    }

    pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;
    SYS_INT_SourceDisable(pMacD->mData._macIntSrc);      // stop Eth ints
    SYS_INT_SourceStatusClear(pMacD->mData._macIntSrc);

    pDcpt = &pMacD->mData._pic32_ev_group_dcpt;
    pDcpt->_TcpNotifyFnc = 0;
    pDcpt->_TcpEnabledEvents = pDcpt->_TcpPendingEvents = TCPIP_MAC_EV_NONE;
    pDcpt->_EthEnabledEvents = pDcpt->_EthPendingEvents = 0;

    _hEventMac = 0;
    return TCPIP_MAC_RES_OK;
}
#endif  // (TCPIP_STACK_MAC_DOWN_OPERATION != 0)


/*******************************************************************************
  Function:
    bool    DRV_ETHMAC_PIC32MACEventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable)

  Summary:
    Enables the MAC events.

  Description:
     This function sets the enabled events.
     Multiple events can be orr-ed together.
     All events that are set will be added to the notification process. The old events will be disabled.
     The stack (or stack user) has to catch the events that are notified and process them:
         - The stack should process the TCPIP_MAC_EV_RX_PKTPEND/TCPIP_MAC_EV_RX_DONE, TCPIP_MAC_EV_TX_DONE transfer events
         - Process the specific condition and acknowledge them calling DRV_ETHMAC_PIC32MACEventAcknowledge() so that they can be re-enabled.

  Precondition:
   DRV_ETHMAC_PIC32MACEventInit should have been called.

  Parameters:
    hMac      - parameter identifying the intended MAC  
    macEvMask - events the user of the stack wants to add/delete for notification
    enable    - if true, the events will  be enabled, else disabled
    
  Returns:
    true  if operation succeeded,
    an error code otherwise

  Example:
    <code>
    DRV_ETHMAC_PIC32MACEventMaskSet( hMac, TCPIP_MAC_EV_RX_OVFLOW | TCPIP_MAC_EV_RX_BUFNA, true );
    </code>

  Remarks:
    The event notification system enables the user of the TCPIP stack to call into the stack
    for processing only when there are relevant events rather than being forced to periodically call
    from within a loop.
    
    If the notification events are nill the interrupt processing will be disabled.
    Otherwise the event notification will be enabled and the interrupts relating to the requested events will be enabled.
    
    Note that once an event has been caught by the stack ISR (and reported if a notification handler is in place)
    it will be disabled until the DRV_ETHMAC_PIC32MACEventAcknowledge() is called.

*****************************************************************************/
bool DRV_ETHMAC_PIC32MACEventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvMask, bool enable)
{
    DRV_ETHMAC_INSTANCE_DCPT*     pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;
    DRV_ETHMAC_EVENT_DCPT*  pDcpt = &pMacD->mData._pic32_ev_group_dcpt; 
    ETH_MODULE_ID   ethId = pMacD->mData.macConfig.ethModuleId;

    if(enable)
    {
        PLIB_ETH_EVENTS  ethSetEvents;
        ethSetEvents = _XtlEventsTcp2Eth(macEvMask);

        if(pDcpt->_TcpEnabledEvents != 0)
        {   // already have some active     
            SYS_INT_SourceDisable(pMacD->mData._macIntSrc);      // stop ints for a while
        }

        pDcpt->_TcpEnabledEvents |= macEvMask;        // add more
        pDcpt->_EthEnabledEvents |= ethSetEvents;  

        if(pDcpt->_TcpEnabledEvents != 0)
        {
            ethSetEvents &= ~pDcpt->_EthPendingEvents;     // keep just the new un-ack events
            PLIB_ETH_EventsClr(ethId, ethSetEvents );   // clear the old pending ones
            PLIB_ETH_EventsEnableSet(ethId, ethSetEvents );                 // enable the new un-ack ones!

            SYS_INT_SourceEnable(pMacD->mData._macIntSrc);       // enable
        }
    }
    else
    {   // disable some events
        PLIB_ETH_EVENTS  ethClrEvents;
        int         ethILev = 0;

        macEvMask &= pDcpt->_TcpEnabledEvents;                  // keep just the enabled ones
        ethClrEvents = _XtlEventsTcp2Eth(macEvMask);

        if(pDcpt->_TcpEnabledEvents != 0)
        {   // already have some active     
            ethILev = SYS_INT_SourceDisable(pMacD->mData._macIntSrc);      // stop ints for a while
        }

        pDcpt->_TcpEnabledEvents &= ~macEvMask;     // clear some of them
        pDcpt->_EthEnabledEvents &= ~ethClrEvents;

        pDcpt->_TcpPendingEvents &= ~macEvMask;     // remove them from un-ack list
        pDcpt->_EthPendingEvents &= ~ethClrEvents;

        PLIB_ETH_EventsEnableClr(ethId, ethClrEvents);   // no longer enabled
        PLIB_ETH_EventsClr(ethId, ethClrEvents);         // clear the pending ones

        if(pDcpt->_TcpEnabledEvents != 0)
        {
            SYS_INT_SourceRestore(pMacD->mData._macIntSrc, ethILev);   // re-enable
        }
    }

    return true;
}


/*******************************************************************************
  Function:
    bool    DRV_ETHMAC_PIC32MACEventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv)

  Summary:
    Acknowledges and re-enables processed events.

  Description:
    This function acknowledges and re-enables processed events.
    Multiple events can be orr-ed together as they are processed together.
    The events acknowledged by this function should be the events that have been retrieved from the stack
    by calling DRV_ETHMAC_PIC32MACEventPendingGet() or have been passed to the user by the stack using the notification handler
    (PIC32MACEventSetNotifyHandler()) and have been processed and have to be re-enabled.


  Precondition:
   DRV_ETHMAC_PIC32MACEventInit should have been called.

  Parameters:
    hMac      - parameter identifying the intended MAC  
    macEvents - the events that the user processed and need to be re-enabled
    
  Returns:
    true if events acknowledged
    false if no events to be acknowledged

  Example:
    <code>
    DRV_ETHMAC_PIC32MACEventAcknowledge( hMac, stackNewEvents );
    </code>

  Remarks:   
    All events should be acknowledged, in order to be re-enabled.

    Some events are fatal errors and should not be acknowledged (TCPIP_MAC_EV_RX_BUSERR, TCPIP_MAC_EV_TX_BUSERR).
    Stack re-initialization is needed under such circumstances.

    Some events are just system/application behavior and they are intended only as simple info (TCPIP_MAC_EV_RX_OVFLOW,
    TCPIP_MAC_EV_RX_BUFNA, TCPIP_MAC_EV_TX_ABORT, TCPIP_MAC_EV_RX_ACT).

    The TCPIP_MAC_EV_RX_FWMARK and TCPIP_MAC_EV_RX_EWMARK events are part of the normal flow control operation (if auto flow control was enabled).
    They should be enabled alternatively, if needed.

    The events are persistent. They shouldn't be re-enabled unless they have been processed and the condition that generated them was removed.
    Re-enabling them immediately without proper processing will have dramatic effects on system performance.

*****************************************************************************/
bool DRV_ETHMAC_PIC32MACEventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT tcpAckEv)
{
    int                   ethILev;
    DRV_ETHMAC_INSTANCE_DCPT*     pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;
    DRV_ETHMAC_EVENT_DCPT*  pDcpt = &pMacD->mData._pic32_ev_group_dcpt; 
    ETH_MODULE_ID   ethId = pMacD->mData.macConfig.ethModuleId;

    if(pDcpt->_TcpEnabledEvents != 0)
    {   // already have some active     
        PLIB_ETH_EVENTS  ethAckEv;

        ethAckEv=_XtlEventsTcp2Eth(tcpAckEv);

        ethILev = SYS_INT_SourceDisable(pMacD->mData._macIntSrc);  // stop ints for a while

        pDcpt->_TcpPendingEvents &= ~tcpAckEv;         // no longer pending

        pDcpt->_EthPendingEvents &= ~ethAckEv;         // no longer pending

        PLIB_ETH_EventsClr(ethId, ethAckEv);                 // clear the old pending ones
        PLIB_ETH_EventsEnableSet(ethId, ethAckEv);           // re-enable the ack ones
        

        SYS_INT_SourceRestore(pMacD->mData._macIntSrc, ethILev);   // re-enable
        return true;
    }

    return false;
}


/*******************************************************************************
  Function:
    TCPIP_MAC_EVENT DRV_ETHMAC_PIC32MACEventPendingGet(DRV_HANDLE hMac)

  Summary:
    Returns the currently pending events.

  Description:
    This function returns the currently pending events belonging to a group.
    Multiple events can be orr-ed together as they accumulate.
    The stack should be called for processing whenever a stack managed event (TCPIP_MAC_EV_RX_PKTPEND, TCPIP_MAC_EV_TX_DONE) is present.
    The other, non critical events, may not be managed by the stack and passed to an user.
    They will have to be eventually acknowledged if re-enabling is needed.

  Precondition:
   hMac      - parameter identifying the intended MAC  
   DRV_ETHMAC_PIC32MACEventInit should have been called.

  Parameters:
    
  Returns:
    The currently stack pending events.

  Example:
    <code>
    TCPIP_MAC_EVENT currEvents = DRV_ETHMAC_PIC32MACEventPendingGet( hMac);
    </code>

  Remarks:   
    This is the preferred method to get the current pending MAC events.
    The stack maintains a proper image of the events from their occurrence to their acknowledgement.
    
    Even with a notification handler in place it's better to use this function to get the current pending events
    rather than using the events passed by the notification handler which could be stale.
    
    The events are persistent. They shouldn't be re-enabled unless they have been processed and
    the condition that generated them was removed.
    Re-enabling them immediately without proper processing will have dramatic effects on system performance.

    The returned value is just a momentary value. The pending events can change any time.
*****************************************************************************/
TCPIP_MAC_EVENT DRV_ETHMAC_PIC32MACEventPendingGet(DRV_HANDLE hMac)
{
    DRV_ETHMAC_INSTANCE_DCPT*     pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)hMac;
    return pMacD->mData._pic32_ev_group_dcpt._TcpPendingEvents;
}


/**************************************
 * local functions
 ****************************************/

static ETH_RX_FILTERS _DRV_ETHMAC_MacToEthFilter(TCPIP_MAC_RX_FILTER_TYPE macFilter)
{
    int ix;
    ETH_RX_FILTERS rxFilter = 0;

    for(ix = 0; ix < 16; ix++)
    {
        if( ((uint32_t)macFilter & (1 << ix)) != 0)
        {
            rxFilter |= _DRV_ETHMAC_FiltConvTbl[ix];
        }
    }

    return rxFilter;
}



/****************************************************************************
 * Function:        DRV_ETHMAC_Tasks_ISR
 *
 * PreCondition:    DRV_ETHMAC_PIC32MACEventInit, DRV_ETHMAC_PIC32MACEventMaskSet should have been called.
 *
 * Input:           macIndex - PIC32 MAC object index
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function processes the Ethernet interrupts and reports the events back to the user.
 *
 * Note:            None
 ******************************************************************************/
void DRV_ETHMAC_Tasks_ISR( SYS_MODULE_OBJ macIndex )
{
    PLIB_ETH_EVENTS          currEthEvents, currGroupEvents;
    DRV_ETHMAC_EVENT_DCPT* pDcpt;
    //DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)p;
    DRV_ETHMAC_INSTANCE_DCPT* pMacD = &_pic32_emb_mac_dcpt[macIndex];
    ETH_MODULE_ID   ethId = pMacD->mData.macConfig.ethModuleId;

    currEthEvents = PLIB_ETH_EventsGet(ethId);


    // process interrupts
    pDcpt = &pMacD->mData._pic32_ev_group_dcpt;
    currGroupEvents = currEthEvents & pDcpt->_EthEnabledEvents;     //  keep just the relevant ones

#if defined(ETH_PIC32_INT_MAC_ISR_TX)
    bool isTxInterrupt = (currGroupEvents & ETH_EV_TXDONE) != 0;
#endif  // defined(ETH_PIC32_INT_MAC_ISR_TX)

    if(currGroupEvents)
    {
        pDcpt->_EthPendingEvents |= currGroupEvents;                    // add the new events
        pDcpt->_TcpPendingEvents |= _XtlEventsEth2Tcp(currGroupEvents);

#if defined(ETH_PIC32_INT_MAC_ISR_TX)
        if(isTxInterrupt)
        {
            PLIB_ETH_EventsEnableClr(ethId, currGroupEvents & ~ETH_EV_TXDONE);   // don't disable TX_DONE
            PLIB_ETH_EventsClr(ethId, currGroupEvents | ETH_EV_TXDONE);          // always ack TX_DONE
        }
        else
#endif  // defined(ETH_PIC32_INT_MAC_ISR_TX)
        {
            PLIB_ETH_EventsEnableClr(ethId, currGroupEvents);         // these will get reported; disable them until ack is received back
            PLIB_ETH_EventsClr(ethId, currGroupEvents);               // acknowledge the ETHC
        }


        if(pDcpt->_TcpNotifyFnc)
        {
            (*pDcpt->_TcpNotifyFnc)(pDcpt->_TcpPendingEvents, pDcpt->_TcpNotifyParam);     // let the user know
        }
    }
    
#if defined(ETH_PIC32_INT_MAC_ISR_TX)
    if(isTxInterrupt)
    {   // locally acknowledge TX done activity
        DRV_ETHMAC_LibTxAcknowledgePacket(pMacD, 0, _MACTxLocalAckCallback, pMacD);
        // schedule the next packet
        TCPIP_MAC_PACKET* pPkt;
        while((pPkt = (TCPIP_MAC_PACKET*)DRV_ETHMAC_SingleListHeadRemove(&pMacD->mData._TxQueue)) != 0)
        {
            DRV_ETHMAC_RESULT ethRes = DRV_ETHMAC_LibTxSendPacket(pMacD, (const DRV_ETHMAC_PKT_DCPT*)pPkt->pDSeg);
            if(ethRes == DRV_ETHMAC_RES_NO_DESCRIPTORS)
            {   // no more descriptors
                pMacD->mData._txStat.nTxQueueFull++;
                break;
            }
            else if(ethRes != DRV_ETHMAC_RES_OK) 
            {   // it must be an error, DRV_ETHMAC_RES_NO_DESCRIPTORS shouldn't happen
                pPkt->ackRes = TCPIP_MAC_PKT_ACK_BUFFER_ERR; 
                DRV_ETHMAC_SingleListTailAdd(&pMacD->mData._TxAckQueue, (DRV_ETHMAC_SGL_LIST_NODE*)pPkt);
                pMacD->mData._txStat.nTxErrorPackets++;
            }
            // else continue as long as we have packets and descriptors to TX !!!
            // to be efficient: if TX_DONE actually carries multiple
            // TX_DONE events and I send just one packet I won't get another
            // TX ISR until this just sent packet is successfully tx-ed!
        }
    }
#endif  // defined(ETH_PIC32_INT_MAC_ISR_TX)

    SYS_INT_SourceStatusClear(pMacD->mData._macIntSrc);         // acknowledge the int Controller
}

#if defined(ETH_PIC32_INT_MAC_ISR_TX)
// acknowledges just the ETHC so that we can send another packet
// the packet acknowledge is deferred
static void	_MACTxLocalAckCallback(void* pBuff, int buffIx, void* fParam)
{
    if(buffIx == 0)
    {
        // restore packet the buffer belongs to
        uint16_t buffOffset = *((uint16_t*)pBuff - 1);

        TCPIP_MAC_PACKET* ptrPacket = (TCPIP_MAC_PACKET*)((uint8_t*)pBuff - buffOffset);
        
        // insert the packet into the local ACK queue
        DRV_ETHMAC_INSTANCE_DCPT* pMacD = (DRV_ETHMAC_INSTANCE_DCPT*)fParam;
        ptrPacket->ackRes = TCPIP_MAC_PKT_ACK_TX_OK; 
        DRV_ETHMAC_SingleListTailAdd(&pMacD->mData._TxAckQueue, (DRV_ETHMAC_SGL_LIST_NODE*)ptrPacket);
        pMacD->mData._txStat.nTxOkPackets++;
    }
}
#endif  // defined(ETH_PIC32_INT_MAC_ISR_TX)


static uint16_t _DRV_ETHMAC_GetFrmTxOk(ETH_MODULE_ID ethId)
{
    return PLIB_ETH_FramesTxdOkCountGet(ethId);
}

static uint16_t _DRV_ETHMAC_GetFrmRxOk(ETH_MODULE_ID ethId)
{
    return PLIB_ETH_FramesRxdOkCountGet(ethId);
}

static uint16_t _DRV_ETHMAC_GetFrmPktCount(ETH_MODULE_ID ethId)
{
    return PLIB_ETH_RxPacketCountGet(ethId);
}

static uint16_t _DRV_ETHMAC_GetFrmRxOvflow(ETH_MODULE_ID ethId)
{
    return PLIB_ETH_RxOverflowCountGet(ethId);
}

static uint16_t _DRV_ETHMAC_GetFrmFcsError(ETH_MODULE_ID ethId)
{
    return PLIB_ETH_FCSErrorCountGet(ethId);
}

static uint16_t _DRV_ETHMAC_GetAlignError(ETH_MODULE_ID ethId)
{
    return PLIB_ETH_AlignErrorCountGet(ethId);
}

static uint16_t _DRV_ETHMAC_GetSCollisionCount(ETH_MODULE_ID ethId)
{
    return PLIB_ETH_SingleCollisionCountGet(ethId);
}

static uint16_t _DRV_ETHMAC_GetMCollisionCount(ETH_MODULE_ID ethId)
{
    return PLIB_ETH_MultipleCollisionCountGet(ethId);
}


