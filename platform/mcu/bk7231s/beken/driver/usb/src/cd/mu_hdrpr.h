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

#ifndef __MUSB_HDRC_PROTO_H__
#define __MUSB_HDRC_PROTO_H__

/*
 * Prototypes for HDRC-specific functions
 * $Revision: 1.17 $
 */

#include "mu_impl.h"

#include "mu_drcpr.h"

/*
 * Initialize (discover FIFO config, etc.)
 */
extern uint8_t MGC_HdrcInit(MGC_Port *pPort);

/**
 * Try to allocate a local FIFO appropriate to the given remote endpoint (host role)
 * or to be identified with the given endpoint descriptor (function role).
 * NOTE: although this behavior is common with the FDRC,
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
extern MGC_EndpointResource *MGC_HdrcBindDynamicEndpoint(MGC_Port *pPort,
        const MUSB_DeviceEndpoint *pUsbEnd,
        MUSB_EndpointResource *pRequest,
        uint8_t bBind, uint8_t *pbIsTx);

/** HDRC ISR */
extern int MGC_HdrcIsr(void *pParam);

/*
* Program the FDRC to start/stop (enable/disable interrupts, bus reset, etc.).
* This is where registers get twiddled; no place above.
*/
extern uint32_t MGC_HdrcStart(MGC_Controller *pController);
extern uint32_t MGC_HdrcStop(MGC_Controller *pController);

extern uint32_t MGC_HdrcDestroy(MGC_Controller *pController);

/*
* Read bus state
*/
extern uint32_t MGC_HdrcReadBusState(MGC_Port *pPort);

/* Program the bus state from the port (see MGC_Port) */
extern uint32_t MGC_HdrcProgramBusState(MGC_Port *pPort);

/*
* Find the first or best (host) or required (function)
* local endpoint resource to communicate with the given remote endpoint (host)
* or to be identified with the given endpoint descriptor (function).
* This is where registers get twiddled; no place above.
*/
extern MGC_EndpointResource *MGC_HdrcBindEndpoint(MGC_Port *pPort,
        const MUSB_DeviceEndpoint *pUsbEnd,
        MUSB_EndpointResource *pRequest,
        uint8_t bBind);

/*
* Program the HDRC to initiate a transfer.
* This is where registers get twiddled; no place above.
*/
extern uint32_t MGC_HdrcStartRx(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                                uint8_t *pBuffer, uint32_t dwTotalBytes, void *pIrp, uint8_t bAllowDma);

extern uint32_t MGC_HdrcStartTx(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                                const uint8_t *pBuffer, uint32_t dwTotalBytes, void *pIrp);

/*
* Program the underlying port to flush the given endpoint resource,
* with respect to the given USB direction (IN or OUT).
* The resource should be ready for normal operation by return time.
*/
extern uint32_t MGC_HdrcFlushEndpoint(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                                      uint8_t bDir, uint8_t bReuse);

/*
* Program the HDRC's given endpoint's stall state
* (TRUE to set halt; FALSE to clear halt)
*/
extern uint32_t MGC_HdrcHaltEndpoint(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                                     uint8_t bDir, uint8_t bHalt);

extern uint32_t MGC_HdrcDefaultEndResponse(MGC_Port *pPort, uint8_t bStall);

/*
* Interrupts
*/
extern uint8_t MGC_HdrcServiceDefaultEnd(MGC_Port *pPort, MGC_BsrItem *pItem);
extern uint8_t MGC_HdrcServiceTransmitAvail(MGC_Port *pPort, uint16_t wEndIndex, MGC_BsrItem *pItem);
extern uint8_t MGC_HdrcServiceReceiveReady(MGC_Port *pPort, uint16_t wEndIndex, MGC_BsrItem *pItem);

/*
* FIFOs
*/
extern uint8_t MGC_HdrcLoadFifo(MGC_Port *pPort, uint8_t bEnd, uint32_t dwCount, const uint8_t *pSource);
extern uint8_t MGC_HdrcUnloadFifo(MGC_Port *pPort, uint8_t bEnd, uint32_t dwCount, uint8_t *pDest);

extern void MGC_HdrcSetPortTestMode(MGC_Port *pPort, MUSB_HsetPortMode eMode);

extern void MGC_HdrcDynamicFifoLocation(MGC_Port *pPort,
                                        uint8_t bEnd,
                                        uint8_t bIsTx,
                                        uint8_t bSet,
                                        uint32_t *pdwBase,
                                        uint8_t *pbSize83,
                                        uint8_t *pbDoubleBuffered);

/**
 * Set Vbus control options for ULPI PHYs.
 * @param pPort port pointer
 * @param bExtSource TRUE if the Vbus source is external; FALSE to use internal
 * @param bExtIndicator TRUE to use an external Vbus valid indicator; FALSE to use internal
 * @return TRUE on success
 * @return FALSE on error
 */
extern uint8_t MGC_HdrcUlpiVbusControl(MGC_Port *pPort, uint8_t bExtSource, uint8_t bExtIndicator);

/**
 * Read a ULPI PHY register.
 * @param pPort port pointer
 * @param bAddr ULPI register address
 * @param pbData where to store register value
 * @return TRUE on success
 * @return FALSE on error (PHY is suspended so read is not possible)
 */
extern uint8_t MGC_HdrcReadUlpiReg(MGC_Port *pPort, uint8_t bAddr, uint8_t *pbData);

/**
 * Write a ULPI PHY register.
 * @param pPort port pointer
 * @param bAddr ULPI register address
 * @param bData value to write
 * @return TRUE on success
 * @return FALSE on error (PHY is suspended so write is not possible)
 */
extern uint8_t MGC_HdrcWriteUlpiReg(MGC_Port *pPort, uint8_t bAddr, uint8_t bData);

/**
 * DMA channel status changed
 */
extern uint8_t MGC_HdrcDmaChannelStatusChanged(
    void *pPrivateData, uint8_t bLocalEnd, uint8_t bTransmit);

#if MUSB_DIAG > 0

/*
* Dump info global to the FDRC
*/
extern int MGC_HdrcDumpState(MGC_Controller *pController);

/*
* Dump info relevant to the given local endpoint resource
*/
extern int MGC_HdrcDumpEndpoint(MGC_Controller *pController, MUSB_EndpointResource *pEnd);

#endif	/* diagnostics enabled */

#endif	/* multiple inclusion protection */
