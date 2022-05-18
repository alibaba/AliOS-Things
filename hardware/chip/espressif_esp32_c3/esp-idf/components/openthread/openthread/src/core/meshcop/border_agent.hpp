/*
 *  Copyright (c) 2018, The OpenThread Authors.
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
 *   This file includes definitions for the BorderAgent role.
 */

#ifndef BORDER_AGENT_HPP_
#define BORDER_AGENT_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE

#include <openthread/border_agent.h>

#include "coap/coap.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/notifier.hpp"
#include "net/udp6.hpp"

namespace ot {

namespace MeshCoP {

class BorderAgent : public InstanceLocator, private NonCopyable
{
    friend class ot::Notifier;

public:
    /**
     * This enumeration defines the Border Agent state.
     *
     */
    enum State : uint8_t
    {
        kStateStopped = OT_BORDER_AGENT_STATE_STOPPED, ///< Border agent is stopped/disabled.
        kStateStarted = OT_BORDER_AGENT_STATE_STARTED, ///< Border agent is started.
        kStateActive  = OT_BORDER_AGENT_STATE_ACTIVE,  ///< Border agent is connected with external commissioner.
    };

    /**
     * This constructor initializes the `BorderAgent` object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit BorderAgent(Instance &aInstance);

    /**
     * This method gets the UDP port of this service.
     *
     * @returns  UDP port number.
     *
     */
    uint16_t GetUdpPort(void) const;

    /**
     * This method starts the Border Agent service.
     *
     */
    void Start(void);

    /**
     * This method stops the Border Agent service.
     *
     */
    void Stop(void);

    /**
     * This method gets the state of the Border Agent service.
     *
     * @returns The state of the Border Agent service.
     *
     */
    State GetState(void) const { return mState; }

    /**
     * This method applies the Mesh Local Prefix.
     *
     */
    void ApplyMeshLocalPrefix(void);

    /**
     * This method returns the UDP Proxy port to which the commissioner is currently
     * bound.
     *
     * @returns  The current UDP Proxy port or 0 if no Proxy Transmit has been received yet.
     *
     */
    uint16_t GetUdpProxyPort(void) const { return mUdpProxyPort; }

private:
    class ForwardContext : public InstanceLocatorInit
    {
    public:
        void     Init(Instance &aInstance, const Coap::Message &aMessage, bool aPetition, bool aSeparate);
        bool     IsPetition(void) const { return mPetition; }
        uint16_t GetMessageId(void) const { return mMessageId; }
        Error    ToHeader(Coap::Message &aMessage, uint8_t aCode);

    private:
        uint16_t mMessageId;                             // The CoAP Message ID of the original request.
        bool     mPetition : 1;                          // Whether the forwarding request is leader petition.
        bool     mSeparate : 1;                          // Whether the original request expects separate response.
        uint8_t  mTokenLength : 4;                       // The CoAP Token Length of the original request.
        uint8_t  mType : 2;                              // The CoAP Type of the original request.
        uint8_t  mToken[Coap::Message::kMaxTokenLength]; // The CoAP Token of the original request.
    };

    void HandleNotifierEvents(Events aEvents);

    Coap::Message::Code CoapCodeFromError(Error aError);
    void                SendErrorMessage(ForwardContext &aForwardContext, Error aError);
    void                SendErrorMessage(const Coap::Message &aRequest, bool aSeparate, Error aError);

    static void HandleConnected(bool aConnected, void *aContext);
    void        HandleConnected(bool aConnected);

    template <Coap::Resource BorderAgent::*aResource>
    static void HandleRequest(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);

    static void HandleTimeout(Timer &aTimer);
    void        HandleTimeout(void);

    static void HandleCoapResponse(void *               aContext,
                                   otMessage *          aMessage,
                                   const otMessageInfo *aMessageInfo,
                                   Error                aResult);
    void        HandleCoapResponse(ForwardContext &aForwardContext, const Coap::Message *aResponse, Error aResult);

    Error       ForwardToLeader(const Coap::Message &   aMessage,
                                const Ip6::MessageInfo &aMessageInfo,
                                const char *            aPath,
                                bool                    aPetition,
                                bool                    aSeparate);
    Error       ForwardToCommissioner(Coap::Message &aForwardMessage, const Message &aMessage);
    void        HandleKeepAlive(const Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
    void        HandleRelayTransmit(const Coap::Message &aMessage);
    void        HandleRelayReceive(const Coap::Message &aMessage);
    void        HandleProxyTransmit(const Coap::Message &aMessage);
    static bool HandleUdpReceive(void *aContext, const otMessage *aMessage, const otMessageInfo *aMessageInfo)
    {
        return static_cast<BorderAgent *>(aContext)->HandleUdpReceive(
            *static_cast<const Message *>(aMessage), *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
    }
    bool HandleUdpReceive(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo);

    static constexpr uint32_t kKeepAliveTimeout = 50 * 1000; // Timeout to reject a commissioner.

    Ip6::MessageInfo mMessageInfo;

    Coap::Resource mCommissionerPetition;
    Coap::Resource mCommissionerKeepAlive;
    Coap::Resource mRelayTransmit;
    Coap::Resource mRelayReceive;
    Coap::Resource mCommissionerGet;
    Coap::Resource mCommissionerSet;
    Coap::Resource mActiveGet;
    Coap::Resource mActiveSet;
    Coap::Resource mPendingGet;
    Coap::Resource mPendingSet;
    Coap::Resource mProxyTransmit;

    Ip6::Udp::Receiver         mUdpReceiver; ///< The UDP receiver to receive packets from external commissioner
    Ip6::Netif::UnicastAddress mCommissionerAloc;

    TimerMilli mTimer;
    State      mState;
    uint16_t   mUdpProxyPort;
};

} // namespace MeshCoP
} // namespace ot

#endif // OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE

#endif // BORDER_AGENT_HPP_
