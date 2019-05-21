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
 * MUSB-MicroSW HFI (Host Filesystem Interface).
 * $Revision: 1.12 $
 */

#ifndef __MUSB_HFI_H__
#define __MUSB_HFI_H__

#include "mu_tools.h"

/**
 * Introduction to Host Filesystem Interface.
 * The functions herein can be implemented by an OS to allow
 * the Mentor USB mass-storage driver to be hooked into the OS'
 * filesystem mechanisms, thus allowing the Mentor driver to be
 * used without additional effort.
 * The callbacks herein are implemented by the Mentor driver,
 * exposing the functionality needed by an OS to use the mass-storage device.
 * The above could also be applied to a custom, non-OS environment.
 * The driver is threadless (it reacts to callbacks from the BSR context
 * and the HFI implementation).  This eases its use in a non-OS environment,
 * and allows flexible and optimal HFI implementation for each OS.
 */

/************************* HFI CONSTANTS **************************/

/** Maximum number of UNICODE characters supported for serial number */
#define MUSB_HFI_MAX_VOLUME_SERIAL	31

/** Maximum number of UNICODE characters supported for disk vendor ID */
#define MUSB_HFI_MAX_DISK_VENDOR	15

/** Maximum number of UNICODE characters supported for disk product ID */
#define MUSB_HFI_MAX_DISK_PRODUCT	31

/** Maximum number of UNICODE characters supported for disk revision ID */
#define MUSB_HFI_MAX_DISK_REVISION	7

/**
 * MUSB_HfiMediaType.
 * HFI Media Type.
 */
typedef enum
{
    /** Unknown media type */
    MUSB_HFI_MEDIA_UNKNOWN,
    /** Fixed media */
    MUSB_HFI_MEDIA_FIXED,
    /** Removable media */
    MUSB_HFI_MEDIA_REMOVABLE
} MUSB_HfiMediaType;

/**
 * MUSB_HfiAccessType.
 * HFI Access Type.
 */
typedef enum
{
    /** Random access read/write (e.g. hard disk, flash, floppy) */
    MUSB_HFI_ACCESS_RANDOM,
    /** Random access write-once (e.g. recordable CD/DVD) */
    MUSB_HFI_ACCESS_RANDOM_WRITE_ONCE,
    /** Random access read-only (e.g. CD/DVD) */
    MUSB_HFI_ACCESS_RANDOM_READ
} MUSB_HfiAccessType;

/**
 * MUSB_HfiStatus.
 * HFI Operation Status.
 */
typedef enum
{
    /** success */
    MUSB_HFI_SUCCESS,
    /** error: unsupported block size (try another if possible) */
    MUSB_HFI_ERROR_UNSUPPORTED_BLOCKSIZE,
    /** error: media is currently write-protected */
    MUSB_HFI_ERROR_MEDIA_PROTECTED,
    /** error: media was removed before operation could complete */
    MUSB_HFI_ERROR_MEDIA_REMOVED,
    /** error: device was removed before operation could complete */
    MUSB_HFI_ERROR_DEVICE_REMOVED,
    /** error: transfer to/from media failed */
    MUSB_HFI_ERROR_TRANSFER,
    /** error: data verify failed */
    MUSB_HFI_ERROR_VERIFY,
    /** error: device busy */
    MUSB_HFI_ERROR_BUSY,
    /** error: out of memory */
    MUSB_HFI_NO_MEMORY
} MUSB_HfiStatus;

/*************************** HFI TYPES ****************************/

/**
 * MUSB_HfiMediumInfo.
 * Storage medium information.
 * @field AccessType the medium's access type
 * @field dwBlockSize the fundamental medium block size, in bytes
 * @field dwBlockCountLo lower 32 bits of block count
 * @field dwBlockCountHi upper 32 bits of block count
 * @field awSerialNumber medium serial number string,
 * up to MUSB_HFI_MAX_VOLUME_SERIAL UNICODE characters,
 * terminated by wide-NUL
 */
typedef struct
{
    MUSB_HfiAccessType AccessType;
    uint32_t dwBlockSize;
    uint32_t dwBlockCountLo;
    uint32_t dwBlockCountHi;
    uint16_t awSerialNumber[MUSB_HFI_MAX_VOLUME_SERIAL + 1];
} MUSB_HfiMediumInfo;

/**
 * MUSB_HfiDeviceInfo.
 * Device information.
 * A mass-storage device driver fills this with any information
 * it knows before calling MUSB_HfiAddDevice.
 *
 * @field MediaType media type
 *
 * @field InitialMedium medium present at call time, if any
 * (for fixed-media devices, this is the permanent medium)
 *
 * @field bmAccessType bitmask representing possible access types
 * (1's shifted left by the MUSB_HfiAccessType values).
 * A fixed-media device should have just one,
 * while removable-media devices may support multiple types.
 *
 * @field bCanFormat TRUE if device can be (low-level) formatted
 *
 * @field bHasCache TRUE if device has a built-in cache,
 * so a flush may be required before removal for data integrity
 *
 * @field bSequential TRUE if device access is fundamentally sequential,
 * i.e. a tape drive
 *
 * @field dwBlockSize the fundamental device block size, in bytes
 *
 * @field awSerialNumber device serial number string,
 * up to MUSB_HFI_MAX_VOLUME_SERIAL UNICODE characters,
 * terminated by wide-NUL
 *
 * @field wVendorId USB VID,
 * for the HFI implementation's use in generating a volume name
 *
 * @field wProductId USB PID,
 * for the HFI implementation's use in generating a volume name
 *
 * @field bcdDevice USB bcdDevice,
 * for the HFI implementation's use in generating a volume name
 *
 * @field bBusAddress USB device address,
 * for the HFI implementation's use in generating a volume name
 *
 * @field awDiskVendor vendor string for the underlying disk,
 * up to MUSB_HFI_MAX_DISK_VENDOR UNICODE characters,
 * terminated by wide-NUL
 *
 * @field awDiskProduct product string for the underlying disk,
 * up to MUSB_HFI_MAX_DISK_PRODUCT UNICODE characters,
 * terminated by wide-NUL
 *
 * @field awDiskVendor revision string for the underlying disk,
 * up to MUSB_HFI_MAX_DISK_REVISION UNICODE characters,
 * terminated by wide-NUL
 *
 */
typedef struct
{
    MUSB_HfiMediaType MediaType;
    MUSB_HfiMediumInfo InitialMedium;
    uint16_t bmAccessType;
    uint8_t bCanFormat;
    uint8_t bHasCache;
    uint32_t dwBlockSize;
    uint16_t awSerialNumber[MUSB_HFI_MAX_VOLUME_SERIAL + 1];
    uint16_t wVendorId;
    uint16_t wProductId;
    uint16_t bcdDevice;
    uint8_t bBusAddress;
    uint8_t bPad[3];
    uint16_t awDiskVendor[MUSB_HFI_MAX_DISK_VENDOR + 1];
    uint16_t awDiskProduct[MUSB_HFI_MAX_DISK_PRODUCT + 1];
    uint16_t awDiskRevision[MUSB_HFI_MAX_DISK_REVISION + 1];
} MUSB_HfiDeviceInfo;

/** Handle created by the HFI implementation */
typedef void *MUSB_HfiVolumeHandle;

/**
 * Mount completion callback.
 * Mount is an asynchronous request, so the HFI implementation
 * supplies this callback to be notified on its completion.
 * @param hVolume a volume handle as filled by a successful MUSB_HfiAddDevice
 * @param bSuccess TRUE on success; FALSE on failure
 */
typedef void (*MUSB_pfHfiMountComplete)(MUSB_HfiVolumeHandle hVolume,
                                        uint8_t bSuccess);

/**
 * Transfer completion callback.
 * Read and write are asynchronous requests, so the HFI implementation
 * supplies this callback to be notified on their completion.
 * @param hVolume a volume handle as filled by a successful MUSB_HfiAddDevice
 * @param wActualBlocks the actual number of blocks successfully read
 * or written(/verified)
 */
typedef uint32_t (*MUSB_pfHfiTransferComplete)(MUSB_HfiVolumeHandle hVolume,
        uint16_t wActualBlocks);

/**
 * Medium-check completion callback.
 * Medium check is an asynchronous request, so the HFI implementation
 * supplies this callback to be notified on its completion.
 * @param hVolume a volume handle as filled by a successful MUSB_HfiAddDevice
 */
typedef void (*MUSB_pfHfiMediumCheckComplete)(MUSB_HfiVolumeHandle hVolume);

/**
 * The HFI implementation is mounting a volume on the device.
 * This is typically in response to a call to its MUSB_HfiAddDevice.
 * @param pDeviceData pPrivateData from associated MUSB_HfiDevice
 * @param pfMountComplete mount completion callback
 * @return status code; typically success, but if the device
 * got removed or unreadied somehow, an appropriate error should be returned
 * @see #MUSB_HfiAddDevice
 */
typedef MUSB_HfiStatus (*MUSB_pfHfiMountVolume)(void *pDeviceData,
        MUSB_pfHfiMountComplete pfMountComplete);

/**
 * Get information about the current medium.
 * @param pDeviceData pPrivateData from associated MUSB_HfiDevice
 * @param pMediumInfo where to store the medium information
 * @return status code
 */
typedef MUSB_HfiStatus (*MUSB_pfHfiGetMediumInfo)(void *pDeviceData,
        MUSB_HfiMediumInfo *pMediumInfo);

/**
 * Begin a read from a mass-storage device.
 * @param pDeviceData pPrivateData from associated MUSB_HfiDevice
 * @param dwStartBlockLo lower 32 bits of starting block
 * @param dwStartBlockHi upper 32 bits of starting block
 * @param wBlockCount how many consecutive blocks to read
 * @param pBuffer pointer to a buffer of sufficient size
 * to hold wBlockCount blocks in the device's dwBlockSize
 * @param pfTransferComplete callback for completion
 * @param bAllowDma TRUE to allow DMA; FALSE otherwise
 * @return status code
 */
typedef MUSB_HfiStatus (*MUSB_pfHfiReadDevice)(void *pDeviceData,
        uint32_t dwStartBlockLo, uint32_t dwStartBlockHi,
        uint16_t wBlockCount, uint8_t *pBuffer,
        MUSB_pfHfiTransferComplete pfTransferComplete,
        uint8_t bAllowDma);

/**
 * Begin a write to a mass-storage device.
 * @param pDeviceData pPrivateData from associated MUSB_HfiDevice
 * @param dwStartBlockLo lower 32 bits of starting block
 * @param dwStartBlockHi upper 32 bits of starting block
 * @param wBlockCount how many consecutive blocks to write
 * @param pBuffer pointer to a buffer of wBlockCount consecutive blocks
 * in the device's dwBlockSize
 * @param bVerify TRUE to verify each block after committing to
 * the actual media (so cache should be bypassed if possible)
 * @param pwActualBlocks where to store the number of blocks successfully written
 * @param bAllowDma TRUE to allow DMA; FALSE otherwise
 * @return status code
 */
typedef MUSB_HfiStatus (*MUSB_pfHfiWriteDevice)(void *pDeviceData,
        uint32_t dwStartBlockLo, uint32_t dwStartBlockHi,
        uint16_t wBlockCount, uint8_t *pBuffer, uint8_t bVerify,
        MUSB_pfHfiTransferComplete pfTransferComplete, uint8_t bAllowDma);

/**
 * Flush the device's built-in cache, if any
 * @param pDeviceData pPrivateData from associated MUSB_HfiDevice
 * @return status code
 */
typedef MUSB_HfiStatus (*MUSB_pfHfiFlushDevice)(void *pDeviceData);

/**
 * Begin formatting the medium.
 * If formatting is synchronous, this call will not return
 * until formatting is complete.
 * Otherwise, it should return after requesting the format to begin
 * and preparing to receive and record progress indications from the device.
 * @param pDeviceData pPrivateData from associated MUSB_HfiDevice
 * @param dwBlockSize the desired medium block size
 * @param pbSuccess where to store TRUE on success or FALSE on failure,
 * when formatting is complete or aborted
 * @return status code
 */
typedef MUSB_HfiStatus (*MUSB_pfHfiFormatMedium)(void *pDeviceData,
        uint32_t dwBlockSize, uint8_t *pbSuccess);

/**
 * Get the progress of the current formatting operation.
 * If formatting is synchronous, this function should return 100.
 * If formatting is asynchronous, this function should use the
 * appropriate means to return a percentage-complete indication.
 * @param pDeviceData pPrivateData from associated MUSB_HfiDevice
 * @return percentage of formatting complete (0-100)
 */
typedef uint8_t (*MUSB_pfHfiGetFormatProgress)(void *pDeviceData);

/**
 * Abort the current formatting operation.
 * @param pDeviceData pPrivateData from associated MUSB_HfiDevice
 * @return status code
 */
typedef MUSB_HfiStatus (*MUSB_pfHfiAbortFormat)(void *pDeviceData);

/**
 * The HFI implementation is unmounting a volume on the device.
 * This may be for implementation-specific reasons,
 * and may be in response to a call to its MUSB_HfiVolumeRemoved.
 * @param pDeviceData pPrivateData from associated MUSB_HfiDevice
 * @return status code
 * @see #MUSB_HfiAddDevice
 */
typedef MUSB_HfiStatus (*MUSB_pfHfiUnmountVolume)(void *pDeviceData);

/**
 * This function checks for the presence of a medium in the given device.
 * The return value does not indicate medium presence;
 * a change in medium presence will be indicated by a call
 * to MUSB_HfiMediumInserted or MUSB_HfiMediumRemoved, as appropriate.
 * The purpose of this function is to provide the threadless mass-storage
 * class driver an opportunity to perform this check.
 * Its intended use is for a media-polling thread to call periodically,
 * if the application wishes to handle medium insertion/removal at any time.
 * @param pDeviceData pPrivateData from associated MUSB_HfiDevice
 * @return status code
 * @see #MUSB_HfiMediumInserted
 * @see #MUSB_HfiMediumRemoved
 * @see #MUSB_HfiAddDevice
 */
typedef MUSB_HfiStatus (*MUSB_pfHfiCheckMedium)(void *pDeviceData);

/**
 * Acts as MUSB_pfHfiCheckMedium, calling the given callback when
 * the medium-check operation is complete
 * @param pDeviceData pPrivateData from associated MUSB_HfiDevice
 * @param pfMediumCheckComplete completion handler for medium-check
 * @return status code
 * @see #MUSB_pfHfiCheckMedium
 */
typedef MUSB_HfiStatus (*MUSB_pfHfiCheckMediumNotify)(void *pDeviceData,
        MUSB_pfHfiMediumCheckComplete pfMediumCheckComplete);

/**
 * MUSB_HfiDevice.
 * Device driver callbacks (dispatch table).
 * @field pPrivateData device driver private data;
 * not to be interpreted by the HFI implementation
 * @field pfMountVolume volume-mount service
 * @field pfGetMediumInfo device medium-info service
 * @field pfReadDevice device read service
 * @field pfWriteDevice device write service
 * @field pfFlushDevice device flush service
 * @field pfFormatMedium device format-start service
 * @field pfGetFormatProgress device format-progress service
 * @field pfAbortFormat device format-abort service
 * @field pfUnmountVolume volume-unmount service
 * @field pfCheckMedim medium-presence-check service
 * @field pfHfiCheckMediumNotify medium-presence-check service
 * with completion notification
 */
typedef struct
{
    void *pPrivateData;
    MUSB_pfHfiMountVolume pfMountVolume;
    MUSB_pfHfiGetMediumInfo pfGetMediumInfo;
    MUSB_pfHfiReadDevice pfReadDevice;
    MUSB_pfHfiWriteDevice pfWriteDevice;
    MUSB_pfHfiFlushDevice pfFlushDevice;
    MUSB_pfHfiFormatMedium pfFormatMedium;
    MUSB_pfHfiGetFormatProgress pfGetFormatProgress;
    MUSB_pfHfiAbortFormat pfAbortFormat;
    MUSB_pfHfiUnmountVolume pfUnmountVolume;
    MUSB_pfHfiCheckMedium pfCheckMedium;
    MUSB_pfHfiCheckMediumNotify pfCheckMediumNotify;
} MUSB_HfiDevice;

/************************* HFI FUNCTIONS **************************/

/**
 * Add a mass-storage device.
 * A mass-storage device driver calls this to inform the HFI implementation
 * that a new device is available.
 * Typically, this happens when a USB mass-storage device is detected
 * and enumerated, and the mass-storage driver has determined that
 * it is ready for use.
 * The HFI implementation uses the device information to determine if
 * it can support the device, assign a volume name, and mount it.
 * Error messages on failure are the HFI implementation's responsibility,
 * since the error messaging mechanism is OS-dependent.
 *
 * @param phVolume where to store a volume handle on success
 * @param pInfo pointer to device information struct
 * @param pDevice pointer to struct with callbacks to access the device
 * @return MUSB_HFI_SUCCESS on success
 * @return MUSB_HFI_ERROR_UNSUPPORTED_BLOCKSIZE if the block size
 * is not supported; the device driver can try a different block
 * size if the underlying device supports multiple sizes
 */
extern MUSB_HfiStatus MUSB_HfiAddDevice(MUSB_HfiVolumeHandle *phVolume,
                                        const MUSB_HfiDeviceInfo *pInfo, MUSB_HfiDevice *pDevice);

/**
 * A mass-storage medium was inserted.
 * A mass-storage device driver calls this to inform the HFI implementation
 * that the device corresponding to a volume has had media inserted
 * @param hVolume a volume handle as filled by a successful MUSB_HfiAddDevice
 */
extern void MUSB_HfiMediumInserted(MUSB_HfiVolumeHandle hVolume,
                                   const MUSB_HfiMediumInfo *pMediumInfo);

/**
 * A mass-storage medium was removed.
 * A mass-storage device driver calls this to inform the HFI implementation
 * that the medium in the device corresponding to a volume has been removed
 * @param hVolume a volume handle as filled by a successful MUSB_HfiAddDevice
 */
extern void MUSB_HfiMediumRemoved(MUSB_HfiVolumeHandle hVolume);

/**
 * A mass-storage device was removed.
 * A mass-storage device driver calls this to inform the HFI implementation
 * that the device corresponding to a volume has been removed.
 * @param hVolume a volume handle as filled by a successful MUSB_HfiAddDevice
 */
extern void MUSB_HfiDeviceRemoved(void);

#endif	/* multiple inclusion protection */
