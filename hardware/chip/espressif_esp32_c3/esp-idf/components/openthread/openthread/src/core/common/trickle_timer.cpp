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
 *   This file implements the trickle timer logic.
 */

#include "trickle_timer.hpp"

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/random.hpp"

namespace ot {

TrickleTimer::TrickleTimer(Instance &aInstance, Handler aHandler)
    : TimerMilli(aInstance, TrickleTimer::HandleTimer)
    , mIntervalMin(0)
    , mIntervalMax(0)
    , mInterval(0)
    , mTimeInInterval(0)
    , mRedundancyConstant(0)
    , mCounter(0)
    , mHandler(aHandler)
    , mMode(kModeTrickle)
    , mPhase(kBeforeRandomTime)
{
}

void TrickleTimer::Start(Mode aMode, uint32_t aIntervalMin, uint32_t aIntervalMax, uint16_t aRedundancyConstant)
{
    OT_ASSERT((aIntervalMax >= aIntervalMin) && (aIntervalMin > 0));

    mIntervalMin        = aIntervalMin;
    mIntervalMax        = aIntervalMax;
    mRedundancyConstant = aRedundancyConstant;
    mMode               = aMode;

    // Select interval randomly from range [Imin, Imax].
    mInterval = Random::NonCrypto::GetUint32InRange(mIntervalMin, mIntervalMax + 1);

    StartNewInterval();
}

void TrickleTimer::IndicateConsistent(void)
{
    if (mCounter < kInfiniteRedundancyConstant)
    {
        mCounter++;
    }
}

void TrickleTimer::IndicateInconsistent(void)
{
    VerifyOrExit(mMode == kModeTrickle);

    // If interval is equal to minimum when an "inconsistent" event
    // is received, do nothing.

    VerifyOrExit(IsRunning() && (mInterval != mIntervalMin));

    mInterval = mIntervalMin;
    StartNewInterval();

exit:
    return;
}

void TrickleTimer::StartNewInterval(void)
{
    uint32_t halfInterval;

    switch (mMode)
    {
    case kModePlainTimer:
        mTimeInInterval = mInterval;
        break;

    case kModeTrickle:
        // Select a random point in the interval taken from the range [I/2, I).
        halfInterval = mInterval / 2;
        mTimeInInterval =
            (halfInterval < mInterval) ? Random::NonCrypto::GetUint32InRange(halfInterval, mInterval) : halfInterval;
        mCounter = 0;
        mPhase   = kBeforeRandomTime;
        break;
    }

    TimerMilli::Start(mTimeInInterval);
}

void TrickleTimer::HandleTimer(Timer &aTimer)
{
    static_cast<TrickleTimer *>(&aTimer)->HandleTimer();
}

void TrickleTimer::HandleTimer(void)
{
    switch (mMode)
    {
    case kModePlainTimer:
        mInterval = Random::NonCrypto::GetUint32InRange(mIntervalMin, mIntervalMax + 1);
        StartNewInterval();
        break;

    case kModeTrickle:
        switch (mPhase)
        {
        case kBeforeRandomTime:
            // We reached end of random `mTimeInInterval` (aka `t`)
            // within the current interval. Trickle timer invokes
            // handler if and only if the counter is less than the
            // redundancy constant.

            mPhase = kAfterRandomTime;
            TimerMilli::Start(mInterval - mTimeInInterval);
            VerifyOrExit(mCounter < mRedundancyConstant);
            break;

        case kAfterRandomTime:
            // Interval has expired. Double the interval length and
            // ensure result is below max.

            if (mInterval == 0)
            {
                mInterval = 1;
            }
            else if (mInterval <= mIntervalMax - mInterval)
            {
                mInterval *= 2;
            }
            else
            {
                mInterval = mIntervalMax;
            }

            StartNewInterval();
            ExitNow(); // Exit so to not call `mHanlder`
        }

        break;
    }

    mHandler(*this);

exit:
    return;
}

} // namespace ot
