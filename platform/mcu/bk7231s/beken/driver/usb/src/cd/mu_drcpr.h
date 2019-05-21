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
 * Dual-Role Controller (DRC) function prototypes.
 * $Revision: 1.11 $
 */

#ifndef __MUSB_DRC_PROTO_H__
#define __MUSB_DRC_PROTO_H__

/*
* Prototypes for DRC-specific functions (at least FDRC/HDRC)
*/

#include "mu_impl.h"

/*
 * Initialize (discover FIFO config, etc.)
 */
extern uint8_t MGC_DrcInit(MGC_Port *pPort);

/*
 * ISR
 */
extern int MGC_DrcIsr(MGC_Controller *pController,
                      uint8_t bIntrUsbValue,
                      uint16_t wIntrTxValue,
                      uint16_t wIntrRxValue);

/**
 * Common USB ISR.
 * Handles common USB interrupts on DRCs (from IntrUSB)
 * @param pPort port poiner
 * @param bReg IntrUSB value
 * @return number of IST work items queued
 */
extern uint8_t MGC_DrcUsbIsr(MGC_Port *pPort, uint8_t bReg);

/**
 * Service default endpoint
 * @param pPort port pointer
 * @param pItem work queue item
 * @param pwCsr CSR0 value (in); what to write to CSR0 (out)
 * @param wCount COUNT0 value
 * @return TRUE if work was queued
 */
extern uint8_t MGC_DrcServiceDefaultEnd(MGC_Port *pPort, MGC_BsrItem *pItem,
                                        uint16_t *pwCsr, uint16_t wCount);

/**
 * the non-interrupt-time processing for a DRC
 * @memo the DRC BSR
 * @param pParam thread parameter
 */
extern uint32_t MGC_DrcBsr(void *pParam);

/* Reset the port (e.g. unbind/flush all endpoints, etc.) */
extern uint32_t MGC_DrcResetPort(MGC_Port *pPort);

/**
 * Change the DRC to given OTG state
 * @memo change OTG state
 * @param toOtg the new state
 * @param pPort port pointer
 */
extern void MGC_DrcChangeOtgState(uint8_t toOtg, MGC_Port *pPort);

/**
 * Complete an OTG state transition
 * (allow suspend in peripheral mode and notify client)
 */
extern void MGC_CompleteOtgTransition(MGC_Port *pPort);

/**
 * Finish resume operations
 */
extern void MGC_DrcFinishResume(MGC_Port *pPort);

/**
 * Called upon OTG timer expiration
 * @memo OTG timer callback
 * @param pParam callback parameter
 * @param wTimer timer index
 */
extern void MGC_DrcOtgTimer(void *pParam);

/**
 * Find the DRC's first (host role) or required (function role)
 * local endpoint resource to communicate with the given remote endpoint (host)
 * or to be identified with the given endpoint descriptor (function).
 * @memo bind a DRC endpoint
 * @param pPort port pointer
 * @param pUsbEnd device endpoint attributes pointer
 * @param pRequest requested local endpoint attributes
 * @param bBind TRUE to actually bind (reserve resources)
 * @param pbIsTx pointer to storage to indicate Tx (TRUE) or Rx (FALSE)
 * @return pointer on success
 * @return NULL on failure
 */
extern MGC_EndpointResource *MGC_DrcBindEndpoint(MGC_Port *pPort,
        const MUSB_DeviceEndpoint *pUsbEnd,
        MUSB_EndpointResource *pRequest,
        uint8_t bBind, uint8_t *pbIsTx);

/**
 * Service the default endpoint as host.
 * @memo service default endpoint (host)
 * @param pPort port pointer
 * @param wCount current byte count
 * @param status status code indicating errors, if any
 * @param pbCsrVal where to store new CSR0 value (only non-zero values written)
 * @return TRUE if more packets are required for this transaction;
 * FALSE otherwise
 */
extern uint8_t MGC_DrcServiceHostDefault(MGC_Port *pPort, uint16_t wCount,
        uint8_t status, uint8_t *pbCsrVal);

/**
 * Service the default endpoint as function.
 * @memo service default endpoint (function)
 * @param pPort port pointer
 * @param wCount current byte count
 * @param pStatus pointer to status code indicating errors, filled with
 * a successful status or STALL
 * @param pbPhase pointer to the source of the current interrupt (IN vs. OUT),
 * filled with direction for next phase (IN vs. OUT)
 * @return TRUE if more packets are required for this transaction;
 * FALSE otherwise
 */
extern uint8_t MGC_DrcServiceFunctionDefault(MGC_Port *pPort, uint16_t wCount,
        uint8_t *pStatus, uint8_t *pbPhase);

/**
 * Accept/reject device.
 * Check if device is acceptable; take appropriate action if not.
 * The enumerator calls this after gathering all configuration data
 * and attempting a driver match.
 * @param pPort port pointer
 * @param pDevice device pointer
 * @param pDriver pointer to driver that matched, or NULL if none
 * @return TRUE if device is acceptable
 * @return FALSE if not (and appropriate action taken)
 */
extern uint8_t MGC_DrcAcceptDevice(MGC_Port *pPort, MUSB_Device *pDevice,
                                   MUSB_DeviceDriver *pDriver);

/**
 * A DMA channel's status has changed
 */
extern uint8_t MGC_DrcDmaChannelStatusChanged(
    void *pPrivateData, uint8_t bLocalEnd, uint8_t bTransmit);

/**
 * Set a DRC's host-mode power delivery capability
 */
extern uint32_t MGC_DrcSetHostPower(MGC_Controller *pController,
                                    uint16_t wPower);

#if MUSB_DIAG > 0
/*
* Dump info relevant to the given pipe
*/
extern int MGC_DrcDumpPipe(MGC_Controller *pController, MGC_Pipe *pPipe);
#endif

#endif	/* multiple inclusion protection */
