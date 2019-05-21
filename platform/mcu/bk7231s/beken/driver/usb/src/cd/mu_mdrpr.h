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

#ifndef __MUSB_MHDRC_PROTO_H__
#define __MUSB_MHDRC_PROTO_H__

/*
 * Prototypes for MHDRC-specific functions
 * $Revision: 1.12 $
 */

#include "mu_impl.h"

#include "mu_drcpr.h"

/*
 * Initialize (discover FIFO config, etc.)
 */
extern uint8_t MGC_MhdrcInit(MGC_Port *pPort);

/*
* Find the first or best (host) or required (function)
* local endpoint resource to communicate with the given remote endpoint (host)
* or to be identified with the given endpoint descriptor (function).
* This is where registers get twiddled; no place above.
*/
extern MGC_EndpointResource *MGC_MhdrcBindEndpoint(MGC_Port *pPort,
        const MUSB_DeviceEndpoint *pUsbEnd,
        MUSB_EndpointResource *pRequest,
        uint8_t bBind);

extern uint32_t MGC_MhdrcStartRx(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                                 uint8_t *pBuffer, uint32_t dwTotalBytes, void *pIrp, uint8_t bAllowDma);

extern uint32_t MGC_MhdrcStartTx(MGC_Port *pPort, MGC_EndpointResource *pEnd,
                                 const uint8_t *pBuffer, uint32_t dwTotalBytes, void *pIrp);

#if MUSB_DIAG > 0

/*
* Dump info global to the MHDRC
*/
extern int MGC_MhdrcDumpState(MGC_Controller *pController);

/*
* Dump info relevant to the given local endpoint resource
*/
extern int MGC_MhdrcDumpEndpoint(MGC_Controller *pController, MUSB_EndpointResource *pEnd);

#endif	/* diagnostics enabled */

#endif	/* multiple inclusion protection */
