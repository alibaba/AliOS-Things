/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @brief
 *  This file defines the OpenThread UDP API.
 *
 */

#ifndef OPENTHREAD_UDP_H_
#define OPENTHREAD_UDP_H_

#include <openthread/ip6.h>
#include <openthread/message.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-udp
 *
 * @brief
 *   This module includes functions that control UDP communication.
 *
 * @{
 *
 */

/**
 * This callback allows OpenThread to provide specific handlers for certain UDP messages.
 *
 * @retval  true    The message is handled by this receiver and should not be further processed.
 * @retval  false   The message is not handled by this receiver.
 *
 */
typedef bool (*otUdpHandler)(void *aContext, const otMessage *aMessage, const otMessageInfo *aMessageInfo);

/**
 * This structure represents a UDP receiver.
 *
 */
typedef struct otUdpReceiver
{
    struct otUdpReceiver *mNext;    ///< A pointer to the next UDP receiver (internal use only).
    otUdpHandler          mHandler; ///< A function pointer to the receiver callback.
    void *                mContext; ///< A pointer to application-specific context.
} otUdpReceiver;

/**
 * This function adds a UDP receiver.
 *
 * @param[in]   aInstance       A pointer to an OpenThread instance.
 * @param[in]   aUdpReceiver    A pointer to the UDP receiver.
 *
 * @retval  OT_ERROR_NONE       The receiver is successfully added.
 * @retval  OT_ERROR_ALREADY    The UDP receiver was already added.
 *
 */
otError otUdpAddReceiver(otInstance *aInstance, otUdpReceiver *aUdpReceiver);

/**
 * This function removes a UDP receiver.
 *
 * @param[in]   aInstance       A pointer to an OpenThread instance.
 * @param[in]   aUdpReceiver    A pointer to the UDP receiver.
 *
 * @retval  OT_ERROR_NONE       The receiver is successfully removed.
 * @retval  OT_ERROR_NOT_FOUND  The UDP receiver was not added.
 *
 */
otError otUdpRemoveReceiver(otInstance *aInstance, otUdpReceiver *aUdpReceiver);

/**
 * This function sends a UDP message without socket.
 *
 * @param[in]  aInstance     A pointer to an OpenThread instance.
 * @param[in]  aMessage      A pointer to a message without UDP header.
 * @param[in]  aMessageInfo  A pointer to a message info associated with @p aMessage.
 *
 * @retval OT_ERROR_NONE     Successfully enqueued the message into an output interface.
 * @retval OT_ERROR_NO_BUFS  Insufficient available buffer to add the IPv6 headers.
 *
 */
otError otUdpSendDatagram(otInstance *aInstance, otMessage *aMessage, otMessageInfo *aMessageInfo);

/**
 * This callback allows OpenThread to inform the application of a received UDP message.
 *
 */
typedef void (*otUdpReceive)(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);

/**
 * This structure represents a UDP socket.
 *
 */
typedef struct otUdpSocket
{
    otSockAddr          mSockName; ///< The local IPv6 socket address.
    otSockAddr          mPeerName; ///< The peer IPv6 socket address.
    otUdpReceive        mHandler;  ///< A function pointer to the application callback.
    void *              mContext;  ///< A pointer to application-specific context.
    void *              mHandle;   ///< A handle to platform's UDP.
    struct otUdpSocket *mNext;     ///< A pointer to the next UDP socket (internal use only).
} otUdpSocket;

/**
 * This enumeration defines the OpenThread network interface identifiers.
 *
 */
typedef enum otNetifIdentifier
{
    OT_NETIF_UNSPECIFIED = 0, ///< Unspecified network interface.
    OT_NETIF_THREAD,          ///< The Thread interface.
    OT_NETIF_BACKBONE,        ///< The Backbone interface.
} otNetifIdentifier;

/**
 * Allocate a new message buffer for sending a UDP message.
 *
 * @note If @p aSettings is 'NULL', the link layer security is enabled and the message priority is set to
 * OT_MESSAGE_PRIORITY_NORMAL by default.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aSettings  A pointer to the message settings or NULL to use default settings.
 *
 * @returns A pointer to the message buffer or NULL if no message buffers are available or parameters are invalid.
 *
 * @sa otMessageFree
 *
 */
otMessage *otUdpNewMessage(otInstance *aInstance, const otMessageSettings *aSettings);

/**
 * Open a UDP/IPv6 socket.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aSocket    A pointer to a UDP socket structure.
 * @param[in]  aCallback  A pointer to the application callback function.
 * @param[in]  aContext   A pointer to application-specific context.
 *
 * @retval OT_ERROR_NONE    Successfully opened the socket.
 * @retval OT_ERROR_FAILED  Failed to open the socket.
 *
 */
otError otUdpOpen(otInstance *aInstance, otUdpSocket *aSocket, otUdpReceive aCallback, void *aContext);

/**
 * Check if a UDP socket is open.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aSocket    A pointer to a UDP socket structure.
 *
 * @returns Whether the UDP socket is open.
 *
 */
bool otUdpIsOpen(otInstance *aInstance, const otUdpSocket *aSocket);

/**
 * Close a UDP/IPv6 socket.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aSocket    A pointer to a UDP socket structure.
 *
 * @retval OT_ERROR_NONE   Successfully closed the socket.
 * @retval OT_ERROR_FAILED Failed to close UDP Socket.
 *
 */
otError otUdpClose(otInstance *aInstance, otUdpSocket *aSocket);

/**
 * Bind a UDP/IPv6 socket.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aSocket    A pointer to a UDP socket structure.
 * @param[in]  aSockName  A pointer to an IPv6 socket address structure.
 * @param[in]  aNetif     The network interface to bind.
 *
 * @retval OT_ERROR_NONE   Bind operation was successful.
 * @retval OT_ERROR_FAILED Failed to bind UDP socket.
 *
 */
otError otUdpBind(otInstance *aInstance, otUdpSocket *aSocket, const otSockAddr *aSockName, otNetifIdentifier aNetif);

/**
 * Connect a UDP/IPv6 socket.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aSocket    A pointer to a UDP socket structure.
 * @param[in]  aSockName  A pointer to an IPv6 socket address structure.
 *
 * @retval OT_ERROR_NONE   Connect operation was successful.
 * @retval OT_ERROR_FAILED Failed to connect UDP socket.
 *
 */
otError otUdpConnect(otInstance *aInstance, otUdpSocket *aSocket, const otSockAddr *aSockName);

/**
 * Send a UDP/IPv6 message.
 *
 * @param[in]  aInstance     A pointer to an OpenThread instance.
 * @param[in]  aSocket       A pointer to a UDP socket structure.
 * @param[in]  aMessage      A pointer to a message buffer.
 * @param[in]  aMessageInfo  A pointer to a message info structure.
 *
 * If the return value is OT_ERROR_NONE, OpenThread takes ownership of @p aMessage, and the caller should no longer
 * reference @p aMessage. If the return value is not OT_ERROR_NONE, the caller retains ownership of @p aMessage,
 * including freeing @p aMessage if the message buffer is no longer needed.
 *
 * @retval OT_ERROR_NONE           The message is successfully scheduled for sending.
 * @retval OT_ERROR_INVALID_ARGS   Invalid arguments are given.
 * @retval OT_ERROR_NO_BUFS        Insufficient available buffer to add the UDP and IPv6 headers.
 *
 */
otError otUdpSend(otInstance *aInstance, otUdpSocket *aSocket, otMessage *aMessage, const otMessageInfo *aMessageInfo);

/**
 * This function gets the head of linked list of UDP Sockets.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 *
 * @returns A pointer to the head of UDP Socket linked list.
 *
 */
otUdpSocket *otUdpGetSockets(otInstance *aInstance);

/**
 * @}
 *
 */

/**
 * @addtogroup api-udp-forward
 *
 * @brief
 *   This module includes functions for UDP forward feature.
 *
 *   The functions in this module are available when udp-forward feature (`OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE`) is
 *   enabled.
 *
 * @{
 *
 */

/**
 * This function pointer delivers the UDP packet to host and host should send the packet through its own network stack.
 *
 * @param[in]  aMessage   A pointer to the UDP Message.
 * @param[in]  aPeerPort  The destination UDP port.
 * @param[in]  aPeerAddr  A pointer to the destination IPv6 address.
 * @param[in]  aSockPort  The source UDP port.
 * @param[in]  aContext   A pointer to application-specific context.
 *
 */
typedef void (*otUdpForwarder)(otMessage *   aMessage,
                               uint16_t      aPeerPort,
                               otIp6Address *aPeerAddr,
                               uint16_t      aSockPort,
                               void *        aContext);

/**
 * Set UDP forward callback to deliver UDP packets to host.
 *
 * @param[in]  aInstance            A pointer to an OpenThread instance.
 * @param[in]  aForwarder           A pointer to a function called to forward UDP packet to host.
 * @param[in]  aContext             A pointer to application-specific context.
 *
 */
void otUdpForwardSetForwarder(otInstance *aInstance, otUdpForwarder aForwarder, void *aContext);

/**
 * Handle a UDP packet received from host.
 *
 * @param[in]  aInstance            A pointer to an OpenThread instance.
 * @param[in]  aMessage             A pointer to the UDP Message.
 * @param[in]  aPeerPort            The source UDP port.
 * @param[in]  aPeerAddr            A pointer to the source address.
 * @param[in]  aSockPort            The destination UDP port.
 *
 * @warning No matter the call success or fail, the message is freed.
 *
 */
void otUdpForwardReceive(otInstance *        aInstance,
                         otMessage *         aMessage,
                         uint16_t            aPeerPort,
                         const otIp6Address *aPeerAddr,
                         uint16_t            aSockPort);

/**
 * Determines if the given UDP port is exclusively opened by OpenThread API.
 *
 * @param[in]  aInstance            A pointer to an OpenThread instance.
 * @param[in]  port                 UDP port number to verify.
 *
 * @retval true    The port is being used exclusively by OpenThread.
 * @retval false   The port is not used by any of the OpenThread API or is shared (e.g. is Backbone socket).
 *
 */
bool otUdpIsPortInUse(otInstance *aInstance, uint16_t port);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_UDP_H_
