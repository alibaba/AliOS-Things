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
 *   This file includes definitions to support ping functionality.
 */

#ifndef PING_SENDER_HPP_
#define PING_SENDER_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE

#include <openthread/ping_sender.h>

#include "common/code_utils.hpp"
#include "common/locator.hpp"
#include "common/message.hpp"
#include "common/non_copyable.hpp"
#include "common/numeric_limits.hpp"
#include "common/time.hpp"
#include "common/timer.hpp"
#include "net/icmp6.hpp"
#include "net/ip6_address.hpp"

namespace ot {
namespace Utils {

/**
 * This class implements sending ICMPv6 Echo Request messages and processing ICMPv6 Echo Reply messages.
 *
 */
class PingSender : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This class represents a ping reply.
     *
     */
    typedef otPingSenderReply Reply;

    /**
     * This class represents the statistics of several ping requests.
     *
     */
    struct Statistics : public otPingSenderStatistics
    {
        Statistics(void) { Clear(); }

        void Clear(void)
        {
            mSentCount          = 0;
            mReceivedCount      = 0;
            mTotalRoundTripTime = 0;
            mMinRoundTripTime   = NumericLimits<uint16_t>::kMax;
            mMaxRoundTripTime   = NumericLimits<uint16_t>::kMin;
            mIsMulticast        = false;
        }
    };

    /**
     * This class represents a ping request configuration.
     *
     */
    class Config : public otPingSenderConfig
    {
        friend class PingSender;

    public:
        /**
         * This method gets the source IPv6 address of the ping.
         *
         * @returns The ping source IPv6 address.
         *
         */
        Ip6::Address &GetSource(void) { return static_cast<Ip6::Address &>(mSource); }

        /**
         * This method gets the source IPv6 address of the ping.
         *
         * @returns The ping source IPv6 address.
         *
         */
        const Ip6::Address &GetSource(void) const { return static_cast<const Ip6::Address &>(mSource); }

        /**
         * This method gets the destination IPv6 address to ping.
         *
         * @returns The ping destination IPv6 address.
         *
         */
        Ip6::Address &GetDestination(void) { return static_cast<Ip6::Address &>(mDestination); }

        /**
         * This method gets the destination IPv6 address to ping.
         *
         * @returns The ping destination IPv6 address.
         *
         */
        const Ip6::Address &GetDestination(void) const { return static_cast<const Ip6::Address &>(mDestination); }

    private:
        static constexpr uint16_t kDefaultSize     = OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_SIZE;
        static constexpr uint16_t kDefaultCount    = OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_COUNT;
        static constexpr uint32_t kDefaultInterval = OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_INTEVRAL;
        static constexpr uint32_t kDefaultTimeout  = OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_TIMEOUT;

        void SetUnspecifiedToDefault(void);
        void InvokeReplyCallback(const Reply &aReply) const;
        void InvokeStatisticsCallback(const Statistics &aStatistics) const;
    };

    /**
     * This constructor initializes the `PingSender` object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit PingSender(Instance &aInstance);

    /**
     * This method starts a ping.
     *
     * @param[in] aConfig          The ping config to use.
     *
     * @retval kErrorNone          The ping started successfully.
     * @retval kErrorBusy          Could not start since busy with a previous ongoing ping request.
     * @retval kErrorInvalidArgs   The @p aConfig contains invalid parameters (e.g., ping interval is too long).
     *
     */
    Error Ping(const Config &aConfig);

    /**
     * This method stops an ongoing ping.
     *
     */
    void Stop(void);

private:
    void        SendPing(void);
    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);
    static void HandleIcmpReceive(void *               aContext,
                                  otMessage *          aMessage,
                                  const otMessageInfo *aMessageInfo,
                                  const otIcmp6Header *aIcmpHeader);
    void        HandleIcmpReceive(const Message &          aMessage,
                                  const Ip6::MessageInfo & aMessageInfo,
                                  const Ip6::Icmp::Header &aIcmpHeader);

    Config             mConfig;
    Statistics         mStatistics;
    uint16_t           mIdentifier;
    uint16_t           mTargetEchoSequence;
    TimerMilli         mTimer;
    Ip6::Icmp::Handler mIcmpHandler;
};

} // namespace Utils
} // namespace ot

#endif // OPENTHREAD_CONFIG_PING_SENDER_ENABLE

#endif // PING_SENDER_HPP_
