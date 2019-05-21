/******************************************************************
*                                                                *
*          Copyright Mentor Graphics Corporation 2005            *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
* Function controller common code
* $Revision: 1.8 $
*/

#include "mu_impl.h"

#ifdef MUSB_FUNCTION

/* TODO: if we ever do FSFC, make a common version */
#include "mu_hfcdf.h"
#include "mu_hfcpr.h"

#include "mu_funpr.h"
#include "mu_pippr.h"

/**************************** FORWARDS ****************************/

static void MGC_FcFlushAll(MGC_Port *pPort);

/*************************** FUNCTIONS ****************************/

extern uint8_t MGC_FcUsbIsr(MGC_Port *pPort, uint8_t bIntrUsbVal);
/*
* FC Interrupt Service Routine
*/
int MGC_FcIsr(MGC_Controller *pController, uint8_t bIntrUsbValue,
              uint16_t wIntrTxValue, uint16_t wIntrRxValue)
{
    MGC_BsrItem qItem;
    uint32_t dwRegVal;
    uint8_t bQueue;
    unsigned int iShift;
    int iResult = -1;
    MUSB_SystemServices *pServices = pController->pSystemServices;
    MGC_Port *pPort = pController->pPort;

#ifdef MUSB_DMA
    if(pPort->pDmaController && pPort->pDmaController->pfDmaControllerIsr)
    {
        if(pPort->pDmaController->pfDmaControllerIsr(
                    pPort->pDmaController->pPrivateData))
        {
            iResult = 0;
        }
    }
#endif	/* DMA enabled */

    if(bIntrUsbValue)
    {
        MGC_DIAG1(3, pController, "IntrUSB=", bIntrUsbValue, 16, 2);
        iResult = MGC_FcUsbIsr(pPort, bIntrUsbValue);
    }
    MGC_DIAG2(3, pController, "IntrTx=", wIntrTxValue, " / IntrRx=",
              wIntrRxValue, 16, 4);

    /* scan for Tx endpoints requiring servicing */
    dwRegVal = wIntrTxValue;
    if(dwRegVal && (iResult < 0))
    {
        iResult = 0;
    }
    /* the low bit is a special case: Tx or Rx on endpoint 0 */
    if(dwRegVal & 1)
    {
        bQueue = pPort->pfServiceDefaultEnd(pPort, &qItem);
        if(bQueue)
        {
            iResult++;
#ifdef MUSB_OVERHEAD
            qItem.dwTime = pController->pUtils->pfGetTime();
#endif
            pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
        }
    }

    /* scan for Tx endpoints 1-15 requiring servicing */
    iShift = 1;
    dwRegVal >>= 1;
    while(dwRegVal)
    {
        if(dwRegVal & 1)
        {
            bQueue = pPort->pfServiceTransmitAvail(pPort, iShift, &qItem);
            if(bQueue)
            {
                iResult++;
#ifdef MUSB_OVERHEAD
                qItem.dwTime = pController->pUtils->pfGetTime();
#endif
                qItem.bLocalEnd = iShift;
                pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
            }
        }
        dwRegVal >>= 1;
        iShift++;
    }

    /* scan for Rx endpoints 1-15 requiring servicing */
    dwRegVal = wIntrRxValue;
    if(dwRegVal && (iResult < 0))
    {
        iResult = 0;
    }
    iShift = 1;
    dwRegVal >>= 1;
    while(dwRegVal)
    {
        if(dwRegVal & 1)
        {
            bQueue = pPort->pfServiceReceiveReady(pPort, iShift, &qItem);
            if(bQueue)
            {
                iResult++;
#ifdef MUSB_OVERHEAD
                qItem.dwTime = pController->pUtils->pfGetTime();
#endif
                qItem.bLocalEnd = iShift;
                pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
            }
        }
        dwRegVal >>= 1;
        iShift++;
    }

    return iResult;
}

uint8_t MGC_FcDmaChannelStatusChanged(
    void *pPrivateData, uint8_t bLocalEnd, uint8_t bTransmit)
{
    MGC_BsrItem qItem;
    uint8_t bQueue;
    MGC_Port *pPort = (MGC_Port *)pPrivateData;
    MGC_Controller *pController = pPort->pController;
    MUSB_SystemServices *pServices = pController->pSystemServices;

    /* TODO: something more specific might be better */
    if(bTransmit)
    {
        bQueue = pPort->pfServiceTransmitAvail(pPort, bLocalEnd, &qItem);
        if(bQueue)
        {
#ifdef MUSB_OVERHEAD
            qItem.dwTime = pController->pUtils->pfGetTime();
#endif
            qItem.bLocalEnd = bLocalEnd;
            pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
        }
    }
    else
    {
        bQueue = pPort->pfServiceReceiveReady(pPort, bLocalEnd, &qItem);
        if(bQueue)
        {
#ifdef MUSB_OVERHEAD
            qItem.dwTime = pController->pUtils->pfGetTime();
#endif
            qItem.bLocalEnd = bLocalEnd;
            pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
        }
    }
    return bQueue;
}

/*
* Find the FC's required local endpoint resource
* to be identified with the given endpoint descriptor.
*/
MGC_EndpointResource *MGC_FcBindEndpoint(MGC_Port *pPort,
        const MUSB_DeviceEndpoint *pUsbEnd,
        MUSB_EndpointResource *pRequest,
        uint8_t bBind, uint8_t *pbIsTx)
{
    uint32_t dwEndCount;
    uint32_t dwEndIndex;
    uint8_t bIsIn;
    uint8_t bIsTx;
    uint8_t bEnd;
    uint8_t bIsClaimed;
    uint16_t wPacketSize;
    MGC_EndpointResource *pEnd;
    uint16_t wMaxPacketSize;
    MGC_EndpointResource *pBestEnd = NULL;
    MGC_EndpointResource *pBestReqEnd = NULL;

    wPacketSize = MUSB_SWAP16P((uint8_t *) & (pUsbEnd->UsbDescriptor.wMaxPacketSize));
    bIsIn = (pUsbEnd->UsbDescriptor.bEndpointAddress & MUSB_ENDPOINT_DIR_MASK) ? TRUE : FALSE;
    bIsTx = bIsIn;
    dwEndCount = MUSB_ArrayLength(&(pPort->LocalEnds));
    for(dwEndIndex = 0; dwEndIndex < dwEndCount; dwEndIndex++)
    {
        pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds), dwEndIndex);
        bIsClaimed = pEnd->bIsFifoShared ?
                     pEnd->bIsClaimed : (bIsTx ? pEnd->bIsClaimed : pEnd->bRxClaimed);
        if(pEnd && !bIsClaimed)
        {
            wMaxPacketSize = bIsTx ? pEnd->wMaxPacketSizeTx : pEnd->wMaxPacketSizeRx;
            /* need address match, compatible direction and FIFO size */
            bEnd = pUsbEnd->UsbDescriptor.bEndpointAddress & MUSB_ENDPOINT_NUMBER_MASK;
            if((wMaxPacketSize >= wPacketSize) &&
                    (pEnd->bLocalEnd == bEnd))
            {
                /* found match */
                pBestEnd = pEnd;
                break;
            }
        }
    }

    /* see if we fulfilled desire */
    if(pBestReqEnd)
    {
        pBestEnd = pBestReqEnd;
    }

    /* if we found one, set it up */
    if(pBestEnd && bBind)
    {
        if(pBestEnd->bIsFifoShared || bIsTx)
        {
            pBestEnd->bIsClaimed = TRUE;
        }
        else
        {
            pBestEnd->bRxClaimed = TRUE;
        }
        pBestEnd->bIsTx = bIsTx;
        pBestEnd->bIsHalted = FALSE;
        if(bIsTx)
        {
            pBestEnd->bBusAddress = pUsbEnd->pDevice->bBusAddress;
            pBestEnd->bBusEnd = pUsbEnd->UsbDescriptor.bEndpointAddress;
            pBestEnd->bInterval = pUsbEnd->UsbDescriptor.bInterval;
            pBestEnd->bTrafficType = pUsbEnd->UsbDescriptor.bmAttributes;
            pBestEnd->wPacketSize = wPacketSize;
        }
        else
        {
            pBestEnd->bRxBusAddress = pUsbEnd->pDevice->bBusAddress;
            pBestEnd->bRxBusEnd = pUsbEnd->UsbDescriptor.bEndpointAddress;
            pBestEnd->bRxInterval = pUsbEnd->UsbDescriptor.bInterval;
            pBestEnd->bRxTrafficType = pUsbEnd->UsbDescriptor.bmAttributes;
            pBestEnd->wRxPacketSize = wPacketSize;
        }
    }

    *pbIsTx = bIsTx;
    return pBestEnd;

}

/* Reset the port (e.g. unbind/flush all endpoints, etc.) */
uint32_t MGC_FcResetPort(MGC_Port *pPort)
{
    MGC_Controller *pController = pPort->pController;

    /* reset FADDR */
    pPort->bFuncAddr = 0;
    pPort->pfProgramBusState(pPort);

    /* stop controller */
    pController->pfProgramStopController(pController);

    /* flush BSR queue */
    pController->pSystemServices->pfFlushBackgroundQueue(
        pController->pSystemServices->pPrivateData);

    MGC_FcFlushAll(pPort);

    pController->pfProgramStartController(pController);

    /* get current readings */
    pPort->pfReadBusState(pPort);

    pPort->bUsbState = MUSB_POWER_OFF;

    return 0;
}

/**
 * Stop all traffic.
 * Flush all local endpoints, kill all pending IRPs
 * @param pPort port pointer
 */
static void MGC_FcFlushAll(MGC_Port *pPort)
{
    uint16_t wIndex, wCount;
    uint32_t dwEndCount;
    uint32_t dwEndIndex;
    void *pIrp;
    MGC_EndpointResource *pEnd;

    /* flush all endpoints */
    dwEndCount = MUSB_ArrayLength(&(pPort->LocalEnds));
    for(dwEndIndex = 1; dwEndIndex < dwEndCount; dwEndIndex++)
    {
        pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds), dwEndIndex);
        if(pEnd)
        {
            pEnd->bIsClaimed = FALSE;
            pPort->pfProgramFlushEndpoint(pPort, pEnd, 0, TRUE);
            pPort->pfProgramFlushEndpoint(pPort, pEnd, MUSB_ENDPOINT_DIR_MASK, TRUE);
            wCount = MUSB_ListLength(&(pEnd->TxIrpList));
            for(wIndex = 0; wIndex < wCount; wIndex++)
            {
                pIrp = MUSB_ListFindItem(&(pEnd->TxIrpList), 0);
                MUSB_ListRemoveItem(&(pEnd->TxIrpList), pIrp);
            }
            wCount = MUSB_ListLength(&(pEnd->RxIrpList));
            for(wIndex = 0; wIndex < wCount; wIndex++)
            {
                pIrp = MUSB_ListFindItem(&(pEnd->RxIrpList), 0);
                MUSB_ListRemoveItem(&(pEnd->RxIrpList), pIrp);
            }
        }
    }

}

/*
* Handle default endpoint interrupt
*/
uint8_t MGC_FcServiceDefaultEnd(MGC_Port *pPort, MGC_BsrItem *pItem,
                                uint16_t *pwCsr, uint16_t wCount)
{
    uint16_t wFifoCount, wSpace;
    uint8_t *pFifoDest;
    MUSB_DeviceRequest *pRequest;
    uint8_t bVal = (uint8_t) * pwCsr;
    uint8_t bOutVal = 0;
    uint8_t bResult = FALSE;
    uint8_t status = MUSB_STATUS_OK;
    MGC_EndpointResource *pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds), 0);

    /* peripheral mode: see if Rx or Tx */
    if(bVal & MGC_M_CSR0_RXPKTRDY)
    {
        pPort->wSetupDataIndex = 0;
        pPort->wSetupDataSize = 0;
        pPort->bEnd0Status = FALSE;
        /* receive */
        wSpace = pPort->pFunctionClient ?
                 (pPort->pFunctionClient->wControlBufferLength - pPort->wSetupDataIndex) : 0;
        wFifoCount = MUSB_MIN(wCount, wSpace);
        /* stall on potential overflow or get data */
        if(!pPort->pFunctionClient || (wCount > wFifoCount))
        {
            /* stall */
            bOutVal |= MGC_M_CSR0_P_SENDSTALL;
        }
        else
        {
            /* unload FIFO to the right place */
            pFifoDest = pPort->pFunctionClient->pControlBuffer + pPort->wSetupDataIndex;
#ifdef MUSB_DMA
            if(pEnd->pDmaChannel && pPort->pDmaController->pfDmaProgramChannel(
                        pEnd->pDmaChannel, pEnd->wPacketSize, pEnd->pDmaChannel->bDesiredMode,
                        pFifoDest,
                        MGC_MIN(wFifoCount, pEnd->pDmaChannel->dwMaxLength)))
            {
                /* prevent caller touching CSR0 */
                *pwCsr = 0;
                return TRUE;
            }
#endif
            pPort->pfUnloadFifo(pPort, 0, wFifoCount, pFifoDest);
            pPort->wSetupDataIndex += wFifoCount;
            /* check request */
            pRequest = (MUSB_DeviceRequest *)pPort->pFunctionClient->pControlBuffer;
            /* if IN or we got enough OUT data, parse */
            if((pRequest->bmRequestType & MUSB_DIR_IN) ||
                    ((pPort->wSetupDataIndex - 8) >= MUSB_SWAP16P((uint8_t *) & (pRequest->wLength))))
            {
                pPort->dwSequenceNumber++;
                pPort->wSetupDataSize = pPort->wSetupDataIndex;
                pPort->wSetupDataIndex = 0;
                bResult = FALSE;
                /* if parsing enabled */
                if(pPort->bParse)
                {
                    /* try to handle it now */
                    if(!MGC_FunctionParseSetup(pPort, &status))
                    {
                        /* callback */
                        if(!pPort->pFunctionClient->pfDeviceRequest(
                                    pPort->pFunctionClient->pPrivateData, (MUSB_BusHandle)pPort,
                                    pPort->dwSequenceNumber, pPort->pFunctionClient->pControlBuffer,
                                    pPort->wSetupDataIndex))
                        {
                            /* client rejected immediately, so STALL */
                            pPort->pfDefaultEndResponse(pPort, TRUE);
                        }
                    }
                    else
                    {
                        /* parser handled it */
                        if(MUSB_STATUS_STALLED == status)
                        {
                            pPort->wSetupDataIndex = 0;
                            pPort->wSetupDataSize = 0;
                            pPort->pfDefaultEndResponse(pPort, TRUE);
                        }
                        else
                        {
                            pPort->pfDefaultEndResponse(pPort, FALSE);
                        }
                    }
                }
                else
                {
                    /* parsing disabled; issue callback */
                    if(!pPort->pFunctionClient->pfDeviceRequest(
                                pPort->pFunctionClient->pPrivateData, (MUSB_BusHandle)pPort,
                                pPort->dwSequenceNumber, pPort->pFunctionClient->pControlBuffer,
                                pPort->wSetupDataIndex))
                    {
                        /* client rejected immediately, so STALL */
                        pPort->pfDefaultEndResponse(pPort, TRUE);
                    }
                }
            }
        }
    }
    else if(!(bVal & MGC_M_CSR0_TXPKTRDY))
    {
        /* transmit-ready */
        wFifoCount = MUSB_MIN(64, (pPort->wSetupDataSize - pPort->wSetupDataIndex));
        if(!wFifoCount)
        {
            /* status ack */
            pPort->wSetupDataIndex = 0;
            pPort->wSetupDataSize = 0;
            return FALSE;
        }
        pFifoDest = pPort->pSetupData + pPort->wSetupDataIndex;
#ifdef MUSB_DMA
        if(pEnd->pDmaChannel && pPort->pDmaController->pfDmaProgramChannel(
                    pEnd->pDmaChannel, pEnd->wPacketSize, pEnd->pDmaChannel->bDesiredMode,
                    pFifoDest,
                    MGC_MIN(wFifoCount, pEnd->pDmaChannel->dwMaxLength)))
        {
            /* prevent caller touching CSR0 */
            *pwCsr = 0;
            return TRUE;
        }
#endif
        /* load and signal need to transmit */
        pPort->pfLoadFifo(pPort, 0, wFifoCount, pFifoDest);
        pPort->wSetupDataIndex += wFifoCount;
        bOutVal |= MGC_M_CSR0_TXPKTRDY;
        if(pPort->wSetupDataIndex >= pPort->wSetupDataSize)
        {
            bOutVal |= MGC_M_CSR0_P_DATAEND;
        }
    }

    *pwCsr = bOutVal;
    return bResult;
}

/*
* Interrupt Service Routine to record USB "global" interrupts.
* Since these do not happen often and signify things of paramount importance,
* it seems OK to check them individually
*/

uint8_t MGC_FcUsbIsr(MGC_Port *pPort, uint8_t bIntrUsbVal)
{
    MGC_BsrItem qItem;
    uint32_t dwEndCount;
    uint32_t dwEndIndex;
    MGC_EndpointResource *pEnd;
    MUSB_IsochIrp *pIsochIrp;
    uint8_t bResult = 0;
    MUSB_SystemServices *pServices = pPort->pController->pSystemServices;

    if(bIntrUsbVal)
    {
        if(bIntrUsbVal & MGC_M_INTR_RESUME)
        {
            bResult++;
            qItem.bCause = MGC_BSR_CAUSE_RESUME;
            pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
        }
        if(bIntrUsbVal & MGC_M_INTR_SUSPEND)
        {
            bResult++;
            qItem.bCause = MGC_BSR_CAUSE_SUSPEND;
            pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
        }
        if(bIntrUsbVal & MGC_M_INTR_RESET)
        {
#ifdef MUSB_STATS
            pPort->dwResetCount++;
#endif
            /* update bus state */
            pPort->pfReadBusState(pPort);
            /* peripheral: process now because next interrupt could be soon */
            MGC_FcFlushAll(pPort);
            MGC_FunctionChangeState(pPort, MUSB_DEFAULT);
            MGC_FunctionSpeedSet(pPort);
        }
        if(bIntrUsbVal & MGC_M_INTR_SOF)
        {
            /* update frame number */
            pPort->pfReadBusState(pPort);

            /* start any waiting isochronous transmits whose wait has expired */
            dwEndCount = MUSB_ArrayLength(&(pPort->LocalEnds));
            for(dwEndIndex = 1; dwEndIndex < dwEndCount; dwEndIndex++)
            {
                pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(
                           &(pPort->LocalEnds), dwEndIndex);
                if(pEnd && (MUSB_ENDPOINT_XFER_ISOC == pEnd->bTrafficType) &&
                        pEnd->dwWaitFrameCount)
                {
                    pIsochIrp = (MUSB_IsochIrp *)pEnd->pTxIrp;
                    if(0 == --pEnd->dwWaitFrameCount)
                    {
                        pPort->pfProgramStartTransmit(pPort, pEnd,
                                                      pIsochIrp->pBuffer, pIsochIrp->adwLength[0], pIsochIrp);
                    }
                }
            }

        }

    }
    return bResult;
}

/*
* Finish FC interrupt processing
*/

uint32_t MGC_FcBsr(void *pParam)
{
#ifdef MUSB_OVERHEAD
    uint32_t dwTime;
    MGC_OverheadStats *pOverheadStats;
#endif
    MGC_BsrItem item;
    uint8_t bOk;
    uint8_t bTrafficType;
    MGC_EndpointResource *pLocalEnd;
    MUSB_Irp *pIrp;
    MUSB_ControlIrp *pControlIrp;
    MUSB_IsochIrp *pIsochIrp;
    MUSB_Port *pIfacePort = (MUSB_Port *)pParam;
    MGC_Port *pPort = (MGC_Port *)pIfacePort->pPrivateData;
    MGC_Controller *pController = (MGC_Controller *)pPort->pController;
    MUSB_SystemServices *pServices = pController->pSystemServices;

    bOk = pServices->pfDequeueBackgroundItem(pServices->pPrivateData, &item);
    while(bOk)
    {
        switch(item.bCause)
        {
        case MGC_BSR_CAUSE_RESUME:
            /* resume */
#ifdef MUSB_STATS
            pPort->dwResumeCount++;
#endif
            MGC_DIAG(2, pController, "Resume Interrupt");
            pPort->pFunctionClient->pfUsbState(pPort->pFunctionClient->pPrivateData,
                                               (MUSB_BusHandle)pPort, (MUSB_State)(pPort->bUsbState));
            break;

        case MGC_BSR_CAUSE_SUSPEND:
#ifdef MUSB_STATS
            pPort->dwSuspendCount++;
#endif
            MGC_DIAG(2, pController, "Suspend Interrupt");
            /* inform function client; things may change further, but that's OK */
            pPort->pFunctionClient->pfUsbState(pPort->pFunctionClient->pPrivateData,
                                               (MUSB_BusHandle)pPort, MUSB_SUSPENDED);
            break;

        case MGC_BSR_CAUSE_RESET:
            MGC_DIAG(2, pController, "Reset Interrupt");
            pPort->pfReadBusState(pPort);
            break;

        case MGC_BSR_CAUSE_IRP_COMPLETE:
        case MGC_BSR_CAUSE_RX_IRP_COMPLETE:
            pLocalEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(
                            &(pPort->LocalEnds), item.bLocalEnd);
#ifdef MUSB_OVERHEAD
            pOverheadStats = MUSB_ArrayFetch(&(pPort->OverheadStats), item.bLocalEnd);
            if(pOverheadStats)
            {
                dwTime = pController->pUtils->pfGetTime() - item.dwTime;
                if(dwTime < pOverheadStats->dwOverheadMin)
                {
                    pOverheadStats->dwOverheadMin = dwTime;
                }
                if(dwTime > pOverheadStats->dwOverheadMax)
                {
                    pOverheadStats->dwOverheadMax = dwTime;
                }
            }
#endif
            if(pLocalEnd)
            {
                bTrafficType = (MGC_BSR_CAUSE_IRP_COMPLETE == item.bCause) ?
                               pLocalEnd->bTrafficType : pLocalEnd->bRxTrafficType;
                switch(bTrafficType)
                {
                case MUSB_ENDPOINT_XFER_BULK:
                case MUSB_ENDPOINT_XFER_INT:
                    pIrp = (MUSB_Irp *)item.pData;
                    if(pIrp && pIrp->pfIrpComplete)
                    {
                        pIrp->pfIrpComplete(pIrp->pCompleteParam, pIrp);
                    }
                    break;

                case MUSB_ENDPOINT_XFER_CONTROL:
                    pControlIrp = (MUSB_ControlIrp *)item.pData;
                    if(pControlIrp && pControlIrp->pfIrpComplete)
                    {
                        pControlIrp->pfIrpComplete(pControlIrp->pCompleteParam,
                                                   pControlIrp);
                    }
                    break;

                case MUSB_ENDPOINT_XFER_ISOC:
                    pIsochIrp = (MUSB_IsochIrp *)item.pData;
                    if(pIsochIrp && pIsochIrp->pfIrpComplete)
                    {
                        pIsochIrp->pfIrpComplete(pIsochIrp->pCompleteParam,
                                                 pIsochIrp);
                    }
                    break;
                }
                /* start next IRP if not started already */
                if(MGC_BSR_CAUSE_IRP_COMPLETE == item.bCause)
                {
                    if(!pLocalEnd->pTxIrp && !pLocalEnd->bStopTx)
                    {
                        MGC_StartNextIrp(pPort, pLocalEnd, TRUE);
                    }
                }
                else
                {
                    if(!pLocalEnd->pRxIrp && !pLocalEnd->bIsRxHalted)
                    {
                        MGC_StartNextIrp(pPort, pLocalEnd, FALSE);
                    }
                }
            }
        }

        /* get next item to process */
        bOk = pServices->pfDequeueBackgroundItem(pServices->pPrivateData, &item);
    }

    return 0;
}

/* Implementation */
MUSB_BusHandle MUSB_RegisterFunctionClient(
    MUSB_Port *pPort,
    MUSB_FunctionClient *pFunctionClient
)
{
    uint32_t dwStatus;
    void *pResult = NULL;
    MGC_Port *pImplPort = (MGC_Port *)pPort->pPrivateData;

    /* session still active? */
    if(!pImplPort->bSessionActive && (MUSB_PORT_TYPE_FUNCTION == pPort->Type))
    {
        /* function client requires verification */
        dwStatus = MGC_FunctionRegisterClient(pImplPort, pFunctionClient);
        if(0 == dwStatus)
        {
            /* nothing active; reset port */
            pImplPort->pfResetPort(pImplPort);

            /* start */
            pImplPort->pFunctionClient = pFunctionClient;
            pImplPort->bSessionActive = TRUE;

            /* set return value */
            pResult = pImplPort;
        }
    }

    return pResult;
}

#if MUSB_DIAG > 0

/*
* Dump info relevant to the given pipe
*/
int MGC_FcDumpPipe(MGC_Controller *pController, MGC_Pipe *pPipe)
{
    /* TODO */
    return 0;
}

#endif	/* MUSB_DIAG */

#else

/* satisfy some compilers */
uint8_t MGC_bNoFc;

#endif	/* FUNCTION */
