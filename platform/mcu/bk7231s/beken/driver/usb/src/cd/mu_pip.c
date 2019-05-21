/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2005              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * Generic USB transfer functionality.
 * $Revision: 1.53 $
 */
#include <stdio.h>
#include <string.h>
#include "mu_impl.h"

#include "mu_pippr.h"

const uint8_t *MGC_FindDescriptor(const uint8_t *pBuffer,
                                  uint16_t wBufferLength,
                                  uint8_t bDescriptorType,
                                  uint8_t bIndex)
{
    int16_t wFoundIndex = -1;
    uint16_t wOffset = 0;
    const MUSB_DescriptorHeader *pHeader = (MUSB_DescriptorHeader *)pBuffer;

    /* handle trivial case */
    if((pHeader->bDescriptorType == bDescriptorType) && !bIndex)
    {
        return (uint8_t *)pHeader;
    }

    /* general case */
    while((wOffset < wBufferLength) && (wFoundIndex < (int16_t)bIndex))
    {
        pHeader = (MUSB_DescriptorHeader *)((uint8_t *)pBuffer + wOffset);
        if(pHeader->bLength < 1)
        {
            /* we're in the weeds */
            return NULL;
        }
        wOffset += pHeader->bLength;
        if(pHeader->bDescriptorType == bDescriptorType)
        {
            wFoundIndex++;
            if(wFoundIndex == (uint16_t)bIndex)
            {
                return (uint8_t *)pHeader;
            }
        }
    }
    if(wOffset < wBufferLength)
    {
        return (uint8_t *)pHeader;
    }
    return NULL;
}

/*
 * Service a received packet; returning indication of transfer complete.
 */
uint8_t MGC_PipePacketReceived(MGC_Port *pPort, uint32_t status,
                               MGC_EndpointResource *pEnd, uint16_t wPacketSize,
                               uint8_t bContinue, void *pCurrentIrp)
{
#ifdef MUSB_STATS
    MGC_EndpointStats *pEndpointStats;
#endif
    uint8_t *pBuffer;
    uint16_t wUnloadCount;
#ifdef MUSB_ISO
    uint16_t wFrameIndex;
    MUSB_IsochIrp *pIsochIrp = NULL;
#endif
    uint8_t bAllowDma = FALSE;
    uint32_t dwReqLength = 0;
    MUSB_Irp *pIrp = NULL;
    uint8_t bComplete = FALSE;

    wUnloadCount = wPacketSize;
    switch(pEnd->bRxTrafficType & MUSB_ENDPOINT_XFERTYPE_MASK)
    {
    case MUSB_ENDPOINT_XFER_BULK:
    case MUSB_ENDPOINT_XFER_INT:
        pIrp = (MUSB_Irp *)pCurrentIrp;
        bAllowDma = pIrp->bAllowDma;
        dwReqLength = pIrp->dwLength;
        pBuffer = pIrp->pBuffer + pEnd->dwRxOffset;
        if(wPacketSize < pEnd->wRxPacketSize)
        {
            /* short packet means transfer complete */
            bComplete = TRUE;
        }
        if((wPacketSize + pEnd->dwRxOffset) >= dwReqLength)
        {
            /* received as much data as requested means transfer complete */
            bComplete = TRUE;
            /* but, the other side may send a 0-byte packet */
            if(pIrp->bAllowShortTransfer && (0 == (wPacketSize % pEnd->wRxPacketSize)))
            {
                bComplete = FALSE;
            }
            wUnloadCount = MGC_MIN(wPacketSize, (uint16_t)(dwReqLength - pEnd->dwRxOffset));
        }
        dwReqLength -= pEnd->dwRxOffset;
        break;

#ifdef MUSB_ISO
    case MUSB_ENDPOINT_XFER_ISOC:
        pIsochIrp = (MUSB_IsochIrp *)pCurrentIrp;
        bAllowDma = pIsochIrp->bAllowDma;
        wFrameIndex = pIsochIrp->wCurrentFrame;
        pIsochIrp->adwActualLength[wFrameIndex] = wPacketSize;
        pIsochIrp->adwStatus[wFrameIndex] = 0;
        if( 0 != wPacketSize )
        {
            pIsochIrp->wCurrentFrame++;
        }
        dwReqLength = pIsochIrp->adwLength[wFrameIndex + 1];
        pBuffer = pIsochIrp->pBuffer + pEnd->dwRxOffset;
        /* complete when frame count is reached */
        bComplete = (pIsochIrp->wCurrentFrame >= pIsochIrp->wFrameCount) ? TRUE : FALSE;
        /* pretend to be complete on callback interval (CompleteIrp won't retire it in this case) */
        if(pIsochIrp->wCallbackInterval && (0 == (pIsochIrp->wCurrentFrame % pIsochIrp->wCallbackInterval)))
        {
            bComplete = TRUE;
        }
        break;
#endif

    default:
        pBuffer = NULL;
        wUnloadCount = 0;
        bComplete = TRUE;
        break;
    }

    pPort->pfUnloadFifo(pPort, pEnd->bLocalEnd, wUnloadCount, pBuffer);
    pEnd->dwRxOffset += wUnloadCount;

    switch(pEnd->bRxTrafficType & MUSB_ENDPOINT_XFERTYPE_MASK)
    {
#ifdef MUSB_ISO
    case MUSB_ENDPOINT_XFER_ISOC:
        if( wUnloadCount > ISOC_PACK_HEAD_LEN )
        {
            pIsochIrp->adwActualLength[wFrameIndex] -= ISOC_PACK_HEAD_LEN;
            pEnd->dwRxOffset -= ISOC_PACK_HEAD_LEN;
            memcpy( &pBuffer[0], &pBuffer[ISOC_PACK_HEAD_LEN], (wUnloadCount - ISOC_PACK_HEAD_LEN) );
        }
        break;
#endif
    default:
        break;
    }

    pBuffer += wUnloadCount;

    if(!bComplete && bContinue)
    {
        pPort->pfProgramStartReceive(pPort, pEnd, pBuffer, dwReqLength, pCurrentIrp, bAllowDma);
    }

#ifdef MUSB_STATS
    pEndpointStats = MUSB_ArrayFetch(&(pPort->EndpointStats), pEnd->bLocalEnd);
    if(pEndpointStats)
    {
        pEndpointStats->dwRxByteCount += wUnloadCount;
    }
#endif

    return bComplete;
}

/*
 * Service a transmit buffer-ready; returning indication of transfer complete.
 */
uint8_t MGC_PipeTransmitReady(MGC_Port *pPort, MGC_EndpointResource *pEnd)
{
#ifdef MUSB_ISO
    uint16_t wFrameIndex;
    MUSB_IsochIrp *pIsochIrp;
#endif
#ifdef MUSB_STATS
    MGC_EndpointStats *pEndpointStats;
#endif
    uint32_t dwReqLength;
    MUSB_Irp *pIrp;
    void *pGenIrp;
    uint8_t *pBuffer = NULL;
    uint8_t bComplete = FALSE;

    if(pEnd->pTxIrp)
    {
        switch(pEnd->bTrafficType)
        {
        case MUSB_ENDPOINT_XFER_BULK:
        case MUSB_ENDPOINT_XFER_INT:
            pIrp = (MUSB_Irp *)pEnd->pTxIrp;
            pGenIrp = pIrp;
            dwReqLength = pIrp->dwLength;
            if(pEnd->dwTxOffset >= dwReqLength)
            {
                /* send 0-byte packet if desired & needed */
                if(pIrp->bAllowShortTransfer && (0 == (pEnd->dwTxSize % pEnd->wPacketSize)))
                {
                    pPort->pfProgramStartTransmit(pPort, pEnd, pBuffer, 0, pIrp);
                    return FALSE;
                }
                /* IRP complete */
                bComplete = TRUE;
            }
            /* update buffer pointer */
            dwReqLength -= pEnd->dwTxOffset;
            pBuffer = pIrp->pBuffer + pEnd->dwTxOffset;
            break;
#ifdef MUSB_ISO
        case MUSB_ENDPOINT_XFER_ISOC:
            pIsochIrp = (MUSB_IsochIrp *)pEnd->pTxIrp;
            pGenIrp = pIsochIrp;
            wFrameIndex = pIsochIrp->wCurrentFrame;
            pIsochIrp->adwActualLength[wFrameIndex] = pIsochIrp->adwLength[wFrameIndex];
            pIsochIrp->adwStatus[wFrameIndex] = 0;
            pIsochIrp->wCurrentFrame++;
            dwReqLength = pIsochIrp->adwLength[wFrameIndex + 1];
            pBuffer = pIsochIrp->pBuffer + pEnd->dwTxOffset;
            /* complete when frame count is reached */
            bComplete = (pIsochIrp->wCurrentFrame >= pIsochIrp->wFrameCount) ? TRUE : FALSE;
            /* pretend to be complete on callback interval (CompleteIrp won't retire it in this case) */
            if(pIsochIrp->wCallbackInterval && (0 == (pIsochIrp->wCurrentFrame % pIsochIrp->wCallbackInterval)))
            {
                bComplete = TRUE;
            }
            break;
#endif
        default:
            bComplete = TRUE;
            dwReqLength = 0;
            MGC_DIAG1(1, pPort->pController, "Internal error on end ", pEnd->bLocalEnd, 10, 0);
            break;
        }
    }
    else
    {
        bComplete = TRUE;
    }

    if(!bComplete)
    {
        pPort->pfProgramStartTransmit(pPort, pEnd, pBuffer, dwReqLength, pGenIrp);

#ifdef MUSB_STATS
        pEndpointStats = MUSB_ArrayFetch(&(pPort->EndpointStats), pEnd->bLocalEnd);
        if(pEndpointStats)
        {
            pEndpointStats->dwTxByteCount += wLoadCount;
        }
#endif
    }

    return bComplete;
}

/*
 * Generic part of completing an IRP
 */
uint8_t MGC_CompleteIrp(MGC_BsrItem *pItem, MGC_EndpointResource *pEnd,
                        uint8_t bStatus, uint8_t bIsTx, void *pCurrentIrp)
{
    uint8_t bTrafficType;
    MUSB_ControlIrp *pControlIrp;
    MUSB_Irp *pIrp;
    MGC_Pipe *pPipe;
#ifdef MUSB_ISO
    MUSB_IsochIrp *pIsochIrp;
#endif

    if(!pCurrentIrp)
    {
        return FALSE;
    }
    if(bIsTx)
    {
        bTrafficType = pEnd->bTrafficType & MUSB_ENDPOINT_XFERTYPE_MASK;
        pItem->bCause = MGC_BSR_CAUSE_IRP_COMPLETE;
    }
    else
    {
        bTrafficType = pEnd->bRxTrafficType & MUSB_ENDPOINT_XFERTYPE_MASK;
        pItem->bCause = MGC_BSR_CAUSE_RX_IRP_COMPLETE;
    }

    /* if control/bulk or isoch limit, endpoint is done with this IRP */
    switch(bTrafficType)
    {
    case MUSB_ENDPOINT_XFER_INT:
        if (bIsTx)
        {
            pEnd->pTxIrp = NULL;
        }
        break;
#ifdef MUSB_ISO
    case MUSB_ENDPOINT_XFER_ISOC:
        pIsochIrp = (MUSB_IsochIrp *)pCurrentIrp;
        if(pIsochIrp->wCurrentFrame >= pIsochIrp->wFrameCount)
        {
            if(bIsTx)
            {
                pEnd->pTxIrp = NULL;
            }
            else
            {
                pEnd->pRxIrp = NULL;
            }
        }
        break;
#endif
    default:
        if(bIsTx)
        {
            pEnd->pTxIrp = NULL;
        }
        else
        {
            pEnd->pRxIrp = NULL;
        }
    }

    /* fill info for queue */
    pItem->bLocalEnd = pEnd->bLocalEnd;
    pItem->bStatus = bStatus;
    pItem->pData = pCurrentIrp;

    /* update IRP status/actual */
    switch(bTrafficType)
    {
    case MUSB_ENDPOINT_XFER_CONTROL:
        pControlIrp = (MUSB_ControlIrp *)pCurrentIrp;
        pControlIrp->dwStatus = bStatus;
        break;
    case MUSB_ENDPOINT_XFER_BULK:
    case MUSB_ENDPOINT_XFER_INT:
        pIrp = (MUSB_Irp *)pCurrentIrp;
        pIrp->dwStatus = bStatus;
        if(bIsTx)
        {
            pIrp->dwActualLength = pEnd->dwTxOffset;
            pEnd->dwTxOffset = 0L;
            pEnd->dwTxSize = 0L;
        }
        else
        {
            pIrp->dwActualLength = pEnd->dwRxOffset;
            pEnd->dwRxOffset = 0L;
        }
        if(pIrp->bIsrCallback)
        {
            MUSB_PRT("++bIsrcallback is valid!!!++\r\n");
            pIrp->pfIrpComplete(pIrp->pCompleteParam, pIrp);
            /* start next IRP if not started already */
            pPipe = (MGC_Pipe *)pIrp->hPipe;
            if(bIsTx)
            {
                if(!pEnd->pTxIrp && !pEnd->bStopTx)
                {
                    MGC_StartNextIrp(pPipe->pPort, pEnd, TRUE);
                }
            }
            else
            {
                if(((MUSB_ENDPOINT_XFER_INT == bTrafficType) || !pEnd->pRxIrp)
                        && !pEnd->bIsRxHalted)
                {
                    MGC_StartNextIrp(pPipe->pPort, pEnd, FALSE);
                }
            }
            return FALSE;
        }
        break;
#ifdef MUSB_ISO
    case MUSB_ENDPOINT_XFER_ISOC:
        pIsochIrp = (MUSB_IsochIrp *)pCurrentIrp;
        if(bStatus || (pIsochIrp->wCurrentFrame >= pIsochIrp->wFrameCount))
        {
            if(bIsTx)
            {
                pEnd->dwTxOffset = 0L;
                pEnd->dwTxSize = 0L;
            }
            else
            {
                pEnd->dwRxOffset = 0L;
            }
        }
        if(pIsochIrp->bIsrCallback)
        {
            pIsochIrp->pfIrpComplete(pIsochIrp->pCompleteParam,
                                     pIsochIrp);
            /* start next IRP if not started already */
            pPipe = (MGC_Pipe *)pIsochIrp->hPipe;
            if(bIsTx)
            {
                if(!pEnd->pTxIrp && !pEnd->bStopTx)
                {
                    MGC_StartNextIrp(pPipe->pPort, pEnd, TRUE);
                }
            }
            else
            {
                if(!pEnd->pRxIrp && !pEnd->bIsRxHalted)
                {
                    MGC_StartNextIrp(pPipe->pPort, pEnd, FALSE);
                }
            }
            return FALSE;
        }
        break;
#endif
    default:
        if(bIsTx)
        {
            pEnd->dwTxOffset = 0L;
        }
        else
        {
            pEnd->dwRxOffset = 0L;
        }
        break;
    }

    return TRUE;
}

/*
 * Start the next IRP, if any, for the given end.
 * In function role, only identical traffic/direction can be queued.
 * In host role, bulk traffic can be re-targetted and direction changed.
 */
uint8_t MGC_StartNextIrp(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                         uint8_t bIsTx)
{
    uint8_t bOk = FALSE;
    uint8_t bTypeOk, bSwitchBulk, bTrafficType;
    uint8_t bAllowDma = FALSE;
    uint8_t *pBuffer = NULL;
    uint32_t dwReqLength = 0;
    MUSB_Irp *pIrp = NULL;
    MUSB_IsochIrp *pIsochIrp = NULL;
    MGC_Pipe *pPipe = NULL;
    void *pNextIrp = bIsTx ? pEnd->pTxIrp : pEnd->pRxIrp;

    if(!pNextIrp)
    {
        pNextIrp = bIsTx ? MUSB_ListFindItem(&(pEnd->TxIrpList), 0) : MUSB_ListFindItem(&(pEnd->RxIrpList), 0);
    }

    if(pNextIrp)
    {
        if(bIsTx)
        {
            MUSB_ListRemoveItem(&(pEnd->TxIrpList), pNextIrp);
        }
        else
        {
            MUSB_ListRemoveItem(&(pEnd->RxIrpList), pNextIrp);
        }
        /* NOTE: relies on both IRP types having hPipe first! */
        pIrp = (MUSB_Irp *)pNextIrp;
        pPipe = (MGC_Pipe *)pIrp->hPipe;
        bTrafficType = bIsTx ? pEnd->bTrafficType : pEnd->bRxTrafficType;
        /* now cast according to type */
        switch(bTrafficType)
        {
        case MUSB_ENDPOINT_XFER_BULK:
        case MUSB_ENDPOINT_XFER_INT:
            pIrp = (MUSB_Irp *)pNextIrp;
            bAllowDma = pIrp->bAllowDma;
            pPipe = (MGC_Pipe *)pIrp->hPipe;
            break;
        case MUSB_ENDPOINT_XFER_ISOC:
            pIsochIrp = (MUSB_IsochIrp *)pNextIrp;
            bAllowDma = pIsochIrp->bAllowDma;
            pPipe = (MGC_Pipe *)pIsochIrp->hPipe;
            break;
        }
        bTypeOk = (pPipe->bTrafficType == bTrafficType);
        bSwitchBulk = (pPort->bIsHost && bTypeOk && (MUSB_ENDPOINT_XFER_BULK == pPipe->bTrafficType));
        if(pPipe && (bTypeOk || bSwitchBulk))
        {
            bOk = TRUE;
            if(bSwitchBulk)
            {
                /* TODO */
            }

            /* program it */
            switch(bTrafficType)
            {
            case MUSB_ENDPOINT_XFER_BULK:
            case MUSB_ENDPOINT_XFER_INT:
                dwReqLength = pIrp->dwLength - (bIsTx ? pEnd->dwTxOffset : pEnd->dwRxOffset);
                pBuffer = pIrp->pBuffer;
                break;
            case MUSB_ENDPOINT_XFER_ISOC:
                dwReqLength = pIsochIrp->adwLength[0];
                pBuffer = pIsochIrp->pBuffer;
                break;
            }
            if(bIsTx)
            {
                /* start next Tx if not interrupt protocol */
                if(MUSB_ENDPOINT_XFER_INT != bTrafficType)
                {
                    pEnd->pTxIrp = pNextIrp;
                    pPort->pfProgramStartTransmit(pPort, pEnd, pBuffer, dwReqLength, pNextIrp);
                }
            }
            else
            {
                pPort->pfProgramStartReceive(pPort, pEnd, pBuffer, dwReqLength,
                                             pNextIrp, bAllowDma);
            }
        }
        else
        {
            /* TODO: log internal error */
            if(bIsTx)
            {
                pEnd->pTxIrp = NULL;
            }
            else
            {
                pEnd->pRxIrp = NULL;
            }
        }
    }

    return bOk;
}

/*
 * Get whether a pipe is halted
 * Returns: TRUE if pipe is halted; FALSE otherwise
 */
uint8_t MUSB_GetPipeHalt(MUSB_PipePtr hPipe)
{
    MGC_Pipe *pPipe = (MGC_Pipe *)hPipe;

    return pPipe->pLocalEnd->bIsHalted;
}

/*
 * Set whether a pipe is halted
 * Returns: 0 on success; error code on failure
 */
uint32_t MUSB_SetPipeHalt(MUSB_PipePtr hPipe, uint8_t bHalt)
{
    MGC_Pipe *pPipe = (MGC_Pipe *)hPipe;
    MGC_Port *pPort = (MGC_Port *)pPipe->hSession;
    uint8_t bIsTx = (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ? TRUE : FALSE;
    uint8_t bIsIn = pPort->bIsHost ? !bIsTx : bIsTx;

    pPipe->pLocalEnd->bIsHalted = bHalt;
    return pPort->pfProgramHaltEndpoint(pPort, pPipe->pLocalEnd,
                                        bIsIn ? MUSB_DIR_IN : 0, bHalt);
}

/*
 * Flush the given pipe
 * Returns: 0 on success; error code on failure
 */
uint32_t MUSB_FlushPipe(MUSB_PipePtr hPipe)
{
    MGC_Pipe *pPipe = (MGC_Pipe *)hPipe;
    MGC_Port *pPort = (MGC_Port *)pPipe->hSession;
    uint8_t bIsTx = (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ? TRUE : FALSE;
    uint8_t bIsIn = pPort->bIsHost ? !bIsTx : bIsTx;

    return (pPort && pPort->pfProgramFlushEndpoint) ? pPort->pfProgramFlushEndpoint(pPort, pPipe->pLocalEnd,
            bIsIn ? MUSB_DIR_IN : 0, FALSE) : MUSB_STATUS_INVALID_ARGUMENT;
}

/*
 * Start a single bulk/interrupt transfer
 * Returns: 0 on success; error code on failure
 */
uint32_t MUSB_StartTransfer(MUSB_Irp *pIrp)
{
    void *pCurrentIrp;
    uint32_t status = MUSB_STATUS_OK;
    MGC_Pipe *pPipe = (MGC_Pipe *)pIrp->hPipe;
    MGC_Port *pPort = (MGC_Port *)pPipe->hSession;
    MUSB_SystemServices *pServices = pPort->pController->pSystemServices;
    MGC_EndpointResource *pEnd = pPipe->pLocalEnd;
    uint8_t bIsTx = (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ? TRUE : FALSE;
    uint8_t bTrafficType = bIsTx ? pEnd->bTrafficType : pEnd->bRxTrafficType;

    if(pPort->bIsHost)
    {
        /* lock end */
        pServices->pfLock(pServices->pPrivateData, 1 + pEnd->bLocalEnd);
    }

    switch(bTrafficType)
    {
    case MUSB_ENDPOINT_XFER_CONTROL:
    case MUSB_ENDPOINT_XFER_ISOC:
        status = MUSB_STATUS_INVALID_TYPE;
        break;

    case MUSB_ENDPOINT_XFER_BULK:
    case MUSB_ENDPOINT_XFER_INT:
        /* since lists use dynamic memory, try only if necessary */
        pCurrentIrp = bIsTx ? pEnd->pTxIrp : pEnd->pRxIrp;
        if(pCurrentIrp)
        {
            if(MUSB_ENDPOINT_XFER_INT == bTrafficType)
            {
                status = MUSB_STATUS_ENDPOINT_BUSY;
            }
            else if(bIsTx && !MUSB_ListAppendItem(&(pEnd->TxIrpList), pIrp, 0))
            {
                status = MUSB_STATUS_NO_MEMORY;
            }
            else if(!bIsTx && !MUSB_ListAppendItem(&(pEnd->RxIrpList), pIrp, 0))
            {
                status = MUSB_STATUS_NO_MEMORY;
            }
            /* if now there is no current IRP and we are at list head, the ISR didn't start us */
            if(bIsTx)
            {
                if(!((volatile MGC_EndpointResource *)pEnd)->pTxIrp &&
                        (pIrp == MUSB_ListFindItem(&(pEnd->TxIrpList), 0)))
                {
                    MGC_StartNextIrp(pPort, pEnd, TRUE);
                }
            }
            else
            {
                if(!((volatile MGC_EndpointResource *)pEnd)->pRxIrp &&
                        (pIrp == MUSB_ListFindItem(&(pEnd->RxIrpList), 0)))
                {
                    MGC_StartNextIrp(pPort, pEnd, FALSE);
                }
            }
        }
        else
        {
            /* nothing pending, so make it this */
            if(bIsTx)
            {
                /* only start a Tx if endpoint is not halted */
                if(!pEnd->bIsHalted)
                {
                    pPort->pfProgramStartTransmit(pPort, pEnd, pIrp->pBuffer,
                                                  pIrp->dwLength, pIrp);
                }
                else
                {
                    pEnd->pTxIrp = pIrp;
                }
            }
            else
            {
                pPort->pfProgramStartReceive(pPort, pEnd, pIrp->pBuffer,
                                             pIrp->dwLength, pIrp, pIrp->bAllowDma);
            }
        }

        break;
    }

    if(pPort->bIsHost)
    {
        /* unlock end */
        pServices->pfUnlock(pServices->pPrivateData, 1 + pEnd->bLocalEnd);
    }

    return status;
}

/*
 * Signal that new interrupt data is ready for transmission
 */
uint32_t MUSB_InterruptReady(MUSB_Irp *pIrp)
{
    MGC_Pipe *pPipe = (MGC_Pipe *)pIrp->hPipe;
    MGC_Port *pPort = (MGC_Port *)pPipe->hSession;
    MGC_EndpointResource *pEnd = pPipe->pLocalEnd;

    pPort->pfProgramStartTransmit(pPort, pEnd, pIrp->pBuffer, pIrp->dwLength, pIrp);
    return 0;
}

/*
 * Cancel a bulk/interrupt transfer
 * Returns: 0 on success; error code on failure
 */
uint32_t MUSB_CancelTransfer(MUSB_Irp *pIrp)
{
    uint8_t bDirection;
    uint32_t dwStatus = MUSB_STATUS_OK;
    MGC_Pipe *pPipe = (MGC_Pipe *)pIrp->hPipe;
    MGC_Port *pPort = (MGC_Port *)pPipe->hSession;
    MGC_EndpointResource *pEnd = pPipe->pLocalEnd;
    MUSB_SystemServices *pServices = pPort->pController->pSystemServices;
    uint8_t bIsTx = (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ? TRUE : FALSE;

    if(pPipe->bmFlags & MGC_PIPEFLAGS_HOST)
    {
        bDirection = bIsTx ? MUSB_DIR_OUT : MUSB_DIR_IN;
    }
    else
    {
        bDirection = bIsTx ? MUSB_DIR_IN : MUSB_DIR_OUT;
    }
    if((bIsTx && (pIrp == pEnd->pTxIrp)) || (!bIsTx && (pIrp == pEnd->pRxIrp)))
    {
        if (pPort->pfProgramFlushEndpoint)
        {
            dwStatus = pPort->pfProgramFlushEndpoint(pPort, pEnd, bDirection, FALSE);
        }

        if (bIsTx)
        {
            pEnd->pTxIrp = NULL;
        }
        else
        {
            pEnd->pRxIrp = NULL;
        }
    }
    else
    {
        if(pPort->bIsHost)
        {
            /* lock end */
            pServices->pfLock(pServices->pPrivateData, 1 + pEnd->bLocalEnd);
        }
        /* remove */
        if(bIsTx)
        {
            MUSB_ListRemoveItem(&(pEnd->TxIrpList), pIrp);
        }
        else
        {
            MUSB_ListRemoveItem(&(pEnd->RxIrpList), pIrp);
        }
        if(pPort->bIsHost)
        {
            /* unlock end */
            pServices->pfUnlock(pServices->pPrivateData, 1 + pEnd->bLocalEnd);
        }
    }
    MGC_StartNextIrp(pPort, pEnd, bIsTx);
    return dwStatus;
}

#ifdef MUSB_ISO
/*
 * Schedule an isochronous transfer
 * Returns: 0 on success; error code on failure
 */
uint32_t MUSB_ScheduleIsochTransfer(
    uint32_t dwWaitCount, MUSB_IsochIrp *pIsochIrp)
{
    uint8_t *pBuffer;
    uint32_t dwLength;
    uint32_t status = MUSB_STATUS_OK;
    MGC_Pipe *pPipe = (MGC_Pipe *)pIsochIrp->hPipe;
    MGC_Port *pPort = (MGC_Port *)pPipe->hSession;
    MUSB_SystemServices *pServices = pPort->pController->pSystemServices;
    MGC_EndpointResource *pEnd = pPipe->pLocalEnd;
    uint8_t bIsTx = (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ? TRUE : FALSE;
    uint8_t bTrafficType = pPipe->bTrafficType;
    void *pTestIrp = bIsTx ? pEnd->pTxIrp : pEnd->pRxIrp;

    if(pPort->bIsHost)
    {
        /* lock end */
        pServices->pfLock(pServices->pPrivateData, 1 + pEnd->bLocalEnd);
    }

    if(pTestIrp)
    {
        status = MUSB_STATUS_ENDPOINT_BUSY;
    }
    else
    {
        switch(bTrafficType)
        {
        case MUSB_ENDPOINT_XFER_CONTROL:
        case MUSB_ENDPOINT_XFER_BULK:
        case MUSB_ENDPOINT_XFER_INT:
            status = MUSB_STATUS_INVALID_TYPE;
            break;

        case MUSB_ENDPOINT_XFER_ISOC:
            pBuffer = pIsochIrp->pBuffer;
            dwLength = pIsochIrp->adwLength[0];
            /* TODO: really this should just go in schedule */
            if(bIsTx)
            {
                if(dwWaitCount)
                {
                    pEnd->dwWaitFrameCount = dwWaitCount;
                }
                else
                {
                    pPort->pfProgramStartTransmit(pPort, pEnd, pBuffer, dwLength, pIsochIrp);
                }
            }
            else
            {
                pPort->pfProgramStartReceive(pPort, pEnd, pBuffer, dwLength,
                                             pIsochIrp, pIsochIrp->bAllowDma);
            }
            break;

        }
    }

    if(pPort->bIsHost)
    {
        /* unlock end */
        pServices->pfUnlock(pServices->pPrivateData, 1 + pEnd->bLocalEnd);
    }

    return status;
}

/*
 * Cancel an isochronous transfer
 * Returns: 0 on success; error code on failure
 */
uint32_t MUSB_CancelIsochTransfer(MUSB_IsochIrp *pIsochIrp)
{
    uint8_t bDirection;
    MGC_Pipe *pPipe = (MGC_Pipe *)pIsochIrp->hPipe;
    MGC_Port *pPort = (MGC_Port *)pPipe->hSession;
    MGC_EndpointResource *pEnd = pPipe->pLocalEnd;

    if(pPipe->bmFlags & MGC_PIPEFLAGS_HOST)
    {
        bDirection = (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ?
                     MUSB_DIR_OUT : MUSB_DIR_IN;
    }
    else
    {
        bDirection = (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ?
                     MUSB_DIR_IN : MUSB_DIR_OUT;
    }

    return (pPort && pPort->pfProgramFlushEndpoint) ? pPort->pfProgramFlushEndpoint(pPort,
            pEnd, bDirection, FALSE) : TRUE;
}
#endif

