/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2007              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

/*
 * MUSB-MicroSW EDI (Ethernet Device Interface).
 * $Revision: 1.1 $
 */

#ifndef __MUSB_EDI_H__
#define __MUSB_EDI_H__

/**
 * Introduction to Ethernet Device Interface.
 * This is the interface between the device-specific code
 * for a CDC/Ethernet USB device and the USB-specific code.
 * The USB-specific code is supplied in src/examples/mu_ethfn.c,
 * while the device-specific code must be supplied by the device
 * designer.
 * The device designer implements functions with the signatures
 * defined herein and registers a struct containing function pointers.
 * Registration should be done at initialization time,
 * since a USB device must be prepared for use by the host
 * whenever it is connected.
 */

/************************* EDI CONSTANTS **************************/

/**
 * MUSB_EdiStatus.
 * EDI Operation Status.
 */
typedef enum
{
    /** success */
    MUSB_EDI_SUCCESS,
    /** error: USB transfer failed */
    MUSB_EDI_ERROR_TRANSFER,
    /** error: USB side busy */
    MUSB_EDI_ERROR_BUSY,
    /** error: device is no longer connected to host */
    MUSB_EDI_DEVICE_DISCONNECTED,
    /** error: device traffic is disabled */
    MUSB_EDI_DEVICE_DISABLED,
    /** error: out of memory */
    MUSB_EDI_NO_MEMORY
} MUSB_EdiStatus;

/*************************** EDI TYPES ****************************/

/**
 * MUSB_EdiRxBuffer.
 * Structure for sharing Rx buffers between the CDC/Ethernet USB
 * code and the device-specific code.
 * @field pBuffer buffer pointer
 * @field dwLength length of buffer in bytes
 * @field dwUsedSize how many bytes are currently used
 */
typedef struct
{
    unsigned char *pBuffer;
    unsigned long dwLength;
    unsigned long dwUsedSize;
} MUSB_EdiRxBuffer;

/**
 * An EDI device received a packet.
 * The USB-specific code calls this to inform the device-specific
 * code that a packet has been received
 * @param pBuffer pointer to packet data
 * @param wLength length of packet in bytes
 * @param bMustCopy TRUE if the callee must copy the contents of
 * the buffer before returning;
 * FALSE if the buffer was loaned, so the data is already in
 * client-owned memory
 */
typedef void (*MUSB_pfEdiPacketReceived)(unsigned char *pBuffer,
        unsigned short wLength, unsigned char bMustCopy);

/**
 * Send completion callback.
 * Sends are asynchronous requests, so the device-specific code
 * supplies this callback to be notified on their completion.
 * @param wActualBytes the actual number of bytes successfully sent
 * @see MUSB_EdiSendPacket
 */
typedef void (*MUSB_pfEdiSendComplete)(unsigned short wActualBytes);

/**
 * Set packet filtering.
 * The USB-specific code calls this function to request the device-specific
 * code to filter packets (if possible) according to the parameters.
 * For every combination of filter parameters supported,
 * the device-specific code should call MUSB_EdiSendPacket only
 * for the packets meeting the criteria.
 * Multiple parameters may be TRUE at the same time.
 * @param bDirected TRUE to accept packets directed to the device's MAC address;
 * FALSE otherwise
 * @param bBroadcast TRUE to accept all broadcast packets; FALSE otherwise
 * @param bMulticast TRUE to accept packets matching the multicast filters;
 * FALSE otherwise
 * @param bAllMulticast TRUE to accept all mulitcast packets; FALSE otherwise
 * @param bPromiscuous TRUE to accept all packets; FALSE otherwise
 * @see MUSB_EdiSendPacket
 */
typedef void (*MUSB_pfEdiSetPacketFilters)(
    unsigned char bDirected,
    unsigned char bBroadcast,
    unsigned char bMulticast,
    unsigned char bAllMulticast,
    unsigned char bPromiscuous);

/**
 * Return a loaned buffer to the device-specific code.
 * @param pBuffer pointer to a client-allocated buffer-sharing struct
 * @see MUSB_EdiLoanBuffer
 */
typedef void (*MUSB_pfEdiReturnRxBuffer)(MUSB_EdiRxBuffer *pBuffer);

/**
 * Set enabled state.
 * The USB-specific code calls this to inform the device-specific
 * code of a change to the enabled state of the USB interface.
 * MUSB_EdiSendPacket can only succeed when the USB interface is enabled.
 * @param bEnabled TRUE to enable port; FALSE to disable it
 * @see MUSB_EdiSendPacket
 */
typedef void (*MUSB_pfEdiSetEnabled)(unsigned char bEnabled);

/**
 * MUSB_EdiDeviceFunctions.
 * Device-specific functions (dispatch table).
 * @field pfEdiSetPacketFilters set-packet-filters function
 * @field pfEdiPacketReceived packet-received function
 * @field pfEdiSendComplete send-complete function
 * @field pfEdiReturnRxBuffer Rx loaned-buffer-return function
 * @field pfSetEnabled enable-state function
 */
typedef struct
{
    MUSB_pfEdiSetPacketFilters pfEdiSetPacketFilters;
    MUSB_pfEdiPacketReceived pfEdiPacketReceived;
    MUSB_pfEdiSendComplete pfEdiSendComplete;
    MUSB_pfEdiReturnRxBuffer pfEdiReturnRxBuffer;
    MUSB_pfEdiSetEnabled pfEdiSetEnabled;
} MUSB_EdiDeviceFunctions;

/************************* EDI FUNCTIONS **************************/

/**
 * Register device-specific functions
 * @param pFunctions pointer to function struct;
 * memory must be maintained by the client
 * @return operation status
 */
extern MUSB_EdiStatus MUSB_EdiRegister(MUSB_EdiDeviceFunctions *pFunctions);

/**
 * Set connection state.
 * The device-specific code calls this to inform the USB-specific
 * code of the current connection state.
 * @param bConnected TRUE if connected; FALSE otherwise
 * @return operation status
 */
extern MUSB_EdiStatus MUSB_EdiSetConnectionState(unsigned char bConnected);

/**
 * Set connection speed.
 * The device-specific code calls this to inform the USB-specific
 * code of the current connection state.
 * @param dwUpstreamBitRate upstream bit rate (bits/sec)
 * @param dwDownstreamBitRate downstream bit rate (bits/sec)
 * @return operation status
 */
extern MUSB_EdiStatus MUSB_EdiSetConnectionSpeed(
    unsigned long dwUpstreamBitRate,
    unsigned long dwDownstreamBitRate);

/**
 * The CDC/Ethernet device-specific code calls this to loan an Rx buffer
 * (which may be used for multiple packets) to the USB-specific code.
 * This allows for zero-copy operations.
 * The buffer may be used by the USB-specific code for several packets,
 * and cannot be considered completely owned by the device-specific code
 * until its buffer-return function is called with a pointer to it.
 * If the USB-specific code does not have buffers loaned to it,
 * it will use a single buffer and throttle using the USB NAK mechanism.
 * @param pBuffer pointer to buffer-sharing struct
 * @return operation status
 */
extern MUSB_EdiStatus MUSB_EdiLoanBuffer(MUSB_EdiRxBuffer *pBuffer);

/**
 * Send Ethernet packet.
 * The device-specific code calls this to send an Ethernet packet
 * to the USB host.
 * This should be called only for packets with valid checksum.
 * to the USB host.
 * @param pBuffer packet buffer
 * @param wLength packet length in bytes, not including checksum
 * @return operation status
 */
extern MUSB_EdiStatus MUSB_EdiSendPacket(const unsigned char *pBuffer,
        unsigned short wLength);

#endif	/* multiple inclusion protection */
