/*****************************************************************************
 *                                                                           *
 *      Copyright Mentor Graphics Corporation 2006                           *
 *                                                                           *
 *                All Rights Reserved.                                       *
 *                                                                           *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION            *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS              *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                               *
 *                                                                           *
 ****************************************************************************/

/*
 * MUSB-MicroSW audio class definitions.
 * $Revision: 1.2 $
 */

#ifndef __MUSB_AUD_H__
#define __MUSB_AUD_H__

#include "mu_tools.h"

#define MUSB_DT_ENDPOINT_AUDIO_SIZE	9	/* Audio extension */

/* Audio synch endpoint descriptor (Audio 1.0) */
#include "mu_pkon.h"
typedef	struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
    uint8_t bRefresh;
    uint8_t bSynchAddress;
} MUSB_AudioSyncEndpointDescriptor;
#include "mu_pkoff.h"

#endif	/* multiple inclusion protection */
