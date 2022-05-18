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
 *   This file implements link quality information processing and storage.
 */

#include "link_quality.hpp"

#include <stdio.h>

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"

namespace ot {

// This array gives the decimal point digits representing 0/8, 1/8, ..., 7/8 (does not include the '.').
static const char *const kDigitsString[8] = {
    // 0/8,  1/8,   2/8,   3/8,   4/8,   5/8,   6/8,   7/8
    "0", "125", "25", "375", "5", "625", "75", "875"};

void SuccessRateTracker::AddSample(bool aSuccess, uint16_t aWeight)
{
    uint32_t oldAverage = mFailureRate;
    uint32_t newValue   = (aSuccess) ? 0 : kMaxRateValue;
    uint32_t n          = aWeight;

    // `n/2` is added to the sum to ensure rounding the value to the nearest integer when dividing by `n`
    // (e.g., 1.2 -> 1, 3.5 -> 4).

    mFailureRate = static_cast<uint16_t>(((oldAverage * (n - 1)) + newValue + (n / 2)) / n);
}

Error RssAverager::Add(int8_t aRss)
{
    Error    error = kErrorNone;
    uint16_t newValue;

    VerifyOrExit(aRss != OT_RADIO_RSSI_INVALID, error = kErrorInvalidArgs);

    // Restrict the RSS value to the closed range [0, -128] so the RSS times precision multiple can fit in 11 bits.
    if (aRss > 0)
    {
        aRss = 0;
    }

    // Multiply the RSS value by a precision multiple (currently -8).

    newValue = static_cast<uint16_t>(-aRss);
    newValue <<= kPrecisionBitShift;

    mCount += (mCount < (1 << kCoeffBitShift));
    // Maintain arithmetic mean.
    // newAverage = newValue * (1/mCount) + oldAverage * ((mCount -1)/mCount)
    mAverage = static_cast<uint16_t>(((mAverage * (mCount - 1)) + newValue) / mCount);

exit:
    return error;
}

int8_t RssAverager::GetAverage(void) const
{
    int8_t average;

    VerifyOrExit(mCount != 0, average = OT_RADIO_RSSI_INVALID);

    average = -static_cast<int8_t>(mAverage >> kPrecisionBitShift);

    // Check for possible round up (e.g., average of -71.5 --> -72)

    if ((mAverage & kPrecisionBitMask) >= (kPrecision >> 1))
    {
        average--;
    }

exit:
    return average;
}

RssAverager::InfoString RssAverager::ToString(void) const
{
    InfoString string;

    VerifyOrExit(mCount != 0);
    string.Append("%d.%s", -(mAverage >> kPrecisionBitShift), kDigitsString[mAverage & kPrecisionBitMask]);

exit:
    return string;
}

void LqiAverager::Add(uint8_t aLqi)
{
    uint8_t count;

    if (mCount < UINT8_MAX)
    {
        mCount++;
    }
    count = OT_MIN((1 << kCoeffBitShift), mCount);

    mAverage = static_cast<uint8_t>(((mAverage * (count - 1)) + aLqi) / count);
}

void LinkQualityInfo::Clear(void)
{
    mRssAverager.Clear();
    SetLinkQuality(0);
    mLastRss = OT_RADIO_RSSI_INVALID;

    mFrameErrorRate.Clear();
    mMessageErrorRate.Clear();
}

void LinkQualityInfo::AddRss(int8_t aRss)
{
    uint8_t oldLinkQuality = kNoLinkQuality;

    VerifyOrExit(aRss != OT_RADIO_RSSI_INVALID);

    mLastRss = aRss;

    if (mRssAverager.HasAverage())
    {
        oldLinkQuality = GetLinkQuality();
    }

    SuccessOrExit(mRssAverager.Add(aRss));

    SetLinkQuality(CalculateLinkQuality(GetLinkMargin(), oldLinkQuality));

exit:
    return;
}

uint8_t LinkQualityInfo::GetLinkMargin(void) const
{
    return ConvertRssToLinkMargin(Get<Mac::SubMac>().GetNoiseFloor(), GetAverageRss());
}

LinkQualityInfo::InfoString LinkQualityInfo::ToInfoString(void) const
{
    InfoString string;

    string.Append("aveRss:%s, lastRss:%d, linkQuality:%d", mRssAverager.ToString().AsCString(), GetLastRss(),
                  GetLinkQuality());

    return string;
}

uint8_t LinkQualityInfo::ConvertRssToLinkMargin(int8_t aNoiseFloor, int8_t aRss)
{
    int8_t linkMargin = aRss - aNoiseFloor;

    if (linkMargin < 0 || aRss == OT_RADIO_RSSI_INVALID)
    {
        linkMargin = 0;
    }

    return static_cast<uint8_t>(linkMargin);
}

uint8_t LinkQualityInfo::ConvertLinkMarginToLinkQuality(uint8_t aLinkMargin)
{
    return CalculateLinkQuality(aLinkMargin, kNoLinkQuality);
}

uint8_t LinkQualityInfo::ConvertRssToLinkQuality(int8_t aNoiseFloor, int8_t aRss)
{
    return ConvertLinkMarginToLinkQuality(ConvertRssToLinkMargin(aNoiseFloor, aRss));
}

int8_t LinkQualityInfo::ConvertLinkQualityToRss(int8_t aNoiseFloor, uint8_t aLinkQuality)
{
    int8_t linkmargin = 0;

    switch (aLinkQuality)
    {
    case 3:
        linkmargin = kLinkQuality3LinkMargin;
        break;

    case 2:
        linkmargin = kLinkQuality2LinkMargin;
        break;

    case 1:
        linkmargin = kLinkQuality1LinkMargin;
        break;

    default:
        linkmargin = kLinkQuality0LinkMargin;
        break;
    }

    return linkmargin + aNoiseFloor;
}

uint8_t LinkQualityInfo::CalculateLinkQuality(uint8_t aLinkMargin, uint8_t aLastLinkQuality)
{
    // Static private method to calculate the link quality from a given
    // link margin while taking into account the last link quality
    // value and adding the hysteresis value to the thresholds. If
    // there is no previous value for link quality, the constant
    // kNoLinkQuality should be passed as the second argument.

    uint8_t threshold1, threshold2, threshold3;
    uint8_t linkQuality = 0;

    threshold1 = kThreshold1;
    threshold2 = kThreshold2;
    threshold3 = kThreshold3;

    // Apply the hysteresis threshold based on the last link quality value.

    switch (aLastLinkQuality)
    {
    case 0:
        threshold1 += kHysteresisThreshold;

        OT_FALL_THROUGH;

    case 1:
        threshold2 += kHysteresisThreshold;

        OT_FALL_THROUGH;

    case 2:
        threshold3 += kHysteresisThreshold;

        OT_FALL_THROUGH;

    default:
        break;
    }

    if (aLinkMargin > threshold3)
    {
        linkQuality = 3;
    }
    else if (aLinkMargin > threshold2)
    {
        linkQuality = 2;
    }
    else if (aLinkMargin > threshold1)
    {
        linkQuality = 1;
    }

    return linkQuality;
}

} // namespace ot
