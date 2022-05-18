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
 *   This file includes definitions for the trickle timer logic.
 */

#ifndef TRICKLE_TIMER_HPP_
#define TRICKLE_TIMER_HPP_

#include "openthread-core-config.h"

#include "common/numeric_limits.hpp"
#include "common/timer.hpp"

namespace ot {

/**
 * @addtogroup core-timer-trickle
 *
 * @brief
 *   This module includes definitions for the trickle timer logic.
 *
 * @{
 *
 */

/**
 * This class implements a trickle timer.
 *
 */
class TrickleTimer : public TimerMilli
{
public:
    /**
     * This enumeration defines the modes of operation for the `TrickleTimer`.
     *
     */
    enum Mode : uint8_t
    {
        kModeTrickle,    ///< Operate as the normal trickle logic (as per RFC 6206).
        kModePlainTimer, ///< Operate as a plain periodic timer with random interval selected within min/max intervals.
    };

    /**
     * Special value for redundancy constant (aka `k`) to indicate infinity (when used, it disables trickle timer's
     * suppression behavior, invoking the handler callback independent of number of "consistent" events).
     *
     */
    static constexpr uint16_t kInfiniteRedundancyConstant = NumericLimits<uint16_t>::kMax;

    /**
     * This function pointer is called when the timer expires (i.e., transmission should happen).
     *
     * @param[in]  aTimer  A reference to the trickle timer.
     *
     */
    typedef void (&Handler)(TrickleTimer &aTimer);

    /**
     * This constructor initializes a `TrickleTimer` instance.
     *
     * @param[in]  aInstance   A reference to the OpenThread instance.
     * @param[in]  aHandler    A handler which is called when transmission should occur.
     *
     */
    TrickleTimer(Instance &aInstance, Handler aHandler);

    /**
     * This method indicates whether or not the trickle timer instance is running.
     *
     * @retval TRUE   If the trickle timer is running.
     * @retval FALSE  If the trickle timer is not running.
     *
     */
    bool IsRunning(void) const { return TimerMilli::IsRunning(); }

    /**
     * This method gets the current operation mode of the trickle timer.
     *
     * @returns The current operation mode of the timer.
     *
     */
    Mode GetMode(void) const { return mMode; }

    /**
     * This method starts the trickle timer.
     *
     * @param[in]  aMode                The operation mode of timer (trickle or plain periodic mode).
     * @param[in]  aIntervalMin         The minimum interval for the timer in milliseconds.
     * @param[in]  aIntervalMax         The maximum interval for the timer in milliseconds.
     * @param[in]  aRedundancyConstant  The redundancy constant for the timer, also known as `k`. The default value
     *                                  is set to `kInfiniteRedundancyConstant` which disables the suppression behavior
     *                                  (i.e., handler is always invoked independent of number of "consistent" events).
     *
     */
    void Start(Mode     aMode,
               uint32_t aIntervalMin,
               uint32_t aIntervalMax,
               uint16_t aRedundancyConstant = kInfiniteRedundancyConstant);

    /**
     * This method stops the trickle timer.
     *
     */
    void Stop(void) { TimerMilli::Stop(); }

    /**
     * This method indicates to the trickle timer a 'consistent' event.
     *
     * The 'consistent' events are used to control suppression behavior. The trickle timer keeps track of the number of
     * 'consistent' events in each interval. The timer handler is invoked only if the number of `consistent` events
     * received in the interval is less than the redundancy constant.
     *
     */
    void IndicateConsistent(void);

    /**
     * This method indicates to the trickle timer an 'inconsistent' event.
     *
     * Receiving an 'inconsistent' event causes the trickle timer to reset (i.e., start with interval set to the min
     * value) unless the current interval being used is already equal to the min interval.
     *
     */
    void IndicateInconsistent(void);

private:
    enum Phase : uint8_t
    {
        kBeforeRandomTime, // Trickle timer is before random time `t` in the current interval.
        kAfterRandomTime,  // Trickle timer is after random time `t` in the current interval.
    };

    void        StartNewInterval(void);
    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);
    void        HandleEndOfTimeInInterval(void);
    void        HandleEndOfInterval(void);

    // Shadow base class `TimerMilli` methods to ensure they are hidden.
    void StartAt(void) {}
    void FireAt(void) {}
    void FireAtIfEarlier(void) {}
    void GetFireTime(void) {}

    uint32_t mIntervalMin;        // Minimum interval (aka `Imin`).
    uint32_t mIntervalMax;        // Maximum interval (aka `Imax`).
    uint32_t mInterval;           // Current interval (aka `I`).
    uint32_t mTimeInInterval;     // Time in interval (aka `t`).
    uint16_t mRedundancyConstant; // Redundancy constant (aka 'k').
    uint16_t mCounter;            // A counter for number of "consistent" transmissions (aka 'c').
    Handler  mHandler;            // Handler callback.
    Mode     mMode;               // Trickle timer operation mode.
    Phase    mPhase;              // Trickle timer phase (before or after time `t` in the current interval).
};

/**
 * @}
 *
 */

} // namespace ot

#endif // TRICKLE_TIMER_HPP_
