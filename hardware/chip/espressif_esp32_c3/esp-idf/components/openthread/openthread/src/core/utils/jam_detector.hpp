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
 *   This file includes definitions for jam detector.
 */

#ifndef JAM_DETECTOR_HPP_
#define JAM_DETECTOR_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE

#include <stdint.h>

#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/notifier.hpp"
#include "common/timer.hpp"

namespace ot {

class ThreadNetif;

namespace Utils {

class JamDetector : public InstanceLocator, private NonCopyable
{
    friend class ot::Notifier;

public:
    /**
     * This function pointer is called if jam state changes (assuming jamming detection is enabled).
     *
     * @param[in]  aJamState  `true` if jam is detected, `false` if jam is cleared.
     * @param[in]  aContext  A pointer to application-specific context.
     *
     */
    typedef void (*Handler)(bool aJamState, void *aContext);

    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit JamDetector(Instance &aInstance);

    /**
     * Start the jamming detection.
     *
     * @param[in]  aHandler             A pointer to a function called when jamming is detected.
     * @param[in]  aContext             A pointer to application-specific context.
     *
     * @retval kErrorNone            Successfully started the jamming detection.
     * @retval kErrorAlready         Jam detection has been started before.
     *
     */
    Error Start(Handler aHandler, void *aContext);

    /**
     * Stop the jamming detection.
     *
     * @retval kErrorNone            Successfully stopped the jamming detection.
     * @retval kErrorAlready         Jam detection is already stopped.
     *
     */
    Error Stop(void);

    /**
     * Get the Jam Detection Status
     *
     * @returns The Jam Detection status (true if enabled, false otherwise).
     */
    bool IsEnabled(void) const { return mEnabled; }

    /**
     * Get the current jam state.
     *
     * @returns The jamming state (`true` if jam is detected, `false` otherwise).
     */
    bool GetState(void) const { return mJamState; }

    /**
     * Set the Jam Detection RSSI Threshold (in dBm).
     *
     * @param[in]  aRssiThreshold  The RSSI threshold.
     *
     */
    void SetRssiThreshold(int8_t aThreshold);

    /**
     * Get the Jam Detection RSSI Threshold (in dBm).
     *
     * @returns The Jam Detection RSSI Threshold.
     */
    int8_t GetRssiThreshold(void) const { return mRssiThreshold; }

    /**
     * Set the Jam Detection Detection Window (in seconds).
     *
     * @param[in]  aWindow            The Jam Detection window (valid range is 1 to 63)
     *
     * @retval kErrorNone          Successfully set the window.
     * @retval kErrorInvalidArgs   The given input parameter not within valid range (1-63)
     *
     */
    Error SetWindow(uint8_t aWindow);

    /**
     * Get the Jam Detection Detection Window (in seconds).
     *
     * @returns The Jam Detection Window.
     */
    uint8_t GetWindow(void) const { return mWindow; }

    /**
     * Set the Jam Detection Busy Period (in seconds).
     *
     * The number of aggregate seconds within the detection window where the RSSI must be above
     * threshold to trigger detection.
     *
     * @param[in]  aBusyPeriod          The Jam Detection busy period (should be non-zero and
                                        less than or equal to Jam Detection Window)
     *
     * @retval kErrorNone           Successfully set the window.
     * @retval kErrorInvalidArgs    The given input is not within the valid range.
     *
     */
    Error SetBusyPeriod(uint8_t aBusyPeriod);

    /**
     * Get the Jam Detection Busy Period (in seconds)
     *
     * @returns The Jam Detection Busy Period
     */
    uint8_t GetBusyPeriod(void) const { return mBusyPeriod; }

    /**
     * Get the current history bitmap.
     *
     * This value provides information about current state of jamming detection
     * module for monitoring/debugging purpose. It provides a 64-bit value where
     * each bit corresponds to one second interval starting with bit 0 for the
     * most recent interval and bit 63 for the oldest intervals (63 earlier).
     * The bit is set to 1 if the jamming detection module observed/detected
     * high signal level during the corresponding one second interval.
     *
     * @returns The current history bitmap.
     */
    uint64_t GetHistoryBitmap(void) const { return mHistoryBitmap; }

private:
    static constexpr uint8_t kMaxWindow            = 63; // Max window size
    static constexpr int8_t  kDefaultRssiThreshold = 0;

    static constexpr uint16_t kMaxSampleInterval = 256;  // in ms
    static constexpr uint16_t kMinSampleInterval = 2;    // in ms
    static constexpr uint32_t kMaxRandomDelay    = 4;    // in ms
    static constexpr uint32_t kOneSecondInterval = 1000; // in ms

    void        CheckState(void);
    void        SetJamState(bool aNewState);
    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);
    void        UpdateHistory(bool aDidExceedThreshold);
    void        UpdateJamState(void);
    void        HandleNotifierEvents(Events aEvents);

    Handler    mHandler;                  // Handler/callback to inform about jamming state
    void *     mContext;                  // Context for handler/callback
    TimerMilli mTimer;                    // RSSI sample timer
    uint64_t   mHistoryBitmap;            // History bitmap, each bit correspond to 1 sec interval
    TimeMilli  mCurSecondStartTime;       // Start time for current 1 sec interval
    uint16_t   mSampleInterval;           // Current sample interval
    uint8_t    mWindow : 6;               // Window (in sec) to monitor jamming
    uint8_t    mBusyPeriod : 6;           // BusyPeriod (in sec) with mWindow to alert jamming
    bool       mEnabled : 1;              // If jam detection is enabled
    bool       mAlwaysAboveThreshold : 1; // State for current 1 sec interval
    bool       mJamState : 1;             // Current jam state
    int8_t     mRssiThreshold;            // RSSI threshold for jam detection
};

/**
 * @}
 *
 */

} // namespace Utils
} // namespace ot

#endif // OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE

#endif // JAM_DETECTOR_HPP_
