/*******************************************************************************
  WILC1000 Wireless Driver

  File Name:
    wdrv_wilc1000_main.c

  Summary:
    Wireless driver PIC32 Implementation for WILC1000 support.

  Description:
    Wireless driver PIC32 Implementation for WILC1000 support.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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

#include "wdrv_wilc1000_main.h"
#include "wdrv_wilc1000_scan_helper.h"

#define TCPIP_THIS_MODULE_ID TCPIP_MODULE_MAC_WILC1000

#define ETH_HEADER_SIZE 14
#define NUM_PREALLOCATED_RX_PACKETS 4
#define MAX_IP_PACKET_SIZE 1564 // including header
#define MAX_RX_PACKET_SIZE 1518
#define MAX_TX_PACKET_SIZE 1518
#define MAX_MULTICAST_FILTER_SIZE 16

#define WAIT_FOR_DISCONNECT_COMPLETE() WDRV_SEM_TAKE(&g_wdrv_priv.disconnectDoneSync, OSAL_WAIT_FOREVER)

// used to keep track of RX packets queued for stack
typedef struct
{
    int front;
    int rear;
    TCPIP_MAC_PACKET *items[NUM_PREALLOCATED_RX_PACKETS + 1];
} t_fifo;

// WILC1000 wireless driver descriptor
typedef struct
{
    const TCPIP_MAC_OBJECT *pObj; // safe cast to TCPIP_MAC_DCPT
    TCPIP_NET_IF *pNetIf;         // interface we belong to
    bool isInit;                  // simple init status flag
    bool isOpen;                  // simple open status flag
    SYS_STATUS sysStat;           // driver status
} WDRV_WILC1000_DCPT;

static TCPIP_MAC_PKT_AllocF s_pktAllocF = 0;
static TCPIP_MAC_PKT_FreeF s_pktFreeF = 0;
static TCPIP_MAC_PKT_AckF s_pktAckF = 0;
static t_fifo s_rxFifo;
static SINGLE_LIST s_dataRxQueue; // queue of data rx packets waiting to be processed by stack from host
static TCPIP_MAC_PACKET *s_rxpacket_buffer[NUM_PREALLOCATED_RX_PACKETS];
static uint8_t s_txpacket_buffer[MAX_IP_PACKET_SIZE];
static TCPIP_MAC_MODULE_CTRL s_stackData;
static TCPIP_MAC_ADDR s_MulticastFilter[MAX_MULTICAST_FILTER_SIZE];
static OSAL_MUTEX_HANDLE_TYPE s_multicastFilterLock;
static OSAL_MUTEX_HANDLE_TYPE s_rxFifoLock;
static OSAL_MUTEX_HANDLE_TYPE s_dataQueueLock;

WDRV_WILC1000_PRIV g_wdrv_priv =
{
    /* explicity initialize .initConn and .isDisconnectRequested to true, */
    /* .isOtaFwUpdateRequested to false                                   */
    .initConn = true,
    .isDisconnectRequested = true,
    .isOtaFwUpdateRequested = false
};

static SYS_MODULE_OBJ WDRV_WILC1000_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT *const init);
static void WDRV_WILC1000_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT *const init);
static void WDRV_WILC1000_Deinitialize(SYS_MODULE_OBJ object);
static void WDRV_WILC1000_Tasks(SYS_MODULE_OBJ object);
static SYS_STATUS WDRV_WILC1000_Status(SYS_MODULE_OBJ object);
static TCPIP_MAC_HANDLE WDRV_WILC1000_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);
static void WDRV_WILC1000_Close(TCPIP_MAC_HANDLE hMac);
static TCPIP_MAC_RES WDRV_WILC1000_RegisterStatisticsGet(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY *pRegEntries,
        int nEntries, int *pHwEntries);
static bool WDRV_WILC1000_CheckLink(TCPIP_MAC_HANDLE hMac);
static bool WDRV_WILC1000_PowerMode(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode);
static TCPIP_MAC_RES WDRV_WILC1000_TxPacket(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_PACKET *ptrPacket);
static TCPIP_MAC_PACKET *WDRV_WILC1000_RxPacket (TCPIP_MAC_HANDLE hMac, TCPIP_MAC_RES *pRes,
        const TCPIP_MAC_PACKET_RX_STAT **ppPktStat);
static TCPIP_MAC_RES WDRV_WILC1000_Process(TCPIP_MAC_HANDLE hMac);
static TCPIP_MAC_RES WDRV_WILC1000_StatisticsGet(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_RX_STATISTICS *pRxStatistics,
        TCPIP_MAC_TX_STATISTICS *pTxStatistics);
static TCPIP_MAC_RES WDRV_WILC1000_ParametersGet(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_PARAMETERS *pMacParams);
static bool WDRV_WILC1000_EventSetMask(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable);
static bool WDRV_WILC1000_EventAcknowledge(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_EVENT macEvents);
static TCPIP_MAC_EVENT WDRV_WILC1000_EventGet(TCPIP_MAC_HANDLE hMac);
static size_t WDRV_WILC1000_GetConfig(TCPIP_MODULE_MAC_ID modId, void *configBuff, size_t buffSize,
        size_t *pConfigSize);
static void PowerDown(void);
static void PowerUp(void);
static void FifoInit(t_fifo *const p_fifo);
static bool isFifoEmpty(t_fifo *const p_fifo);
static void FifoInsert(t_fifo *const p_fifo, TCPIP_MAC_PACKET *p_packet);
static TCPIP_MAC_PACKET *FifoRemove(t_fifo *const p_fifo);
static TCPIP_MAC_PACKET *GetRxPacket(void);
static bool RxDataCallback(TCPIP_MAC_PACKET *pktHandle, const void *ackParam);
static void InitRxBuffer(void);
static void DeInitRxBuffer(void);

TCPIP_MAC_RES WDRV_WILC1000_MulticastFilterSet(TCPIP_MAC_HANDLE hMac, const TCPIP_MAC_ADDR *DestMACAddr);

const TCPIP_MAC_OBJECT WDRV_WILC1000_MACObject =
{
    TCPIP_MODULE_MAC_WILC1000,
    "WILC1000",
    WDRV_WILC1000_Initialize,
    WDRV_WILC1000_Deinitialize,
    WDRV_WILC1000_Reinitialize,
    WDRV_WILC1000_Status,
    WDRV_WILC1000_Tasks,
    WDRV_WILC1000_Open,
    WDRV_WILC1000_Close,
    WDRV_WILC1000_CheckLink,
    WDRV_WILC1000_MulticastFilterSet,
    WDRV_WILC1000_PowerMode,
    WDRV_WILC1000_TxPacket,
    WDRV_WILC1000_RxPacket,
    WDRV_WILC1000_Process,
    WDRV_WILC1000_StatisticsGet,
    WDRV_WILC1000_ParametersGet,
    WDRV_WILC1000_RegisterStatisticsGet,
    WDRV_WILC1000_GetConfig,
    WDRV_WILC1000_EventSetMask,
    WDRV_WILC1000_EventAcknowledge,
    WDRV_WILC1000_EventGet,
};

// Only one hardware instance for now!
static WDRV_WILC1000_DCPT wdrv_wilc1000_dcpt =
{
	&WDRV_WILC1000_MACObject,	// specific PIC32 MAC data
	NULL,						// pNetIf
	false,						// isInit
	false,						// isOpen
	SYS_STATUS_UNINITIALIZED,	// sysStat
};

extern uint8_t g_initConn;

static TCPIP_MAC_RES WDRV_WILC1000_RegisterStatisticsGet(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY *pRegEntries,
    int nEntries, int *pHwEntries)
{
    /* unsupported */
    return TCPIP_MAC_RES_OP_ERR;
}

static void WDRV_WILC1000_Close(TCPIP_MAC_HANDLE hMac)
{
	WDRV_WILC1000_DCPT *pDcpt = (WDRV_WILC1000_DCPT *)hMac;
	if (pDcpt->isOpen) {
		if (isLinkUp()) {
			WDRV_Disconnect();
			WAIT_FOR_DISCONNECT_COMPLETE();
		}
		g_wdrv_priv.isDriverOpen = false;
		pDcpt->isOpen = false;
	}
}

static bool WDRV_WILC1000_CheckLink(TCPIP_MAC_HANDLE hMac)
{
    return isLinkUp();
}

static void MulticastFilter_Initialize(void)
{
    int i;

    for (i = 0; i < MAX_MULTICAST_FILTER_SIZE; i++) {
        memset(&s_MulticastFilter[i], 0, sizeof(TCPIP_MAC_ADDR));
    }
}

static bool isMulticastAddrSet(const TCPIP_MAC_ADDR *addr)
{
    int i;

    for (i = 0; i < MAX_MULTICAST_FILTER_SIZE; i++) {
        if (memcmp(addr, &s_MulticastFilter[i], 6) == 0)
            return true;
    }

    return false;
}

static void MulticastAddr_Set(const TCPIP_MAC_ADDR *addr, int index)
{
     memcpy(&s_MulticastFilter[index], addr, 6);
}

TCPIP_MAC_RES WDRV_WILC1000_MulticastFilterSet(TCPIP_MAC_HANDLE hMac, const TCPIP_MAC_ADDR *DestMACAddr)
{
    int i;
    TCPIP_MAC_RES res;
    uint8_t all_zeros[6] = {0, 0, 0, 0, 0, 0};

    WDRV_MUTEX_LOCK(&s_multicastFilterLock, OSAL_WAIT_FOREVER);

    do {
        if (isMulticastAddrSet(DestMACAddr)) {
            res = TCPIP_MAC_RES_OK;
            break;
        }

        for (i = 0; i < MAX_MULTICAST_FILTER_SIZE; i++) {
            if (memcmp(all_zeros, &s_MulticastFilter[i], 6) == 0)
                break;
        }

        if (i == MAX_MULTICAST_FILTER_SIZE) {
            WDRV_ASSERT(false, "Multicast filter is full\r\n");
            res = TCPIP_MAC_RES_OP_ERR;
            break;
        }

        MulticastAddr_Set(DestMACAddr, i);
        WDRV_EXT_MulticastFilterSet((uint8_t *)DestMACAddr);
        res = TCPIP_MAC_RES_OK;
    } while(0);

    WDRV_MUTEX_UNLOCK(&s_multicastFilterLock);

    return res;
}

static bool WDRV_WILC1000_PowerMode(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode)
{
    if (pwrMode == TCPIP_MAC_POWER_FULL)
        PowerUp();
    else if (pwrMode == TCPIP_MAC_POWER_DOWN)
        PowerDown();

    return true;
}

static TCPIP_MAC_RES WDRV_WILC1000_TxPacket(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_PACKET *ptrPacket)
{
    TCPIP_MAC_RES res = TCPIP_MAC_RES_OK;
    TCPIP_MAC_PACKET const *p_packet = ptrPacket;
    TCPIP_MAC_DATA_SEGMENT *p_seg;
    uint8_t *p_segData;
    uint32_t sendResult;
    uint16_t curIndex = 0;

    if (isLinkUp() == false) {
        WDRV_DBG_INFORM_MESSAGE(("WILC1000 is in unconnected state, dropped the Tx packet\r\n"));
        res = TCPIP_MAC_RES_PACKET_ERR;
        // call stack ack function to let it know packet was transmitted
        if (s_pktAckF)
            s_pktAckF(ptrPacket, TCPIP_MAC_PKT_ACK_TX_OK, TCPIP_THIS_MODULE_ID);
        else
            WDRV_ASSERT(false, "Should never happen");
        return res;
    }

    // write out the first segment to WILC1000, including prepended internal header
    p_seg = p_packet->pDSeg; // point to first segment
    p_segData = p_seg->segLoad;

    if (p_seg->segLen > MAX_TX_PACKET_SIZE) {
        WDRV_DBG_ERROR_PRINT(("Invalid packet length %d, dropped the Tx packet\r\n", p_seg->segLen));
        res = TCPIP_MAC_RES_PACKET_ERR;
        // call stack ack function to let it know packet was transmitted
        if (s_pktAckF)
            s_pktAckF(ptrPacket, TCPIP_MAC_PKT_ACK_TX_OK, TCPIP_THIS_MODULE_ID);
        else
            WDRV_ASSERT(false, "Should never happen");
        return res;
    }

    if (p_seg->next == NULL) {
         if (p_seg->segLen > MAX_TX_PACKET_SIZE) {
                WDRV_DBG_ERROR_PRINT(("Invalid packet length %d, dropped the Tx packet\r\n", p_seg->segLen));
                res = TCPIP_MAC_RES_PACKET_ERR;
                // call stack ack function to let it know packet was transmitted
                if (s_pktAckF) {
                    s_pktAckF(ptrPacket, TCPIP_MAC_PKT_ACK_TX_OK, TCPIP_THIS_MODULE_ID);
                } else {
                    WDRV_ASSERT(false, "Should never happen");
                }
                return res;
         }
         sendResult = WDRV_EXT_DataSend(p_seg->segLen, p_segData);
    } else {
        memcpy(s_txpacket_buffer, p_segData, p_seg->segLen);
        curIndex += p_seg->segLen;
        while (p_seg->next != NULL) {
            if (curIndex > MAX_TX_PACKET_SIZE) {
                WDRV_DBG_ERROR_PRINT(("Invalid packet length %d, dropped the Tx packet\r\n", curIndex));
                res = TCPIP_MAC_RES_PACKET_ERR;
                // call stack ack function to let it know packet was transmitted
                if (s_pktAckF) {
                    s_pktAckF(ptrPacket, TCPIP_MAC_PKT_ACK_TX_OK, TCPIP_THIS_MODULE_ID);
                } else {
                    WDRV_ASSERT(false, "Should never happen");
                }
                return res;
            }
            p_seg = p_seg->next;
            p_segData = p_seg->segLoad;
            memcpy(s_txpacket_buffer + curIndex, p_segData, p_seg->segLen);
            curIndex += p_seg->segLen;
        }
        sendResult = WDRV_EXT_DataSend(curIndex, s_txpacket_buffer);
    }

    if (sendResult != 0) {
        res = TCPIP_MAC_RES_PACKET_ERR;
        WDRV_DBG_TRACE_MESSAGE(("No Tx buffer is available, dropped the packet\r\n"));
    } else {
        WDRV_DBG_TRACE_MESSAGE(("Sent packet to module\r\n"));
    }

    // call stack ack function to let it know packet was transmitted
    if (s_pktAckF)
        s_pktAckF(ptrPacket, TCPIP_MAC_PKT_ACK_TX_OK, TCPIP_THIS_MODULE_ID);
    else
        WDRV_ASSERT(false, "Should never happen");
    return res;
}

static TCPIP_MAC_PACKET *WDRV_WILC1000_RxPacket (TCPIP_MAC_HANDLE hMac, TCPIP_MAC_RES *pRes,
    const TCPIP_MAC_PACKET_RX_STAT **ppPktStat)
{
    WDRV_DBG_TRACE_MESSAGE(("Received packet from module\r\n"));
    return GetRxPacket();
}

static TCPIP_MAC_RES WDRV_WILC1000_Process(TCPIP_MAC_HANDLE hMac)
{
    /* unsupported */
    return TCPIP_MAC_RES_OK;
}

static TCPIP_MAC_RES WDRV_WILC1000_StatisticsGet(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_RX_STATISTICS *pRxStatistics,
    TCPIP_MAC_TX_STATISTICS *pTxStatistics)
{
    /* unsupported */
    return TCPIP_MAC_RES_OP_ERR;
}

static TCPIP_MAC_RES WDRV_WILC1000_ParametersGet(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_PARAMETERS *pMacParams)
{
    WDRV_WILC1000_DCPT *pDcpt = (WDRV_WILC1000_DCPT *)hMac;

    if (pDcpt->sysStat == SYS_STATUS_READY) {
        if (pMacParams) {
            memcpy(pMacParams->ifPhyAddress.v, s_stackData.ifPhyAddress.v, sizeof(pMacParams->ifPhyAddress));
            pMacParams->processFlags = (TCPIP_MAC_PROCESS_FLAG_RX | TCPIP_MAC_PROCESS_FLAG_TX);
            pMacParams->macType = TCPIP_MAC_TYPE_WLAN;
        }
        return TCPIP_MAC_RES_OK;
    }
    return TCPIP_MAC_RES_IS_BUSY;
}

static bool WDRV_WILC1000_EventSetMask(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable)
{
    return WDRV_TrafficEventMask(hMac, macEvents, enable);
}

static bool WDRV_WILC1000_EventAcknowledge(TCPIP_MAC_HANDLE hMac, TCPIP_MAC_EVENT macEvents)
{
    return WDRV_TrafficEventAck(hMac, macEvents);
}

static TCPIP_MAC_EVENT WDRV_WILC1000_EventGet(TCPIP_MAC_HANDLE hMac)
{
    return  WDRV_TrafficEventGet(hMac);
}

static void PowerDown(void)
{
    WDRV_EXT_ModuleUpDown(false);
}

static void PowerUp(void)
{
    WDRV_EXT_ModuleUpDown(true);
}

static void FifoInit(t_fifo *const p_fifo)
{
    memset(p_fifo, 0x00, sizeof(t_fifo));
    p_fifo->front = p_fifo->rear = NUM_PREALLOCATED_RX_PACKETS - 1;
}

static bool isFifoEmpty(t_fifo *const p_fifo)
{
    return p_fifo->front == p_fifo->rear;
}

static bool isFifoFull(t_fifo *const p_fifo)
{
    if ((p_fifo->front == 0) && (p_fifo->rear == NUM_PREALLOCATED_RX_PACKETS - 1))
        return true;
    else if (p_fifo->front == p_fifo->rear + 1)
        return true;
    else
        return false;
}

static void FifoInsert(t_fifo *const p_fifo, TCPIP_MAC_PACKET *p_packet)
{
    WDRV_MUTEX_LOCK(&s_rxFifoLock, OSAL_WAIT_FOREVER);

    if (isFifoFull(p_fifo))
    {
        if ((*p_packet->ackFunc)(p_packet, p_packet->ackParam))
        {
            p_packet->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
        }
    }

    if (p_fifo->rear == NUM_PREALLOCATED_RX_PACKETS - 1)
        p_fifo->rear = 0;
    else
        ++p_fifo->rear;

    p_fifo->items[p_fifo->rear] = p_packet;

    WDRV_MUTEX_UNLOCK(&s_rxFifoLock);
}

static TCPIP_MAC_PACKET *FifoRemove(t_fifo *const p_fifo)
{
    if (p_fifo->front == NUM_PREALLOCATED_RX_PACKETS - 1)
        p_fifo->front = 0;
    else
        ++p_fifo->front;

    return p_fifo->items[p_fifo->front];
}

// retrieve the oldest of the queued Rx packets to deliver to the stack
static TCPIP_MAC_PACKET *GetRxPacket(void)
{
    TCPIP_MAC_PACKET *ret;

    WDRV_MUTEX_LOCK(&s_rxFifoLock, OSAL_WAIT_FOREVER);

    if (!isFifoEmpty(&s_rxFifo))
        ret = FifoRemove(&s_rxFifo);
    else
        ret = NULL; // signals no rx packet available.

    WDRV_MUTEX_UNLOCK(&s_rxFifoLock);

    return ret;
}
static bool RxDataCallback(TCPIP_MAC_PACKET *pktHandle, const void *ackParam)
{
    ackParam = ackParam; // ignore warning

    if (pktHandle){
        // if this is packet allocated at init and is going to be reused
        if ((pktHandle->pDSeg->segFlags & TCPIP_MAC_SEG_FLAG_RX_STICKY) == TCPIP_MAC_SEG_FLAG_RX_STICKY) {
            pktHandle->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;
            WDRV_MUTEX_LOCK(&s_dataQueueLock, OSAL_WAIT_FOREVER);
            TCPIP_Helper_SingleListTailAdd(&s_dataRxQueue, (SGL_LIST_NODE *)pktHandle); // add packet back to free list
            WDRV_MUTEX_UNLOCK(&s_dataQueueLock);
        }
    } else {
        WDRV_ASSERT(false, "pktHandle cannot be null");
    }

    return false;
}

static void InitRxBuffer(void)
{
    int32_t i;
    // create a queue to hold pointers to preallocated Rx packets
    TCPIP_Helper_SingleListInitialize(&s_dataRxQueue);

    for (i = 0; i < NUM_PREALLOCATED_RX_PACKETS; ++i) {
        // preallocate Rx buffers to store Rx packets as they come in (1500 bytes data plus header and checksum)
        s_rxpacket_buffer[i] = s_pktAllocF ? s_pktAllocF(sizeof(TCPIP_MAC_PACKET), MAX_RX_PACKET_SIZE, 0) : 0; //_TCPIP_PKT_PacketAllocDebug

        if (s_rxpacket_buffer[i] != NULL) {
            s_rxpacket_buffer[i]->next = NULL;
            s_rxpacket_buffer[i]->ackFunc = RxDataCallback;
            s_rxpacket_buffer[i]->ackParam = NULL;
            s_rxpacket_buffer[i]->pktFlags = 0;
            s_rxpacket_buffer[i]->pDSeg->segFlags |= TCPIP_MAC_SEG_FLAG_RX_STICKY;
            TCPIP_Helper_SingleListTailAdd(&s_dataRxQueue, (SGL_LIST_NODE *)s_rxpacket_buffer[i]);
        } else {
            WDRV_ASSERT(false, "");
        }
    }

    FifoInit(&s_rxFifo);
}

static void DeInitRxBuffer(void)
{
    int i;

    if (s_pktFreeF != NULL) {
        for (i = 0; i < NUM_PREALLOCATED_RX_PACKETS; ++i) {
            if (s_rxpacket_buffer[i] != NULL) {
                s_pktFreeF(s_rxpacket_buffer[i]);
                s_rxpacket_buffer[i] = NULL;
            }
        }
    }
}

// Finds an available RX packet structure from the list that was allocated and
// queued up at initialization process.
static void *GetAvailRxBuf(void)
{
    TCPIP_MAC_PACKET *p_packet = NULL;

    WDRV_MUTEX_LOCK(&s_dataQueueLock, OSAL_WAIT_FOREVER);
    if (s_dataRxQueue.nNodes > 0) {
        p_packet = (TCPIP_MAC_PACKET *)TCPIP_Helper_SingleListHeadRemove(&s_dataRxQueue);
        WDRV_ASSERT(p_packet != NULL, "Should never happen");
    }
    WDRV_MUTEX_UNLOCK(&s_dataQueueLock);

    return p_packet;
}

#ifdef ETH_RX_ZERO_COPY
void PushFrameToFifo(uint32_t len, uint8_t const *const frame)
{
    TCPIP_MAC_PACKET *p_packet = (TCPIP_MAC_PACKET *)frame;

    WDRV_DBG_TRACE_MESSAGE(("Received packet\r\n"));

    // mark packet as queued and stuff in timestamp
    p_packet->pDSeg->segLen = len - ETH_HEADER_SIZE;
    p_packet->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
    p_packet->tStamp = SYS_TMR_TickCountGet();

    // Note: re-set pMacLayer and pNetLayer; IPv6 changes these pointers inside the packet, so
    //       when Rx packets are reused this is needed.
    p_packet->pMacLayer = p_packet->pDSeg->segLoad;
    p_packet->pNetLayer = p_packet->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);

    // store packet pointer in FIFO and signal stack that rx packet ready to process
    FifoInsert(&s_rxFifo, p_packet);

    // notify stack of Rx packet has arrived.
    WDRV_TrafficEventReq(TCPIP_EV_RX_DONE, 0);
}
#else /* !ETH_RX_ZERO_COPY */
void PushFrameToFifo(uint32_t len, uint8_t const *const frame)
{
    TCPIP_MAC_PACKET *p_packet;

    WDRV_DBG_TRACE_MESSAGE(("Received packet\r\n"));

	p_packet = GetAvailRxBuf();
	if (p_packet == NULL)
		return;

    // mark packet as queued and stuff in timestamp
    p_packet->pDSeg->segLen = len - ETH_HEADER_SIZE;
    p_packet->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
    p_packet->tStamp = SYS_TMR_TickCountGet();

	memcpy(p_packet->pDSeg->segLoad, frame, len);

    // Note: re-set pMacLayer and pNetLayer; IPv6 changes these pointers inside the packet, so
    //       when Rx packets are reused this is needed.
    p_packet->pMacLayer = p_packet->pDSeg->segLoad;
    p_packet->pNetLayer = p_packet->pMacLayer + sizeof(TCPIP_MAC_ETHERNET_HEADER);

    // store packet pointer in FIFO and signal stack that rx packet ready to process
    FifoInsert(&s_rxFifo, p_packet);

    // notify stack of Rx packet has arrived.
    WDRV_TrafficEventReq(TCPIP_EV_RX_DONE, 0);
}
#endif /* ETH_RX_ZERO_COPY */

static void RfReadyHandler(uint8_t const *const addr)
{
	g_wdrv_priv.updateMacAddressRequired = 1;
    memcpy(g_wdrv_priv.macAddr, addr, 6);
}

static void ScanDoneHandler(uint32_t status)
{
    if (status == 0) {
        uint16_t numberOfResults = 0;

        WDRV_EXT_ScanDoneSet();
        WDRV_EXT_CmdScanGet(&numberOfResults);
        g_wdrv_priv.isScanDone = true;
        g_scanStatus.numberOfResults = numberOfResults;
        SCAN_CLEAR_IN_PROGRESS(g_scanStatus.scanState);
        SCAN_SET_VALID(g_scanStatus.scanState);
        SCAN_SET_DISPLAY(g_scanStatus.scanState);
        g_scanStatus.displayIdx = 0;
    } else {
        WDRV_DBG_INFORM_PRINT(("Scan failed, status = %d\r\n", status));
    }
}

static SYS_MODULE_OBJ WDRV_WILC1000_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT *const init)
{
    const TCPIP_MAC_MODULE_CTRL *const stackData = ((TCPIP_MAC_INIT *)init)->macControl;
    WDRV_HOOKS hooks;

	if (isWdrvExtReady())
		return (SYS_MODULE_OBJ)&wdrv_wilc1000_dcpt;

	WDRV_ASSERT((stackData != NULL), "stackData is null");

	// already initialized, have a client connected
	if (wdrv_wilc1000_dcpt.isOpen)
		return (SYS_MODULE_OBJ)&wdrv_wilc1000_dcpt;

    /* initialize g_wdrv_priv, intentionally exclude initConn */
    g_wdrv_priv.isDriverOpen = false;
    g_wdrv_priv.updateMacAddressRequired = false;
    g_wdrv_priv.isScanDone = false;
    g_wdrv_priv.isDisconnectRequested = true; // allow module to stay idle if it doesn't call WDRV_Connect()
    WDRV_SEM_INIT(&g_wdrv_priv.disconnectDoneSync);

    s_dataQueueLock = NULL;
    s_rxFifoLock = NULL;
    s_multicastFilterLock = NULL;
    WDRV_MUTEX_CREATE(&s_dataQueueLock);
    WDRV_MUTEX_CREATE(&s_rxFifoLock);
    WDRV_MUTEX_CREATE(&s_multicastFilterLock);
    memset(g_wdrv_priv.macAddr, 0, sizeof(uint8_t) * 6);

    s_stackData = *stackData;
    s_pktAllocF = (TCPIP_MAC_PKT_AllocF)stackData->pktAllocF;
    s_pktFreeF = (TCPIP_MAC_PKT_FreeF)stackData->pktFreeF;
    s_pktAckF = (TCPIP_MAC_PKT_AckF)stackData->pktAckF;

    if (stackData->moduleId != TCPIP_MODULE_MAC_WILC1000)
        return SYS_MODULE_OBJ_INVALID; // only type WILC1000 is supported

    WDRV_TrafficEventInit(stackData->eventF, stackData->eventParam);

    InitRxBuffer();
    WDRV_AllEventClear();

#if TCPIP_STACK_COMMANDS_WIFI_ENABLE
    WDRV_CLI_Init();
#endif

    hooks.frame_received = PushFrameToFifo;
    hooks.get_rx_buf = GetAvailRxBuf;
    hooks.RFReady = RfReadyHandler;
    hooks.ScanDone = ScanDoneHandler;
    hooks.ConnectEvent = ConnectEventCB;
    hooks.WPSDone = WPSDoneCB;
    hooks.isOtaFwUpdateRequested = g_wdrv_priv.isOtaFwUpdateRequested;
    if (hooks.isOtaFwUpdateRequested)
        hooks.fwOtaServerUrl = g_wdrv_priv.fwOtaServerUrl;

	hooks.isEthMode = true;

#ifdef FW_UPDATE_OVER_SERIAL_ENABLED
	hooks.isSerialFwUpdateRequested = true;
#else
	hooks.isSerialFwUpdateRequested = false;
#endif

    WDRV_EXT_Initialize(&hooks, false);

    if (!WDRV_CONFIG_Load())
        return SYS_MODULE_OBJ_INVALID;

    wdrv_wilc1000_dcpt.isInit = true;
    wdrv_wilc1000_dcpt.sysStat = SYS_STATUS_BUSY;

    return (SYS_MODULE_OBJ)&wdrv_wilc1000_dcpt;
}

static void WDRV_WILC1000_Tasks(SYS_MODULE_OBJ object)
{
    TCPIP_MAC_RES res = TCPIP_MAC_RES_PENDING;
    WDRV_WILC1000_DCPT *pDcpt = (WDRV_WILC1000_DCPT *)object;

    switch (pDcpt->sysStat) {
    case SYS_STATUS_UNINITIALIZED:
        break;
    case SYS_STATUS_BUSY:
		if (g_wdrv_priv.updateMacAddressRequired) {
			memcpy((uint8_t *)(s_stackData.ifPhyAddress.v), g_wdrv_priv.macAddr, 6);
			WDRV_DBG_INFORM_PRINT(("Module MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
					s_stackData.ifPhyAddress.v[0], s_stackData.ifPhyAddress.v[1],
					s_stackData.ifPhyAddress.v[2], s_stackData.ifPhyAddress.v[3],
					s_stackData.ifPhyAddress.v[4], s_stackData.ifPhyAddress.v[5]));
			g_wdrv_priv.updateMacAddressRequired = false;
		}

		if (isWdrvExtReady()) {
			if (g_wdrv_priv.initConn)
				g_initConn = 1;
			res = TCPIP_MAC_RES_OK;
		}

		if (res == TCPIP_MAC_RES_OK) {
			g_wdrv_priv.isDriverOpen = true;
			pDcpt->sysStat = SYS_STATUS_READY;
			WDRV_DBG_TRACE_MESSAGE(("WILC1000: Initialization is complete\r\n"));
		} else if (res != TCPIP_MAC_RES_PENDING) {
			WDRV_CONFIG_Save();
			WDRV_AllEventClear();
			WDRV_TrafficEventDeinit();
			pDcpt->sysStat = SYS_STATUS_ERROR;
		}
        break;
    case SYS_STATUS_READY:
    case SYS_STATUS_READY_EXTENDED:
        if (isEventPending())
            WDRV_PendingEventProcess();
        break;
    default: // SYS_STATUS_ERROR
        WDRV_ASSERT(false, "Should never happen");
        break;
    }
}

static void WDRV_WILC1000_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT *const init)
{
    /* unsupported */
}

static void WDRV_WILC1000_Deinitialize(SYS_MODULE_OBJ object)
{
	WDRV_WILC1000_DCPT *pDcpt = (WDRV_WILC1000_DCPT *)object;

	if (!isWdrvExtReady())
		return;

	pDcpt->isInit = false;
	pDcpt->sysStat = SYS_STATUS_UNINITIALIZED;

	WDRV_CONFIG_Save();
	WDRV_AllEventClear();
	WDRV_TrafficEventDeinit();
	DeInitRxBuffer();
	WDRV_EXT_Deinitialize();
	WDRV_MUTEX_DELETE(&s_dataQueueLock);
	WDRV_MUTEX_DELETE(&s_rxFifoLock);
	WDRV_MUTEX_DELETE(&s_multicastFilterLock);
	WDRV_SEM_DEINIT(&g_wdrv_priv.disconnectDoneSync);
}

static SYS_STATUS WDRV_WILC1000_Status(SYS_MODULE_OBJ object)
{
    return ((WDRV_WILC1000_DCPT *)object)->sysStat;
}

static size_t WDRV_WILC1000_GetConfig(TCPIP_MODULE_MAC_ID modId, void *configBuff, size_t buffSize, size_t *pConfigSize)
{
    /* unsupported */
    return 0;
}

static TCPIP_MAC_HANDLE WDRV_WILC1000_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent)
{
    if (!wdrv_wilc1000_dcpt.isOpen) {
        MulticastFilter_Initialize();
		wdrv_wilc1000_dcpt.isOpen = true;
    }
    return (TCPIP_MAC_HANDLE)&wdrv_wilc1000_dcpt;
}

bool isClientCacheUpdated(bool *connected, uint8_t *mac)
{
    if (g_wdrv_priv.clientCache.updated) {
        int i;
        for (i = 0; i <  WDRV_MAX_CLIENT_TABLE_SLOTS; ++i) {
            if (g_wdrv_priv.clientCache.updated & 1 << i) {
                *connected = g_wdrv_priv.clientCache.bitMap & 1 << i ? true: false;
                memcpy(mac, g_wdrv_priv.clientCache.mac[i].addr, 6 * sizeof(uint8_t));
                g_wdrv_priv.clientCache.updated &= ~(i << i);
                return true;
            }
        }
    }
    return false;
}

void ClientCacheUpdate(bool connected, uint8_t const *const mac)
{
    int i;
    int idx = 0;

    if (connected) {
        /* Check if the MAC address is already in the table. If so, we just update timestamp and return. */
        if (g_wdrv_priv.clientCache.bitMap) {
            for (i = 0; i <  WDRV_MAX_CLIENT_TABLE_SLOTS; ++i) {
                if (g_wdrv_priv.clientCache.bitMap & 1 << i) {
                    if (!memcmp(g_wdrv_priv.clientCache.mac[i].addr, mac, 6)) {
                        g_wdrv_priv.clientCache.mac[i].timeStamp = g_wdrv_priv.clientCache.seqNum++;
                        return;
                    }
                }
            }
        }

        /* Try to find an empty slot in the table. */
        for (i = 0; i <  WDRV_MAX_CLIENT_TABLE_SLOTS; ++i) {
            if (!(g_wdrv_priv.clientCache.bitMap & 1 << i)) {
                idx = i;
                g_wdrv_priv.clientCache.bitMap |= 1 << idx;
                g_wdrv_priv.clientCache.updated |= 1 << idx;
                memcpy(g_wdrv_priv.clientCache.mac[idx].addr, mac, 6);
                g_wdrv_priv.clientCache.mac[idx].timeStamp = g_wdrv_priv.clientCache.seqNum++;
                return;
            }
        }

        /* Cache table is full. Let's kick out the oldest. */
        for (i = 0; i <  WDRV_MAX_CLIENT_TABLE_SLOTS; ++i) {
            uint32_t min = 0;
            if (g_wdrv_priv.clientCache.mac[i].timeStamp >= min) {
                min = g_wdrv_priv.clientCache.mac[i].timeStamp;
                idx = i;
            }
        }
        g_wdrv_priv.clientCache.bitMap |= 1 << idx;
        g_wdrv_priv.clientCache.updated |= 1 << idx;
        memcpy(g_wdrv_priv.clientCache.mac[idx].addr, mac, 6);
        g_wdrv_priv.clientCache.mac[idx].timeStamp = g_wdrv_priv.clientCache.seqNum++;
        return;
    } else {
        /* If the MAC address is in the table, update its status to unconnected. */
        for (i = 0; i <  WDRV_MAX_CLIENT_TABLE_SLOTS; ++i) {
            if (g_wdrv_priv.clientCache.bitMap & 1 << i) {
                if (!memcmp(mac, g_wdrv_priv.clientCache.mac[i].addr, 6)) {
                    g_wdrv_priv.clientCache.bitMap &= ~(1 << i);
                    g_wdrv_priv.clientCache.updated |= 1 << i;
                    return;
                }
            }
        }
    }
}

void WDRV_IsDisconnectRequestedSet(bool bValue)
{
	g_wdrv_priv.isDisconnectRequested = bValue;
}
//DOM-IGNORE-END
