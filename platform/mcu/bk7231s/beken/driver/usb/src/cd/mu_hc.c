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

/*
 * Host controller common code
 * $Revision: 1.3 $
 */

#include "mu_impl.h"

#ifndef MUSB_BABBLE_RETRIES
#define MUSB_BABBLE_RETRIES 4
#endif

#ifdef MUSB_HOST

/*
 * Unsupported version of MUSB_RegisterHostClient for OTG controller family,
 * to make it easy to test host-only clients
 */
MUSB_BusHandle MUSB_RegisterHostClient(MUSB_Port *pPort,
                                       MUSB_HostClient *pHostClient)
{
    void *pResult = NULL;
    MGC_Port *pImplPort = (MGC_Port *)pPort->pPrivateData;

    /* session still active? */
    if(!pImplPort->bSessionActive)
    {
        /* host client just has callbacks */
        pImplPort->pHostClient = pHostClient;

        pImplPort->bVbusErrorRetries = MUSB_VBUS_ERROR_RETRIES;
        pImplPort->bBabbleRetries = MUSB_BABBLE_RETRIES;

        /* nothing active; reset port */
        pImplPort->pfResetPort(pImplPort);

        /* start */
        pImplPort->bWantSession = TRUE;
        pImplPort->pfProgramBusState(pImplPort);
        pImplPort->bOtgState = MUSB_A_WAIT_BCON;

        /* set return value */
        pResult = pImplPort;
    }

    return pResult;
}

#endif	/* MUSB_HOST */
