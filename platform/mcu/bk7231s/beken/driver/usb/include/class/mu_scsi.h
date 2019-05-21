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
 * MUSB-MicroSW SCSI definitions.
 * $Revision: 1.9 $
 */

#ifndef __MUSB_MSD_SCSI_H__
#define __MUSB_MSD_SCSI_H__

#include "mu_tools.h"

/** Maximum Length of command */
#define MGC_SCSI_MAX_CMD_LENGTH             0x10

/* SCSI Commands */
#define MGC_SCSI_INQUIRY            0x12
#define MGC_SCSI_REQUEST_SENSE      0x03
#define MGC_SCSI_SEND_DIAGNOSTIC    0x1D
#define MGC_SCSI_TEST_UNIT_READY    0x00
#define MGC_SCSI_FORMAT_UNIT        0x04
#define MGC_SCSI_READ6              0x08
#define MGC_SCSI_WRITE6             0x0A
#define MGC_SCSI_READ10             0x28
#define MGC_SCSI_WRITE10            0x2A
#define MGC_SCSI_READ12             0xA8
#define MGC_SCSI_WRITE12            0xAA
#define MGC_SCSI_READ_CAPACITY      0x25
#define MGC_SCSI_RELEASE            0x17
#define MGC_SCSI_RESERVE            0x16
#define MGC_SCSI_RD_FMT_CAPC        0x23
#define MGC_SCSI_MODE_SENSE         0x1A
#define MGC_SCSI_MODE_SENSE10       0x5A
#define MGC_SCSI_VERIFY             0x2F
#define MGC_SCSI_START_STOP_UNIT    0x1B
#define MGC_SCSI_SYNC_CACHE	    0x35
#define MGC_SCSI_MODE_SELECT	    0x15
#define MGC_SCSI_MODE_SELECT10      0x55
#define MGC_SCSI_WRITE_BUFFER	    0x3B
#define MGC_SCSI_PREVENT_ALLOW_MED_REMOVE   0x1e
#define MGC_SCSI_READ_BUFFER        0x3C
#define MGC_SCSI_SEEK6              0x0B
#define MGC_SCSI_SEEK10             0x2B
#define MGC_SCSI_CHNGDEF            0x40
#define MGC_SCSI_COMPARE            0x39
#define MGC_SCSI_COPY               0x18
#define MGC_SCSI_COPY_VERIFY        0x3A
#define MGC_SCSI_LOG_SELECT         0x4C
#define MGC_SCSI_LOG_SENSE          0x4D
#define MGC_SCSI_CMD_RECV_DIAGNOSTIC 0x1C

/* Mode pages */
#define MGC_SCSI_PAGE_CONTROL		0x0a
#define MGC_SCSI_PAGE_DISCONNECT	0x02
#define MGC_SCSI_PAGE_INFO_EXCEPTIONS	0x1c
#define MGC_SCSI_PAGE_POWER_CONDITION	0x1a
#define MGC_SCSI_PAGE_PROTOCOL_LUN	0x18
#define MGC_SCSI_PAGE_PROTOCOL_PORT	0x19

/* VPD pages */
#define MGC_SCSI_PAGE_SUPPORTED_VPD	0x00
#define MGC_SCSI_PAGE_UNIT_SERIAL_NUM	0x80
#define MGC_SCSI_PAGE_ASCII_OPER_DEF	0x82
#define MGC_SCSI_PAGE_DEVICE_ID		0x83

/* status codes */
#define MGC_SCSI_STATUS_CHECK_CONDITION	0x02

/* device types */
#define MGC_SCSI_DEVICE_TYPE_DIRECT	0
#define MGC_SCSI_DEVICE_TYPE_SEQUENTIAL	0x1
#define MGC_SCSI_DEVICE_TYPE_PRINTER	0x2
#define MGC_SCSI_DEVICE_TYPE_PROCESSOR	0x3
#define MGC_SCSI_DEVICE_TYPE_WORM	0x4
#define MGC_SCSI_DEVICE_TYPE_CDROM	0x5
#define MGC_SCSI_DEVICE_TYPE_SCANNER	0x6
#define MGC_SCSI_DEVICE_TYPE_OPTICAL	0x7
#define MGC_SCSI_DEVICE_TYPE_CHANGER	0x8
#define MGC_SCSI_DEVICE_TYPE_COMM	0x9
#define MGC_SCSI_DEVICE_TYPE_RAID	0xc
#define MGC_SCSI_DEVICE_TYPE_RBC	0xe
#define MGC_SCSI_DEVICE_TYPE_CARDREADER	0xf

/* sense codes */
typedef enum
{
    MGC_SCSI_SC_NONE,
    MGC_SCSI_SC_RECOVERED_ERROR,
    MGC_SCSI_SC_NOT_READY,
    MGC_SCSI_SC_MEDIUM_ERROR,
    MGC_SCSI_SC_HARDWARE_ERROR,
    MGC_SCSI_SC_ILLEGAL_REQUEST,
    MGC_SCSI_SC_UNIT_ATTENTION,
    MGC_SCSI_SC_DATA_PROTECT,
    MGC_SCSI_SC_BLANK_CHECK,
    MGC_SCSI_SC_VENDOR,
    MGC_SCSI_SC_COPY_ABORTED,
    MGC_SCSI_SC_ABORTED_COMMAND,
    MGC_SCSI_SC_OBSOLETE,
    MGC_SCSI_SC_VOLUME_OVERFLOW,
    MGC_SCSI_SC_MISCOMPARE
} MGC_ScsiSenseCode;

/* additional sense codes */
#define MGC_SCSI_ASC_UNIT_NOT_READY	0x04
#define MGC_SCSI_ASC_INVALID_OPCODE	0x20
#define MGC_SCSI_ASC_INVALID_ADDRESS	0x21
#define MGC_SCSI_ASC_INVALID_CDB_FIELD	0x24
#define MGC_SCSI_ASC_WRITE_PROTECTED	0x27
#define MGC_SCSI_ASC_MEDIUM_CHANGE	0x28
#define MGC_SCSI_ASC_PARAMS_CHANGE	0x2a
#define MGC_SCSI_ASC_MEDIUM_ERROR	0x30
#define MGC_SCSI_ASC_MEDIUM_NOT_PRESENT	0x3a

/* additional sense code qualifiers */
#define MGC_SCSI_ASCQ_INVALID_CDB_FIELD	0x00

/* byte-swapping */
#ifndef BIG_ENDIAN

#define MGC_SCSI_SWAP16(_data) (((_data) << 8) | ((_data) >> 8))
#define MGC_SCSI_SWAP32(_data) (((uint32_t)(_data) << 24) | \
    (((_data) << 8) & 0x00ff0000) | \
    (((_data) >> 8) & 0xff00) | ((uint32_t)(_data) >> 24))

#else

#define MGC_SCSI_SWAP16(_data) _data
#define MGC_SCSI_SWAP32(_data) _data

#endif

#include "mu_pkon.h"
/**
* MGC_MsdCommand6.
* SPC-2 6-byte command.
* @field bOpcode opcode
* @field bBlockHi5 upper 5 bits of block
* @field wBlockLo16 lower 16 bits of block
* @field bLength length (transfer, allocation, or parameter block)
* @field bControl control byte (always 0 for RBC)
*/
typedef struct
{
    uint8_t bOpcode;
    uint8_t bBlockHi5;
    uint16_t wBlockLo16;
    uint8_t bLength;
    uint8_t bControl;
} MGC_MsdCommand6;
#include "mu_pkoff.h"

#include "mu_pkon.h"
/**
* MGC_MsdInquiryCommand.
* SPC-2 inquiry command.
* @field bOpcode opcode
* @field bmFlags CMD_DT and EVPD
* @field bPage page code
* @field bLength allocation length
* @field bControl control byte (always 0 for RBC)
*/
typedef struct
{
    uint8_t bOpcode;
    uint8_t bmFlags;
    uint8_t bPage;
    uint8_t bReserved;
    uint8_t bLength;
    uint8_t bControl;
} MGC_MsdInquiryCommand;
#include "mu_pkoff.h"

/** Send command support data */
#define MGC_M_MSD_INQUIRY_CMD_DT	0x02
/** Enable Vital Product Data */
#define MGC_M_MSD_INQUIRY_EVPD		0x01

#include "mu_pkon.h"
/**
* MGC_MsdCommand10.
* SPC-2 10-byte command.
* @field bOpcode opcode
* @field bServiceAction Logical Unit Number in bits 7:5 in UFI
* @field dwBlock block address
* @field wLength length (transfer, allocation, or parameter block)
* @field bControl control byte (always 0 for RBC)
*/
typedef struct
{
    uint8_t bOpcode;
    uint8_t bServiceAction;
    uint32_t dwBlock;
    uint8_t bReserved;
    uint16_t wLength;
    uint8_t bControl;
} MGC_MsdCommand10;
#include "mu_pkoff.h"

#include "mu_pkon.h"
/**
* MGC_MsdCommand12.
* SPC-2 12-byte command.
* @field bOpcode opcode
* @field bServiceAction Logical Unit Number in bits 7:5 in UFI
* @field dwBlock block address
* @field dwLength length (transfer, allocation, or parameter block)
* @field bControl control byte (always 0 for RBC)
*/
typedef struct
{
    uint8_t bOpcode;
    uint8_t bServiceAction;
    uint32_t dwBlock;
    uint32_t dwLength;
    uint8_t bReserved;
    uint8_t bControl;
} MGC_MsdCommand12;
#include "mu_pkoff.h"

typedef MGC_MsdCommand6 MGC_MsdModeSense6;
typedef MGC_MsdCommand6 MGC_MsdModeSelect6;
typedef MGC_MsdCommand6 MGC_MsdRead6;
typedef MGC_MsdCommand6 MGC_MsdWrite6;
typedef MGC_MsdCommand10 MGC_MsdModeSense10;
typedef MGC_MsdCommand10 MGC_MsdModeSelect10;
typedef MGC_MsdCommand10 MGC_MsdRead10;
typedef MGC_MsdCommand10 MGC_MsdWrite10;

#include "mu_pkon.h"
/**
* MGC_MsdStandardInquiryData.
* SPC-2 standard inquiry data (when CMD_DT=0 and EVPD=0 in inquiry command)
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
    char aVid[8];
    char aPid[16];
    char aRevision[4];
} MGC_MsdStandardInquiryData;
#include "mu_pkoff.h"

/* bRmb: */
#define MGC_M_MSD_INQUIRY_RMB		0x80
/* bFormat: */
#define MGC_M_MSD_INQUIRY_AERC		0x80
#define MGC_M_MSD_INQUIRY_NORM_ACA	0x20
#define MGC_M_MSD_INQUIRY_HI_SUP	0x10
#define MGC_M_MSD_INQUIRY_FORMAT	0x0F
/* abmFlags[0]: */
#define MGC_M_MSD_INQUIRY_SCCS		0x80
/* abmFlags[1]: */
#define MGC_M_MSD_INQUIRY_B_QUE		0x80
#define MGC_M_MSD_INQUIRY_ENC_SERV	0x40
#define MGC_M_MSD_INQUIRY_VS1		0x20
#define MGC_M_MSD_INQUIRY_MULTI_P	0x10
#define MGC_M_MSD_INQUIRY_M_CHNGR	0x08
#define MGC_M_MSD_INQUIRY_ADDR16	0x01
/* abmFlags[2]: */
#define MGC_M_MSD_INQUIRY_REL_ADR	0x80
#define MGC_M_MSD_INQUIRY_WBUS16	0x20
#define MGC_M_MSD_INQUIRY_SYNC		0x10
#define MGC_M_MSD_INQUIRY_LINKED	0x08
#define MGC_M_MSD_INQUIRY_CMD_QUE	0x02
#define MGC_M_MSD_INQUIRY_VS2		0x01

#include "mu_pkon.h"
/**
 * MGC_MsdModeParamHeader6.
 * Mode parameter header (6-byte version)
 * @field bDataLength length of data following itself
 * @field bMediumType medium type (0 for RBC)
 * @field bDeviceSpecific device-specific parameter (0 for RBC)
 * @field bBlockDescLength block descriptor length (0 for RBC)
 */
typedef struct
{
    uint8_t bDataLength;
    uint8_t bMediumType;
    uint8_t bDeviceSpecific;
    uint8_t bBlockDescLength;
} MGC_MsdModeParamHeader6;
#include "mu_pkoff.h"

#include "mu_pkon.h"
/**
* MGC_MsdRbcDeviceParams.
* RBC device parameters.
* @field bPageCode use 0x86
* @field bPageLength use 0x0b
* @field bWcd WCD flag
* @field wBlockSize block size (bytes)
* @field bBlockCountHi8 upper 8 bits block count
* @field dwBlockCountLo32 lower 32 bits block count
* @field bPowerPerformance 0=maximize power-saving .. ff=maximize performance
* @field bFlags READD, WRITED, FORMATD, LOCKD flags
*/
typedef struct
{
    uint8_t bPageCode;
    uint8_t bPageLength;
    uint8_t bWcd;
    uint16_t wBlockSize;
    uint8_t bBlockCountHi8;
    uint32_t dwBlockCountLo32;
    uint8_t bPowerPerformance;
    uint8_t bFlags;
    uint8_t bReserved;
} MGC_MsdRbcDeviceParams;
#include "mu_pkoff.h"

/* bWcd: */
#define MGC_M_MSD_RBC_WCD		0x01
/* bFlags: */
#define MGC_M_MSD_RBC_READD		0x08
#define MGC_M_MSD_RBC_WRITED		0x04
#define MGC_M_MSD_RBC_FORMATD		0x02
#define MGC_M_MSD_RBC_LOCKD		0x01

#include "mu_pkon.h"
/**
* MGC_MsdModePage.
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
} MGC_MsdModePage;
#include "mu_pkoff.h"

/** Parameters saveable (in bPage) */
#define MGC_M_MSD_MODE_PAGE_PS		0x80
/** Page code (in bPage) */
#define MGC_M_MSD_MODE_PAGE_CODE	0x3f

#include "mu_pkon.h"
/**
 * MGC_MsdCommandSupportData.
 * Command support data
 * @field bDeviceType as in standard inquiry data
 * @field bSupport 0=info unavailable, 1=unsupported, 3=supported (standard-conforming), 5=supported (vendor-specific)
 * @field bVersion as in standard inquiry data
 * @field bCdbSize size of data (command code plus usage map)
 * @field aData variable-size data
 */
typedef struct
{
    uint8_t bDeviceType;
    uint8_t bSupport;
    uint8_t bVersion;
    uint8_t bReserved[2];
    uint8_t bCdbSize;
    uint8_t aData[1];
} MGC_MsdCommandSupportData;
#include "mu_pkoff.h"

#include "mu_pkon.h"
/**
 * MGC_MsdScsiSenseData.
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
} MGC_MsdScsiSenseData;
#include "mu_pkoff.h"

/* bResponseCode: */
#define MGC_M_MSD_SCSI_SENSE_DATA_VALID		0x80
/* bSenseKey: */
#define MGC_M_MSD_SCSI_SENSE_DATA_FILEMARK	0x80
#define MGC_M_MSD_SCSI_SENSE_DATA_EOM		0x40
#define MGC_M_MSD_SCSI_SENSE_DATA_ILI		0x20
/* aKeyFlags[0]: */
#define MGC_M_MSD_SCSI_SENSE_DATA_SKSV		0x80

#include "mu_pkon.h"
/**
* MGC_MsdCapacityResponse.
* Response to read capacity
* @field dwLastBlock last logical block index
* @field dwBytesPerBlock bytes per block
*/
typedef struct
{
    uint32_t dwLastBlock;
    uint32_t dwBytesPerBlock;
} MGC_MsdCapacityResponse;
#include "mu_pkoff.h"

#include "mu_pkon.h"
/**
 * MGC_MsdFormatCapacityResponse.
 * Response to read format capacities
 */
typedef struct
{
    uint32_t dwListLength;
    uint32_t dwBlockCount;
    uint32_t dwBytesPerBlock;
} MGC_MsdFormatCapacityResponse;
#include "mu_pkoff.h"

#endif	/* multiple inclusion protection */
