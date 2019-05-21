/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2006              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * MUSBMicroSW HDRC implementation.
 * $Revision: 1.89 $
 */
#include "include.h"
#include "mu_impl.h"
#include "mu_mem.h"

#if CFG_USB
#if defined(MUSB_HDRC) || defined(MUSB_MHDRC)
/**
 * Undefine this to avoid embedded HSET overheads
 * (but this would preclude USB-IF high-speed OTG certification)
 */
#define MUSB_EHSET

/*
* Implementations of HDRC-specific functions
*/

#include "hdrc_cnf.h"

#include "mu_diag.h"

#include "mu_hdrpr.h"
#include "mu_hdrdf.h"
#include "mu_pippr.h"
#include "mu_funpr.h"
#include "mu_hstpr.h"

static const uint8_t MGC_aTestPacket[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
    0xee, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0xdf,
    0xef, 0xf7, 0xfb, 0xfd, 0xfc, 0x7e, 0xbf, 0xdf,
    0xef, 0xf7, 0xfb, 0xfd, 0x7e
};

/*
 * Discover HDRC FIFO configuration
 */
uint8_t MGC_HdrcInit(MGC_Port *pPort)
{
#ifdef MUSB_C_DYNFIFO_DEF
    MGC_EndpointResource end;
    uint8_t bEnd;
#endif
    uint8_t config;
    MGC_Controller *pImplController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pImplController->pControllerAddressIst;

    /* read config data */
    MGC_SelectEnd(pBase, 0);
    config = MGC_ReadCsr8(pBase, MGC_O_HDRC_CONFIGDATA, 0);
    if(config & MGC_M_CONFIGDATA_MPRXE)
    {
        pPort->bHasBulkCombine = TRUE;
    }
    if(config & MGC_M_CONFIGDATA_MPTXE)
    {
        pPort->bHasBulkSplit = TRUE;
    }
    if(config & MGC_M_CONFIGDATA_HBRXE)
    {
        pPort->bHasHbIsoRx = TRUE;
    }
    if(config & MGC_M_CONFIGDATA_HBTXE)
    {
        pPort->bHasHbIsoTx = TRUE;
    }

#ifdef MUSB_C_DYNFIFO_DEF
    if(!(config & MGC_M_CONFIGDATA_DYNFIFO))
    {
        MGC_DIAG(1, pImplController, "Dynamic FIFO support built but not present in hardware; please rebuild");
        return FALSE;
    }
    pPort->bEndCount = MUSB_C_NUM_EPS;
    pPort->wEndMask =  1;
    pPort->pPrivateData = MUSB_MemAlloc(MUSB_C_NUM_EPS *
                                        sizeof(MGC_EndpointResource));
    if(pPort->pPrivateData)
    {
        if(MUSB_ArrayInit(&(pPort->LocalEnds), sizeof(MGC_EndpointResource),
                          MUSB_C_NUM_EPS, pPort->pPrivateData))
        {
            /* add endpoint 0 */
            MUSB_MemSet((void *)&end, 0, sizeof(end));
            end.bIsFifoShared = TRUE;
            end.bTrafficType = MUSB_ENDPOINT_XFER_CONTROL;
            end.wMaxPacketSizeTx = MGC_END0_FIFOSIZE;
            end.wMaxPacketSizeRx = MGC_END0_FIFOSIZE;
            end.bIsClaimed = TRUE;
            MUSB_ArrayAppend(&(pPort->LocalEnds), &end);
            MGC_SelectEnd(pBase, 0);
            MGC_Write8(pBase, MGC_O_HDRC_RXFIFOSZ, 3);
            MGC_Write8(pBase, MGC_O_HDRC_TXFIFOSZ, 3);
            MGC_Write16(pBase, MGC_O_HDRC_RXFIFOADD, 0);
            MGC_Write16(pBase, MGC_O_HDRC_TXFIFOADD, 0);

            /* add others, but don't set sizes */
            for(bEnd = 1; bEnd < (uint8_t)(MUSB_C_NUM_EPS & 0xff); bEnd++)
            {
                MUSB_MemSet((void *)&end, 0, sizeof(end));
                end.bLocalEnd = bEnd;
                MUSB_ArrayAppend(&(pPort->LocalEnds), &end);
                pPort->wEndMask |= (1 << bEnd);
            }

            return TRUE;
        }
        else
        {
            MUSB_MemFree(pPort->pPrivateData);
        }
    }
    return FALSE;
#else
    if(config & MGC_M_CONFIGDATA_DYNFIFO)
    {
        MGC_DIAG(1, pImplController, "Dynamic FIFOs detected but support not built; please rebuild");
        return FALSE;
    }
    return MGC_DrcInit(pPort);
#endif
}

void MGC_HdrcDynamicFifoLocation(MGC_Port *pPort,
                                 uint8_t bEnd,
                                 uint8_t bIsTx,
                                 uint8_t bSet,
                                 uint32_t *pdwBase,
                                 uint8_t *pbSize83,
                                 uint8_t *pbDoubleBuffered)
{
#ifdef MUSB_C_DYNFIFO_DEF
    uint8_t bVal;
    MGC_Controller *pImplController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pImplController->pControllerAddressIst;
    MGC_EndpointResource *pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds), bEnd);

    MGC_SelectEnd(pBase, bEnd);
    if(bSet)
    {
        /* set new location/size */
        bVal = *pbSize83;
        if(*pbDoubleBuffered)
        {
            bVal |= MGC_M_FIFOSZ_DPB;
        }
        if(bIsTx)
        {
            pEnd->bTxFifoSize = bVal;
            MGC_Write8(pBase, MGC_O_HDRC_TXFIFOSZ, bVal);
            MGC_Write16(pBase, MGC_O_HDRC_TXFIFOADD,
                        (uint16_t)((*pdwBase >> 3) & 0xffff));
        }
        else
        {
            pEnd->bRxFifoSize = bVal;
            MGC_Write8(pBase, MGC_O_HDRC_RXFIFOSZ, bVal);
            MGC_Write16(pBase, MGC_O_HDRC_RXFIFOADD,
                        (uint16_t)((*pdwBase >> 3) & 0xffff));
        }
    }
    else
    {
        /* get current location/size */
        if(bIsTx)
        {
            bVal = pEnd->bTxFifoSize;
            *pdwBase = MGC_Read16(pBase, MGC_O_HDRC_TXFIFOADD) << 3;
        }
        else
        {
            bVal = pEnd->bRxFifoSize;
            *pdwBase = MGC_Read16(pBase, MGC_O_HDRC_RXFIFOADD) << 3;
        }
        *pbSize83 = bVal & MGC_M_FIFOSZ_SIZE;
        *pbDoubleBuffered = (bVal & MGC_M_FIFOSZ_DPB) ? TRUE : FALSE;
    }
#endif	/* dynamic FIFOs */
}

#ifdef MUSB_C_DYNFIFO_DEF
#define MGC_DFIFO_TOTAL (1 << (MUSB_C_RAM_BITS + 2))
#define MGC_DFIFO_HIGH (MGC_DFIFO_TOTAL >> 1)
MGC_EndpointResource *MGC_HdrcBindDynamicEndpoint(MGC_Port *pPort,
        const MUSB_DeviceEndpoint *pUsbEnd,
        MUSB_EndpointResource *pRequest,
        uint8_t bBind, uint8_t *pbIsTx)
{
    return MGC_AllocateDynamicFifo(pPort, pUsbEnd, pRequest, bBind, pbIsTx,
                                   MUSB_C_NUM_EPS, MGC_DFIFO_TOTAL, MGC_DFIFO_HIGH);
}
#endif	/* dynamic FIFOs */

int MGC_HdrcIsr(void *pParam)
{
    int result;
    uint8_t bIntrUsbValue;
    uint16_t wIntrTxValue, wIntrRxValue;
    MUSB_Controller *pController = (MUSB_Controller *)pParam;
    MGC_Controller *pControllerImpl = (MGC_Controller *)pController->pPrivateData;
#ifdef MUSB_OVERHEAD
    uint32_t dwTime = 0;
    MUSB_SystemUtils *pUtils = pControllerImpl->pUtils;
    uint32_t dwStartTime = pUtils->pfGetTime();
#endif
    uint8_t *pBase = (uint8_t *)pControllerImpl->pControllerAddressIst;
    uint8_t bIndex = MGC_Read8(pBase, MGC_O_HDRC_INDEX);

    bIntrUsbValue = MGC_Read8(pBase, MGC_O_HDRC_INTRUSB);
    wIntrTxValue = MGC_Read16(pBase, MGC_O_HDRC_INTRTX);
    wIntrRxValue = MGC_Read16(pBase, MGC_O_HDRC_INTRRX);
    result = MGC_DrcIsr(pControllerImpl, bIntrUsbValue, wIntrTxValue, wIntrRxValue);

#ifdef MUSB_OVERHEAD
    dwTime = pUtils->pfGetTime() - dwStartTime;
    if(dwTime > pPort->IsrOverhead.dwOverheadMax)
    {
        pPort->IsrOverhead.dwOverheadMax = dwTime;
    }
    if(dwTime < pPort->IsrOverhead.dwOverheadMin)
    {
        pPort->IsrOverhead.dwOverheadMin = dwTime;
    }
#endif

    /* restore index */
    MGC_Write8(pBase, MGC_O_HDRC_INDEX, bIndex);

    return result;
}

/*
* Program the HDRC to start (enable interrupts, etc.).
*/
uint32_t MGC_HdrcStart(MGC_Controller *pController)
{
    uint16_t val;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIsr;
    MGC_Port *pPort = pController->pPort;

    /*  Set INT enable registers */
    val = 0xffff & pPort->wEndMask;
    MGC_DIAG1(3, pController, "Setting IntrTxE to ", val, 16, 4);
    MGC_Write16(pBase, MGC_O_HDRC_INTRTXE, val);
    val = 0xfffe & pPort->wEndMask;
    MGC_DIAG1(3, pController, "Setting IntrRxE to ", val, 16, 4);
    MGC_Write16(pBase, MGC_O_HDRC_INTRRXE, val);
#ifdef MUSB_SCHEDULER
    /* TODO: enable SOF interrupts only when needed (maybe in ProgramBusState when the schedule is non-empty) */
    val = 0xff;
#else
    val = (uint16_t)~MGC_M_INTR_SOF;
#endif
    MGC_DIAG1(3, pController, "Setting IntrUsbE to ", (val & 0xff), 16, 2);
    MGC_Write8(pBase, MGC_O_HDRC_INTRUSBE, (val & 0xff));

#ifdef MUSB_FORCE_FULLSPEED
    MGC_Write8(pBase, MGC_O_HDRC_FSEOF1, (0xff & 0xff));
#endif

    /* TODO: always set ISOUPDATE in POWER (periph mode) and leave it on! */
    pPort->bOtgState = MUSB_AB_IDLE;
    return 0;
}

/*
* Program the HDRC to stop (disable interrupts, etc.).
*/
uint32_t MGC_HdrcStop(MGC_Controller *pController)
{
    uint16_t temp;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;

    /* disable interrupts */
    MGC_Write8(pBase, MGC_O_HDRC_INTRUSBE, 0);
    MGC_Write16(pBase, MGC_O_HDRC_INTRTXE, 0);
    MGC_Write16(pBase, MGC_O_HDRC_INTRRXE, 0);

    /*  flush pending interrupts */
    temp = MGC_Read8(pBase, MGC_O_HDRC_INTRUSB);
    temp = MGC_Read16(pBase, MGC_O_HDRC_INTRTX);
    temp = MGC_Read16(pBase, MGC_O_HDRC_INTRRX);

    temp = 0;
    return temp;
}

uint32_t MGC_HdrcDestroy(MGC_Controller *pController)
{
    MGC_Port *pPort = pController->pPort;

    MGC_HostDestroy(pPort);
    if(pPort->pPrivateData)
    {
        MUSB_MemFree(pPort->pPrivateData);
    }
    return MGC_FunctionDestroy(pPort);
}

/* Fill the bus state info from the controller */
uint32_t MGC_HdrcReadBusState(MGC_Port *pPort)
{
    uint8_t devctl;
    uint8_t power;
    uint8_t vbus;
    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;

    /* bConnectorId, bIsSession, bIsHost */
    devctl = MGC_Read8(pBase, MGC_O_HDRC_DEVCTL);
    if(devctl & MGC_M_DEVCTL_SESSION)
    {
        pPort->bConnectorId = (devctl & MGC_M_DEVCTL_BDEVICE) >> 7;
    }
    else
    {
        pPort->bConnectorId = MGC_CID_UNKNOWN;
    }
    pPort->bIsSession = (devctl & MGC_M_DEVCTL_SESSION) ? TRUE : FALSE;
    pPort->bIsHost = (devctl & MGC_M_DEVCTL_HM) ? TRUE : FALSE;
    pPort->bIsLowSpeed = (devctl & MGC_M_DEVCTL_LSDEV) ? TRUE : FALSE;

    /* bIsHighSpeed, bIsSuspend, bIsReset, bIsResume, bIsSession, bBusVoltage */
    power = MGC_Read8(pBase, MGC_O_HDRC_POWER);
    pPort->bIsHighSpeed = (power & MGC_M_POWER_HSMODE) ? TRUE : FALSE;
    pPort->bIsSuspend = (power & MGC_M_POWER_SUSPENDM) ? TRUE : FALSE;
    pPort->bIsReset = (power & MGC_M_POWER_RESET) ? TRUE : FALSE;
    pPort->bIsResume = (power & MGC_M_POWER_RESUME) ? TRUE : FALSE;
    vbus = (devctl & MGC_VBUS_MASK) >> 3;
    switch (vbus)
    {
    case 0:
        pPort->bBusVoltage = MGC_VBUS_BELOW_SESSION_END;
        break;
    case 1:
        pPort->bBusVoltage = MGC_VBUS_ABOVE_SESSION_END;
        break;
    case 2:
        pPort->bBusVoltage = MGC_VBUS_ABOVE_AVALID;
        break;
    case 3:
        pPort->bBusVoltage = MGC_VBUS_ABOVE_VBUS_VALID;
        break;
    default:
        pPort->bBusVoltage = MGC_VBUS_ERROR;
    }

    /* dwFrame */
    /* if some one is using this, then have protection for updating it */
    pPort->dwFrame = MGC_Read16(pBase, MGC_O_HDRC_FRAME);

    if(pPort->bIsReset)
    {
        MUSB_PRT("[MGC] HdrcReadBusState:%d\r\n", pPort->bIsReset);
    }

    if(FALSE == pPort->bIsHost)
    {
        MUSB_PRT("[MGC] HdrcReadBusState DEVICE\r\n");
    }

    return 0;
}

/* Program the bus state from the port (see MGC_Port) */
uint32_t MGC_HdrcProgramBusState(MGC_Port *pPort)
{
    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;
    uint8_t power = MGC_Read8(pBase, MGC_O_HDRC_POWER);
    uint8_t nPower = power;
    uint8_t devctl = MGC_Read8(pBase, MGC_O_HDRC_DEVCTL);
    uint8_t nDevCtl = devctl;
    uint8_t bFuncAddr = MGC_Read8(pBase, MGC_O_HDRC_FADDR);

    /* reset? */
    if(pPort->bWantReset)
    {
        nPower |= MGC_M_POWER_RESET;
    }
    else
    {
        nPower &= ~MGC_M_POWER_RESET;
    }

    /* resume? */
    if(pPort->bWantResume)
    {
        nPower |= MGC_M_POWER_RESUME;
    }
    else
    {
        nPower &= ~MGC_M_POWER_RESUME;
    }

    /* suspend? */
    if(pPort->bWantSuspend)
    {
        if(pPort->bIsHost)
        {
            nPower |= MGC_M_POWER_SUSPENDM;
        }
        else
        {
            nPower |= MGC_M_POWER_ENSUSPEND;
        }
    }
    else
    {
        nPower &= ~MGC_M_POWER_SUSPENDM;
        //nPower &= ~MGC_M_POWER_ENSUSPEND; // maybe it is a issue
    }

    /* high-speed? */
    if(pPort->bWantHighSpeed)
    {
        nPower |= MGC_M_POWER_HSENAB;
    }
    else
    {
        nPower &= ~MGC_M_POWER_HSENAB;
    }

    /* session? */
    if(pPort->bWantSession)
    {
        nDevCtl |= MGC_M_DEVCTL_SESSION;
    }
    else
    {
        nDevCtl &= ~MGC_M_DEVCTL_SESSION;
    }

    /* host? */
    if(pPort->bWantHost)
    {
        nDevCtl |= MGC_M_DEVCTL_HR;
    }
    else
    {
        nDevCtl &= ~MGC_M_DEVCTL_HR;
    }

    /* make it so, but only on changes */
    if(pPort->bFuncAddr != bFuncAddr)
    {
        MUSB_PRT("[MGC] O_HDRC_FADDR:0x%x\r\n", pPort->bFuncAddr);
        MGC_Write8(pBase, MGC_O_HDRC_FADDR, pPort->bFuncAddr);
    }
    if(nPower != power)
    {
        MUSB_PRT("[MGC] O_HDRC_POWER:0x%x\r\n", nPower);
        MGC_Write8(pBase, MGC_O_HDRC_POWER, nPower);
    }
    if(nDevCtl != devctl)
    {
        MUSB_PRT("[MGC] O_HDRC_DEVCTL:0x%x\r\n", nDevCtl);
        MGC_Write8(pBase, MGC_O_HDRC_DEVCTL, nDevCtl);
    }

    return 0;
}

/*
* Find the HDRC's first (host) or required (function)
* local endpoint resource to communicate with the given remote endpoint (host)
* or to be identified with the given endpoint descriptor (function).
*/
MGC_EndpointResource *MGC_HdrcBindEndpoint(MGC_Port *pPort,
        const MUSB_DeviceEndpoint *pUsbEnd,
        MUSB_EndpointResource *pRequest,
        uint8_t bBind)
{
    uint8_t bEnd, bIsTx, bTrafficType;
    uint8_t bInterval;
    MGC_EndpointResource *pEnd;
    uint8_t reg = 0;
    uint16_t csr = 0;
    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;

#ifdef MUSB_C_DYNFIFO_DEF
    pEnd = MGC_HdrcBindDynamicEndpoint(pPort, pUsbEnd, pRequest, bBind, &bIsTx);
#else
    pEnd = MGC_DrcBindEndpoint(pPort, pUsbEnd, pRequest, bBind, &bIsTx);
#endif
    if(pEnd && bBind)
    {
        /* prepare endpoint registers according to flags */
        bTrafficType = bIsTx ? pEnd->bTrafficType : pEnd->bRxTrafficType;
        bEnd = pEnd->bLocalEnd;
        MGC_SelectEnd(pBase, bEnd);

        /* proto reg */
        reg |= bIsTx ? (pEnd->bBusEnd & MUSB_ENDPOINT_NUMBER_MASK) :
               (pEnd->bRxBusEnd & MUSB_ENDPOINT_NUMBER_MASK);
        if(bEnd)
        {
            reg |= (bTrafficType << 4);
        }

        if(bIsTx)
        {
            /* transmit */
            if(pPort->bIsHost)
            {
                /* protocol/endpoint/interval/NAKlimit */
                if(bEnd)
                {
                    MGC_WriteCsr8(pBase, MGC_O_HDRC_TXTYPE, bEnd, reg);
                    /*
                     * TODO: at least for bulk, setting the upper 4 interval bits causes the core
                     * to continuously assert Rx interrupts with RxError set
                     */
                    bInterval = MUSB_MIN(pUsbEnd->UsbDescriptor.bInterval, 0x10);
                    if(MUSB_ENDPOINT_XFER_BULK == bTrafficType)
                    {
                        /* for bulk, use NAK limit and always use some limit */
                        bInterval = MUSB_MIN(pUsbEnd->wNakLimit, 0x10);
                        bInterval = MUSB_MAX(bInterval, 2);
                        if(0xffff == pUsbEnd->wNakLimit)
                        {
                            bInterval = 0;
                        }
                    }
                    MGC_WriteCsr8(pBase, MGC_O_HDRC_TXINTERVAL, bEnd, bInterval);
                }
                else
                {
                    MGC_WriteCsr8(pBase, MGC_O_HDRC_NAKLIMIT0, 0, 0x10);
                }
            }

            /* CSR */
            if(bEnd)
            {
                /* packet size */
                if((MUSB_ENDPOINT_XFER_BULK == pEnd->bTrafficType) && pPort->bHasBulkSplit)
                {
                    /* bulk splitting: set up for FIFO size */
                    MGC_WriteCsr16(pBase, MGC_O_HDRC_TXMAXP, bEnd,
                                   pEnd->wPacketSize |
                                   ((pEnd->wMaxPacketSizeTx / pEnd->wPacketSize) - 1) << 11);
                }
                else
                {
                    /* normal case: set up for packet size */
                    MGC_WriteCsr16(pBase, MGC_O_HDRC_TXMAXP, bEnd, pEnd->wPacketSize);
                }
                csr = MGC_M_TXCSR_MODE;
#ifdef MUSB_ISO
                if(!pPort->bIsHost && (pEnd->bTrafficType == MUSB_ENDPOINT_XFER_ISOC))
                {
                    csr |= MGC_M_TXCSR_ISO;
                }
#endif
                /* flush twice in case of double packet buffering */
                MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd,
                               MGC_M_TXCSR_FLUSHFIFO | MGC_M_TXCSR_CLRDATATOG);
                MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd,
                               MGC_M_TXCSR_FLUSHFIFO | MGC_M_TXCSR_CLRDATATOG);
                /* final CSR setup */
                MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, csr);
            }
            else
            {
                /* endpoint 0: just flush */
                MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0,
                               MGC_M_CSR0_FLUSHFIFO);
                MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0,
                               MGC_M_CSR0_FLUSHFIFO);
            }
        }
        else
        {
            /* receive */
            if(pPort->bIsHost)
            {
                /* protocol/endpoint/interval/NAKlimit */
                if(bEnd)
                {
                    /* first, ensure Rx mode */
                    MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, 0);
                    MGC_WriteCsr8(pBase, MGC_O_HDRC_RXTYPE, bEnd, reg);

                    /*
                     * TODO: at least for bulk, setting the upper 4 interval bits causes the core
                     * to continuously assert Rx interrupts with RxError set
                     */
                    bInterval = MUSB_MIN(pUsbEnd->UsbDescriptor.bInterval, 0x10);
                    if(MUSB_ENDPOINT_XFER_BULK == pEnd->bRxTrafficType)
                    {
                        /* for bulk, use NAK limit and always use some limit */
                        bInterval = MUSB_MIN(pUsbEnd->wNakLimit, 0x10);
                        bInterval = MUSB_MAX(bInterval, 2);
                        if(0xffff == pUsbEnd->wNakLimit)
                        {
                            bInterval = 0;
                        }
                    }
                    MGC_WriteCsr8(pBase, MGC_O_HDRC_RXINTERVAL, bEnd, bInterval);
                }
                else
                {
                    MGC_WriteCsr8(pBase, MGC_O_HDRC_NAKLIMIT0, 0, 0x10);
                    MGC_WriteCsr8(pBase, MGC_O_HDRC_TYPE0, 0, reg & 0xc0);
                }
            }

            /* CSR */
            if(bEnd)
            {
                /* packet size */
                MGC_WriteCsr16(pBase, MGC_O_HDRC_RXMAXP, bEnd, pEnd->wRxPacketSize);
                if(pPort->bIsHost)
                {
                    csr = 0;
                }
                else
                {
#ifdef MUSB_ISO
                    if(pEnd->bTrafficType == MUSB_ENDPOINT_XFER_ISOC)
                    {
                        csr = MGC_M_RXCSR_P_ISO;
                    }
#endif
                }
                /* twice in case of double packet buffering */
                MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd,
                               MGC_M_RXCSR_FLUSHFIFO | MGC_M_RXCSR_CLRDATATOG);
                MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd,
                               MGC_M_RXCSR_FLUSHFIFO | MGC_M_RXCSR_CLRDATATOG);
                if(MUSB_ENDPOINT_XFER_INT == pEnd->bRxTrafficType)
                {
                    csr |= MGC_M_RXCSR_DISNYET;
                }
                /* use AutoClear in peripheral mode */
                if(!pPort->bIsHost)
                {
                    csr |= MGC_M_RXCSR_AUTOCLEAR;
                }
                MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, csr);
            }
        }
    }
    return pEnd;
}

/*
* Program the HDRC to initiate reception.
*/
uint32_t MGC_HdrcStartRx(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                         uint8_t *pBuffer, uint32_t dwTotalBytes,
                         void *pIrp, uint8_t bAllowDma)
{
#ifdef MUSB_DMA
    MUSB_DmaController *pDmaController;
    MUSB_DmaChannel *pDmaChannel;
    uint8_t bDmaOk;
#endif
    uint16_t wCsr, wRxCount, wIntrRxE;
    MGC_BsrItem qItem;
    uint8_t bResult;
    uint8_t reg = 0;
    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;
    MUSB_SystemServices *pServices = pController->pSystemServices;
    uint8_t bEnd = pEnd->bLocalEnd;

    MGC_SelectEnd(pBase, bEnd);

    /* clear mode bit in TxCSR */
    wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd);
    if((wCsr & MGC_M_TXCSR_MODE) && pEnd->bIsFifoShared)
    {
        wCsr &= ~MGC_M_TXCSR_MODE;
        MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, wCsr);
    }

    /* read RxCSR for preparation */
    wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd);

    /* prepare for the non-DMA case */
    wCsr &= ~(MGC_M_RXCSR_DMAENAB | MGC_M_RXCSR_DMAMODE);
    if(pPort->bIsHost)
    {
        wCsr |= MGC_M_RXCSR_H_REQPKT;
        wCsr &= ~MGC_M_RXCSR_RXPKTRDY;

        /* write protocol/endpoint */
        reg = pEnd->bRxTrafficType << MGC_S_TYPE_PROTO;
        reg |= pEnd->bRxBusEnd & MGC_M_TYPE_REMOTE_END;
        MGC_WriteCsr8(pBase, MGC_O_HDRC_RXTYPE, bEnd, reg);
    }

    /* normal case: set up for packet size */
    MGC_WriteCsr16(pBase, MGC_O_HDRC_RXMAXP, bEnd, pEnd->wRxPacketSize);

    pEnd->bBsrResidual = pEnd->bIsrResidual;

    /* disable endpoint interrupt */
    wIntrRxE = MGC_Read16(pBase, MGC_O_HDRC_INTRRXE) | (1 << bEnd);
    MGC_Write16(pBase, MGC_O_HDRC_INTRRXE, wIntrRxE & ~(1 << bEnd));

    if(!pPort->bIsHost)
    {
        /* handle residual if any */
        if(MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd) & MGC_M_RXCSR_RXPKTRDY)
        {
            /* poll until IRP is complete, since handing it to the ISR will always have races */
            while(TRUE)
            {
                if(wCsr & MGC_M_RXCSR_RXPKTRDY)
                {
                    wRxCount = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCOUNT, bEnd);
                    MUSB_DIAG1(3, "StartRx: residual byte count=", wRxCount, 16, 0);
#ifdef MUSB_DMA
                    pDmaController = pPort->pDmaController;
                    pDmaChannel = pEnd->pRxDmaChannel;
                    if(!bAllowDma && pDmaChannel)
                    {
                        /* release previously-allocated channel */
                        pDmaController->pfDmaReleaseChannel(pDmaChannel);
                        pEnd->pRxDmaChannel = NULL;
                    }
                    else if(bAllowDma && pServices->pfSystemToBusAddress(pServices->pPrivateData, pBuffer))
                    {
                        /* candidate for DMA */
                        if(pDmaController && !pDmaChannel)
                        {
                            pDmaChannel = pEnd->pRxDmaChannel = pDmaController->pfDmaAllocateChannel(
                                                                    pDmaController->pPrivateData, bEnd, FALSE,
                                                                    pEnd->bRxTrafficType, pEnd->wRxPacketSize);
                        }
                        if(pDmaChannel)
                        {
                            pDmaChannel->dwActualLength = 0L;
                            pEnd->bDmaTx = FALSE;

                            /*
                            * Speculate successful DMA programming and clear these fields if not.
                            * This is because although third-party DMACs obey the interface,
                            * the Inventra DMAC in some cases will immediately activate, finish,
                            * and interrupt us before the RxCSR write below, and these fields
                            * are needed by its ISR.
                            */
                            pEnd->bDmaResidual = TRUE;
                            pEnd->pRxIrp = pIrp;

                            /* attempt to program the channel */
                            bDmaOk = pDmaController->pfDmaProgramChannel(pDmaChannel,
                                     pEnd->wRxPacketSize, 0, pBuffer,
                                     MGC_MIN(dwTotalBytes, pDmaChannel->dwMaxLength));
                            if(bDmaOk)
                            {
                                if(((volatile MGC_EndpointResource *)pEnd)->pRxIrp)
                                {
                                    /* external DMAC; need to assert request line */
                                    wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd);
                                    wCsr |= (MGC_M_RXCSR_AUTOCLEAR | MGC_M_RXCSR_DMAENAB);
                                    /* this special sequence is required to get DMAReq to activate */
                                    MGC_WriteCsr8(pBase, (MGC_O_HDRC_RXCSR + 1), bEnd,
                                                  (uint8_t)((wCsr | MGC_M_RXCSR_DMAMODE) >> 8));
                                    MGC_WriteCsr8(pBase, (MGC_O_HDRC_RXCSR + 1), bEnd,
                                                  (uint8_t)(wCsr >> 8));
                                }
                                MGC_DIAG(3, pPort->pController, "DMA unload - StartRx]");
                                return 0;
                            }
                            else
                            {
                                pEnd->bDmaResidual = FALSE;
                                pEnd->pRxIrp = NULL;
                                pDmaController->pfDmaReleaseChannel(pDmaChannel);
                                pEnd->pRxDmaChannel = NULL;
                            }
                        }
                    }
#endif
                    bResult = MGC_PipePacketReceived(pPort, MUSB_STATUS_OK, pEnd, wRxCount,
                                                     FALSE, pIrp);

                    /* clear recv condition if necessary */
                    wCsr &= ~MGC_M_RXCSR_RXPKTRDY;
                    if((wRxCount < pEnd->wRxPacketSize) || !(wCsr & MGC_M_RXCSR_AUTOCLEAR))
                    {
                        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wCsr);
                    }
                    /* complete IRP if necessary */
                    if(bResult)
                    {
                        if(MGC_CompleteIrp(&qItem, pEnd, (uint8_t)MUSB_STATUS_OK, FALSE, pIrp))
                        {
                            pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
                        }
                        MGC_DIAG(3, pPort->pController, "IRP Complete - StartRx]");
                        /* re-enable interrupt */
                        MGC_Write16(pBase, MGC_O_HDRC_INTRRXE, wIntrRxE);
                        MGC_DIAG(3, pPort->pController, "DMA unload - StartRx]");
                        return 0;
                    }
                }
                /* in the non-DMA case we may be slow enough that another packet is ready now */
                wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd);
            }
        }
    }

    /* no residual; set current IRP */
    pEnd->pRxIrp = pIrp;

#ifdef MUSB_DMA
    pDmaController = pPort->pDmaController;
    pDmaChannel = pEnd->pRxDmaChannel;
    if(!bAllowDma && pDmaChannel)
    {
        /* release previously-allocated channel */
        pDmaController->pfDmaReleaseChannel(pDmaChannel);
        pEnd->pRxDmaChannel = NULL;
    }
    else if(bAllowDma && pServices->pfSystemToBusAddress(pServices->pPrivateData, pBuffer))
    {
        /* candidate for DMA */
        if(pDmaController && !pDmaChannel)
        {
            pDmaChannel = pEnd->pRxDmaChannel = pDmaController->pfDmaAllocateChannel(
                                                    pDmaController->pPrivateData, bEnd, FALSE,
                                                    pEnd->bRxTrafficType, pEnd->wRxPacketSize);
        }
        if(pDmaChannel)
        {
            pEnd->bDmaTx = FALSE;
            pDmaChannel->dwActualLength = 0L;
            /* Program DMA channel */
            bDmaOk = pDmaController->pfDmaProgramChannel(pDmaChannel,
                     pEnd->wRxPacketSize, pDmaChannel->bDesiredMode, pBuffer,
                     MGC_MIN(dwTotalBytes, pDmaChannel->dwMaxLength));
            if(bDmaOk)
            {
                /* DMA channel is ready; finish RxCSR programming */
                if(pPort->bIsHost)
                {
                    wCsr &= ~MGC_M_RXCSR_H_REQPKT;
                    wCsr |= MGC_M_RXCSR_H_AUTOREQ;
                }
                wCsr |= (MGC_M_RXCSR_AUTOCLEAR | MGC_M_RXCSR_DMAENAB |
                         (pDmaChannel->bDesiredMode ? MGC_M_RXCSR_DMAMODE : 0));
            }
            else
            {
                pDmaController->pfDmaReleaseChannel(pDmaChannel);
                pEnd->pRxDmaChannel = NULL;
            }
        }
    }
#endif

    /* write RxCSR */
    if(pPort->bIsHost)
    {
        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wCsr);
    }
    else
    {
        MGC_WriteCsr8(pBase, (MGC_O_HDRC_RXCSR + 1), bEnd, (uint8_t)(wCsr >> 8));
    }

#ifdef MUSB_DMA
    if(bAllowDma && pDmaChannel && bDmaOk)
    {
        MGC_DIAG(3, pPort->pController, "StartRx]");
        return 0;
    }
#endif

    /* re-enable interrupt */
    MGC_Write16(pBase, MGC_O_HDRC_INTRRXE, wIntrRxE);

    MGC_DIAG(3, pPort->pController, "StartRx]");

    return 0;
}

/*
* Program the HDRC to initiate transmit.
*/
uint32_t MGC_HdrcStartTx(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                         const uint8_t *pBuffer, uint32_t dwTotalBytes,
                         void *pGenIrp)
{
#ifdef MUSB_DMA
    MUSB_DmaController *pDmaController;
    MUSB_DmaChannel *pDmaChannel;
    uint8_t bDmaOk;
#endif
    uint32_t dwLoadCount;
    uint16_t wCsr = 0;
    uint8_t reg = 0;
    MGC_Controller *pController = pPort->pController;
#ifdef MUSB_DMA
    uint8_t bAllowDma = FALSE;
    MUSB_Irp *pIrp = NULL;
    MUSB_IsochIrp *pIsochIrp = NULL;
    MUSB_ControlIrp *pControlIrp = NULL;
    MUSB_SystemServices *pServices = pController->pSystemServices;
#endif
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;
    uint8_t bEnd = pEnd->bLocalEnd;

    MGC_SelectEnd(pBase, bEnd);

    if(pPort->bIsHost && bEnd)
    {
        /* host mode; program protocol/endpoint */
        reg = pEnd->bTrafficType << MGC_S_TYPE_PROTO;
        reg |= pEnd->bBusEnd & MGC_M_TYPE_REMOTE_END;
        MGC_WriteCsr8(pBase, MGC_O_HDRC_TXTYPE, bEnd, reg);
    }

    /* determine how much to load into FIFO for non-DMA case */
    if((MUSB_ENDPOINT_XFER_BULK == pEnd->bTrafficType) && pPort->bHasBulkSplit)
    {
        dwLoadCount = MGC_MIN(pEnd->wMaxPacketSizeTx, dwTotalBytes);
    }
    else
    {
        dwLoadCount = MGC_MIN(pEnd->wPacketSize, dwTotalBytes);
    }

    if(bEnd)
    {
        /* read CSR for preparation */
        wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd) & ~MGC_M_TXCSR_P_UNDERRUN;
    }
    else
    {
        /* ensure FIFO ready for writing */
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, 0);
    }

#ifdef MUSB_DMA
    bDmaOk = FALSE;
    switch(pEnd->bTrafficType)
    {
    case MUSB_ENDPOINT_XFER_ISOC:
        pIsochIrp = (MUSB_IsochIrp *)pGenIrp;
        bAllowDma = pIsochIrp->bAllowDma;
        break;
    case MUSB_ENDPOINT_XFER_CONTROL:
        pControlIrp = (MUSB_ControlIrp *)pGenIrp;
        bAllowDma = pControlIrp->bAllowDma;
        break;
    default:
        pIrp = (MUSB_Irp *)pGenIrp;
        bAllowDma = pIrp->bAllowDma;
    }
    pDmaController = pPort->pDmaController;
    pDmaChannel = pEnd->pDmaChannel;
    if(!bAllowDma && pDmaChannel)
    {
        /* release previously-allocated channel */
        pDmaController->pfDmaReleaseChannel(pDmaChannel);
        pEnd->pDmaChannel = NULL;
    }
    else if(bAllowDma && pServices->pfSystemToBusAddress(pServices->pPrivateData, pBuffer))
    {
        /* candidate for DMA */
        if(pDmaController && !pDmaChannel)
        {
            pDmaChannel = pEnd->pDmaChannel = pDmaController->pfDmaAllocateChannel(
                                                  pDmaController->pPrivateData, bEnd, TRUE,
                                                  pEnd->bTrafficType, pEnd->wPacketSize);
        }
        if(pDmaChannel)
        {
            pEnd->bDmaTx = TRUE;
            pDmaChannel->dwActualLength = 0L;
            bDmaOk = pDmaController->pfDmaProgramChannel(pDmaChannel,
                     pEnd->wPacketSize, pDmaChannel->bDesiredMode, pBuffer,
                     MGC_MIN(dwTotalBytes, pDmaChannel->dwMaxLength));
            if(bDmaOk)
            {
                dwLoadCount = 0;
                if(bEnd)
                {
                    wCsr |= (MGC_M_TXCSR_AUTOSET | MGC_M_TXCSR_DMAENAB |
                             (pDmaChannel->bDesiredMode ? MGC_M_TXCSR_DMAMODE : 0));
                }
            }
            else
            {
                pDmaController->pfDmaReleaseChannel(pDmaChannel);
                pEnd->pDmaChannel = NULL;
            }
        }
    }
    if(!bDmaOk)
    {
#endif
        /* load FIFO */
        pPort->pfLoadFifo(pPort, pEnd->bLocalEnd, dwLoadCount, pBuffer);
        pEnd->dwTxSize = dwLoadCount;
        /* prepare CSR */
        if(bEnd)
        {
            /* since we cannot clear DMAMODE before/during clearing DMAENAB, never clear DMAMODE */
            wCsr &= ~(MGC_M_TXCSR_AUTOSET | MGC_M_TXCSR_DMAENAB);
            wCsr |= MGC_M_TXCSR_TXPKTRDY;
        }
        else
        {
            /* endpoint 0 is a fixed case */
            wCsr = MGC_M_CSR0_H_SETUPPKT | MGC_M_CSR0_TXPKTRDY;
            if(pPort->bHasDisablePing)
            {
                wCsr |= MGC_M_CSR0_H_NO_PING;
            }
        }
#ifdef MUSB_DMA
    }
#endif

    /* write CSR */
    pEnd->pTxIrp = pGenIrp;
    if(bEnd)
    {
        MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, wCsr | MGC_M_TXCSR_MODE);
    }
    else
    {
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, wCsr);
    }

    return 0;
}

/* Program the HDRC to flush the given endpoint wrt the given USB direction */
uint32_t MGC_HdrcFlushEndpoint(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                               uint8_t bDir, uint8_t bReuse)
{
    uint16_t wCsr;
    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;
    uint8_t bTx = FALSE;
    uint8_t bEnd = pEnd->bLocalEnd;

    /* select hw ep */
    MGC_SelectEnd(pBase, bEnd);

    /* determine TX/RX */
    if(bDir & MUSB_DIR_IN)
    {
        if(!pPort->bIsHost)
        {
            bTx = TRUE;
        }
    }
    else
    {
        if(pPort->bIsHost)
        {
            bTx = TRUE;
        }
    }

    if(bTx)
    {
        MGC_DIAG1(2, pController, "Flushing Tx on ", bEnd, 16, 2);
#ifdef MUSB_DMA
        if(pEnd->pDmaChannel && pEnd->bDmaTx)
        {
            pPort->pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
            pEnd->pDmaChannel = NULL;
        }
#endif
        /*  flush the xmit fifo, reset the data tog to 0 */
        wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd);
        MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd,
                       wCsr | (MGC_M_TXCSR_FLUSHFIFO | MGC_M_TXCSR_CLRDATATOG));
        MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, wCsr | MGC_M_TXCSR_FLUSHFIFO);
    }
    else
    {
        MGC_DIAG1(2, pController, "Flushing Rx on ", bEnd, 16, 2);
        if(pPort->bIsHost)
        {
            if(bEnd)
            {
                wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd);
                wCsr &= ~MGC_M_RXCSR_H_REQPKT;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wCsr);
            }
            else
            {
                wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_CSR0, 0);
                wCsr &= ~MGC_M_CSR0_H_REQPKT;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, wCsr);
            }
        }
#ifdef MUSB_DMA
        if(pEnd->pDmaChannel && !pEnd->bDmaTx)
        {
            pPort->pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
            pEnd->pDmaChannel = NULL;
        }
#endif
        /*  flush the recv fifo, reset the data tog to 0 */
        wCsr = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd);
        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd,
                       wCsr | (MGC_M_RXCSR_FLUSHFIFO | MGC_M_RXCSR_CLRDATATOG));
        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wCsr | MGC_M_RXCSR_FLUSHFIFO);
    }

#ifdef MUSB_C_DYNFIFO_DEF
    if(bReuse)
    {
        pEnd->wMaxPacketSizeTx = pEnd->wMaxPacketSizeRx = 0;
    }
#endif

    return 0;
}

/* Program the HDRC to set/clear the halting (stall) of an endpoint */
uint32_t MGC_HdrcHaltEndpoint(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                              uint8_t bDir, uint8_t bHalt)
{
    uint16_t val;
    uint8_t bTx = FALSE;
    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;

    /* determine TX/RX */
    if(bDir & MUSB_DIR_IN)
    {
        if(!pPort->bIsHost)
        {
            bTx = TRUE;
        }
    }
    else
    {
        if(pPort->bIsHost)
        {
            bTx = TRUE;
        }
    }

    MGC_SelectEnd(pBase, pEnd->bLocalEnd);

    if(bTx)
    {
        /* Tx */
        val = MGC_ReadCsr16(pBase, MGC_O_HDRC_TXCSR, pEnd->bLocalEnd);
        if(bHalt)
        {
            if(pPort->bIsHost)
            {
                /* mark for stopping */
                pEnd->bStopTx = TRUE;
                if(!pEnd->pTxIrp)
                {
                    /* if already stopped, indicate it */
                    pEnd->bIsHalted = TRUE;
                }
            }
            else
            {
                val &= ~(MGC_M_TXCSR_P_UNDERRUN | MGC_M_TXCSR_P_INCOMPTX);
                val |= MGC_M_TXCSR_P_SENDSTALL;
            }
        }
        else
        {
            if(pPort->bIsHost)
            {
                /* restart Tx traffic */
                pEnd->bIsHalted = FALSE;
                /* start next IRP (if any) */
                MGC_StartNextIrp(pPort, pEnd, TRUE);
            }
            else
            {
                val &= ~(MGC_M_TXCSR_P_SENDSTALL | MGC_M_TXCSR_P_UNDERRUN | MGC_M_TXCSR_P_INCOMPTX);
                /* apparently the hardware is not doing this: */
                val |= MGC_M_TXCSR_CLRDATATOG;
            }
        }
        MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, pEnd->bLocalEnd, val);
    }
    else
    {
        /* Rx */
        val = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, pEnd->bLocalEnd);
        if(bHalt)
        {
            if(pPort->bIsHost)
            {
                /* stop Rx traffic */
                pEnd->bIsRxHalted = TRUE;
                val &= ~(MGC_M_RXCSR_H_REQPKT | MGC_M_RXCSR_H_AUTOREQ | MGC_M_RXCSR_DMAENAB);
            }
            else
            {
                val |= MGC_M_RXCSR_P_SENDSTALL;
            }
        }
        else
        {
            if(pPort->bIsHost)
            {
                /* restart Rx traffic */
                pEnd->bIsRxHalted = FALSE;
                if(pEnd->pRxIrp)
                {
                    /* restart pending IRP */
#ifdef MUSB_DMA
                    val |= (pEnd->pDmaChannel && !pEnd->bDmaTx) ?
                           (MGC_M_RXCSR_H_AUTOREQ | MGC_M_RXCSR_DMAENAB) : MGC_M_RXCSR_H_REQPKT;
#else
                    val |= MGC_M_RXCSR_H_REQPKT;
#endif
                }
                else
                {
                    /* start next IRP (if any) */
                    MGC_StartNextIrp(pPort, pEnd, FALSE);
                }
            }
            else
            {
                val &= ~MGC_M_RXCSR_P_SENDSTALL;
                /* apparently the hardware is not doing this: */
                val |= MGC_M_RXCSR_CLRDATATOG;
            }
        }
        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, pEnd->bLocalEnd, val);
    }

    return 0;
}

/*
* Respond on default endpoint
*/
uint32_t MGC_HdrcDefaultEndResponse(MGC_Port *pPort, uint8_t bStall)
{
    uint16_t wSize, wCsrVal;
    uint8_t bVal = MGC_M_CSR0_P_SVDRXPKTRDY;
    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;

    MGC_SelectEnd(pBase, 0);
    wCsrVal = MGC_ReadCsr16(pBase, MGC_O_HDRC_CSR0, 0);

    if(wCsrVal & MGC_M_CSR0_P_SETUPEND)
    {
        bVal |= MGC_M_CSR0_P_SVDSETUPEND;
    }
    /* prepare stall response if required */
    if(bStall)
    {
        bVal |= MGC_M_CSR0_P_SENDSTALL;
    }
    else if(pPort->wSetupDataSize)
    {
        /* we need to transmit; ack RxPktRdy BEFORE loading FIFO */
        MGC_WriteCsr8(pBase, MGC_O_HDRC_CSR0, 0, bVal);
        bVal = 0;
        /* start transmit */
        wSize = MUSB_MIN(64, pPort->wSetupDataSize);
        MGC_HdrcLoadFifo(pPort, 0, wSize, pPort->pSetupData);
        pPort->wSetupDataIndex = wSize;
        bVal |= MGC_M_CSR0_TXPKTRDY;
        if(wSize == pPort->wSetupDataSize)
        {
            bVal |= MGC_M_CSR0_P_DATAEND;
            pPort->bEnd0Status = TRUE;
            pPort->wSetupDataIndex = 0;
            pPort->wSetupDataSize = 0;
        }
    }
    else
    {
        /* just acknowledge end of data */
        bVal |= MGC_M_CSR0_P_DATAEND;
        pPort->bEnd0Status = TRUE;
    }

    /* write CSR0 */
    MGC_DIAG1(3, pPort->pController, "DefaultEndResponse: Setting CSR0 to ", bVal, 16, 2);
    MGC_WriteCsr8(pBase, MGC_O_HDRC_CSR0, 0, bVal);

    return 0;
}

/*
 * Handle default endpoint interrupt
 */
uint8_t MGC_HdrcServiceDefaultEnd(MGC_Port *pPort, MGC_BsrItem *pItem)
{
    uint16_t wCsrVal, wCount;
#ifdef MUSB_OVERHEAD
    MUSB_SystemUtils *pUtils = pPort->pController->pUtils;
    uint32_t dwStartTime = pUtils->pfGetTime();
#endif
    uint8_t bTestVal;
    uint8_t bError = FALSE;
    uint8_t bResult = FALSE;
    uint8_t status = MUSB_STATUS_OK;
    MGC_EndpointResource *pEnd = MUSB_ArrayFetch(&(pPort->LocalEnds), 0);
    uint8_t *pBase = (uint8_t *)pPort->pController->pControllerAddressIsr;

    MGC_SelectEnd(pBase, 0);
    wCsrVal = MGC_ReadCsr16(pBase, MGC_O_HDRC_CSR0, 0);
    wCount = MGC_ReadCsr8(pBase, MGC_O_HDRC_COUNT0, 0);

    if(pPort->bIsHost)
    {
        if(wCsrVal & MGC_M_CSR0_H_RXSTALL)
        {
            MUSB_PRT("End 0 stall\r\n");
            status = MUSB_STATUS_STALLED;
            bError = TRUE;
        }
        else if(wCsrVal & MGC_M_CSR0_H_ERROR)
        {
            MUSB_PRT("end 0: no response (error)\r\n");
            status = MUSB_STATUS_NACK_LIMIT;
            bError = TRUE;
        }
        else if(wCsrVal & MGC_M_CSR0_H_NAKTIMEOUT)
        {
            MUSB_PRT("end 0: no response (NAK timeout)\r\n");
            status = MUSB_STATUS_NACK_LIMIT;
            bError = TRUE;
        }
        if(MUSB_STATUS_NACK_LIMIT == status)
        {
            MUSB_PRT("nak limit\r\n");
            /* use the proper sequence to abort the transfer */
            if(wCsrVal & MGC_M_CSR0_H_REQPKT)
            {
                wCsrVal &= ~MGC_M_CSR0_H_REQPKT;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, wCsrVal);
                wCsrVal &= ~MGC_M_CSR0_H_NAKTIMEOUT;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, wCsrVal);
            }
            else
            {
                wCsrVal |= MGC_M_CSR0_FLUSHFIFO;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, wCsrVal);
                MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, wCsrVal);
                wCsrVal &= ~MGC_M_CSR0_H_NAKTIMEOUT;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, wCsrVal);
            }
        }

        if(bError)
        {
            /* clear it */
            MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, 0);
        }
    }
    else if(wCsrVal & MGC_M_CSR0_P_SENTSTALL)
    {
        MUSB_PRT("[MGC] M_CSR0_P_SENTSTALL\r\n");
        bError = TRUE;
        wCsrVal &= ~MGC_M_CSR0_P_SENTSTALL;
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, wCsrVal);
    }
    else if(wCsrVal & MGC_M_CSR0_P_SETUPEND)
    {
        MUSB_PRT("[MGC] M_CSR0_P_SETUPEND\r\n");
        pPort->wSetupDataIndex = 0;
        pPort->wSetupDataSize = 0;
#ifdef MUSB_STATS
        pPort->dwResponseTimeoutCount++;
#endif
    }
    else if(!wCount && pPort->bEnd0Status)
    {
        /* this is the "acknowledge" interrupt */
        pPort->bEnd0Status = FALSE;
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, 0);

        /* set address if required */
        if(pPort->bSetAddress)
        {
            MGC_HdrcProgramBusState(pPort);
            pPort->bSetAddress = FALSE;
        }

        /* set TESTMODE register if required */
        if(pPort->bWantTestMode)
        {
            MUSB_PRT("mgc_test_mode\r\n");
            pPort->bWantTestMode = FALSE;
            bTestVal = 0;
            switch(pPort->bTestMode)
            {
            case MUSB_TESTMODE_TEST_J:
                bTestVal = MGC_M_TEST_J;
                break;
            case MUSB_TESTMODE_TEST_K:
                bTestVal = MGC_M_TEST_K;
                break;
            case MUSB_TESTMODE_TEST_SE0_NAK:
                bTestVal = MGC_M_TEST_SE0_NAK;
                break;
            case MUSB_TESTMODE_TEST_PACKET:
                bTestVal = MGC_M_TEST_PACKET;
                MGC_HdrcLoadFifo(pPort, 0, sizeof(MGC_aTestPacket),
                                 MGC_aTestPacket);
                /* despite explicit instruction, we still must kick-start */
                MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, MGC_M_CSR0_TXPKTRDY);
                break;
            default:
                break;
            }
            MGC_Write8(pBase, MGC_O_HDRC_TESTMODE, bTestVal);
        }

        return FALSE;
    }

    if(bError)
    {
        MUSB_PRT("mgc_HdrcService_error\r\n");
        if(pPort->bIsHost)
        {
            if(MGC_CompleteIrp(pItem, pEnd, status, TRUE, pEnd->pTxIrp))
            {
                bResult = TRUE;
            }
            /* start next transfer if any */
            MGC_StartNextControlTransfer(pPort);
        }
    }
    else
    {
        /* call common logic */
        bResult = MGC_DrcServiceDefaultEnd(pPort, pItem, &wCsrVal, wCount);
        /* write CSR0 */
        if(wCsrVal)
        {
            if(pPort->bIsHost && pPort->bHasDisablePing)
            {
                wCsrVal |= MGC_M_CSR0_H_NO_PING;
            }

            MUSB_PRT("ServiceDefaultEnd: Setting CSR0 to 0x%x\r\n", wCsrVal);
            MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, wCsrVal);
        }
    }
#ifdef MUSB_OVERHEAD
    pEnd->dwIsrTime += pUtils->pfGetTime() - dwStartTime;
#endif

    return bResult;
}

uint8_t MGC_HdrcServiceTransmitAvail(MGC_Port *pPort, uint16_t wEndIndex, MGC_BsrItem *pItem)
{
    uint16_t wVal;
    MGC_EndpointResource *pEnd;
    uint8_t bResult = FALSE;
    uint32_t status = MUSB_STATUS_OK;
    uint8_t bEnd = (uint8_t)wEndIndex;
    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;

    pItem->bStatus = MUSB_STATUS_OK;
    pItem->bCause = MGC_BSR_CAUSE_NONE;

    MGC_SelectEnd(pBase, bEnd);
    wVal = MGC_ReadCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd);

    pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds),
            wEndIndex);
    do
    {
        if(pPort->bIsHost)
        {
            /* check for stall */
            if(wVal & MGC_M_TXCSR_H_RXSTALL)
            {
                status = MUSB_STATUS_STALLED;
            }
            /* check for NAK timeout */
            if((wVal & MGC_M_TXCSR_H_ERROR) || (wVal & MGC_M_TXCSR_H_NAKTIMEOUT))
            {
                status = MUSB_STATUS_TRANSMIT_ERROR;
                /* do the proper sequence to abort the transfer */
                wVal &= ~MGC_M_TXCSR_FIFONOTEMPTY;
                wVal |= MGC_M_TXCSR_FLUSHFIFO;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, wVal);
                MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, wVal);
            }
            if(status)
            {
                /* reset error and DMA bits and force FIFO flush */
                wVal &= ~(MGC_M_TXCSR_H_ERROR | MGC_M_TXCSR_H_RXSTALL |
                          MGC_M_TXCSR_H_NAKTIMEOUT | MGC_M_TXCSR_DMAENAB | MGC_M_TXCSR_DMAMODE);
                wVal |= MGC_M_TXCSR_FRCDATATOG;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, wVal);
                bResult = MGC_CompleteIrp(pItem, pEnd, (uint8_t)status, TRUE, pEnd->pTxIrp);
                /* start next IRP if any */
                if(pEnd->bStopTx)
                {
                    pEnd->bIsHalted = TRUE;
                }
                return bResult;
            }
            else
            {
                if(pEnd)
                {
#ifdef MUSB_DMA
                    if(pEnd->pDmaChannel)
                    {
                        if(MUSB_DMA_STATUS_FREE ==
                                pPort->pDmaController->pfDmaGetChannelStatus(pEnd->pDmaChannel))
                        {
                            pEnd->dwTxOffset = pEnd->pDmaChannel->dwActualLength;
                        }
                    }
                    else
#endif
                        pEnd->dwTxOffset += pEnd->dwTxSize;
                    bResult = MGC_PipeTransmitReady(pPort, pEnd);
                }
                else
                {
                    bResult = TRUE;
                    status = MUSB_STATUS_INTERNAL_ERROR;
                }
            }
            if(bResult)
            {
#ifdef MUSB_DMA
                if(pEnd->pDmaChannel)
                {
                    pPort->pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
                    pEnd->pDmaChannel = NULL;
                }
                /* clear DMA bits */
                wVal = MGC_ReadCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd);
                wVal &= ~(MGC_M_TXCSR_DMAENAB | MGC_M_TXCSR_DMAMODE);
                MGC_WriteCsr8(pBase, (MGC_O_HDRC_TXCSR + 1), bEnd, wVal >> 8);
#endif
                bResult = MGC_CompleteIrp(pItem, pEnd, (uint8_t)status, TRUE, pEnd->pTxIrp);
                /* start next IRP if any */
                if(pEnd->bStopTx)
                {
                    pEnd->bIsHalted = TRUE;
                }
            }
        }
        else
        {
            /* Function role */

            if(wVal & MGC_M_TXCSR_P_SENTSTALL)
            {
                wVal &= ~MGC_M_TXCSR_P_SENTSTALL;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, wVal);
                break;
            }
            if(wVal & MGC_M_TXCSR_P_UNDERRUN)
            {
                wVal &= ~MGC_M_TXCSR_P_UNDERRUN;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd, wVal);
            }

            if(pEnd)
            {
#ifdef MUSB_DMA
                if(pEnd->pDmaChannel)
                {
                    if(MUSB_DMA_STATUS_FREE ==
                            pPort->pDmaController->pfDmaGetChannelStatus(pEnd->pDmaChannel))
                    {
                        pEnd->dwTxOffset = pEnd->pDmaChannel->dwActualLength;
                    }
                }
                else
#endif
                    pEnd->dwTxOffset += pEnd->dwTxSize;
                if(pEnd->pTxIrp)
                {
                    bResult = MGC_PipeTransmitReady(pPort, pEnd);
                    if(bResult)
                    {
#ifdef MUSB_DMA
                        if(pEnd->pDmaChannel)
                        {
                            pPort->pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
                            pEnd->pDmaChannel = NULL;
                        }
                        /* clear DMA bits */
                        wVal = MGC_ReadCsr16(pBase, MGC_O_HDRC_TXCSR, bEnd);
                        wVal &= ~(MGC_M_TXCSR_DMAENAB | MGC_M_TXCSR_DMAMODE);
                        MGC_WriteCsr8(pBase, (MGC_O_HDRC_TXCSR + 1), bEnd, wVal >> 8);
#endif
                        bResult = MGC_CompleteIrp(pItem, pEnd, (uint8_t)status, TRUE, pEnd->pTxIrp);
                    }
                }
            }
        }
    }
    while(FALSE);

    return bResult;
}

uint8_t MGC_HdrcServiceReceiveReady(MGC_Port *pPort, uint16_t wEndIndex, MGC_BsrItem *pItem)
{
    uint16_t wVal;
    MGC_EndpointResource *pEnd;
    uint16_t wRxCount;
    uint8_t bEnd = (uint8_t)wEndIndex;
    uint8_t bResult = FALSE;
    uint32_t status = MUSB_STATUS_OK;
    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;

    pItem->bStatus = MUSB_STATUS_OK;
    pItem->bCause = MGC_BSR_CAUSE_NONE;

    MGC_SelectEnd(pBase, bEnd);
    wVal = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd);

    pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds),
            wEndIndex);
    do
    {
        if(pPort->bIsHost)
        {
            /* check for stall */
            if(wVal & MGC_M_RXCSR_H_RXSTALL)
            {
                MUSB_PRT("[MGC] M_RXCSR_H_RXSTALL\r\n");
                status = MUSB_STATUS_STALLED;
                /* avoid a race with next StartRx by clearing ReqPkt */
                wVal &= ~MGC_M_RXCSR_H_REQPKT;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wVal);
            }
            /* check for RX error */
            if(wVal & (MGC_M_RXCSR_H_ERROR | MGC_M_RXCSR_DATAERR))
            {
                MUSB_PRT("[MGC] M_RXCSR_ERROR:0x%x\r\n", wVal);

                status = MUSB_STATUS_RECEIVE_ERROR;
                /* do the proper sequence to abort the transfer */
                wVal &= ~MGC_M_RXCSR_H_REQPKT;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wVal);
            }
            /* be sure a packet is ready for unloading */
            if(!status && !(wVal & MGC_M_RXCSR_RXPKTRDY))
            {
                status = MUSB_STATUS_INTERNAL_ERROR;
                /* do the proper sequence to abort the transfer */
                wVal &= ~MGC_M_RXCSR_H_REQPKT;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wVal);
            }
            if(status)
            {
                /* reset error bits */
                wVal &= ~(MGC_M_RXCSR_H_ERROR | MGC_M_RXCSR_DATAERR |
                          MGC_M_RXCSR_H_RXSTALL | MGC_M_RXCSR_RXPKTRDY);
                MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wVal);
                bResult = MGC_CompleteIrp(pItem, pEnd, (uint8_t)status, FALSE, pEnd->pRxIrp);
                bResult = TRUE;
            }
            else
            {
                if(pEnd && pEnd->pRxIrp)
                {
                    /* we are expecting traffic */
#ifdef MUSB_DMA
                    /* if we were using DMA, update actual byte count */
                    if(pEnd->pRxDmaChannel)
                    {
                        if(MUSB_DMA_STATUS_FREE ==
                                pPort->pDmaController->pfDmaGetChannelStatus(pEnd->pRxDmaChannel))
                        {
                            pEnd->dwRxOffset = pEnd->pRxDmaChannel->dwActualLength;
                        }
                    }
#endif
                    /* read FIFO count and see if current IRP is satisfied */
                    wRxCount = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCOUNT, bEnd);
                    bResult = MGC_PipePacketReceived(pPort, status, pEnd, wRxCount,
                                                     TRUE, pEnd->pRxIrp);
                    if(bResult)
                    {
                        /* curent IRP is satisfied */
#ifdef MUSB_DMA
                        if(pEnd->pRxDmaChannel)
                        {
                            pPort->pDmaController->pfDmaReleaseChannel(pEnd->pRxDmaChannel);
                            pEnd->pRxDmaChannel = NULL;
                        }
#endif
                        /* ack recv */
                        wVal = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd);
                        wVal &= ~MGC_M_RXCSR_RXPKTRDY;
                        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wVal);
                        /* complete IRP */
                        bResult = MGC_CompleteIrp(pItem, pEnd, (uint8_t)status, FALSE, pEnd->pRxIrp);
                    }
                }
                else
                {
                    bResult = TRUE;
                    status = MUSB_STATUS_INTERNAL_ERROR;
                }
            }
        }
        else
        {
            /*
            *  PERIPHERAL RX ( HOST OUT )
            */
            if(wVal & MGC_M_RXCSR_P_SENTSTALL)
            {
                wVal &= ~MGC_M_RXCSR_P_SENTSTALL;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wVal);
                break;
            }
            if(wVal & MGC_M_RXCSR_P_OVERRUN)
            {
                wVal &= ~MGC_M_RXCSR_P_OVERRUN;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wVal);
                break;
            }
            /*
            *  Fresh packet received
            */
            if(!pEnd || pEnd->bIsHalted)
            {
                /* couldn't find endpoint or it is halted */
                status = pEnd ? 0 : MUSB_STATUS_INTERNAL_ERROR;
                MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, MGC_M_RXCSR_P_SENDSTALL);
                break;
            }

            if(pEnd && pEnd->pRxIrp)
            {
                /* we are expecting traffic */
                while(pEnd->pRxIrp &&
                        ((pEnd->bBsrResidual != pEnd->bIsrResidual) || (wVal & MGC_M_RXCSR_RXPKTRDY)))
                {
                    if(pEnd->bBsrResidual != pEnd->bIsrResidual)
                    {
                        pEnd->bBsrResidual++;
                    }
#ifdef MUSB_DMA
                    /* if we were using DMA, update actual byte count */
                    if(pEnd->pRxDmaChannel)
                    {
                        if(MUSB_DMA_STATUS_FREE ==
                                pPort->pDmaController->pfDmaGetChannelStatus(pEnd->pRxDmaChannel))
                        {
                            pEnd->dwRxOffset = pEnd->pRxDmaChannel->dwActualLength;
                            pEnd->bDmaResidual = FALSE;
                        }
                    }
                    if(pEnd->bDmaResidual)
                    {
                        return FALSE;
                    }
#endif
                    /* read FIFO count and see if current IRP is satisfied */
                    wRxCount = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCOUNT, bEnd);
                    bResult = MGC_PipePacketReceived(pPort, status, pEnd, wRxCount,
                                                     FALSE, pEnd->pRxIrp);
                    /* re-read CSR */
                    wVal = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd);
                    /* clear recv condition if necessary */
                    if((wRxCount < pEnd->wRxPacketSize) || !(wVal & MGC_M_RXCSR_AUTOCLEAR))
                    {
                        wVal &= ~MGC_M_RXCSR_RXPKTRDY;
                        MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wVal);
                    }
                    if(bResult)
                    {
                        /* current IRP is satisfied */
#ifdef MUSB_DMA
                        if(pEnd->pRxDmaChannel)
                        {
                            pPort->pDmaController->pfDmaReleaseChannel(pEnd->pRxDmaChannel);
                            pEnd->pRxDmaChannel = NULL;
                        }
#endif
                        bResult = MGC_CompleteIrp(pItem, pEnd, (uint8_t)status, FALSE, pEnd->pRxIrp);

                        /* clear DMA bits */
                        MGC_WriteCsr8(pBase, (MGC_O_HDRC_RXCSR + 1), bEnd,
                                      (uint8_t)((wVal & ~(MGC_M_RXCSR_DMAENAB | MGC_M_RXCSR_DMAMODE)) >> 8));

                    }
                }
            }
            else
            {
                pEnd->bIsrResidual++;
            }
        }   /* END: PERIPHERAL RX ( HOST OUT )  */

    }
    while(FALSE);

    return bResult;
}

#ifdef MUSB_EHSET
static void MGC_HdrcResetOff(void *pParam, uint16_t wTimer)
{
    uint8_t bReg;
    MGC_Controller *pController = (MGC_Controller *)pParam;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;

    MUSB_PRT("[MGC] HdrcResetOff\r\n");
    bReg = MGC_Read8(pBase, MGC_O_HDRC_POWER);
    MGC_Write8(pBase, MGC_O_HDRC_POWER, bReg & ~MGC_M_POWER_RESET);
}

static void MGC_HdrcResumeOff(void *pParam, uint16_t wTimer)
{
    uint8_t bReg;
    MGC_Controller *pController = (MGC_Controller *)pParam;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;

    MUSB_PRT("[MGC] HdrcResumeOff\r\n");
    bReg = MGC_Read8(pBase, MGC_O_HDRC_POWER);
    MGC_Write8(pBase, MGC_O_HDRC_POWER, bReg & ~MGC_M_POWER_RESUME);
}
#endif

void MGC_HdrcSetPortTestMode(MGC_Port *pPort, MUSB_HsetPortMode eMode)
{
#ifdef MUSB_EHSET
    uint8_t bReg;

    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;
    uint16_t wReg = MGC_Read16(pBase, MGC_O_HDRC_INTRTX);

    switch(eMode)
    {
    case MUSB_HSET_PORT_NONE:
        MGC_Write8(pBase, MGC_O_HDRC_TESTMODE, 0);
        /* re-enable endpoint 0 interrupts */
        MGC_Write16(pBase, MGC_O_HDRC_INTRTX, wReg | 1);
        break;

    case MUSB_HSET_PORT_TEST_J:
        MGC_Write8(pBase, MGC_O_HDRC_TESTMODE, MGC_M_TEST_J);
        break;

    case MUSB_HSET_PORT_TEST_K:
        MGC_Write8(pBase, MGC_O_HDRC_TESTMODE, MGC_M_TEST_K);
        break;

    case MUSB_HSET_PORT_TEST_SE0_NAK:
        MGC_Write8(pBase, MGC_O_HDRC_TESTMODE, MGC_M_TEST_SE0_NAK);
        break;

    case MUSB_HSET_PORT_TEST_PACKET:
        MGC_SelectEnd(pBase, 0);
        MGC_HdrcLoadFifo(pPort, 0, sizeof(MGC_aTestPacket), MGC_aTestPacket);
        /* despite explicit instruction, we still must kick-start */
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, MGC_M_CSR0_TXPKTRDY);
        MGC_Write8(pBase, MGC_O_HDRC_TESTMODE, MGC_M_TEST_PACKET);
        break;

    case MUSB_HSET_PORT_RESET:
        MUSB_PRT("[MGC] HSET_PORT_RESET\r\n");
        bReg = MGC_Read8(pBase, MGC_O_HDRC_POWER);
        MGC_Write8(pBase, MGC_O_HDRC_POWER, bReg | MGC_M_POWER_RESET);
        pController->pSystemServices->pfArmTimer(
            pController->pSystemServices->pPrivateData, 0, 60, FALSE,
            MGC_HdrcResetOff);
        break;

    case MUSB_HSET_PORT_SUSPEND:
        MUSB_PRT("[MGC] HSET_PORT_SUSPEND\r\n");
        bReg = MGC_Read8(pBase, MGC_O_HDRC_POWER);
        MGC_Write8(pBase, MGC_O_HDRC_POWER, bReg | MGC_M_POWER_SUSPENDM);
        break;

    case MUSB_HSET_PORT_RESUME:
        MUSB_PRT("[MGC] HSET_PORT_RESUME\r\n");
        bReg = MGC_Read8(pBase, MGC_O_HDRC_POWER) & ~MGC_M_POWER_SUSPENDM;
        MGC_Write8(pBase, MGC_O_HDRC_POWER, bReg | MGC_M_POWER_RESUME);
        pController->pSystemServices->pfArmTimer(
            pController->pSystemServices->pPrivateData, 0, 10, FALSE,
            MGC_HdrcResumeOff);
        break;

    case MUSB_HSET_PORT_SETUP_START:
        /* disable endpoint 0 interrupts as a cheat for single-step mode */
        MGC_Write16(pBase, MGC_O_HDRC_INTRTX, wReg & ~0xfffe);
        MGC_SelectEnd(pBase, 0);
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, MGC_M_CSR0_FLUSHFIFO);
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, MGC_M_CSR0_FLUSHFIFO);
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, MGC_M_CSR0_TXPKTRDY | MGC_M_CSR0_H_SETUPPKT);
        break;

    case MUSB_HSET_PORT_SETUP_IN:
        /* disable endpoint 0 interrupts as a cheat for single-step mode */
        MGC_Write16(pBase, MGC_O_HDRC_INTRTX, wReg & ~0xfffe);
        MGC_SelectEnd(pBase, 0);
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, MGC_M_CSR0_H_REQPKT);
        break;

    case MUSB_HSET_PORT_SETUP_STATUS:
        /* disable endpoint 0 interrupts as a cheat for single-step mode */
        MGC_Write16(pBase, MGC_O_HDRC_INTRTX, wReg & ~0xfffe);
        MGC_SelectEnd(pBase, 0);
        MGC_WriteCsr16(pBase, MGC_O_HDRC_CSR0, 0, MGC_M_CSR0_H_REQPKT | MGC_M_CSR0_H_STATUSPKT);
        break;
    }
#endif
}

uint8_t MGC_HdrcLoadFifo(MGC_Port *pPort, uint8_t bEnd, uint32_t dwCount, const uint8_t *pSource)
{
    uint32_t dwIndex, dwIndex32;
    uint32_t dwDatum;
    uint32_t dwCount32 = ((uint32_t)pSource & 3) ? 0 : (dwCount >> 2);
    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;
    uint8_t bFifoOffset = MGC_FIFO_OFFSET(bEnd);

    /* doublewords when possible */
    for(dwIndex = dwIndex32 = 0; dwIndex32 < dwCount32; dwIndex32++, dwIndex += 4)
    {
        dwDatum = *((uint32_t *) & (pSource[dwIndex]));
        MGC_Write32(pBase, bFifoOffset, dwDatum);
    }
    while(dwIndex < dwCount)
    {
        MGC_Write8(pBase, bFifoOffset, pSource[dwIndex++]);
    }

    return TRUE;
}

uint8_t MGC_HdrcUnloadFifo(MGC_Port *pPort, uint8_t bEnd, uint32_t dwCount, uint8_t *pDest)
{
    uint32_t dwIndex, dwIndex32;
    uint32_t dwCount32 = ((uint32_t)pDest & 3) ? 0 : (dwCount >> 2);
    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;
    uint8_t bFifoOffset = MGC_FIFO_OFFSET(bEnd);

    /* doublewords when possible */
    for(dwIndex = dwIndex32 = 0; dwIndex32 < dwCount32; dwIndex32++, dwIndex += 4)
    {
        *((uint32_t *) & (pDest[dwIndex])) = MGC_Read32(pBase, bFifoOffset);
    }
    while(dwIndex < dwCount)
    {
        pDest[dwIndex++] = MGC_Read8(pBase, bFifoOffset);
    }

    return TRUE;
}

uint8_t MGC_HdrcUlpiVbusControl(MGC_Port *pPort, uint8_t bExtSource, uint8_t bExtIndicator)
{
    uint8_t bVal;
    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;

    /* ensure not powered down */
    if(MGC_Read8(pBase, MGC_O_HDRC_POWER) & MGC_M_POWER_ENSUSPEND)
    {
        return FALSE;
    }

    bVal = bExtSource ? MGC_M_ULPI_VBUSCTL_USEEXTVBUS : 0;
    bVal |= bExtIndicator ? MGC_M_ULPI_VBUSCTL_USEEXTVBUSIND : 0;
    MGC_Write8(pBase, MGC_O_HDRC_ULPI_VBUSCTL, bVal);
    return TRUE;
}

uint8_t MGC_HdrcReadUlpiReg(MGC_Port *pPort, uint8_t bAddr, uint8_t *pbData)
{
    uint8_t bCtl = 0;
    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;

    /* ensure not powered down */
    if(MGC_Read8(pBase, MGC_O_HDRC_POWER) & MGC_M_POWER_ENSUSPEND)
    {
        return FALSE;
    }

    /* polled */
    MGC_Write8(pBase, MGC_O_HDRC_ULPI_REGADDR, bAddr);
    MGC_Write8(pBase, MGC_O_HDRC_ULPI_REGCTL,
               MGC_M_ULPI_REGCTL_READNOTWRITE | MGC_M_ULPI_REGCTL_REG);
    while(!(MGC_M_ULPI_REGCTL_COMPLETE & bCtl))
    {
        bCtl = MGC_Read8(pBase, MGC_O_HDRC_ULPI_REGCTL);
    }
    *pbData = MGC_Read8(pBase, MGC_O_HDRC_ULPI_REGDATA);
    MGC_Write8(pBase, MGC_O_HDRC_ULPI_REGCTL, 0);

    return TRUE;
}

uint8_t MGC_HdrcWriteUlpiReg(MGC_Port *pPort, uint8_t bAddr, uint8_t bData)
{
    uint8_t bCtl = 0;
    MGC_Controller *pController = pPort->pController;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIst;

    /* ensure not powered down */
    if(MGC_Read8(pBase, MGC_O_HDRC_POWER) & MGC_M_POWER_ENSUSPEND)
    {
        return FALSE;
    }

    /* polled */
    MGC_Write8(pBase, MGC_O_HDRC_ULPI_REGADDR, bAddr);
    MGC_Write8(pBase, MGC_O_HDRC_ULPI_REGDATA, bData);
    MGC_Write8(pBase, MGC_O_HDRC_ULPI_REGCTL, MGC_M_ULPI_REGCTL_REG);
    while(!(MGC_M_ULPI_REGCTL_COMPLETE & bCtl))
    {
        bCtl = MGC_Read8(pBase, MGC_O_HDRC_ULPI_REGCTL);
    }
    MGC_Write8(pBase, MGC_O_HDRC_ULPI_REGCTL, 0);

    return TRUE;
}

uint8_t MGC_HdrcDmaChannelStatusChanged(
    void *pPrivateData, uint8_t bLocalEnd, uint8_t bTransmit)
{
#ifdef MUSB_DMA
    MGC_BsrItem qItem;
    uint8_t bQueue;
    MGC_EndpointResource *pEnd;
    uint16_t wVal, wIntrRxE;
    uint16_t wRxCount = 0;
    uint32_t status = MUSB_STATUS_OK;
    MGC_Port *pPort = (MGC_Port *)pPrivateData;
    MGC_Controller *pController = pPort->pController;
    MUSB_SystemServices *pServices = pController->pSystemServices;
    uint8_t *pBase = (uint8_t *)pController->pControllerAddressIsr;
    /* save index */
    uint8_t bIndex = MGC_Read8(pBase, MGC_O_HDRC_INDEX);

    pEnd = (MGC_EndpointResource *)MUSB_ArrayFetch(&(pPort->LocalEnds),
            bLocalEnd);
    if((bTransmit && !pEnd->pDmaChannel) || (!bTransmit && !pEnd->pRxDmaChannel))
    {
        return FALSE;
    }

    /* TODO: something more specific might be better */
    if(bTransmit)
    {
        bQueue = MGC_HdrcServiceTransmitAvail(pPort, bLocalEnd, &qItem);
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
        MGC_SelectEnd(pBase, bLocalEnd);
        wVal = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCSR, bLocalEnd);

        /* update actual byte count */
        if(pEnd->pRxDmaChannel)
        {
            if(MUSB_DMA_STATUS_FREE ==
                    pPort->pDmaController->pfDmaGetChannelStatus(pEnd->pRxDmaChannel))
            {
                pEnd->dwRxOffset = pEnd->pRxDmaChannel->dwActualLength;
                pEnd->bDmaResidual = FALSE;
            }
        }
        if(pEnd->bDmaResidual)
        {
            return FALSE;
        }
        /* read FIFO count and see if current IRP is satisfied */
        wRxCount = MGC_ReadCsr16(pBase, MGC_O_HDRC_RXCOUNT, bLocalEnd);

        bQueue = MGC_PipePacketReceived(pPort, status, pEnd, wRxCount,
                                        FALSE, pEnd->pRxIrp);
        /* clear recv condition if necessary */
        if((wRxCount < pEnd->wRxPacketSize) || !(wVal & MGC_M_RXCSR_AUTOCLEAR))
        {
            wVal &= ~MGC_M_RXCSR_RXPKTRDY;
            MGC_WriteCsr16(pBase, MGC_O_HDRC_RXCSR, bEnd, wVal);
        }
        if(bQueue)
        {
            /* current IRP is satisfied */
            if(pEnd->pRxDmaChannel)
            {
                pPort->pDmaController->pfDmaReleaseChannel(pEnd->pRxDmaChannel);
                pEnd->pRxDmaChannel = NULL;
            }
            bQueue = MGC_CompleteIrp(&qItem, pEnd, (uint8_t)status, FALSE, pEnd->pRxIrp);

            /* clear DMA bits */
            MGC_WriteCsr8(pBase, (MGC_O_HDRC_RXCSR + 1), bLocalEnd,
                          (uint8_t)((wVal & ~(MGC_M_RXCSR_DMAENAB | MGC_M_RXCSR_DMAMODE)) >> 8));
            /* enable endpoint interrupt */
            wIntrRxE = MGC_Read16(pBase, MGC_O_HDRC_INTRRXE);
            MGC_Write16(pBase, MGC_O_HDRC_INTRRXE, wIntrRxE | (1 << bLocalEnd));

#ifdef MUSB_OVERHEAD
            qItem.dwTime = pController->pUtils->pfGetTime();
#endif
            qItem.bLocalEnd = bLocalEnd;
            pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
        }
    }
    pPort->bDmaCompleted = bQueue;

    /* restore index */
    MGC_Write8(pBase, MGC_O_HDRC_INDEX, bIndex);

    return bQueue;

#else

    return FALSE;

#endif
}

#if MUSB_DIAG > 0

/*
* Dump info global to the HDRC
*/
int MGC_HdrcDumpState(MGC_Controller *pController)
{
    /* TODO */
    return 0;
}

/*
* Dump info relevant to the given local endpoint resource
*/
int MGC_HdrcDumpEndpoint(MGC_Controller *pController, MUSB_EndpointResource *pEnd)
{
    /* TODO */
    return 0;
}

#endif	/* diagnostics enabled */

#endif	/* HDRC enabled */
#endif	/* CFG_USB */
// eof

