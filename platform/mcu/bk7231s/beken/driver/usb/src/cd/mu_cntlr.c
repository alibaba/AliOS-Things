/******************************************************************
*                                                                *
*          Copyright Mentor Graphics Corporation 2006            *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/
#include "mu_impl.h"
#include "mu_cntlr.h"
#include "mu_diag.h"
#include "mu_mem.h"
#include "include.h"

#if CFG_USB

#ifdef MUSB_FDRC
#include "mu_mdrpr.h"
#endif

#ifndef MUSB_IRP_QUEUE_LENGTH
#define MUSB_IRP_QUEUE_LENGTH	64  /* double-buffered, all endpoints both directions */
#endif

#ifdef MUSB_FDRC
#include "mu_fdrpr.h"
#ifdef MUSB_DMA
extern MUSB_DmaControllerFactory MUSB_FdrcDmaControllerFactory;
static MUSB_DmaControllerFactory *MGC_pFdrcDmaControllerFactory = &MUSB_FdrcDmaControllerFactory;
void MUSB_SetFsAhbDmaControllerFactory(MUSB_DmaControllerFactory *pFactory)
{
    MGC_pFdrcDmaControllerFactory = pFactory;
}
#endif
#endif

#if defined(MUSB_HDRC) || defined(MUSB_MHDRC) || defined(MUSB_HSFC)
#include "mu_hdrpr.h"
#ifdef MUSB_DMA
extern MUSB_DmaControllerFactory MUSB_HdrcDmaControllerFactory;
static MUSB_DmaControllerFactory *MGC_pHdrcDmaControllerFactory = &MUSB_HdrcDmaControllerFactory;
void MUSB_SetHsDmaControllerFactory(MUSB_DmaControllerFactory *pFactory)
{
    MGC_pHdrcDmaControllerFactory = pFactory;
}
#endif
#endif
/***************************** TYPES ******************************/

/**
 * All interface and implementation controller/port structs
 * together (to reduce mallocs).
 * NOTE: code assumes this member order;
 * if you change member order, change code.
 * @field ControllerImpl controller implementation struct
 * @field Controller controller interface struct
 * @field PortImpl port implementation struct
 * @field Port port interface struct
 */
typedef struct
{
    MGC_Controller ControllerImpl;
    MUSB_Controller Controller;

    MGC_Port PortImpl;
    MUSB_Port Port;
} MGC_ControllerWrapper;

/**************************** GLOBALS *****************************/

/** Requirements for one 10-millisecond timer */
static uint32_t MGC_aOneTimer10Ms[] = { 10 };

/** List of all controllers */
static MUSB_LinkedList MGC_ControllerList;

/** Next controller index */
static uint16_t MGC_iController = 0;

#if MUSB_DIAG > 0
/** Diagnostic message buffer */
char MGC_DiagMsg[MGC_DIAG_MSG_SIZE];
/** Diagnostic number buffer */
char MGC_DiagNumber[MGC_DIAG_NUM_SIZE];
/** Diagnostic level */
uint8_t MGC_bDiagLevel = MUSB_DIAG;
#endif

/*************************** FUNCTIONS ****************************/

/*
 * Implementation
 */
void MUSB_SetDiagnosticLevel(uint8_t bLevel)
{
#if MUSB_DIAG > 0
    MGC_bDiagLevel = bLevel;
#endif
}

static void MGC_ControllerInit(MGC_ControllerWrapper *pWrapper,
                               void *pControllerAddressIsr,
                               void *pControllerAddressIst)
{
    pWrapper->ControllerImpl.pControllerAddressIsr = pControllerAddressIsr;
    pWrapper->ControllerImpl.pControllerAddressIst = pControllerAddressIst;
    pWrapper->ControllerImpl.pPort = &(pWrapper->PortImpl);

#if defined(MUSB_HOST) || defined(MUSB_OTG)
    MUSB_ListInit(&(pWrapper->PortImpl.ConnectedDeviceList));
    MUSB_ListInit(&(pWrapper->PortImpl.Schedule.ScheduleSlots));
#endif

    pWrapper->PortImpl.pInterfacePort = &(pWrapper->Port);
    pWrapper->Controller.wVersion = MUSB_CONTROLLER_VERSION;
    pWrapper->Controller.pPrivateData = &(pWrapper->ControllerImpl);
    pWrapper->Port.bcdCdiRev = MUSB_CDI_REVISION;
    pWrapper->Port.pPrivateData = &(pWrapper->PortImpl);
    pWrapper->PortImpl.pController = &(pWrapper->ControllerImpl);

#ifdef MUSB_OVERHEAD
    pWrapper->PortImpl.IsrOverhead.dwOverheadMin = 0xffffffffL;
    pWrapper->PortImpl.IsrOverhead.dwOverheadMax = 0L;
#endif
}

static uint16_t MGC_DiscoverController(void *pBase)
{
#ifdef MUSB_AHB_ID
    uint8_t bData, bData2;
#endif
    uint16_t wType = 0;

#ifdef MUSB_AHB_ID
    bData = MGC_Read8(pBase, 0x400);
    if('M' == bData)
    {
        wType = MUSB_CONTROLLER_MHDRC;
    }
    else
    {
        bData2 = MGC_Read8(pBase, 0x401);
        if('H' == bData)
        {
            if('D' == bData2)
            {
                wType = MUSB_CONTROLLER_HDRC;
            }
            else
            {
                bData2 = MGC_Read8(pBase, 0x402);
                if('F' == bData2)
                {
                    wType = MUSB_CONTROLLER_HSFC;
                }
            }
        }
        else if('F' == bData)
        {
            if('D' == bData2)
            {
                wType = MUSB_CONTROLLER_FDRC;
            }
            else
            {
                bData2 = MGC_Read8(pBase, 0x402);
                if('F' == bData2)
                {
                    wType = MUSB_CONTROLLER_FSFC;
                }
            }
        }
    }
#endif

    return wType;
}

/*
 * Instantiate a controller
 动态分配MGC_ControllerWrapper结构体，并返回结构体中成员变量Controller的地址
 */
MUSB_Controller *MUSB_NewController(MUSB_SystemUtils *pUtils,
                                    uint16_t wControllerType,
                                    void *pControllerAddressIsr,
                                    void *pControllerAddressBsr
                                   )
{
#ifdef MUSB_STATS
    void *pOverheads;
    MGC_OverheadStats Stats;
#endif
    uint8_t bEnd;
    uint8_t bOK = FALSE;
    MGC_EndpointResource *pEnd;
    MUSB_Controller *pResult = NULL;
    uint16_t wType = wControllerType;
    static MGC_ControllerWrapper pWrapper;

    MUSB_MemSet(&pWrapper, 0, sizeof(MGC_ControllerWrapper));
    if(!MUSB_ListAppendItem(&MGC_ControllerList, &pWrapper, 0))
    {
    }
    else
    {
        MGC_iController++;
    }

    pWrapper.ControllerImpl.pUtils = pUtils;
    /* common init */
    MGC_ControllerInit(&pWrapper, pControllerAddressIsr,
                       pControllerAddressBsr);

    /* discover controller type if not given */
    if(!wType)
    {
        wType = MGC_DiscoverController(pControllerAddressBsr);
    }

    /* initialize data & dispatch table */
    switch(wType)
    {
#ifdef MUSB_FDRC
    case MUSB_CONTROLLER_FDRC:
        MUSB_DIAG1(2, "Initializing FDRC at ", (uint32_t)pControllerAddressIsr, 16, 8);
        /* endpoint config discovery, etc. */
        bOK = MGC_FdrcInit(&(pWrapper.PortImpl));
        /* fill info for system */
        pWrapper.Controller.wQueueLength = MUSB_IRP_QUEUE_LENGTH + 16;
        pWrapper.Controller.wQueueItemSize = sizeof(MGC_BsrItem);
        pWrapper.Controller.wTimerCount = 1;
        pWrapper.Controller.adwTimerResolutions = MGC_aOneTimer10Ms;
        pWrapper.Controller.wLockCount = 17;
        pWrapper.Controller.pfIsr = MGC_FdrcIsr;
        pWrapper.Controller.pIsrParam = &(pWrapper.Controller);
        pWrapper.Controller.pfBsr = MGC_DrcBsr;
        pWrapper.Controller.pBsrParam = &(pWrapper.Port);

#ifdef MUSB_DMA
        pWrapper->ControllerImpl.pDmaControllerFactory =
            MGC_pFdrcDmaControllerFactory;
#endif

        /* fill functions for generic code */
        pWrapper.ControllerImpl.pfProgramStartController = MGC_FdrcStart;
        pWrapper.ControllerImpl.pfProgramStopController = MGC_FdrcStop;
        pWrapper.ControllerImpl.pfDestroyController = MGC_FdrcDestroy;
        pWrapper.ControllerImpl.pfSetControllerHostPower = MGC_DrcSetHostPower;
#if MUSB_DIAG > 0
        pWrapper.ControllerImpl.pfDumpControllerState = MGC_FdrcDumpState;
        pWrapper.ControllerImpl.pfDumpPipeState = MGC_DrcDumpPipe;
        pWrapper.ControllerImpl.pfDumpLocalEndState = MGC_FdrcDumpEndpoint;
#endif	/* diagnostics enabled */
        pWrapper.PortImpl.pfReadBusState = MGC_FdrcReadBusState;
        pWrapper.PortImpl.pfProgramBusState = MGC_FdrcProgramBusState;
        pWrapper.PortImpl.pfResetPort = MGC_DrcResetPort;
        pWrapper.PortImpl.pfBindEndpoint = MGC_FdrcBindEndpoint;
        pWrapper.PortImpl.pfProgramStartReceive = MGC_FdrcStartRx;
        pWrapper.PortImpl.pfProgramStartTransmit = MGC_FdrcStartTx;
        pWrapper.PortImpl.pfProgramFlushEndpoint = MGC_FdrcFlushEndpoint;
        pWrapper.PortImpl.pfProgramHaltEndpoint = MGC_FdrcHaltEndpoint;
        pWrapper.PortImpl.pfDefaultEndResponse = MGC_FdrcDefaultEndResponse;
        pWrapper.PortImpl.pfServiceDefaultEnd = MGC_FdrcServiceDefaultEnd;
        pWrapper.PortImpl.pfServiceReceiveReady = MGC_FdrcServiceReceiveReady;
        pWrapper.PortImpl.pfServiceTransmitAvail = MGC_FdrcServiceTransmitAvail;
        pWrapper.PortImpl.pfLoadFifo = MGC_FdrcLoadFifo;
        pWrapper.PortImpl.pfUnloadFifo = MGC_FdrcUnloadFifo;
        pWrapper.PortImpl.pfAcceptDevice = MGC_DrcAcceptDevice;
        pWrapper.PortImpl.pfDmaChannelStatusChanged = MGC_FdrcDmaChannelStatusChanged;
        pWrapper.PortImpl.pfDynamicFifoLocation = MGC_FdrcDynamicFifoLocation;
        pWrapper.Port.Type = MUSB_PORT_TYPE_OTG;
        pWrapper.Port.Speed = MUSB_PORT_SPEED_FULL;
        break;
#endif	/* FDRC */

#ifdef MUSB_HDRC
    case MUSB_CONTROLLER_HDRC:
        MUSB_DIAG1(2, "Initializing HDRC at ", (uint32_t)pControllerAddressIsr, 16, 8);
        /* endpoint config discovery, etc. */
        bOK = MGC_HdrcInit(&(pWrapper->PortImpl));
#ifndef MUSB_FORCE_FULLSPEED
        pWrapper->PortImpl.bWantHighSpeed = TRUE;
#endif
        /* fill info for system */
        pWrapper.Controller.wQueueLength = MUSB_IRP_QUEUE_LENGTH + 16;
        pWrapper.Controller.wQueueItemSize = sizeof(MGC_BsrItem);
        pWrapper.Controller.wTimerCount = 1;
        pWrapper.Controller.adwTimerResolutions = MGC_aOneTimer10Ms;
        pWrapper.Controller.wLockCount = 17;
        pWrapper.Controller.pfIsr = MGC_HdrcIsr;
        pWrapper.Controller.pIsrParam = &(pWrapper->Controller);
        pWrapper.Controller.pfBsr = MGC_DrcBsr;
        pWrapper.Controller.pBsrParam = &(pWrapper->Port);

#ifdef MUSB_DMA
        pWrapper->ControllerImpl.pDmaControllerFactory =
            MGC_pHdrcDmaControllerFactory;
#endif

        /* fill functions for generic code */
        pWrapper.ControllerImpl.pfProgramStartController = MGC_HdrcStart;
        pWrapper.ControllerImpl.pfProgramStopController = MGC_HdrcStop;
        pWrapper.ControllerImpl.pfDestroyController = MGC_HdrcDestroy;
        pWrapper.ControllerImpl.pfSetControllerHostPower = MGC_DrcSetHostPower;
#if MUSB_DIAG > 0
        pWrapper.ControllerImpl.pfDumpControllerState = MGC_HdrcDumpState;
        pWrapper.ControllerImpl.pfDumpPipeState = MGC_DrcDumpPipe;
        pWrapper.ControllerImpl.pfDumpLocalEndState = MGC_HdrcDumpEndpoint;
#endif	/* diagnostics enabled */
        pWrapper.PortImpl.pfReadBusState = MGC_HdrcReadBusState;
        pWrapper.PortImpl.pfProgramBusState = MGC_HdrcProgramBusState;
        pWrapper.PortImpl.pfResetPort = MGC_DrcResetPort;
        pWrapper.PortImpl.pfBindEndpoint = MGC_HdrcBindEndpoint;
        pWrapper.PortImpl.pfProgramStartReceive = MGC_HdrcStartRx;
        pWrapper.PortImpl.pfProgramStartTransmit = MGC_HdrcStartTx;
        pWrapper.PortImpl.pfProgramFlushEndpoint = MGC_HdrcFlushEndpoint;
        pWrapper.PortImpl.pfProgramHaltEndpoint = MGC_HdrcHaltEndpoint;
        pWrapper.PortImpl.pfDefaultEndResponse = MGC_HdrcDefaultEndResponse;
        pWrapper.PortImpl.pfServiceDefaultEnd = MGC_HdrcServiceDefaultEnd;
        pWrapper.PortImpl.pfServiceReceiveReady = MGC_HdrcServiceReceiveReady;
        pWrapper.PortImpl.pfServiceTransmitAvail = MGC_HdrcServiceTransmitAvail;
        pWrapper.PortImpl.pfLoadFifo = MGC_HdrcLoadFifo;
        pWrapper.PortImpl.pfUnloadFifo = MGC_HdrcUnloadFifo;
        pWrapper.PortImpl.pfAcceptDevice = MGC_DrcAcceptDevice;
        pWrapper.PortImpl.pfDmaChannelStatusChanged = MGC_HdrcDmaChannelStatusChanged;
        pWrapper.PortImpl.pfSetPortTestMode = MGC_HdrcSetPortTestMode;
        pWrapper.PortImpl.pfDynamicFifoLocation = MGC_HdrcDynamicFifoLocation;
        pWrapper.Port.Type = MUSB_PORT_TYPE_OTG;
        pWrapper.Port.Speed = MUSB_PORT_SPEED_HIGH;
        break;
#endif	/* HDRC */

#ifdef MUSB_MHDRC
    case MUSB_CONTROLLER_MHDRC:
        MUSB_DIAG1(2, "Initializing MHDRC at ", (uint32_t)pControllerAddressIsr, 16, 8);
        /* endpoint config discovery, etc. */
        bOK = MGC_MhdrcInit(&(pWrapper.PortImpl));

#ifdef MUSB_FORCE_FULLSPEED
        pWrapper.PortImpl.bWantHighSpeed = FALSE;
#else
        pWrapper.PortImpl.bWantHighSpeed = TRUE;
#endif
        pWrapper.PortImpl.bIsMultipoint = FALSE; // TRUE wangzhilei

        /* fill info for system */
        pWrapper.Controller.wQueueLength = MUSB_IRP_QUEUE_LENGTH + 16;
        pWrapper.Controller.wQueueItemSize = sizeof(MGC_BsrItem);
        pWrapper.Controller.wTimerCount = 1;
        pWrapper.Controller.adwTimerResolutions = MGC_aOneTimer10Ms;
        pWrapper.Controller.wLockCount = 17;
        pWrapper.Controller.pfIsr = MGC_HdrcIsr;
        pWrapper.Controller.pIsrParam = &(pWrapper.Controller);
        pWrapper.Controller.pfBsr = MGC_DrcBsr;
        pWrapper.Controller.pBsrParam = &(pWrapper.Port);

#ifdef MUSB_DMA
        pWrapper.ControllerImpl.pDmaControllerFactory =
            MGC_pHdrcDmaControllerFactory;
#endif

        /* fill functions for generic code */
        pWrapper.ControllerImpl.pfProgramStartController = MGC_HdrcStart;
        pWrapper.ControllerImpl.pfProgramStopController = MGC_HdrcStop;
        pWrapper.ControllerImpl.pfDestroyController = MGC_HdrcDestroy;
        pWrapper.ControllerImpl.pfSetControllerHostPower = MGC_DrcSetHostPower;
#if MUSB_DIAG > 0
        pWrapper.ControllerImpl.pfDumpControllerState = MGC_MhdrcDumpState;
        pWrapper.ControllerImpl.pfDumpPipeState = MGC_DrcDumpPipe;
        pWrapper.ControllerImpl.pfDumpLocalEndState = MGC_MhdrcDumpEndpoint;
#endif	/* diagnostics enabled */
        pWrapper.PortImpl.pfReadBusState = MGC_HdrcReadBusState;
        pWrapper.PortImpl.pfProgramBusState = MGC_HdrcProgramBusState;
        pWrapper.PortImpl.pfResetPort = MGC_DrcResetPort;
        pWrapper.PortImpl.pfBindEndpoint = MGC_MhdrcBindEndpoint;
        pWrapper.PortImpl.pfProgramStartReceive = MGC_MhdrcStartRx;
        pWrapper.PortImpl.pfProgramStartTransmit = MGC_MhdrcStartTx;
        pWrapper.PortImpl.pfProgramFlushEndpoint = MGC_HdrcFlushEndpoint;
        pWrapper.PortImpl.pfProgramHaltEndpoint = MGC_HdrcHaltEndpoint;
        pWrapper.PortImpl.pfDefaultEndResponse = MGC_HdrcDefaultEndResponse;
        pWrapper.PortImpl.pfServiceDefaultEnd = MGC_HdrcServiceDefaultEnd;
        pWrapper.PortImpl.pfServiceReceiveReady = MGC_HdrcServiceReceiveReady;
        pWrapper.PortImpl.pfServiceTransmitAvail = MGC_HdrcServiceTransmitAvail;
        pWrapper.PortImpl.pfLoadFifo = MGC_HdrcLoadFifo;
        pWrapper.PortImpl.pfUnloadFifo = MGC_HdrcUnloadFifo;
        pWrapper.PortImpl.pfAcceptDevice = MGC_DrcAcceptDevice;
        pWrapper.PortImpl.pfDmaChannelStatusChanged = MGC_HdrcDmaChannelStatusChanged;
        pWrapper.PortImpl.pfSetPortTestMode = MGC_HdrcSetPortTestMode;
        pWrapper.PortImpl.pfDynamicFifoLocation = MGC_HdrcDynamicFifoLocation;
        pWrapper.Port.Type = MUSB_PORT_TYPE_OTG;
        pWrapper.Port.Speed = MUSB_PORT_SPEED_HIGH;
        break;
#endif	/* MHDRC */

#ifdef MUSB_HSFC
    case MUSB_CONTROLLER_HSFC:
        MUSB_DIAG1(2, "Initializing HSFC at ", (uint32_t)pControllerAddressIsr, 16, 8);
        /* endpoint config discovery, etc. */
        bOK = MGC_HsfcInit(&(pWrapper->PortImpl));
#ifndef MUSB_FORCE_FULLSPEED
        pWrapper.PortImpl.bWantHighSpeed = TRUE;
#endif
        /* fill info for system */
        pWrapper.Controller.wQueueLength = MUSB_IRP_QUEUE_LENGTH + 8;
        pWrapper.Controller.wQueueItemSize = sizeof(MGC_BsrItem);
        pWrapper.Controller.wTimerCount = 0;
        pWrapper.Controller.adwTimerResolutions = NULL;
        pWrapper.Controller.wLockCount = 17;
        pWrapper.Controller.pfIsr = MGC_HsfcIsr;
        pWrapper.Controller.pIsrParam = &(pWrapper->Controller);
        pWrapper.Controller.pfBsr = MGC_FcBsr;
        pWrapper.Controller.pBsrParam = &(pWrapper->Port);

#ifdef MUSB_DMA
        pWrapper.ControllerImpl.pDmaControllerFactory =
            MGC_pHdrcDmaControllerFactory;
#endif

        /* fill functions for generic code */
        pWrapper.ControllerImpl.pfProgramStartController = MGC_HsfcStart;
        pWrapper.ControllerImpl.pfProgramStopController = MGC_HsfcStop;
        pWrapper.ControllerImpl.pfDestroyController = MGC_HsfcDestroy;
#if MUSB_DIAG > 0
        pWrapper.ControllerImpl.pfDumpControllerState = MGC_HsfcDumpState;
        pWrapper.ControllerImpl.pfDumpPipeState = MGC_FcDumpPipe;
        pWrapper.ControllerImpl.pfDumpLocalEndState = MGC_HsfcDumpEndpoint;
#endif	/* diagnostics enabled */
        pWrapper.PortImpl.pfReadBusState = MGC_HdrcReadBusState;
        pWrapper.PortImpl.pfProgramBusState = MGC_HdrcProgramBusState;
        pWrapper.PortImpl.pfResetPort = MGC_FcResetPort;
        pWrapper.PortImpl.pfBindEndpoint = MGC_HsfcBindEndpoint;
        pWrapper.PortImpl.pfProgramStartReceive = MGC_HsfcStartRx;
        pWrapper.PortImpl.pfProgramStartTransmit = MGC_HsfcStartTx;
        pWrapper.PortImpl.pfProgramFlushEndpoint = MGC_HsfcFlushEndpoint;
        pWrapper.PortImpl.pfProgramHaltEndpoint = MGC_HsfcHaltEndpoint;
        pWrapper.PortImpl.pfDefaultEndResponse = MGC_HsfcDefaultEndResponse;
        pWrapper.PortImpl.pfServiceDefaultEnd = MGC_HsfcServiceDefaultEnd;
        pWrapper.PortImpl.pfServiceReceiveReady = MGC_HsfcServiceReceiveReady;
        pWrapper.PortImpl.pfServiceTransmitAvail = MGC_HsfcServiceTransmitAvail;
        pWrapper.PortImpl.pfLoadFifo = MGC_HsfcLoadFifo;
        pWrapper.PortImpl.pfUnloadFifo = MGC_HsfcUnloadFifo;
        pWrapper.PortImpl.pfAcceptDevice = NULL;
        pWrapper.PortImpl.pfDmaChannelStatusChanged = MGC_HsfcDmaChannelStatusChanged;
        pWrapper.PortImpl.pfSetPortTestMode = MGC_HsfcSetPortTestMode;
        pWrapper.PortImpl.pfDynamicFifoLocation = MGC_HsfcDynamicFifoLocation;
        pWrapper.Port.Type = MUSB_PORT_TYPE_FUNCTION;
        pWrapper.Port.Speed = MUSB_PORT_SPEED_HIGH;
        break;
#endif	/* HDRC */
    }	/* switch on controller type */

    if(bOK)
    {

#if MUSB_DIAG > 0
        MUSB_DIAG_STRING(2, "Configuration:");
        if(pWrapper->PortImpl.bHasBulkCombine)
        {
            MUSB_DIAG_STRING(2, "  Bulk packeting combining");
        }
        if(pWrapper->PortImpl.bHasBulkSplit)
        {
            MUSB_DIAG_STRING(2, "  Bulk packeting splitting");
        }
        if(pWrapper->PortImpl.bHasHbIsoRx)
        {
            MUSB_DIAG_STRING(2, "  High-bandwidth isochronous Rx");
        }
        if(pWrapper->PortImpl.bHasHbIsoTx)
        {
            MUSB_DIAG_STRING(2, "  High-bandwidth isochronous Tx");
        }
        for(bEnd = 1; bEnd < pWrapper->PortImpl.bEndCount; bEnd++)
        {
            pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pWrapper->PortImpl.LocalEnds), bEnd);
            MUSB_DIAG2(2, "  Endpoint=", bEnd, " / Shared FIFO=", pEnd->bIsFifoShared, 10, 0);
            MUSB_DIAG2(2, "    Tx FIFO size=", pEnd->wMaxPacketSizeTx, " / Rx FIFO size=",
                       pEnd->wMaxPacketSizeRx, 16, 4);
        }
#endif

        /* initialize IRP lists */
        for(bEnd = 0; bEnd < pWrapper.PortImpl.bEndCount; bEnd++)
        {
            pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pWrapper.PortImpl.LocalEnds), bEnd);
            MUSB_ListInit(&(pEnd->TxIrpList));
            MUSB_ListInit(&(pEnd->RxIrpList));
        }
#ifdef MUSB_OVERHEAD
        /* initialize statistics collection */
        pOverheads = MUSB_MemAlloc(pWrapper->PortImpl.bEndCount * sizeof(MGC_OverheadStats));
        MUSB_ArrayInit(&(pWrapper->PortImpl.OverheadStats), sizeof(MGC_OverheadStats),
                       pWrapper->PortImpl.bEndCount, pOverheads);
        for(bEnd = 0; bEnd < pWrapper->PortImpl.bEndCount; bEnd++)
        {
            Stats.dwOverheadMax = 0L;
            Stats.dwOverheadMin = 0xffffffffL;
            MUSB_ArrayAppend(&(pWrapper->PortImpl.OverheadStats), &Stats);
        }
#endif
        /* ready */
        pResult = &(pWrapper.Controller);
    }

    return pResult;
}

/*
 * Set a controller's host-mode power delivery capability
 */
uint32_t MUSB_SetControllerHostPower(MUSB_Controller *pController,
                                     uint16_t wPower)
{
    uint32_t result = MUSB_STATUS_INVALID_ARGUMENT;
    MGC_Controller *pImpl = NULL;

    if(pController)
    {
        pImpl = (MGC_Controller *)pController->pPrivateData;
    }
    if(pImpl)
    {
        if (pImpl->pfSetControllerHostPower)
        {
            result = pImpl->pfSetControllerHostPower(pImpl, wPower);
        }
    }
    return result;
}

/*
 * Start (or restart) a controller
 */
uint32_t MUSB_StartController(MUSB_Controller *pController,
                              MUSB_SystemServices *pSystemServices
                             )
{
    MGC_Controller *pImpl = NULL;

    if(pController)
    {
        pImpl = (MGC_Controller *)pController->pPrivateData;
    }
    if(pImpl)
    {
#ifdef MUSB_DMA
        /* try to instantiate DMA controller */
        if(pImpl->pDmaControllerFactory)
        {
            pImpl->pPort->pDmaController = pImpl->pDmaControllerFactory->pfNewDmaController(
                                               pImpl->pPort->pfDmaChannelStatusChanged, pImpl->pPort,
                                               pSystemServices->pfSystemToBusAddress, pSystemServices->pPrivateData,
                                               pImpl->pControllerAddressIsr, pImpl->pControllerAddressIst);
        }
        /* if successful, start it */
        if(pImpl->pPort->pDmaController)
        {
            pImpl->pPort->pDmaController->pfDmaStartController(
                pImpl->pPort->pDmaController->pPrivateData);
        }
#endif
        pImpl->pSystemServices = pSystemServices;
        return pImpl->pfProgramStartController(pImpl);
    }
    return MUSB_STATUS_UNSUPPORTED;
}

/* Discover the number of ports */
uint16_t MUSB_CountPorts()
{
    return MGC_iController;
}

/* Get a port */
MUSB_Port *MUSB_GetPort(uint16_t index)
{
    MUSB_Port *pResult = NULL;
    MGC_ControllerWrapper *pWrapper;

    if(index < MGC_iController)
    {
        /* typically called at init time only, so linear search OK */
        pWrapper = (MGC_ControllerWrapper *)MUSB_ListFindItem(&MGC_ControllerList, index);
        if(pWrapper)
        {
            pResult = &(pWrapper->Port);
        }
    }
    return pResult;
}

/*
 * Implementation
 */
uint8_t MUSB_DeactivateClient(MUSB_BusHandle hBus)
{
    MGC_Port *pPort = (MGC_Port *)hBus;

    /* clear for next session */
    pPort->bSessionActive = FALSE;

    /* try */
    pPort->bWantSession = FALSE;
    pPort->pfProgramBusState(pPort);

    return TRUE;
}

/*
 * Implementation
 */
uint32_t MUSB_GetBusFrame(MUSB_BusHandle hBus)
{
    MGC_Port *pPort = (MGC_Port *)hBus;

    pPort->pfReadBusState(pPort);
    return pPort->dwFrame;
}

/*
 * Implementation
 */
void MUSB_SuspendBus(MUSB_BusHandle hBus)
{
    uint16_t wCount;
    uint16_t wIndex;
    MUSB_Device *pDevice;
    MGC_Device *pImplDevice;
    MUSB_DeviceDriver *pDriver;
    MGC_Port *pPort = (MGC_Port *)hBus;

    if(pPort)
    {
        /* try */
#ifdef MUSB_OTG
        pPort->bRelinquish = FALSE;
#endif
        pPort->bWantSuspend = TRUE;
        pPort->pfProgramBusState(pPort);
#if defined(MUSB_HOST) || defined(MUSB_OTG)
        /* inform drivers */
        wCount = MUSB_ListLength(&(pPort->ConnectedDeviceList));
        for(wIndex = 0; wIndex < wCount; wIndex++)
        {
            pDevice = (MUSB_Device *)MUSB_ListFindItem(&(pPort->ConnectedDeviceList), wIndex);
            if(pDevice)
            {
                pImplDevice = pDevice->pPrivateData;
                if(pImplDevice)
                {
                    pDriver = pImplDevice->pDriver;
                    if(pDriver->pfBusSuspended)
                    {
                        pDriver->pfBusSuspended(pDriver->pPrivateData, hBus);
                    }
                }
            }
        }
#endif
    }
}

/*
 * Implementation
 */
void MUSB_ResumeBus(MUSB_BusHandle hBus)
{
    MGC_Port *pPort = (MGC_Port *)hBus;

    if(pPort)
    {
        if(pPort->bIsHost || pPort->bCanWakeup)
        {
            /* try */
            pPort->bWantSuspend = FALSE;
            pPort->bWantResume = TRUE;
            pPort->pfProgramBusState(pPort);

#if defined(MUSB_HOST) || defined(MUSB_OTG)
            switch(pPort->pController->wControllerType)
            {
            case MUSB_CONTROLLER_FDRC:
            case MUSB_CONTROLLER_HDRC:
            case MUSB_CONTROLLER_MHDRC:
                MGC_DrcFinishResume(pPort);
                break;
            }
#endif
        }
    }
}

/*
 * If DMA controller present, get a channel (if not already) and
 * try to allocate a buffer.  If allocation failed and we grabbed
 * a channel to try it, release the channel.
 */
uint8_t *MUSB_AllocDmaBuffer(MUSB_PipePtr hPipe, uint32_t dwLength)
{
#ifdef MUSB_DMA
    uint8_t bHadChannel = TRUE;
    uint8_t *pResult = NULL;
    MGC_Pipe *pPipe = (MGC_Pipe *)hPipe;
    MGC_EndpointResource *pEnd = pPipe->pLocalEnd;
    MGC_Port *pPort = pPipe->pPort;
    MUSB_DmaController *pDmaController = pPort->pDmaController;
    uint8_t bIsTx = (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ? TRUE : FALSE;

    if(pDmaController)
    {
        if(bIsTx)
        {
            if(!pEnd->pDmaChannel)
            {
                bHadChannel = FALSE;
                pEnd->pDmaChannel = pDmaController->pfDmaAllocateChannel(
                                        pDmaController->pPrivateData, pEnd->bLocalEnd,
                                        bIsTx, pPipe->bTrafficType,
                                        bIsTx ? pEnd->wMaxPacketSizeTx : pEnd->wMaxPacketSizeRx);
            }
            if(pEnd->pDmaChannel)
            {
                pResult = pDmaController->pfDmaAllocateBuffer(
                              pEnd->pDmaChannel, dwLength);
                if(pResult)
                {
                    /* success: increment count */
                    pEnd->wDmaBufferCount++;
                }
                else if(!pResult && !bHadChannel)
                {
                    /* failure: release channel if we allocated it */
                    pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
                    pEnd->pDmaChannel = NULL;
                }
            }
        }
        else
        {
            if(!pEnd->pRxDmaChannel)
            {
                bHadChannel = FALSE;
                pEnd->pRxDmaChannel = pDmaController->pfDmaAllocateChannel(
                                          pDmaController->pPrivateData, pEnd->bLocalEnd,
                                          bIsTx, pPipe->bTrafficType,
                                          bIsTx ? pEnd->wMaxPacketSizeTx : pEnd->wMaxPacketSizeRx);
            }
            if(pEnd->pRxDmaChannel)
            {
                pResult = pDmaController->pfDmaAllocateBuffer(
                              pEnd->pRxDmaChannel, dwLength);
                if(pResult)
                {
                    /* success: increment count */
                    pEnd->wRxDmaBufferCount++;
                }
                else if(!pResult && !bHadChannel)
                {
                    /* failure: release channel if we allocated it */
                    pDmaController->pfDmaReleaseChannel(pEnd->pRxDmaChannel);
                    pEnd->pRxDmaChannel = NULL;
                }
            }
        }
    }
    return pResult;
#else
    return NULL;
#endif
}

void MUSB_FreeDmaBuffer(MUSB_PipePtr hPipe, uint8_t *pBuffer)
{
#ifdef MUSB_DMA
    MGC_Pipe *pPipe = (MGC_Pipe *)hPipe;
    MGC_EndpointResource *pEnd = pPipe->pLocalEnd;
    MGC_Port *pPort = pPipe->pPort;
    MUSB_DmaController *pDmaController = pPort->pDmaController;
    uint8_t bIsTx = (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ? TRUE : FALSE;

    if(pDmaController)
    {
        if(bIsTx && pEnd->pDmaChannel)
        {
            pDmaController->pfDmaReleaseBuffer(
                pEnd->pDmaChannel, pBuffer);
            if(pEnd->wDmaBufferCount)
            {
                --pEnd->wDmaBufferCount;
                /* if buffer count goes to zero, release channel for others */
                if(!pEnd->wDmaBufferCount)
                {
                    pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
                    pEnd->pDmaChannel = NULL;
                }
            }
        }
        if(!bIsTx && pEnd->pRxDmaChannel)
        {
            pDmaController->pfDmaReleaseBuffer(
                pEnd->pRxDmaChannel, pBuffer);
            if(pEnd->wDmaBufferCount)
            {
                --pEnd->wDmaBufferCount;
                /* if buffer count goes to zero, release channel for others */
                if(!pEnd->wDmaBufferCount)
                {
                    pDmaController->pfDmaReleaseChannel(pEnd->pRxDmaChannel);
                    pEnd->pRxDmaChannel = NULL;
                }
            }
        }
    }
#endif
}

static const uint16_t MGC_awDynamicSize[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 };

MGC_EndpointResource *MGC_AllocateDynamicFifo(MGC_Port *pPort,
        const MUSB_DeviceEndpoint *pUsbEnd,
        MUSB_EndpointResource *pRequest,
        uint8_t bBind,
        uint8_t *pbIsTx,
        uint8_t bEndCount,
        uint32_t dwTotalSize,
        uint32_t dwHighWater)
{
    uint8_t bEnd;
    uint16_t wSize;
    uint8_t bVal;
    uint8_t bIsClaimed;
    MGC_EndpointResource *pEnd;
    uint32_t dwAvailSize;
    int32_t dwDistance;
    uint32_t dwNearest;
    uint32_t dwBase, dwBaseCandidate;
    uint8_t bBuf, bBufCount, bBufCandidate;
    uint32_t adwBase[32];
    uint16_t awSize[32];
    uint8_t abDouble[32];
    uint8_t abEnd[32];
    uint8_t abTx[32];
    uint8_t bTxHalted;
    uint8_t bNearestBuf = 0;
    uint8_t bStealDoubles = FALSE;
    uint8_t bDouble = 0;
    uint32_t dwDoubleSize = 0L;
    uint32_t dwUsedSize = 0L;
    uint8_t bTrafficType = pUsbEnd->UsbDescriptor.bmAttributes & MUSB_ENDPOINT_XFERTYPE_MASK;
    uint16_t wPacketSize = MUSB_SWAP16P((uint8_t *) & (pUsbEnd->UsbDescriptor.wMaxPacketSize));
    uint8_t bIsIn = (pUsbEnd->UsbDescriptor.bEndpointAddress & MUSB_ENDPOINT_DIR_MASK) ? TRUE : FALSE;
    uint8_t bIsTx = pPort->bIsHost ? !bIsIn : bIsIn;

    if(pPort->bIsHost)
    {
        /* host mode: find a space, compacting if necessary */
        /* collect all buffer info for analysis, and accumulate used size and double-buffering size */
        MUSB_MemSet(adwBase, 0, sizeof(adwBase));
        MUSB_MemSet(awSize, 0, sizeof(awSize));
        MUSB_MemSet(abDouble, 0, sizeof(abDouble));
        MUSB_MemSet(abEnd, 0, sizeof(abEnd));
        MUSB_MemSet(abTx, 0, sizeof(abTx));
        pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds), 0);
        adwBase[0] = 0;
        awSize[0] = pEnd->wMaxPacketSizeTx;
        dwUsedSize = awSize[0];
        for(bBufCount = bEnd = 1; bEnd < pPort->bEndCount; bEnd++)
        {
            pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds), bEnd);
            pPort->pfDynamicFifoLocation(pPort, bEnd, TRUE, FALSE,
                                         &(adwBase[bBufCount]), &bVal, &(abDouble[bBufCount]));
            if(adwBase[bBufCount])
            {
                awSize[bBufCount] = MGC_awDynamicSize[bVal];
                abEnd[bBufCount] = bEnd;
                abTx[bBufCount] = TRUE;
                dwUsedSize += pEnd->wMaxPacketSizeTx;
                if(abDouble[bBufCount])
                {
                    dwDoubleSize += awSize[bBufCount];
                }
                MUSB_DIAG2(2, "Dynamic FIFO Tx buffer for end=", bEnd,
                           " / Double-buffered=", abDouble[bBufCount], 10, 0);
                MUSB_DIAG2(2, "\tBase=", adwBase[bBufCount], " / Size=",
                           awSize[bBufCount], 16, 8);
                bBufCount++;
            }
            if(!pEnd->bIsFifoShared)
            {
                pPort->pfDynamicFifoLocation(pPort, bEnd, FALSE, FALSE,
                                             &(adwBase[bBufCount]), &bVal, &(abDouble[bBufCount]));
                if(adwBase[bBufCount])
                {
                    awSize[bBufCount] = MGC_awDynamicSize[bVal];
                    abEnd[bBufCount] = bEnd;
                    abTx[bBufCount] = FALSE;
                    dwUsedSize += pEnd->wMaxPacketSizeRx;
                    if(abDouble[bBufCount])
                    {
                        dwDoubleSize += awSize[bBufCount];
                    }
                    MUSB_DIAG2(2, "Dynamic FIFO Rx buffer for end=", bEnd,
                               " / Double-buffered=", abDouble[bBufCount], 10, 0);
                    MUSB_DIAG2(2, "\tBase=", adwBase[bBufCount], " / Size=",
                               awSize[bBufCount], 16, 8);
                    bBufCount++;
                }
            }
        }

        /* fail now if current used size + what could be stolen is insufficient */
        if((dwTotalSize - dwUsedSize + dwDoubleSize) < wPacketSize)
        {
            return NULL;
        }

        /* steal all double buffers if current used size is insufficient */
        if((dwTotalSize - dwUsedSize) < wPacketSize)
        {
            bStealDoubles = TRUE;
        }

        /* compact now, because the work is required in any case */
        for(bBufCandidate = 0; bBufCandidate < bBufCount; bBufCandidate++)
        {
            dwBaseCandidate = adwBase[bBufCandidate] + awSize[bBufCandidate];
            if(abDouble[bBufCandidate])
            {
                dwBaseCandidate += awSize[bBufCandidate];
            }
            dwNearest = dwTotalSize;
            for(bBuf = bBufCandidate + 1; bBuf < bBufCount; bBuf++)
            {
                dwDistance = adwBase[bBuf] - dwBaseCandidate;
                if(0 == dwDistance)
                {
                    /* compaction candidate is already where it should be */
                    dwNearest = dwTotalSize;
                    break;
                }
                if((dwDistance > 0) && (dwDistance < (int32_t)dwNearest))
                {
                    dwNearest = dwDistance;
                    bNearestBuf = bBuf;
                }
            }
            if(dwNearest < dwTotalSize)
            {
                /* move it down */
                bEnd = abEnd[bNearestBuf];
                pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds), bEnd);
                /* stop traffic */
                MUSB_DIAG2(2, "Compacting FIFOs: stopping traffic on end=",
                           bEnd, " / Tx=", abTx[bNearestBuf], 10, 0);
                if(abTx[bNearestBuf])
                {
                    pPort->pfProgramHaltEndpoint(pPort, pEnd, 0, TRUE);
                    /* no choice but to poll */
                    bTxHalted = FALSE;
                    while(!bTxHalted)
                    {
                        bTxHalted = *(volatile uint8_t *) & (pEnd->bIsHalted);
                    }
                }
                else
                {
                    pPort->pfProgramHaltEndpoint(pPort, pEnd, MUSB_DIR_IN, TRUE);
                }
                /* set new location */
                pPort->pfDynamicFifoLocation(pPort, bEnd, abTx[bNearestBuf],
                                             FALSE, &dwBase, &bVal, &bDouble);
                if(bStealDoubles)
                {
                    bDouble = FALSE;
                }
                MUSB_DIAG2(2, "New base=", dwBaseCandidate,
                           " / Double-buffered=", bDouble, 16, 8);
                pPort->pfDynamicFifoLocation(pPort, bEnd, abTx[bNearestBuf],
                                             TRUE, &dwBaseCandidate, &bVal, &bDouble);
                /* update array entry */
                adwBase[bNearestBuf] = dwBaseCandidate;
                abDouble[bNearestBuf] = bDouble;
                MUSB_DIAG_STRING(2, "Restarting traffic");
                /* restart traffic */
                pPort->pfProgramHaltEndpoint(pPort, pEnd,
                                             abTx[bNearestBuf] ? 0 : MUSB_DIR_IN, FALSE);
            }
        }

        /* now find the top and go for it */
        dwBaseCandidate = 0;
        for(bBuf = 0; bBuf < bBufCount; bBuf++)
        {
            dwBase = adwBase[bBuf] + awSize[bBuf];
            if(abDouble[bBuf])
            {
                dwBase += awSize[bBuf];
            }
            if(dwBase > dwBaseCandidate)
            {
                dwBaseCandidate = dwBase;
            }
        }

        for(bEnd = 0; bEnd < pPort->bEndCount; bEnd++)
        {
            pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds), bEnd);
            bIsClaimed = pEnd->bIsFifoShared ?
                         pEnd->bIsClaimed : (bIsTx ? pEnd->bIsClaimed : pEnd->bRxClaimed);
            if(!bIsClaimed)
            {
                dwBase = dwUsedSize;
                break;
            }
        }
    }
    else
    {
        /* device mode; accumulate used size (NOTE: assumes endpoint descriptors are in order) */
        for(dwUsedSize = bEnd = 0; bEnd < pPort->bEndCount; bEnd++)
        {
            pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds), bEnd);
            bIsClaimed = pEnd->bIsFifoShared ?
                         pEnd->bIsClaimed : (bIsTx ? pEnd->bIsClaimed : pEnd->bRxClaimed);
            dwUsedSize += pEnd->wMaxPacketSizeTx +
                          (pEnd->bIsFifoShared ? 0 : pEnd->wMaxPacketSizeRx);
        }
        /* use endpoint from descriptor */
        bEnd = pUsbEnd->UsbDescriptor.bEndpointAddress & MUSB_ENDPOINT_NUMBER_MASK;
        pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds), bEnd);
        if(!bBind)
        {
            /* no bind; just check */
            dwAvailSize = dwTotalSize - dwUsedSize;
            wPacketSize = MUSB_SWAP16P((uint8_t *) & (pUsbEnd->UsbDescriptor.wMaxPacketSize));
            if(dwAvailSize >= wPacketSize)
            {
                return pEnd;
            }
            return NULL;
        }
        dwBase = dwUsedSize;
    }

    /* if we found an endpoint, allocate space and program */
    if(bEnd < bEndCount)
    {
        dwAvailSize = dwTotalSize - dwUsedSize;
        wSize = wPacketSize;
        bVal = 0;
        while(wSize > 1)
        {
            bVal++;
            wSize >>= 1;
        }
        if(bVal >= 3)
        {
            bVal -= 3;
        }
        else
        {
            bVal = 0;
        }
        wSize = MGC_awDynamicSize[bVal];
        switch(bTrafficType)
        {
        case MUSB_ENDPOINT_XFER_BULK:
        case MUSB_ENDPOINT_XFER_ISOC:
            /* if we are below high-water mark, try double-buffered for bulk/isoch */
#ifndef MUSB_DISABLE_DB_DYNFIFO
            /* NOTE: does not work with MHDRC 1.1; works fine with 1.3 */
            if(dwUsedSize < dwHighWater)
            {
                bDouble = 1;
                wSize <<= 1;
            }
#endif
            break;
        }
        if(bDouble && ((wSize > dwAvailSize) ||
                       ((dwAvailSize - wSize) < ((bEndCount - bEnd) * 8))))
        {
            /* back off double-buffering */
            bDouble = 0;
            wSize >>= 1;
        }
        if((wSize < dwAvailSize) &&
                ((dwAvailSize - wSize) >= ((bEndCount - bEnd) * 8)))
        {
            /* we're good to go */
            MUSB_DIAG2(2, "Programming dynamic FIFO end=", bEnd,
                       " / Tx=", bIsTx, 10, 0);
            MUSB_DIAG2(2, "\tBase=", dwBase, " / Effective Size=", wSize, 16, 8);
            pPort->pfDynamicFifoLocation(pPort, bEnd, bIsTx, TRUE,
                                         &dwBase, &bVal, &bDouble);
            if(bIsTx || pEnd->bIsFifoShared)
            {
                pEnd->bIsClaimed = TRUE;
            }
            else
            {
                pEnd->bRxClaimed = TRUE;
            }
            pEnd->bIsTx = bIsTx;
            pEnd->bIsHalted = FALSE;
            if(bIsTx)
            {
                pEnd->bBusAddress = pUsbEnd->pDevice->bBusAddress;
                pEnd->bBusEnd = pUsbEnd->UsbDescriptor.bEndpointAddress;
                pEnd->bInterval = pUsbEnd->UsbDescriptor.bInterval;
                pEnd->bTrafficType = pUsbEnd->UsbDescriptor.bmAttributes;
                pEnd->wPacketSize = wPacketSize;
                pEnd->wMaxPacketSizeTx = wSize;
            }
            else
            {
                pEnd->bRxBusAddress = pUsbEnd->pDevice->bBusAddress;
                pEnd->bRxBusEnd = pUsbEnd->UsbDescriptor.bEndpointAddress;
                pEnd->bRxInterval = pUsbEnd->UsbDescriptor.bInterval;
                pEnd->bRxTrafficType = pUsbEnd->UsbDescriptor.bmAttributes;
                pEnd->wRxPacketSize = wPacketSize;
                pEnd->wMaxPacketSizeRx = wSize;
            }
            *pbIsTx = bIsTx;
            return pEnd;
        }
    }
    return NULL;
}
#endif /*CFG_USB*/
// eof
