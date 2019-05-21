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

#ifndef	__MUSB_TRANSPORT_TEST_H__
#define	__MUSB_TRANSPORT_TEST_H__

/*
 * MUSB-MicroSW "transport" test definitions.
 * $Revision: 1.6 $
 */

#include "mu_dev.h"

#define MUSB_TRANSPORT_VID	0x04d6
#define MUSB_TRANSPORT_PID	0xffff

/* sent as vendor-specific to other recipient */
#define MUSB_TRANSPORT_REQ_CONFIGURE_END    1

/*
* Command definitions
*/

#include "mu_pkon.h"
/**
 * Command to configure endpoint(s)
 * @field bType one of the MUSB_TRANSPORT_TYPE_* constants
 * @field bOptions one of the MUSB_TRANSPORT_OPTIONS_* constants
 * @field bmAttributes as per chapter 9, protocol only
 * @field bInOutEnds IN endpoint address (0 => no IN) in upper 4 bits;
 * OUT endpoint address (0 => no OUT) in lower 4 bits
 * @field bStartPattern starting data pattern (for source only)
 * @field bPacketCount packets per transfer
 * @field wMaxPacketSize maximum packet size
 */
typedef struct
{
    uint8_t bType;
    uint8_t bOptions;
    uint8_t bmAttributes;
    uint8_t bInOutEnds;
    uint8_t bStartPattern;
    uint8_t bPacketCount;
    uint16_t wMaxPacketSize;
} MUSB_TransportConfigureEnd;
#include "mu_pkoff.h"

#define MUSB_TRANSPORT_REQ_SET_NONE		0
#define MUSB_TRANSPORT_REQ_SET_LOOPBACK		1
#define MUSB_TRANSPORT_REQ_SET_SINK		2
#define MUSB_TRANSPORT_REQ_SET_SOURCE		3

#define MUSB_TRANSPORT_OPTIONS_NONE		0
#define MUSB_TRANSPORT_OPTIONS_INCREMENT	1
#define MUSB_TRANSPORT_OPTIONS_DECREMENT	2
#define MUSB_TRANSPORT_OPTIONS_ROTATE_LEFT	3
#define MUSB_TRANSPORT_OPTIONS_ROTATE_RIGHT	4

#define MUSB_M_TRANSPORT_IN_END			0x03
#define MUSB_S_TRANSPORT_IN_END			4
#define MUSB_M_TRANSPORT_OUT_END		0x03
#define MUSB_S_TRANSPORT_OUT_END		0

#endif	/* multiple inclusion protection */
