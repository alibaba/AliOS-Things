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
 * MUSB-MicroSW Image Transfer Interface (ITI).
 * $Revision: 1.4 $
 */

#ifndef __MUSB_ITI_H__
#define __MUSB_ITI_H__

/**
 * Introduction to Image Transfer Interface.
 * The functions herein can be implemented by an OS to allow
 * the Mentor USB PTP/DSC/Pict-Bridge driver to be hooked into the OS'
 * mechanisms, thus allowing the Mentor driver to be
 * used without additional effort.
 * The callbacks herein are implemented by the Mentor driver,
 * exposing the functionality needed by an OS to use the device.
 * The above could also be applied to a custom, non-OS environment.
 * Finally, an implementation can be created solely for demonstration purposes
 * (to avoid relying on platforms with live media support,
 * and/or to allow the Mentor USB driver to be tested).
 */

#include "mu_tools.h"

/************************* ITI CONSTANTS **************************/

/** Maximum number of Unicode characters returned in folder names */
#define MUSB_ITI_MAX_FOLDER_NAME  31
/** Maximum number of Unicode characters returned in image names */
#define MUSB_ITI_MAX_IMAGE_NAME   31

/**
 * MUSB_ItiDeviceType.
 * ITI Device Type.
 */
typedef enum
{
    /** Unknown device type */
    MUSB_ITI_DEVICE_UNKNOWN,
    /** Picture source */
    MUSB_ITI_DEVICE_PICTURE_SOURCE,
    /** Picture sink */
    MUSB_ITI_DEVICE_PICTURE_SINK
} MUSB_ItiDeviceType;

/**
 * MUSB_ItiFormat.
 * ITI format.
 */
typedef enum
{
    /** Format not enumerated; use GetName for presentation */
    MUSB_ITI_FORMAT_UNKNOWN,
    /** EXIF/JPEG */
    MUSB_ITI_FORMAT_EXIF_JPEG,
    /** Tag Image File Format for Electronic Photography */
    MUSB_ITI_FORMAT_TIFF_EP,
    /** FlashPix */
    MUSB_ITI_FORMAT_FLASHPIX,
    /** Microsoft Windows BMP */
    MUSB_ITI_FORMAT_BMP,
    /** Canon Camera Image File Format */
    MUSB_ITI_FORMAT_CIFF,
    /** GIF */
    MUSB_ITI_FORMAT_GIF,
    /** JFIF */
    MUSB_ITI_FORMAT_JFIF,
    /** PhotoCD image pac */
    MUSB_ITI_FORMAT_PCD,
    /** QuickDraw */
    MUSB_ITI_FORMAT_PICT,
    /** Portable Network Graphics */
    MUSB_ITI_FORMAT_PNG,
    /** Tag Image File Format */
    MUSB_ITI_FORMAT_TIFF,
    /** Tag Image File Format for Information Technology */
    MUSB_ITI_FORMAT_TIFF_IT,
    /** JPEG2000 Baseline File Format */
    MUSB_ITI_FORMAT_JP2,
    /** JPEG2000 Extended File Format */
    MUSB_ITI_FORMAT_JPX,
    /** Text */
    MUSB_ITI_FORMAT_TEXT,
    /** HyperText Markup Language */
    MUSB_ITI_FORMAT_HTML,
    /** Digital Print Order Format */
    MUSB_ITI_FORMAT_DPOF,
    /** AIFF (audio) */
    MUSB_ITI_FORMAT_AIFF,
    /** WAV (audio) */
    MUSB_ITI_FORMAT_WAV,
    /** MP3 (audio) */
    MUSB_ITI_FORMAT_MP3,
    /** AVI (video clip) */
    MUSB_ITI_FORMAT_AVI,
    /** MPEG (video clip) */
    MUSB_ITI_FORMAT_MPEG,
    /** Microsoft Advanced Streaming Format (video) */
    MUSB_ITI_FORMAT_ASF,
    /** Device-model-specific script */
    MUSB_ITI_FORMAT_SCRIPT,
    /** Device-model-specific binary executable */
    MUSB_ITI_FORMAT_EXE
} MUSB_ItiFormat;

/**
 * MUSB_ItiStatus.
 * ITI Operation Status.
 */
typedef enum
{
    /** success */
    MUSB_ITI_SUCCESS,
    /** error: device was removed before operation could complete */
    MUSB_ITI_ERROR_DEVICE_REMOVED,
    /** error: transfer to/from device failed */
    MUSB_ITI_ERROR_TRANSFER,
    /** error: device busy */
    MUSB_ITI_ERROR_BUSY
} MUSB_ItiStatus;

/*************************** ITI TYPES ****************************/

/** Handle created by the ITI client */
typedef void *MUSB_ItiClientHandle;

/** Handle to an ITI device */
typedef void *MUSB_ItiDeviceHandle;

typedef struct
{
    uint16_t wYear;
    uint8_t bMonth;
    uint8_t bDay;
    uint8_t bHour;
    uint8_t bMinute;
    uint8_t bSecond;
} MUSB_ItiDateTime;

/**
 * MUSB_ItiDimensions.
 * image dimension information
 * @field dwWidth width in pixels
 * @field dwHeight height height in pixels
 * @field dwBitDepth the number of bits required to represent each pixel
 */
typedef struct
{
    uint32_t dwWidth;
    uint32_t dwHeight;
    uint32_t dwBitDepth;
} MUSB_ItiDimensions;

/**
 * MUSB_ItiFolderInfo.
 * Information about a folder
 * @field dwHandle folder handle
 * @field wFolderCount how many child folders this folder contains
 * @field wImageCount how many images are in this folder
 * @field awName folder name as an array of up to MUSB_ITI_MAX_FOLDER_NAME
 * Unicode characters, terminated with Unicode null
 */
typedef struct
{
    uint32_t dwHandle;
    uint16_t wFolderCount;
    uint16_t wImageCount;
    uint16_t awName[MUSB_ITI_MAX_FOLDER_NAME + 1];
} MUSB_ItiFolderInfo;

/**
 * MUSB_ItiImageInfo.
 * Information about an image
 * @field dwHandle folder handle
 * @field dwCompressedSize size (in bytes) of compressed image
 * (this is the size required to get the image)
 * @field CreationTime creation date/time
 * @field ModificationTime modification date/time
 * @field Dimensions image dimension information
 * @field awName image name as an array of up to MUSB_ITI_MAX_IMAGE_NAME
 * Unicode characters, terminated with Unicode null
 * @field Format image format
 */
typedef struct
{
    uint32_t dwHandle;
    uint32_t dwCompressedSize;
    MUSB_ItiDateTime CreationTime;
    MUSB_ItiDateTime ModificationTime;
    MUSB_ItiDimensions Dimensions;
    uint16_t awName[MUSB_ITI_MAX_IMAGE_NAME + 1];
    MUSB_ItiFormat Format;
} MUSB_ItiImageInfo;

/**
 * @field hDevice handle to pass to service functions
 * @field awVendorName
 * @field aImageFormats
 * @field wType bitmask formed by shifting 1's left by MUSB_ItiDeviceType values
 * @field wVendorId USB vendor ID, for client's use in generating a device name
 * @field wProductId USB product ID, for client's use in generating a device name
 * @field bcdDevice USB revision, for client's use in generating a device name
 * @field bBusAddress USB device address, for client's use in generating a device name
 */
typedef struct
{
    MUSB_ItiDeviceHandle hDevice;
    const uint16_t *awVendorName;
    const uint16_t *awModelName;
    const uint16_t *awVersionName;
    const uint16_t *awSerialNumber;
    const MUSB_ItiFormat *aCaptureFormats;
    const MUSB_ItiFormat *aImageFormats;
    uint16_t wType;
    uint16_t wVendorId;
    uint16_t wProductId;
    uint16_t bcdDevice;
    uint8_t bBusAddress;
} MUSB_ItiDeviceInfo;

/**
 * Callback to an ITI client to notify it of a new ITI device
 * becoming available - the client should return a "handle"
 * which is passed to the client's other handlers
 * @param pDeviceInfo pointer to device information struct
 * @return NULL if the client does not wish to handle this device
 * @return a non-NULL "handle" if the client will handle this
 * device; this handle is passed to the client's other handlers
 */
typedef MUSB_ItiClientHandle (*MUSB_pfItiDeviceAddHandler)(
    const MUSB_ItiDeviceInfo *pDeviceInfo);

/**
 * Callback to an ITI client to notify it of ITI device removal
 * @param hClient the "handle" returned by the client's device-add handler
 * for this device
 * @param pDeviceInfo pointer to device information struct
 */
typedef void (*MUSB_pfItiDeviceRemoveHandler)(
    MUSB_ItiClientHandle hClient,
    const MUSB_ItiDeviceInfo *pDeviceInfo);

/**
 * MUSB_ItiClient.
 * An ITI client
 * @field pfDeviceAddHandler device-add handler
 * @field pfDeviceRemoveHandler device-remove handler
 */
typedef struct
{
    MUSB_pfItiDeviceAddHandler pfDeviceAddHandler;
    MUSB_pfItiDeviceRemoveHandler pfDeviceRemoveHandler;
} MUSB_ItiClient;

/************************* ITI FUNCTIONS **************************/

/**
 * Register the ITI client
 * @param pClient pointer to client struct
 */
extern void MUSB_ItiRegisterClient(MUSB_ItiClient *pClient);

/**
 * Unregister any known ITI client
 */
extern void MUSB_ItiUnregisterClient(void);

/**
 * Get the number of top-level folders
 * @param hDevice device handle from info passed to device-add handler
 * @param pwCount pointer to storage for the number of top-level folders
 * @return status code
 */
extern MUSB_ItiStatus MUSB_CountTopFolders(
    MUSB_ItiDeviceHandle hDevice, uint16_t *pwCount);

/**
 * Get the number of top-level images
 * @param hDevice device handle from info passed to device-add handler
 * @param pwCount pointer to storage for the number of top-level images
 * @return status code
 */
extern MUSB_ItiStatus MUSB_CountTopImages(
    MUSB_ItiDeviceHandle hDevice, uint16_t *pwCount);

/**
 * Get one or more folder information entries
 * @param hDevice device handle from info passed to device-add handler
 * @param pParent pointer to parent folder's information,
 * or NULL to get top-level folders
 * @param wMaxInfoCount the maximum number of entries to fill in aInfo
 * @param aInfo a client-allocated buffer for folder information entries
 * @param pwActualInfoCount pointer to storage for the actual number
 * of entries stored in aInfo
 * @return status code
 */
extern MUSB_ItiStatus MUSB_GetFolderInfo(
    MUSB_ItiDeviceHandle hDevice,
    const MUSB_ItiFolderInfo *pParent,
    uint16_t wMaxInfoCount,
    MUSB_ItiFolderInfo *aInfo,
    uint16_t *pwActualInfoCount);

/**
 * Get one or more image information entries
 * @param hDevice device handle from info passed to device-add handler
 * @param pParent pointer to parent folder's information,
 * or NULL to get top-level images
 * @param wMaxInfoCount the maximum number of entries to fill in aInfo
 * @param aInfo a client-allocated buffer for image information entries
 * @param pwActualInfoCount pointer to storage for the actual number
 * of entries stored in aInfo
 * @return status code
 */
extern MUSB_ItiStatus MUSB_GetImageInfo(
    MUSB_ItiDeviceHandle hDevice,
    const MUSB_ItiFolderInfo *pParent,
    uint16_t wMaxInfoCount,
    MUSB_ItiImageInfo *aInfo,
    uint16_t *pwActualInfoCount);

/**
 * Get an image
 * @param hDevice device handle from info passed to device-add handler
 * @param dwHandle image handle
 * @param Format requested format
 * @param pBuffer data buffer
 * @param dwBufferSize maximum number of bytes to store in pBuffer
 * @return status code
 */
extern MUSB_ItiStatus MUSB_ItiGetImage(
    MUSB_ItiDeviceHandle hDevice,
    uint32_t dwHandle, MUSB_ItiFormat Format,
    uint8_t *pBuffer, uint32_t dwBufferSize);

#endif	/* multiple inclusion protection */
