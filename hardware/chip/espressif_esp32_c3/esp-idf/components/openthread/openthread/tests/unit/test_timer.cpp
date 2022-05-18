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

#include "test_platform.h"

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/timer.hpp"

enum
{
    kCallCountIndexAlarmStop = 0,
    kCallCountIndexAlarmStart,
    kCallCountIndexTimerHandler,

    kCallCountIndexMax
};

uint32_t sNow;
uint32_t sPlatT0;
uint32_t sPlatDt;
bool     sTimerOn;
uint32_t sCallCount[kCallCountIndexMax];

extern "C" {

void otPlatAlarmMilliStop(otInstance *)
{
    sTimerOn = false;
    sCallCount[kCallCountIndexAlarmStop]++;
}

void otPlatAlarmMilliStartAt(otInstance *, uint32_t aT0, uint32_t aDt)
{
    sTimerOn = true;
    sCallCount[kCallCountIndexAlarmStart]++;
    sPlatT0 = aT0;
    sPlatDt = aDt;
}

uint32_t otPlatAlarmMilliGetNow(void)
{
    return sNow;
}

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
void otPlatAlarmMicroStop(otInstance *)
{
    sTimerOn = false;
    sCallCount[kCallCountIndexAlarmStop]++;
}

void otPlatAlarmMicroStartAt(otInstance *, uint32_t aT0, uint32_t aDt)
{
    sTimerOn = true;
    sCallCount[kCallCountIndexAlarmStart]++;
    sPlatT0 = aT0;
    sPlatDt = aDt;
}

uint32_t otPlatAlarmMicroGetNow(void)
{
    return sNow;
}
#endif

} // extern "C"

void InitCounters(void)
{
    memset(sCallCount, 0, sizeof(sCallCount));
}

/**
 * `TestTimer` sub-classes `ot::TimerMilli` and provides a handler and a counter to keep track of number of times timer
 * gets fired.
 */
template <typename TimerType> class TestTimer : public TimerType
{
public:
    explicit TestTimer(ot::Instance &aInstance)
        : TimerType(aInstance, TestTimer::HandleTimerFired)
        , mFiredCounter(0)
    {
    }

    static void HandleTimerFired(ot::Timer &aTimer) { static_cast<TestTimer &>(aTimer).HandleTimerFired(); }

    void HandleTimerFired(void)
    {
        sCallCount[kCallCountIndexTimerHandler]++;
        mFiredCounter++;
    }

    uint32_t GetFiredCounter(void) { return mFiredCounter; }

    void ResetFiredCounter(void) { mFiredCounter = 0; }

    static void RemoveAll(ot::Instance &aInstance) { TimerType::RemoveAll(aInstance); }

private:
    uint32_t mFiredCounter; //< Number of times timer has been fired so far
};

template <typename TimerType> void AlarmFired(otInstance *aInstance);

template <> void AlarmFired<ot::TimerMilli>(otInstance *aInstance)
{
    otPlatAlarmMilliFired(aInstance);
}

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
template <> void AlarmFired<ot::TimerMicro>(otInstance *aInstance)
{
    otPlatAlarmMicroFired(aInstance);
}
#endif

/**
 * Test the TimerScheduler's behavior of one timer started and fired.
 */
template <typename TimerType> int TestOneTimer(void)
{
    const uint32_t       kTimeT0        = 1000;
    const uint32_t       kTimerInterval = 10;
    ot::Instance *       instance       = testInitInstance();
    TestTimer<TimerType> timer(*instance);

    // Test one Timer basic operation.

    TestTimer<TimerType>::RemoveAll(*instance);
    InitCounters();

    printf("TestOneTimer() ");

    sNow = kTimeT0;
    timer.Start(kTimerInterval);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 1, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 0, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 0, "Handler CallCount Failed.");
    VerifyOrQuit(sPlatT0 == 1000 && sPlatDt == 10, "Start params Failed.");
    VerifyOrQuit(timer.IsRunning(), "Timer running Failed.");
    VerifyOrQuit(sTimerOn, "Platform Timer State Failed.");

    sNow += kTimerInterval;

    AlarmFired<TimerType>(instance);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 1, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 1, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 1, "Handler CallCount Failed.");
    VerifyOrQuit(timer.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(sTimerOn == false, "Platform Timer State Failed.");

    // Test one Timer that spans the 32-bit wrap.

    InitCounters();

    sNow = 0 - (kTimerInterval - 2);
    timer.Start(kTimerInterval);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 1, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 0, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 0, "Handler CallCount Failed.");
    VerifyOrQuit(sPlatT0 == 0 - (kTimerInterval - 2) && sPlatDt == 10, "Start params Failed.");
    VerifyOrQuit(timer.IsRunning(), "Timer running Failed.");
    VerifyOrQuit(sTimerOn, "Platform Timer State Failed.");

    sNow += kTimerInterval;

    AlarmFired<TimerType>(instance);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 1, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 1, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 1, "Handler CallCount Failed.");
    VerifyOrQuit(timer.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(sTimerOn == false, "Platform Timer State Failed.");

    // Test one Timer that is late by several msec

    InitCounters();

    sNow = kTimeT0;
    timer.Start(kTimerInterval);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 1, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 0, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 0, "Handler CallCount Failed.");
    VerifyOrQuit(sPlatT0 == 1000 && sPlatDt == 10, "Start params Failed.");
    VerifyOrQuit(timer.IsRunning(), "Timer running Failed.");
    VerifyOrQuit(sTimerOn, "Platform Timer State Failed.");

    sNow += kTimerInterval + 5;

    AlarmFired<TimerType>(instance);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 1, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 1, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 1, "Handler CallCount Failed.");
    VerifyOrQuit(timer.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(sTimerOn == false, "Platform Timer State Failed.");

    // Test one Timer that is early by several msec

    InitCounters();

    sNow = kTimeT0;
    timer.Start(kTimerInterval);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 1, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 0, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 0, "Handler CallCount Failed.");
    VerifyOrQuit(sPlatT0 == 1000 && sPlatDt == 10, "Start params Failed.");
    VerifyOrQuit(timer.IsRunning(), "Timer running Failed.");
    VerifyOrQuit(sTimerOn, "Platform Timer State Failed.");

    sNow += kTimerInterval - 2;

    AlarmFired<TimerType>(instance);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 2, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 0, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 0, "Handler CallCount Failed.");
    VerifyOrQuit(timer.IsRunning() == true, "Timer running Failed.");
    VerifyOrQuit(sTimerOn == true, "Platform Timer State Failed.");

    sNow += kTimerInterval;

    AlarmFired<TimerType>(instance);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 2, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 1, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 1, "Handler CallCount Failed.");
    VerifyOrQuit(timer.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(sTimerOn == false, "Platform Timer State Failed.");

    printf(" --> PASSED\n");

    testFreeInstance(instance);

    return 0;
}

/**
 * Test the TimerScheduler's behavior of two timers started and fired.
 */
template <typename TimerType> int TestTwoTimers(void)
{
    const uint32_t       kTimeT0        = 1000;
    const uint32_t       kTimerInterval = 10;
    ot::Instance *       instance       = testInitInstance();
    TestTimer<TimerType> timer1(*instance);
    TestTimer<TimerType> timer2(*instance);

    TestTimer<TimerType>::RemoveAll(*instance);
    printf("TestTwoTimers() ");

    // Test when second timer stars at the fire time of first timer (before alarm callback).

    InitCounters();

    sNow = kTimeT0;
    timer1.Start(kTimerInterval);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 1, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 0, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 0, "Handler CallCount Failed.");
    VerifyOrQuit(sPlatT0 == kTimeT0 && sPlatDt == kTimerInterval, "Start params Failed.");
    VerifyOrQuit(timer1.IsRunning(), "Timer running Failed.");
    VerifyOrQuit(timer2.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(sTimerOn, "Platform Timer State Failed.");

    sNow += kTimerInterval;

    timer2.Start(kTimerInterval);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 1, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 0, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 0, "Handler CallCount Failed.");
    VerifyOrQuit(sPlatT0 == kTimeT0 && sPlatDt == kTimerInterval, "Start params Failed.");
    VerifyOrQuit(timer1.IsRunning() == true, "Timer running Failed.");
    VerifyOrQuit(timer2.IsRunning() == true, "Timer running Failed.");
    VerifyOrQuit(sTimerOn, "Platform Timer State Failed.");

    AlarmFired<TimerType>(instance);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 2, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 0, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 1, "Handler CallCount Failed.");
    VerifyOrQuit(timer1.GetFiredCounter() == 1, "Fire Counter failed.");
    VerifyOrQuit(sPlatT0 == sNow && sPlatDt == kTimerInterval, "Start params Failed.");
    VerifyOrQuit(timer1.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(timer2.IsRunning() == true, "Timer running Failed.");
    VerifyOrQuit(sTimerOn == true, "Platform Timer State Failed.");

    sNow += kTimerInterval;
    AlarmFired<TimerType>(instance);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 2, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 1, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 2, "Handler CallCount Failed.");
    VerifyOrQuit(timer2.GetFiredCounter() == 1, "Fire Counter failed.");
    VerifyOrQuit(timer1.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(timer2.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(sTimerOn == false, "Platform Timer State Failed.");

    // Test when second timer starts at the fire time of first timer (before AlarmFired<TimerType>()) and its fire time
    // is before the first timer. Ensure that the second timer handler is invoked before the first one.

    InitCounters();
    timer1.ResetFiredCounter();
    timer2.ResetFiredCounter();

    sNow = kTimeT0;
    timer1.Start(kTimerInterval);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 1, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 0, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 0, "Handler CallCount Failed.");
    VerifyOrQuit(sPlatT0 == kTimeT0 && sPlatDt == kTimerInterval, "Start params Failed.");
    VerifyOrQuit(timer1.IsRunning(), "Timer running Failed.");
    VerifyOrQuit(timer2.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(sTimerOn, "Platform Timer State Failed.");

    sNow += kTimerInterval;

    timer2.StartAt(ot::TimeMilli(kTimeT0), kTimerInterval - 2); // Timer 2 is even before timer 1

    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 0, "Handler CallCount Failed.");
    VerifyOrQuit(timer1.IsRunning() == true, "Timer running Failed.");
    VerifyOrQuit(timer2.IsRunning() == true, "Timer running Failed.");
    VerifyOrQuit(sTimerOn, "Platform Timer State Failed.");

    AlarmFired<TimerType>(instance);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 0, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 1, "Handler CallCount Failed.");
    VerifyOrQuit(timer2.GetFiredCounter() == 1, "Fire Counter failed.");
    VerifyOrQuit(sPlatT0 == sNow && sPlatDt == 0, "Start params Failed.");
    VerifyOrQuit(timer1.IsRunning() == true, "Timer running Failed.");
    VerifyOrQuit(timer2.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(sTimerOn == true, "Platform Timer State Failed.");

    AlarmFired<TimerType>(instance);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 1, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 2, "Handler CallCount Failed.");
    VerifyOrQuit(timer1.GetFiredCounter() == 1, "Fire Counter failed.");
    VerifyOrQuit(timer1.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(timer2.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(sTimerOn == false, "Platform Timer State Failed.");

    // Timer 1 fire callback is late by some ticks/ms, and second timer is scheduled (before call to
    // AlarmFired) with a maximum interval. This is to test (corner-case) scenario where the fire time of two
    // timers spanning over the maximum interval.

    InitCounters();
    timer1.ResetFiredCounter();
    timer2.ResetFiredCounter();

    sNow = kTimeT0;
    timer1.Start(kTimerInterval);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 1, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 0, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 0, "Handler CallCount Failed.");
    VerifyOrQuit(sPlatT0 == kTimeT0 && sPlatDt == kTimerInterval, "Start params Failed.");
    VerifyOrQuit(timer1.IsRunning(), "Timer running Failed.");
    VerifyOrQuit(timer2.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(sTimerOn, "Platform Timer State Failed.");

    sNow += kTimerInterval + 5;

    timer2.Start(ot::Timer::kMaxDelay);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 1, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 0, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 0, "Handler CallCount Failed.");
    VerifyOrQuit(timer1.IsRunning() == true, "Timer running Failed.");
    VerifyOrQuit(timer2.IsRunning() == true, "Timer running Failed.");
    VerifyOrQuit(sTimerOn, "Platform Timer State Failed.");

    AlarmFired<TimerType>(instance);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 2, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 0, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 1, "Handler CallCount Failed.");
    VerifyOrQuit(timer1.GetFiredCounter() == 1, "Fire Counter failed.");
    VerifyOrQuit(sPlatT0 == sNow, "Start params Failed.");
    VerifyOrQuit(sPlatDt == ot::Timer::kMaxDelay, "Start params Failed.");
    VerifyOrQuit(timer1.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(timer2.IsRunning() == true, "Timer running Failed.");
    VerifyOrQuit(sTimerOn == true, "Platform Timer State Failed.");

    sNow += ot::Timer::kMaxDelay;
    AlarmFired<TimerType>(instance);

    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 2, "Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 1, "Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 2, "Handler CallCount Failed.");
    VerifyOrQuit(timer2.GetFiredCounter() == 1, "Fire Counter failed.");
    VerifyOrQuit(timer1.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(timer2.IsRunning() == false, "Timer running Failed.");
    VerifyOrQuit(sTimerOn == false, "Platform Timer State Failed.");

    printf(" --> PASSED\n");

    testFreeInstance(instance);

    return 0;
}

/**
 * Test the TimerScheduler's behavior of ten timers started and fired.
 *
 * `aTimeShift` is added to the t0 and trigger times for all timers. It can be used to check the ten timer behavior
 * at different start time (e.g., around a 32-bit wrap).
 */
template <typename TimerType> static void TenTimers(uint32_t aTimeShift)
{
    const uint32_t kNumTimers                 = 10;
    const uint32_t kNumTriggers               = 7;
    const uint32_t kTimeT0[kNumTimers]        = {1000, 1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008};
    const uint32_t kTimerInterval[kNumTimers] = {
        20, 100, (ot::Timer::kMaxDelay - kTimeT0[2]), 100000, 1000000, 10, ot::Timer::kMaxDelay, 200, 200, 200};
    // Expected timer fire order
    // timer #     Trigger time
    //   5            1014
    //   0            1020
    //   1            1100
    //   7            1206
    //   8            1207
    //   9            1208
    //   3          101002
    //   4         1001003
    //   2          kMaxDuration
    //   6   kMaxDuration + 1005
    const uint32_t kTriggerTimes[kNumTriggers] = {
        1014, 1020, 1100, 1207, 101004, ot::Timer::kMaxDelay, ot::Timer::kMaxDelay + kTimeT0[6]};
    // Expected timers fired by each kTriggerTimes[] value
    //  Trigger #    Timers Fired
    //    0             5
    //    1             0
    //    2             1
    //    3             7, 8
    //    4             9, 3
    //    5             4, 2
    //    6             6
    const bool kTimerStateAfterTrigger[kNumTriggers][kNumTimers] = {
        {true, true, true, true, true, false, true, true, true, true},         // 5
        {false, true, true, true, true, false, true, true, true, true},        // 0
        {false, false, true, true, true, false, true, true, true, true},       // 1
        {false, false, true, true, true, false, true, false, false, true},     // 7, 8
        {false, false, true, false, true, false, true, false, false, false},   // 9, 3
        {false, false, false, false, false, false, true, false, false, false}, // 4, 2
        {false, false, false, false, false, false, false, false, false, false} // 6
    };

    const bool kSchedulerStateAfterTrigger[kNumTriggers] = {true, true, true, true, true, true, false};

    const uint32_t kTimerHandlerCountAfterTrigger[kNumTriggers] = {1, 2, 3, 5, 7, 9, 10};

    const uint32_t kTimerStopCountAfterTrigger[kNumTriggers] = {0, 0, 0, 0, 0, 0, 1};

    const uint32_t kTimerStartCountAfterTrigger[kNumTriggers] = {3, 4, 5, 7, 9, 11, 11};

    ot::Instance *instance = testInitInstance();

    TestTimer<TimerType>  timer0(*instance);
    TestTimer<TimerType>  timer1(*instance);
    TestTimer<TimerType>  timer2(*instance);
    TestTimer<TimerType>  timer3(*instance);
    TestTimer<TimerType>  timer4(*instance);
    TestTimer<TimerType>  timer5(*instance);
    TestTimer<TimerType>  timer6(*instance);
    TestTimer<TimerType>  timer7(*instance);
    TestTimer<TimerType>  timer8(*instance);
    TestTimer<TimerType>  timer9(*instance);
    TestTimer<TimerType> *timers[kNumTimers] = {&timer0, &timer1, &timer2, &timer3, &timer4,
                                                &timer5, &timer6, &timer7, &timer8, &timer9};
    size_t                i;

    printf("TestTenTimer() with aTimeShift=%-10u ", aTimeShift);

    // Start the Ten timers.

    TestTimer<TimerType>::RemoveAll(*instance);
    InitCounters();

    for (i = 0; i < kNumTimers; i++)
    {
        sNow = kTimeT0[i] + aTimeShift;
        timers[i]->Start(kTimerInterval[i]);
    }

    // given the order in which timers are started, the TimerScheduler should call otPlatAlarmMilliStartAt 2 times.
    // one for timer[0] and one for timer[5] which will supercede timer[0].
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == 2, "TestTenTimer: Start CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == 0, "TestTenTimer: Stop CallCount Failed.");
    VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == 0, "TestTenTimer: Handler CallCount Failed.");
    VerifyOrQuit(sPlatT0 == kTimeT0[5] + aTimeShift, "TestTenTimer: Start params Failed.");
    VerifyOrQuit(sPlatDt == kTimerInterval[5], "TestTenTimer: Start params Failed.");
    VerifyOrQuit(sTimerOn, "TestTenTimer: Platform Timer State Failed.");

    for (i = 0; i < kNumTimers; i++)
    {
        VerifyOrQuit(timers[i]->IsRunning(), "TestTenTimer: Timer running Failed.");
    }

    // Issue the triggers and test the State after each trigger.

    for (size_t trigger = 0; trigger < kNumTriggers; trigger++)
    {
        sNow = kTriggerTimes[trigger] + aTimeShift;

        do
        {
            // By design, each call to AlarmFired<TimerType>() can result in 0 or 1 calls to a timer handler.
            // For some combinations of sNow and Timers queued, it is necessary to call AlarmFired<TimerType>()
            // multiple times in order to handle all the expired timers.  It can be determined that another
            // timer is ready to be triggered by examining the aDt arg passed into otPlatAlarmMilliStartAt().  If
            // that value is 0, then AlarmFired should be fired immediately. This loop calls
            // AlarmFired<TimerType>() the requisite number of times based on the aDt argument.
            AlarmFired<TimerType>(instance);
        } while (sPlatDt == 0);

        VerifyOrQuit(sCallCount[kCallCountIndexAlarmStart] == kTimerStartCountAfterTrigger[trigger],
                     "TestTenTimer: Start CallCount Failed.");
        VerifyOrQuit(sCallCount[kCallCountIndexAlarmStop] == kTimerStopCountAfterTrigger[trigger],
                     "TestTenTimer: Stop CallCount Failed.");
        VerifyOrQuit(sCallCount[kCallCountIndexTimerHandler] == kTimerHandlerCountAfterTrigger[trigger],
                     "TestTenTimer: Handler CallCount Failed.");
        VerifyOrQuit(sTimerOn == kSchedulerStateAfterTrigger[trigger], "TestTenTimer: Platform Timer State Failed.");

        for (i = 0; i < kNumTimers; i++)
        {
            VerifyOrQuit(timers[i]->IsRunning() == kTimerStateAfterTrigger[trigger][i],
                         "TestTenTimer: Timer running Failed.");
        }
    }

    for (i = 0; i < kNumTimers; i++)
    {
        VerifyOrQuit(timers[i]->GetFiredCounter() == 1, "TestTenTimer: Timer fired counter Failed.");
    }

    printf("--> PASSED\n");

    testFreeInstance(instance);
}

template <typename TimerType> int TestTenTimers(void)
{
    // Time shift to change the start/fire time of ten timers.
    const uint32_t kTimeShift[] = {
        0, 100000U, 0U - 1U, 0U - 1100U, ot::Timer::kMaxDelay, ot::Timer::kMaxDelay + 1020U,
    };

    size_t i;

    for (i = 0; i < OT_ARRAY_LENGTH(kTimeShift); i++)
    {
        TenTimers<TimerType>(kTimeShift[i]);
    }

    return 0;
}

/**
 * Test the `Timer::Time` class.
 */
int TestTimerTime(void)
{
    const uint32_t kMaxTime      = 0xffffffff;
    const uint32_t kStartTimes[] = {0, 100, kMaxTime / 2, kMaxTime - 100, kMaxTime};
    const uint32_t kDurations[]  = {1, 100, ot::Timer::kMaxDelay - 1, ot::Timer::kMaxDelay};

    ot::Time t1;
    ot::Time t2;

    for (uint32_t startTime : kStartTimes)
    {
        for (uint32_t duration : kDurations)
        {
            printf("TestTimerTime() start=%-10x  duration=%-10x ", startTime, duration);

            t1.SetValue(startTime);
            VerifyOrQuit(t1.GetValue() == startTime, "Time::SetValue() failed.");

            t2 = t1;
            VerifyOrQuit(t1.GetValue() == startTime, "Time assignment failed.");

            VerifyOrQuit(t1 == t2, "Time == failed.");
            VerifyOrQuit(!(t1 != t2), "Time != failed.");
            VerifyOrQuit(!(t1 < t2), "Time < failed.");
            VerifyOrQuit((t1 <= t2), "Time <= failed.");
            VerifyOrQuit(!(t1 > t2), "Time > failed.");
            VerifyOrQuit((t1 >= t2), "Time >= failed.");
            VerifyOrQuit(t2 - t1 == 0, "Time difference failed");

            t2 = t1 + duration;
            VerifyOrQuit(!(t1 == t2), "Time == failed.");
            VerifyOrQuit((t1 != t2), "Time != failed.");
            VerifyOrQuit((t1 < t2), "Time < failed.");
            VerifyOrQuit((t1 <= t2), "Time <= failed.");
            VerifyOrQuit(!(t1 > t2), "Time > failed.");
            VerifyOrQuit(!(t1 >= t2), "Time >= failed.");
            VerifyOrQuit(t2 - t1 == duration, "Time difference failed");

            t2 = t1;
            t2 += duration;
            VerifyOrQuit(!(t1 == t2), "Time == failed.");
            VerifyOrQuit((t1 != t2), "Time != failed.");
            VerifyOrQuit((t1 < t2), "Time < failed.");
            VerifyOrQuit((t1 <= t2), "Time <= failed.");
            VerifyOrQuit(!(t1 > t2), "Time > failed.");
            VerifyOrQuit(!(t1 >= t2), "Time >= failed.");
            VerifyOrQuit(t2 - t1 == duration, "Time difference failed");

            t2 = t1 - duration;
            VerifyOrQuit(!(t1 == t2), "Time == failed.");
            VerifyOrQuit((t1 != t2), "Time != failed.");
            VerifyOrQuit(!(t1 < t2), "Time < failed.");
            VerifyOrQuit(!(t1 <= t2), "Time <= failed.");
            VerifyOrQuit((t1 > t2), "Time > failed.");
            VerifyOrQuit((t1 >= t2), "Time >= failed.");
            VerifyOrQuit(t1 - t2 == duration, "Time difference failed");

            t2 = t1;
            t2 -= duration;
            VerifyOrQuit(!(t1 == t2), "Time == failed.");
            VerifyOrQuit((t1 != t2), "Time != failed.");
            VerifyOrQuit(!(t1 < t2), "Time < failed.");
            VerifyOrQuit(!(t1 <= t2), "Time <= failed.");
            VerifyOrQuit((t1 > t2), "Time > failed.");
            VerifyOrQuit((t1 >= t2), "Time >= failed.");
            VerifyOrQuit(t1 - t2 == duration, "Time difference failed");

            t2 = t1.GetDistantFuture();
            VerifyOrQuit((t1 < t2), "GetDistanceFuture() failed");
            t2 += 1;
            VerifyOrQuit(!(t1 < t2), "GetDistanceFuture() failed");

            t2 = t1.GetDistantPast();
            VerifyOrQuit((t1 > t2), "GetDistantPast() failed");
            t2 -= 1;
            VerifyOrQuit(!(t1 > t2), "GetDistantPast() failed");

            printf("--> PASSED\n");
        }
    }

    return 0;
}

template <typename TimerType> void RunTimerTests(void)
{
    TestOneTimer<TimerType>();
    TestTwoTimers<TimerType>();
    TestTenTimers<TimerType>();
}

int main(void)
{
    RunTimerTests<ot::TimerMilli>();
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    RunTimerTests<ot::TimerMicro>();
#endif
    TestTimerTime();
    printf("All tests passed\n");
    return 0;
}
