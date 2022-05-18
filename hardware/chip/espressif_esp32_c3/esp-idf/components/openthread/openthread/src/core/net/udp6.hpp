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
 *   This file includes definitions for UDP/IPv6 sockets.
 */

#ifndef UDP6_HPP_
#define UDP6_HPP_

#include "openthread-core-config.h"

#include <openthread/udp.h>
#include <openthread/platform/udp.h>

#include "common/clearable.hpp"
#include "common/linked_list.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "net/ip6_headers.hpp"

namespace ot {
namespace Ip6 {

class Udp;

/**
 * @addtogroup core-udp
 *
 * @brief
 *   This module includes definitions for UDP/IPv6 sockets.
 *
 * @{
 *
 */

#if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE && OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE
#error "OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE and OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE must not both be set."
#endif

/**
 * This class implements core UDP message handling.
 *
 */
class Udp : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This class implements a UDP/IPv6 socket.
     *
     */
    class SocketHandle : public otUdpSocket, public LinkedListEntry<SocketHandle>, public Clearable<SocketHandle>
    {
        friend class Udp;
        friend class LinkedList<SocketHandle>;

    public:
        /**
         * This method indicates whether or not the socket is bound.
         *
         * @retval TRUE if the socket is bound (i.e. source port is non-zero).
         * @retval FALSE if the socket is not bound (source port is zero).
         *
         */
        bool IsBound(void) const { return mSockName.mPort != 0; }

        /**
         * This method returns the local socket address.
         *
         * @returns A reference to the local socket address.
         *
         */
        SockAddr &GetSockName(void) { return *static_cast<SockAddr *>(&mSockName); }

        /**
         * This method returns the local socket address.
         *
         * @returns A reference to the local socket address.
         *
         */
        const SockAddr &GetSockName(void) const { return *static_cast<const SockAddr *>(&mSockName); }

        /**
         * This method returns the peer's socket address.
         *
         * @returns A reference to the peer's socket address.
         *
         */
        SockAddr &GetPeerName(void) { return *static_cast<SockAddr *>(&mPeerName); }

        /**
         * This method returns the peer's socket address.
         *
         * @returns A reference to the peer's socket address.
         *
         */
        const SockAddr &GetPeerName(void) const { return *static_cast<const SockAddr *>(&mPeerName); }

    private:
        bool Matches(const MessageInfo &aMessageInfo) const;

        void HandleUdpReceive(Message &aMessage, const MessageInfo &aMessageInfo)
        {
            mHandler(mContext, &aMessage, &aMessageInfo);
        }
    };

    /**
     * This class implements a UDP/IPv6 socket.
     *
     */
    class Socket : public InstanceLocator, public SocketHandle
    {
        friend class Udp;

    public:
        /**
         * This constructor initializes the object.
         *
         * @param[in]  aInstance  A reference to OpenThread instance.
         *
         */
        explicit Socket(Instance &aInstance);

        /**
         * This method returns a new UDP message with sufficient header space reserved.
         *
         * @param[in]  aReserved  The number of header bytes to reserve after the UDP header.
         * @param[in]  aSettings  The message settings (default is used if not provided).
         *
         * @returns A pointer to the message or nullptr if no buffers are available.
         *
         */
        Message *NewMessage(uint16_t aReserved, const Message::Settings &aSettings = Message::Settings::GetDefault());

        /**
         * This method opens the UDP socket.
         *
         * @param[in]  aHandler  A pointer to a function that is called when receiving UDP messages.
         * @param[in]  aContext  A pointer to arbitrary context information.
         *
         * @retval kErrorNone     Successfully opened the socket.
         * @retval kErrorFailed   Failed to open the socket.
         *
         */
        Error Open(otUdpReceive aHandler, void *aContext);

        /**
         * This method returns if the UDP socket is open.
         *
         * @returns If the UDP socket is open.
         *
         */
        bool IsOpen(void) const;

        /**
         * This method binds the UDP socket.
         *
         * @param[in]  aSockAddr            A reference to the socket address.
         * @param[in]  aNetifIdentifier     The network interface identifier.
         *
         * @retval kErrorNone            Successfully bound the socket.
         * @retval kErrorInvalidArgs     Unable to bind to Thread network interface with the given address.
         * @retval kErrorFailed          Failed to bind UDP Socket.
         *
         */
        Error Bind(const SockAddr &aSockAddr, otNetifIdentifier aNetifIdentifier = OT_NETIF_THREAD);

        /**
         * This method binds the UDP socket.
         *
         * @param[in]  aPort                A port number.
         * @param[in]  aNetifIdentifier     The network interface identifier.
         *
         * @retval kErrorNone            Successfully bound the socket.
         * @retval kErrorFailed          Failed to bind UDP Socket.
         *
         */
        Error Bind(uint16_t aPort, otNetifIdentifier aNetifIdentifier = OT_NETIF_THREAD);

        /**
         * This method binds the UDP socket.
         *
         * @retval kErrorNone    Successfully bound the socket.
         * @retval kErrorFailed  Failed to bind UDP Socket.
         *
         */
        Error Bind(void) { return Bind(0); }

        /**
         * This method connects the UDP socket.
         *
         * @param[in]  aSockAddr  A reference to the socket address.
         *
         * @retval kErrorNone    Successfully connected the socket.
         * @retval kErrorFailed  Failed to connect UDP Socket.
         *
         */
        Error Connect(const SockAddr &aSockAddr);

        /**
         * This method connects the UDP socket.
         *
         * @param[in]  aPort        A port number.
         *
         * @retval kErrorNone    Successfully connected the socket.
         * @retval kErrorFailed  Failed to connect UDP Socket.
         *
         */
        Error Connect(uint16_t aPort);

        /**
         * This method connects the UDP socket.
         *
         * @retval kErrorNone    Successfully connected the socket.
         * @retval kErrorFailed  Failed to connect UDP Socket.
         *
         */
        Error Connect(void) { return Connect(0); }

        /**
         * This method closes the UDP socket.
         *
         * @retval kErrorNone    Successfully closed the UDP socket.
         * @retval kErrorFailed  Failed to close UDP Socket.
         *
         */
        Error Close(void);

        /**
         * This method sends a UDP message.
         *
         * @param[in]  aMessage      The message to send.
         * @param[in]  aMessageInfo  The message info associated with @p aMessage.
         *
         * @retval kErrorNone         Successfully sent the UDP message.
         * @retval kErrorInvalidArgs  If no peer is specified in @p aMessageInfo or by Connect().
         * @retval kErrorNoBufs       Insufficient available buffer to add the UDP and IPv6 headers.
         *
         */
        Error SendTo(Message &aMessage, const MessageInfo &aMessageInfo);

#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
        /**
         * This method configures the UDP socket to join a mutlicast group on a Host network interface.
         *
         * @param[in]  aNetifIdentifier     The network interface identifier.
         * @param[in]  aAddress             The multicast group address.
         *
         * @retval  kErrorNone    Successfully joined the multicast group.
         * @retval  kErrorFailed  Failed to join the multicast group.
         *
         */
        Error JoinNetifMulticastGroup(otNetifIdentifier aNetifIdentifier, const Address &aAddress);

        /**
         * This method configures the UDP socket to leave a multicast group on a Host network interface.
         *
         * @param[in]  aNetifIdentifier     The network interface identifier.
         * @param[in]  aAddress             The multicast group address.
         *
         * @retval  kErrorNone   Successfully left the multicast group.
         * @retval  kErrorFailed Failed to leave the multicast group.
         *
         */
        Error LeaveNetifMulticastGroup(otNetifIdentifier aNetifIdentifier, const Address &aAddress);
#endif
    };

    /**
     * This class implements a UDP receiver.
     *
     */
    class Receiver : public otUdpReceiver, public LinkedListEntry<Receiver>
    {
        friend class Udp;

    public:
        /**
         * This constructor initializes the UDP receiver.
         *
         * @param[in]   aUdpHandler     A pointer to the function to handle UDP message.
         * @param[in]   aContext        A pointer to arbitrary context information.
         *
         */
        Receiver(otUdpHandler aHandler, void *aContext)
        {
            mNext    = nullptr;
            mHandler = aHandler;
            mContext = aContext;
        }

    private:
        bool HandleMessage(Message &aMessage, const MessageInfo &aMessageInfo)
        {
            return mHandler(mContext, &aMessage, &aMessageInfo);
        }
    };

    /**
     * This class implements UDP header generation and parsing.
     *
     */
    OT_TOOL_PACKED_BEGIN
    class Header
    {
    public:
        static constexpr uint16_t kSourcePortFieldOffset = 0; ///< Byte offset of Source Port field in UDP header.
        static constexpr uint16_t kDestPortFieldOffset   = 2; ///< Byte offset of Destination Port field in UDP header.
        static constexpr uint16_t kLengthFieldOffset     = 4; ///< Byte offset of Length field in UDP header.
        static constexpr uint16_t kChecksumFieldOffset   = 6; ///< Byte offset of Checksum field in UDP header.

        /**
         * This method returns the UDP Source Port.
         *
         * @returns The UDP Source Port.
         *
         */
        uint16_t GetSourcePort(void) const { return HostSwap16(mSourcePort); }

        /**
         * This method sets the UDP Source Port.
         *
         * @param[in]  aPort  The UDP Source Port.
         *
         */
        void SetSourcePort(uint16_t aPort) { mSourcePort = HostSwap16(aPort); }

        /**
         * This method returns the UDP Destination Port.
         *
         * @returns The UDP Destination Port.
         *
         */
        uint16_t GetDestinationPort(void) const { return HostSwap16(mDestinationPort); }

        /**
         * This method sets the UDP Destination Port.
         *
         * @param[in]  aPort  The UDP Destination Port.
         *
         */
        void SetDestinationPort(uint16_t aPort) { mDestinationPort = HostSwap16(aPort); }

        /**
         * This method returns the UDP Length.
         *
         * @returns The UDP Length.
         *
         */
        uint16_t GetLength(void) const { return HostSwap16(mLength); }

        /**
         * This method sets the UDP Length.
         *
         * @param[in]  aLength  The UDP Length.
         *
         */
        void SetLength(uint16_t aLength) { mLength = HostSwap16(aLength); }

        /**
         * This method returns the UDP Checksum.
         *
         * @returns The UDP Checksum.
         *
         */
        uint16_t GetChecksum(void) const { return HostSwap16(mChecksum); }

        /**
         * This method sets the UDP Checksum.
         *
         * @param[in]  aChecksum  The UDP Checksum.
         *
         */
        void SetChecksum(uint16_t aChecksum) { mChecksum = HostSwap16(aChecksum); }

    private:
        uint16_t mSourcePort;
        uint16_t mDestinationPort;
        uint16_t mLength;
        uint16_t mChecksum;

    } OT_TOOL_PACKED_END;

    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance  A reference to OpenThread instance.
     *
     */
    explicit Udp(Instance &aInstance);

    /**
     * This method adds a UDP receiver.
     *
     * @param[in]  aReceiver  A reference to the UDP receiver.
     *
     * @retval kErrorNone    Successfully added the UDP receiver.
     * @retval kErrorAlready The UDP receiver was already added.
     *
     */
    Error AddReceiver(Receiver &aReceiver);

    /**
     * This method removes a UDP receiver.
     *
     * @param[in]  aReceiver  A reference to the UDP receiver.
     *
     * @retval kErrorNone       Successfully removed the UDP receiver.
     * @retval kErrorNotFound   The UDP receiver was not added.
     *
     */
    Error RemoveReceiver(Receiver &aReceiver);

    /**
     * This method opens a UDP socket.
     *
     * @param[in]  aSocket   A reference to the socket.
     * @param[in]  aHandler  A pointer to a function that is called when receiving UDP messages.
     * @param[in]  aContext  A pointer to arbitrary context information.
     *
     * @retval kErrorNone     Successfully opened the socket.
     * @retval kErrorFailed   Failed to open the socket.
     *
     */
    Error Open(SocketHandle &aSocket, otUdpReceive aHandler, void *aContext);

    /**
     * This method returns if a UDP socket is open.
     *
     * @param[in]  aSocket   A reference to the socket.
     *
     * @returns If the UDP socket is open.
     *
     */
    bool IsOpen(const SocketHandle &aSocket) const { return mSockets.Contains(aSocket); }

    /**
     * This method binds a UDP socket.
     *
     * @param[in]  aSocket          A reference to the socket.
     * @param[in]  aSockAddr        A reference to the socket address.
     * @param[in]  aNetifIdentifier The network interface identifier.
     *
     * @retval kErrorNone            Successfully bound the socket.
     * @retval kErrorInvalidArgs     Unable to bind to Thread network interface with the given address.
     * @retval kErrorFailed          Failed to bind UDP Socket.
     *
     */
    Error Bind(SocketHandle &aSocket, const SockAddr &aSockAddr, otNetifIdentifier aNetifIdentifier);

    /**
     * This method connects a UDP socket.
     *
     * @param[in]  aSocket    A reference to the socket.
     * @param[in]  aSockAddr  A reference to the socket address.
     *
     * @retval kErrorNone    Successfully connected the socket.
     * @retval kErrorFailed  Failed to connect UDP Socket.
     *
     */
    Error Connect(SocketHandle &aSocket, const SockAddr &aSockAddr);

    /**
     * This method closes the UDP socket.
     *
     * @param[in]  aSocket    A reference to the socket.
     *
     * @retval kErrorNone    Successfully closed the UDP socket.
     * @retval kErrorFailed  Failed to close UDP Socket.
     *
     */
    Error Close(SocketHandle &aSocket);

    /**
     * This method sends a UDP message using a socket.
     *
     * @param[in]  aSocket       A reference to the socket.
     * @param[in]  aMessage      The message to send.
     * @param[in]  aMessageInfo  The message info associated with @p aMessage.
     *
     * @retval kErrorNone         Successfully sent the UDP message.
     * @retval kErrorInvalidArgs  If no peer is specified in @p aMessageInfo or by Connect().
     * @retval kErrorNoBufs       Insufficient available buffer to add the UDP and IPv6 headers.
     *
     */
    Error SendTo(SocketHandle &aSocket, Message &aMessage, const MessageInfo &aMessageInfo);

    /**
     * This method returns a new ephemeral port.
     *
     * @returns A new ephemeral port.
     *
     */
    uint16_t GetEphemeralPort(void);

    /**
     * This method returns a new UDP message with sufficient header space reserved.
     *
     * @param[in]  aReserved  The number of header bytes to reserve after the UDP header.
     * @param[in]  aSettings  The message settings.
     *
     * @returns A pointer to the message or nullptr if no buffers are available.
     *
     */
    Message *NewMessage(uint16_t aReserved, const Message::Settings &aSettings = Message::Settings::GetDefault());

    /**
     * This method sends an IPv6 datagram.
     *
     * @param[in]  aMessage      A reference to the message.
     * @param[in]  aMessageInfo  A reference to the message info associated with @p aMessage.
     * @param[in]  aIpProto      The Internet Protocol value.
     *
     * @retval kErrorNone    Successfully enqueued the message into an output interface.
     * @retval kErrorNoBufs  Insufficient available buffer to add the IPv6 headers.
     *
     */
    Error SendDatagram(Message &aMessage, MessageInfo &aMessageInfo, uint8_t aIpProto);

    /**
     * This method handles a received UDP message.
     *
     * @param[in]  aMessage      A reference to the UDP message to process.
     * @param[in]  aMessageInfo  A reference to the message info associated with @p aMessage.
     *
     * @retval kErrorNone  Successfully processed the UDP message.
     * @retval kErrorDrop  Could not fully process the UDP message.
     *
     */
    Error HandleMessage(Message &aMessage, MessageInfo &aMessageInfo);

    /**
     * This method handles a received UDP message with offset set to the payload.
     *
     * @param[in]  aMessage      A reference to the UDP message to process.
     * @param[in]  aMessageInfo  A reference to the message info associated with @p aMessage.
     *
     */
    void HandlePayload(Message &aMessage, MessageInfo &aMessageInfo);

    /**
     * This method returns the head of UDP Sockets list.
     *
     * @returns A pointer to the head of UDP Socket linked list.
     *
     */
    SocketHandle *GetUdpSockets(void) { return mSockets.GetHead(); }

#if OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE
    /**
     * This method sets the forward sender.
     *
     * @param[in]   aForwarder  A function pointer to forward UDP packets.
     * @param[in]   aContext    A pointer to arbitrary context information.
     *
     */
    void SetUdpForwarder(otUdpForwarder aForwarder, void *aContext)
    {
        mUdpForwarder        = aForwarder;
        mUdpForwarderContext = aContext;
    }
#endif

    /**
     * This method returns whether a udp port is being used by OpenThread or any of it's optional
     * features, e.g. CoAP API.
     *
     * @param[in]   aPort       The udp port
     *
     * @retval True when port is used by the OpenThread.
     * @retval False when the port is not used by OpenThread.
     *
     */
    bool IsPortInUse(uint16_t aPort) const;

    /**
     * This method returns whether a udp port belongs to the platform or the stack.
     *
     * @param[in]   aPort       The udp port
     *
     * @retval True when the port belongs to the platform.
     * @retval False when the port belongs to the stack.
     *
     */
    bool ShouldUsePlatformUdp(uint16_t aPort) const;

private:
    static constexpr uint16_t kDynamicPortMin = 49152; // Service Name and Transport Protocol Port Number Registry
    static constexpr uint16_t kDynamicPortMax = 65535; // Service Name and Transport Protocol Port Number Registry

    // Reserved range for use by SRP server
    static constexpr uint16_t kSrpServerPortMin = OPENTHREAD_CONFIG_SRP_SERVER_UDP_PORT_MIN;
    static constexpr uint16_t kSrpServerPortMax = OPENTHREAD_CONFIG_SRP_SERVER_UDP_PORT_MAX;

    static bool IsPortReserved(uint16_t aPort);

    void AddSocket(SocketHandle &aSocket);
    void RemoveSocket(SocketHandle &aSocket);
#if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
    bool ShouldUsePlatformUdp(const SocketHandle &aSocket) const;
#endif

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    void                SetBackboneSocket(SocketHandle &aSocket);
    const SocketHandle *GetBackboneSockets(void) const;
    bool                IsBackboneSocket(const SocketHandle &aSocket) const;
#endif

    uint16_t                 mEphemeralPort;
    LinkedList<Receiver>     mReceivers;
    LinkedList<SocketHandle> mSockets;
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    SocketHandle *mPrevBackboneSockets;
#endif
#if OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE
    void *         mUdpForwarderContext;
    otUdpForwarder mUdpForwarder;
#endif
};

/**
 * @}
 *
 */

} // namespace Ip6
} // namespace ot

#endif // UDP6_HPP_
