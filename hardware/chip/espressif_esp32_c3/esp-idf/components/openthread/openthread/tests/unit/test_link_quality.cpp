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

#include "common/code_utils.hpp"
#include "thread/link_quality.hpp"

#include "test_platform.h"
#include "test_util.h"

namespace ot {

static ot::Instance *sInstance;

enum
{
    kMaxRssValue = 0,
    kMinRssValue = -128,

    kStringBuffferSize = 80,

    kRssAverageMaxDiff = 16,
    kNumRssAdds        = 300,

    kRawAverageBitShift = 3,
    kRawAverageMultiple = (1 << kRawAverageBitShift),
    kRawAverageBitMask  = (1 << kRawAverageBitShift) - 1,
};

#define MIN_RSS(_rss1, _rss2) (((_rss1) < (_rss2)) ? (_rss1) : (_rss2))
#define MAX_RSS(_rss1, _rss2) (((_rss1) < (_rss2)) ? (_rss2) : (_rss1))
#define ABS(value) (((value) >= 0) ? (value) : -(value))

// This struct contains RSS values and test data for checking link quality info calss.
struct RssTestData
{
    const int8_t *mRssList;             // Array of RSS values.
    size_t        mRssListSize;         // Size of RSS list.
    uint8_t       mExpectedLinkQuality; // Expected final link quality value.
};

int8_t sNoiseFloor = -100; // dBm

// Check and verify the raw average RSS value to match the value from GetAverage().
void VerifyRawRssValue(int8_t aAverage, uint16_t aRawValue)
{
    if (aAverage != OT_RADIO_RSSI_INVALID)
    {
        VerifyOrQuit(aAverage == -static_cast<int16_t>((aRawValue + (kRawAverageMultiple / 2)) >> kRawAverageBitShift),
                     "Raw value does not match the average.");
    }
    else
    {
        VerifyOrQuit(aRawValue == 0, "Raw value does not match the average.");
    }
}

// This function prints the values in the passed in link info instance. It is invoked as the final step in test-case.
void PrintOutcome(LinkQualityInfo &aLinkInfo)
{
    printf("%s -> PASS \n", aLinkInfo.ToInfoString().AsCString());
}

void TestLinkQualityData(RssTestData aRssData)
{
    LinkQualityInfo linkInfo;
    int8_t          rss, ave, min, max;
    size_t          i;

    sInstance = testInitInstance();
    VerifyOrQuit(sInstance != nullptr);
    linkInfo.Init(*sInstance);

    printf("- - - - - - - - - - - - - - - - - -\n");
    linkInfo.Clear();
    min = kMinRssValue;
    max = kMaxRssValue;

    for (i = 0; i < aRssData.mRssListSize; i++)
    {
        rss = aRssData.mRssList[i];
        min = MIN_RSS(rss, min);
        max = MAX_RSS(rss, max);
        linkInfo.AddRss(rss);
        VerifyOrQuit(linkInfo.GetLastRss() == rss);
        ave = linkInfo.GetAverageRss();
        VerifyOrQuit(ave >= min, "GetAverageRss() is smaller than min value.");
        VerifyOrQuit(ave <= max, "GetAverageRss() is larger than min value");
        VerifyRawRssValue(linkInfo.GetAverageRss(), linkInfo.GetAverageRssRaw());
        printf("%02u) AddRss(%4d): ", static_cast<unsigned int>(i), rss);
        PrintOutcome(linkInfo);
    }

    VerifyOrQuit(linkInfo.GetLinkQuality() == aRssData.mExpectedLinkQuality);
}

// Check and verify the raw average RSS value to match the value from GetAverage().
void VerifyRawRssValue(RssAverager &aRssAverager)
{
    int8_t   average  = aRssAverager.GetAverage();
    uint16_t rawValue = aRssAverager.GetRaw();

    if (average != OT_RADIO_RSSI_INVALID)
    {
        VerifyOrQuit(average == -static_cast<int16_t>((rawValue + (kRawAverageMultiple / 2)) >> kRawAverageBitShift),
                     "Raw value does not match the average.");
    }
    else
    {
        VerifyOrQuit(rawValue == 0, "Raw value does not match the average.");
    }
}

// This function prints the values in the passed link info instance. It is invoked as the final step in test-case.
void PrintOutcome(RssAverager &aRssAverager)
{
    printf("%s -> PASS\n", aRssAverager.ToString().AsCString());
}

int8_t GetRandomRss(void)
{
    uint32_t value;

    value = rand() % 128;
    return -static_cast<int8_t>(value);
}

void TestRssAveraging(void)
{
    RssAverager  rssAverager;
    int8_t       rss, rss2, ave;
    int16_t      diff;
    size_t       i, j, k;
    const int8_t rssValues[] = {kMinRssValue, -70, -40, -41, -10, kMaxRssValue};
    int16_t      sum;

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Values after initialization/reset.

    rssAverager.Clear();

    printf("\nAfter Clear: ");
    VerifyOrQuit(rssAverager.GetAverage() == OT_RADIO_RSSI_INVALID, "Initial value from GetAverage() is incorrect.");
    VerifyRawRssValue(rssAverager);
    PrintOutcome(rssAverager);

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Adding a single value
    rss = -70;
    printf("AddRss(%d): ", rss);
    IgnoreError(rssAverager.Add(rss));
    VerifyOrQuit(rssAverager.GetAverage() == rss, "GetAverage() failed after a single AddRss().");
    VerifyRawRssValue(rssAverager);
    PrintOutcome(rssAverager);

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Clear

    printf("Clear(): ");
    rssAverager.Clear();
    VerifyOrQuit(rssAverager.GetAverage() == OT_RADIO_RSSI_INVALID, "GetAverage() after Clear() is incorrect.");
    VerifyRawRssValue(rssAverager);
    PrintOutcome(rssAverager);

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Adding the same value many times.

    printf("- - - - - - - - - - - - - - - - - -\n");

    for (j = 0; j < sizeof(rssValues); j++)
    {
        rssAverager.Clear();
        rss = rssValues[j];
        printf("AddRss(%4d) %d times: ", rss, kNumRssAdds);

        for (i = 0; i < kNumRssAdds; i++)
        {
            IgnoreError(rssAverager.Add(rss));
            VerifyOrQuit(rssAverager.GetAverage() == rss, "GetAverage() returned incorrect value.");
            VerifyRawRssValue(rssAverager);
        }

        PrintOutcome(rssAverager);
    }

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Adding two RSS values:

    printf("- - - - - - - - - - - - - - - - - -\n");

    for (j = 0; j < sizeof(rssValues); j++)
    {
        rss = rssValues[j];

        for (k = 0; k < sizeof(rssValues); k++)
        {
            if (k == j)
            {
                continue;
            }

            rss2 = rssValues[k];
            rssAverager.Clear();
            IgnoreError(rssAverager.Add(rss));
            IgnoreError(rssAverager.Add(rss2));
            printf("AddRss(%4d), AddRss(%4d): ", rss, rss2);
            VerifyOrQuit(rssAverager.GetAverage() == ((rss + rss2) >> 1), "GetAverage() returned incorrect value.");
            VerifyRawRssValue(rssAverager);
            PrintOutcome(rssAverager);
        }
    }

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Adding one value many times and a different value once:

    printf("- - - - - - - - - - - - - - - - - -\n");

    for (j = 0; j < sizeof(rssValues); j++)
    {
        rss = rssValues[j];

        for (k = 0; k < sizeof(rssValues); k++)
        {
            if (k == j)
            {
                continue;
            }

            rss2 = rssValues[k];
            rssAverager.Clear();

            for (i = 0; i < kNumRssAdds; i++)
            {
                IgnoreError(rssAverager.Add(rss));
            }

            IgnoreError(rssAverager.Add(rss2));
            printf("AddRss(%4d) %d times, AddRss(%4d): ", rss, kNumRssAdds, rss2);
            ave = rssAverager.GetAverage();
            VerifyOrQuit(ave >= MIN_RSS(rss, rss2), "GetAverage() returned incorrect value.");
            VerifyOrQuit(ave <= MAX_RSS(rss, rss2), "GetAverage() returned incorrect value.");
            VerifyRawRssValue(rssAverager);
            PrintOutcome(rssAverager);
        }
    }

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Adding two alternating values many times:

    printf("- - - - - - - - - - - - - - - - - -\n");

    for (j = 0; j < sizeof(rssValues); j++)
    {
        rss = rssValues[j];

        for (k = 0; k < sizeof(rssValues); k++)
        {
            if (k == j)
            {
                continue;
            }

            rss2 = rssValues[k];
            rssAverager.Clear();

            for (i = 0; i < kNumRssAdds; i++)
            {
                IgnoreError(rssAverager.Add(rss));
                IgnoreError(rssAverager.Add(rss2));
                ave = rssAverager.GetAverage();
                VerifyOrQuit(ave >= MIN_RSS(rss, rss2), "GetAverage() is smaller than min value.");
                VerifyOrQuit(ave <= MAX_RSS(rss, rss2), "GetAverage() is larger than min value.");
                diff = ave;
                diff -= (rss + rss2) >> 1;
                VerifyOrQuit(ABS(diff) <= kRssAverageMaxDiff, "GetAverage() is incorrect");
                VerifyRawRssValue(rssAverager);
            }

            printf("[AddRss(%4d),  AddRss(%4d)] %d times: ", rss, rss2, kNumRssAdds);
            PrintOutcome(rssAverager);
        }
    }

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // For the first 8 values the average should be the arithmetic mean.

    printf("- - - - - - - - - - - - - - - - - -\n");

    for (i = 0; i < 1000; i++)
    {
        double mean;

        rssAverager.Clear();
        sum = 0;

        printf("\n");

        for (j = 1; j <= 8; j++)
        {
            rss = GetRandomRss();
            IgnoreError(rssAverager.Add(rss));
            sum += rss;
            mean = static_cast<double>(sum) / static_cast<double>(j);
            VerifyOrQuit(ABS(rssAverager.GetAverage() - mean) < 1, "Average does not match the arithmetic mean!");
            VerifyRawRssValue(rssAverager);
            printf("AddRss(%4d) sum=%-5d, mean=%-8.2f RssAverager=", rss, sum, mean);
            PrintOutcome(rssAverager);
        }
    }
}

void TestLinkQualityCalculations(void)
{
    const int8_t      rssList1[] = {-81, -80, -79, -78, -76, -80, -77, -75, -77, -76, -77, -74};
    const RssTestData rssData1   = {
        rssList1,         // mRssList
        sizeof(rssList1), // mRssListSize
        3                 // mExpectedLinkQuality
    };

    const int8_t      rssList2[] = {-90, -80, -85};
    const RssTestData rssData2   = {
        rssList2,         // mRssList
        sizeof(rssList2), // mRssListSize
        2                 // mExpectedLinkQuality
    };

    const int8_t      rssList3[] = {-95, -96, -98, -99, -100, -100, -98, -99, -100, -100, -100, -100, -100};
    const RssTestData rssData3   = {
        rssList3,         // mRssList
        sizeof(rssList3), // mRssListSize
        0                 // mExpectedLinkQuality
    };

    const int8_t      rssList4[] = {-75, -100, -100, -100, -100, -100, -95, -92, -93, -94, -93, -93};
    const RssTestData rssData4   = {
        rssList4,         // mRssList
        sizeof(rssList4), // mRssListSize
        1                 // mExpectedLinkQuality
    };

    TestLinkQualityData(rssData1);
    TestLinkQualityData(rssData2);
    TestLinkQualityData(rssData3);
    TestLinkQualityData(rssData4);
}

void TestSuccessRateTracker(void)
{
    SuccessRateTracker rateTracker;
    uint16_t           sampleCount;

    const uint16_t kMaxSamples = 5000;

    const uint16_t kMaxRate       = SuccessRateTracker::kMaxRateValue;
    const double   kMaxError      = 1.0; // Max permitted error in percentage
    const uint16_t kWeightLimit[] = {64, 128, 256, 300, 512, 810, 900};

    printf("\nTesting SuccessRateTracker\n");

    rateTracker.Clear();

    VerifyOrQuit(rateTracker.GetSuccessRate() == kMaxRate, "SuccessRateTracker: Initial value incorrect");
    VerifyOrQuit(rateTracker.GetFailureRate() == 0, "SuccessRateTracker: Initial value incorrect");

    // Adding all success
    for (sampleCount = 1; sampleCount < kMaxSamples; sampleCount++)
    {
        rateTracker.AddSample(true, sampleCount);

        VerifyOrQuit(rateTracker.GetSuccessRate() == kMaxRate, "SuccessRateTracker: incorrect rate all success case");
        VerifyOrQuit(rateTracker.GetFailureRate() == 0, "SuccessRateTracker: incorrect rate in all success case");
    }

    rateTracker.Clear();
    VerifyOrQuit(rateTracker.GetSuccessRate() == kMaxRate, "SuccessRateTracker: Rate incorrect after reset");
    VerifyOrQuit(rateTracker.GetFailureRate() == 0, "SuccessRateTracker: Rate incorrect after reset");

    // Adding all failures
    for (sampleCount = 1; sampleCount < kMaxRate; sampleCount++)
    {
        rateTracker.AddSample(false, sampleCount);

        VerifyOrQuit(rateTracker.GetSuccessRate() == 0, "SuccessRateTracker: rate incorrect all failure case");
        VerifyOrQuit(rateTracker.GetFailureRate() == kMaxRate,
                     "SuccessRateTracker: rate incorrect in all failure case");
    }

    // Adding success/failure at different rates and checking the RateTracker rate for every sample

    for (uint16_t testRound = 0; testRound < OT_ARRAY_LENGTH(kWeightLimit) * 2; testRound++)
    {
        uint16_t weightLimit;
        bool     reverseLogic;
        double   maxDiff = 0;

        // Reverse the logic (add success instead of failure) on even test rounds
        reverseLogic = ((testRound % 2) == 0);

        // Select a different weight limit based on the current test round
        weightLimit = kWeightLimit[testRound / 2];

        printf("TestRound %02d, weightLimit %3d, reverseLogic %d ", testRound, weightLimit, reverseLogic);

        for (uint16_t period = 1; period < 101; period++)
        {
            uint16_t failureCount = 0;

            rateTracker.Clear();

            for (sampleCount = 1; sampleCount < kMaxSamples; sampleCount++)
            {
                double   expectedRate;
                double   failureRate;
                double   diff;
                bool     isSuccess = ((sampleCount % period) == 0);
                uint16_t weight;

                if (reverseLogic)
                {
                    isSuccess = !isSuccess;
                }

                weight = sampleCount;

                if (weight > weightLimit)
                {
                    weight = weightLimit;
                }

                rateTracker.AddSample(isSuccess, weight);

                if (!isSuccess)
                {
                    failureCount++;
                }

                // Calculate the failure rate from rateTracker and expected rate.

                failureRate  = static_cast<double>(rateTracker.GetFailureRate()) * 100.0 / kMaxRate; // in percent
                expectedRate = static_cast<double>(failureCount) * 100.0 / sampleCount;              // in percent

                diff = failureRate - expectedRate;
                diff = ABS(diff);

                VerifyOrQuit(diff <= kMaxError, "SuccessRateTracker: rate does not match expected value");

                if (diff > maxDiff)
                {
                    maxDiff = diff;
                }
            }
        }

        printf(" MaxDiff = %.3f%%-> PASS\n", maxDiff);
    }
}

} // namespace ot

int main(void)
{
    ot::TestRssAveraging();
    ot::TestLinkQualityCalculations();
    ot::TestSuccessRateTracker();
    printf("\nAll tests passed\n");
    return 0;
}
