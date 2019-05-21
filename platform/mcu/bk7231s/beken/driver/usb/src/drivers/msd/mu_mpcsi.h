/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2005              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * MUSBStack-S mass-storage Protocol/Command-Set Interface (PCSI)
 * $Revision: 1.9 $
 */

#ifndef __MUSB_MSD_PCSI_H__
#define __MUSB_MSD_PCSI_H__

#include "mu_cdi.h"
#include "mu_hfi.h"

/**
 * Mass-storage Protocol/Command-Set Interface (MPCSI) introduction.
 * The MPCSI is the interface between the protocol (also called "transport")
 * and the command-set implementations.
 * In USB mass-storage devices, the protocol is given in the bInterfaceProtocol
 * field, and the command-set is given in the bInterfaceSubclass field.
 */

/************************** MPCSI TYPES ***************************/

/**
 * Command-complete callback.
 * Callback for command completion
 * @param pPrivateData private data
 * @param pDataBuffer associated data
 * @param dwDataLength length of data
 * @param bUsbError TRUE if a USB error occurred while obtaining
 * the data and/or status (STALL, NAK limit, packet too short)
 * @param bWrapperStatus wrapper status byte from wrapper
 * @param pStatusBuffer buffer of status (CB/CBI only)
 * @param wStatusLength length of status buffer
 */
typedef uint32_t (*MGC_pfMsdProtocolCmdComplete)(void *pPrivateData,
        const uint8_t *pDataBuffer,
        uint32_t dwDataLength,
        uint8_t bUsbError,
        uint8_t bWrapperStatus,
        const uint8_t *pStatusBuffer,
        uint16_t wStatusLength);

/**
 * Start a mass-storage device.
 * Implementations should perform any protocol-specific device initialization,
 * and eventually call the command-set's discovery function.
 * @param pProtocolData protocol handler instance data
 * @param pDevice UCDI device to start
 * @return TRUE on success
 * @return FALSE on failure
 * @see #MGC_pfMsdCmdSetDiscoverDevice
 */
typedef uint8_t (*MGC_pfMsdProtocolStartDevice)(void *pProtocolData,
        MUSB_Device *pDevice);

/**
 * Stop a mass-storage device.
 * Implementations should perform any protocol-specific device shutdown.
 * @param pProtocolData protocol handler instance data
 * @param pDevice UCDI device to stop
 * @return TRUE on success
 * @return FALSE on failure
 */
typedef uint8_t (*MGC_pfMsdProtocolStopDevice)(void *pProtocolData,
        MUSB_Device *pDevice);

/**
 * Send a mass-storage command.
 * Using the appropriate transport, send a command to the device
 * and call the callback when data and status is received.
 * @param pProtocolData protocol handler instance data
 * @param pPrivateData caller private data; passed in completion callback
 * @param bLun LUN
 * @param pCmdBuffer a buffer containing the command
 * @param bCmdLength the length of the command buffer
 * @param pDataBuffer data buffer
 * @param dwDataLength the length of data to send or max expected to recv
 * @param bSend TRUE to send data; FALSE to receive
 * @param pfCmdComplete completion callback
 * @param bAllowDma TRUE to allow DMA; FALSE otherwise
 * @param bDelay delay, in milliseconds, before sending command (0 implies no delay)
 * @return TRUE on successful queueing of the command
 * @return FALSE on failure
 */
typedef uint8_t (*MGC_pfMsdProtocolSendCmd)(void *pProtocolData,
        void *pPrivateData,
        uint8_t bLun,
        const uint8_t *pCmdBuffer,
        uint8_t bCmdLength,
        uint8_t *pDataBuffer,
        uint32_t dwDataLength,
        uint8_t bSend,
        MGC_pfMsdProtocolCmdComplete pfCmdComplete,
        uint8_t bAllowDma,
        uint8_t bDelay);

/**
 * Set device information.
 * The implementation will set the fields required from the USB device
 * (vendor/device ID, product version, bus address).
 * @param pProtocolData protocol handler instance data
 * @param bLun LUN
 * @param pInfo pointer to device information; callee makes a copy
 */
typedef uint8_t (*MGC_pfMsdProtocolSetDeviceInfo)(void *pProtocolData,
        uint8_t bLun,
        const MUSB_HfiDeviceInfo *pInfo);

/**
 * Set medium information.
 * @param pProtocolData protocol handler instance data
 * @param bLun LUN
 * @param pInfo pointer to medium information; callee makes a copy
 */
typedef uint8_t (*MGC_pfMsdProtocolSetMediumInfo)(void *pProtocolData,
        uint8_t bLun,
        const MUSB_HfiMediumInfo *pInfo);

/**
 * Set readiness.
 * This should be called with bIsReady=TRUE only after
 * the device info has been set, and if the medium is removable,
 * after the medium information is available.
 * It should be called with bIsReady=FALSE if a removable medium
 * is removed, during formatting, and any other time the device
 * becomes unexpectedly unable to process commands.
 * @param pProtocolData protocol handler instance data
 * @param bLun LUN
 * @param bIsReady TRUE if device/medium is ready to accept a new command;
 * FALSE otherwise
 */
typedef void (*MGC_pfMsdProtocolSetReady)(void *pProtocolData,
        uint8_t bLun,
        uint8_t bIsReady);

/**
 * Set format progress.
 * @param pProtocolData protocol handler instance data
 * @param bLun LUN
 * @param bProgress format progress in percentage
 */
typedef void (*MGC_pfMsdProtocolSetFormatProgress)(void *pProtocolData,
        uint8_t bLun,
        uint8_t bProgress);

/**
 * MGC_MsdProtocol.
 * A mass-storage protocol implementation (e.g. BOT, CB, CBI).
 */
typedef struct
{
    void *pProtocolData;
    MGC_pfMsdProtocolStartDevice pfStartDevice;
    MGC_pfMsdProtocolStopDevice pfStopDevice;
    MGC_pfMsdProtocolSendCmd pfSendCmd;
    MGC_pfMsdProtocolSetDeviceInfo pfSetDeviceInfo;
    MGC_pfMsdProtocolSetMediumInfo pfSetMediumInfo;
    MGC_pfMsdProtocolSetReady pfSetReady;
    MGC_pfMsdProtocolSetFormatProgress pfSetFormatProgress;
} MGC_MsdProtocol;

/**
 * Command-set-specific device discovery.
 * Using mechanisms specific to a device command set,
 * discover the device's capabilities.
 * It should accomplish this using the protocol's pfSendCmd,
 * and eventually result in a call to the protocol's pfSetDeviceInfo.
 * For example, a SCSI command set implementation would need to
 * issue commands like INQUIRY, READ_CAPACITY and/or
 * READ_FORMAT_CAPACITY, and parse the results.
 * @param pInstance instance pointer
 * @param pProtocol pointer to protocol handler
 * @param bLun LUN
 * @see #MGC_pfMsdProtocolSendCmd
 * @see #MGC_pfMsdProtocolSetDeviceInfo
 */
typedef uint8_t (*MGC_pfMsdCmdSetDiscoverDevice)(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun);

/**
 * Mount completion callback.
 * @param pPrivateData protocol handler private data
 * @param bSuccess TRUE on success; FALSE on failure
 */
typedef void (*MGC_pfMsdMountComplete)(void *pPrivateData, uint8_t bSuccess);

/**
 * Command-set-specific mount handling.
 * Using mechanisms specific to a device command set,
 * perform any operations prudent when a device is mounted
 * (e.g. disable manual eject).
 * If a command needs to be sent, it should first call
 * the protocol's pfSetReady(FALSE) to prevent queuing further
 * commands, then use pfSendCmd, then eventually call pfSetReady(TRUE)
 * if all goes well.
 * @param pInstance instance pointer
 * @param pfMountComplete mount completion callback
 * @param pPrivateData passed to pfMountComplete
 * @param pProtocol pointer to protocol handler
 * @param bLun LUN
 * @see #MGC_pfMsdProtocolSendCmd
 * @see #MGC_pfMsdProtocolSetReady
 */
typedef uint8_t (*MGC_pfMsdCmdSetMountDevice)(void *pInstance,
        MGC_pfMsdMountComplete pfMountComplete,
        void *pPrivateData,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun);

/**
 * Command-set-specific unmount handling.
 * Using mechanisms specific to a device command set,
 * perform any operations prudent when a device is unmounted
 * (e.g. enable manual eject).
 * If a command needs to be sent, it should first call
 * the protocol's pfSetReady(FALSE) to prevent queuing further
 * commands, then use pfSendCmd, then eventually call pfSetReady(TRUE)
 * if all goes well.
 * @param pInstance instance pointer
 * @param pProtocol pointer to protocol handler
 * @param bLun LUN
 * @see #MGC_pfMsdProtocolSendCmd
 * @see #MGC_pfMsdProtocolSetReady
 */
typedef uint8_t (*MGC_pfMsdCmdSetUnmountDevice)(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun);

/**
 * Command-set-specific read command generation.
 * Fill the given buffer with a command suitable to perform
 * a read under the given conditions.
 * @param pInstance instance pointer
 * @param pCmdBuffer buffer in which to store the command
 * @param bMaxCmdLength buffer length
 * @param dwBlockLo lower 32 bits of starting block
 * @param dwBlockHi upper 32 bits of starting block
 * @param wBlockCount how many blocks to read
 * @param bLun LUN
 * @return command length (the relevant portion of the command buffer)
 */
typedef uint8_t (*MGC_pfMsdCmdSetGetReadCmd)(void *pInstance,
        uint8_t *pCmdBuffer,
        uint8_t bMaxCmdLength,
        uint32_t dwBlockLo,
        uint32_t dwBlockHi,
        uint16_t wBlockCount,
        uint8_t bLun);

/**
 * Command-set-specific write command generation.
 * Fill the given buffer with a command suitable to perform
 * a write under the given conditions.
 * @param pInstance instance pointer
 * @param pCmdBuffer buffer in which to store the command
 * @param bMaxCmdLength buffer length
 * @param dwBlockLo lower 32 bits of starting block
 * @param dwBlockHi upper 32 bits of starting block
 * @param wBlockCount how many blocks to write
 * @param bLun LUN
 * @return command length (the relevant portion of the command buffer)
 */
typedef uint8_t (*MGC_pfMsdCmdSetGetWriteCmd)(void *pInstance,
        uint8_t *pCmdBuffer,
        uint8_t bMaxCmdLength,
        uint32_t dwBlockLo,
        uint32_t dwBlockHi,
        uint16_t wBlockCount,
        uint8_t bLun);

/**
 * Command-set-specific device flush.
 * Using mechanisms specific to a device command set,
 * cause the device to flush its cache (if any).
 * If a command needs to be sent, it should first call
 * the protocol's pfSetReady(FALSE) to prevent queuing further
 * commands, then use pfSendCmd, then eventually call pfSetReady(TRUE)
 * if all goes well.
 * @param pInstance instance pointer
 * @param pProtocol pointer to protocol handler
 * @param bLun LUN
 * @see #MGC_pfMsdProtocolSendCmd
 * @see #MGC_pfMsdProtocolSetReady
 */
typedef uint8_t (*MGC_pfMsdCmdSetFlushDevice)(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun);

/**
 * Command-set-specific device format.
 * Using mechanisms specific to a device command set,
 * cause the device to initiate a low-level format (if supported).
 * If a command needs to be sent, it should first call
 * the protocol's pfSetReady(FALSE) to prevent queuing further
 * commands, then use pfSendCmd, then eventually call pfSetReady(TRUE)
 * if all goes well.
 * @param pInstance instance pointer
 * @param pProtocol pointer to protocol handler
 * @param dwBlockSize block size to use
 * @param bLun LUN
 * @see #MGC_pfMsdProtocolSendCmd
 * @see #MGC_pfMsdProtocolSetReady
 */
typedef uint8_t (*MGC_pfMsdCmdSetFormatDevice)(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint32_t dwBlockSize,
        uint8_t bLun);

/**
 * Command-set-specific abort format.
 * @param pInstance instance pointer
 * @param pProtocol pointer to protocol handler
 * @param bLun LUN
 * @see #MGC_pfMsdProtocolSendCmd
 * @see #MGC_pfMsdProtocolSetReady
 */
typedef uint8_t (*MGC_pfMsdCmdSetAbortFormat)(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun);

/**
 * Command-set-specific interrupt parsing (CBIT only).
 */
typedef void (*MGC_pfMsdCmdSetParseInterrupt)(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        const uint8_t *pMessage,
        uint16_t wMessageLength,
        uint8_t bLun);

/**
 * Command-set-specific check medium presence.
 * @param pInstance instance pointer
 * @param pProtocol pointer to protocol handler
 * @param bLun LUN
 */
typedef uint8_t (*MGC_pfMsdCmdSetCheckMedium)(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun);

/**
 * Create a new command-set instance
 * @param bLunCount number of LUNs in device
 */
typedef void *(*MGC_pfMsdCmdSetCreateInstance)(uint8_t bLunCount);

/**
 * Destroy a command-set instance
 */
typedef void (*MGC_pfMsdCmdSetDestroyInstance)(void *pInstance);

/**
 * MGC_MsdCmdSet.
 * A mass-storage command set.
 * This allows the transport layer to use any supported command set
 * (e.g. SCSI emulation, RBC, MMC-2, etc.)
 */
typedef struct
{
    MGC_pfMsdCmdSetCreateInstance pfCreateInstance;
    MGC_pfMsdCmdSetDestroyInstance pfDestroyInstance;
    MGC_pfMsdCmdSetDiscoverDevice pfDiscoverDevice;
    MGC_pfMsdCmdSetMountDevice pfMountDevice;
    MGC_pfMsdCmdSetUnmountDevice pfUnmountDevice;
    MGC_pfMsdCmdSetGetReadCmd pfGetReadCmd;
    MGC_pfMsdCmdSetGetWriteCmd pfGetWriteCmd;
    MGC_pfMsdCmdSetFlushDevice pfFlushDevice;
    MGC_pfMsdCmdSetFormatDevice pfFormatDevice;
    MGC_pfMsdCmdSetAbortFormat pfAbortFormat;
    MGC_pfMsdCmdSetParseInterrupt pfParseInterrupt;
    MGC_pfMsdCmdSetCheckMedium pfCheckMedium;
} MGC_MsdCmdSet;

#endif	/* multiple inclusion protection */
