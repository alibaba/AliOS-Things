/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 *   This file includes definitions for time instance.
 */

#ifndef TIME_HPP_
#define TIME_HPP_

#include "openthread-core-config.h"

#include <stddef.h>
#include <stdint.h>

#include "common/equatable.hpp"

namespace ot {

/**
 * @addtogroup core-timer
 *
 * @brief
 *   This module includes definitions for the time instance.
 *
 * @{
 *
 */

/**
 * This class represents a time instance.
 *
 */
class Time : public Unequatable<Time>
{
public:
    /**
     * This constant defines a maximum time duration ensured to be longer than any other duration.
     *
     */
    static const uint32_t kMaxDuration = ~static_cast<uint32_t>(0UL);

    /**
     * This is the default constructor for a `Time` object.
     *
     */
    Time(void) = default;

    /**
     * This constructor initializes a `Time` object with a given value.
     *
     * @param[in] aValue   The numeric time value to initialize the `Time` object.
     *
     */
    explicit Time(uint32_t aValue) { SetValue(aValue); }

    /**
     * This method gets the numeric time value associated with the `Time` object.
     *
     * @returns The numeric `Time` value.
     *
     */
    uint32_t GetValue(void) const { return mValue; }

    /**
     * This method sets the numeric time value.
     *
     * @param[in] aValue   The numeric time value.
     *
     */
    void SetValue(uint32_t aValue) { mValue = aValue; }

    /**
     * This method calculates the time duration between two `Time` instances.
     *
     * @note Expression `(t1 - t2)` returns the duration of the interval starting from `t2` and ending at `t1`. When
     * calculating the duration, `t2 is assumed to be in the past relative to `t1`. The duration calculation correctly
     * takes into account the wrapping of numeric value of `Time` instances. The returned value can span the entire
     * range of the `uint32_t` type.
     *
     * @param[in]   aOther  A `Time` instance to subtract from.
     *
     * @returns The duration of interval from @p aOther to this `Time` object.
     *
     */
    uint32_t operator-(const Time &aOther) const { return mValue - aOther.mValue; }

    /**
     * This method returns a new `Time` which is ahead of this `Time` object by a given duration.
     *
     * @param[in]   aDuration  A duration.
     *
     * @returns A new `Time` which is ahead of this object by @aDuration.
     *
     */
    Time operator+(uint32_t aDuration) const { return Time(mValue + aDuration); }

    /**
     * This method returns a new `Time` which is behind this `Time` object by a given duration.
     *
     * @param[in]   aDuration  A duration.
     *
     * @returns A new `Time` which is behind this object by @aDuration.
     *
     */
    Time operator-(uint32_t aDuration) const { return Time(mValue - aDuration); }

    /**
     * This method moves this `Time` object forward by a given duration.
     *
     * @param[in]   aDuration  A duration.
     *
     */
    void operator+=(uint32_t aDuration) { mValue += aDuration; }

    /**
     * This method moves this `Time` object backward by a given duration.
     *
     * @param[in]   aDuration  A duration.
     *
     */
    void operator-=(uint32_t aDuration) { mValue -= aDuration; }

    /**
     * This method indicates whether two `Time` instances are equal.
     *
     * @param[in]   aOther   A `Time` instance to compare with.
     *
     * @retval TRUE    The two `Time` instances are equal.
     * @retval FALSE   The two `Time` instances are not equal.
     *
     */
    bool operator==(const Time &aOther) const { return mValue == aOther.mValue; }

    /**
     * This method indicates whether this `Time` instance is strictly before another one.
     *
     * @note The comparison operators correctly take into account the wrapping of `Time` numeric value. For a given
     * `Time` instance `t0`, any `Time` instance `t` where `(t - t0)` is less than half the range of `uint32_t` type
     * is considered to be after `t0`, otherwise it is considered to be before 't0' (or equal to it). As an example
     * to illustrate this model we can use clock hours: If we are at hour 12, hours 1 to 5 are considered to be
     * after 12, and hours 6 to 11 are considered to be before 12.
     *
     * @param[in]   aOther   A `Time` instance to compare with.
     *
     * @retval TRUE    This `Time` instance is strictly before @p aOther.
     * @retval FALSE   This `Time` instance is not strictly before @p aOther.
     *
     */
    bool operator<(const Time &aOther) const { return ((mValue - aOther.mValue) & (1UL << 31)) != 0; }

    /**
     * This method indicates whether this `Time` instance is after or equal to another one.
     *
     * @param[in]   aOther   A `Time` instance to compare with.
     *
     * @retval TRUE    This `Time` instance is after or equal to @p aOther.
     * @retval FALSE   This `Time` instance is not after or equal to @p aOther.
     *
     */
    bool operator>=(const Time &aOther) const { return !(*this < aOther); }

    /**
     * This method indicates whether this `Time` instance is before or equal to another one.
     *
     * @param[in]   aOther   A `Time` instance to compare with.
     *
     * @retval TRUE    This `Time` instance is before or equal to @p aOther.
     * @retval FALSE   This `Time` instance is not before or equal to @p aOther.
     *
     */
    bool operator<=(const Time &aOther) const { return (aOther >= *this); }

    /**
     * This method indicates whether this `Time` instance is strictly after another one.
     *
     * @param[in]   aOther   A `Time` instance to compare with.
     *
     * @retval TRUE    This `Time` instance is strictly after @p aOther.
     * @retval FALSE   This `Time` instance is not strictly after @p aOther.
     *
     */
    bool operator>(const Time &aOther) const { return (aOther < *this); }

    /**
     * This method returns a new `Time` instance which is in distant future relative to current `Time` object.
     *
     * The returned distance future `Time` is guaranteed to be equal or after (as defined by comparison operator `<=`)
     * any other `Time` which is after this `Time` object, i.e., for any `t` for which we have `*this <= t`, it is
     * ensured that `t <= this->GetGetDistantFuture()`.
     *
     * @returns A new `Time` in distance future relative to current `Time` object.
     *
     */
    Time GetDistantFuture(void) const { return Time(mValue + kDistantFuture); }

    /**
     * This method returns a new `Time` instance which is in distant past relative to current `Time` object.
     *
     * The returned distance past `Time` is guaranteed to be equal or before (as defined by comparison operator `>=`)
     * any other `Time` which is before this `Time` object, i.e., for any `t` for which we have `*this >= t`, it is
     * ensured that `t >= this->GetDistantPast()`.
     *
     * @returns A new `Time` in distance past relative to current `Time` object.
     *
     */
    Time GetDistantPast(void) const { return Time(mValue - kDistantFuture); }

    /**
     * This static method converts a given number of seconds to milliseconds.
     *
     * @returns The number of milliseconds.
     *
     */
    static uint32_t SecToMsec(uint32_t aSeconds) { return aSeconds * 1000u; }

    /**
     * This static method converts a given number of milliseconds to seconds.
     *
     * @returns The number of seconds.
     *
     */
    static uint32_t MsecToSec(uint32_t aMilliseconds) { return aMilliseconds / 1000u; }

private:
    static constexpr uint32_t kDistantFuture = (1UL << 31);

    uint32_t mValue;
};

/**
 * This type represents a time instance (millisecond time).
 *
 */
typedef Time TimeMilli;

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE

/**
 * This type represents a time instance (microsecond time).
 *
 */
typedef Time TimeMicro;

#endif // OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE

/**
 * @}
 *
 */

} // namespace ot

#endif // TIME_HPP_
