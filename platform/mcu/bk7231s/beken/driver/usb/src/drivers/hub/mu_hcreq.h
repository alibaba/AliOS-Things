/*****************************************************************************
 *                                                                           *
 *      Copyright Mentor Graphics Corporation 2003-2005                      *
 *                                                                           *
 *                All Rights Reserved.                                       *
 *                                                                           *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION            *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS              *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                               *
 *                                                                           *
 ****************************************************************************/

/*
 * Hub-class-specific request definitions
 * $Revision: 1.4 $
 */

#ifndef _MU_HCREQ_H_
#define _MU_HCREQ_H_

#include "mu_dev.h"
#include "mu_hcore.h"

/*   Different HUB class "Status" request. */
#define MUSB_HubGetHubStatus(pHubDevice, complete) \
        MUSB_HubGetStatus (pHubDevice,  complete, (uint8_t)0x00, \
                           (MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_DEVICE))

#define MUSB_HubGetPortStatus(pHubDevice, complete, bPortNum) \
        MUSB_HubGetStatus (pHubDevice, complete, bPortNum, \
                           (MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_OTHER))


/*   Different HUB class "Feature" request. */
#define MUSB_HubClearHubFeature(pHubDevice, wFeatureSelector, complete) \
        MUSB_HubFeatureRequest (pHubDevice, wFeatureSelector,  complete, (uint8_t) 0x00,\
           (MUSB_TYPE_CLASS | MUSB_RECIP_DEVICE), MUSB_REQ_CLEAR_FEATURE)

#define MUSB_HubSetHubFeature(pHubDevice, wFeatureSelector, complete) \
        MUSB_HubFeatureRequest (pHubDevice, wFeatureSelector,  complete, (uint8_t)0x00,\
            (MUSB_TYPE_CLASS | MUSB_RECIP_DEVICE), MUSB_REQ_SET_FEATURE)

#define MUSB_HubClearPortFeature(pHubDevice, wFeatureSelector, complete, bPortNum) \
        MUSB_HubFeatureRequest (pHubDevice, wFeatureSelector,  complete, bPortNum,\
            (MUSB_TYPE_CLASS | MUSB_RECIP_OTHER), MUSB_REQ_CLEAR_FEATURE)

#define MUSB_HubSetPortFeature(pHubDevice, wFeatureSelector, complete, bPortNum) \
        MUSB_HubFeatureRequest (pHubDevice, wFeatureSelector, complete, bPortNum,\
            (MUSB_TYPE_CLASS | MUSB_RECIP_OTHER), MUSB_REQ_SET_FEATURE)


/**
 * @doc This function is used to get status of hub as well as status of port.
 * @param pHubDevice Pointer to hub structure.
 * @param complete Callback function for the request and it is given by caller.
 * @param bPortNum Port number at which request is going to sent. If this request
 *                 is ment for Hub then bPortNum should be 0x00.
 * @param bmRequest As per USB specification 2.0 section 11.24.2.
 * @return MUSB_SUCCESS when control transaction is started properly otherwise
 *         MUSB_FAILURE.
 */
extern uint32_t MUSB_HubGetStatus (MUSB_Hub *pHubDevice,
                                   MUSB_pfControlIrpComplete complete,
                                   uint8_t bPortNum, uint8_t bmRequest);

/**
 * @doc This function is used to get status of hub as well as status of port.
 * @param pHubDevice Pointer to hub structure.
 * @param wFeatureSelector As per section table 11-17 in USB 2.0 specification.
 * @param complete Callback function for the request and it is given by caller.
 * @param bPortNum Port number at which request is going to sent. If this request
 *                 is ment for Hub then bPortNum should be 0x00.
 * @param bmRequest As per USB specification 2.0 section 11.24.2.
 * @param bRequest  As per USB specification 2.0 section 11.24.2.
 * @return MUSB_SUCCESS when control transaction is started properly otherwise
 *         MUSB_FAILURE.
 */
extern uint32_t MUSB_HubFeatureRequest (MUSB_Hub *pHubDevice,
                                        uint16_t wFeatureSelector,
                                        MUSB_pfControlIrpComplete complete,
                                        uint8_t bPortNum,
                                        uint8_t bmRequest,
                                        uint8_t bRequest);

#endif


