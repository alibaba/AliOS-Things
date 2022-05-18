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
 *  This file includes definitions for the Joiner Router role.
 */

#ifndef JOINER_ROUTER_HPP_
#define JOINER_ROUTER_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_FTD

#include "coap/coap.hpp"
#include "coap/coap_message.hpp"
#include "common/locator.hpp"
#include "common/message.hpp"
#include "common/non_copyable.hpp"
#include "common/notifier.hpp"
#include "common/timer.hpp"
#include "mac/mac_types.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "net/udp6.hpp"
#include "thread/key_manager.hpp"

namespace ot {

namespace MeshCoP {

class JoinerRouter : public InstanceLocator, private NonCopyable
{
    friend class ot::Notifier;

public:
    /**
     * This constructor initializes the Joiner Router object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit JoinerRouter(Instance &aInstance);

    /**
     * This method returns the Joiner UDP Port.
     *
     * @returns The Joiner UDP Port number .
     *
     */
    uint16_t GetJoinerUdpPort(void);

    /**
     * This method sets the Joiner UDP Port.
     *
     * @param[in]  The Joiner UDP Port number.
     *
     */
    void SetJoinerUdpPort(uint16_t aJoinerUdpPort);

private:
    static constexpr uint32_t kJoinerEntrustTxDelay = 50; // in msec

    struct JoinerEntrustMetadata
    {
        Error AppendTo(Message &aMessage) const { return aMessage.Append(*this); }
        void  ReadFrom(const Message &aMessage);

        Ip6::MessageInfo mMessageInfo; // Message info of the message to send.
        TimeMilli        mSendTime;    // Time when the message shall be sent.
        Kek              mKek;         // KEK used by MAC layer to encode this message.
    };

    void HandleNotifierEvents(Events aEvents);

    static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleUdpReceive(Message &aMessage, const Ip6::MessageInfo &aMessageInfo);

    static void HandleRelayTransmit(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleRelayTransmit(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);

    static void HandleJoinerEntrustResponse(void *               aContext,
                                            otMessage *          aMessage,
                                            const otMessageInfo *aMessageInfo,
                                            Error                aResult);
    void HandleJoinerEntrustResponse(Coap::Message *aMessage, const Ip6::MessageInfo *aMessageInfo, Error aResult);

    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);

    void           Start(void);
    void           DelaySendingJoinerEntrust(const Ip6::MessageInfo &aMessageInfo, const Kek &aKek);
    void           SendDelayedJoinerEntrust(void);
    Error          SendJoinerEntrust(const Ip6::MessageInfo &aMessageInfo);
    Coap::Message *PrepareJoinerEntrustMessage(void);

    Ip6::Udp::Socket mSocket;
    Coap::Resource   mRelayTransmit;

    TimerMilli   mTimer;
    MessageQueue mDelayedJoinEnts;

    uint16_t mJoinerUdpPort;

    bool mIsJoinerPortConfigured : 1;
};

} // namespace MeshCoP
} // namespace ot

#endif // OPENTHREAD_FTD

#endif // JOINER_ROUTER_HPP_
