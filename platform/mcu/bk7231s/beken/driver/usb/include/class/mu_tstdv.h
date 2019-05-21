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
 * MUSB-MicroSW compliance test device definitions.
 * $Revision: 1.5 $
 */

#ifndef	__MUSB_COMPLIANCE_DEVICE_H__
#define	__MUSB_COMPLIANCE_DEVICE_H__

#include "mu_tools.h"

/*
* Definitions for USB compliance test device
*/

/** subclass for programmable compliance devices */
#define MUSB_SUBCLASS_COMPLIANCE	0x01

/*
* Command definitions
*/

/** change endpoint characteristics */
#define MUSB_DIAG_REQ_VEN_CHANGE_EP		0x51

/**
 * (optional) disconnect:
 * wValue.MSB=seconds before disconnect (1-60);
 * wValue.LSB=seconds between disconnect/re-connect (1-60)
 */
#define MUSB_DIAG_REQ_VEN_DO_DISCONNECT		0x52

/**
 * get entire endpoint capabilities table
 * (first byte returned is length of table)
 */
#define MUSB_DIAG_REQ_VEN_GET_EP_TABLE		0x53

/** (optional) set device PID */
#define MUSB_DIAG_REQ_VEN_SET_PID		0x54

/** remote wakeup (wValue=seconds from suspend to remote wakeup) */
#define MUSB_DIAG_REQ_VEN_SET_RMTWK_TO		0x55

/** read endpoint-specific count (4 bytes) */
#define MUSB_DIAG_REQ_VEN_GET_COUNT		0x58

/** read firmware/FPGA version (4 bytes) */
#define MUSB_DIAG_REQ_VEN_GET_VERSION		0x59

/*
 * (high-speed only; optional) set chirp behavior:
 * wValue=chirp delay, microsecs [1-6000]
 * wIndex=chirp duration, microsecs [500-8000]
 */
#define MUSB_DIAG_REQ_VEN_SET_CHIRP		0x5A

/** write arbitrary data on control pipe */
#define MUSB_DIAG_REQ_VEN_CONROL_WRITE_DATA	0x5B

/** read last data written on control pipe */
#define MUSB_DIAG_REQ_VEN_CONROL_READ_DATA	0x5C

#define MUSB_DIAG_REQ_VEN_GET_EP_BUFFERSIZE	0x5D

/** valid */
#define MUSB_S_EPCHANGE_BFLAGS_VALID			0
/** do loopback */
#define MUSB_S_EPCHANGE_BFLAGS_LOOPBACK			1
/** endpoint type */
#define MUSB_S_EPCHANGE_BFLAGS_TYPE			2
/** number of additional transaction opportunities per micro-frame */
#define MUSB_S_EPCHANGE_BFLAGS_EXTRATRANSACTIONS	4

/** normal response mode */
#define MUSB_DIAG_RESPONSE_NORMAL   0
/** always STALL */
#define MUSB_DIAG_RESPONSE_STALL    1
/** always NAK */
#define MUSB_DIAG_RESPONSE_NAK	    2
/** always time out */
#define MUSB_DIAG_RESPONSE_TIMEOUT  4
/** always CRC error */
#define MUSB_DIAG_RESPONSE_CRCERROR 8

#define MUSB_S_DIAG_ATTIBUTES_HIGHBANDWIDTH	0
#define MUSB_S_DIAG_ATTIBUTES_LOOPBACK		1
#define MUSB_S_DIAG_ATTIBUTES_PERFECTSOURCESINK	2
#define MUSB_S_DIAG_ATTIBUTES_TRANSACTIONCOUNT	5

#include "mu_pkon.h"

/**
 * MUSB_EndpointChangeData (for MGC_USB_REQ_VEN_CHANGE_EP).
 * @field bFlags flags; see MUSB_S_EPCHANGE_FLAGS_*
 * @field r0 reserved for future use
 * @field wm_MaxPacketSize the max, modifiable
 * @field dm_Pattern base data pattern by device
 * @field dm_Modifer data pattern modifier
 * @field bPeriod for interrupt, the poll interval
 * @field bFixedRespMode fixed response mode, see MUSB_DIAG_RESPONSE_*
 * @field r1 reserved for future use
 */
typedef struct
{
    uint8_t bFlags;
    uint8_t r0;
    uint16_t wm_MaxPacketSize;
    uint32_t dm_Pattern;
    uint32_t dm_Modifer;
    uint8_t bPeriod;
    uint8_t bFixedRespMode;
    uint16_t r1;
} MUSB_DiagEndpointChangeData;
#include "mu_pkoff.h"

#include "mu_pkon.h"
/**
 * Transfer type capability
 * @field bValid TRUE if transfer type is supported
 * @field bMaxExtraBuffer max for this transfer type
 * @field wMaxPacketSize max for this transfer type
 */
typedef struct
{
    uint8_t bValid;
    uint8_t bMaxExtraBuffer;
    uint16_t wMaxPacketSize;
} MUSB_DiagTransferTypeCapability;
#include "mu_pkoff.h"

#include "mu_pkon.h"
/**
 * Diagnostic device capabilities
 * @field bAddress endpoint address ([3:0]) and direction ([7])
 * @field bLoopbackIndex index of loopback partner
 * @field bmAttributes see MUSB_S_DIAG_ATTIBUTES_*
 * @field bFixedRespCaps for setting fixed response mode
 * @field BulkCapabilities capabilities as a bulk endpoint
 * @field InterruptCapabilities capabilities as an interrupt endpoint
 * @field IsochCapabilities capabilities as an isochronous endpoint
 * @field ControlCapabilities capabilities as a control endpoint
 */
typedef struct
{
    uint8_t bAddress;
    uint8_t bLoopbackIndex;
    uint8_t bmAttributes;
    uint8_t bFixedRespCaps;
    MUSB_DiagTransferTypeCapability BulkCapabilities;
    MUSB_DiagTransferTypeCapability InterruptCapabilities;
    MUSB_DiagTransferTypeCapability IsochCapabilities;
    MUSB_DiagTransferTypeCapability ControlCapabilities;
} MUSB_DiagCapabilities;
#include "mu_pkoff.h"

#include "mu_pkon.h"
/**
 * Diagnostic device capabilities
 * @field bLength number of table entries
 * @field Table variable-length table
 */
typedef struct
{
    uint8_t bLength;
    MUSB_DiagCapabilities Table[1];
} MUSB_DiagCapabilityTable;

#include "mu_pkoff.h"

#endif	/* multiple inclusion protection */
