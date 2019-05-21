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

#ifndef __MUSB_HSFC_PROTO_H__
#define __MUSB_HSFC_PROTO_H__

/*
 * Prototypes for HSFC-specific functions
 * $Revision: 1.6 $
 */

#include "mu_impl.h"

#include "mu_fcpr.h"

/*
 * Initialize (discover FIFO config, etc.)
 */
extern uint8_t MGC_HsfcInit(MGC_Port *pPort);

/**
 * Try to allocate a local FIFO to be identified with the given endpoint descriptor.
 * NOTE: although this behavior is common with the FSFC,
 * the way the code is conditionally compiled has the effect that it can't
 * reside in a common file (unless always gets compiled, which is undesirable).
 * @param pPort port pointer
 * @param pUsbEnd device endpoint attributes pointer
 * @param pRequest requested local endpoint attributes
 * @param bBind TRUE to actually bind (reserve resources)
 * @param pbIsTx pointer to storage to indicate Tx (TRUE) or Rx (FALSE)
 * @return pointer on success
 * @return NULL on failure
 */
extern MGC_EndpointResource *MGC_HsfcBindDynamicEndpoint(MGC_Port *pPort,
        const MUSB_DeviceEndpoint *pUsbEnd,
        MUSB_EndpointResource *pRequest,
        uint8_t bBind, uint8_t *pbIsTx);

/** HSFC ISR */
extern int MGC_HsfcIsr(void *pParam);

/*
* Program the HSFC to start/stop (enable/disable interrupts, bus reset, etc.).
* This is where registers get twiddled; no place above.
*/
extern uint32_t MGC_HsfcStart(MGC_Controller *pController);
extern uint32_t MGC_HsfcStop(MGC_Controller *pController);

extern uint32_t MGC_HsfcDestroy(MGC_Controller *pController);

/*
* Read bus state
*/
extern uint32_t MGC_HsfcReadBusState(MGC_Port *pPort);

/* Program the bus state from the port (see MGC_Port) */
extern uint32_t MGC_HsfcProgramBusState(MGC_Port *pPort);

/*
* Find the required local endpoint resource
* to be identified with the given endpoint descriptor.
* This is where registers get twiddled; no place above.
*/
extern MGC_EndpointResource *MGC_HsfcBindEndpoint(MGC_Port *pPort,
        const MUSB_DeviceEndpoint *pUsbEnd,
        MUSB_EndpointResource *pRequest,
        uint8_t bBind);

/*
* Program the HSFC to initiate a transfer.
* This is where registers get twiddled; no place above.
*/
extern uint32_t MGC_HsfcStartRx(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                                uint8_t *pBuffer, uint32_t dwTotalBytes, void *pIrp, uint8_t bAllowDma);

extern uint32_t MGC_HsfcStartTx(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                                const uint8_t *pBuffer, uint32_t dwTotalBytes, void *pIrp);

/*
* Program the underlying port to flush the given endpoint resource,
* with respect to the given USB direction (IN or OUT).
* The resource should be ready for normal operation by return time.
*/
extern uint32_t MGC_HsfcFlushEndpoint(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                                      uint8_t bDir, uint8_t bReuse);

/*
* Program the HSFC's given endpoint's stall state
* (TRUE to set halt; FALSE to clear halt)
*/
extern uint32_t MGC_HsfcHaltEndpoint(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                                     uint8_t bDir, uint8_t bHalt);

extern uint32_t MGC_HsfcDefaultEndResponse(MGC_Port *pPort, uint8_t bStall);

/*
* Interrupts
*/
extern uint8_t MGC_HsfcServiceDefaultEnd(MGC_Port *pPort, MGC_BsrItem *pItem);
extern uint8_t MGC_HsfcServiceTransmitAvail(MGC_Port *pPort, uint16_t wEndIndex, MGC_BsrItem *pItem);
extern uint8_t MGC_HsfcServiceReceiveReady(MGC_Port *pPort, uint16_t wEndIndex, MGC_BsrItem *pItem);

/*
* FIFOs
*/
extern uint8_t MGC_HsfcLoadFifo(MGC_Port *pPort, uint8_t bEnd, uint32_t dwCount, const uint8_t *pSource);
extern uint8_t MGC_HsfcUnloadFifo(MGC_Port *pPort, uint8_t bEnd, uint32_t dwCount, uint8_t *pDest);

extern void MGC_HsfcSetPortTestMode(MGC_Port *pPort, MUSB_HsetPortMode eMode);

extern void MGC_HsfcDynamicFifoLocation(MGC_Port *pPort,
                                        uint8_t bEnd,
                                        uint8_t bIsTx,
                                        uint8_t bSet,
                                        uint32_t *pdwBase,
                                        uint8_t *pbSize83,
                                        uint8_t *pbDoubleBuffered);

/**
 * DMA channel status changed
 */
extern uint8_t MGC_HsfcDmaChannelStatusChanged(
    void *pPrivateData, uint8_t bLocalEnd, uint8_t bTransmit);

#if MUSB_DIAG > 0

/*
* Dump info global to the FDRC
*/
extern int MGC_HsfcDumpState(MGC_Controller *pController);

/*
* Dump info relevant to the given local endpoint resource
*/
extern int MGC_HsfcDumpEndpoint(MGC_Controller *pController, MUSB_EndpointResource *pEnd);

#endif	/* diagnostics enabled */

#endif	/* multiple inclusion protection */
