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
 *   This file implements the subset of IEEE 802.15.4 MAC primitives.
 */

#include "sub_mac.hpp"

#include <stdio.h>

#include <openthread/platform/time.h>

#include "mac_frame.hpp"
#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"
#include "common/time.hpp"

namespace ot {
namespace Mac {

SubMac::SubMac(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mRadioCaps(Get<Radio>().GetCaps())
    , mTransmitFrame(Get<Radio>().GetTransmitBuffer())
    , mCallbacks(aInstance)
    , mPcapCallback(nullptr)
    , mPcapCallbackContext(nullptr)
    , mTimer(aInstance, SubMac::HandleTimer)
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    , mCslParentAccuracy(kCslWorstCrystalPpm)
    , mCslParentUncert(kCslWorstUncertainty)
    , mCslTimer(aInstance, SubMac::HandleCslTimer)
#endif
{
    Init();
}

void SubMac::Init(void)
{
    mState           = kStateDisabled;
    mCsmaBackoffs    = 0;
    mTransmitRetries = 0;
    mShortAddress    = kShortAddrInvalid;
    mExtAddress.Clear();
    mRxOnWhenBackoff   = true;
    mEnergyScanMaxRssi = kInvalidRssiValue;
    mEnergyScanEndTime = Time{0};

    mPrevKey.Clear();
    mCurrKey.Clear();
    mNextKey.Clear();

    mFrameCounter = 0;
    mKeyId        = 0;
    mTimer.Stop();

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    mCslPeriod             = 0;
    mCslChannel            = 0;
    mIsCslChannelSpecified = false;
    mCslSampleTime         = TimeMicro{0};
    mCslLastSync           = TimeMicro{0};
    mCslState              = kCslIdle;
    mCslTimer.Stop();
#endif
}

otRadioCaps SubMac::GetCaps(void) const
{
    otRadioCaps caps;

#if OPENTHREAD_RADIO || OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    caps = mRadioCaps;

#if OPENTHREAD_CONFIG_MAC_SOFTWARE_ACK_TIMEOUT_ENABLE
    caps |= OT_RADIO_CAPS_ACK_TIMEOUT;
#endif

#if OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE
    caps |= OT_RADIO_CAPS_CSMA_BACKOFF;
#endif

#if OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE
    caps |= OT_RADIO_CAPS_TRANSMIT_RETRIES;
#endif

#if OPENTHREAD_CONFIG_MAC_SOFTWARE_ENERGY_SCAN_ENABLE
    caps |= OT_RADIO_CAPS_ENERGY_SCAN;
#endif

#if OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE && (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    caps |= OT_RADIO_CAPS_TRANSMIT_SEC;
#endif

#if OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE && (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    caps |= OT_RADIO_CAPS_TRANSMIT_TIMING;
#endif

#if OPENTHREAD_CONFIG_MAC_SOFTWARE_RX_TIMING_ENABLE && (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    caps |= OT_RADIO_CAPS_RECEIVE_TIMING;
#endif

#else
    caps = OT_RADIO_CAPS_ACK_TIMEOUT | OT_RADIO_CAPS_CSMA_BACKOFF | OT_RADIO_CAPS_TRANSMIT_RETRIES |
           OT_RADIO_CAPS_ENERGY_SCAN | OT_RADIO_CAPS_TRANSMIT_SEC | OT_RADIO_CAPS_TRANSMIT_TIMING |
           OT_RADIO_CAPS_RECEIVE_TIMING;
#endif

    return caps;
}

void SubMac::SetPanId(PanId aPanId)
{
    Get<Radio>().SetPanId(aPanId);
    otLogDebgMac("RadioPanId: 0x%04x", aPanId);
}

void SubMac::SetShortAddress(ShortAddress aShortAddress)
{
    mShortAddress = aShortAddress;
    Get<Radio>().SetShortAddress(mShortAddress);
    otLogDebgMac("RadioShortAddress: 0x%04x", mShortAddress);
}

void SubMac::SetExtAddress(const ExtAddress &aExtAddress)
{
    ExtAddress address;

    mExtAddress = aExtAddress;

    // Reverse the byte order before setting on radio.
    address.Set(aExtAddress.m8, ExtAddress::kReverseByteOrder);
    Get<Radio>().SetExtendedAddress(address);

    otLogDebgMac("RadioExtAddress: %s", mExtAddress.ToString().AsCString());
}

void SubMac::SetPcapCallback(otLinkPcapCallback aPcapCallback, void *aCallbackContext)
{
    mPcapCallback        = aPcapCallback;
    mPcapCallbackContext = aCallbackContext;
}

Error SubMac::Enable(void)
{
    Error error = kErrorNone;

    VerifyOrExit(mState == kStateDisabled);

    SuccessOrExit(error = Get<Radio>().Enable());
    SuccessOrExit(error = Get<Radio>().Sleep());

    SetState(kStateSleep);

exit:
    OT_ASSERT(error == kErrorNone);
    return error;
}

Error SubMac::Disable(void)
{
    Error error;

    mTimer.Stop();
    SuccessOrExit(error = Get<Radio>().Sleep());
    SuccessOrExit(error = Get<Radio>().Disable());
    SetState(kStateDisabled);

exit:
    return error;
}

Error SubMac::Sleep(void)
{
    Error error = Get<Radio>().Sleep();

    if (error != kErrorNone)
    {
        otLogWarnMac("RadioSleep() failed, error: %s", ErrorToString(error));
        ExitNow();
    }

    SetState(kStateSleep);

exit:
    return error;
}

Error SubMac::Receive(uint8_t aChannel)
{
    Error error = Get<Radio>().Receive(aChannel);

    if (error != kErrorNone)
    {
        otLogWarnMac("RadioReceive() failed, error: %s", ErrorToString(error));
        ExitNow();
    }

    SetState(kStateReceive);

exit:
    return error;
}

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
Error SubMac::CslSample(uint8_t aPanChannel)
{
    Error error = kErrorNone;

    if (!IsCslChannelSpecified())
    {
        mCslChannel = aPanChannel;
    }

    switch (mCslState)
    {
    case kCslSample:
        error = Get<Radio>().Receive(mCslChannel);
        break;
    case kCslSleep:
#if !OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE
        error = Get<Radio>().Sleep(); // Don't actually sleep for debugging
#endif
        break;
    case kCslIdle:
        ExitNow(error = kErrorInvalidState);
    default:
        OT_ASSERT(false);
    }

    SetState(kStateCslSample);

exit:
    if (error != kErrorNone)
    {
        otLogWarnMac("CslSample() failed, error: %s", ErrorToString(error));
    }
    return error;
}
#endif

void SubMac::HandleReceiveDone(RxFrame *aFrame, Error aError)
{
    if (mPcapCallback && (aFrame != nullptr) && (aError == kErrorNone))
    {
        mPcapCallback(aFrame, false, mPcapCallbackContext);
    }

    if (!ShouldHandleTransmitSecurity() && aFrame != nullptr && aFrame->mInfo.mRxInfo.mAckedWithSecEnhAck)
    {
        SignalFrameCounterUsed(aFrame->mInfo.mRxInfo.mAckFrameCounter);
    }

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (aFrame != nullptr && aError == kErrorNone)
    {
        // Assuming the risk of the parent missing the Enh-ACK in favor of smaller CSL receive window
        if ((mCslPeriod > 0) && aFrame->mInfo.mRxInfo.mAckedWithSecEnhAck)
        {
            mCslLastSync = TimeMicro(static_cast<uint32_t>(aFrame->mInfo.mRxInfo.mTimestamp));
        }

#if OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE
        // Split the log into two lines for RTT to output
        otLogDebgMac("Received frame in state (SubMac %s, CSL %s), timestamp %u", StateToString(mState),
                     CslStateToString(mCslState), static_cast<uint32_t>(aFrame->mInfo.mRxInfo.mTimestamp));
        otLogDebgMac("Target sample start time %u, time drift %d", mCslSampleTime.GetValue(),
                     static_cast<uint32_t>(aFrame->mInfo.mRxInfo.mTimestamp) - mCslSampleTime.GetValue());
#endif
    }
#endif

    mCallbacks.ReceiveDone(aFrame, aError);
}

Error SubMac::Send(void)
{
    Error error = kErrorNone;

    switch (mState)
    {
    case kStateDisabled:
    case kStateCsmaBackoff:
#if !OPENTHREAD_MTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    case kStateCslTransmit:
#endif
    case kStateTransmit:
    case kStateEnergyScan:
        ExitNow(error = kErrorInvalidState);
        OT_UNREACHABLE_CODE(break);

    case kStateSleep:
    case kStateReceive:
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    case kStateCslSample:
#endif
        break;
    }

    ProcessTransmitSecurity();
    mCsmaBackoffs    = 0;
    mTransmitRetries = 0;
    StartCsmaBackoff();

exit:
    return error;
}

void SubMac::ProcessTransmitSecurity(void)
{
    const ExtAddress *extAddress = nullptr;
    uint8_t           keyIdMode;

    VerifyOrExit(mTransmitFrame.GetSecurityEnabled());
    VerifyOrExit(!mTransmitFrame.IsSecurityProcessed());

    SuccessOrExit(mTransmitFrame.GetKeyIdMode(keyIdMode));

    if (!mTransmitFrame.IsHeaderUpdated())
    {
        mTransmitFrame.SetKeyId(mKeyId);
    }

    VerifyOrExit(ShouldHandleTransmitSecurity());
    VerifyOrExit(keyIdMode == Frame::kKeyIdMode1);

    mTransmitFrame.SetAesKey(GetCurrentMacKey());

    if (!mTransmitFrame.IsHeaderUpdated())
    {
        uint32_t frameCounter = GetFrameCounter();

        mTransmitFrame.SetFrameCounter(frameCounter);
        SignalFrameCounterUsed(frameCounter);
    }

    extAddress = &GetExtAddress();

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    // Transmit security will be processed after time IE content is updated.
    VerifyOrExit(mTransmitFrame.GetTimeIeOffset() == 0);
#endif

    mTransmitFrame.ProcessTransmitAesCcm(*extAddress);

exit:
    return;
}

void SubMac::StartCsmaBackoff(void)
{
    uint32_t backoff;
    uint32_t backoffExponent = kMinBE + mTransmitRetries + mCsmaBackoffs;

#if !OPENTHREAD_MTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    if (mTransmitFrame.mInfo.mTxInfo.mTxDelay != 0)
    {
        SetState(kStateCslTransmit);

        if (ShouldHandleTransmitTargetTime())
        {
            if (Time(static_cast<uint32_t>(otPlatRadioGetNow(&GetInstance()))) <
                Time(mTransmitFrame.mInfo.mTxInfo.mTxDelayBaseTime) + mTransmitFrame.mInfo.mTxInfo.mTxDelay -
                    kCcaSampleInterval)
            {
                mTimer.StartAt(Time(mTransmitFrame.mInfo.mTxInfo.mTxDelayBaseTime) - kCcaSampleInterval,
                               mTransmitFrame.mInfo.mTxInfo.mTxDelay);
            }
            else // Transmit without delay
            {
                BeginTransmit();
            }
        }
        else
        {
            BeginTransmit();
        }

        ExitNow();
    }
#endif // !OPENTHREAD_MTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    SetState(kStateCsmaBackoff);

    VerifyOrExit(ShouldHandleCsmaBackOff(), BeginTransmit());

    if (backoffExponent > kMaxBE)
    {
        backoffExponent = kMaxBE;
    }

    backoff = Random::NonCrypto::GetUint32InRange(0, static_cast<uint32_t>(1UL << backoffExponent));
    backoff *= (kUnitBackoffPeriod * OT_RADIO_SYMBOL_TIME);

    if (mRxOnWhenBackoff)
    {
        IgnoreError(Get<Radio>().Receive(mTransmitFrame.GetChannel()));
    }
    else
    {
        IgnoreError(Get<Radio>().Sleep());
    }

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    mTimer.Start(backoff);
#else
    mTimer.Start(backoff / 1000UL);
#endif

exit:
    return;
}

void SubMac::BeginTransmit(void)
{
    Error error;

    OT_UNUSED_VARIABLE(error);

#if !OPENTHREAD_MTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    VerifyOrExit(mState == kStateCsmaBackoff || mState == kStateCslTransmit);
#else
    VerifyOrExit(mState == kStateCsmaBackoff);
#endif

    if ((mRadioCaps & OT_RADIO_CAPS_SLEEP_TO_TX) == 0)
    {
        error = Get<Radio>().Receive(mTransmitFrame.GetChannel());
        OT_ASSERT(error == kErrorNone);
    }

    SetState(kStateTransmit);

    if (mPcapCallback)
    {
        mPcapCallback(&mTransmitFrame, true, mPcapCallbackContext);
    }

    error = Get<Radio>().Transmit(mTransmitFrame);
    if (error == kErrorInvalidState && mTransmitFrame.mInfo.mTxInfo.mTxDelay > 0)
    {
        // Platform `transmit_at` fails and we send the frame directly.
        mTransmitFrame.mInfo.mTxInfo.mTxDelay         = 0;
        mTransmitFrame.mInfo.mTxInfo.mTxDelayBaseTime = 0;
        error                                         = Get<Radio>().Transmit(mTransmitFrame);
    }
    OT_ASSERT(error == kErrorNone);

exit:
    return;
}

void SubMac::HandleTransmitStarted(TxFrame &aFrame)
{
    if (ShouldHandleAckTimeout() && aFrame.GetAckRequest())
    {
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
        mTimer.Start(kAckTimeout * 1000UL);
#else
        mTimer.Start(kAckTimeout);
#endif
    }
}

void SubMac::HandleTransmitDone(TxFrame &aFrame, RxFrame *aAckFrame, Error aError)
{
    bool ccaSuccess = true;
    bool shouldRetx;

    // Stop ack timeout timer.

    mTimer.Stop();

    // Record CCA success or failure status.

    switch (aError)
    {
    case kErrorAbort:
        // Do not record CCA status in case of `ABORT` error
        // since there may be no CCA check performed by radio.
        break;

    case kErrorChannelAccessFailure:
        ccaSuccess = false;

        OT_FALL_THROUGH;

    case kErrorNone:
    case kErrorNoAck:
        if (aFrame.IsCsmaCaEnabled())
        {
            mCallbacks.RecordCcaStatus(ccaSuccess, aFrame.GetChannel());
        }
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
        // Actual synchronization timestamp should be from the sent frame instead of the current time.
        // Assuming the error here since it is bounded and has very small effect on the final window duration.
        if (mCslPeriod > 0)
        {
            mCslLastSync = TimeMicro(static_cast<uint32_t>(otPlatRadioGetNow(&GetInstance())));
        }
#endif
        break;

    default:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(ExitNow());
    }

    SignalFrameCounterUsedOnTxDone(aFrame);

    // Determine whether a CSMA retry is required.

    if (!ccaSuccess && ShouldHandleCsmaBackOff() && mCsmaBackoffs < aFrame.GetMaxCsmaBackoffs())
    {
        mCsmaBackoffs++;
        StartCsmaBackoff();
        ExitNow();
    }

    mCsmaBackoffs = 0;

    // Determine whether to re-transmit the frame.

    shouldRetx = ((aError != kErrorNone) && ShouldHandleRetries() && (mTransmitRetries < aFrame.GetMaxFrameRetries()));

    mCallbacks.RecordFrameTransmitStatus(aFrame, aAckFrame, aError, mTransmitRetries, shouldRetx);

    if (shouldRetx)
    {
        mTransmitRetries++;
        aFrame.SetIsARetransmission(true);
        StartCsmaBackoff();
        ExitNow();
    }

    SetState(kStateReceive);

    mCallbacks.TransmitDone(aFrame, aAckFrame, aError);

exit:
    return;
}

void SubMac::SignalFrameCounterUsedOnTxDone(const TxFrame &aFrame)
{
    uint8_t  keyIdMode;
    uint32_t frameCounter;
    bool     allowError = false;

    OT_UNUSED_VARIABLE(allowError);

    VerifyOrExit(!ShouldHandleTransmitSecurity() && aFrame.GetSecurityEnabled() && aFrame.IsHeaderUpdated());

    // In an FTD/MTD build, if/when link-raw is enabled, the `TxFrame`
    // is prepared and given by user and may not necessarily follow 15.4
    // frame format (link raw can be used with vendor-specific format),
    // so we allow failure when parsing the frame (i.e., do not assert
    // on an error). In other cases (in an RCP build or in an FTD/MTD
    // build without link-raw) since the `TxFrame` should be prepared by
    // OpenThread core, we expect no error and therefore assert if
    // parsing fails.

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    allowError = Get<LinkRaw>().IsEnabled();
#endif

    VerifyOrExit(aFrame.GetKeyIdMode(keyIdMode) == kErrorNone, OT_ASSERT(allowError));
    VerifyOrExit(keyIdMode == Frame::kKeyIdMode1);

    VerifyOrExit(aFrame.GetFrameCounter(frameCounter) == kErrorNone, OT_ASSERT(allowError));
    SignalFrameCounterUsed(frameCounter);

exit:
    return;
}

int8_t SubMac::GetRssi(void) const
{
    return Get<Radio>().GetRssi();
}

int8_t SubMac::GetNoiseFloor(void)
{
    return Get<Radio>().GetReceiveSensitivity();
}

Error SubMac::EnergyScan(uint8_t aScanChannel, uint16_t aScanDuration)
{
    Error error = kErrorNone;

    switch (mState)
    {
    case kStateDisabled:
    case kStateCsmaBackoff:
    case kStateTransmit:
#if !OPENTHREAD_MTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    case kStateCslTransmit:
#endif
    case kStateEnergyScan:
        ExitNow(error = kErrorInvalidState);

    case kStateReceive:
    case kStateSleep:
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    case kStateCslSample:
#endif
        break;
    }

    if (RadioSupportsEnergyScan())
    {
        IgnoreError(Get<Radio>().EnergyScan(aScanChannel, aScanDuration));
        SetState(kStateEnergyScan);
    }
    else if (ShouldHandleEnergyScan())
    {
        error = Get<Radio>().Receive(aScanChannel);
        OT_ASSERT(error == kErrorNone);

        SetState(kStateEnergyScan);
        mEnergyScanMaxRssi = kInvalidRssiValue;
        mEnergyScanEndTime = TimerMilli::GetNow() + static_cast<uint32_t>(aScanDuration);
        mTimer.Start(0);
    }
    else
    {
        error = kErrorNotImplemented;
    }

exit:
    return error;
}

void SubMac::SampleRssi(void)
{
    OT_ASSERT(!RadioSupportsEnergyScan());

    int8_t rssi = GetRssi();

    if (rssi != kInvalidRssiValue)
    {
        if ((mEnergyScanMaxRssi == kInvalidRssiValue) || (rssi > mEnergyScanMaxRssi))
        {
            mEnergyScanMaxRssi = rssi;
        }
    }

    if (TimerMilli::GetNow() < mEnergyScanEndTime)
    {
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
        mTimer.StartAt(mTimer.GetFireTime(), kEnergyScanRssiSampleInterval * 1000UL);
#else
        mTimer.StartAt(mTimer.GetFireTime(), kEnergyScanRssiSampleInterval);
#endif
    }
    else
    {
        HandleEnergyScanDone(mEnergyScanMaxRssi);
    }
}

void SubMac::HandleEnergyScanDone(int8_t aMaxRssi)
{
    SetState(kStateReceive);
    mCallbacks.EnergyScanDone(aMaxRssi);
}

void SubMac::HandleTimer(Timer &aTimer)
{
    aTimer.Get<SubMac>().HandleTimer();
}

void SubMac::HandleTimer(void)
{
    switch (mState)
    {
#if !OPENTHREAD_MTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    case kStateCslTransmit:
        BeginTransmit();
        break;
#endif
    case kStateCsmaBackoff:
        BeginTransmit();
        break;

    case kStateTransmit:
        otLogDebgMac("Ack timer timed out");
        IgnoreError(Get<Radio>().Receive(mTransmitFrame.GetChannel()));
        HandleTransmitDone(mTransmitFrame, nullptr, kErrorNoAck);
        break;

    case kStateEnergyScan:
        SampleRssi();
        break;

    default:
        break;
    }
}

bool SubMac::ShouldHandleTransmitSecurity(void) const
{
    bool swTxSecurity = true;

    VerifyOrExit(!RadioSupportsTransmitSecurity(), swTxSecurity = false);

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    VerifyOrExit(Get<LinkRaw>().IsEnabled());
#endif

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE || OPENTHREAD_RADIO
    swTxSecurity = OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE;
#endif

exit:
    return swTxSecurity;
}

bool SubMac::ShouldHandleCsmaBackOff(void) const
{
    bool swCsma = true;

    VerifyOrExit(!RadioSupportsCsmaBackoff(), swCsma = false);

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    VerifyOrExit(Get<LinkRaw>().IsEnabled());
#endif

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE || OPENTHREAD_RADIO
    swCsma = OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE;
#endif

exit:
    return swCsma;
}

bool SubMac::ShouldHandleAckTimeout(void) const
{
    bool swAckTimeout = true;

    VerifyOrExit(!RadioSupportsAckTimeout(), swAckTimeout = false);

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    VerifyOrExit(Get<LinkRaw>().IsEnabled());
#endif

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE || OPENTHREAD_RADIO
    swAckTimeout = OPENTHREAD_CONFIG_MAC_SOFTWARE_ACK_TIMEOUT_ENABLE;
#endif

exit:
    return swAckTimeout;
}

bool SubMac::ShouldHandleRetries(void) const
{
    bool swRetries = true;

    VerifyOrExit(!RadioSupportsRetries(), swRetries = false);

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    VerifyOrExit(Get<LinkRaw>().IsEnabled());
#endif

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE || OPENTHREAD_RADIO
    swRetries = OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE;
#endif

exit:
    return swRetries;
}

bool SubMac::ShouldHandleEnergyScan(void) const
{
    bool swEnergyScan = true;

    VerifyOrExit(!RadioSupportsEnergyScan(), swEnergyScan = false);

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    VerifyOrExit(Get<LinkRaw>().IsEnabled());
#endif

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE || OPENTHREAD_RADIO
    swEnergyScan = OPENTHREAD_CONFIG_MAC_SOFTWARE_ENERGY_SCAN_ENABLE;
#endif

exit:
    return swEnergyScan;
}

bool SubMac::ShouldHandleTransmitTargetTime(void) const
{
    bool swTxDelay = true;

    VerifyOrExit(!RadioSupportsTransmitTiming(), swTxDelay = false);

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    VerifyOrExit(Get<LinkRaw>().IsEnabled());
#endif

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE || OPENTHREAD_RADIO
    swTxDelay = OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE;
#endif

exit:
    return swTxDelay;
}

void SubMac::SetState(State aState)
{
    if (mState != aState)
    {
        otLogDebgMac("RadioState: %s -> %s", StateToString(mState), StateToString(aState));
        mState = aState;
    }
}

void SubMac::SetMacKey(uint8_t            aKeyIdMode,
                       uint8_t            aKeyId,
                       const KeyMaterial &aPrevKey,
                       const KeyMaterial &aCurrKey,
                       const KeyMaterial &aNextKey)
{
    switch (aKeyIdMode)
    {
    case Frame::kKeyIdMode0:
    case Frame::kKeyIdMode2:
        break;
    case Frame::kKeyIdMode1:
        mKeyId   = aKeyId;
        mPrevKey = aPrevKey;
        mCurrKey = aCurrKey;
        mNextKey = aNextKey;
        break;

    default:
        OT_ASSERT(false);
        break;
    }

    VerifyOrExit(!ShouldHandleTransmitSecurity());

    Get<Radio>().SetMacKey(aKeyIdMode, aKeyId, aPrevKey, aCurrKey, aNextKey);

exit:
    return;
}

void SubMac::SignalFrameCounterUsed(uint32_t aFrameCounter)
{
    mCallbacks.FrameCounterUsed(aFrameCounter);

    // It not always guaranteed that this method is invoked in order
    // for different counter values (i.e., we may get it for a
    // smaller counter value after a lager one). This may happen due
    // to a new counter value being used for an enhanced-ack during
    // tx of a frame. Note that the newer counter used for enhanced-ack
    // is processed from `HandleReceiveDone()` which can happen before
    // processing of the older counter value from `HandleTransmitDone()`.

    VerifyOrExit(mFrameCounter <= aFrameCounter);
    mFrameCounter = aFrameCounter + 1;

exit:
    return;
}

void SubMac::SetFrameCounter(uint32_t aFrameCounter)
{
    mFrameCounter = aFrameCounter;

    VerifyOrExit(!ShouldHandleTransmitSecurity());

    Get<Radio>().SetMacFrameCounter(aFrameCounter);

exit:
    return;
}

// LCOV_EXCL_START

const char *SubMac::StateToString(State aState)
{
    static const char *const kStateStrings[] = {
        "Disabled",    // (0) kStateDisabled
        "Sleep",       // (1) kStateSleep
        "Receive",     // (2) kStateReceive
        "CsmaBackoff", // (3) kStateCsmaBackoff
        "Transmit",    // (4) kStateTransmit
        "EnergyScan",  // (5) kStateEnergyScan
#if !OPENTHREAD_MTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
        "CslTransmit", // (6) kStateCslTransmit
#endif
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
        "CslSample", // (7) kStateCslSample
#endif
    };

    static_assert(kStateDisabled == 0, "kStateDisabled value is not correct");
    static_assert(kStateSleep == 1, "kStateSleep value is not correct");
    static_assert(kStateReceive == 2, "kStateReceive value is not correct");
    static_assert(kStateCsmaBackoff == 3, "kStateCsmaBackoff value is not correct");
    static_assert(kStateTransmit == 4, "kStateTransmit value is not correct");
    static_assert(kStateEnergyScan == 5, "kStateEnergyScan value is not correct");
#if !OPENTHREAD_MTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    static_assert(kStateCslTransmit == 6, "kStateCslTransmit value is not correct");
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    static_assert(kStateCslSample == 7, "kStateCslSample value is not correct");
#endif
#else
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    static_assert(kStateCslSample == 6, "kStateCslSample value is not correct");
#endif
#endif

    return kStateStrings[aState];
}

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
const char *SubMac::CslStateToString(CslState aCslState)
{
    static const char *const kCslStateStrings[] = {
        "CslIdle",   // (0) kCslIdle
        "CslSample", // (1) kCslSample
        "CslSleep",  // (2) kCslSleep
    };

    static_assert(kCslIdle == 0, "kCslIdle value is incorrect");
    static_assert(kCslSample == 1, "kCslSample value is incorrect");
    static_assert(kCslSleep == 2, "kCslSleep value is incorrect");

    return kCslStateStrings[aCslState];
}
#endif

// LCOV_EXCL_STOP

//---------------------------------------------------------------------------------------------------------------------
// CSL Receiver methods

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
void SubMac::SetCslChannel(uint8_t aChannel)
{
    mCslChannel = aChannel;
}

void SubMac::SetCslPeriod(uint16_t aPeriod)
{
    VerifyOrExit(mCslPeriod != aPeriod);

    mCslPeriod = aPeriod;

    mCslTimer.Stop();

    if (mCslPeriod > 0)
    {
        mCslSampleTime = TimeMicro(static_cast<uint32_t>(otPlatRadioGetNow(&GetInstance())));
        mCslState      = kCslSleep;
        HandleCslTimer();
    }
    else
    {
        mCslState = kCslIdle;

        if (mState == kStateCslSample)
        {
            IgnoreError(Get<Radio>().Sleep());
            SetState(kStateSleep);
        }
    }

    otLogDebgMac("CSL Period: %u", mCslPeriod);

exit:
    return;
}

void SubMac::HandleCslTimer(Timer &aTimer)
{
    aTimer.Get<SubMac>().HandleCslTimer();
}

void SubMac::HandleCslTimer(void)
{
    /*
     * CSL sample timing diagram
     *    |<---------------------------------Sample--------------------------------->|<--------Sleep--------->|
     *    |                                                                          |                        |
     *    |<--Ahead-->|<--UnCert-->|<--Drift-->|<--Drift-->|<--UnCert-->|<--MinWin-->|                        |
     *    |           |            |           |           |            |            |                        |
     * ---|-----------|------------|-----------|-----------|------------|------------|----------//------------|---
     * -timeAhead                           CslPhase                             +timeAfter             -timeAhead
     */
    uint32_t periodUs = mCslPeriod * kUsPerTenSymbols;
    uint32_t timeAhead, timeAfter;

    GetCslWindowEdges(timeAhead, timeAfter);

    switch (mCslState)
    {
    case kCslSample:
        mCslState = kCslSleep;

        mCslTimer.FireAt(mCslSampleTime - timeAhead);
        if (mState == kStateCslSample)
        {
#if !OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE
            IgnoreError(Get<Radio>().Sleep()); // Don't actually sleep for debugging
#endif
            otLogDebgMac("CSL sleep %u", mCslTimer.GetNow().GetValue());
        }
        break;

    case kCslSleep:
        mCslSampleTime += periodUs;

        if (RadioSupportsReceiveTiming())
        {
            mCslTimer.FireAt(mCslSampleTime - timeAhead);
            timeAhead -= kCslReceiveTimeAhead;
        }
        else
        {
            mCslTimer.FireAt(mCslSampleTime + timeAfter);
            mCslState = kCslSample;
        }

        Get<Radio>().UpdateCslSampleTime(mCslSampleTime.GetValue());
        if (mState == kStateCslSample)
        {
            if (RadioSupportsReceiveTiming())
            {
                IgnoreError(Get<Radio>().ReceiveAt(mCslChannel, mCslSampleTime.GetValue() - periodUs - timeAhead,
                                                   timeAhead + timeAfter));
            }
            else
            {
                IgnoreError(Get<Radio>().Receive(mCslChannel));
                otLogDebgMac("CSL sample %u, duration %u", mCslTimer.GetNow().GetValue(), timeAhead + timeAfter);
            }
        }
        break;

    case kCslIdle:
        break;

    default:
        OT_ASSERT(false);
        break;
    }
}

void SubMac::GetCslWindowEdges(uint32_t &ahead, uint32_t &after)
{
    uint32_t semiPeriod = mCslPeriod * kUsPerTenSymbols / 2;
    uint64_t curTime    = otPlatRadioGetNow(&GetInstance());
    uint64_t elapsed;
    uint32_t semiWindow;

    if (mCslLastSync.GetValue() > curTime)
    {
        elapsed = UINT64_MAX - mCslLastSync.GetValue() + curTime;
    }
    else
    {
        elapsed = curTime - mCslLastSync.GetValue();
    }

    semiWindow = static_cast<uint32_t>(elapsed * (Get<Radio>().GetCslAccuracy() + mCslParentAccuracy) / 1000000);
    semiWindow += mCslParentUncert * kUsPerUncertUnit;

    ahead = (semiWindow + kCslReceiveTimeAhead > semiPeriod) ? semiPeriod : semiWindow + kCslReceiveTimeAhead;
    after = (semiWindow + kMinCslWindow > semiPeriod) ? semiPeriod : semiWindow + kMinCslWindow;
}
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

} // namespace Mac
} // namespace ot
