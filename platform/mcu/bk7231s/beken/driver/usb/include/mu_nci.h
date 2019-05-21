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
 * MUSB-MicroSW NCI (Network Communications Interface).
 * $Revision: 1.1 $
 */

#ifndef __MUSB_NCI_H__
#define __MUSB_NCI_H__

/**
 * Introduction to Network Communications Interface.
 * The functions herein can be implemented by an OS to allow
 * the Mentor USB CDC/Ethernet driver to be hooked into the OS'
 * filesystem mechanisms, thus allowing the Mentor driver to be
 * used without additional effort.
 * The callbacks herein are implemented by the Mentor driver,
 * exposing the functionality needed by an OS to use the Ethernet device.
 * The above could also be applied to a custom, non-OS environment.
 * The driver is threadless (it reacts to callbacks from the BSR context
 * and the NCI implementation).  This eases its use in a non-OS environment,
 * and allows flexible and optimal NCI implementation for each OS.
 */

#include "mu_tools.h"

/************************* NCI CONSTANTS **************************/

#define MUSB_NCI_MAX_CONNECT_NAME 31

/**
 * MUSB_NciStatus.
 * NCI Operation Status.
 */
typedef enum
{
    /** success */
    MUSB_NCI_SUCCESS,
    /** error: transfer to/from device failed */
    MUSB_NCI_ERROR_TRANSFER,
    /** error: device busy */
    MUSB_NCI_ERROR_BUSY,
    /** error: device is no longer connected */
    MUSB_NCI_DEVICE_DISCONNECTED,
    /** error: out of memory */
    MUSB_NCI_NO_MEMORY
} MUSB_NciStatus;

/*************************** NCI TYPES ****************************/

/**
 * MUSB_NciConnectionInfo.
 */
typedef struct
{
    uint32_t dwUpstreamBitRate;
    uint32_t dwDownstreamBitRate;
    uint16_t awConnectionName[MUSB_NCI_MAX_CONNECT_NAME + 1];
    uint8_t bConnected;
} MUSB_NciConnectionInfo;

/**
 * MUSB_NciPortInfo.
 * Information about an NCI port (a physical connection to the network).
 * A CDC/Ethernet device driver fills this with any information
 * it knows before calling MUSB_NciAddPort.
 *
 * @field abMacAddress MAC address, MSB to LSB
 *
 * @field wVendorId USB VID,
 * for the NCI implementation's use in generating a volume name
 *
 * @field wProductId USB PID,
 * for the NCI implementation's use in generating a volume name
 *
 * @field bcdDevice USB bcdDevice,
 * for the NCI implementation's use in generating a volume name
 *
 * @field bBusAddress USB device address,
 * for the NCI implementation's use in generating a volume name
 */
typedef struct
{
    uint16_t wMaxSegmentSize;
    uint16_t wVendorId;
    uint16_t wProductId;
    uint16_t bcdDevice;
    uint8_t bBusAddress;
    uint8_t abMacAddress[6];
} MUSB_NciPortInfo;

/** Handle created by the NCI implementation */
typedef void *MUSB_NciClientHandle;

/** Port handle */
typedef void *MUSB_NciPortHandle;

/**
 * MUSB_NciRxBuffer.
 * Structure for sharing buffers between the CDC/Ethernet driver
 * and NCI implementation for supporting zero-copy Rx operations.
 * @field hClient client handle, not interpreted by the CDC/Ethernet driver
 * @field pBuffer buffer pointer
 * @field dwLength length of buffer in bytes
 * @field dwUsedSize how many bytes are currently used
 */
typedef struct
{
    void *hClient;
    uint8_t *pBuffer;
    uint32_t dwLength;
    uint32_t dwUsedSize;
} MUSB_NciRxBuffer;

/**
 * Send completion callback.
 * Sends are asynchronous requests, so the NCI implementation
 * supplies this callback to be notified on their completion.
 * @param hVolume a volume handle as filled by a successful MUSB_NciAddPort
 * @param wActualBytes the actual number of bytes successfully sent
 */
typedef void (*MUSB_pfNciSendComplete)(MUSB_NciClientHandle hClient,
                                       uint16_t wActualBytes);

/**
 * Send a packet.
 * @param hPort from associated MUSB_NciPortServices
 * @param pBuffer packet buffer, which must be valid until the
 * pfSendComplete callback is called - should contain Ethernet packet
 * but without CRC
 * @param wLength length of data to send in bytes
 * @param pfSendComplete send completion callback
 * @return status code; typically success, but if the device
 * got removed or unreadied somehow, an appropriate error should be returned
 * @see #MUSB_NciAddPort
 */
typedef MUSB_NciStatus (*MUSB_pfNciSendPacket)(MUSB_NciPortHandle hPort,
        uint8_t *pBuffer, uint16_t wLength, MUSB_pfNciSendComplete pfSendComplete);

/**
 * Loan a buffer to the CDC/Ethernet driver for receiving packets.
 * This allows for zero-copy operations.
 * The buffer may be used by the driver for several packets,
 * and cannot be considered completely owned by the implementation
 * until MUSB_NciReturnBuffer is called with a pointer to it.
 * If the driver does not have buffers loaned from the NCI implementation,
 * it will allocate its own buffers and Rx data must be copied
 * in MUSB_NciPortPacketReceived.
 * @param hPort from associated MUSB_NciPortServices
 * @param pBuffer pointer to a client-allocated buffer-sharing struct
 * @return MUSB_NCI_SUCCESS on success
 * @return MUSB_NCI_NO_MEMORY if the CDC/Ethernet driver could
 * not allocate memory necessary for managing the loaned buffer
 */
typedef MUSB_NciStatus (*MUSB_pfNciLoanRxBuffer)(MUSB_NciPortHandle hPort,
        MUSB_NciRxBuffer *pBuffer);

/**
 * Set enabled state.
 * @param hPort from associated MUSB_NciPortServices
 * @param bEnabled TRUE to enable port; FALSE to disable it
 * @return status code; typically success, but if the device
 * got removed or unreadied somehow, an appropriate error should be returned
 * @see #MUSB_NciAddPort
 */
typedef MUSB_NciStatus (*MUSB_pfNciSetEnabled)(MUSB_NciPortHandle hPort,
        uint8_t bEnabled);

/**
 * MUSB_NciPortServices.
 * Device driver callbacks (dispatch table).
 * @field hPort port handle to be passed to service functions
 * @field pfSendPacket packet-send service
 * @field pfLoanRxBuffer Rx buffer-loaning service
 * @field pfSetEnabled enable-state service
 */
typedef struct
{
    MUSB_NciPortHandle hPort;
    MUSB_pfNciSendPacket pfSendPacket;
    MUSB_pfNciLoanRxBuffer pfLoanRxBuffer;
    MUSB_pfNciSetEnabled pfSetEnabled;
} MUSB_NciPortServices;

typedef struct
{
    MUSB_NciPortInfo PortInfo;
    MUSB_NciConnectionInfo ConnectionInfo;
    MUSB_NciPortServices *pPortServices;
    uint8_t aLocalMacAddr[6];
    uint8_t aLocalIpAddr[4];
    uint8_t aTargetMacAddr[6];
    uint8_t aTargetIpAddr[4];
    uint16_t wNumber;
    uint8_t aBuffer[2048];
} MGC_TestNciPort;

/************************* NCI FUNCTIONS **************************/
/**
 * Add an NCI port.
 * A CDC/Ethernet device driver calls this to inform the NCI implementation
 * that a new port is available.
 * Typically, this happens when a USB CDC/Ethernet device is detected
 * and enumerated, and the CDC/Ethernet driver has determined that
 * it is ready for use.
 * The NCI implementation uses the port information to determine if
 * it can support the port, possibly assign a network interface name, and use it.
 * If the implementation wishes to support zero-copy operation,
 * it should call the pfLoanBuffer service function to provide
 * the driver with storage for incoming packets.
 * Error messages on failure are the NCI implementation's responsibility,
 * since the error messaging mechanism is OS-dependent.
 *
 * @param phClient where to store a client handle on success
 * @param pPortInfo pointer to port information struct
 * @param pConnectionInfo pointer to current connection information
 * @param pPortServices pointer to port services dispatch table
 * @return MUSB_NCI_SUCCESS on success; another code on failure
 */
extern MUSB_NciStatus MUSB_NciAddPort(MUSB_NciClientHandle *phClient,
                                      const MUSB_NciPortInfo *pPortInfo,
                                      const MUSB_NciConnectionInfo *pConnectionInfo,
                                      MUSB_NciPortServices *pPortServices);

/**
 * An NCI port is connected to its network.
 * A CDC/Ethernet device driver calls this to inform the NCI implementation
 * that the port has been connected to the network
 * @param hClient a client handle as filled by a successful MUSB_NciAddPort
 * @param pConnectionInfo pointer to connection information
 */
extern void MUSB_NciPortConnected(MUSB_NciClientHandle hClient,
                                  const MUSB_NciConnectionInfo *pConnectionInfo);

/**
 * An NCI port received a packet.
 * A CDC/Ethernet device driver calls this to inform the NCI implementation
 * that a packet has been received
 * @param hClient a client handle as filled by a successful MUSB_NciAddPort
 * @param pBuffer pointer to packet data
 * @param wLength length of packet in bytes
 * @param bMustCopy TRUE if the callee must copy the contents of
 * the buffer before returning;
 * FALSE if the buffer was loaned by the NCI implementation and
 * therefore this segment is once again owned by it
 */
extern void MUSB_NciPortPacketReceived(MUSB_NciClientHandle hClient,
                                       uint8_t *pBuffer, uint16_t wLength, uint8_t bMustCopy);

/**
 * The CDC/Ethernet driver is returning a loaned buffer.
 * @param hClient a client handle as filled by a successful MUSB_NciAddPort
 * @param pBuffer pointer to buffer-sharing struct
 */
extern void MUSB_NciReturnBuffer(MUSB_NciClientHandle hClient,
                                 MUSB_NciRxBuffer *pBuffer);

/**
 * An NCI port is disconnected from its network.
 * A CDC/Ethernet device driver calls this to inform the NCI implementation
 * that the port has been disconnected from the network
 * @param hClient a client handle as filled by a successful MUSB_NciAddPort
 */
extern void MUSB_NciPortDisconnected(MUSB_NciClientHandle hClient);

/**
 * An NCI port was removed.
 * A CDC/Ethernet device driver calls this to inform the NCI implementation
 * that a port is no longer available (e.g. the device has been removed).
 * @param hClient a client handle as filled by a successful MUSB_NciAddPort
 */
extern void MUSB_NciPortRemoved(MUSB_NciClientHandle hClient);

#endif	/* multiple inclusion protection */
