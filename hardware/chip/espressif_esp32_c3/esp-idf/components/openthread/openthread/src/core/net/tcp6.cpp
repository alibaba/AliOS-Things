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
 *   This file implements TCP/IPv6 sockets.
 */

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_TCP_ENABLE

#include "tcp6.hpp"

#include "common/code_utils.hpp"
#include "common/error.hpp"
#include "common/instance.hpp"
#include "common/logging.hpp"
#include "net/ip6.hpp"

namespace ot {
namespace Ip6 {

Tcp::Tcp(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mTimer(aInstance, Tcp::HandleTimer)
    , mEphemeralPort(kDynamicPortMin)
{
    OT_UNUSED_VARIABLE(mEphemeralPort);
}

Error Tcp::Endpoint::Initialize(Instance &aInstance, otTcpEndpointInitializeArgs &aArgs)
{
    Error error;

    SuccessOrExit(error = aInstance.Get<Tcp>().mEndpoints.Add(*this));

    mContext                  = aArgs.mContext;
    mEstablishedCallback      = aArgs.mEstablishedCallback;
    mSendDoneCallback         = aArgs.mSendDoneCallback;
    mSendReadyCallback        = aArgs.mSendReadyCallback;
    mReceiveAvailableCallback = aArgs.mReceiveAvailableCallback;
    mDisconnectedCallback     = aArgs.mDisconnectedCallback;

    mInstance = &aInstance;

    memset(mTimers, 0x00, sizeof(mTimers));

exit:
    return error;
}

Instance &Tcp::Endpoint::GetInstance(void)
{
    return *static_cast<Instance *>(mInstance);
}

const SockAddr &Tcp::Endpoint::GetLocalAddress(void) const
{
    static otSockAddr temp;
    return *static_cast<SockAddr *>(&temp);
}

const SockAddr &Tcp::Endpoint::GetPeerAddress(void) const
{
    static otSockAddr temp;
    return *static_cast<SockAddr *>(&temp);
}

Error Tcp::Endpoint::Bind(const SockAddr &aSockName)
{
    OT_UNUSED_VARIABLE(aSockName);

    return kErrorNotImplemented;
}

Error Tcp::Endpoint::Connect(const SockAddr &aSockName, uint32_t aFlags)
{
    OT_UNUSED_VARIABLE(aSockName);
    OT_UNUSED_VARIABLE(aFlags);

    return kErrorNotImplemented;
}

Error Tcp::Endpoint::SendByReference(otLinkedBuffer &aBuffer, uint32_t aFlags)
{
    OT_UNUSED_VARIABLE(aBuffer);
    OT_UNUSED_VARIABLE(aFlags);

    return kErrorNotImplemented;
}

Error Tcp::Endpoint::SendByExtension(size_t aNumBytes, uint32_t aFlags)
{
    OT_UNUSED_VARIABLE(aNumBytes);
    OT_UNUSED_VARIABLE(aFlags);

    return kErrorNotImplemented;
}

Error Tcp::Endpoint::ReceiveByReference(const otLinkedBuffer *&aBuffer) const
{
    OT_UNUSED_VARIABLE(aBuffer);

    return kErrorNotImplemented;
}

Error Tcp::Endpoint::ReceiveContiguify(void)
{
    return kErrorNotImplemented;
}

Error Tcp::Endpoint::CommitReceive(size_t aNumBytes, uint32_t aFlags)
{
    OT_UNUSED_VARIABLE(aNumBytes);
    OT_UNUSED_VARIABLE(aFlags);

    return kErrorNotImplemented;
}

Error Tcp::Endpoint::SendEndOfStream(void)
{
    return kErrorNotImplemented;
}

Error Tcp::Endpoint::Abort(void)
{
    return kErrorNotImplemented;
}

Error Tcp::Endpoint::Deinitialize(void)
{
    Error error;

    Tcp &tcp = GetInstance().Get<Tcp>();

    SuccessOrExit(error = tcp.mEndpoints.Remove(*this));
    SetNext(nullptr);

exit:
    return error;
}

uint8_t Tcp::Endpoint::TimerFlagToIndex(uint8_t aTimerFlag)
{
    OT_UNUSED_VARIABLE(aTimerFlag);
    /*
     * TODO: Convert from the timer flag provided by TCPlp to the index in
     * our timers array.
     */
    return 0;
}

bool Tcp::Endpoint::IsTimerActive(uint8_t aTimerIndex)
{
    OT_UNUSED_VARIABLE(aTimerIndex);
    /* TODO: Check whether TCPlp has marked this timer as active. */
    return false;
}

void Tcp::Endpoint::SetTimer(uint8_t aTimerFlag, uint32_t aDelay)
{
    /*
     * TCPlp has already set the flag for this timer to record that it's
     * running. So, all that's left to do is record the expiry time and
     * (re)set the main timer as appropriate.
     */

    TimeMilli now         = TimerMilli::GetNow();
    TimeMilli newFireTime = now + aDelay;
    uint8_t   timerIndex  = TimerFlagToIndex(aTimerFlag);

    mTimers[timerIndex] = newFireTime.GetValue();
    otLogDebgTcp("Endpoint %p set timer %u to %u ms", static_cast<void *>(this), static_cast<unsigned int>(timerIndex),
                 static_cast<unsigned int>(aDelay));

    GetInstance().Get<Tcp>().mTimer.FireAtIfEarlier(newFireTime);
}

void Tcp::Endpoint::CancelTimer(uint8_t aTimerFlag)
{
    /*
     * TCPlp has already cleared the timer flag before calling this. Since the
     * main timer's callback properly handles the case where no timers are
     * actually due, there's actually no work to be done here.
     */

    OT_UNUSED_VARIABLE(aTimerFlag);

    otLogDebgTcp("Endpoint %p cancelled timer %u", static_cast<void *>(this),
                 static_cast<unsigned int>(TimerFlagToIndex(aTimerFlag)));
}

bool Tcp::Endpoint::FirePendingTimers(TimeMilli aNow, bool &aHasFutureTimer, TimeMilli &aEarliestFutureExpiry)
{
    /*
     * NOTE: Firing a timer might potentially activate/deactivate other timers.
     * If timers x and y expire at the same time, but the callback for timer x
     * (for x < y) cancels or postpones timer y, should timer y's callback be
     * called? Our answer is no, since timer x's callback has updated the
     * TCP stack's state in such a way that it no longer expects timer y's
     * callback to to be called. Because the TCP stack thinks that timer y
     * has been cancelled, calling timer y's callback could potentially cause
     * problems.
     *
     * If the timer callbacks set other timers, then they may not be taken
     * into account when setting aEarliestFutureExpiry. But mTimer's expiry
     * time will be updated by those, so we can just compare against mTimer's
     * expiry time when resetting mTimer.
     */
    for (uint8_t timerIndex = 0; timerIndex != kNumTimers; timerIndex++)
    {
        if (IsTimerActive(timerIndex))
        {
            TimeMilli expiry(mTimers[timerIndex]);

            if (expiry <= aNow)
            {
                /* TODO: Call TCPlp's callback for this timer. */
                /* If a user callback is called, then return true. */
            }
            else
            {
                aHasFutureTimer       = true;
                aEarliestFutureExpiry = OT_MIN(aEarliestFutureExpiry, expiry);
            }
        }
    }

    return false;
}

Error Tcp::Listener::Initialize(Instance &aInstance, otTcpListenerInitializeArgs &aArgs)
{
    Error error;

    SuccessOrExit(error = aInstance.Get<Tcp>().mListeners.Add(*this));

    mContext             = aArgs.mContext;
    mAcceptReadyCallback = aArgs.mAcceptReadyCallback;
    mAcceptDoneCallback  = aArgs.mAcceptDoneCallback;

    mInstance = &aInstance;

exit:
    return error;
}

Instance &Tcp::Listener::GetInstance(void)
{
    return *static_cast<Instance *>(mInstance);
}

Error Tcp::Listener::Listen(const SockAddr &aSockName)
{
    OT_UNUSED_VARIABLE(aSockName);

    return kErrorNotImplemented;
}

Error Tcp::Listener::StopListening(void)
{
    return kErrorNotImplemented;
}

Error Tcp::Listener::Deinitialize(void)
{
    Error error;

    Tcp &tcp = GetInstance().Get<Tcp>();

    SuccessOrExit(error = tcp.mListeners.Remove(*this));
    SetNext(nullptr);

exit:
    return error;
}

Error Tcp::HandleMessage(ot::Ip6::Header &aIp6Header, Message &aMessage, MessageInfo &aMessageInfo)
{
    OT_UNUSED_VARIABLE(aIp6Header);
    OT_UNUSED_VARIABLE(aMessage);
    OT_UNUSED_VARIABLE(aMessageInfo);

    Error error = kErrorNotImplemented;

    for (Endpoint &active : mEndpoints)
    {
        OT_UNUSED_VARIABLE(active);
    }

    for (Listener &passive : mListeners)
    {
        OT_UNUSED_VARIABLE(passive);
    }

    return error;
}

void Tcp::HandleTimer(Timer &aTimer)
{
    OT_ASSERT(&aTimer == &aTimer.GetInstance().Get<Tcp>().mTimer);
    otLogDebgTcp("Main TCP timer expired");
    aTimer.GetInstance().Get<Tcp>().ProcessTimers();
}

void Tcp::ProcessTimers()
{
    TimeMilli now = TimerMilli::GetNow();
    bool      pendingTimer;
    TimeMilli earliestPendingTimerExpiry;

    OT_ASSERT(!mTimer.IsRunning());

    /*
     * The timer callbacks could potentially set/reset/cancel timers.
     * Importantly, Endpoint::SetTimer and Endpoint::CancelTimer do not call
     * this function to recompute the timer. If they did, we'd have a
     * re-entrancy problem, where the callbacks called in this function could
     * wind up re-entering this function in a nested call frame.
     *
     * In general, calling this function from Endpoint::SetTimer and
     * Endpoint::CancelTimer could be inefficient, since those functions are
     * called multiple times on each received TCP segment. If we want to
     * prevent the main timer from firing except when an actual TCP timer
     * expires, a better alternative is to reset the main timer in
     * HandleMessage, right before processing signals. That would achieve that
     * objective while avoiding re-entrancy issues altogether.
     */
restart:
    pendingTimer               = false;
    earliestPendingTimerExpiry = now.GetDistantFuture();

    for (Endpoint &endpoint : mEndpoints)
    {
        if (endpoint.FirePendingTimers(now, pendingTimer, earliestPendingTimerExpiry))
        {
            /*
             * If a non-OpenThread callback is called --- which, in practice,
             * happens if the connection times out and the user-defined
             * connection lost callback is called --- then we might have to
             * start over. The reason is that the user might deinitialize
             * endpoints, changing the structure of the linked list. For
             * example, if the user deinitializes both this endpoint and the
             * next one in the linked list, then we can't continue traversing
             * the linked list.
             */
            goto restart;
        }
    }

    if (pendingTimer)
    {
        /*
         * We need to use Timer::FireAtIfEarlier instead of timer::FireAt
         * because one of the earlier callbacks might have set TCP timers,
         * in which case `mTimer` would have been set to the earliest of those
         * timers.
         */
        mTimer.FireAtIfEarlier(earliestPendingTimerExpiry);
        otLogDebgTcp("Reset main TCP timer to %u ms", static_cast<unsigned int>(earliestPendingTimerExpiry - now));
    }
    else
    {
        otLogDebgTcp("Did not reset main TCP timer");
    }
}

} // namespace Ip6
} // namespace ot

#endif // OPENTHREAD_CONFIG_TCP_ENABLE
