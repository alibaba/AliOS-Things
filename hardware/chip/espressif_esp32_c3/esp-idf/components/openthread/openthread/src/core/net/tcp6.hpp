/*
 *  Copyright (c) 2021, The OpenThread Authors.
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
 *   This file includes definitions for TCP/IPv6 sockets.
 */

#ifndef TCP6_HPP_
#define TCP6_HPP_

#include "openthread-core-config.h"

#include <openthread/tcp.h>

#include "net/ip6_headers.hpp"
#include "net/socket.hpp"

#include "common/linked_list.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/timer.hpp"

namespace ot {
namespace Ip6 {

/**
 * @addtogroup core-tcp
 *
 * @brief
 *   This module includes definitions for TCP/IPv6 sockets.
 *
 * @{
 *
 */

/**
 * This class implements TCP message handling.
 *
 */
class Tcp : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This class represents an endpoint of a TCP/IPv6 connection.
     *
     */
    class Endpoint : public otTcpEndpoint, public LinkedListEntry<Endpoint>
    {
        friend class Tcp;

    public:
        /**
         * Initializes a TCP endpoint.
         *
         * Calling this function causes OpenThread to keep track of this Endpoint
         * and store and retrieve TCP data inside of it. The application
         * should refrain from directly accessing or modifying the fields in
         * this Endpoint. If the application needs to reclaim the memory backing
         * this Endpoint, it should call otTcpEndpointDeinitialize().
         *
         * @sa otTcpEndpointInitialize in include/openthread/tcp.h.
         *
         * @param[in]  aInstance  A pointer to an OpenThread instance.
         * @param[in]  aArgs      A pointer to a structure of arguments.
         *
         * @retval kErrorNone    Successfully opened the TCP endpoint.
         * @retval kErrorFailed  Failed to open the TCP endpoint.
         *
         */
        Error Initialize(Instance &aInstance, otTcpEndpointInitializeArgs &aArgs);

        /**
         * Obtains the Instance that was associated with this Endpoint upon
         * initialization.
         *
         * @sa otTcpEndpointGetInstance
         *
         * @returns  The Instance pointer associated with this Endpoint.
         *
         */
        Instance &GetInstance(void);

        /**
         * Obtains the context pointer that was associated this Endpoint upon
         * initialization.
         *
         * @sa otTcpEndpointGetContext
         *
         * @returns  The context pointer associated with this Endpoint.
         *
         */
        void *GetContext(void) { return mContext; }

        /**
         * Obtains a pointer to a TCP endpoint's local host and port.
         *
         * The contents of the host and port may be stale if this socket is not in a
         * connected state and has not been bound after it was last disconnected.
         *
         * @sa otTcpGetLocalAddress
         *
         * @returns  The local host and port of this Endpoint.
         *
         */
        const SockAddr &GetLocalAddress(void) const;

        /**
         * Obtains a pointer to a TCP endpoint's peer's host and port.
         *
         * The contents of the host and port may be stale if this socket is not in a
         * connected state.
         *
         * @sa otTcpGetPeerAddress
         *
         * @returns  The host and port of the connection peer of this Endpoint.
         *
         */
        const SockAddr &GetPeerAddress(void) const;

        /**
         * Binds the TCP endpoint to an IP address and port.
         *
         * @sa otTcpBind
         *
         * @param[in]  aSockName   The address and port to which to bind this TCP endpoint.
         *
         * @retval kErrorNone    Successfully bound the TCP endpoint.
         * @retval kErrorFailed  Failed to bind the TCP endpoint.
         *
         */
        Error Bind(const SockAddr &aSockName);

        /**
         * Records the remote host and port for this connection.
         *
         * By default TCP Fast Open is used. This means that this function merely
         * records the remote host and port, and that the TCP connection establishment
         * handshake only happens on the first call to otTcpSendByReference(). TCP Fast
         * Open can be explicitly disabled using @p aFlags, in which case the TCP
         * connection establishment handshake is initiated immediately.
         *
         * @sa otTcpConnect
         *
         * @param[in]  aSockName  The IP address and port of the host to which to connect.
         * @param[in]  aFlags     Flags specifying options for this operation (see enumeration above).
         *
         * @retval kErrorNone    Successfully completed the operation.
         * @retval kErrorFailed  Failed to complete the operation.
         *
         */
        Error Connect(const SockAddr &aSockName, uint32_t aFlags);

        /**
         * Adds data referenced by the linked buffer pointed to by @p aBuffer to the
         * send buffer.
         *
         * Upon a successful call to this function, the linked buffer and data it
         * references are owned by the TCP stack; they should not be modified by the
         * application until a "send done" callback returns ownership of those objects
         * to the application. It is acceptable to call this function to add another
         * linked buffer to the send queue, even if the "send done" callback for a
         * previous invocation of this function has not yet fired.
         *
         * Note that @p aBuffer should not be chained; its mNext field should be
         * NULL. If additional data will be added right after this call, then the
         * OT_TCP_SEND_MORE_TO_COME flag should be used as a hint to the TCP
         * implementation.
         *
         * @sa otTcpSendByReference
         *
         * @param[in]  aBuffer    A pointer to the linked buffer chain referencing data to add to the send buffer.
         * @param[in]  aFlags     Flags specifying options for this operation (see enumeration above).
         *
         * @retval kErrorNone    Successfully added data to the send buffer.
         * @retval kErrorFailed  Failed to add data to the send buffer.
         *
         */
        Error SendByReference(otLinkedBuffer &aBuffer, uint32_t aFlags);

        /**
         * Adds data to the send buffer by extending the length of the final
         * otLinkedBuffer in the send buffer by the specified amount.
         *
         * If the send buffer is empty, then the operation fails.
         *
         * @sa otTcpSendByExtension
         *
         * @param[in]  aNumBytes  The number of bytes by which to extend the length of the final linked buffer.
         * @param[in]  aFlags     Flags specifying options for this operation (see enumeration above).
         *
         * @retval kErrorNone    Successfully added data to the send buffer.
         * @retval kErrorFailed  Failed to add data to the send buffer.
         *
         */
        Error SendByExtension(size_t aNumBytes, uint32_t aFlags);

        /**
         * Provides the application with a linked buffer chain referencing data
         * currently in the TCP receive buffer.
         *
         * The linked buffer chain is valid until the "receive ready" callback is next
         * invoked, or until the next call to otTcpReceiveContiguify() or
         * otTcpCommitReceive().
         *
         * @sa otTcpReceiveByReference
         *
         * @param[out]  aBuffer    A pointer to the linked buffer chain referencing data currently in the receive
         * buffer.
         *
         * @retval kErrorNone    Successfully completed the operation.
         * @retval kErrorFailed  Failed to complete the operation.
         */
        Error ReceiveByReference(const otLinkedBuffer *&aBuffer) const;

        /**
         * Reorganizes the receive buffer to be entirely contiguous in memory.
         *
         * This is optional; an application can simply traverse the linked buffer
         * chain obtained by calling @p otTcpReceiveByReference. Some
         * applications may wish to call this function to make the receive buffer
         * contiguous to simplify their data processing, but this comes at the expense
         * of CPU time to reorganize the data in the receive buffer.
         *
         * @sa otTcpReceiveContiguify
         *
         * @retval kErrorNone    Successfully completed the operation.
         * @retval kErrorFailed  Failed to complete the operation.
         *
         */
        Error ReceiveContiguify(void);

        /**
         * Informs the TCP stack that the application has finished processing
         * @p aNumBytes bytes of data at the start of the receive buffer and that the
         * TCP stack need not continue maintaining those bytes in the receive buffer.
         *
         * @sa otTcpCommitReceive
         *
         * @param[in]  aNumBytes  The number of bytes consumed.
         * @param[in]  aFlags     Flags specifying options for this operation (none yet).
         *
         * @retval kErrorNone    Successfully completed the receive operation.
         * @retval kErrorFailed  Failed to complete the receive operation.
         *
         */
        Error CommitReceive(size_t aNumBytes, uint32_t aFlags);

        /**
         * Informs the connection peer that this TCP endpoint will not send more data.
         *
         * This should be used when the application has no more data to send to the
         * connection peer. For this connection, future reads on the connection peer
         * will result in the "end of stream" condition, and future writes on this
         * connection endpoint will fail.
         *
         * The "end of stream" condition only applies after any data previously
         * provided to the TCP stack to send out has been received by the connection
         * peer.
         *
         * @sa otTcpSendEndOfStream
         *
         * @retval kErrorNone    Successfully queued the "end of stream" condition for transmission.
         * @retval kErrorFailed  Failed to queue the "end of stream" condition for transmission.
         *
         */
        Error SendEndOfStream(void);

        /**
         * Forcibly ends the TCP connection associated with this TCP endpoint.
         *
         * This immediately makes the TCP endpoint free for use for another connection
         * and empties the send and receive buffers, transferring ownership of any data
         * provided by the application in otTcpSendByReference() calls back to
         * the application. The TCP endpoint's callbacks and memory for the receive
         * buffer remain associated with the TCP endpoint.
         *
         * @sa otTcpAbort
         *
         * @retval kErrorNone    Successfully aborted the TCP endpoint's connection.
         * @retval kErrorFailed  Failed to abort the TCP endpoint's connection.
         *
         */
        Error Abort(void);

        /**
         * Deinitializes this TCP endpoint.
         *
         * This means that OpenThread no longer keeps track of this TCP endpoint and
         * deallocates all resources it has internally allocated for this TCP endpoint.
         * The application can reuse the memory backing the TCP endpoint as it sees fit.
         *
         * If it corresponds to a live TCP connection, the connection is terminated
         * unceremoniously (as in otTcpAbort()). All resources the application has
         * provided for this TCP endpoint (linked buffers for the send buffer, memory
         * for the receive buffer, this Endpoint structure itself, etc.) are
         * immediately returned to the application.
         *
         * @sa otTcpEndpointDeinitialize
         *
         * @retval kErrorNone    Successfully deinitialized the TCP endpoint.
         * @retval kErrorFailed  Failed to deinitialize the TCP endpoint.
         *
         */
        Error Deinitialize(void);

    private:
        enum : uint8_t
        {
            kTimerDelack       = 0,
            kTimerRexmtPersist = 1,
            kTimerKeep         = 2,
            kTimer2Msl         = 3,
            kNumTimers         = 4,
        };

        static uint8_t TimerFlagToIndex(uint8_t aTimerFlag);

        bool IsTimerActive(uint8_t aTimerIndex);
        void SetTimer(uint8_t aTimerFlag, uint32_t aDelay);
        void CancelTimer(uint8_t aTimerFlag);
        bool FirePendingTimers(TimeMilli aNow, bool &aHasFutureTimer, TimeMilli &aEarliestFutureExpiry);
    };

    /**
     * This class represents a TCP/IPv6 listener.
     *
     */
    class Listener : public otTcpListener, public LinkedListEntry<Listener>
    {
    public:
        /**
         * Initializes a TCP listener.
         *
         * Calling this function causes OpenThread to keep track of the TCP listener
         * and store and retrieve TCP data inside this Listener. The application should
         * refrain from directly accessing or modifying the fields in this Listener. If
         * the application needs to reclaim the memory backing this Listener, it should
         * call otTcpListenerDeinitialize().
         *
         * @sa otTcpListenerInitialize
         *
         * @param[in]  aInstance  A pointer to an OpenThread instance.
         * @param[in]  aArgs      A pointer to a structure of arguments.
         *
         * @retval kErrorNone    Successfully opened the TCP listener.
         * @retval kErrorFailed  Failed to open the TCP listener.
         *
         */
        Error Initialize(Instance &aInstance, otTcpListenerInitializeArgs &aArgs);

        /**
         * Obtains the otInstance that was associated with this Listener upon
         * initialization.
         *
         * @sa otTcpListenerGetInstance
         *
         * @returns  The otInstance pointer associated with this Listener.
         *
         */
        Instance &GetInstance(void);

        /**
         * Obtains the context pointer that was associated with this Listener upon
         * initialization.
         *
         * @sa otTcpListenerGetContext
         *
         * @returns  The context pointer associated with this Listener.
         *
         */
        void *GetContext(void) { return mContext; }

        /**
         * Causes incoming TCP connections that match the specified IP address and port
         * to trigger this TCP listener's callbacks.
         *
         * @sa otTcpListen
         *
         * @param[in]  aSockName  The address and port on which to listen for incoming connections.
         *
         * @retval kErrorNone    Successfully initiated listening on the TCP listener.
         * @retval kErrorFailed  Failed to initiate listening on the TCP listener.
         *
         */
        Error Listen(const SockAddr &aSockName);

        /**
         * Causes this TCP listener to stop listening for incoming connections.
         *
         * @sa otTcpStopListening
         *
         * @retval kErrorNone    Successfully stopped listening on the TCP listener.
         * @retval kErrorFailed  Failed to stop listening on the TCP listener.
         *
         */
        Error StopListening(void);

        /**
         * Deinitializes this TCP listener.
         *
         * This means that OpenThread no longer keeps track of this TCP listener and
         * deallocates all resources it has internally allocated for this TCP endpoint.
         * The application can reuse the memory backing the TCP listener as it sees
         * fit.
         *
         * If the TCP listener is currently listening, it stops listening.
         *
         * @sa otTcpListenerDeinitialize
         *
         * @retval kErrorNone    Successfully deinitialized the TCP listener.
         * @retval kErrorFailed  Failed to deinitialize the TCP listener.
         *
         */
        Error Deinitialize(void);
    };

    /**
     * This class implements TCP header parsing.
     *
     */
    OT_TOOL_PACKED_BEGIN
    class Header
    {
    public:
        static constexpr uint8_t kChecksumFieldOffset = 16; ///< Byte offset of the Checksum field in the TCP header.

        /**
         * This method returns the TCP Source Port.
         *
         * @returns The TCP Source Port.
         *
         */
        uint16_t GetSourcePort(void) const { return HostSwap16(mSource); }

        /**
         * This method returns the TCP Destination Port.
         *
         * @returns The TCP Destination Port.
         *
         */
        uint16_t GetDestinationPort(void) const { return HostSwap16(mDestination); }

        /**
         * This method returns the TCP Sequence Number.
         *
         * @returns The TCP Sequence Number.
         *
         */
        uint32_t GetSequenceNumber(void) const { return HostSwap32(mSequenceNumber); }

        /**
         * This method returns the TCP Acknowledgment Sequence Number.
         *
         * @returns The TCP Acknowledgment Sequence Number.
         *
         */
        uint32_t GetAcknowledgmentNumber(void) const { return HostSwap32(mAckNumber); }

        /**
         * This method returns the TCP Flags.
         *
         * @returns The TCP Flags.
         *
         */
        uint16_t GetFlags(void) const { return HostSwap16(mFlags); }

        /**
         * This method returns the TCP Window.
         *
         * @returns The TCP Window.
         *
         */
        uint16_t GetWindow(void) const { return HostSwap16(mWindow); }

        /**
         * This method returns the TCP Checksum.
         *
         * @returns The TCP Checksum.
         *
         */
        uint16_t GetChecksum(void) const { return HostSwap16(mChecksum); }

        /**
         * This method returns the TCP Urgent Pointer.
         *
         * @returns The TCP Urgent Pointer.
         *
         */
        uint16_t GetUrgentPointer(void) const { return HostSwap16(mUrgentPointer); }

    private:
        uint16_t mSource;
        uint16_t mDestination;
        uint32_t mSequenceNumber;
        uint32_t mAckNumber;
        uint16_t mFlags;
        uint16_t mWindow;
        uint16_t mChecksum;
        uint16_t mUrgentPointer;
    } OT_TOOL_PACKED_END;

    /**
     * This constructor initializes the object.
     *
     * @param[in] aInstance  A reference to the OpenThread instance.
     *
     */
    explicit Tcp(Instance &aInstance);

    /**
     * Processes a received TCP segment.
     *
     * @param[in]  aIp6Header    A reference to a structure containing the segment's IPv6 header.
     * @param[in]  aMessage      A reference to the message containing the TCP segment.
     * @param[in]  aMessageInfo  A reference to the message info associated with @p aMessage.
     *
     * @retval kErrorNone  Successfully processed the TCP segment.
     * @retval kErrorDrop  Dropped the TCP segment due to an invalid checksum.
     *
     */
    Error HandleMessage(ot::Ip6::Header &aIp6Header, Message &aMessage, MessageInfo &aMessageInfo);

private:
    enum
    {
        kDynamicPortMin = 49152, ///< Service Name and Transport Protocol Port Number Registry
        kDynamicPortMax = 65535, ///< Service Name and Transport Protocol Port Number Registry
    };

    static void HandleTimer(Timer &aTimer);
    void        ProcessTimers(void);

    TimerMilli mTimer;

    LinkedList<Endpoint> mEndpoints;
    LinkedList<Listener> mListeners;
    uint16_t             mEphemeralPort;
};

} // namespace Ip6
} // namespace ot

#endif // TCP6_HPP_
