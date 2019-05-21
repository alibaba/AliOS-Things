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

#ifndef __MUSB_HOST_PROTO_H__
#define __MUSB_HOST_PROTO_H__

/*
 * Prototypes for host-specific functions
 * $Revision: 1.8 $
 */

#include "mu_impl.h"

/**
 * Allocate a device address.
 * @param pEnumData pointer to persistent per-port enumeration data
 * @return a non-zero address on success
 * @return 0 on error (no more addresses available)
 */
extern uint8_t MGC_AllocateAddress(MGC_EnumerationData *pEnumData);

/**
 * Release a device address.
 * @param pEnumData pointer to persistent per-port enumeration data
 * @param bAddress device address; must not be used after this call
 */
extern void MGC_ReleaseAddress(MGC_EnumerationData *pEnumData, uint8_t bAddress);

/**
 * Begin enumeration of a device.
 * @param pPort port pointer
 * @param pHubDevice the device's parent hub, or NULL if none
 * @param bAddress the address to assign the device
 * @param bHubPort the port to which the device is connected on its parent hub;
 * ignored if no parent hub
 * @param bSpeed the device's connection speed (CONNECTION_SPEED_* constants)
 * @param pfHubEnumerationComplete completion function, or NULL if no notification needed
 */
extern uint8_t MGC_EnumerateDevice(MGC_Port *pPort, MUSB_Device *pHubDevice,
                                   uint8_t bAddress, uint8_t bHubPort, uint8_t bSpeed,
                                   MUSB_pfHubEnumerationComplete pfHubEnumerationComplete);

/**
 * Perform host-specific cleanup
 * @param pPort port pointer
 */
extern uint8_t MGC_HostDestroy(MGC_Port *pPort);

/**
 * Start the next control transfer, if any.
 * @param pPort port pointer
 * @return TRUE if a new transfer is started
 * @return FALSE if no pending IRPs to start
 */
extern uint8_t MGC_StartNextControlTransfer(MGC_Port *pPort);

/**
 * Run scheduled transfers.
 * @param pPort port pointer
 * @return TRUE on success
 * @return FALSE on failure
 */
extern uint8_t MGC_RunScheduledTransfers(MGC_Port *pPort);

/**
 * Set maximum power capability
 * @param pPort port pointer
 * @param wPower maximum deliverable current,
 * in units of 2 mA
 */
extern uint32_t MGC_HostSetMaxPower(MGC_Port *pPort,
                                    uint16_t wPower);

#endif	/* multiple inclusion protection */
