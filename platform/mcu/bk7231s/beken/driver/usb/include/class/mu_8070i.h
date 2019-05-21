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
 * MUSB-MicroSW SFF-8070i definitions.
 * $Revision: 1.3 $
 */

#ifndef __MUSB_MSD_SFF8070I_H__
#define __MUSB_MSD_SFF8070I_H__

#include "mu_tools.h"

/** Maximum Length of command */
#define MGC_SFF8070I_MAX_CMD_LENGTH             12

/* Commands */
#define MGC_SFF8070I_FORMAT_UNIT        0x04
#define MGC_SFF8070I_INQUIRY            0x12
#define MGC_SFF8070I_MODE_SELECT        0x55
#define MGC_SFF8070I_MODE_SENSE         0x5A
#define MGC_SFF8070I_PREVENT_ALLOW_MED_REMOVE   0x1e
#define MGC_SFF8070I_READ10             0x28
#define MGC_SFF8070I_READ12             0xA8
#define MGC_SFF8070I_READ_CAPACITY      0x25
#define MGC_SFF8070I_RD_FMT_CAPC        0x23
#define MGC_SFF8070I_REQUEST_SENSE      0x03
#define MGC_SFF8070I_SEEK               0x2B
#define MGC_SFF8070I_START_STOP_UNIT    0x1B
#define MGC_SFF8070I_TEST_UNIT_READY    0x00
#define MGC_SFF8070I_VERIFY             0x2F
#define MGC_SFF8070I_WRITE10            0x2A
#define MGC_SFF8070I_WRITE12            0xAA
#define MGC_SFF8070I_WRITE_VERIFY       0x2E

/* Mode pages */
#define MGC_SFF8070I_PAGE_ERR_RECOV_CTL	0x01
#define MGC_SFF8070I_PAGE_CACHING		0x08
#define MGC_SFF8070I_PAGE_REM_BLK_ACC	0x1b
#define MGC_SFF8070I_PAGE_TIMER_PROTECT	0x1c
#define MGC_SFF8070I_PAGE_ALL			0x3f

/* device types */
#define MGC_SFF8070I_DEVICE_TYPE_DIRECT	0
#define MGC_SFF8070I_DEVICE_TYPE_SEQUENTIAL	0x1
#define MGC_SFF8070I_DEVICE_TYPE_WORM	0x4
#define MGC_SFF8070I_DEVICE_TYPE_CDROM	0x5
#define MGC_SFF8070I_DEVICE_TYPE_OPTICAL	0x7

/* sense codes */
typedef enum
{
    MGC_SFF8070I_SC_NONE,
    MGC_SFF8070I_SC_RECOVERED_ERROR,
    MGC_SFF8070I_SC_NOT_READY,
    MGC_SFF8070I_SC_MEDIUM_ERROR,
    MGC_SFF8070I_SC_HARDWARE_ERROR,
    MGC_SFF8070I_SC_ILLEGAL_REQUEST,
    MGC_SFF8070I_SC_UNIT_ATTENTION,
    MGC_SFF8070I_SC_DATA_PROTECT,
    MGC_SFF8070I_SC_BLANK_CHECK,
    MGC_SFF8070I_SC_VENDOR,
    MGC_SFF8070I_SC_RESERVED1,
    MGC_SFF8070I_SC_ABORTED_COMMAND,
    MGC_SFF8070I_SC_RESERVED2,
    MGC_SFF8070I_SC_VOLUME_OVERFLOW,
    MGC_SFF8070I_SC_MISCOMPARE
} MGC_SFF8070ISenseCode;

/* additional sense codes */
#define MGC_SFF8070I_ASC_UNIT_NOT_READY	0x04
#define MGC_SFF8070I_ASC_INVALID_OPCODE	0x20
#define MGC_SFF8070I_ASC_INVALID_ADDRESS	0x21
#define MGC_SFF8070I_ASC_INVALID_CDB_FIELD	0x24
#define MGC_SFF8070I_ASC_WRITE_PROTECTED	0x27
#define MGC_SFF8070I_ASC_MEDIUM_CHANGE	0x28
#define MGC_SFF8070I_ASC_PARAMS_CHANGE	0x2a
#define MGC_SFF8070I_ASC_MEDIUM_ERROR	0x30
#define MGC_SFF8070I_ASC_MEDIUM_NOT_PRESENT	0x3a

/* additional sense code qualifiers */
#define MGC_SFF8070I_ASCQ_INVALID_CDB_FIELD	0x00

/* byte-swapping */
#ifndef BIG_ENDIAN

#define MGC_SFF8070I_SWAP16(_data) (((_data) << 8) | ((_data) >> 8))
#define MGC_SFF8070I_SWAP32(_data) (((uint32_t)(_data) << 24) | \
    (((_data) << 8) & 0x00ff0000) | \
    (((_data) >> 8) & 0xff00) | ((uint32_t)(_data) >> 24))

#else

#define MGC_SFF8070I_SWAP16(_data) _data
#define MGC_SFF8070I_SWAP32(_data) _data

#endif

#include "mu_pkon.h"
/**
* MGC_Sff8070iInquiryCommand.
* @field bOpcode opcode
* @field bLun LUN in upper 3 bits
* @field bLength allocation length
*/
typedef struct
{
    uint8_t bOpcode;
    uint8_t bLun;
    uint8_t abReserved1[2];
    uint8_t bLength;
    uint8_t abReserved2[7];
} MGC_Sff8070iInquiryCommand;
#include "mu_pkoff.h"

#define MGC_S_SFF8070I_INQUIRY_LUN	5
#define MGC_M_SFF8070I_INQUIRY_LUN 0x7

#include "mu_pkon.h"
/**
* MGC_Sff8070iCommand.
* @field bOpcode opcode
* @field bLun LUN (upper 3 bits)
* @field dwLba Logical Block Address
* @field wLength length
*/
typedef struct
{
    uint8_t bOpcode;
    uint8_t bLun;
    uint32_t dwLba;
    uint8_t bReserved;
    uint16_t wLength;
    uint8_t abReserved[3];
} MGC_Sff8070iCommand;
#include "mu_pkoff.h"

#include "mu_pkon.h"
/**
* MGC_Sff8070iCommandExt.
* @field bOpcode opcode
* @field bLun LUN (upper 3 bits)
* @field dwLba Logical Block Address
* @field dwLength length
*/
typedef struct
{
    uint8_t bOpcode;
    uint8_t bLun;
    uint32_t dwLba;
    uint32_t dwLength;
    uint8_t abReserved[2];
} MGC_Sff8070iCommandExt;
#include "mu_pkoff.h"

#include "mu_pkon.h"
/**
* MGC_Sff8070iInquiryData.
* @field bDeviceType qualifier and type (RBC type=0x0e)
* @field bRmb set bit 7 for removable media
* @field bVersion version of applicable standard
* @field bFormat AERC, NORM_ACA, HI_SUP, FORMAT fields (use format=2)
* @field bExtraLength length, in bytes, of parameters
* @field abmFlags various flags
* @field aVid space-padded ASCII vendor ID
* @field aPid space-padded ASCII product ID
* @field aRevision space-padded product revision
*/
typedef struct
{
    uint8_t bDeviceType;
    uint8_t bRmb;
    uint8_t bVersion;
    uint8_t bFormat;
    uint8_t bExtraLength;
    uint8_t abmFlags[3];
    char aVendorId[8];
    char aProductId[8];
    char aProductRev[8];
} MGC_Sff8070iInquiryData;
#include "mu_pkoff.h"

/* bRmb: */
#define MGC_M_SFF8070I_INQUIRY_RMB		0x80
/* bFormat: */
#define MGC_M_SFF8070I_INQUIRY_STD 1

#include "mu_pkon.h"
/**
* MGC_Sff8070iModePage.
* Mode page
* @field bPage page code and PS
* @field bLength length of data
* @field aData variable-sized data
*/
typedef struct
{
    uint8_t bPage;
    uint8_t bLength;
    uint8_t aData[1];
} MGC_Sff8070iModePage;
#include "mu_pkoff.h"

/** Parameters saveable (in bPage) */
#define MGC_M_SFF8070I_MODE_PAGE_PS		0x80
/** Page code (in bPage) */
#define MGC_M_SFF8070I_MODE_PAGE_CODE	0x3f

#include "mu_pkon.h"
/**
 * MGC_MsdSff8070iSenseData.
 * Sense data
 * @field bResponseCode response code (current errors=0x70), plus VALID
 * @field bSenseKey sense key, plus FILEMARK, EOM, and ILI
 * @field dwInfo information (LBA for direct-access devices)
 * @field bAdditionalLength length of additional data (everything else here)
 * @field dwCommandInfo command-specific information
 * @field bAsc ASC
 * @field bAscq ASCQ
 * @field bFruCode Field-Replaceable Unit code
 * @field aKeyFlags key-specific data, plus SKSV
 * @field aAdditional variable-length additional data
 */
typedef struct
{
    uint8_t bResponseCode;
    uint8_t bObsolete;
    uint8_t bSenseKey;
    uint32_t dwInfo;
    uint8_t bAdditionalLength;
    uint32_t dwCommandInfo;
    uint8_t bAsc;
    uint8_t bAscq;
    uint8_t bFruCode;
    uint8_t aKeyFlags[3];
    uint8_t aAdditional[1];
} MGC_MsdSff8070iSenseData;
#include "mu_pkoff.h"

/* bResponseCode: */
#define MGC_M_SFF8070I_SENSE_DATA_VALID		0x80
/* bSenseKey: */
#define MGC_M_SFF8070I_SENSE_DATA_FILEMARK	0x80
#define MGC_M_SFF8070I_SENSE_DATA_EOM		0x40
#define MGC_M_SFF8070I_SENSE_DATA_ILI		0x20
/* aKeyFlags[0]: */
#define MGC_M_SFF8070I_SENSE_DATA_SKSV		0x80

#include "mu_pkon.h"
/**
* MGC_Sff8070iCapacityResponse.
* Response to read capacity
* @field dwLastBlock last logical block index
* @field dwBytesPerBlock bytes per block
*/
typedef struct
{
    uint32_t dwLastBlock;
    uint32_t dwBytesPerBlock;
} MGC_Sff8070iCapacityResponse;
#include "mu_pkoff.h"

#include "mu_pkon.h"
/**
 * MGC_Sff8070iFormatCapacityResponse.
 * Response to read format capacities
 */
typedef struct
{
    uint32_t dwListLength;
    uint32_t dwBlockCount;
    uint32_t dwBytesPerBlock;
} MGC_Sff8070iFormatCapacityResponse;
#include "mu_pkoff.h"

#endif	/* multiple inclusion protection */
