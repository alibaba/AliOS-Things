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
 * MUSB-MicroSW PTP (Picture Transfer Protocol) definitions.
 * $Revision: 1.1 $
 */

#ifndef __MUSB_PTP_H__
#define __MUSB_PTP_H__

/* upper 4 bits of 16-bit codes */
#define MGC_PTP_DATACODE_TYPE_STD_OPERATION          0x1
#define MGC_PTP_DATACODE_TYPE_STD_RESPONSE           0x2
#define MGC_PTP_DATACODE_TYPE_STD_OBJECT_FORMAT      0x3
#define MGC_PTP_DATACODE_TYPE_STD_EVENT              0x4
#define MGC_PTP_DATACODE_TYPE_STD_DEVICE_PROP        0x5
#define MGC_PTP_DATACODE_TYPE_VENDOR_OPERATION       0x9
#define MGC_PTP_DATACODE_TYPE_VENDOR_RESPONSE        0xa
#define MGC_PTP_DATACODE_TYPE_VENDOR_OBJECT_FORMAT   0xb
#define MGC_PTP_DATACODE_TYPE_VENDOR_EVENT           0xc
#define MGC_PTP_DATACODE_TYPE_VENDOR_DEVICE_PROP     0xe
/* shift for type field */
#define MGC_S_PTP_DATACODE_TYPE		12

/* data type codes */
#define MGC_PTP_DATATYPE_INT8		0x0001
#define MGC_PTP_DATATYPE_UINT8		0x0002
#define MGC_PTP_DATATYPE_INT16		0x0003
#define MGC_PTP_DATATYPE_UINT16		0x0004
#define MGC_PTP_DATATYPE_INT32		0x0005
#define MGC_PTP_DATATYPE_UINT32		0x0006
#define MGC_PTP_DATATYPE_INT64		0x0007
#define MGC_PTP_DATATYPE_UINT64		0x0008
#define MGC_PTP_DATATYPE_INT128		0x0009
#define MGC_PTP_DATATYPE_UINT128	0x000a
/* arrays (32-bit length followed by data) */
#define MGC_PTP_DATATYPE_AINT8		0x4001
#define MGC_PTP_DATATYPE_AUINT8		0x4002
#define MGC_PTP_DATATYPE_AINT16		0x4003
#define MGC_PTP_DATATYPE_AUINT16	0x4004
#define MGC_PTP_DATATYPE_AINT32		0x4005
#define MGC_PTP_DATATYPE_AUINT32	0x4006
#define MGC_PTP_DATATYPE_AINT64		0x4007
#define MGC_PTP_DATATYPE_AUINT64	0x4008
#define MGC_PTP_DATATYPE_AINT128	0x4009
#define MGC_PTP_DATATYPE_AUINT128	0x400a
/* Unicode string (1-byte length [includes null], then null-terminated data) */
#define MGC_PTP_DATATYPE_STR		0xffff

/* operation codes */
#define MGC_PTP_OPCODE_GET_DEVICE_INFO         0x1001
#define MGC_PTP_OPCODE_OPEN_SESSION            0x1002 /* SessionID */
#define MGC_PTP_OPCODE_CLOSE_SESSION           0x1003
#define MGC_PTP_OPCODE_GET_STORAGE_IDS         0x1004
#define MGC_PTP_OPCODE_GET_STORAGE_INFO        0x1005 /* StorageID */
#define MGC_PTP_OPCODE_GET_NUM_OBJECTS         0x1006 /* StorageID, [ObjectFormatCode], [handle of association], <= NumObjects */
#define MGC_PTP_OPCODE_GET_OBJECT_HANDLES      0x1007 /* StorageID, [ObjectFormatCode], [handle of association] */
#define MGC_PTP_OPCODE_GET_OBJECT_INFO         0x1008 /* ObjectHandle */
#define MGC_PTP_OPCODE_GET_OBJECT              0x1009 /* ObjectHandle */
#define MGC_PTP_OPCODE_GET_THUMB               0x100a /* ObjectHandle */
#define MGC_PTP_OPCODE_DELETE_OBJECT           0x100b /* ObjectHandle, [ObjectFormatCode] */
#define MGC_PTP_OPCODE_SEND_OBJECT_INFO        0x100c /* [destination storageID], [parent handle], <= StorageID, parent handle, new handle */
#define MGC_PTP_OPCODE_SEND_OBJECT             0x100d
#define MGC_PTP_OPCODE_INITIATE_CAPTURE        0x100e /* [StorageID], [ObjectFormatCode] */
#define MGC_PTP_OPCODE_FORMAT_STORE            0x100f /* StorageID, [FilesystemFormat] */
#define MGC_PTP_OPCODE_RESET_DEVICE            0x1010
#define MGC_PTP_OPCODE_SELF_TEST               0x1011 /* [SelfTestType] */
#define MGC_PTP_OPCODE_SET_OBJECT_PROTECTION   0x1012 /* handle, ProtectionStatus */
#define MGC_PTP_OPCODE_POWER_DOWN              0x1013
#define MGC_PTP_OPCODE_GET_DEVICE_PROP_DESC    0x1014 /* DevicePropCode */
#define MGC_PTP_OPCODE_GET_DEVICE_PROP_VALUE   0x1015 /* DevicePropCode */
#define MGC_PTP_OPCODE_SET_DEVICE_PROP_VALUE   0x1016 /* DevicePropCode */
#define MGC_PTP_OPCODE_RESET_DEVICE_PROP_VALUE 0x1017 /* DevicePropCode */
#define MGC_PTP_OPCODE_TERMINATE_OPEN_CAPTURE  0x1018 /* TransactionID */
#define MGC_PTP_OPCODE_MOVE_OBJECT             0x1019 /* handle, StorageID, handle of new parent */
#define MGC_PTP_OPCODE_COPY_OBJECT             0x101a /* handle, StorageID, handle of destination parent, <= new handle */
#define MGC_PTP_OPCODE_GET_PARTIAL_OBJECT      0x101b /* handle, byte offset, byte count, <= actual byte count */
#define MGC_PTP_OPCODE_INITIATE_OPEN_CAPTURE   0x101c /* [StorageID], [ObjectFormatCode] */

/* response codes */
#define MGC_PTP_RESPONSE_OK                         0x2001
#define MGC_PTP_RESPONSE_GENERAL_ERROR              0x2002
#define MGC_PTP_RESPONSE_SESSION_NOT_OPEN           0x2003
#define MGC_PTP_RESPONSE_INVALID_TRANS_ID           0x2004
#define MGC_PTP_RESPONSE_OP_NOT_SUPPORTED           0x2005
#define MGC_PTP_RESPONSE_PARAM_NOT_SUPPORTED        0x2006
#define MGC_PTP_RESPONSE_INCOMPLETE_TRANSFER        0x2007
#define MGC_PTP_RESPONSE_INVALID_STORAGE_ID         0x2008
#define MGC_PTP_RESPONSE_INVALID_OBJECT_HANDLE      0x2009
#define MGC_PTP_RESPONSE_DEVICE_PROP_NOT_SUPPORTED  0x200a
#define MGC_PTP_RESPONSE_INVALID_OBJECT_FORMAT      0x200b
#define MGC_PTP_RESPONSE_STORE_FULL                 0x200c
#define MGC_PTP_RESPONSE_OBJECT_WRITE_PROTECTED     0x200d
#define MGC_PTP_RESPONSE_STORE_READ_ONLY            0x200e
#define MGC_PTP_RESPONSE_ACCESS_DENIED              0x200f
#define MGC_PTP_RESPONSE_NO_THUMBNAIL               0x2010
#define MGC_PTP_RESPONSE_SELFTEST_FAILED            0x2011
#define MGC_PTP_RESPONSE_PARTIAL_DELETION           0x2012
#define MGC_PTP_RESPONSE_STORE_NOT_AVAILABLE        0x2013
#define MGC_PTP_RESPONSE_SPEC_BY_FORMAT_UNSUPPORTED 0x2014
#define MGC_PTP_RESPONSE_NO_VALID_OBJECT_INFO       0x2015
#define MGC_PTP_RESPONSE_INVALID_CODE_FORMAT        0x2016
#define MGC_PTP_RESPONSE_UNKNOWN_VENDOR_CODE        0x2017
#define MGC_PTP_RESPONSE_CAPTURE_ALREADY_TERMINATED 0x2018
#define MGC_PTP_RESPONSE_DEVICE_BUSY                0x2019
#define MGC_PTP_RESPONSE_INVALID_PARENT_OBJECT      0x201s
#define MGC_PTP_RESPONSE_INVALID_DEVICE_PROP_FORMAT 0x201b
#define MGC_PTP_RESPONSE_INVALID_DEVICE_PROP_VALUE  0x201c
#define MGC_PTP_RESPONSE_INVALID_PARAMETER          0x201d
#define MGC_PTP_RESPONSE_SESSION_ALREADY_OPEN       0x201e
#define MGC_PTP_RESPONSE_TRANSACTION_CANCELLED      0x201f
#define MGC_PTP_RESPONSE_SPECT_OF_DEST_UNSUPPORTED  0x2020

/* event codes */
#define MGC_PTP_EVENT_CANCEL_TRANSACTION      0x4001
#define MGC_PTP_EVENT_OBJECT_ADDED            0x4002 /* handle */
#define MGC_PTP_EVENT_OBJECT_REMOVED          0x4003 /* handle */
#define MGC_PTP_EVENT_STORE_ADDED             0x4004 /* StorageID */
#define MGC_PTP_EVENT_STORE_REMOVED           0x4005 /* StorageID */
#define MGC_PTP_EVENT_DEVICE_PROP_CHANGED     0x4006 /* DevicePropCode */
#define MGC_PTP_EVENT_OBJECT_INFO_CHANGED     0x4007 /* handle */
#define MGC_PTP_EVENT_DEVICE_INFO_CHANGED     0x4008
#define MGC_PTP_EVENT_REQUEST_OBJECT_TRANSFER 0x4009 /* handle */
#define MGC_PTP_EVENT_STORE_FULL              0x400a /* StorageID */
#define MGC_PTP_EVENT_DEVICE_RESET            0x400b
#define MGC_PTP_EVENT_STORAGE_INFO_CHANGED    0x400c /* StorageID */
#define MGC_PTP_EVENT_CAPTURE_COMPLETE        0x400d /* TransactionID */
#define MGC_PTP_EVENT_UNREPORTED_STATUS       0x400e

/* device property codes */
#define MGC_PTP_DEVPROP_BATTERY_LEVEL              0x5001
#define MGC_PTP_DEVPROP_FUNCTIONAL_MODE            0x5002
#define MGC_PTP_DEVPROP_IMAGE_SIZE                 0x5003
#define MGC_PTP_DEVPROP_COMPRESSION_SETTING        0x5004
#define MGC_PTP_DEVPROP_WHITE_BALANCE              0x5005
#define MGC_PTP_DEVPROP_RGB_GAIN                   0x5006
#define MGC_PTP_DEVPROP_F_NUMBER                   0x5007
#define MGC_PTP_DEVPROP_FOCAL_LENGTH               0x5008
#define MGC_PTP_DEVPROP_FOCUS_DISTANCE             0x5009
#define MGC_PTP_DEVPROP_FOCUS_MODE                 0x500a
#define MGC_PTP_DEVPROP_EXPOSURE_METERING_MODE     0x500b
#define MGC_PTP_DEVPROP_FLASH_MODE                 0x500c
#define MGC_PTP_DEVPROP_EXPOSURE_TIME              0x500d
#define MGC_PTP_DEVPROP_EXPOSURE_PROGRAM_MODE      0x500e
#define MGC_PTP_DEVPROP_EXPOSURE_INDEX             0x500f
#define MGC_PTP_DEVPROP_EXPOSURE_BIAS_COMPENSATION 0x5010
#define MGC_PTP_DEVPROP_DATE_TIME                  0x5011
#define MGC_PTP_DEVPROP_CAPTURE_DELAY              0x5012
#define MGC_PTP_DEVPROP_STILL_CAPTURE_MODE         0x5013
#define MGC_PTP_DEVPROP_CONTRAST                   0x5014
#define MGC_PTP_DEVPROP_SHARPNESS                  0x5015
#define MGC_PTP_DEVPROP_DIGITAL_ZOOM               0x5016
#define MGC_PTP_DEVPROP_EFFECT_MODE                0x5017
#define MGC_PTP_DEVPROP_BURST_NUMBER               0x5018
#define MGC_PTP_DEVPROP_BURST_INTERVAL             0x5019
#define MGC_PTP_DEVPROP_TIME_LAPSE_NUMBER          0x501a
#define MGC_PTP_DEVPROP_TIME_LAPSE_INTERVAL        0x501b
#define MGC_PTP_DEVPROP_FOCUS_METERING_MODE        0x501c
#define MGC_PTP_DEVPROP_UPLOAD_URL                 0x501d
#define MGC_PTP_DEVPROP_ARTIST                     0x501e
#define MGC_PTP_DEVPROP_COPYRIGHT_INFO             0x501f

/*
 * DeviceInfo dataset:
 * UINT16  StandardVersion
 * UINT32  VendorExtensionID
 * UINT16  VendorExtensionVersion
 * STR     VendorExtensionDesc
 * UINT16  FunctionalMode
 * AUINT16 OperationsSupported
 * AUINT16 EventsSupported
 * AUINT16 DevicePropertiesSupported
 * AUINT16 CaptureFormats
 * AUINT16 ImageFormats
 * STR     Manufacturer
 * STR     Model
 * STR     DeviceVersion
 * STR     SerialNumber
 */
#define MGC_PTP_FUNCTIONAL_MODE_STANDARD    0x0000
#define MGC_PTP_FUNCTIONAL_MODE_SLEEP_STATE 0x0001

/*
 * ObjectInfo dataset:
 * UINT32  StorageID
 * UINT16  ObjectFormat
 * UINT16  ProtectionStatus
 * UINT32  ObjectCompressedSize
 * UINT16  ThumbFormat
 * UINT32  ThumbCompressedSize
 * UINT32  ThumbPixWidth
 * UINT32  ThumbPixHeight
 * UINT32  ImagePixWidth
 * UINT32  ImagePixHeight
 * UINT32  ImageBitDepth
 * UINT32  ParentObject
 * UINT16  AssociationType
 * UINT32  AssociationDesc
 * UINT32  SequenceNumber
 * STR     Filename
 * STR     CaptureDate
 * STR     ModificationDate
 * STR     Keywords
 */
#define MGC_PTP_OBJECT_PROTECTION_NONE      0x0000
#define MGC_PTP_OBJECT_PROTECTION_READ_ONLY 0x0001

/*
 * StorageInfo dataset:
 * UINT16  StorageType
 * UINT16  FilesystemType
 * UINT16  AccessCapability
 * UINT64  MaxCapacity
 * UINT64  FreeSpaceInBytes
 * UINT32  FreeSpaceInImages
 * STR     StorageDescription
 * STR     VolumeLabel
 */
#define MGC_PTP_STORAGE_TYPE_FIXED_ROM     0x0001
#define MGC_PTP_STORAGE_TYPE_REMOVABLE_ROM 0x0002
#define MGC_PTP_STORAGE_TYPE_FIXED_RAM     0x0003
#define MGC_PTP_STORAGE_TYPE_REMOVABLE_RAM 0x0004

#define MGC_PTP_FILESYSTEM_TYPE_GENERIC_FLAT     0x0001
#define MGC_PTP_FILESYSTEM_TYPE_GENERIC_HIERARCH 0x0002
#define MGC_PTP_FILESYSTEM_TYPE_DCF              0x0003

#define MGC_PTP_ACCESS_READ_WRITE              0x0000
#define MGC_PTP_ACCESS_READ_ONLY_NO_DELETION   0x0001
#define MGC_PTP_ACCESS_READ_ONLY_WITH_DELETION 0x0002

/* ObjectFormat codes */
#define MGC_PTP_FORMAT_ASSOCIATION     0x3001
#define MGC_PTP_FORMAT_SCRIPT          0x3002
#define MGC_PTP_FORMAT_EXECUTABLE      0x3003
#define MGC_PTP_FORMAT_TEXT            0x3004
#define MGC_PTP_FORMAT_HTML            0x3005
#define MGC_PTP_FORMAT_DPOF            0x3006
#define MGC_PTP_FORMAT_AIFF            0x3007
#define MGC_PTP_FORMAT_WAV             0x3008
#define MGC_PTP_FORMAT_MP3             0x3009
#define MGC_PTP_FORMAT_AVI             0x300a
#define MGC_PTP_FORMAT_MPEG            0x300b
#define MGC_PTP_FORMAT_ASF             0x300c
#define MGC_PTP_FORMAT_EXIF_JPEG       0x3801
#define MGC_PTP_FORMAT_TIFF_EP         0x3802
#define MGC_PTP_FORMAT_FLASHPIX        0x3803
#define MGC_PTP_FORMAT_BMP             0x3804
#define MGC_PTP_FORMAT_CIFF            0x3805
#define MGC_PTP_FORMAT_GIF             0x3806
#define MGC_PTP_FORMAT_JFIF            0x3807
#define MGC_PTP_FORMAT_PCD             0x3808
#define MGC_PTP_FORMAT_PICT            0x3809
#define MGC_PTP_FORMAT_PNG             0x380b
#define MGC_PTP_FORMAT_TIFF            0x380d
#define MGC_PTP_FORMAT_TIFF_IT         0x380e
/** JPEG2000 baseline file format */
#define MGC_PTP_FORMAT_JP2             0x380f
/** JPEG2000 extended file format */
#define MGC_PTP_FORMAT_JPX             0x3810

/* Association codes */
#define MGC_PTP_ASSOC_GENERIC_FOLDER  0x0001
#define MGC_PTP_ASSOC_ALBUM           0x0002
#define MGC_PTP_ASSOC_TIME_SEQUENCE   0x0003 /* desc=DefaultPlaybackDelta */
#define MGC_PTP_ASSOC_HORIZ_PANORAMIC 0x0004
#define MGC_PTP_ASSOC_VERT_PANORAMIC  0x0005
#define MGC_PTP_ASSOC_2D_PANORAMIC    0x0006 /* desc=ImagesPerRow */
#define MGC_PTP_ASSOC_ANCILLARY_DATA  0x0007

/*
 * OperationRequest dataset:
 * UINT16  OperationCode
 * UINT32  SessionID
 * UINT32  TransactionID
 * UINT32  Parameter1
 * UINT32  Parameter2
 * UINT32  Parameter3
 * UINT32  Parameter4
 * UINT32  Parameter5
 */
#define MGC_PTP_REQUEST_PARAM_ALL 0xffffffffUL

/*
 * Response dataset:
 * UINT16  ResponseCode
 * UINT32  SessionID
 * UINT32  TransactionID
 * UINT32  Parameter1
 * UINT32  Parameter2
 * UINT32  Parameter3
 * UINT32  Parameter4
 * UINT32  Parameter5
 */

/*
 * Event dataset:
 * UINT16  EventCode
 * UINT32  SessionID
 * UINT32  TransactionID
 * UINT32  Parameter1
 * UINT32  Parameter2
 * UINT32  Parameter3
 */

#endif	/* multiple inclusion protection */
