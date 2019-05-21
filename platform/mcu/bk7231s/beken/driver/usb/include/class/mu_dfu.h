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
 * MUSB-MicroSW DFU (Device Firmware Upgrade) definitions.
 * $Revision: 1.1 $
 */

#ifndef __MUSB_DFU_H__
#define __MUSB_DFU_H__

#include "mu_tools.h"

/*
 * subclass/protocol (class=MUSB_CLASS_APP_SPEC) -
 * in normal mode, used in interface; in DFU mode, used in device
 */
#define MGC_DFU_SUBCLASS          0x01
#define MGC_DFU_PROTOCOL          0x00

/* type of functional descriptor */
#define MGC_DFU_FUNCTIONAL_DESCTYPE 0x21

/* bmRequestType values */
#define MGC_DFU_REQTYPE_OUT       (MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)
#define MGC_DFU_REQTYPE_IN        (MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE)

/* requests (bRequest values) */
#define MGC_DFU_REQUEST_DETACH    0 /* wValue=wTimeout, wIndex=interface */
/* download data in normal control transfers using wLength, send zero-length at end of file */
#define MGC_DFU_REQUEST_DNLOAD    1 /* wValue=wBlockNum, wIndex=interface */
#define MGC_DFU_REQUEST_UPLOAD    2 /* wIndex=interface */
#define MGC_DFU_REQUEST_GETSTATUS 3 /* wIndex=interface, wLength=6 */
#define MGC_DFU_REQUEST_CLRSTATUS 4 /* wIndex=interface */
#define MGC_DFU_REQUEST_GETSTATE  5 /* wIndex=interface, wLength=1 */
#define MGC_DFU_REQUEST_ABORT     6 /* wIndex=interface */

/**
 * MGC_DfuFunctionalDescriptor.
 * DFU Functional descriptor.
 * @field bLength descriptor length
 * @field bDescriptorType MGC_DFU_FUNCTIONAL_DESCTYPE
 * @field bmAttributes see MGC_S_DFU_ATTR_*
 * @field wDetachTimeout timeout, in ms, between DFU_DETACH request
 * and USB reset, before device terminates reconfiguration and
 * reverts to normal operation
 * @field wTransferSize the maximum number of bytes the device can accept
 * per control write
 */
#include "mu_pkon.h"
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bmAttributes;
    uint16_t wDetachTimeout;
    uint16_t wTransferSize;
} MGC_DfuFunctionalDescriptor;
#include "mu_pkoff.h"

/* bmAttributes: */
/** can download */
#define MGC_S_DFU_ATTR_CAN_DNLOAD             0
/** can upload */
#define MGC_S_DFU_ATTR_CAN_UPLOAD             1
/** can communicate over USB after manifestation (otherwise needs reset) */
#define MGC_S_DFU_ATTR_MANIFESTATION_TOLERANT 2

/**
 * MGC_DfuStatus.
 * Status from device.
 * @field bStatus see MGC_DFU_STATUS_*
 * @field bwPollTimeout min time, in ms, before host should request status again
 * @field bState see MGC_DFU_STATE_*
 * @field iString index for string descriptor for vendor-defined status
 */
#include "mu_pkon.h"
typedef struct
{
    uint8_t bStatus;
    uint8_t bwPollTimeout[3];
    uint8_t bState;
    uint8_t iString;
} MGC_DfuStatus;
#include "mu_pkoff.h"

/* bStatus: */
#define MGC_DFU_STATUS_OK               0x00
/** file is not targeted for device */
#define MGC_DFU_STATUS_ERR_TARGET       0x01
/** file failed verification */
#define MGC_DFU_STATUS_ERR_FILE         0x02
/** unable to write memory */
#define MGC_DFU_STATUS_ERR_WRITE        0x03
/** unable to erase memory */
#define MGC_DFU_STATUS_ERR_ERASE        0x04
/** erase check failed */
#define MGC_DFU_STATUS_ERR_CHECK_ERASED 0x05
/** program memory failed */
#define MGC_DFU_STATUS_ERR_PROG         0x06
/** memory verify failed */
#define MGC_DFU_STATUS_ERR_VERIFY       0x07
/** address out of range for programming */
#define MGC_DFU_STATUS_ERR_ADDRESS      0x08
/** received DFU_DNLOAD wLength=0, but device thinks it should not be end-of-file */
#define MGC_DFU_STATUS_ERR_NOT_DONE     0x09
/** device firmware is corrupt; cannot resume normal operation */
#define MGC_DFU_STATUS_ERR_FIRMWARE     0x0a
/** iString indicates vendor-specific error */
#define MGC_DFU_STATUS_ERR_VENDOR       0x0b
/** device detected unexpected USB reset */
#define MGC_DFU_STATUS_ERR_USB_RESET    0x0c
/** device detected unexpected power-on reset */
#define MGC_DFU_STATUS_ERR_POR          0x0d
#define MGC_DFU_STATUS_ERR_UNKNOWN      0x0e
/** device stalled an unexpected request */
#define MGC_DFU_STATUS_ERR_STALLED_PKT  0x0f

/* bState: */
#define MGC_DFU_STATE_APP_IDLE                0
#define MGC_DFU_STATE_APP_DETACH              1
#define MGC_DFU_STATE_DFU_IDLE                2
/** device got DNLOAD, expecting GET_STATUS */
#define MGC_DFU_STATE_DFU_DNLOAD_SYNC         3
#define MGC_DFU_STATE_DFU_DNBUSY              4
/** device expecting DNLOAD requests */
#define MGC_DFU_STATE_DFU_DNLOAD_IDLE         5
/** device got last DNLOAD; awaiting GET_STATUS for manifestation start or progress */
#define MGC_DFU_STATE_DFU_MANIFEST_SYNC       6
#define MGC_DFU_STATE_DFU_MANIFEST            7
/** device programmed and awaiting reset */
#define MGC_DFU_STATE_DFU_MANIFEST_WAIT_RESET 8
/** expecting UPLOAD request */
#define MGC_DFU_STATE_DFU_UPLOAD_IDLE         9
/** error; awaiting CLR_STATUS */
#define MGC_DFU_STATE_DFU_ERROR               10

/**
 * MGC_DfuFileSuffix.
 * Required suffix for downloadable files.
 * @field bcdDevice for matching to device revision or 0xffff for any
 * @field idProduct for matching to device product or 0xffff for any
 * @field idVendor for matching to device vendor or 0xffff for any
 * @field bcdDFU DFU class revision (0x0100 for 1.0)
 * @field ucDfuSignature 0x55, 0x46, 0x44
 * @field bLength length of suffix (16)
 * @field dwCRC CRC including entire file and suffix except CRC itself
 */
#include "mu_pkon.h"
typedef struct
{
    uint16_t bcdDevice;
    uint16_t idProduct;
    uint16_t idVendor;
    uint16_t bcdDFU;
    uint8_t ucDfuSignatue[3];
    uint8_t bLength;
    uint32_t dwCRC;
} MGC_DfuFileSuffix;
#include "mu_pkoff.h"

#endif	/* multiple inclusion protection */
