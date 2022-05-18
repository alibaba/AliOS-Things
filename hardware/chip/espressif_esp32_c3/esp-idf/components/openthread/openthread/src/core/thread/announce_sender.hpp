/*
 *  Copyright (c) 2016-2018, The OpenThread Authors.
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
 *   This file includes definition for AnnounceSender.
 */

#ifndef ANNOUNCE_SENDER_HPP_
#define ANNOUNCE_SENDER_HPP_

#include "openthread-core-config.h"

#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/notifier.hpp"
#include "common/timer.hpp"
#include "common/trickle_timer.hpp"
#include "mac/mac.hpp"

namespace ot {

/**
 * This class implements the base class for an `AnnounceSender` and `AnnounceBeginSever`.
 *
 * This class provides APIs to schedule periodic transmission of MLE Announcement messages for a given number
 * transmissions per channel.
 */
class AnnounceSenderBase : public InstanceLocator, private NonCopyable
{
protected:
    /**
     * This constant defines the special channel value to start from the first channel in the channel mask.
     *
     */
    static constexpr uint8_t kChannelIteratorFirst = Mac::ChannelMask::kChannelIteratorFirst;

    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance   A reference to the OpenThread instance.
     * @param[in]  aHandler    A timer handler provided by sub-class.
     *
     */
    AnnounceSenderBase(Instance &aInstance, Timer::Handler aHandler);

    /**
     * This method schedules the MLE Announce transmissions.
     *
     * This method requests @p aCount additional MLE transmission cycles to be scheduled. Each cycle covers all the
     * channels in the specified channel mask from `GetChannelMask()`, with `GetPeriod()` as the time interval between
     * any two successive MLE Announcement transmissions (possibly) on different channels from the mask. The
     * `GetJitter()` value is used to add a random interval from `[-jitter, jitter]` to each period interval.
     *
     * If a "starting channel" is specified using `SetStartingChannel()`, then the cycle starts with MLE Announce
     * transmission on the given starting channel. Otherwise the cycle starts with the first channel (with smallest
     * channel number) in the mask.
     *
     * If a previously requested MLE Announce transmission is still ongoing, a subsequent call to this method adds
     * the @p aCount cycles to the schedule.
     *
     * If there is no active MLE Announce transmission, after call to `SendAnnounce()`, the first MLE Announce
     * transmission happens within a short random interval selected from range `[0, jitter]`.
     *
     * @param[in]  aCount     The number of cycles to schedule.
     *
     */
    void SendAnnounce(uint8_t aCount);

    /**
     * This method stops the ongoing MLE Announce transmissions.
     *
     */
    void Stop(void);

    /**
     * This method indicates whether a previously scheduled MLE Announce transmission is currently in progress or is
     * finished.
     *
     * @returns TRUE if the MLE Announce transmission is in progress, FALSE otherwise.
     *
     */
    bool IsRunning(void) const { return mTimer.IsRunning(); }

    /**
     * This method gets the period interval.
     *
     * @returns The period interval (in milliseconds).
     *
     */
    uint32_t GetPeriod(void) const { return mPeriod; }

    /**
     * This method sets the period interval.
     *
     * The period along with jitter value from (`Get/SetJitter()`) determines the interval between two successive MLE
     * Announcement transmissions (possibly) on different channels from the specified channel mask.
     *
     * @param[in] aPeriod   The period interval (in milliseconds).
     *
     */
    void SetPeriod(uint32_t aPeriod) { mPeriod = aPeriod; }

    /**
     * This method gets the current jitter interval.
     *
     * @returns The jitter interval (in milliseconds).
     *
     */
    uint16_t GetJitter(void) const { return mJitter; }

    /**
     * This method sets the jitter interval.
     *
     * @param[in] aJitter  The jitter interval (in milliseconds).
     *
     */
    void SetJitter(uint16_t aJitter) { mJitter = aJitter; }

    /**
     * This method gets the channel mask.
     *
     * @returns The channel mask.
     *
     */
    const Mac::ChannelMask GetChannelMask(void) const { return mChannelMask; }

    /**
     * This method sets the channel mask.
     *
     * @param[in] aChannelMask   The channel mask.
     *
     */
    void SetChannelMask(Mac::ChannelMask aChannelMask);

    /**
     * This method gets the starting channel, i.e., the first channel in a TX cycle to send MLE Announcement on.
     *
     * @returns The current starting channel.
     *
     */
    uint8_t GetStartingChannel(void) const { return mStartingChannel; }

    /**
     * This method sets the starting channel, i.e., the first channel in a TX cycle to send MLE Announcement on.
     *
     * @p aStartingChannel MUST be present in the current channel mask (from `GetChannelMask()`), otherwise it is
     * ignored and an MLE transmission cycle starts with the first channel (with smallest channel number) in the channel
     * mask.
     *
     * @param[in] aStartingChannel  The starting channel.
     *
     */
    void SetStartingChannel(uint8_t aStartingChannel);

    /**
     * This method is the timer handler and must be invoked by sub-class when the timer expires from the `aHandler`
     * callback function provided in the constructor.
     *
     */
    void HandleTimer(void);

private:
    void SelectStartingChannel(void);

    Mac::ChannelMask mChannelMask;
    uint32_t         mPeriod;
    uint16_t         mJitter;
    uint8_t          mCount;
    uint8_t          mChannel;
    uint8_t          mStartingChannel;
    TimerMilli       mTimer;
};

#if OPENTHREAD_CONFIG_ANNOUNCE_SENDER_ENABLE

/**
 * This class implements an AnnounceSender.
 *
 */
class AnnounceSender : public AnnounceSenderBase
{
    friend class ot::Notifier;

public:
    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance   A reference to the OpenThread instance.
     *
     */
    explicit AnnounceSender(Instance &aInstance);

    /**
     * This method notifies the `AnnounceSender` that a MLE Announcement message was received with a current timestamp
     * to update its internal state (decide whether or not to skip transmission of MLE Announcement in this cycle).
     *
     */
    void UpdateOnReceivedAnnounce(void);

private:
    // Specifies the time interval (in milliseconds) between
    // `AnnounceSender` transmit cycles. Within a cycle, device
    // sends MLE Announcements on all channels from Active
    // Dataset's channel mask.
    static constexpr uint32_t kInterval = OPENTHREAD_CONFIG_ANNOUNCE_SENDER_INTERVAL;

    // Specifies the sub-interval (in millisecond) within a cycle
    // to send MLE announcement messages. We use half of the cycle
    // interval length for transmission. This ensures that the
    // transmissions are finished before device needs to make a
    // decision about the next cycle. This time is divided by the
    // number of channels to determine the time between two
    // successive Announce tx (on different channels).
    static constexpr uint32_t kTxInterval = (kInterval / 2 + 1);

    // Specifies the number of MLE Announcement messages that the
    // device must receive within a cycle interval to skip sending the
    // Announcement itself. This is used as `mTrickleTimer` redundancy
    // constant in `AnnounceSender`.
    static constexpr uint16_t kRedundancyConstant = OPENTHREAD_CONFIG_ANNOUNCE_SENDER_REDUNDANCY_CONSTANT;

    // Jitter interval (in milliseconds) applied to the period
    // between any two successive MLE Announcement transmissions
    // (possibly) on different channels.
    static constexpr uint16_t kMaxJitter = OPENTHREAD_CONFIG_ANNOUNCE_SENDER_JITTER_INTERVAL;

    void        Stop(void);
    static void HandleTimer(Timer &aTimer);
    static void HandleTrickleTimer(TrickleTimer &aTimer);
    void        HandleTrickleTimer(void);
    void        HandleNotifierEvents(Events aEvents);
    void        HandleRoleChanged(void);
    void        HandleActiveDatasetChanged(void);
    void        HandleThreadChannelChanged(void);

    TrickleTimer mTrickleTimer;
};

#endif // OPENTHREAD_CONFIG_ANNOUNCE_SENDER_ENABLE

/**
 * @}
 */

} // namespace ot

#endif // ANNOUNCE_SENDER_HPP_
