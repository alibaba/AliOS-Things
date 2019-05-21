/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2004              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

#ifndef __MUSB_FDRC_PROTO_H__
#define __MUSB_FDRC_PROTO_H__

/*
 * Prototypes for FDRC-specific functions
 * $Revision: 1.17 $
 */

#include "mu_impl.h"

#include "mu_drcpr.h"

/*
 * Initialize (discover FIFO config, etc.)
 */
extern uint8_t MGC_FdrcInit(MGC_Port *pPort);

/**
 * Try to allocate a local FIFO appropriate to the given remote endpoint (host role)
 * or to be identified with the given endpoint descriptor (function role).
 * NOTE: although this behavior is common with the HDRC,
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
extern MGC_EndpointResource *MGC_FdrcBindDynamicEndpoint(MGC_Port *pPort,
        const MUSB_DeviceEndpoint *pUsbEnd,
        MUSB_EndpointResource *pRequest,
        uint8_t bBind, uint8_t *pbIsTx);

/*
 * Interrupt Service Routine
 */
extern int MGC_FdrcIsr(void *pParam);


/*
* Program the FDRC to start (enable interrupts, bus reset, etc.).
* This is where registers get twiddled; no place above.
*/
extern uint32_t MGC_FdrcStart(MGC_Controller *pController);

/*
* Program the FDRC to stop (disable & flush interrupts).
* This is where registers get twiddled; no place above.
*/
extern uint32_t MGC_FdrcStop(MGC_Controller *pController);

extern uint32_t MGC_FdrcDestroy(MGC_Controller *pController);

/*
* Read bus state
*/
extern uint32_t MGC_FdrcReadBusState(MGC_Port *pPort);

/* Program the bus state from the port (see MGC_Port) */
extern uint32_t MGC_FdrcProgramBusState(MGC_Port *pPort);

/*
* Find the first or best (host) or required (function)
* local endpoint resource to communicate with the given remote endpoint (host)
* or to be identified with the given endpoint descriptor (function).
* This is where registers get twiddled; no place above.
*/
extern MGC_EndpointResource *MGC_FdrcBindEndpoint(MGC_Port *pPort,
        const MUSB_DeviceEndpoint *pUsbEnd,
        MUSB_EndpointResource *pRequest,
        uint8_t bBind);

/*
* Program the FDRC to prepare for a receive.
* This is where registers get twiddled; no place above.
*/
extern uint32_t MGC_FdrcStartRx(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                                uint8_t *pBuffer, uint32_t dwTotalBytes, void *pIrp, uint8_t bAllowDma);

/*
* Program the FDRC to initiate a transmit.
* This is where registers get twiddled; no place above.
*/
extern uint32_t MGC_FdrcStartTx(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                                const uint8_t *pBuffer, uint32_t dwTotalBytes, void *pIrp);

/*
* Program the underlying port to flush the given endpoint resource,
* with respect to the given USB direction (IN or OUT).
* The resource should be ready for normal operation by return time.
*/
extern uint32_t MGC_FdrcFlushEndpoint(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                                      uint8_t bDir, uint8_t bReuse);

/*
* Program the FDRC's given endpoint's stall state
* (TRUE to set halt; FALSE to clear halt)
*/
extern uint32_t MGC_FdrcHaltEndpoint(MGC_Port *pPort,
                                     MGC_EndpointResource *pEnd,
                                     uint8_t bDir, uint8_t bHalt);

extern uint32_t MGC_FdrcDefaultEndResponse(MGC_Port *pPort, uint8_t bStall);

/*
* Interrupts
*/
extern uint8_t MGC_FdrcServiceDefaultEnd(MGC_Port *pPort, MGC_BsrItem *pItem);
extern uint8_t MGC_FdrcServiceTransmitAvail(MGC_Port *pPort, uint16_t wEndIndex, MGC_BsrItem *pItem);
extern uint8_t MGC_FdrcServiceReceiveReady(MGC_Port *pPort, uint16_t wEndIndex, MGC_BsrItem *pItem);

/*
* FIFOs
*/
extern uint8_t MGC_FdrcLoadFifo(MGC_Port *pPort, uint8_t bEnd, uint32_t dwCount, const uint8_t *pSource);
extern uint8_t MGC_FdrcUnloadFifo(MGC_Port *pPort, uint8_t bEnd, uint32_t dwCount, uint8_t *pDest);

extern void MGC_FdrcDynamicFifoLocation(MGC_Port *pPort,
                                        uint8_t bEnd,
                                        uint8_t bIsTx,
                                        uint8_t bSet,
                                        uint32_t *pdwBase,
                                        uint8_t *pbSize83,
                                        uint8_t *pbDoubleBuffered);

/**
 * DMA channel status changed
 */
extern uint8_t MGC_FdrcDmaChannelStatusChanged(
    void *pPrivateData, uint8_t bLocalEnd, uint8_t bTransmit);

#if MUSB_DIAG > 0

/*
* Dump info global to the FDRC
*/
extern int MGC_FdrcDumpState(MGC_Controller *pController);

/*
* Dump info relevant to the given local endpoint resource
*/
extern int MGC_FdrcDumpEndpoint(MGC_Controller *pController, MUSB_EndpointResource *pEnd);

#endif	/* diagnostics enabled */

#endif	/* multiple inclusion protection */
