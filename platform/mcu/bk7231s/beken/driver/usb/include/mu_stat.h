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
 * MUSB-MicroSW status codes.
 * $Revision: 1.4 $
 */

#ifndef __MUSB_STATUS_H__
#define __MUSB_STATUS_H__

/*
 * Status codes (bit 7 means error)
 */
#define MGC_M_STATUS_ERROR		    0x80
#define MGC_M_STATUS_WARNING		0x40

/*
 * Macros to test status type
 */

/**
 * Test code for error.
 * @param _code_ status code
 * @return TRUE if code represents an error; FALSE otherwise
 */
#define MUSB_STATUS_ISERROR(_code_) (_code_ & MGC_M_STATUS_ERROR)

/**
 * Test code for warning.
 * @param _code_ status code
 * @return TRUE if code represents a warning; FALSE otherwise
 */
#define MUSB_STATUS_ISWARNING(_code_) (_code_ & MGC_M_STATUS_WARNING)

/**
 * Test code for success.
 * @param _code_ status code
 * @return TRUE if code represents success; FALSE otherwise
 */
#define MUSB_STATUS_ISOK(_code_) (0 == (_code_ & (MGC_M_STATUS_ERROR | MGC_M_STATUS_WARNING)))

/** Successful status */
#define MUSB_STATUS_NOERROR		0

/** Successful status */
#define MUSB_STATUS_SUCCESS		0

/** Successful status */
#define MUSB_STATUS_OK			0

/** IRP was started, but is not complete (pending) */
#define MUSB_STATUS_IRP_PENDING		0x03

/** One or more devices were suspended or unconfigured to save power */
#define MUSB_STATUS_POWER_LOW		0x04

/** warning: device not supported (OTG targetted peripheral list) */
#define MUSB_STATUS_UNSUPPORTED_DEVICE	(MGC_M_STATUS_WARNING | 0x01)

/** warning: hub not supported (OTG targetted peripheral list) */
#define MUSB_STATUS_UNSUPPORTED_HUB	(MGC_M_STATUS_WARNING | 0x02)

/** error: received USB stall handshake */
#define MUSB_STATUS_STALLED		(MGC_M_STATUS_ERROR | 0x00)

/** error: IRP timeout */
#define MUSB_STATUS_TIMEOUT		(MGC_M_STATUS_ERROR | 0x01)

/** error: NACK limit reached */
#define MUSB_STATUS_NACK_LIMIT		(MGC_M_STATUS_ERROR | 0x02)

/** error: ACK timeout */
#define MUSB_STATUS_ACK_TIMEOUT		(MGC_M_STATUS_ERROR | 0x03)

/** error: unsupported operation requested */
#define MUSB_STATUS_UNSUPPORTED		(MGC_M_STATUS_ERROR | 0x04)

/** error: the requested endpoint is busy (e.g. pending IRP prevents starting request) */
#define MUSB_STATUS_ENDPOINT_BUSY	(MGC_M_STATUS_ERROR | 0x05)

/** error: short transfer */
#define MUSB_STATUS_SHORT_TRANSFER	(MGC_M_STATUS_ERROR | 0x06)

/** error: internal error */
#define MUSB_STATUS_INTERNAL_ERROR	(MGC_M_STATUS_ERROR | 0x07)

/** error: general transmit error */
#define MUSB_STATUS_TRANSMIT_ERROR	(MGC_M_STATUS_ERROR | 0x08)

/** error: general receive error */
#define MUSB_STATUS_RECEIVE_ERROR	(MGC_M_STATUS_ERROR | 0x09)

/** error: CRC error */
#define MUSB_STATUS_CRC_ERROR		(MGC_M_STATUS_ERROR | 0x10)
/** error: bit-stuffing error */
#define MUSB_STATUS_BITSTUFF_ERROR	(MGC_M_STATUS_ERROR | 0x11)
/** error: data overrun */
#define MUSB_STATUS_DATA_OVERRUN	(MGC_M_STATUS_ERROR | 0x12)
/** error: data underrun */
#define MUSB_STATUS_DATA_UNDERRUN	(MGC_M_STATUS_ERROR | 0x13)
/** error: buffer overrun */
#define MUSB_STATUS_BUFFER_OVERRUN	(MGC_M_STATUS_ERROR | 0x14)
/** error: buffer underrun */
#define MUSB_STATUS_BUFFER_UNDERRUN	(MGC_M_STATUS_ERROR | 0x15)
/** error: DMA error */
#define MUSB_STATUS_DMA_ERROR		(MGC_M_STATUS_ERROR | 0x16)

/** error: transfer aborted because peer was removed from bus */
#define MUSB_STATUS_PEER_REMOVED	(MGC_M_STATUS_ERROR | 0x20)
/** error: babble detected */
#define MUSB_STATUS_BABBLING_HOST	(MGC_M_STATUS_ERROR | 0x21)
/** error: out of bandwidth */
#define MUSB_STATUS_NO_BANDWIDTH	(MGC_M_STATUS_ERROR | 0x23)
/** error: bad traffic type */
#define MUSB_STATUS_INVALID_TYPE	(MGC_M_STATUS_ERROR | 0x24)
/** error: out of resources (usually local ends) */
#define MUSB_STATUS_NO_RESOURCES	(MGC_M_STATUS_ERROR | 0x25)
/** error: out of memory */
#define MUSB_STATUS_NO_MEMORY		(MGC_M_STATUS_ERROR | 0x26)
/** error: inconsistency in descriptors */
#define MUSB_STATUS_BAD_DESCRIPTORS	(MGC_M_STATUS_ERROR | 0x27)
/** error: invalid argument */
#define MUSB_STATUS_INVALID_ARGUMENT	(MGC_M_STATUS_ERROR | 0x28)

/*
 *	Added for OTG
 */

/** error: invalid Vbus detected */
#define MUSB_STATUS_OTG_VBUS_INVALID	(MGC_M_STATUS_ERROR | 0x30)

/** Device did not respond (OTG requires this be signalled to user */
#define MUSB_STATUS_OTG_NO_RESPONSE	(MGC_M_STATUS_ERROR | 0x31)

/** SRP failed (OTG requires this be signalled to user */
#define MUSB_STATUS_OTG_SRP_FAILED	(MGC_M_STATUS_ERROR | 0x32)

#endif	/* multiple inclusion protection */
