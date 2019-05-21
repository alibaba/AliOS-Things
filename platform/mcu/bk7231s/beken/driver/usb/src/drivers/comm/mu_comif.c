/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2007              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * Top-level communications class driver module
 * $Revision: 1.10 $
 */
#include "include.h"

#include "mu_arch.h"
#include "mu_bits.h"
#include "mu_cdi.h"
#include "mu_descs.h"
#include "mu_diag.h"
#include "mu_list.h"
#include "mu_mem.h"
#include "mu_nci.h"
#include "mu_stdio.h"
#include "class/mu_ccd.h"
#include "mu_comif.h"

#define MUSB_CCD_ETHERNET_RX_QSIZE	4

/* Debug message values for success and error */
#define MUSB_CCD_DIAG_ERROR                     0x1
#define MUSB_CCD_DIAG_SUCCESS                   0x2

/*
 * NOTE: Sharp Zaurus (many models) blacklisted in Linux due to non-conformance
 *
 * candidate devices:
 * Motorola SB4100
 *
 * But it might be possible for the single-interface bulk-only devices
 * to generate a MAC address (terrible!)
 */

/**************************** TYPES *******************************/

struct _MGC_CcdDevice;

/**
 * Ethernet interface instance data
 */
typedef struct
{
    struct _MGC_CcdDevice *pCcdDevice;
    const MUSB_InterfaceDescriptor *pCommIface;
    const MUSB_InterfaceDescriptor *pDataIface;
    const MGC_CdcEthernetDescriptor *pEthernetDescriptor;
    MUSB_DeviceRequest Setup;
    MUSB_ControlIrp ControlIrp;
    MUSB_NciClientHandle NciClientHandle;
    MUSB_NciPortInfo NciPortInfo;
    MUSB_NciConnectionInfo NciCurrentConnectInfo;
    MUSB_pfNciSendComplete pfNciSendComplete;
    MUSB_NciPortServices NciPortServices;
    MUSB_LinkedList LoanedBufferList;
    MUSB_NciRxBuffer *pLoanedBuffer;
    uint32_t dwLoanedBufferOffset;
    uint8_t *pIntrBuffer;
    MUSB_Irp IntrIrp;
    uint8_t *pRxBuffer;
    MUSB_PipePtr hRxPipe;
    MUSB_Irp aRxIrp[MUSB_CCD_ETHERNET_RX_QSIZE];
    MUSB_Irp TxIrp;
    uint32_t dwQueuedRxIrps;
    uint32_t dwCompletedRxIrps;
    uint8_t bTrafficAltSetting;
    uint8_t bTxBusy;
    uint8_t bLocalRxIrp;
    uint8_t bRxBufferIndex;
    uint8_t bRxBufferMax;
    uint8_t bNciAccepted;
    uint8_t bEnabling;
    uint8_t bClosing;
} MGC_NciEthernetIface;

/**
 * Device instance data
 */
typedef struct _MGC_CcdDevice
{
    MUSB_BusHandle          hBus;
    MUSB_Device             *pUsbDevice;
    MUSB_DeviceDriver       *pDriver;
    MUSB_DeviceRequest      Setup;
    MUSB_ControlIrp         ControlIrp;
    MGC_NciEthernetIface   *aEthernetIface;
    uint16_t                awMacAddress[16];
    uint8_t                 bEthernetCount;
    uint8_t                 bEthernetIndex;
} MGC_CcdDevice;

/*************************** FORWARDS *****************************/
#if CFG_SUPPORT_CCD
static uint8_t MGC_CcdEthernetNciInit(MGC_NciEthernetIface *pEthernet,
                                      MGC_CcdDevice *pCcdDevice, const MUSB_InterfaceDescriptor *pCommIface,
                                      const MUSB_InterfaceDescriptor *pDataIface);
static uint8_t MGC_CcdConnect(void *pPrivateData, MUSB_BusHandle hBus,
                              MUSB_Device *pUsbDevice, const uint8_t *pPeripheralList);
static void MGC_CcdDisconnect(void *pPrivateData, MUSB_BusHandle hBus,
                              MUSB_Device *pUsbDevice);
static void MGC_CcdDestroy(MGC_CcdDevice *pCcdDevice);

static uint32_t MGC_CcdNciSendPacketComplete(void *pContext, MUSB_Irp *pIrp);
static uint32_t MGC_CcdNciRxPacketComplete(void *pContext, MUSB_Irp *pIrp);
static uint32_t MGC_CcdNciRxIntrComplete(void *pContext, MUSB_Irp *pIrp);

static uint32_t MGC_CcdEtherGetMacAddress(MGC_CcdDevice *pCcdDevice);
static uint32_t MGC_CcdEtherGetMacAddressCallback(void *pContext,
        MUSB_ControlIrp *pControlIrp);

static uint32_t MGC_CcdConfigureDevice(MGC_CcdDevice *pCcdDevice);
static uint32_t MGC_CcdSetConfigCallback(void *pContext,
        MUSB_ControlIrp *pControlIrp);

static MUSB_NciStatus MGC_CcdNciSendPacket(MUSB_NciPortHandle hPort,
        uint8_t *pBuffer, uint16_t wLength, MUSB_pfNciSendComplete pfSendComplete);
static MUSB_NciStatus MGC_CcdNciLoanRxBuffer(MUSB_NciPortHandle hPort,
        MUSB_NciRxBuffer *pBuffer);
static uint32_t MGC_CcdSetEnabledCallback(void *pContext,
        MUSB_ControlIrp *pControlIrp);
static MUSB_NciStatus MGC_CcdNciSetEnabled(MUSB_NciPortHandle hPort,
        uint8_t bEnabled);
static void MGC_CcdNciStartRx(MGC_NciEthernetIface *pEthernet);

/**************************** GLOBALS *****************************/

static const uint8_t MGC_aCcdPeripheralList1[] =
{
    /* Ethernet */
    MUSB_TARGET_CLASS, MUSB_CLASS_PER_INTERFACE,
    MUSB_TARGET_INTERFACE, 0,
    MUSB_TARGET_CLASS, MUSB_CLASS_COMM,
    MUSB_TARGET_SUBCLASS, MGC_CCD_SUBCLASS_COMM_ETHERNET,
    MUSB_TARGET_ACCEPT,
    0 /* placeholder for driver index */
};

static const uint8_t MGC_aCcdPeripheralList2[] =
{
    /* DOCSIS(?) D-Link DCM-202 */
    MUSB_TARGET_CLASS, MUSB_CLASS_COMM,
    MUSB_TARGET_INTERFACE, 0,
    MUSB_TARGET_CLASS, MUSB_CLASS_COMM,
    MUSB_TARGET_SUBCLASS, MGC_CCD_SUBCLASS_COMM_ABSTRACT,
    MUSB_TARGET_ACCEPT,
    0 /* placeholder for driver index */
};

/* SMC something is VID=0b95, PID=1720 */

/** Communications class driver */
MUSB_DeviceDriver MGC_CommDeviceDriver =
{
    NULL,
    0,
    NULL,
    MGC_CcdConnect,
    MGC_CcdDisconnect,
    NULL,
    NULL,
};

static const uint8_t MGC_aCcdGetStringData[] =
{
    (uint8_t)(MUSB_DIR_IN | MUSB_TYPE_STANDARD | MUSB_RECIP_DEVICE),
    MUSB_REQ_GET_DESCRIPTOR,
    0, /* index (set in run-time copy) */
    MUSB_DT_STRING,
    0, 0,
    16, 0,   /* wLength */
};

/*************************** FUNCTIONS ****************************/

/* public API (driver/app glue) implementation */
uint16_t MUSB_FillCommPeripheralList(uint8_t bDriverIndex, uint8_t *pList,
                                     uint16_t wListRemain)
{
    uint16_t wCount;
    uint16_t wResult = 0;

    wCount = MUSB_MIN((uint16_t)sizeof(MGC_aCcdPeripheralList1),
                      wListRemain - wResult);
    MUSB_MemCopy((void *)(pList + wResult), MGC_aCcdPeripheralList1, wCount);
    wResult += wCount;
    pList[wResult - 1] = bDriverIndex;

    wCount = MUSB_MIN((uint16_t)sizeof(MGC_aCcdPeripheralList2),
                      wListRemain - wResult);
    MUSB_MemCopy((void *)(pList + wResult), MGC_aCcdPeripheralList2, wCount);
    wResult += wCount;
    pList[wResult - 1] = bDriverIndex;

    return wResult;
}

/* public API (driver/app glue) implementation */
MUSB_DeviceDriver *MUSB_GetCommClassDriver()
{
    return &MGC_CommDeviceDriver;
}

/** Callback for packet send IRP complete - inform NCI client if possible */
static uint32_t MGC_CcdNciSendPacketComplete(void *pContext, MUSB_Irp *pIrp)
{
    MGC_NciEthernetIface *pEthernet = (MGC_NciEthernetIface *)pContext;

    MUSB_PrintLine("send packet complete");
    pEthernet->bTxBusy = FALSE;
    if(pEthernet->pfNciSendComplete)
    {
        pEthernet->pfNciSendComplete(pEthernet->NciClientHandle,
                                     pIrp->dwActualLength);
    }

    return TRUE;
}

/* NCI service implementation */
static MUSB_NciStatus MGC_CcdNciSendPacket(MUSB_NciPortHandle hPort,
        uint8_t *pBuffer, uint16_t wLength, MUSB_pfNciSendComplete pfSendComplete)
{
    uint32_t dwStatus;
    MUSB_NciStatus Status = MUSB_NCI_ERROR_BUSY;
    MGC_NciEthernetIface *pEthernet = (MGC_NciEthernetIface *)hPort;

    if(!pEthernet->bTxBusy)
    {
        pEthernet->bTxBusy = TRUE;
        pEthernet->pfNciSendComplete = pfSendComplete;
        pEthernet->TxIrp.pBuffer = pBuffer;
        pEthernet->TxIrp.dwLength = wLength;
        pEthernet->TxIrp.dwActualLength = 0;
        Status = MUSB_NCI_SUCCESS;
        dwStatus = MUSB_StartTransfer(&(pEthernet->TxIrp));
        if(MUSB_STATUS_OK != dwStatus)
        {
            Status = MUSB_NCI_ERROR_TRANSFER;
        }
    }

    return(Status);
}

/* NCI service implementation */
static MUSB_NciStatus MGC_CcdNciLoanRxBuffer(MUSB_NciPortHandle hPort,
        MUSB_NciRxBuffer *pBuffer)
{
    MUSB_NciStatus Status = MUSB_NCI_NO_MEMORY;
    MGC_NciEthernetIface *pEthernet = (MGC_NciEthernetIface *)hPort;

    if(!pEthernet->bClosing &&
            (pBuffer->dwLength >= pEthernet->NciPortInfo.wMaxSegmentSize))
    {
        if(MUSB_ListAppendItem(&(pEthernet->LoanedBufferList), pBuffer, 0))
        {
            Status = MUSB_NCI_SUCCESS;
        }
    }
    return(Status);
}

static uint32_t MGC_CcdSetEnabledCallback(void *pContext,
        MUSB_ControlIrp *pControlIrp)
{
    MGC_NciEthernetIface *pEthernet = (MGC_NciEthernetIface *)pContext;

    if(!pControlIrp->dwStatus && pEthernet->bEnabling &&
            !pEthernet->IntrIrp.hPipe)
    {
        MGC_CcdNciStartRx(pEthernet);
    }

    return TRUE;
}

/* NCI service implementation */
static MUSB_NciStatus MGC_CcdNciSetEnabled(MUSB_NciPortHandle hPort,
        uint8_t bEnabled)
{
    MUSB_DeviceRequest      *pSetup;
    MUSB_ControlIrp         *pControlIrp;
    uint32_t                 dwStatus;
    MUSB_NciStatus Status = MUSB_NCI_DEVICE_DISCONNECTED;
    MGC_NciEthernetIface *pEthernet = (MGC_NciEthernetIface *)hPort;

    pSetup          = &(pEthernet->Setup);
    pControlIrp     = &(pEthernet->ControlIrp);

    if(!pEthernet->bClosing && !pEthernet->bTrafficAltSetting)
    {
        MGC_CcdNciStartRx(pEthernet);
        Status = MUSB_NCI_SUCCESS;
    }
    else if(!pEthernet->bClosing)
    {
        pEthernet->bEnabling = bEnabled;
        /** Prepare the Setup Packet for sending Set Config Request */
        pSetup->bmRequestType = (uint8_t)(MUSB_DIR_OUT | MUSB_TYPE_STANDARD | MUSB_RECIP_DEVICE);
        pSetup->bRequest      = (uint8_t)MUSB_REQ_SET_INTERFACE;
        pSetup->wValue        = bEnabled ? pEthernet->bTrafficAltSetting : (uint16_t)0;
        pSetup->wIndex        = pEthernet->pDataIface->bInterfaceNumber;
        pSetup->wLength       = (uint16_t)0;

        /** Fill Control Irp */
        pControlIrp->pDevice           = pEthernet->pCcdDevice->pUsbDevice;
        pControlIrp->pOutBuffer        = (uint8_t *)pSetup;
        pControlIrp->dwOutLength       = sizeof(MUSB_DeviceRequest);
        pControlIrp->pInBuffer         = NULL;
        pControlIrp->dwInLength        = 0;
        pControlIrp->dwStatus          = 0;
        pControlIrp->dwActualOutLength = 0;
        pControlIrp->dwActualInLength  = 0;
        pControlIrp->pfIrpComplete     = MGC_CcdSetEnabledCallback;
        pControlIrp->pCompleteParam    = (void *)pEthernet;

        dwStatus = MUSB_StartControlTransfer(
                       pEthernet->pCcdDevice->pUsbDevice->pPort,
                       pControlIrp);
        if (dwStatus)
        {
            /* Log an Error and return state */
            MUSB_DIAG1(MUSB_CCD_DIAG_ERROR,
                       "CCD Error: Set Configuration Request failed, dwStatus: ",
                       dwStatus, 16, FALSE);
            MUSB_RejectDevice(pEthernet->pCcdDevice->hBus,
                              pEthernet->pCcdDevice->pUsbDevice);
            Status = MUSB_NCI_ERROR_TRANSFER;
        }
        else
        {
            Status = MUSB_NCI_SUCCESS;
        }
    }
    return(Status);
}

static void MGC_CcdNciStartRx(MGC_NciEthernetIface *pEthernet)
{
    MUSB_Irp *pIrp;
    uint_fast16_t wCount;
    uint32_t dwStatus;

    if(pEthernet->dwCompletedRxIrps == pEthernet->dwQueuedRxIrps)
    {
        /* we caught up; start another IRP */
        pEthernet->dwQueuedRxIrps++;

        pEthernet->bRxBufferIndex++;
        if(pEthernet->bRxBufferIndex >= MUSB_CCD_ETHERNET_RX_QSIZE)
        {
            pEthernet->bRxBufferIndex = 0;
        }
        pIrp = &(pEthernet->aRxIrp[pEthernet->bRxBufferIndex]);

        /* see if we can use a loaned buffer */
        if(!pEthernet->pLoanedBuffer)
        {
            wCount = MUSB_ListLength(&(pEthernet->LoanedBufferList));
            if(wCount)
            {
                /* get and remove next one */
                pEthernet->pLoanedBuffer =
                    (MUSB_NciRxBuffer *)MUSB_ListFindItem(
                        &(pEthernet->LoanedBufferList), 0);
                MUSB_ListRemoveItem(&(pEthernet->LoanedBufferList),
                                    pEthernet->pLoanedBuffer);
                /* set up */
                pEthernet->bRxBufferIndex = 0;
                pEthernet->bRxBufferMax =
                    pEthernet->pLoanedBuffer->dwLength /
                    pEthernet->NciPortInfo.wMaxSegmentSize;
            }
        }

        /* set up IRP's buffer */
        if(pEthernet->pLoanedBuffer)
        {
            pEthernet->bLocalRxIrp = FALSE;
            pIrp->pBuffer = pEthernet->pLoanedBuffer->pBuffer +
                            (pEthernet->bRxBufferIndex * pEthernet->NciPortInfo.wMaxSegmentSize);
        }
        else
        {
            pEthernet->bLocalRxIrp = TRUE;
            pIrp->pBuffer = pEthernet->pRxBuffer +
                            (pEthernet->bRxBufferIndex * pEthernet->NciPortInfo.wMaxSegmentSize);
        }

        /* start IRP */
        dwStatus = MUSB_StartTransfer(pIrp);
        if(dwStatus)
        {
            MUSB_DIAG1(MUSB_CCD_DIAG_ERROR,
                       "CCD Error: StartTransfer(Rx) failed, dwStatus: ",
                       dwStatus, 16, FALSE);
        }
    }
}

static uint32_t MGC_CcdNciRxPacketComplete(void *pContext, MUSB_Irp *pIrp)
{
    MUSB_NciRxBuffer *pLoanedBuffer;
    MGC_NciEthernetIface *pEthernet = (MGC_NciEthernetIface *)pContext;

    pEthernet->dwCompletedRxIrps++;
    MUSB_NciPortPacketReceived(pEthernet->NciClientHandle,
                               pIrp->pBuffer, (uint16_t)pIrp->dwActualLength, TRUE);

    if(pEthernet->pLoanedBuffer)
    {
        /* update loaned buffer used size and check if we have exhausted */
        pLoanedBuffer = pEthernet->pLoanedBuffer;
        pLoanedBuffer->dwUsedSize += pIrp->dwActualLength;
        if(pEthernet->bRxBufferIndex >= (pEthernet->bRxBufferMax - 1))
        {
            /* return loaned buffer */
            pEthernet->pLoanedBuffer = NULL;
            MUSB_NciReturnBuffer(pEthernet->NciClientHandle, pLoanedBuffer);
        }
    }

    /* if no interrupt pipe, ensure another Rx IRP is queued */
    if(!pEthernet->IntrIrp.hPipe)
    {
        MGC_CcdNciStartRx(pEthernet);
    }

    return TRUE;
}

static uint32_t MGC_CcdNciRxIntrComplete(void *pContext, MUSB_Irp *pIrp)
{
    MGC_NciEthernetIface *pEthernet = (MGC_NciEthernetIface *)pContext;
    const uint8_t *pBuffer = pIrp->pBuffer;
    const MUSB_DeviceRequest *pRequest = (const MUSB_DeviceRequest *)pBuffer;

    if(MGC_CCD_REQTYPE_IN == pRequest->bmRequestType)
    {
        switch(pRequest->bRequest)
        {
        case MGC_CCD_NOTIF_NETWORK_CONNECTION:
            if(pRequest->wValue)
            {
                pEthernet->NciCurrentConnectInfo.bConnected = TRUE;
                MUSB_NciPortConnected(pEthernet->NciClientHandle,
                                      &(pEthernet->NciCurrentConnectInfo));
            }
            else
            {
                pEthernet->NciCurrentConnectInfo.bConnected = FALSE;
                MUSB_NciPortDisconnected(pEthernet->NciClientHandle);
            }
            break;
        case MGC_CCD_NOTIF_RESPONSE_AVAILABLE:
            MGC_CcdNciStartRx(pEthernet);
            break;
        case MGC_CCD_NOTIF_CONNECTION_SPEED_CHANGE:
            pEthernet->NciCurrentConnectInfo.dwUpstreamBitRate =
                pBuffer[8] | (pBuffer[9] << 8) |
                (pBuffer[10] << 16) | (pBuffer[10] << 24);
            pEthernet->NciCurrentConnectInfo.dwDownstreamBitRate =
                pBuffer[11] | (pBuffer[12] << 8) |
                (pBuffer[13] << 16) | (pBuffer[14] << 24);
            pEthernet->NciCurrentConnectInfo.bConnected = TRUE;
            MUSB_NciPortConnected(pEthernet->NciClientHandle,
                                  &(pEthernet->NciCurrentConnectInfo));
            break;
        }
    }

    return TRUE;
}

/*
 * Allocate buffers and open pipes
 * @return TRUE on success; FALSE otherwise
 */
static uint8_t MGC_CcdNciPrepareTraffic(MGC_NciEthernetIface *pEthernet,
                                        const MUSB_InterfaceDescriptor *pIfaceDescriptor)
{
    uint8_t bEnd, bEndCount, bIndex;
    uint32_t dwStatus;
    uint16_t wSize;
    const MUSB_EndpointDescriptor *pEnd;
    MUSB_DeviceEndpoint end;
    MGC_CcdDevice *pCcdDevice = pEthernet->pCcdDevice;
    uint8_t bResult = TRUE;
    uint8_t bGotTx = FALSE;
    uint8_t bGotRx = FALSE;
    uint8_t bGotIntr = FALSE;

    /* find suitable endpoint */
    bEndCount = 0;
    for(bEnd = 0; bEnd < pIfaceDescriptor->bNumEndpoints; bEnd++)
    {
        pEnd = MUSB_FindEndpointDescriptor(
                   pEthernet->pCcdDevice->pUsbDevice->pCurrentConfiguration,
                   pIfaceDescriptor, bEnd);
        if(MUSB_ENDPOINT_XFER_BULK == (pEnd->bmAttributes & MUSB_ENDPOINT_XFERTYPE_MASK))
        {
            bEndCount++;
            end.pDevice = pCcdDevice->pUsbDevice;
            MUSB_MemCopy(&(end.UsbDescriptor), pEnd,
                         sizeof(end.UsbDescriptor));
            end.wNakLimit = 0xffff;
            if(!bGotRx && (MUSB_ENDPOINT_DIR_MASK & pEnd->bEndpointAddress))
            {
                /* Rx data pipe */
                bGotRx = TRUE;
                pEthernet->hRxPipe = MUSB_OpenPipe(
                                         pCcdDevice->hBus, &end, NULL);
                if(pEthernet->hRxPipe)
                {
                    for(bIndex = 0; bIndex < MUSB_CCD_ETHERNET_RX_QSIZE;
                            bIndex++)
                    {
                        pEthernet->aRxIrp[bIndex].hPipe = pEthernet->hRxPipe;
                        pEthernet->aRxIrp[bIndex].dwLength =
                            pEthernet->NciPortInfo.wMaxSegmentSize;
                        pEthernet->aRxIrp[bIndex].pfIrpComplete =
                            MGC_CcdNciRxPacketComplete;
                        pEthernet->aRxIrp[bIndex].pCompleteParam = pEthernet;
                    }
                }
                else
                {
                    MUSB_DIAG1(1, "CCD: pipe open error on endpoint ",
                               pEnd->bEndpointAddress, 10, 0);
                    bResult = FALSE;
                }
            }
            else if(!bGotTx)
            {
                /* Tx data pipe */
                bGotTx = TRUE;
                pEthernet->TxIrp.hPipe = MUSB_OpenPipe(
                                             pCcdDevice->hBus, &end, NULL);
                if(!pEthernet->TxIrp.hPipe)
                {
                    MUSB_DIAG1(1, "CCD: pipe open error on endpoint ",
                               pEnd->bEndpointAddress, 10, 0);
                    bResult = FALSE;
                }
                else
                {
                    pEthernet->TxIrp.pfIrpComplete = MGC_CcdNciSendPacketComplete;
                    pEthernet->TxIrp.pCompleteParam = pEthernet;
                }
            }
        }
        else if(!bGotIntr && (MUSB_ENDPOINT_DIR_MASK & pEnd->bEndpointAddress) &&
                (MUSB_ENDPOINT_XFER_INT == (pEnd->bmAttributes & MUSB_ENDPOINT_XFERTYPE_MASK)))
        {
            /* interrupt Rx pipe */
            bGotIntr = TRUE;
            bEndCount++;
            end.pDevice = pCcdDevice->pUsbDevice;
            MUSB_MemCopy(&(end.UsbDescriptor), pEnd,
                         sizeof(end.UsbDescriptor));
            end.wNakLimit = 0xffff;
            pEthernet->IntrIrp.hPipe = MUSB_OpenPipe(
                                           pCcdDevice->hBus, &end, NULL);
            if(!pEthernet->IntrIrp.hPipe)
            {
                bResult = FALSE;
                MUSB_DIAG1(1, "CCD: pipe open error on endpoint ",
                           pEnd->bEndpointAddress, 10, 0);
            }
            else
            {
                /* allocate pEthernet->pIntrBuffer */
                wSize = MUSB_SWAP16(pEnd->wMaxPacketSize);
                pEthernet->pIntrBuffer = (uint8_t *)MUSB_MemAlloc(wSize);
                if(pEthernet->pIntrBuffer)
                {
                    pEthernet->IntrIrp.pBuffer = pEthernet->pIntrBuffer;
                    pEthernet->IntrIrp.dwLength = pEnd->wMaxPacketSize;
                    pEthernet->IntrIrp.pfIrpComplete = MGC_CcdNciRxIntrComplete;
                    pEthernet->IntrIrp.pCompleteParam = pEthernet;
                    dwStatus = MUSB_StartTransfer(&(pEthernet->IntrIrp));
                    if (dwStatus)
                    {
                        /* warn on transfer start error */
                        bResult = FALSE;
                        MUSB_DIAG1(1, "CCD: MUSB_StartTransfer() status=",
                                   dwStatus, 16, 0);
                    }
                }
                else
                {
                    bResult = FALSE;
                    MUSB_DIAG1(1, "CCD: failed to alloc intr buffer size=0x",
                               wSize, 16, 0);
                }
            }
        }
    }
    if(bEndCount < 2)
    {
        bResult = FALSE;
    }

    return(bResult);
}

/**
 * Perform NCI-side init for ethernet
 */
static uint8_t MGC_CcdEthernetNciInit(MGC_NciEthernetIface *pEthernet,
                                      MGC_CcdDevice *pCcdDevice, const MUSB_InterfaceDescriptor *pCommIface,
                                      const MUSB_InterfaceDescriptor *pDataIface)
{
    uint8_t bIndex;
    const uint8_t *pDescriptor;
    const MUSB_InterfaceDescriptor *pTrafficIface = NULL;
    MUSB_Device *pUsbDevice = pCcdDevice->pUsbDevice;
    MUSB_ConfigurationDescriptor *pConfig = pUsbDevice->pCurrentConfiguration;
    uint16_t wMaxSegmentSize = 0;
    uint8_t bResult = FALSE;

    pEthernet->pCcdDevice = pCcdDevice;
    pEthernet->pCommIface = pCommIface;
    pEthernet->pDataIface = pDataIface;
    MUSB_ListInit(&(pEthernet->LoanedBufferList));

    /* try to find functional descriptor */
    bIndex = 0;
    do
    {
        pDescriptor = MUSB_FindDescriptorInBlock(pConfig,
                      (const uint8_t *)pEthernet->pCommIface,
                      MGC_CCD_FUNC_DESC_TYPE_CS_INTERFACE, bIndex++);
        if(pDescriptor)
        {
            switch(pDescriptor[2])
            {
            case MGC_CCD_FUNC_DESC_SUBTYPE_ETHERNET:
                pEthernet->pEthernetDescriptor =
                    (const MGC_CdcEthernetDescriptor *)pDescriptor;
                wMaxSegmentSize =
                    pEthernet->pEthernetDescriptor->wMaxSegmentSize;
                break;
            }
        }
    }
    while(pDescriptor);
    if(!wMaxSegmentSize)
    {
        wMaxSegmentSize = 1514;
        pEthernet->NciCurrentConnectInfo.bConnected = TRUE;
    }

    /* get traffic interface for compliant devices and the nutso ones too */
    if(pDataIface)
    {
        /* should always be first alternate */
        pEthernet->bTrafficAltSetting = 1;
        pTrafficIface = MUSB_FindInterfaceDescriptor(pConfig,
                        pDataIface->bInterfaceNumber, pEthernet->bTrafficAltSetting);
    }
    if(!pTrafficIface)
    {
        pEthernet->bTrafficAltSetting = 0;
        pTrafficIface = pDataIface;
    }
    if(!pTrafficIface)
    {
        pTrafficIface = pCommIface;
        wMaxSegmentSize = 1514;
    }

    if(wMaxSegmentSize && pTrafficIface)
    {
        /* fill NciPortInfo except abMacAddress (requires another request) */
        pEthernet->NciPortInfo.bBusAddress = pUsbDevice->bBusAddress;
        pEthernet->NciPortInfo.wVendorId = pUsbDevice->DeviceDescriptor.idVendor;
        pEthernet->NciPortInfo.wProductId = pUsbDevice->DeviceDescriptor.idProduct;
        pEthernet->NciPortInfo.bcdDevice = pUsbDevice->DeviceDescriptor.bcdDevice;
        pEthernet->NciPortInfo.wMaxSegmentSize = wMaxSegmentSize;

        /* fill NciPortServices */
        pEthernet->NciPortServices.hPort = pEthernet;
        pEthernet->NciPortServices.pfSendPacket = MGC_CcdNciSendPacket;
        pEthernet->NciPortServices.pfLoanRxBuffer = MGC_CcdNciLoanRxBuffer;
        pEthernet->NciPortServices.pfSetEnabled = MGC_CcdNciSetEnabled;

        /* allocate Rx buffer */
        pEthernet->pRxBuffer = (uint8_t *)MUSB_MemAlloc(
                                   MUSB_CCD_ETHERNET_RX_QSIZE * wMaxSegmentSize);
        if(pEthernet->pRxBuffer)
        {
            bResult = TRUE;
            for(bIndex = 0; bIndex < MUSB_CCD_ETHERNET_RX_QSIZE;
                    bIndex++)
            {
                pEthernet->aRxIrp[bIndex].pBuffer =
                    pEthernet->pRxBuffer + (bIndex * wMaxSegmentSize);
            }
        }
        else
        {
            MUSB_DIAG_STRING(1, "CCD: failed to allocate Rx buffer");
        }
        if(bResult)
        {
            bResult = MGC_CcdNciPrepareTraffic(pEthernet, pTrafficIface);
        }
    }

    return bResult;
}

static void MGC_CcdEthernetNciRelease(MGC_NciEthernetIface *pEthernet)
{
    uint_fast16_t wIndex;
    uint_fast16_t wCount;
    MUSB_NciRxBuffer *pBuffer;

    pEthernet->bClosing = TRUE;

    if(pEthernet->bNciAccepted)
    {
        /* inform NCI client */
        MUSB_NciPortRemoved(pEthernet->NciClientHandle);
    }

    /* disable traffic */
    MGC_CcdNciSetEnabled(pEthernet, FALSE);

    /* cancel IRPs, close pipes, free buffers */
    if(pEthernet->IntrIrp.hPipe)
    {
        MUSB_CancelTransfer(&(pEthernet->IntrIrp));
        MUSB_ClosePipe(pEthernet->IntrIrp.hPipe);
        pEthernet->IntrIrp.hPipe = NULL;
        MUSB_MemFree(pEthernet->pIntrBuffer);
        pEthernet->pIntrBuffer = NULL;
    }
    if(pEthernet->TxIrp.hPipe)
    {
        if(pEthernet->bTxBusy)
        {
            MUSB_CancelTransfer(&(pEthernet->TxIrp));
            pEthernet->bTxBusy = FALSE;
        }
        MUSB_ClosePipe(pEthernet->TxIrp.hPipe);
        pEthernet->TxIrp.hPipe = NULL;
    }
    if(pEthernet->hRxPipe)
    {
        for(wIndex = 0; wIndex < (uint_fast16_t)MUSB_CCD_ETHERNET_RX_QSIZE;
                wIndex++)
        {
            MUSB_CancelTransfer(&(pEthernet->aRxIrp[wIndex]));
        }
        MUSB_ClosePipe(pEthernet->hRxPipe);
        pEthernet->hRxPipe = NULL;
        MUSB_MemFree(pEthernet->pRxBuffer);
        pEthernet->pRxBuffer = NULL;
    }

    /* return any loaned buffers */
    wCount = MUSB_ListLength(&(pEthernet->LoanedBufferList));
    for(wIndex = 0; wIndex < wCount; wIndex++)
    {
        pBuffer = (MUSB_NciRxBuffer *)MUSB_ListFindItem(
                      &(pEthernet->LoanedBufferList), 0);
        MUSB_NciReturnBuffer(pEthernet->NciClientHandle, pBuffer);
        MUSB_ListRemoveItem(&(pEthernet->LoanedBufferList), pBuffer);
    }
}

static uint32_t MGC_CcdEtherGetMacAddress(MGC_CcdDevice *pCcdDevice)
{
    MUSB_DeviceRequest      *pSetup;
    MUSB_ControlIrp         *pControlIrp;
    uint32_t                 dwStatus;
    uint8_t                *pSetupBuffer;
    MGC_NciEthernetIface *pEthernet;

    pSetup       = &(pCcdDevice->Setup);
    pSetupBuffer = (uint8_t *)pSetup;
    pControlIrp  = &(pCcdDevice->ControlIrp);
    pEthernet = &(pCcdDevice->aEthernetIface[pCcdDevice->bEthernetIndex]);

    MUSB_MemCopy(pSetup, MGC_aCcdGetStringData,
                 sizeof(MGC_aCcdGetStringData));
    pSetupBuffer[2] = pEthernet->pEthernetDescriptor->iMACAddress;

    /** Fill Control Irp */
    pControlIrp->pDevice           = pCcdDevice->pUsbDevice;
    pControlIrp->pOutBuffer        = (uint8_t *)pSetup;
    pControlIrp->dwOutLength       = sizeof(MUSB_DeviceRequest);
    pControlIrp->pInBuffer         = (uint8_t *)pCcdDevice->awMacAddress;
    pControlIrp->dwInLength        = sizeof(pCcdDevice->awMacAddress);
    pControlIrp->dwStatus          = 0;
    pControlIrp->dwActualOutLength = 0;
    pControlIrp->dwActualInLength  = 0;
    pControlIrp->pfIrpComplete     = MGC_CcdEtherGetMacAddressCallback;
    pControlIrp->pCompleteParam    = (void *)pCcdDevice;

    dwStatus = MUSB_StartControlTransfer(pCcdDevice->pUsbDevice->pPort,
                                         pControlIrp);
    if (dwStatus)
    {
        /* Log an Error and return state */
        MUSB_DIAG1(MUSB_CCD_DIAG_ERROR,
                   "CCD Error: Get MAC address request failed, dwStatus: ",
                   dwStatus, 16, FALSE);
    }
    return (dwStatus);
}

/** Completion of the get-string-descriptor for the MAC address */
static uint32_t MGC_CcdEtherGetMacAddressCallback(void *pContext,
        MUSB_ControlIrp *pControlIrp)
{
    uint8_t bIndex, bNibble, bMacByte;
    uint16_t wChar;
    uint32_t dwStatus;
    MGC_NciEthernetIface *pEthernet;
    MGC_CcdDevice *pCcdDevice = (MGC_CcdDevice *)pContext;

    if(MUSB_STATUS_OK != pControlIrp->dwStatus)
    {
        MUSB_DIAG1(MUSB_CCD_DIAG_ERROR,
                   "CCD Error: Set Config Callback Status=",
                   pControlIrp->dwStatus, 10, FALSE);
        MUSB_RejectDevice(pCcdDevice->hBus, pCcdDevice->pUsbDevice);
        return FALSE;
    }

    /* translate MAC address */
    pEthernet = &(pCcdDevice->aEthernetIface[pCcdDevice->bEthernetIndex]);
    for(bIndex = 0; bIndex < 6; bIndex++)
    {
        wChar = MUSB_SWAP16(pCcdDevice->awMacAddress[(bIndex << 1)]);
        bNibble = (wChar >= 'A') ?
                  (uint8_t)(wChar - 'A') : (uint8_t)(wChar - '0');
        bMacByte = bNibble << 4;
        wChar = MUSB_SWAP16(pCcdDevice->awMacAddress[(bIndex << 1) + 1]);
        bNibble = (wChar >= 'A') ?
                  (uint8_t)(wChar - 'A') : (uint8_t)(wChar - '0');
        bMacByte |= bNibble;
        pEthernet->NciPortInfo.abMacAddress[bIndex] = bMacByte;
    }

    /* determine next step */
    pCcdDevice->bEthernetIndex++;
    if(pCcdDevice->bEthernetIndex < pCcdDevice->bEthernetCount)
    {
        /* get next MAC address */
        dwStatus = MGC_CcdEtherGetMacAddress(pCcdDevice);
    }
    else
    {
        /* Configure the device */
        dwStatus = MGC_CcdConfigureDevice(pCcdDevice);
    }
    if(MUSB_STATUS_OK != dwStatus)
    {
        MUSB_RejectDevice(pCcdDevice->hBus, pCcdDevice->pUsbDevice);
        MGC_CcdDestroy(pCcdDevice);
    }

    return TRUE;
}

#if MUSB_DIAG > 1
static const char *MGC_CcdDescSubtypes[] =
{
    "header", "call management", "abstract control mananagement",
    "direct line management", "telephone ringer", "telephone reporting",
    "union", "country selector", "telephone opmodes", "USB terminal",
    "net channel terminal", "protocol unit", "extension unit",
    "multichannel management", "CAPI control management",
    "Ethernet", "ATM", "wireless handset control", "mobile direct line",
    "MDLM detail", "device management", "OBEX", "command set",
    "command set detail", "telephone control"
};
#endif

static void MGC_CcdDumpDesc(MUSB_Device *pUsbDevice)
{
#if MUSB_DIAG > 1
    char number[12];
    char line[80];
    uint8_t bConfig, bIndex, bWhich, bSlaveIndex, bHas;
    const uint8_t *pDesc;
    const MUSB_InterfaceDescriptor *pIfaceDesc;
    const MGC_CdcHeaderDescriptor *pHeaderDesc;
    const MGC_CdcCallMgmtDescriptor *pCallMgmtDesc;
    const MGC_CdcAbstractCntrlDescriptor *pAbstractMgmtDesc;
    const MGC_CdcUnionDescriptor *pUnionDesc;
    const MGC_CdcProtocolUnitDescriptor *pProtocolDesc;
    const MUSB_InterfaceDescriptor *pIface = NULL;
    MUSB_ConfigurationDescriptor *pConfig = NULL;

    for(bConfig = 0;
            bConfig < pUsbDevice->DeviceDescriptor.bNumConfigurations; bConfig++)
    {
        pConfig = (MUSB_ConfigurationDescriptor *)pUsbDevice->apConfigDescriptors[bConfig];
        for(bIndex = 0; bIndex < pConfig->bNumInterfaces; bIndex++)
        {
            pIfaceDesc = MUSB_FindInterfaceDescriptor(pConfig, bIndex, 0);
            bHas = FALSE;
            line[0] = 0;
            MUSB_StringConcat(line, 80, "CS_INTERFACE descriptors for iface ");
            MUSB_Stringize(number, 12, bIndex, 10, 0);
            MUSB_StringConcat(line, 80, number);
            MUSB_StringConcat(line, 80, ":");
            bWhich = 0;
            do
            {
                pDesc = MUSB_FindDescriptorInBlock(pConfig, (const uint8_t *)pIfaceDesc,
                                                   MGC_CCD_FUNC_DESC_TYPE_CS_INTERFACE, bWhich++);
                if(pDesc)
                {
                    if(!bHas)
                    {
                        MUSB_PrintLine(line);
                        bHas = TRUE;
                    }
                    line[0] = 0;
                    MUSB_StringConcat(line, 80, "  Type: ");
                    MUSB_StringConcat(line, 80, MGC_CcdDescSubtypes[pDesc[2]]);
                    MUSB_PrintLine(line);
                    switch(pDesc[2])
                    {
                    case MGC_CCD_FUNC_DESC_SUBTYPE_HEADER:
                        pHeaderDesc = (const MGC_CdcHeaderDescriptor *)pDesc;
                        line[0] = 0;
                        MUSB_StringConcat(line, 80, "    CDC spec revision: ");
                        MUSB_Stringize(number, 12, pHeaderDesc->bcdCDC, 16, 4);
                        MUSB_StringConcat(line, 80, number);
                        MUSB_PrintLine(line);
                        break;
                    case MGC_CCD_FUNC_DESC_SUBTYPE_CALL_MGMT:
                        pCallMgmtDesc = (const MGC_CdcCallMgmtDescriptor *)pDesc;
                        line[0] = 0;
                        MUSB_StringConcat(line, 80, "    Data interface: ");
                        MUSB_Stringize(number, 12, pCallMgmtDesc->bDataInterface, 10, 0);
                        MUSB_StringConcat(line, 80, number);
                        MUSB_PrintLine(line);
                        if(pCallMgmtDesc->bmCapabilities & MGC_M_CCD_CALL_MGMT_HANDLES_CALL_MGMT)
                        {
                            MUSB_PrintLine("    Handles call management");
                        }
                        if(pCallMgmtDesc->bmCapabilities & MGC_M_CCD_CALL_MGMT_CALL_MGMT_DATA_IF)
                        {
                            MUSB_PrintLine("    Accepts call management commands on data interface");
                        }
                        break;
                    case MGC_CCD_FUNC_DESC_SUBTYPE_ABSTRACT_CNTRL_MGMT:
                        pAbstractMgmtDesc = (const MGC_CdcAbstractCntrlDescriptor *)pDesc;
                        if(pAbstractMgmtDesc->bmCapabilities & MGC_M_CCD_ABSTRACT_CNTRL_SUPP_COMM_FEATURE)
                        {
                            MUSB_PrintLine("    Supports *_COMM_FEATURE commands");
                        }
                        if(pAbstractMgmtDesc->bmCapabilities & MGC_M_CCD_ABSTRACT_CNTRL_SUPP_SERIAL_LINE)
                        {
                            MUSB_PrintLine("    Supports line coding commands & serial state notification");
                        }
                        if(pAbstractMgmtDesc->bmCapabilities & MGC_M_CCD_ABSTRACT_CNTRL_SUPP_SEND_BREAK)
                        {
                            MUSB_PrintLine("    Supports SEND_BREAK command");
                        }
                        if(pAbstractMgmtDesc->bmCapabilities & MGC_M_CCD_ABSTRACT_CNTRL_NET_CONNECTION)
                        {
                            MUSB_PrintLine("    Supports NETWORK_CONNECTION notification");
                        }
                        break;
                    case MGC_CCD_FUNC_DESC_SUBTYPE_UNION:
                        pUnionDesc = (const MGC_CdcUnionDescriptor *)pDesc;
                        line[0] = 0;
                        MUSB_StringConcat(line, 80, "    Master interface: ");
                        MUSB_Stringize(number, 12, pUnionDesc->bMasterInterface, 10, 0);
                        MUSB_StringConcat(line, 80, number);
                        MUSB_PrintLine(line);
                        for(bSlaveIndex = sizeof(MGC_CdcUnionDescriptor) - 1;
                                bSlaveIndex < pUnionDesc->bFunctionLength; bSlaveIndex++)
                        {
                            line[0] = 0;
                            MUSB_StringConcat(line, 80, "    Slave interface: ");
                            MUSB_Stringize(number, 12, pDesc[bSlaveIndex], 10, 0);
                            MUSB_StringConcat(line, 80, number);
                            MUSB_PrintLine(line);
                        }
                        break;
                        /*
                        case MGC_CCD_FUNC_DESC_SUBTYPE_COUNTRY_SELECTION:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_TELEPHONE_OPMODES:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_USB_TERMINAL:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_NET_CHANNEL_TERMINAL:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_PROTOCOL_UNIT:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_EXTENSION_UNIT:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_MULTICHANNEL_MGMT:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_CAPI_CNTRL_MGMT:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_ATM:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_WIRELESS_HANDSET_CTL:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_MOBILE_DIRECT_LINE:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_MDLM_DETAIL:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_DEVICE_MGMT:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_OBEX:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_COMMAND_SET:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_COMMAND_SET_DETAIL:
                        case MGC_CCD_FUNC_DESC_SUBTYPE_TELEPHONE_CONTROL:
                        */
                    case MGC_CCD_FUNC_DESC_SUBTYPE_ETHERNET:
                        break;
                    }
                }
            }
            while(pDesc);
        }
    }
#endif
}

static const MUSB_InterfaceDescriptor *MGC_CcdGetDataInterface(
    MUSB_ConfigurationDescriptor *pConfig,
    const MUSB_InterfaceDescriptor *pIfaceDesc)
{
    uint8_t bIndex, bWhich, bIface;
    const uint8_t *pDesc;
    const MGC_CdcCallMgmtDescriptor *pCallMgmtDesc;
    const MUSB_InterfaceDescriptor *pIface = NULL;

    bWhich = 0;
    do
    {
        pDesc = MUSB_FindDescriptorInBlock(pConfig, (const uint8_t *)pIfaceDesc,
                                           MGC_CCD_FUNC_DESC_TYPE_CS_INTERFACE, bWhich++);
        if(pDesc)
        {
            switch(pDesc[2])
            {
            case MGC_CCD_FUNC_DESC_SUBTYPE_CALL_MGMT:
                pCallMgmtDesc = (const MGC_CdcCallMgmtDescriptor *)pDesc;
                bIface = pCallMgmtDesc->bDataInterface;
                pIface = MUSB_FindInterfaceDescriptor(pConfig, bIface, 0);
                if(pIface->bNumEndpoints < 2)
                {
                    pIface = NULL;
                }
                break;
            }
        }
    }
    while(pDesc);

    if(!pIface)
    {
        /* loop through config and find data-class iface */
        for(bIndex = pIfaceDesc->bInterfaceNumber;
                !pIface && (bIndex < pConfig->bNumInterfaces); bIndex++)
        {
            pIface = MUSB_FindInterfaceDescriptor(pConfig, bIndex, 0);
            switch(pIface->bInterfaceClass)
            {
            case MGC_CCD_CLASS_DATA:
                break;
            default:
                pIface = NULL;
            }
        }
    }
    return(pIface);
}

/** This function is called when a CDC/Ethernet device is connected */
static uint8_t MGC_CcdConnect(void *pPrivateData, MUSB_BusHandle hBus,
                              MUSB_Device *pUsbDevice, const uint8_t *pPeripheralList)
{
    uint8_t bConfig, bIndex, bIndex2;
    MGC_CcdDevice *pCcdDevice;
    uint32_t dwSize, dwStatus;
    uint8_t *pBuffer;
    const MUSB_InterfaceDescriptor *apCommIface[16];
    const MUSB_InterfaceDescriptor *apDataIface[16];
    uint8_t abRejections[16];
    uint8_t bEthernetCount = 0;
    uint8_t bEthernetIndex = 0;
    uint8_t bRejectIndex = 0;
    const MUSB_InterfaceDescriptor *pDataIface;
    const MUSB_InterfaceDescriptor *pIface = NULL;
    MUSB_ConfigurationDescriptor *pConfig = NULL;
    uint8_t bStatus = FALSE;

    /* Device is connected */
    MUSB_DIAG_STRING(MUSB_CCD_DIAG_SUCCESS, "CDC/Ethernet Device Connected");

    MGC_CcdDumpDesc(pUsbDevice);

    /* find interfaces with supported subclass/protocol combinations and reject others */
    for(bConfig = 0;
            bConfig < pUsbDevice->DeviceDescriptor.bNumConfigurations; bConfig++)
    {
        pConfig = (MUSB_ConfigurationDescriptor *)pUsbDevice->apConfigDescriptors[bConfig];
        for(bIndex = 0; bIndex < pConfig->bNumInterfaces; bIndex++)
        {
            /* don't need to examine alternates */
            pIface = MUSB_FindInterfaceDescriptor(pConfig, bIndex, 0);
            if(pIface &&
                    ((MUSB_CLASS_COMM == pIface->bInterfaceClass) ||
                     (MUSB_CLASS_COMM == pUsbDevice->DeviceDescriptor.bDeviceClass)))
            {
                switch(pIface->bInterfaceSubClass)
                {
                case 0:
                    apCommIface[bEthernetCount] = NULL;
                    apDataIface[bEthernetCount++] = pIface;
                    break;
                case MGC_CCD_SUBCLASS_COMM_ABSTRACT:
                case MGC_CCD_SUBCLASS_COMM_ETHERNET:
                    pDataIface = MGC_CcdGetDataInterface(pConfig, pIface);
                    if(pDataIface)
                    {
                        apCommIface[bEthernetCount] = pIface;
                        apDataIface[bEthernetCount++] = pDataIface;
                        /* loop trick */
                        bIndex = pDataIface->bInterfaceNumber;
                    }
                    break;
                default:
                    abRejections[bRejectIndex++] = pIface->bInterfaceNumber;
                }
            }
        }
    }
    /* the big exception for the whacko devices */
    if(!bEthernetCount && (0 == pUsbDevice->DeviceDescriptor.bDeviceClass) &&
            (0 == pUsbDevice->DeviceDescriptor.bDeviceSubClass) &&
            (0 == pUsbDevice->DeviceDescriptor.bDeviceProtocol) &&
            (1 == pUsbDevice->DeviceDescriptor.bNumConfigurations))
    {
        pConfig = (MUSB_ConfigurationDescriptor *)
                  pUsbDevice->apConfigDescriptors[0];
        if(1 == pConfig->bNumInterfaces)
        {
            pIface = MUSB_FindInterfaceDescriptor(pConfig, 0, 0);
            if(pIface && (0 == pIface->bInterfaceClass) &&
                    (0 == pIface->bInterfaceSubClass) &&
                    (0 == pIface->bInterfaceProtocol) &&
                    (pIface->bNumEndpoints > 1))
            {
                bRejectIndex = 0;
                bEthernetCount = 1;
                apCommIface[0] = NULL;
                apDataIface[0] = pIface;
            }
        }
    }

    /* if we found interfaces to reject, do it */
    if(bRejectIndex)
    {
        MUSB_RejectInterfaces(hBus, pUsbDevice, abRejections, bRejectIndex);
    }

    /* if we found supported interfaces, init, configure and start */
    if(bEthernetCount &&
            (!pUsbDevice->pCurrentConfiguration || pUsbDevice->pCurrentConfiguration == pConfig))
    {
        pUsbDevice->pCurrentConfiguration = pConfig;
        dwSize = sizeof(MGC_CcdDevice) +
                 (bEthernetCount * sizeof(MGC_NciEthernetIface));
        pBuffer = (uint8_t *)MUSB_MemAlloc(dwSize);
        if(pBuffer)
        {
            MUSB_MemSet((void *)pBuffer, 0, dwSize);
            pCcdDevice = (MGC_CcdDevice *)pBuffer;
            pCcdDevice->pUsbDevice     = pUsbDevice;
            pCcdDevice->hBus           = hBus;
            pCcdDevice->pDriver = &MGC_CommDeviceDriver;
            pCcdDevice->bEthernetCount = bEthernetCount;
            pCcdDevice->aEthernetIface =
                (MGC_NciEthernetIface *)(pBuffer + sizeof(MGC_CcdDevice));
            pUsbDevice->pDriverPrivateData = pCcdDevice;
            bStatus = TRUE;
            for(bEthernetIndex = 0; bEthernetIndex < bEthernetCount;
                    bEthernetIndex++)
            {
                if(bStatus)
                {
                    bStatus = MGC_CcdEthernetNciInit(
                                  &(pCcdDevice->aEthernetIface[bEthernetIndex]),
                                  pCcdDevice, apCommIface[bEthernetIndex],
                                  apDataIface[bEthernetIndex]);
                    if(!bStatus)
                    {
                        /* release any that were already created */
                        for(bIndex2 = 0; bIndex2 < bEthernetIndex; bIndex2++)
                        {
                            MGC_CcdEthernetNciRelease(
                                &(pCcdDevice->aEthernetIface[bIndex2]));
                        }
                    }
                }
            }
            if(bStatus)
            {
                if(!pCcdDevice->aEthernetIface[0].pEthernetDescriptor)
                {
                    /* nothing to do but configure */
                    dwStatus = MGC_CcdConfigureDevice(pCcdDevice);
                    if (MUSB_STATUS_OK == dwStatus)
                    {
                        bStatus = TRUE;
                    }
                }
                else
                {
                    /* get first MAC address */
                    pCcdDevice->bEthernetIndex = 0;
                    dwStatus = MGC_CcdEtherGetMacAddress(pCcdDevice);
                    if (MUSB_STATUS_OK == dwStatus)
                    {
                        bStatus = TRUE;
                    }
                }
            }
            else
            {
                MUSB_MemFree(pCcdDevice);
            }
        }
        else
        {
            MUSB_DIAG_STRING(MUSB_CCD_DIAG_ERROR,
                             "CCD Error: Insufficient memory");
        }
    }
    else
    {
        MUSB_DIAG_STRING(MUSB_CCD_DIAG_ERROR,
                         "CCD Error: No interface has supported subclass/protocol");
    }

    return (bStatus);
}

static void MGC_CcdDestroy(MGC_CcdDevice *pCcdDevice)
{
    uint8_t bEthernetIndex;

    /* disconnect & release NCI ports */
    for(bEthernetIndex = 0; bEthernetIndex < pCcdDevice->bEthernetCount;
            bEthernetIndex++)
    {
        MGC_CcdEthernetNciRelease(
            &(pCcdDevice->aEthernetIface[bEthernetIndex]));
    }

    /* Release the resources held */
    pCcdDevice->pUsbDevice->pDriverPrivateData = NULL;
    MUSB_MemFree(pCcdDevice);
}

/** Disconnect Handler for communications class Driver */
static void MGC_CcdDisconnect(void *pPrivateData, MUSB_BusHandle hBus,
                              MUSB_Device *pUsbDevice)
{
    MGC_CcdDevice *pCcdDevice;

    pCcdDevice = (MGC_CcdDevice *)pUsbDevice->pDriverPrivateData;

    /* Check against the USB device and bus handle */
    if((hBus != pCcdDevice->hBus) || (pUsbDevice != pCcdDevice->pUsbDevice))
    {
        MUSB_DIAG_STRING(MUSB_CCD_DIAG_ERROR,
                         "CCD Error: CCD Device Disconnect Callback");
        MUSB_DIAG_STRING(MUSB_CCD_DIAG_ERROR,
                         "CCD Error: Wrong 'Usb Device' and 'Bus' handles ");
        return;
    }

    MGC_CcdDestroy(pCcdDevice);
    MUSB_DIAG_STRING(MUSB_CCD_DIAG_SUCCESS,
                     "CCD Device Disconnected Successfully");
}

/** Configure the device */
static uint32_t MGC_CcdConfigureDevice(MGC_CcdDevice *pCcdDevice)
{
    MUSB_DeviceRequest      *pSetup;
    MUSB_ControlIrp         *pControlIrp;
    uint32_t                 dwStatus;

    pSetup          = &(pCcdDevice->Setup);
    pControlIrp     = &(pCcdDevice->ControlIrp);

    /** Prepare the Setup Packet for sending Set Config Request */
    pSetup->bmRequestType = (uint8_t)(MUSB_DIR_OUT | MUSB_TYPE_STANDARD | MUSB_RECIP_DEVICE);
    pSetup->bRequest      = (uint8_t)MUSB_REQ_SET_CONFIGURATION;
    pSetup->wValue        = (uint16_t)pCcdDevice->pUsbDevice->pCurrentConfiguration->bConfigurationValue;
    pSetup->wIndex        = (uint16_t)0;
    pSetup->wLength       = (uint16_t)0;

    /** Fill Control Irp */
    pControlIrp->pDevice           = pCcdDevice->pUsbDevice;
    pControlIrp->pOutBuffer        = (uint8_t *)pSetup;
    pControlIrp->dwOutLength       = sizeof(MUSB_DeviceRequest);
    pControlIrp->pInBuffer         = NULL;
    pControlIrp->dwInLength        = 0;
    pControlIrp->dwStatus          = 0;
    pControlIrp->dwActualOutLength = 0;
    pControlIrp->dwActualInLength  = 0;
    pControlIrp->pfIrpComplete     = MGC_CcdSetConfigCallback;
    pControlIrp->pCompleteParam    = (void *)pCcdDevice;

    dwStatus = MUSB_StartControlTransfer(pCcdDevice->pUsbDevice->pPort,
                                         pControlIrp);
    if (dwStatus)
    {
        /* Log an Error and return state */
        MUSB_DIAG1(MUSB_CCD_DIAG_ERROR,
                   "CCD Error: Set Configuration Request failed, dwStatus: ",
                   dwStatus, 16, FALSE);
        MUSB_RejectDevice(pCcdDevice->hBus, pCcdDevice->pUsbDevice);
    }
    return (dwStatus);
}

/** Callback function when device acknowledges set config reqeust. */
static uint32_t MGC_CcdSetConfigCallback(void *pContext,
        MUSB_ControlIrp *pControlIrp)
{
    uint8_t bEthernetIndex;
    MUSB_NciStatus NciStatus;
    MGC_CcdDevice *pCcdDevice;
    MGC_NciEthernetIface *pEthernet;

    pCcdDevice = (MGC_CcdDevice *)pContext;

    if(MUSB_STATUS_OK != pControlIrp->dwStatus)
    {
        MUSB_DIAG1(MUSB_CCD_DIAG_ERROR,
                   "CCD Error: Set Config Callback Status=",
                   pControlIrp->dwStatus, 10, FALSE);
        MUSB_RejectDevice(pCcdDevice->hBus, pCcdDevice->pUsbDevice);
        return FALSE;
    }

    /* Device is connected */
    MUSB_DIAG_STRING(MUSB_CCD_DIAG_SUCCESS,
                     "Communications Device Configured Successfully");

    /* inform NCI client */
    for(bEthernetIndex = 0; bEthernetIndex < pCcdDevice->bEthernetCount;
            bEthernetIndex++)
    {
        pEthernet = &(pCcdDevice->aEthernetIface[bEthernetIndex]);
        /* call NCI implementation */
        NciStatus = MUSB_NciAddPort(&(pEthernet->NciClientHandle),
                                    &(pEthernet->NciPortInfo), &(pEthernet->NciCurrentConnectInfo),
                                    &(pEthernet->NciPortServices));
        if(MUSB_NCI_SUCCESS == NciStatus)
        {
            pEthernet->bNciAccepted = TRUE;
            MGC_CcdNciSetEnabled(pEthernet, TRUE);
        }
        else
        {
            MGC_CcdEthernetNciRelease(pEthernet);
        }
    }

    return TRUE;
}
#endif // CFG_USB
// eof

