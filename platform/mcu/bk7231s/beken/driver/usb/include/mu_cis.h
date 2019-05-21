/*****************************************************************************
 *                                                                           *
 *      Copyright Mentor Graphics Corporation 2007                           *
 *                                                                           *
 *                All Rights Reserved.                                       *
 *                                                                           *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION            *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS              *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                               *
 *                                                                           *
 ****************************************************************************/

/*
 * MUSB-MicroSW interface for special HID (Human Interface Device)
 * functionality for devices supporting Communication Interface
 * Specification (CIS) version 2.0.
 * $Revision: 1.6 $
 */

#ifndef __MUSB_CIS_H__
#define __MUSB_CIS_H__

/**
 * Introduction to Communication Interface Specification (CIS) support.
 * The interface herein allows a client application adhering to
 * CIS version 2.0 to communicate with CIS version 2.0 devices on USB.
 * The client first "registers" itself using MUSB_CisRegisterClient,
 * passing a pointer to a struct containing notification function pointers.
 * The client can "unregister" itself at any time.
 * While registered, the client will receive notifications of:
 * <ul>
 * <li>Addition of CIS version 2.0 devices on the USB -
 * the client can decide, based upon passed device information,
 * whether or not to handle each device
 * <li>Removal of CIS devices on USB
 * <li>Input data from devices the client has opted to handle
 * </ul>
 * While handling a device, the client can send commands to it
 * using MUSB_CisSendCommand or MUSB_CisSendRawMessage.
 * @see MUSB_CisRegisterClient
 * @see MUSB_CisUnregisterClient
 * @see MUSB_CisSendCommand
 * @see MUSB_CisSendRawMessage
 */

/*************************** CONSTANTS ****************************/

/**
 * MUSB_CisStatus.
 * CIS Operation Status.
 */
typedef enum
{
    /** success */
    MUSB_CIS_SUCCESS,
    /** device busy */
    MUSB_CIS_BUSY,
    /** error: device characteristics not supported */
    MUSB_CIS_ERROR_UNSUPPORTED_DEVICE,
    /** error: device was removed before operation could complete */
    MUSB_CIS_ERROR_DEVICE_REMOVED,
    /** error: transfer to/from device failed */
    MUSB_CIS_ERROR_TRANSFER,
    /** error: request too large */
    MUSB_CIS_ERROR_REQ_TOO_LARGE
} MUSB_CisStatus;

/***************************** TYPES ******************************/

/**
 * MUSB_CisInfo.
 * Information about CIS capabilities
 * @field wSize size, in bytes
 * @field bId ID
 */
typedef struct
{
    uint16_t wSize;
    uint8_t bId;
} MUSB_CisInfo;

/**
 * MUSB_CisDeviceInfo.
 * Device information.
 * A generic HID driver fills this with any information
 * it knows before calling the CIS client's registered
 * device-add handler.
 *
 * @field pImplData pointer to implementation data;
 * for passing to request functions
 *
 * @field aCommandInfo array of command information
 * @field aResponseInfo array of response information
 * @field wCommandInfoLength how many elements are in aCommandInfo
 * @field wResponseInfoLength how many elements are in aResponseInfo
 *
 * @field wLanguage USB language ID
 *
 * @field wInputMax maximum possible input data size
 *
 * @field wVendorId USB VID,
 * for the CIS client's information (e.g. for generating a device name)
 *
 * @field wProductId USB PID,
 * for the CIS client's information (e.g. for generating a device name)
 *
 * @field bcdDevice USB bcdDevice,
 * for the CIS client's information (e.g. for generating a device name)
 *
 * @field bBusAddress USB device address,
 * for the CIS client's information (e.g. for generating a device name)
 */
typedef struct
{
    void *pImplData;
    MUSB_CisInfo *aCommandInfo;
    MUSB_CisInfo *aResponseInfo;
    unsigned short wCommandInfoLength;
    unsigned short wResponseInfoLength;
    unsigned short wInputMax;
    unsigned short wLanguage;
    unsigned short wVendorId;
    unsigned short wProductId;
    unsigned short bcdDevice;
    unsigned char bBusAddress;
} MUSB_CisDeviceInfo;

/**
 * MUSB_CisClientHandle.
 * A client "handle" - an opaque pointer to client device-instance data
 */
typedef void *MUSB_CisClientHandle;

/**
 * Callback to a CIS client to notify it of a new CIS device
 * becoming available - the client should return a "handle"
 * which is passed to the client's other handlers
 * @param pDeviceInfo pointer to device information struct
 * @return NULL if the client does not wish to handle this device
 * @return a non-NULL "handle" if the client will handle this
 * device; this handle is passed to the client's other handlers
 */
typedef MUSB_CisClientHandle (*MUSB_pfCisDeviceAddHandler)(
    const MUSB_CisDeviceInfo *pDeviceInfo);

/**
 * Callback to a CIS client to notify it of CIS device removal
 * @param hClient the "handle" returned by the client's device-add handler
 * for this device
 * @param pDeviceInfo pointer to device information struct
 */
typedef void (*MUSB_pfCisDeviceRemoveHandler)(
    MUSB_CisClientHandle hClient,
    const MUSB_CisDeviceInfo *pDeviceInfo);

/**
 * Callback to a CIS client to notify it of new input from the device.
 * The handler is called only for the devices "accepted" by the client
 * (when the client returns a non-NULL handle in its device-add handler).
 * @param hClient the "handle" returned by the client's device-add handler
 * for this device
 * @param pInput input data
 * @param wLength input data length, in bytes
 * @param bChecksumOk FALSE if any packet's checksum failed; TRUE otherwise
 * @param bStatus computed status
 */
typedef void (*MUSB_pfCisDeviceInputHandler)(
    MUSB_CisClientHandle hClient,
    const unsigned char *pInput,
    unsigned short wLength,
    unsigned char bChecksumOk,
    MUSB_CisStatus bStatus);

/**
 * Callback to a CIS client to notify it that its last output request
 * is complete.
 * The handler is called only for the devices "accepted" by the client
 * (when the client returns a non-NULL handle in its device-add handler).
 * @param hClient the "handle" returned by the client's device-add handler
 * for this device
 * @param bStatus status of output request
 */
typedef void (*MUSB_pfCisOutputCompleteHandler)(
    MUSB_CisClientHandle hClient,
    MUSB_CisStatus bStatus);
/**
 * MUSB_CisClient.
 * A CIS client
 * @field pfDeviceAddHandler device-add handler
 * @field pfDeviceRemoveHandler device-remove handler
 * @field pfDeviceInputHandler device-input handler
 * @field pfCisOutputCompleteHandler output-complete handler
 */
typedef struct
{
    MUSB_pfCisDeviceAddHandler pfDeviceAddHandler;
    MUSB_pfCisDeviceRemoveHandler pfDeviceRemoveHandler;
    MUSB_pfCisDeviceInputHandler pfDeviceInputHandler;
    MUSB_pfCisOutputCompleteHandler pfOutputCompleteHandler;
} MUSB_CisClient;

/*************************** FUNCTIONS ****************************/

/**
 * Register the CIS client
 * @param pClient pointer to client struct
 */
extern void MUSB_CisRegisterClient(MUSB_CisClient *pClient);

/**
 * Unregister any known CIS client
 */
extern void MUSB_CisUnregisterClient(void);

/**
 * Send a CIS command to a supported USB device
 * @param pImplData implementation data pointer from device-info struct
 * @param pCommand pointer to command data
 * @param wCmdLength length of command
 */
extern MUSB_CisStatus MUSB_CisSendCommand(
    void *pImplData,
    const unsigned char *pCommand,
    unsigned short wCmdLength);

/**
 * Send a raw CIS message to a supported USB device.
 * Use this function if the message contains multiple commands.
 * This function packetizes the message as per the HID
 * requirements (it determines a suitable HID report ID,
 * and packetizes it with appropriate Link Control Bytes [LCBs]
 * and zero-padding as necessary).
 * This means the caller's message must include the device ID,
 * followed by one or more set of {payload length, payload, checksum}.
 * @param pImplData implementation data pointer from device-info struct
 * @param pMessage pointer to message data
 * @param wMsgLength length of message
 * @param bEnd TRUE when this is the last message in a command
 */
extern MUSB_CisStatus MUSB_CisSendRawMessage(
    void *pImplData,
    const unsigned char *pMessage,
    unsigned short wMsgLength,
    unsigned char bEnd);

#endif	/* multiple inclusion protection */
