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
 * DMA implementation for high-speed controllers.
 * $Revision: 1.14 $
 */

#include "mu_impl.h"
#include "mu_hsdcf.h"
#include "mu_diag.h"
#include "mu_hdrdf.h"
#include "mu_pippr.h"
#include "mu_mem.h"

#include "include.h"
#include "mem_pub.h"

/****************************** CONSTANTS ********************************/

#define MGC_O_HSDMA_BASE    0x200
#define MGC_O_HSDMA_INTR    0x200

#define MGC_O_HSDMA_CONTROL 4
#define MGC_O_HSDMA_ADDRESS 8
#define MGC_O_HSDMA_COUNT   0xc

#define MGC_HSDMA_CHANNEL_OFFSET(_bChannel, _bOffset) (MGC_O_HSDMA_BASE + (_bChannel << 4) + _bOffset)

/* control register (16-bit): */
#define MGC_S_HSDMA_ENABLE	0
#define MGC_S_HSDMA_TRANSMIT	1
#define MGC_S_HSDMA_MODE1	2
#define MGC_S_HSDMA_IRQENABLE	3
#define MGC_S_HSDMA_ENDPOINT	4
#define MGC_S_HSDMA_BUSERROR	8
#define MGC_S_HSDMA_BURSTMODE	9
#define MGC_M_HSDMA_BURSTMODE	(3 << MGC_S_HSDMA_BURSTMODE)
#define MGC_HSDMA_BURSTMODE_UNSPEC  0
#define MGC_HSDMA_BURSTMODE_INCR4   1
#define MGC_HSDMA_BURSTMODE_INCR8   2
#define MGC_HSDMA_BURSTMODE_INCR16  3

/******************************** TYPES **********************************/

#if defined(MUSB_DMA) && defined(MUSB_HSDMA) && (MUSB_HSDMA_CHANNELS > 0)

struct _MGC_HsDmaController;

typedef struct
{
    MUSB_DmaChannel Channel;
    struct _MGC_HsDmaController *pController;
    uint32_t dwStartAddress;
    uint32_t dwCount;
    uint16_t wMaxPacketSize;
    uint8_t bIndex;
    uint8_t bEnd;
    uint8_t bProtocol;
    uint8_t bTransmit;
    uint8_t bMode;
} MGC_HsDmaChannel;

typedef struct _MGC_HsDmaController
{
    MUSB_DmaController Controller;
    MGC_HsDmaChannel aChannel[MUSB_HSDMA_CHANNELS];
    MUSB_pfSystemToBusAddress pfSystemToBusAddress;
    void *pSystemPrivate;
    MUSB_pfDmaChannelStatusChanged pfDmaChannelStatusChanged;
    void *pDmaPrivate;
    uint8_t *pCoreBaseIsr;
    uint8_t *pCoreBaseBsr;
    uint8_t bChannelCount;
    uint8_t bmUsedChannels;
} MGC_HsDmaController;

#endif	/* MUSB_HSDMA && have at least one channel */

/******************************* FORWARDS ********************************/

#if defined(MUSB_DMA) && defined(MUSB_HSDMA) && (MUSB_HSDMA_CHANNELS > 0)

static uint8_t MGC_HsDmaStartController(void *pPrivateData);

static uint8_t MGC_HsDmaStopController(void *pPrivateData);

static MUSB_DmaChannel *MGC_HsDmaAllocateChannel(void *pPrivateData,
        uint8_t bLocalEnd, uint8_t bTransmit, uint8_t bProtocol,
        uint16_t wMaxPacketSize);

static void MGC_HsDmaReleaseChannel(MUSB_DmaChannel *pChannel);

static uint8_t *MGC_HsDmaAllocateBuffer(MUSB_DmaChannel *pChannel,
                                        uint32_t dwLength);

static uint8_t MGC_HsDmaReleaseBuffer(MUSB_DmaChannel *pChannel,
                                      uint8_t *pBuffer);

static uint8_t MGC_HsDmaProgramChannel(MUSB_DmaChannel *pChannel,
                                       uint16_t wPacketSize, uint8_t bMode,
                                       const uint8_t *pBuffer, uint32_t dwLength);

static MUSB_DmaChannelStatus MGC_HsDmaGetChannelStatus(
    MUSB_DmaChannel *pChannel);

static uint8_t MGC_HsDmaControllerIsr(void *pPrivateData);

#endif	/* MUSB_HSDMA && have at least one channel */

static MUSB_DmaController *MGC_HsNewDmaController(
    MUSB_pfDmaChannelStatusChanged pfDmaChannelStatusChanged,
    void *pDmaPrivate,
    MUSB_pfSystemToBusAddress pfSystemToBusAddress, void *pSystemPrivate,
    uint8_t *pCoreBaseIsr, uint8_t *pCoreBaseBsr);

static void MGC_HsDestroyDmaController(MUSB_DmaController *pController);

/******************************* GLOBALS *********************************/

MUSB_DmaControllerFactory MUSB_HdrcDmaControllerFactory =
{
    0x300,
    MGC_HsNewDmaController,
    MGC_HsDestroyDmaController
};

/****************************** FUNCTIONS ********************************/

#if defined(MUSB_DMA) && defined(MUSB_HSDMA) && (MUSB_HSDMA_CHANNELS > 0)

static uint8_t MGC_HsDmaStartController(void *pPrivateData)
{
    /* nothing to do */
    return TRUE;
}

static uint8_t MGC_HsDmaStopController(void *pPrivateData)
{
    uint8_t bChannel;
    MGC_HsDmaChannel *pImplChannel;
    MGC_HsDmaController *pController = (MGC_HsDmaController *)pPrivateData;
    uint8_t *pBase = pController->pCoreBaseBsr;

    for(bChannel = 0; bChannel < MUSB_HSDMA_CHANNELS; bChannel++)
    {
        MGC_Write16(pBase,
                    MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_CONTROL), 0);
        pImplChannel = (MGC_HsDmaChannel *) & (pController->aChannel[bChannel]);
        pImplChannel->Channel.bStatus = MUSB_DMA_STATUS_FREE;
        pImplChannel->Channel.dwActualLength = 0;
    }
    pController->bmUsedChannels = 0;
    return TRUE;
}

static MUSB_DmaChannel *MGC_HsDmaAllocateChannel(void *pPrivateData,
        uint8_t bLocalEnd, uint8_t bTransmit, uint8_t bProtocol,
        uint16_t wMaxPacketSize)
{
    uint8_t bBit;
    MUSB_DmaChannel *pChannel = NULL;
    MGC_HsDmaChannel *pImplChannel = NULL;
    MGC_HsDmaController *pController = (MGC_HsDmaController *)pPrivateData;

    if(bLocalEnd && (MUSB_ENDPOINT_XFER_BULK == bProtocol))
    {
        for(bBit = 0; bBit < MUSB_HSDMA_CHANNELS; bBit++)
        {
            if(!(pController->bmUsedChannels & (1 << bBit)))
            {
                pController->bmUsedChannels |= (1 << bBit);
                pImplChannel = &(pController->aChannel[bBit]);
                pImplChannel->pController = pController;
                pImplChannel->wMaxPacketSize = wMaxPacketSize;
                pImplChannel->bIndex = bBit;
                pImplChannel->bEnd = bLocalEnd;
                pImplChannel->bProtocol = bProtocol;
                pImplChannel->bTransmit = bTransmit;
                pChannel = &(pImplChannel->Channel);
                pChannel->pPrivateData = pImplChannel;
                pChannel->bStatus = MUSB_DMA_STATUS_FREE;
                pChannel->dwMaxLength = bTransmit ? 0x10000 : wMaxPacketSize;
                /* Tx => mode 1; Rx => mode 0 */
                pChannel->bDesiredMode = bTransmit;
                break;
            }
        }
    }
    return pChannel;
}

static void MGC_HsDmaReleaseChannel(MUSB_DmaChannel *pChannel)
{
    MGC_HsDmaChannel *pImplChannel = (MGC_HsDmaChannel *)pChannel->pPrivateData;
    MGC_HsDmaController *pController = pImplChannel->pController;
    uint8_t *pBase = pController->pCoreBaseBsr;

    MGC_Write16(pBase,
                MGC_HSDMA_CHANNEL_OFFSET(pImplChannel->bIndex, MGC_O_HSDMA_CONTROL), 0);
    pImplChannel->pController->bmUsedChannels &= ~(1 << pImplChannel->bIndex);
    pImplChannel->Channel.bStatus = MUSB_DMA_STATUS_FREE;
}

static uint8_t *MGC_HsDmaAllocateBuffer(MUSB_DmaChannel *pChannel,
                                        uint32_t dwLength)
{
    /* TODO: this may require platform-specific implementation */
    return (uint8_t *)MUSB_MemAlloc(dwLength);
}

static uint8_t MGC_HsDmaReleaseBuffer(MUSB_DmaChannel *pChannel,
                                      uint8_t *pBuffer)
{
    /* TODO: this may require platform-specific implementation */
    MUSB_MemFree(pBuffer);
    return TRUE;
}

static uint8_t MGC_HsDmaProgramChannel(MUSB_DmaChannel *pChannel,
                                       uint16_t wPacketSize, uint8_t bMode,
                                       const uint8_t *pBuffer, uint32_t dwLength)
{
    uint16_t wIntrRxE;
    MGC_HsDmaChannel *pImplChannel = (MGC_HsDmaChannel *)pChannel->pPrivateData;
    MGC_HsDmaController *pController = pImplChannel->pController;
    uint8_t *pBase = bMode ? pController->pCoreBaseBsr : pController->pCoreBaseIsr;
    uint16_t wCsr = (pImplChannel->bEnd << MGC_S_HSDMA_ENDPOINT) | (1 << MGC_S_HSDMA_ENABLE);
    uint8_t bChannel = pImplChannel->bIndex;
    MGC_Port *pPort = (MGC_Port *)pController->pDmaPrivate;
    MGC_EndpointResource *pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(
                                     &(pPort->LocalEnds), pImplChannel->bEnd);
    MUSB_Irp *pIrp = pImplChannel->bTransmit ? (MUSB_Irp *)pEnd->pTxIrp : (MUSB_Irp *)pEnd->pRxIrp;
    uint32_t dwBoundary = 0x400 - ((uint32_t)pBuffer & 0x3ff);
    uint16_t wBurstSize = MGC_MIN((uint16_t)dwBoundary, pImplChannel->wMaxPacketSize);

    /* remember the core's mode */
    pImplChannel->bMode = bMode;

    /* reject below threshold (packet size) */
    if(dwLength < wPacketSize)
    {
        return FALSE;
    }

    /* we always use the DMAC's mode 1 */
    wCsr |= 1 << MGC_S_HSDMA_MODE1;

    /* set burst mode */
    if(wBurstSize >= 64)
    {
        wCsr |= MGC_HSDMA_BURSTMODE_INCR16 << MGC_S_HSDMA_BURSTMODE;
    }
    else if(wBurstSize >= 32)
    {
        wCsr |= MGC_HSDMA_BURSTMODE_INCR8 << MGC_S_HSDMA_BURSTMODE;
    }
    else if(wBurstSize >= 16)
    {
        wCsr |= MGC_HSDMA_BURSTMODE_INCR4 << MGC_S_HSDMA_BURSTMODE;
    }

    if(!pImplChannel->bTransmit && (!pIrp || (dwLength < (pIrp->dwLength - pEnd->dwRxOffset))))
    {
        if(pIrp && (dwLength < (pIrp->dwLength - pEnd->dwRxOffset)))
        {
            /* Rx and partial (so must be residual): disable endpoint interrupt */
            MUSB_DIAG2(3, "HSDMA: Disabling endpoint interrupt due to dwLength=",
                       dwLength, " < IRP's remaining length=", pIrp->dwLength - pEnd->dwRxOffset, 16, 0);
            wIntrRxE = MGC_Read16(pBase, MGC_O_HDRC_INTRRXE);
            MGC_Write16(pBase, MGC_O_HDRC_INTRRXE, wIntrRxE & ~(1 << pImplChannel->bEnd));
        }
    }

    if(pImplChannel->bTransmit)
    {
        wCsr |= 1 << MGC_S_HSDMA_TRANSMIT;
    }
    wCsr |= 1 << MGC_S_HSDMA_IRQENABLE;

    /* address/count */
    MGC_Write32(pBase,
                MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_ADDRESS),
                (uint32_t)pController->pfSystemToBusAddress(pController->pSystemPrivate, pBuffer));
    MGC_Write32(pBase,
                MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_COUNT), dwLength);

    /* control (this should start things) */
    pChannel->dwActualLength = 0L;
    pImplChannel->dwStartAddress = (uint32_t)pBuffer;
    pImplChannel->dwCount = dwLength;
    MGC_Write16(pBase,
                MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_CONTROL), wCsr);

    return TRUE;
}

static MUSB_DmaChannelStatus MGC_HsDmaGetChannelStatus(
    MUSB_DmaChannel *pChannel)
{
    uint32_t dwCount;
    MGC_HsDmaChannel *pImplChannel = (MGC_HsDmaChannel *)pChannel->pPrivateData;
    MGC_HsDmaController *pController = pImplChannel->pController;
    uint8_t *pBase = pController->pCoreBaseIsr;
    uint8_t bChannel = pImplChannel->bIndex;
    uint16_t wCsr = MGC_Read16(pBase,
                               MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_CONTROL));

    if(wCsr & (1 << MGC_S_HSDMA_BUSERROR))
    {
        return MUSB_DMA_STATUS_BUS_ABORT;
    }
    dwCount = MGC_Read32(pBase, MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_COUNT));
    pChannel->dwActualLength = pImplChannel->dwCount - dwCount;
    if(dwCount)
    {
        return MUSB_DMA_STATUS_BUSY;
    }
    return MUSB_DMA_STATUS_FREE;
}

static uint8_t MGC_HsDmaControllerIsr(void *pPrivateData)
{
    uint8_t bAllowDma = 0;
    MGC_BsrItem qItem;
    MUSB_Irp *pIrp;
#ifdef MUSB_ISO
    MUSB_IsochIrp *pIsochIrp;
#endif
    MGC_EndpointResource *pEnd;
    uint8_t bChannel;
    uint16_t wCsr;
    uint16_t wIntrRxE;
    uint32_t dwCount;
    MGC_HsDmaChannel *pImplChannel;
    MGC_HsDmaController *pController = (MGC_HsDmaController *)pPrivateData;
    uint8_t *pBuffer = NULL;
    uint32_t dwReqLength = 0;
    uint8_t bComplete = FALSE;
    uint32_t dwStatus = MUSB_STATUS_OK;
    MGC_Port *pPort = (MGC_Port *)pController->pDmaPrivate;
    MUSB_SystemServices *pServices = pPort->pController->pSystemServices;
    uint8_t *pBase = pController->pCoreBaseIsr;
    uint8_t bIntr = MGC_Read8(pBase, MGC_O_HSDMA_INTR);

    if(!bIntr)
    {
        return FALSE;
    }
    for(bChannel = 0; bChannel < MUSB_HSDMA_CHANNELS; bChannel++)
    {
        if(bIntr & (1 << bChannel))
        {
            pImplChannel = (MGC_HsDmaChannel *) & (pController->aChannel[bChannel]);
            wCsr = MGC_Read16(pBase,
                              MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_CONTROL));
            dwCount = MGC_Read32(pBase, MGC_HSDMA_CHANNEL_OFFSET(bChannel, MGC_O_HSDMA_COUNT));
            pImplChannel->Channel.dwActualLength = pImplChannel->dwCount - dwCount;

            if(wCsr & (1 << MGC_S_HSDMA_BUSERROR))
            {
                pImplChannel->Channel.bStatus = MUSB_DMA_STATUS_BUS_ABORT;
                dwStatus = MUSB_STATUS_DMA_ERROR;
            }
            else
            {
                pImplChannel->Channel.bStatus = MUSB_DMA_STATUS_FREE;
            }

            if(pImplChannel->bMode || pImplChannel->bTransmit)
            {
                if(!pImplChannel->bMode && pImplChannel->bTransmit &&
                        (pImplChannel->dwCount < pImplChannel->wMaxPacketSize))
                {
                    MGC_SelectEnd(pBase, pImplChannel->bEnd);
                    wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_TXCSR, pImplChannel->bEnd);
                    wCsr |= MGC_M_TXCSR_TXPKTRDY;
                    MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, pImplChannel->bEnd, wCsr);
                }
                else
                {
                    pController->pfDmaChannelStatusChanged(pController->pDmaPrivate,
                                                           pImplChannel->bEnd, pImplChannel->bTransmit);
                }
            }
            else if(!pImplChannel->bTransmit)
            {
                /*
                 * NOTE: assume mode 0 implies the residual handling case,
                 * and therefore use our NASTY hack, because this is all
                 * specific to the Inventra DMAC
                 */
                pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(
                           &(pPort->LocalEnds), pImplChannel->bEnd);
                pEnd->dwRxOffset += pImplChannel->dwCount;
                pEnd->bDmaResidual = FALSE;

                switch(pEnd->bRxTrafficType)
                {
                case MUSB_ENDPOINT_XFER_BULK:
                case MUSB_ENDPOINT_XFER_INT:
                    pIrp = (MUSB_Irp *)pEnd->pRxIrp;
                    bAllowDma = pIrp->bAllowDma;
                    pBuffer = pIrp->pBuffer + pEnd->dwRxOffset;
                    dwReqLength = pIrp->dwLength;
                    if((pEnd->dwRxOffset >= pIrp->dwLength) ||
                            (pImplChannel->dwCount < pEnd->wRxPacketSize))
                    {
                        bComplete = TRUE;
                    }
                    break;
#ifdef MUSB_ISO
                case MUSB_ENDPOINT_XFER_ISOC:
                    bAllowDma = pIsochIrp->bAllowDma;
                    pIsochIrp = (MUSB_IsochIrp *)pEnd->pRxIrp;
                    pBuffer = pIsochIrp->pBuffer + pEnd->dwRxOffset;
                    bComplete = (pIsochIrp->wCurrentFrame >= pIsochIrp->wFrameCount) ? TRUE : FALSE;
                    break;
#endif
                }

                /* re-enable endpoint interrupt */
                wIntrRxE = MGC_Read16(pBase, MGC_O_HDRC_INTRRXE);
                MGC_Write16(pBase, MGC_O_HDRC_INTRRXE, wIntrRxE | (1 << pImplChannel->bEnd));

                if(bComplete)
                {
                    if(MGC_CompleteIrp(&qItem, pEnd, dwStatus, FALSE, pEnd->pRxIrp))
                    {
                        pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
                    }
                    /* start next IRP if any */
                    MGC_StartNextIrp(pPort, pEnd, FALSE);
                }
                else
                {
                    pPort->pfProgramStartReceive(pPort, pEnd, pBuffer,
                                                 (dwReqLength - pEnd->dwRxOffset), pEnd->pRxIrp, bAllowDma);
                }

            }
        }
    }
    return TRUE;
}

#endif	/* MUSB_HSDMA && have at least one channel */

static MUSB_DmaController *MGC_HsNewDmaController(
    MUSB_pfDmaChannelStatusChanged pfDmaChannelStatusChanged,
    void *pDmaPrivate,
    MUSB_pfSystemToBusAddress pfSystemToBusAddress,
    void *pSystemPrivate,
    uint8_t *pCoreBaseIsr, uint8_t *pCoreBaseBsr)
{
    MUSB_DmaController *pResult = NULL;
#if defined(MUSB_DMA) && defined(MUSB_HSDMA) && (MUSB_HSDMA_CHANNELS > 0)
    MGC_HsDmaController *pController = (MGC_HsDmaController *)MUSB_MemAlloc(
                                           sizeof(MGC_HsDmaController));
    if(pController)
    {
        MUSB_MemSet(pController, 0, sizeof(MGC_HsDmaController));
        pController->bChannelCount = MUSB_HSDMA_CHANNELS;
        pController->pfSystemToBusAddress = pfSystemToBusAddress;
        pController->pSystemPrivate = pSystemPrivate;
        pController->pfDmaChannelStatusChanged = pfDmaChannelStatusChanged;
        pController->pDmaPrivate = pDmaPrivate;
        pController->pCoreBaseIsr = pCoreBaseIsr;
        pController->pCoreBaseBsr = pCoreBaseBsr;
        pController->Controller.pPrivateData = pController;
        pController->Controller.pfDmaStartController = MGC_HsDmaStartController;
        pController->Controller.pfDmaStopController = MGC_HsDmaStopController;
        pController->Controller.pfDmaAllocateChannel = MGC_HsDmaAllocateChannel;
        pController->Controller.pfDmaReleaseChannel = MGC_HsDmaReleaseChannel;
        pController->Controller.pfDmaAllocateBuffer = MGC_HsDmaAllocateBuffer;
        pController->Controller.pfDmaReleaseBuffer = MGC_HsDmaReleaseBuffer;
        pController->Controller.pfDmaProgramChannel = MGC_HsDmaProgramChannel;
        pController->Controller.pfDmaGetChannelStatus = MGC_HsDmaGetChannelStatus;
        pController->Controller.pfDmaControllerIsr = MGC_HsDmaControllerIsr;
        pResult = &(pController->Controller);
    }
#endif	/* MUSB_HSDMA && have at least one channel */
    return pResult;
}

static void MGC_HsDestroyDmaController(MUSB_DmaController *pController)
{
#if defined(MUSB_DMA) && defined(MUSB_HSDMA) && (MUSB_HSDMA_CHANNELS > 0)
    MGC_HsDmaController *pHsController =
        (MGC_HsDmaController *)pController->pPrivateData;

    if(pHsController)
    {
        pHsController->Controller.pPrivateData = NULL;
        MUSB_MemFree(pHsController);
    }
#endif	/* MUSB_HSDMA && have at least one channel */
}

