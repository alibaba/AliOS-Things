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

#ifndef __MUSB_PIPE_PROTO_H__
#define __MUSB_PIPE_PROTO_H__

/*
 * Prototypes for pipe-specific functions
 * $Revision: 1.8 $
 */

#include "mu_impl.h"

/*
* As per USB 2.0, a USB data transfer may consist of multiple USB
* transactions.  Bulk and control transfers consist of a finite
* number of transactions (they are "one-time" in nature),
* while interrupt and isochronous transfers are expected
* to continue for indefinite periods (numbers of USB transactions).
*/

/**
 * New packet received.
 * Implement protocol rules for a received packet.
 * This is called by controller-specific interrupt-processing.
 * @param pPort port pointer
 * @param status status code
 * @param pEnd local endpoint pointer
 * @param wPacketSize size of received packet
 * @param bContinue TRUE to continue transfer if needed;
 * FALSE if not
 * @param pIrp pointer to associated IRP
 * @return TRUE if the transfer is deemed complete
 */
extern uint8_t MGC_PipePacketReceived(MGC_Port *pPort,
                                      uint32_t status,
                                      MGC_EndpointResource *pEnd,
                                      uint16_t wPacketSize,
                                      uint8_t bContinue, void *pIrp);

/**
 * Transmit buffer ready.
 * Implement protocol rules for a transmit buffer-ready condition
 * (possibly begin transmission of next packet).
 * This is called by controller-specific interrupt-processing.
 * @param pPort port pointer
 * @param pEnd local endpoint pointer
 * @return TRUE if the transfer is deemed complete
 */
extern uint8_t MGC_PipeTransmitReady(MGC_Port *pPort,
                                     MGC_EndpointResource *pEnd);

/**
 * Complete an IRP.
 * This is called from interrupt-handling code,
 * so it actually just fills a BSR item to be queued.
 * @param pItem pointer to a BSR item
 * @param pEnd local endpoint resource pointer
 * @param bStatus status code
 * @param bIsTx TRUE for Tx IRP; FALSE for Rx IRP
 * @param pCurrentIrp associated IRP pointer
 * @return TRUE to queue the BSR item; FALSE otherwise
 */
extern uint8_t MGC_CompleteIrp(MGC_BsrItem *pItem, MGC_EndpointResource *pEnd,
                               uint8_t bStatus, uint8_t bIsTx, void *pCurrentIrp);

/**
 * Start the next IRP, if any, queued for the given endpoint.
 * @param pPort port pointer
 * @param pEnd local endpoint resource pointer
 * @param bIsTx TRUE for Tx IRP; FALSE for Rx IRP
 * @return TRUE if an IRP has been started
 * @return FALSE if no IRPs were queued
 */
extern uint8_t MGC_StartNextIrp(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                                uint8_t bIsTx);

#endif	/* multiple inclusion protection */
