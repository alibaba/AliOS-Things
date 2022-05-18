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
 *   This file includes definitions for Thread network time synchronization service.
 */

#ifndef TIME_SYNC_HPP_
#define TIME_SYNC_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE

#include <openthread/network_time.h>

#include "common/locator.hpp"
#include "common/message.hpp"
#include "common/non_copyable.hpp"
#include "common/notifier.hpp"
#include "common/timer.hpp"

namespace ot {

/**
 * This class implements OpenThread Time Synchronization Service.
 *
 */
class TimeSync : public InstanceLocator, private NonCopyable
{
    friend class ot::Notifier;

public:
    /**
     * This constructor initializes the object.
     *
     */
    TimeSync(Instance &aInstance);

    /**
     * Get the Thread network time.
     *
     * @param[inout] aNetworkTime  The Thread network time in microseconds.
     *
     * @returns The time synchronization status.
     *
     */
    otNetworkTimeStatus GetTime(uint64_t &aNetworkTime) const;

    /**
     * Handle the message which includes time synchronization information.
     *
     * @param[in] aMessage  The message which includes time synchronization information.
     *
     */
    void HandleTimeSyncMessage(const Message &aMessage);

#if OPENTHREAD_FTD
    /**
     * Send a time synchronization message when it is required.
     *
     * Note: A time synchronization message is required in following cases:
     *       1. Leader send time sync message periodically;
     *       2. Router(except Leader) received a time sync message with newer sequence;
     *       3. Router received a time sync message with older sequence.
     *
     */
    void ProcessTimeSync(void);
#endif

    /**
     * This method gets the time synchronization sequence.
     *
     * @returns The time synchronization sequence.
     *
     */
    uint8_t GetTimeSyncSeq(void) const { return mTimeSyncSeq; }

    /**
     * This method gets the time offset to the Thread network time.
     *
     * @returns The time offset to the Thread network time, in microseconds.
     *
     */
    int64_t GetNetworkTimeOffset(void) const { return mNetworkTimeOffset; }

    /**
     * Set the time synchronization period.
     *
     * @param[in] aTimeSyncPeriod   The time synchronization period, in seconds.
     *
     */
    void SetTimeSyncPeriod(uint16_t aTimeSyncPeriod) { mTimeSyncPeriod = aTimeSyncPeriod; }

    /**
     * Get the time synchronization period.
     *
     * @returns The time synchronization period, in seconds.
     *
     */
    uint16_t GetTimeSyncPeriod(void) const { return mTimeSyncPeriod; }

    /**
     * Set the time synchronization XTAL accuracy threshold for Router.
     *
     * @param[in] aXtalThreshold   The XTAL accuracy threshold for Router, in PPM.
     *
     */
    void SetXtalThreshold(uint16_t aXtalThreshold) { mXtalThreshold = aXtalThreshold; }

    /**
     * Get the time synchronization XTAL accuracy threshold for Router.
     *
     * @returns The XTAL accuracy threshold for Router, in PPM.
     *
     */
    uint16_t GetXtalThreshold(void) const { return mXtalThreshold; }

    /**
     * Set the time sync callback to be notified of a network time update.
     *
     * @param[in] aCallback The callback to be called when time sync is handled.
     * @param[in] aCallbackContext The context to be passed to callback.
     *
     */
    void SetTimeSyncCallback(otNetworkTimeSyncCallbackFn aCallback, void *aCallbackContext)
    {
        mTimeSyncCallback        = aCallback;
        mTimeSyncCallbackContext = aCallbackContext;
    }

    /**
     * Callback to be called when timer expires.
     *
     */
    void HandleTimeout(void);

private:
    /**
     * Callback to be called when thread state changes.
     *
     * @param[in] aFlags Flags that denote the state change events.
     *
     */
    void HandleNotifierEvents(Events aEvents);

    /**
     * Callback to be called when timer expires.
     *
     * @param[in] aTimer The corresponding timer.
     *
     */
    static void HandleTimeout(Timer &aTimer);

    /**
     * Check and handle any status change, and notify observers if applicable.
     *
     * @param[in] aNotifyTimeUpdated True to denote that observers should be notified due to a time change, false
     * otherwise.
     *
     */
    void CheckAndHandleChanges(bool aNotifyTimeUpdated);

    /**
     * Increase the time synchronization sequence.
     *
     */
    void IncrementTimeSyncSeq(void);

    /**
     * Notify any listener of a network time sync update event.
     *
     */
    void NotifyTimeSyncCallback(void);

    bool     mTimeSyncRequired; ///< Indicate whether or not a time synchronization message is required.
    uint8_t  mTimeSyncSeq;      ///< The time synchronization sequence.
    uint16_t mTimeSyncPeriod;   ///< The time synchronization period.
    uint16_t mXtalThreshold;    ///< The XTAL accuracy threshold for a device to become a Router, in PPM.
#if OPENTHREAD_FTD
    TimeMilli mLastTimeSyncSent; ///< The time when the last time synchronization message was sent.
#endif
    TimeMilli mLastTimeSyncReceived; ///< The time when the last time synchronization message was received.
    int64_t   mNetworkTimeOffset;    ///< The time offset to the Thread Network time
    otNetworkTimeSyncCallbackFn
                        mTimeSyncCallback; ///< The callback to be called when time sync is handled or status updated.
    void *              mTimeSyncCallbackContext; ///< The context to be passed to callback.
    TimerMilli          mTimer;                   ///< Timer for checking if a resync is required.
    otNetworkTimeStatus mCurrentStatus;           ///< Current network time status.
};

/**
 * @}
 */

} // namespace ot

#endif // OPENTHREAD_CONFIG_TIME_SYNC_ENABLE

#endif // TIME_SYNC_HPP_
