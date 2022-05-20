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
 *   This file implements the subset of IEEE 802.15.4 primitives required for Thread.
 */

#include "mac.hpp"

#include <stdio.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"
#include "common/string.hpp"
#include "crypto/aes_ccm.hpp"
#include "crypto/sha256.hpp"
#include "mac/mac_frame.hpp"
#include "radio/radio.hpp"
#include "thread/child_table.hpp"
#include "thread/link_quality.hpp"
#include "thread/mle_router.hpp"
#include "thread/thread_netif.hpp"
#include "thread/topology.hpp"

namespace ot {
namespace Mac {

const otExtAddress Mac::sMode2ExtAddress = {
    {0x35, 0x06, 0xfe, 0xb8, 0x23, 0xd4, 0x87, 0x12},
};

const otExtendedPanId Mac::sExtendedPanidInit = {
    {0xde, 0xad, 0x00, 0xbe, 0xef, 0x00, 0xca, 0xfe},
};

const char Mac::sNetworkNameInit[] = "OpenThread";

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
const char Mac::sDomainNameInit[] = "DefaultDomain";
#endif

Mac::Mac(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mEnabled(false)
    , mPendingActiveScan(false)
    , mPendingEnergyScan(false)
    , mPendingTransmitBeacon(false)
    , mPendingTransmitDataDirect(false)
#if OPENTHREAD_FTD
    , mPendingTransmitDataIndirect(false)
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    , mPendingTransmitDataCsl(false)
#endif
#endif
    , mPendingTransmitPoll(false)
    , mPendingWaitingForData(false)
    , mShouldTxPollBeforeData(false)
    , mRxOnWhenIdle(false)
    , mPromiscuous(false)
    , mBeaconsEnabled(false)
    , mUsingTemporaryChannel(false)
#if OPENTHREAD_CONFIG_MAC_STAY_AWAKE_BETWEEN_FRAGMENTS
    , mShouldDelaySleep(false)
    , mDelayingSleep(false)
#endif
    , mOperation(kOperationIdle)
    , mBeaconSequence(Random::NonCrypto::GetUint8())
    , mDataSequence(Random::NonCrypto::GetUint8())
    , mBroadcastTransmitCount(0)
    , mPanId(kPanIdBroadcast)
    , mPanChannel(OPENTHREAD_CONFIG_DEFAULT_CHANNEL)
    , mRadioChannel(OPENTHREAD_CONFIG_DEFAULT_CHANNEL)
    , mSupportedChannelMask(Get<Radio>().GetSupportedChannelMask())
    , mScanChannel(Radio::kChannelMin)
    , mScanDuration(0)
    , mMaxFrameRetriesDirect(kDefaultMaxFrameRetriesDirect)
#if OPENTHREAD_FTD
    , mMaxFrameRetriesIndirect(kDefaultMaxFrameRetriesIndirect)
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    , mCslTxFireTime(TimeMilli::kMaxDuration)
#endif
#endif
    , mActiveScanHandler(nullptr) // Initialize `mActiveScanHandler` and `mEnergyScanHandler` union
    , mScanHandlerContext(nullptr)
    , mLinks(aInstance)
    , mOperationTask(aInstance, Mac::HandleOperationTask)
    , mTimer(aInstance, Mac::HandleTimer)
    , mKeyIdMode2FrameCounter(0)
    , mCcaSampleCount(0)
#if OPENTHREAD_CONFIG_MULTI_RADIO
    , mTxError(kErrorNone)
#endif
{
    ExtAddress randomExtAddress;

    static const otMacKey sMode2Key = {
        {0x78, 0x58, 0x16, 0x86, 0xfd, 0xb4, 0x58, 0x0f, 0xb0, 0x92, 0x54, 0x6a, 0xec, 0xbd, 0x15, 0x66}};

    randomExtAddress.GenerateRandom();

    mCcaSuccessRateTracker.Clear();
    ResetCounters();
    mExtendedPanId.Clear();

    SetEnabled(true);
    mLinks.Enable();

    Get<KeyManager>().UpdateKeyMaterial();
    SetExtendedPanId(static_cast<const ExtendedPanId &>(sExtendedPanidInit));
    IgnoreError(SetNetworkName(sNetworkNameInit));
#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    IgnoreError(SetDomainName(sDomainNameInit));
#endif
    SetPanId(mPanId);
    SetExtAddress(randomExtAddress);
    SetShortAddress(GetShortAddress());

    mMode2KeyMaterial.SetFrom(static_cast<const Key &>(sMode2Key));
}

Error Mac::ActiveScan(uint32_t aScanChannels, uint16_t aScanDuration, ActiveScanHandler aHandler, void *aContext)
{
    Error error = kErrorNone;

    VerifyOrExit(IsEnabled(), error = kErrorInvalidState);
    VerifyOrExit(!IsActiveScanInProgress() && !IsEnergyScanInProgress(), error = kErrorBusy);

    mActiveScanHandler  = aHandler;
    mScanHandlerContext = aContext;

    if (aScanDuration == 0)
    {
        aScanDuration = kScanDurationDefault;
    }

    Scan(kOperationActiveScan, aScanChannels, aScanDuration);

exit:
    return error;
}

Error Mac::EnergyScan(uint32_t aScanChannels, uint16_t aScanDuration, EnergyScanHandler aHandler, void *aContext)
{
    Error error = kErrorNone;

    VerifyOrExit(IsEnabled(), error = kErrorInvalidState);
    VerifyOrExit(!IsActiveScanInProgress() && !IsEnergyScanInProgress(), error = kErrorBusy);

    mEnergyScanHandler  = aHandler;
    mScanHandlerContext = aContext;

    Scan(kOperationEnergyScan, aScanChannels, aScanDuration);

exit:
    return error;
}

void Mac::Scan(Operation aScanOperation, uint32_t aScanChannels, uint16_t aScanDuration)
{
    mScanDuration = aScanDuration;
    mScanChannel  = ChannelMask::kChannelIteratorFirst;

    if (aScanChannels == 0)
    {
        aScanChannels = GetSupportedChannelMask().GetMask();
    }

    mScanChannelMask.SetMask(aScanChannels);
    mScanChannelMask.Intersect(mSupportedChannelMask);
    StartOperation(aScanOperation);
}

bool Mac::IsInTransmitState(void) const
{
    bool retval = false;

    switch (mOperation)
    {
    case kOperationTransmitDataDirect:
#if OPENTHREAD_FTD
    case kOperationTransmitDataIndirect:
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    case kOperationTransmitDataCsl:
#endif
#endif
    case kOperationTransmitBeacon:
    case kOperationTransmitPoll:
        retval = true;
        break;

    case kOperationIdle:
    case kOperationActiveScan:
    case kOperationEnergyScan:
    case kOperationWaitingForData:
        retval = false;
        break;
    }

    return retval;
}

Error Mac::ConvertBeaconToActiveScanResult(const RxFrame *aBeaconFrame, ActiveScanResult &aResult)
{
    Error                error = kErrorNone;
    Address              address;
    const Beacon *       beacon        = nullptr;
    const BeaconPayload *beaconPayload = nullptr;
    uint16_t             payloadLength;

    memset(&aResult, 0, sizeof(ActiveScanResult));

    VerifyOrExit(aBeaconFrame != nullptr, error = kErrorInvalidArgs);

    VerifyOrExit(aBeaconFrame->GetType() == Frame::kFcfFrameBeacon, error = kErrorParse);
    SuccessOrExit(error = aBeaconFrame->GetSrcAddr(address));
    VerifyOrExit(address.IsExtended(), error = kErrorParse);
    aResult.mExtAddress = address.GetExtended();

    if (kErrorNone != aBeaconFrame->GetSrcPanId(aResult.mPanId))
    {
        IgnoreError(aBeaconFrame->GetDstPanId(aResult.mPanId));
    }

    aResult.mChannel = aBeaconFrame->GetChannel();
    aResult.mRssi    = aBeaconFrame->GetRssi();
    aResult.mLqi     = aBeaconFrame->GetLqi();

    payloadLength = aBeaconFrame->GetPayloadLength();

    beacon        = reinterpret_cast<const Beacon *>(aBeaconFrame->GetPayload());
    beaconPayload = reinterpret_cast<const BeaconPayload *>(beacon->GetPayload());

    if ((payloadLength >= (sizeof(*beacon) + sizeof(*beaconPayload))) && beacon->IsValid() && beaconPayload->IsValid())
    {
        aResult.mVersion    = beaconPayload->GetProtocolVersion();
        aResult.mIsJoinable = beaconPayload->IsJoiningPermitted();
        aResult.mIsNative   = beaconPayload->IsNative();
        IgnoreError(static_cast<NetworkName &>(aResult.mNetworkName).Set(beaconPayload->GetNetworkName()));
        VerifyOrExit(IsValidUtf8String(aResult.mNetworkName.m8), error = kErrorParse);
        aResult.mExtendedPanId = beaconPayload->GetExtendedPanId();
    }

    LogBeacon("Received", *beaconPayload);

exit:
    return error;
}

Error Mac::UpdateScanChannel(void)
{
    Error error;

    VerifyOrExit(IsEnabled(), error = kErrorAbort);

    error = mScanChannelMask.GetNextChannel(mScanChannel);

exit:
    return error;
}

void Mac::PerformActiveScan(void)
{
    if (UpdateScanChannel() == kErrorNone)
    {
        // If there are more channels to scan, send the beacon request.
        BeginTransmit();
    }
    else
    {
        mLinks.SetPanId(mPanId);
        FinishOperation();
        ReportActiveScanResult(nullptr);
        PerformNextOperation();
    }
}

void Mac::ReportActiveScanResult(const RxFrame *aBeaconFrame)
{
    VerifyOrExit(mActiveScanHandler != nullptr);

    if (aBeaconFrame == nullptr)
    {
        mActiveScanHandler(nullptr, mScanHandlerContext);
    }
    else
    {
        ActiveScanResult result;

        SuccessOrExit(ConvertBeaconToActiveScanResult(aBeaconFrame, result));
        mActiveScanHandler(&result, mScanHandlerContext);
    }

exit:
    return;
}

void Mac::PerformEnergyScan(void)
{
    Error error = kErrorNone;

    SuccessOrExit(error = UpdateScanChannel());

    if (mScanDuration == 0)
    {
        while (true)
        {
            mLinks.Receive(mScanChannel);
            ReportEnergyScanResult(mLinks.GetRssi());
            SuccessOrExit(error = UpdateScanChannel());
        }
    }
    else
    {
        error = mLinks.EnergyScan(mScanChannel, mScanDuration);
    }

exit:

    if (error != kErrorNone)
    {
        FinishOperation();

        if (mEnergyScanHandler != nullptr)
        {
            mEnergyScanHandler(nullptr, mScanHandlerContext);
        }

        PerformNextOperation();
    }
}

void Mac::ReportEnergyScanResult(int8_t aRssi)
{
    EnergyScanResult result;

    VerifyOrExit((mEnergyScanHandler != nullptr) && (aRssi != kInvalidRssiValue));

    result.mChannel = mScanChannel;
    result.mMaxRssi = aRssi;

    mEnergyScanHandler(&result, mScanHandlerContext);

exit:
    return;
}

void Mac::EnergyScanDone(int8_t aEnergyScanMaxRssi)
{
    ReportEnergyScanResult(aEnergyScanMaxRssi);
    PerformEnergyScan();
}

void Mac::SetRxOnWhenIdle(bool aRxOnWhenIdle)
{
    VerifyOrExit(mRxOnWhenIdle != aRxOnWhenIdle);

    mRxOnWhenIdle = aRxOnWhenIdle;

    // If the new value for `mRxOnWhenIdle` is `true` (i.e., radio should
    // remain in Rx while idle) we stop any ongoing or pending `WaitingForData`
    // operation (since this operation only applies to sleepy devices).

    if (mRxOnWhenIdle)
    {
        if (mPendingWaitingForData)
        {
            mTimer.Stop();
            mPendingWaitingForData = false;
        }

        if (mOperation == kOperationWaitingForData)
        {
            mTimer.Stop();
            FinishOperation();
            mOperationTask.Post();
        }

#if OPENTHREAD_CONFIG_MAC_STAY_AWAKE_BETWEEN_FRAGMENTS
        mDelayingSleep    = false;
        mShouldDelaySleep = false;
#endif
    }

    mLinks.SetRxOnWhenBackoff(mRxOnWhenIdle || mPromiscuous);
    UpdateIdleMode();

exit:
    return;
}

Error Mac::SetPanChannel(uint8_t aChannel)
{
    Error error = kErrorNone;

    VerifyOrExit(mSupportedChannelMask.ContainsChannel(aChannel), error = kErrorInvalidArgs);

    SuccessOrExit(Get<Notifier>().Update(mPanChannel, aChannel, kEventThreadChannelChanged));

    mCcaSuccessRateTracker.Clear();

    VerifyOrExit(!mUsingTemporaryChannel);

    mRadioChannel = mPanChannel;

    UpdateIdleMode();

exit:
    return error;
}

Error Mac::SetTemporaryChannel(uint8_t aChannel)
{
    Error error = kErrorNone;

    VerifyOrExit(mSupportedChannelMask.ContainsChannel(aChannel), error = kErrorInvalidArgs);

    mUsingTemporaryChannel = true;
    mRadioChannel          = aChannel;

    UpdateIdleMode();

exit:
    return error;
}

void Mac::ClearTemporaryChannel(void)
{
    if (mUsingTemporaryChannel)
    {
        mUsingTemporaryChannel = false;
        mRadioChannel          = mPanChannel;
        UpdateIdleMode();
    }
}

void Mac::SetSupportedChannelMask(const ChannelMask &aMask)
{
    ChannelMask newMask = aMask;

    newMask.Intersect(ChannelMask(Get<Radio>().GetSupportedChannelMask()));
    IgnoreError(Get<Notifier>().Update(mSupportedChannelMask, newMask, kEventSupportedChannelMaskChanged));
}

Error Mac::SetNetworkName(const char *aNameString)
{
    return SignalNetworkNameChange(mNetworkName.Set(aNameString));
}

Error Mac::SetNetworkName(const NameData &aNameData)
{
    return SignalNetworkNameChange(mNetworkName.Set(aNameData));
}

Error Mac::SignalNetworkNameChange(Error aError)
{
    switch (aError)
    {
    case kErrorNone:
        Get<Notifier>().Signal(kEventThreadNetworkNameChanged);
        break;

    case kErrorAlready:
        Get<Notifier>().SignalIfFirst(kEventThreadNetworkNameChanged);
        aError = kErrorNone;
        break;

    default:
        break;
    }

    return aError;
}

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
Error Mac::SetDomainName(const char *aNameString)
{
    Error error = mDomainName.Set(aNameString);

    return (error == kErrorAlready) ? kErrorNone : error;
}

Error Mac::SetDomainName(const NameData &aNameData)
{
    Error error = mDomainName.Set(aNameData);

    return (error == kErrorAlready) ? kErrorNone : error;
}
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

void Mac::SetPanId(PanId aPanId)
{
    SuccessOrExit(Get<Notifier>().Update(mPanId, aPanId, kEventThreadPanIdChanged));
    mLinks.SetPanId(mPanId);

exit:
    return;
}

void Mac::SetExtendedPanId(const ExtendedPanId &aExtendedPanId)
{
    IgnoreError(Get<Notifier>().Update(mExtendedPanId, aExtendedPanId, kEventThreadExtPanIdChanged));
}

void Mac::RequestDirectFrameTransmission(void)
{
    VerifyOrExit(IsEnabled());
    VerifyOrExit(!mPendingTransmitDataDirect && (mOperation != kOperationTransmitDataDirect));

    StartOperation(kOperationTransmitDataDirect);

exit:
    return;
}

#if OPENTHREAD_FTD
void Mac::RequestIndirectFrameTransmission(void)
{
    VerifyOrExit(IsEnabled());
    VerifyOrExit(!mPendingTransmitDataIndirect && (mOperation != kOperationTransmitDataIndirect));

    StartOperation(kOperationTransmitDataIndirect);

exit:
    return;
}

#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
void Mac::RequestCslFrameTransmission(uint32_t aDelay)
{
    VerifyOrExit(mEnabled);

    mCslTxFireTime = TimerMilli::GetNow() + aDelay;

    StartOperation(kOperationTransmitDataCsl);

exit:
    return;
}
#endif
#endif // OPENTHREAD_FTD

Error Mac::RequestDataPollTransmission(void)
{
    Error error = kErrorNone;

    VerifyOrExit(IsEnabled(), error = kErrorInvalidState);
    VerifyOrExit(!mPendingTransmitPoll && (mOperation != kOperationTransmitPoll), error = kErrorAlready);

    // We ensure data frame and data poll tx requests are handled in the
    // order they are requested. So if we have a pending direct data frame
    // tx request, it should be sent before the poll frame.

    mShouldTxPollBeforeData = !mPendingTransmitDataDirect;

    StartOperation(kOperationTransmitPoll);

exit:
    return error;
}

void Mac::UpdateIdleMode(void)
{
    bool shouldSleep = !mRxOnWhenIdle && !mPromiscuous;

    VerifyOrExit(mOperation == kOperationIdle);

    if (!mRxOnWhenIdle)
    {
#if OPENTHREAD_CONFIG_MAC_STAY_AWAKE_BETWEEN_FRAGMENTS
        if (mShouldDelaySleep)
        {
            mTimer.Start(kSleepDelay);
            mShouldDelaySleep = false;
            mDelayingSleep    = true;
            otLogDebgMac("Idle mode: Sleep delayed");
        }

        if (mDelayingSleep)
        {
            shouldSleep = false;
        }
#endif
    }
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    else if (mPendingTransmitDataCsl)
    {
        mTimer.FireAt(mCslTxFireTime);
    }
#endif

    if (shouldSleep)
    {
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
        if (IsCslEnabled())
        {
            mLinks.CslSample(mRadioChannel);
            ExitNow();
        }
#endif
        mLinks.Sleep();
        otLogDebgMac("Idle mode: Radio sleeping");
    }
    else
    {
        mLinks.Receive(mRadioChannel);
        otLogDebgMac("Idle mode: Radio receiving on channel %d", mRadioChannel);
    }

exit:
    return;
}

void Mac::StartOperation(Operation aOperation)
{
    if (aOperation != kOperationIdle)
    {
        otLogDebgMac("Request to start operation \"%s\"", OperationToString(aOperation));

#if OPENTHREAD_CONFIG_MAC_STAY_AWAKE_BETWEEN_FRAGMENTS
        if (mDelayingSleep)
        {
            otLogDebgMac("Canceling sleep delay");
            mTimer.Stop();
            mDelayingSleep    = false;
            mShouldDelaySleep = false;
        }
#endif
    }

    switch (aOperation)
    {
    case kOperationIdle:
        break;

    case kOperationActiveScan:
        mPendingActiveScan = true;
        break;

    case kOperationEnergyScan:
        mPendingEnergyScan = true;
        break;

    case kOperationTransmitBeacon:
        mPendingTransmitBeacon = true;
        break;

    case kOperationTransmitDataDirect:
        mPendingTransmitDataDirect = true;
        break;

#if OPENTHREAD_FTD
    case kOperationTransmitDataIndirect:
        mPendingTransmitDataIndirect = true;
        break;

#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    case kOperationTransmitDataCsl:
        mPendingTransmitDataCsl = true;
        break;
#endif
#endif

    case kOperationTransmitPoll:
        mPendingTransmitPoll = true;
        break;

    case kOperationWaitingForData:
        mPendingWaitingForData = true;
        break;
    }

    if (mOperation == kOperationIdle)
    {
        mOperationTask.Post();
    }
}

void Mac::HandleOperationTask(Tasklet &aTasklet)
{
    aTasklet.Get<Mac>().PerformNextOperation();
}

void Mac::PerformNextOperation(void)
{
    VerifyOrExit(mOperation == kOperationIdle);

    if (!IsEnabled())
    {
        mPendingWaitingForData     = false;
        mPendingActiveScan         = false;
        mPendingEnergyScan         = false;
        mPendingTransmitBeacon     = false;
        mPendingTransmitDataDirect = false;
#if OPENTHREAD_FTD
        mPendingTransmitDataIndirect = false;
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
        mPendingTransmitDataCsl = false;
#endif
#endif
        mPendingTransmitPoll = false;
        mTimer.Stop();
#if OPENTHREAD_CONFIG_MAC_STAY_AWAKE_BETWEEN_FRAGMENTS
        mDelayingSleep    = false;
        mShouldDelaySleep = false;
#endif
        ExitNow();
    }

    // `WaitingForData` should be checked before any other pending
    // operations since radio should remain in receive mode after
    // a data poll ack indicating a pending frame from parent.
    if (mPendingWaitingForData)
    {
        mPendingWaitingForData = false;
        mOperation             = kOperationWaitingForData;
    }
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    else if (mPendingTransmitDataCsl && TimerMilli::GetNow() >= mCslTxFireTime)
    {
        mPendingTransmitDataCsl = false;
        mOperation              = kOperationTransmitDataCsl;
    }
#endif
    else if (mPendingActiveScan)
    {
        mPendingActiveScan = false;
        mOperation         = kOperationActiveScan;
    }
    else if (mPendingEnergyScan)
    {
        mPendingEnergyScan = false;
        mOperation         = kOperationEnergyScan;
    }
    else if (mPendingTransmitBeacon)
    {
        mPendingTransmitBeacon = false;
        mOperation             = kOperationTransmitBeacon;
    }
#if OPENTHREAD_FTD
    else if (mPendingTransmitDataIndirect)
    {
        mPendingTransmitDataIndirect = false;
        mOperation                   = kOperationTransmitDataIndirect;
    }
#endif // OPENTHREAD_FTD
    else if (mPendingTransmitPoll && (!mPendingTransmitDataDirect || mShouldTxPollBeforeData))
    {
        mPendingTransmitPoll = false;
        mOperation           = kOperationTransmitPoll;
    }
    else if (mPendingTransmitDataDirect)
    {
        mPendingTransmitDataDirect = false;
        mOperation                 = kOperationTransmitDataDirect;

        if (mPendingTransmitPoll)
        {
            // Ensure that a pending "transmit poll" operation request
            // is prioritized over any future "transmit data" requests.
            mShouldTxPollBeforeData = true;
        }
    }

    if (mOperation != kOperationIdle)
    {
        otLogDebgMac("Starting operation \"%s\"", OperationToString(mOperation));
        mTimer.Stop(); // Stop the timer before any non-idle operation, have the operation itself be responsible to
                       // start the timer (if it wants to).
    }

    switch (mOperation)
    {
    case kOperationIdle:
        UpdateIdleMode();
        break;

    case kOperationActiveScan:
        PerformActiveScan();
        break;

    case kOperationEnergyScan:
        PerformEnergyScan();
        break;

    case kOperationTransmitBeacon:
    case kOperationTransmitDataDirect:
#if OPENTHREAD_FTD
    case kOperationTransmitDataIndirect:
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    case kOperationTransmitDataCsl:
#endif
#endif
    case kOperationTransmitPoll:
        BeginTransmit();
        break;

    case kOperationWaitingForData:
        mLinks.Receive(mRadioChannel);
        mTimer.Start(kDataPollTimeout);
        break;
    }

exit:
    return;
}

void Mac::FinishOperation(void)
{
    otLogDebgMac("Finishing operation \"%s\"", OperationToString(mOperation));
    mOperation = kOperationIdle;
}

TxFrame *Mac::PrepareBeaconRequest(void)
{
    TxFrame &frame = mLinks.GetTxFrames().GetBroadcastTxFrame();
    uint16_t fcf   = Frame::kFcfFrameMacCmd | Frame::kFcfDstAddrShort | Frame::kFcfSrcAddrNone;

    frame.InitMacHeader(fcf, Frame::kSecNone);
    frame.SetDstPanId(kShortAddrBroadcast);
    frame.SetDstAddr(kShortAddrBroadcast);
    IgnoreError(frame.SetCommandId(Frame::kMacCmdBeaconRequest));

    otLogInfoMac("Sending Beacon Request");

    return &frame;
}

TxFrame *Mac::PrepareBeacon(void)
{
    TxFrame *      frame;
    uint8_t        beaconLength;
    uint16_t       fcf;
    Beacon *       beacon        = nullptr;
    BeaconPayload *beaconPayload = nullptr;

#if OPENTHREAD_CONFIG_MULTI_RADIO
    OT_ASSERT(!mTxBeaconRadioLinks.IsEmpty());
    frame = &mLinks.GetTxFrames().GetTxFrame(mTxBeaconRadioLinks);
    mTxBeaconRadioLinks.Clear();
#else
    frame = &mLinks.GetTxFrames().GetBroadcastTxFrame();
#endif

    fcf = Frame::kFcfFrameBeacon | Frame::kFcfDstAddrNone | Frame::kFcfSrcAddrExt;
    frame->InitMacHeader(fcf, Frame::kSecNone);
    IgnoreError(frame->SetSrcPanId(mPanId));
    frame->SetSrcAddr(GetExtAddress());

    beacon = reinterpret_cast<Beacon *>(frame->GetPayload());
    beacon->Init();
    beaconLength = sizeof(*beacon);

    beaconPayload = reinterpret_cast<BeaconPayload *>(beacon->GetPayload());

    if (Get<KeyManager>().GetSecurityPolicy().mBeaconsEnabled)
    {
        beaconPayload->Init();

        if (IsJoinable())
        {
            beaconPayload->SetJoiningPermitted();
        }
        else
        {
            beaconPayload->ClearJoiningPermitted();
        }

        beaconPayload->SetNetworkName(mNetworkName.GetAsData());
        beaconPayload->SetExtendedPanId(mExtendedPanId);

        beaconLength += sizeof(*beaconPayload);
    }

    frame->SetPayloadLength(beaconLength);

    LogBeacon("Sending", *beaconPayload);

    return frame;
}

bool Mac::ShouldSendBeacon(void) const
{
    bool shouldSend = false;

    VerifyOrExit(IsEnabled());

    shouldSend = IsBeaconEnabled();

#if OPENTHREAD_CONFIG_MAC_BEACON_RSP_WHEN_JOINABLE_ENABLE
    if (!shouldSend)
    {
        // When `ENABLE_BEACON_RSP_WHEN_JOINABLE` feature is enabled,
        // the device should transmit IEEE 802.15.4 Beacons in response
        // to IEEE 802.15.4 Beacon Requests even while the device is not
        // router capable and detached (i.e., `IsBeaconEnabled()` is
        // false) but only if it is in joinable state (unsecure port
        // list is not empty).

        shouldSend = IsJoinable();
    }
#endif

exit:
    return shouldSend;
}

bool Mac::IsJoinable(void) const
{
    uint8_t numUnsecurePorts;

    Get<Ip6::Filter>().GetUnsecurePorts(numUnsecurePorts);

    return (numUnsecurePorts != 0);
}

void Mac::ProcessTransmitSecurity(TxFrame &aFrame)
{
    KeyManager &      keyManager = Get<KeyManager>();
    uint8_t           keyIdMode;
    const ExtAddress *extAddress = nullptr;

    VerifyOrExit(aFrame.GetSecurityEnabled());

    IgnoreError(aFrame.GetKeyIdMode(keyIdMode));

    switch (keyIdMode)
    {
    case Frame::kKeyIdMode0:
        aFrame.SetAesKey(keyManager.GetKek());
        extAddress = &GetExtAddress();

        if (!aFrame.IsHeaderUpdated())
        {
            aFrame.SetFrameCounter(keyManager.GetKekFrameCounter());
            keyManager.IncrementKekFrameCounter();
        }

        break;

    case Frame::kKeyIdMode1:

        // For 15.4 radio link, the AES CCM* and frame security counter (under MAC
        // key ID mode 1) are managed by `SubMac` or `Radio` modules.
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
#if !OPENTHREAD_CONFIG_MULTI_RADIO
        ExitNow();
#else
        VerifyOrExit(aFrame.GetRadioType() != kRadioTypeIeee802154);
#endif
#endif

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        aFrame.SetAesKey(*mLinks.GetCurrentMacKey(aFrame));
        extAddress = &GetExtAddress();

        // If the frame header is marked as updated, `MeshForwarder` which
        // prepared the frame should set the frame counter and key id to the
        // same values used in the earlier transmit attempt. For a new frame (header
        // not updated), we get a new frame counter and key id from the key
        // manager.

        if (!aFrame.IsHeaderUpdated())
        {
            mLinks.SetMacFrameCounter(aFrame);
            aFrame.SetKeyId((keyManager.GetCurrentKeySequence() & 0x7f) + 1);
        }
#endif
        break;

    case Frame::kKeyIdMode2:
    {
        const uint8_t keySource[] = {0xff, 0xff, 0xff, 0xff};

        aFrame.SetAesKey(mMode2KeyMaterial);

        mKeyIdMode2FrameCounter++;
        aFrame.SetFrameCounter(mKeyIdMode2FrameCounter);
        aFrame.SetKeySource(keySource);
        aFrame.SetKeyId(0xff);
        extAddress = static_cast<const ExtAddress *>(&sMode2ExtAddress);
        break;
    }

    default:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);
    }

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    // Transmit security will be processed after time IE content is updated.
    VerifyOrExit(aFrame.GetTimeIeOffset() == 0);
#endif

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    // Transmit security will be processed after time IE content is updated.
    VerifyOrExit(aFrame.mInfo.mTxInfo.mCslPresent == 0);
#endif

    aFrame.ProcessTransmitAesCcm(*extAddress);

exit:
    return;
}

void Mac::BeginTransmit(void)
{
    TxFrame * frame    = nullptr;
    TxFrames &txFrames = mLinks.GetTxFrames();
    Address   dstAddr;

    txFrames.Clear();

#if OPENTHREAD_CONFIG_MULTI_RADIO
    mTxPendingRadioLinks.Clear();
    mTxError = kErrorAbort;
#endif

    VerifyOrExit(IsEnabled());

    switch (mOperation)
    {
    case kOperationActiveScan:
        mLinks.SetPanId(kPanIdBroadcast);
        frame = PrepareBeaconRequest();
        VerifyOrExit(frame != nullptr);
        frame->SetChannel(mScanChannel);
        frame->SetSequence(0);
        frame->SetMaxCsmaBackoffs(kMaxCsmaBackoffsDirect);
        frame->SetMaxFrameRetries(mMaxFrameRetriesDirect);
        break;

    case kOperationTransmitBeacon:
        frame = PrepareBeacon();
        VerifyOrExit(frame != nullptr);
        frame->SetChannel(mRadioChannel);
        frame->SetSequence(mBeaconSequence++);
        frame->SetMaxCsmaBackoffs(kMaxCsmaBackoffsDirect);
        frame->SetMaxFrameRetries(mMaxFrameRetriesDirect);
        break;

    case kOperationTransmitPoll:
        txFrames.SetChannel(mRadioChannel);
        txFrames.SetMaxCsmaBackoffs(kMaxCsmaBackoffsDirect);
        txFrames.SetMaxFrameRetries(mMaxFrameRetriesDirect);
        frame = Get<DataPollSender>().PrepareDataRequest(txFrames);
        VerifyOrExit(frame != nullptr);
        frame->SetSequence(mDataSequence++);
        break;

    case kOperationTransmitDataDirect:
        // Set channel and retry counts on all TxFrames before asking
        // the next layer (`MeshForwarder`) to prepare the frame. This
        // allows next layer to possibility change these parameters.
        txFrames.SetChannel(mRadioChannel);
        txFrames.SetMaxCsmaBackoffs(kMaxCsmaBackoffsDirect);
        txFrames.SetMaxFrameRetries(mMaxFrameRetriesDirect);
        frame = Get<MeshForwarder>().HandleFrameRequest(txFrames);
        VerifyOrExit(frame != nullptr);
        frame->SetSequence(mDataSequence++);
        break;

#if OPENTHREAD_FTD
    case kOperationTransmitDataIndirect:
        txFrames.SetChannel(mRadioChannel);
        txFrames.SetMaxCsmaBackoffs(kMaxCsmaBackoffsIndirect);
        txFrames.SetMaxFrameRetries(mMaxFrameRetriesIndirect);
        frame = Get<DataPollHandler>().HandleFrameRequest(txFrames);
        VerifyOrExit(frame != nullptr);

        // If the frame is marked as retransmission, then data sequence number is already set.
        if (!frame->IsARetransmission())
        {
            frame->SetSequence(mDataSequence++);
        }
        break;

#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    case kOperationTransmitDataCsl:
        txFrames.SetMaxCsmaBackoffs(kMaxCsmaBackoffsCsl);
        txFrames.SetMaxFrameRetries(kMaxFrameRetriesCsl);
        frame = Get<CslTxScheduler>().HandleFrameRequest(txFrames);
        VerifyOrExit(frame != nullptr);

        // If the frame is marked as retransmission, then data sequence number is already set.
        if (!frame->IsARetransmission())
        {
            frame->SetSequence(mDataSequence++);
        }

        break;

#endif
#endif // OPENTHREAD_FTD

    default:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);
    }

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    {
        uint8_t timeIeOffset = GetTimeIeOffset(*frame);

        frame->SetTimeIeOffset(timeIeOffset);

        if (timeIeOffset != 0)
        {
            frame->SetTimeSyncSeq(Get<TimeSync>().GetTimeSyncSeq());
            frame->SetNetworkTimeOffset(Get<TimeSync>().GetNetworkTimeOffset());
        }
    }
#endif

    if (!frame->IsSecurityProcessed())
    {
#if OPENTHREAD_CONFIG_MULTI_RADIO
        // Go through all selected radio link types for this tx and
        // copy the frame into correct `TxFrame` for each radio type
        // (if it is not already prepared).

        for (uint8_t index = 0; index < OT_ARRAY_LENGTH(RadioTypes::kAllRadioTypes); index++)
        {
            RadioType radio = RadioTypes::kAllRadioTypes[index];

            if (txFrames.GetSelectedRadioTypes().Contains(radio))
            {
                TxFrame &txFrame = txFrames.GetTxFrame(radio);

                if (txFrame.IsEmpty())
                {
                    txFrame.CopyFrom(*frame);
                }
            }
        }

        // Go through all selected radio link types for this tx and
        // process security for each radio type separately. This
        // allows radio links to handle security differently, e.g.,
        // with different keys or link frame counters.
        for (uint8_t index = 0; index < OT_ARRAY_LENGTH(RadioTypes::kAllRadioTypes); index++)
        {
            RadioType radio = RadioTypes::kAllRadioTypes[index];

            if (txFrames.GetSelectedRadioTypes().Contains(radio))
            {
                ProcessTransmitSecurity(txFrames.GetTxFrame(radio));
            }
        }
#else
        ProcessTransmitSecurity(*frame);
#endif
    }

    mBroadcastTransmitCount = 0;

#if OPENTHREAD_CONFIG_MULTI_RADIO
    mTxPendingRadioLinks = txFrames.GetSelectedRadioTypes();

    // If the "required radio type set" is empty,`mTxError` starts as
    // `kErrorAbort`. In this case, successful tx over any radio
    // link is sufficient for overall tx to be considered successful.
    // When the "required radio type set" is not empty, `mTxError`
    // starts as `kErrorNone` and we update it if tx over any link
    // in the required set fails.

    if (!txFrames.GetRequiredRadioTypes().IsEmpty())
    {
        mTxError = kErrorNone;
    }
#endif

#if OPENTHREAD_CONFIG_MAC_STAY_AWAKE_BETWEEN_FRAGMENTS
    if (!mRxOnWhenIdle && !mPromiscuous)
    {
        mShouldDelaySleep = frame->GetFramePending();
        otLogDebgMac("Delay sleep for pending tx");
    }
#endif

#if OPENTHREAD_CONFIG_MULTI_RADIO
    mLinks.Send(*frame, mTxPendingRadioLinks);
#else
    mLinks.Send();
#endif

exit:

    if (frame == nullptr)
    {
        // If the frame could not be prepared and the tx is being
        // aborted, we set the frame length to zero to mark it as empty.
        // The empty frame helps differentiate between an aborted tx due
        // to OpenThread itself not being able to prepare the frame, versus
        // the radio platform aborting the tx operation.

        frame = &txFrames.GetBroadcastTxFrame();
        frame->SetLength(0);
        HandleTransmitDone(*frame, nullptr, kErrorAbort);
    }
}

void Mac::RecordCcaStatus(bool aCcaSuccess, uint8_t aChannel)
{
    if (!aCcaSuccess)
    {
        mCounters.mTxErrCca++;
    }

    // Only track the CCA success rate for frame transmissions
    // on the PAN channel.

    if (aChannel == mPanChannel)
    {
        if (mCcaSampleCount < kMaxCcaSampleCount)
        {
            mCcaSampleCount++;
        }

        mCcaSuccessRateTracker.AddSample(aCcaSuccess, mCcaSampleCount);
    }
}

void Mac::RecordFrameTransmitStatus(const TxFrame &aFrame,
                                    const RxFrame *aAckFrame,
                                    Error          aError,
                                    uint8_t        aRetryCount,
                                    bool           aWillRetx)
{
    bool      ackRequested = aFrame.GetAckRequest();
    Address   dstAddr;
    Neighbor *neighbor;

    VerifyOrExit(!aFrame.IsEmpty());

    IgnoreError(aFrame.GetDstAddr(dstAddr));
    neighbor = Get<NeighborTable>().FindNeighbor(dstAddr);

    // Record frame transmission success/failure state (for a neighbor).

    if ((neighbor != nullptr) && ackRequested)
    {
        bool frameTxSuccess = true;

        // CCA or abort errors are excluded from frame tx error
        // rate tracking, since when they occur, the frame is
        // not actually sent over the air.

        switch (aError)
        {
        case kErrorNoAck:
            frameTxSuccess = false;

            OT_FALL_THROUGH;

        case kErrorNone:
            neighbor->GetLinkInfo().AddFrameTxStatus(frameTxSuccess);
            break;

        default:
            break;
        }
    }

    // Log frame transmission failure.

    if (aError != kErrorNone)
    {
        LogFrameTxFailure(aFrame, aError, aRetryCount, aWillRetx);
        otDumpDebgMac("TX ERR", aFrame.GetHeader(), 16);

        if (aWillRetx)
        {
            mCounters.mTxRetry++;

            // Since this failed transmission will be retried by `SubMac` layer
            // there is no need to update any other MAC counter. MAC counters
            // are updated on the final transmission attempt.

            ExitNow();
        }
    }

    // Update neighbor's RSSI link info from the received Ack.

    if ((aError == kErrorNone) && ackRequested && (aAckFrame != nullptr) && (neighbor != nullptr))
    {
        neighbor->GetLinkInfo().AddRss(aAckFrame->GetRssi());
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
        neighbor->AggregateLinkMetrics(/* aSeriesId */ 0, aAckFrame->GetType(), aAckFrame->GetLqi(),
                                       aAckFrame->GetRssi());
        ProcessEnhAckProbing(*aAckFrame, *neighbor);
#endif
#if OPENTHREAD_FTD
        if (aAckFrame->GetVersion() == Frame::kFcfFrameVersion2015)
        {
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
            ProcessCsl(*aAckFrame, dstAddr);
#endif
        }
#endif // OPENTHREAD_FTD
    }

    // Update MAC counters.

    mCounters.mTxTotal++;

    if (aError == kErrorAbort)
    {
        mCounters.mTxErrAbort++;
    }

    if (aError == kErrorChannelAccessFailure)
    {
        mCounters.mTxErrBusyChannel++;
    }

    if (ackRequested)
    {
        mCounters.mTxAckRequested++;

        if (aError == kErrorNone)
        {
            mCounters.mTxAcked++;
        }
    }
    else
    {
        mCounters.mTxNoAckRequested++;
    }

    if (dstAddr.IsBroadcast())
    {
        mCounters.mTxBroadcast++;
    }
    else
    {
        mCounters.mTxUnicast++;
    }

exit:
    return;
}

void Mac::HandleTransmitDone(TxFrame &aFrame, RxFrame *aAckFrame, Error aError)
{
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
    if (!aFrame.IsEmpty()
#if OPENTHREAD_CONFIG_MULTI_RADIO
        && (aFrame.GetRadioType() == kRadioTypeIeee802154)
#endif
    )
    {
        Address dstAddr;

        // Determine whether to re-transmit a broadcast frame.

        IgnoreError(aFrame.GetDstAddr(dstAddr));

        if (dstAddr.IsBroadcast())
        {
            mBroadcastTransmitCount++;

            if (mBroadcastTransmitCount < kTxNumBcast)
            {
#if OPENTHREAD_CONFIG_MULTI_RADIO
                {
                    RadioTypes radioTypes;
                    radioTypes.Add(kRadioTypeIeee802154);
                    mLinks.Send(aFrame, radioTypes);
                }
#else
                mLinks.Send();
#endif
                ExitNow();
            }

            mBroadcastTransmitCount = 0;
        }

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
        // Verify Enh-ACK integrity by checking its MIC
        if ((aError == kErrorNone) && (aAckFrame != nullptr) &&
            (ProcessEnhAckSecurity(aFrame, *aAckFrame) != kErrorNone))
        {
            aError = kErrorNoAck;
        }
#endif
    }
#endif // #if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE

#if OPENTHREAD_CONFIG_MULTI_RADIO
    if (!aFrame.IsEmpty())
    {
        RadioType  radio          = aFrame.GetRadioType();
        RadioTypes requriedRadios = mLinks.GetTxFrames().GetRequiredRadioTypes();

        Get<RadioSelector>().UpdateOnSendDone(aFrame, aError);

        if (requriedRadios.IsEmpty())
        {
            // If the "required radio type set" is empty, successful
            // tx over any radio link is sufficient for overall tx to
            // be considered successful. In this case `mTxError`
            // starts as `kErrorAbort` and we update it only when
            // it is not already `kErrorNone`.

            if (mTxError != kErrorNone)
            {
                mTxError = aError;
            }
        }
        else
        {
            // When the "required radio type set" is not empty we
            // expect the successful frame tx on all links in this set
            // to consider the overall tx successful. In this case,
            // `mTxError` starts as `kErrorNone` and we update it
            // if tx over any link in the set fails.

            if (requriedRadios.Contains(radio) && (aError != kErrorNone))
            {
                otLogDebgMac("Frame tx failed on required radio link %s with error %s", RadioTypeToString(radio),
                             ErrorToString(aError));

                mTxError = aError;
            }
        }

        // Keep track of radio links on which the frame is sent
        // and wait for all radio links to finish.
        mTxPendingRadioLinks.Remove(radio);

        VerifyOrExit(mTxPendingRadioLinks.IsEmpty());

        aError = mTxError;
    }
#endif

    // Determine next action based on current operation.

    switch (mOperation)
    {
    case kOperationActiveScan:
        mCounters.mTxBeaconRequest++;
        mTimer.Start(mScanDuration);
        break;

    case kOperationTransmitBeacon:
        mCounters.mTxBeacon++;
        FinishOperation();
        PerformNextOperation();
        break;

    case kOperationTransmitPoll:
        OT_ASSERT(aFrame.IsEmpty() || aFrame.GetAckRequest());

        if ((aError == kErrorNone) && (aAckFrame != nullptr))
        {
            bool framePending = aAckFrame->GetFramePending();

            if (IsEnabled() && framePending)
            {
                StartOperation(kOperationWaitingForData);
            }

            otLogInfoMac("Sent data poll, fp:%s", framePending ? "yes" : "no");
        }

        mCounters.mTxDataPoll++;
        FinishOperation();
        Get<DataPollSender>().HandlePollSent(aFrame, aError);
        PerformNextOperation();
        break;

    case kOperationTransmitDataDirect:
        mCounters.mTxData++;

        if (aError != kErrorNone)
        {
            mCounters.mTxDirectMaxRetryExpiry++;
        }
#if OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE
        else if (mLinks.GetTransmitRetries() < OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_MAX_SIZE_COUNT_DIRECT)
        {
            mRetryHistogram.mTxDirectRetrySuccess[mLinks.GetTransmitRetries()]++;
        }
#endif

        otDumpDebgMac("TX", aFrame.GetHeader(), aFrame.GetLength());
        FinishOperation();
        Get<MeshForwarder>().HandleSentFrame(aFrame, aError);
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
        Get<DataPollSender>().ProcessTxDone(aFrame, aAckFrame, aError);
#endif
        PerformNextOperation();
        break;

#if OPENTHREAD_FTD
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    case kOperationTransmitDataCsl:
        mCounters.mTxData++;

        otDumpDebgMac("TX", aFrame.GetHeader(), aFrame.GetLength());
        FinishOperation();
        Get<CslTxScheduler>().HandleSentFrame(aFrame, aError);
        PerformNextOperation();

        break;
#endif
    case kOperationTransmitDataIndirect:
        mCounters.mTxData++;

        if (aError != kErrorNone)
        {
            mCounters.mTxIndirectMaxRetryExpiry++;
        }
#if OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE
        else if (mLinks.GetTransmitRetries() < OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_MAX_SIZE_COUNT_INDIRECT)
        {
            mRetryHistogram.mTxIndirectRetrySuccess[mLinks.GetTransmitRetries()]++;
        }
#endif

        otDumpDebgMac("TX", aFrame.GetHeader(), aFrame.GetLength());
        FinishOperation();
        Get<DataPollHandler>().HandleSentFrame(aFrame, aError);
        PerformNextOperation();
        break;
#endif

    default:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(ExitNow()); // Added to suppress "unused label exit" warning (in TREL radio only).
        OT_UNREACHABLE_CODE(break);
    }

exit:
    return;
}

void Mac::HandleTimer(Timer &aTimer)
{
    aTimer.Get<Mac>().HandleTimer();
}

void Mac::HandleTimer(void)
{
    switch (mOperation)
    {
    case kOperationActiveScan:
        PerformActiveScan();
        break;

    case kOperationWaitingForData:
        otLogDebgMac("Data poll timeout");
        FinishOperation();
        Get<DataPollSender>().HandlePollTimeout();
        PerformNextOperation();
        break;

    case kOperationIdle:
        if (!mRxOnWhenIdle)
        {
#if OPENTHREAD_CONFIG_MAC_STAY_AWAKE_BETWEEN_FRAGMENTS
            if (mDelayingSleep)
            {
                otLogDebgMac("Sleep delay timeout expired");
                mDelayingSleep = false;
                UpdateIdleMode();
            }
#endif
        }
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
        else if (mPendingTransmitDataCsl)
        {
            PerformNextOperation();
        }
#endif
        break;

    default:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);
    }
}

Error Mac::ProcessReceiveSecurity(RxFrame &aFrame, const Address &aSrcAddr, Neighbor *aNeighbor)
{
    KeyManager &       keyManager = Get<KeyManager>();
    Error              error      = kErrorSecurity;
    uint8_t            securityLevel;
    uint8_t            keyIdMode;
    uint32_t           frameCounter;
    uint8_t            keyid;
    uint32_t           keySequence = 0;
    const KeyMaterial *macKey;
    const ExtAddress * extAddress;

    VerifyOrExit(aFrame.GetSecurityEnabled(), error = kErrorNone);

    IgnoreError(aFrame.GetSecurityLevel(securityLevel));
    VerifyOrExit(securityLevel == Frame::kSecEncMic32);

    IgnoreError(aFrame.GetFrameCounter(frameCounter));
    otLogDebgMac("Rx security - frame counter %u", frameCounter);

    IgnoreError(aFrame.GetKeyIdMode(keyIdMode));

    switch (keyIdMode)
    {
    case Frame::kKeyIdMode0:
        macKey     = &keyManager.GetKek();
        extAddress = &aSrcAddr.GetExtended();
        break;

    case Frame::kKeyIdMode1:
        VerifyOrExit(aNeighbor != nullptr);

        IgnoreError(aFrame.GetKeyId(keyid));
        keyid--;

        if (keyid == (keyManager.GetCurrentKeySequence() & 0x7f))
        {
            keySequence = keyManager.GetCurrentKeySequence();
            macKey      = mLinks.GetCurrentMacKey(aFrame);
        }
        else if (keyid == ((keyManager.GetCurrentKeySequence() - 1) & 0x7f))
        {
            keySequence = keyManager.GetCurrentKeySequence() - 1;
            macKey      = mLinks.GetTemporaryMacKey(aFrame, keySequence);
        }
        else if (keyid == ((keyManager.GetCurrentKeySequence() + 1) & 0x7f))
        {
            keySequence = keyManager.GetCurrentKeySequence() + 1;
            macKey      = mLinks.GetTemporaryMacKey(aFrame, keySequence);
        }
        else
        {
            ExitNow();
        }

        // If the frame is from a neighbor not in valid state (e.g., it is from a child being
        // restored), skip the key sequence and frame counter checks but continue to verify
        // the tag/MIC. Such a frame is later filtered in `RxDoneTask` which only allows MAC
        // Data Request frames from a child being restored.

        if (aNeighbor->IsStateValid())
        {
            VerifyOrExit(keySequence >= aNeighbor->GetKeySequence());

            if (keySequence == aNeighbor->GetKeySequence())
            {
                uint32_t neighborFrameCounter;

#if OPENTHREAD_CONFIG_MULTI_RADIO
                neighborFrameCounter = aNeighbor->GetLinkFrameCounters().Get(aFrame.GetRadioType());
#else
                neighborFrameCounter = aNeighbor->GetLinkFrameCounters().Get();
#endif

                // If frame counter is one off, then frame is a duplicate.
                VerifyOrExit((frameCounter + 1) != neighborFrameCounter, error = kErrorDuplicated);

                VerifyOrExit(frameCounter >= neighborFrameCounter);
            }
        }

        extAddress = &aSrcAddr.GetExtended();

        break;

    case Frame::kKeyIdMode2:
        macKey     = &mMode2KeyMaterial;
        extAddress = static_cast<const ExtAddress *>(&sMode2ExtAddress);
        break;

    default:
        ExitNow();
        OT_UNREACHABLE_CODE(break);
    }

    SuccessOrExit(aFrame.ProcessReceiveAesCcm(*extAddress, *macKey));

    if ((keyIdMode == Frame::kKeyIdMode1) && aNeighbor->IsStateValid())
    {
        if (aNeighbor->GetKeySequence() != keySequence)
        {
            aNeighbor->SetKeySequence(keySequence);
            aNeighbor->SetMleFrameCounter(0);
        }

#if OPENTHREAD_CONFIG_MULTI_RADIO
        aNeighbor->GetLinkFrameCounters().Set(aFrame.GetRadioType(), frameCounter + 1);
#else
        aNeighbor->GetLinkFrameCounters().Set(frameCounter + 1);
#endif

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2) && OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
#if OPENTHREAD_CONFIG_MULTI_RADIO
        if (aFrame.GetRadioType() == kRadioTypeIeee802154)
#endif
        {
            if ((frameCounter + 1) > aNeighbor->GetLinkAckFrameCounter())
            {
                aNeighbor->SetLinkAckFrameCounter(frameCounter + 1);
            }
        }
#endif

        if (keySequence > keyManager.GetCurrentKeySequence())
        {
            keyManager.SetCurrentKeySequence(keySequence);
        }
    }

    error = kErrorNone;

exit:
    return error;
}

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
Error Mac::ProcessEnhAckSecurity(TxFrame &aTxFrame, RxFrame &aAckFrame)
{
    Error              error = kErrorSecurity;
    uint8_t            securityLevel;
    uint8_t            txKeyId;
    uint8_t            ackKeyId;
    uint8_t            keyIdMode;
    uint32_t           frameCounter;
    Address            srcAddr;
    Address            dstAddr;
    Neighbor *         neighbor   = nullptr;
    KeyManager &       keyManager = Get<KeyManager>();
    const KeyMaterial *macKey;

    VerifyOrExit(aAckFrame.GetSecurityEnabled(), error = kErrorNone);
    VerifyOrExit(aAckFrame.IsVersion2015());

    IgnoreError(aAckFrame.GetSecurityLevel(securityLevel));
    VerifyOrExit(securityLevel == Frame::kSecEncMic32);

    IgnoreError(aAckFrame.GetKeyIdMode(keyIdMode));
    VerifyOrExit(keyIdMode == Frame::kKeyIdMode1, error = kErrorNone);

    IgnoreError(aTxFrame.GetKeyId(txKeyId));
    IgnoreError(aAckFrame.GetKeyId(ackKeyId));

    VerifyOrExit(txKeyId == ackKeyId);

    IgnoreError(aAckFrame.GetFrameCounter(frameCounter));
    otLogDebgMac("Rx security - Ack frame counter %u", frameCounter);

    IgnoreError(aAckFrame.GetSrcAddr(srcAddr));

    if (!srcAddr.IsNone())
    {
        neighbor = Get<NeighborTable>().FindNeighbor(srcAddr);
    }
    else
    {
        IgnoreError(aTxFrame.GetDstAddr(dstAddr));

        if (!dstAddr.IsNone())
        {
            // Get neighbor from destination address of transmitted frame
            neighbor = Get<NeighborTable>().FindNeighbor(dstAddr);
        }
    }

    if (!srcAddr.IsExtended() && neighbor != nullptr)
    {
        srcAddr.SetExtended(neighbor->GetExtAddress());
    }

    VerifyOrExit(srcAddr.IsExtended() && neighbor != nullptr);

    ackKeyId--;

    if (ackKeyId == (keyManager.GetCurrentKeySequence() & 0x7f))
    {
        macKey = &mLinks.GetSubMac().GetCurrentMacKey();
    }
    else if (ackKeyId == ((keyManager.GetCurrentKeySequence() - 1) & 0x7f))
    {
        macKey = &mLinks.GetSubMac().GetPreviousMacKey();
    }
    else if (ackKeyId == ((keyManager.GetCurrentKeySequence() + 1) & 0x7f))
    {
        macKey = &mLinks.GetSubMac().GetNextMacKey();
    }
    else
    {
        ExitNow();
    }

    if (neighbor->IsStateValid())
    {
        VerifyOrExit(frameCounter >= neighbor->GetLinkAckFrameCounter());
    }

    error = aAckFrame.ProcessReceiveAesCcm(srcAddr.GetExtended(), *macKey);
    SuccessOrExit(error);

    if (neighbor->IsStateValid())
    {
        neighbor->SetLinkAckFrameCounter(frameCounter + 1);
    }

exit:
    if (error != kErrorNone)
    {
        otLogInfoMac("Frame tx attempt failed, error: Enh-ACK security check fail");
    }

    return error;
}
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

void Mac::HandleReceivedFrame(RxFrame *aFrame, Error aError)
{
    Address   srcaddr;
    Address   dstaddr;
    PanId     panid;
    Neighbor *neighbor;
    Error     error = aError;

    mCounters.mRxTotal++;

    SuccessOrExit(error);
    VerifyOrExit(aFrame != nullptr, error = kErrorNoFrameReceived);
    VerifyOrExit(IsEnabled(), error = kErrorInvalidState);

    // Ensure we have a valid frame before attempting to read any contents of
    // the buffer received from the radio.
    SuccessOrExit(error = aFrame->ValidatePsdu());

    IgnoreError(aFrame->GetSrcAddr(srcaddr));
    IgnoreError(aFrame->GetDstAddr(dstaddr));
    neighbor = Get<NeighborTable>().FindNeighbor(srcaddr);

    // Destination Address Filtering
    switch (dstaddr.GetType())
    {
    case Address::kTypeNone:
        break;

    case Address::kTypeShort:
        VerifyOrExit((mRxOnWhenIdle && dstaddr.IsBroadcast()) || dstaddr.GetShort() == GetShortAddress(),
                     error = kErrorDestinationAddressFiltered);

#if OPENTHREAD_FTD
        // Allow multicasts from neighbor routers if FTD
        if (neighbor == nullptr && dstaddr.IsBroadcast() && Get<Mle::MleRouter>().IsFullThreadDevice())
        {
            neighbor = Get<NeighborTable>().FindRxOnlyNeighborRouter(srcaddr);
        }
#endif

        break;

    case Address::kTypeExtended:
        VerifyOrExit(dstaddr.GetExtended() == GetExtAddress(), error = kErrorDestinationAddressFiltered);
        break;
    }

    // Verify destination PAN ID if present
    if (kErrorNone == aFrame->GetDstPanId(panid))
    {
        VerifyOrExit(panid == kShortAddrBroadcast || panid == mPanId, error = kErrorDestinationAddressFiltered);
    }

    // Source Address Filtering
    switch (srcaddr.GetType())
    {
    case Address::kTypeNone:
        break;

    case Address::kTypeShort:
        otLogDebgMac("Received frame from short address 0x%04x", srcaddr.GetShort());

        VerifyOrExit(neighbor != nullptr, error = kErrorUnknownNeighbor);

        srcaddr.SetExtended(neighbor->GetExtAddress());

        OT_FALL_THROUGH;

    case Address::kTypeExtended:

        // Duplicate Address Protection
        VerifyOrExit(srcaddr.GetExtended() != GetExtAddress(), error = kErrorInvalidSourceAddress);

#if OPENTHREAD_CONFIG_MAC_FILTER_ENABLE
        {
            int8_t fixedRss;

            SuccessOrExit(error = mFilter.Apply(srcaddr.GetExtended(), fixedRss));

            if (fixedRss != Filter::kFixedRssDisabled)
            {
                aFrame->SetRssi(fixedRss);

                // Clear any previous link info to ensure the fixed RSSI
                // value takes effect quickly.
                if (neighbor != nullptr)
                {
                    neighbor->GetLinkInfo().Clear();
                }
            }
        }
#endif

        break;
    }

    if (dstaddr.IsBroadcast())
    {
        mCounters.mRxBroadcast++;
    }
    else
    {
        mCounters.mRxUnicast++;
    }

    error = ProcessReceiveSecurity(*aFrame, srcaddr, neighbor);

    switch (error)
    {
    case kErrorDuplicated:

        // Allow a duplicate received frame pass, only if the
        // current operation is `kOperationWaitingForData` (i.e.,
        // the sleepy device is waiting to receive a frame after
        // a data poll ack from parent indicating there is a
        // pending frame for it). This ensures that the sleepy
        // device goes to sleep faster and avoids a data poll
        // timeout.
        //
        // Note that `error` is checked again later after the
        // operation `kOperationWaitingForData` is processed
        // so the duplicate frame will not be passed to next
        // layer (`MeshForwarder`).

        VerifyOrExit(mOperation == kOperationWaitingForData);

        OT_FALL_THROUGH;

    case kErrorNone:
        break;

    default:
        ExitNow();
    }

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    if (aFrame->GetVersion() == Frame::kFcfFrameVersion2015)
    {
        ProcessCsl(*aFrame, srcaddr);
    }
#endif

    Get<DataPollSender>().ProcessRxFrame(*aFrame);

    if (neighbor != nullptr)
    {
        neighbor->GetLinkInfo().AddRss(aFrame->GetRssi());
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
        neighbor->AggregateLinkMetrics(/* aSeriesId */ 0, aFrame->GetType(), aFrame->GetLqi(), aFrame->GetRssi());
#endif

        if (aFrame->GetSecurityEnabled())
        {
            uint8_t keyIdMode;

            IgnoreError(aFrame->GetKeyIdMode(keyIdMode));

            if (keyIdMode == Frame::kKeyIdMode1)
            {
                switch (neighbor->GetState())
                {
                case Neighbor::kStateValid:
                    break;

                case Neighbor::kStateRestored:
                case Neighbor::kStateChildUpdateRequest:

                    // Only accept a "MAC Data Request" frame from a child being restored.
                    VerifyOrExit(aFrame->IsDataRequestCommand(), error = kErrorDrop);
                    break;

                default:
                    ExitNow(error = kErrorUnknownNeighbor);
                }

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2 && OPENTHREAD_FTD
                // From Thread 1.2, MAC Data Frame can also act as keep-alive message if child supports
                if (aFrame->GetType() == Frame::kFcfFrameData && !neighbor->IsRxOnWhenIdle() &&
                    neighbor->IsEnhancedKeepAliveSupported())
                {
                    neighbor->SetLastHeard(TimerMilli::GetNow());
                }
#endif
            }

#if OPENTHREAD_CONFIG_MULTI_RADIO
            Get<RadioSelector>().UpdateOnReceive(*neighbor, aFrame->GetRadioType(), /* aIsDuplicate */ false);
#endif
        }
    }

    switch (mOperation)
    {
    case kOperationActiveScan:

        if (aFrame->GetType() == Frame::kFcfFrameBeacon)
        {
            mCounters.mRxBeacon++;
            ReportActiveScanResult(aFrame);
            ExitNow();
        }

        OT_FALL_THROUGH;

    case kOperationEnergyScan:

        // We can possibly receive a data frame while either active or
        // energy scan is ongoing. We continue to process the frame only
        // if the current scan channel matches `mPanChannel`.

        VerifyOrExit(mScanChannel == mPanChannel, mCounters.mRxOther++);
        break;

    case kOperationWaitingForData:

        if (!dstaddr.IsNone())
        {
            mTimer.Stop();

#if OPENTHREAD_CONFIG_MAC_STAY_AWAKE_BETWEEN_FRAGMENTS
            if (!mRxOnWhenIdle && !mPromiscuous && aFrame->GetFramePending())
            {
                mShouldDelaySleep = true;
                otLogDebgMac("Delay sleep for pending rx");
            }
#endif
            FinishOperation();
            PerformNextOperation();
        }

        SuccessOrExit(error);

        break;

    default:
        break;
    }

    switch (aFrame->GetType())
    {
    case Frame::kFcfFrameMacCmd:
        if (HandleMacCommand(*aFrame)) // returns `true` when handled
        {
            ExitNow(error = kErrorNone);
        }

        break;

    case Frame::kFcfFrameBeacon:
        mCounters.mRxBeacon++;
        break;

    case Frame::kFcfFrameData:
        mCounters.mRxData++;
        break;

    default:
        mCounters.mRxOther++;
        ExitNow();
    }

    otDumpDebgMac("RX", aFrame->GetHeader(), aFrame->GetLength());
    Get<MeshForwarder>().HandleReceivedFrame(*aFrame);

    UpdateIdleMode();

exit:

    if (error != kErrorNone)
    {
        LogFrameRxFailure(aFrame, error);

        switch (error)
        {
        case kErrorSecurity:
            mCounters.mRxErrSec++;
            break;

        case kErrorFcs:
            mCounters.mRxErrFcs++;
            break;

        case kErrorNoFrameReceived:
            mCounters.mRxErrNoFrame++;
            break;

        case kErrorUnknownNeighbor:
            mCounters.mRxErrUnknownNeighbor++;
            break;

        case kErrorInvalidSourceAddress:
            mCounters.mRxErrInvalidSrcAddr++;
            break;

        case kErrorAddressFiltered:
            mCounters.mRxAddressFiltered++;
            break;

        case kErrorDestinationAddressFiltered:
            mCounters.mRxDestAddrFiltered++;
            break;

        case kErrorDuplicated:
            mCounters.mRxDuplicated++;
            break;

        default:
            mCounters.mRxErrOther++;
            break;
        }
    }
}

bool Mac::HandleMacCommand(RxFrame &aFrame)
{
    bool    didHandle = false;
    uint8_t commandId;

    IgnoreError(aFrame.GetCommandId(commandId));

    switch (commandId)
    {
    case Frame::kMacCmdBeaconRequest:
        mCounters.mRxBeaconRequest++;
        otLogInfoMac("Received Beacon Request");

        if (ShouldSendBeacon())
        {
#if OPENTHREAD_CONFIG_MULTI_RADIO
            mTxBeaconRadioLinks.Add(aFrame.GetRadioType());
#endif
            StartOperation(kOperationTransmitBeacon);
        }

        didHandle = true;
        break;

    case Frame::kMacCmdDataRequest:
        mCounters.mRxDataPoll++;
#if OPENTHREAD_FTD
        Get<DataPollHandler>().HandleDataPoll(aFrame);
        didHandle = true;
#endif
        break;

    default:
        mCounters.mRxOther++;
        break;
    }

    return didHandle;
}

void Mac::SetPromiscuous(bool aPromiscuous)
{
    mPromiscuous = aPromiscuous;
    Get<Radio>().SetPromiscuous(aPromiscuous);

#if OPENTHREAD_CONFIG_MAC_STAY_AWAKE_BETWEEN_FRAGMENTS
    mDelayingSleep    = false;
    mShouldDelaySleep = false;
#endif

    mLinks.SetRxOnWhenBackoff(mRxOnWhenIdle || mPromiscuous);
    UpdateIdleMode();
}

#if OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE
const uint32_t *Mac::GetDirectRetrySuccessHistogram(uint8_t &aNumberOfEntries)
{
    if (mMaxFrameRetriesDirect >= OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_MAX_SIZE_COUNT_DIRECT)
    {
        aNumberOfEntries = OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_MAX_SIZE_COUNT_DIRECT;
    }
    else
    {
        aNumberOfEntries = mMaxFrameRetriesDirect + 1;
    }

    return mRetryHistogram.mTxDirectRetrySuccess;
}

#if OPENTHREAD_FTD
const uint32_t *Mac::GetIndirectRetrySuccessHistogram(uint8_t &aNumberOfEntries)
{
    if (mMaxFrameRetriesIndirect >= OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_MAX_SIZE_COUNT_INDIRECT)
    {
        aNumberOfEntries = OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_MAX_SIZE_COUNT_INDIRECT;
    }
    else
    {
        aNumberOfEntries = mMaxFrameRetriesIndirect + 1;
    }

    return mRetryHistogram.mTxIndirectRetrySuccess;
}
#endif

void Mac::ResetRetrySuccessHistogram()
{
    memset(&mRetryHistogram, 0, sizeof(mRetryHistogram));
}
#endif // OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE

// LCOV_EXCL_START

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_MAC == 1)

const char *Mac::OperationToString(Operation aOperation)
{
    static const char *const kOperationStrings[] = {
        "Idle",               // (0) kOperationIdle
        "ActiveScan",         // (1) kOperationActiveScan
        "EnergyScan",         // (2) kOperationEnergyScan
        "TransmitBeacon",     // (3) kOperationTransmitBeacon
        "TransmitDataDirect", // (4) kOperationTransmitDataDirect
        "TransmitPoll",       // (5) kOperationTransmitPoll
        "WaitingForData",     // (6) kOperationWaitingForData
#if OPENTHREAD_FTD
        "TransmitDataIndirect", // (7) kOperationTransmitDataIndirect
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
        "TransmitDataCsl", // (8) kOperationTransmitDataCsl
#endif
#endif
    };

    static_assert(kOperationIdle == 0, "kOperationIdle value is incorrect");
    static_assert(kOperationActiveScan == 1, "kOperationActiveScan value is incorrect");
    static_assert(kOperationEnergyScan == 2, "kOperationEnergyScan value is incorrect");
    static_assert(kOperationTransmitBeacon == 3, "kOperationTransmitBeacon value is incorrect");
    static_assert(kOperationTransmitDataDirect == 4, "kOperationTransmitDataDirect value is incorrect");
    static_assert(kOperationTransmitPoll == 5, "kOperationTransmitPoll value is incorrect");
    static_assert(kOperationWaitingForData == 6, "kOperationWaitingForData value is incorrect");
#if OPENTHREAD_FTD
    static_assert(kOperationTransmitDataIndirect == 7, "kOperationTransmitDataIndirect value is incorrect");
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    static_assert(kOperationTransmitDataCsl == 8, "TransmitDataCsl value is incorrect");
#endif
#endif

    return kOperationStrings[aOperation];
}

void Mac::LogFrameRxFailure(const RxFrame *aFrame, Error aError) const
{
    otLogLevel logLevel;

    switch (aError)
    {
    case kErrorAbort:
    case kErrorNoFrameReceived:
    case kErrorDestinationAddressFiltered:
        logLevel = OT_LOG_LEVEL_DEBG;
        break;

    default:
        logLevel = OT_LOG_LEVEL_INFO;
        break;
    }

    if (aFrame == nullptr)
    {
        otLogMac(logLevel, "Frame rx failed, error:%s", ErrorToString(aError));
    }
    else
    {
        otLogMac(logLevel, "Frame rx failed, error:%s, %s", ErrorToString(aError), aFrame->ToInfoString().AsCString());
    }
}

void Mac::LogFrameTxFailure(const TxFrame &aFrame, Error aError, uint8_t aRetryCount, bool aWillRetx) const
{
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE && OPENTHREAD_CONFIG_MULTI_RADIO
    if (aFrame.GetRadioType() == kRadioTypeIeee802154)
#elif OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
    if (true)
#else
    if (false)
#endif
    {
        uint8_t maxAttempts = aFrame.GetMaxFrameRetries() + 1;
        uint8_t curAttempt  = aWillRetx ? (aRetryCount + 1) : maxAttempts;

        otLogInfoMac("Frame tx attempt %d/%d failed, error:%s, %s", curAttempt, maxAttempts, ErrorToString(aError),
                     aFrame.ToInfoString().AsCString());
    }
    else
    {
        otLogInfoMac("Frame tx failed, error:%s, %s", ErrorToString(aError), aFrame.ToInfoString().AsCString());
    }
}

void Mac::LogBeacon(const char *aActionText, const BeaconPayload &aBeaconPayload) const
{
    otLogInfoMac("%s Beacon, %s", aActionText, aBeaconPayload.ToInfoString().AsCString());
}

#else // #if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_MAC == 1)

void Mac::LogFrameRxFailure(const RxFrame *, Error) const
{
}

void Mac::LogBeacon(const char *, const BeaconPayload &) const
{
}

void Mac::LogFrameTxFailure(const TxFrame &, Error, uint8_t, bool) const
{
}

#endif // #if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_MAC == 1)

// LCOV_EXCL_STOP

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
uint8_t Mac::GetTimeIeOffset(const Frame &aFrame)
{
    uint8_t        offset = 0;
    const uint8_t *base   = aFrame.GetPsdu();
    const uint8_t *cur    = nullptr;

    cur = reinterpret_cast<const uint8_t *>(aFrame.GetTimeIe());
    VerifyOrExit(cur != nullptr);

    cur += sizeof(VendorIeHeader);
    offset = static_cast<uint8_t>(cur - base);

exit:
    return offset;
}
#endif

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
void Mac::SetCslChannel(uint8_t aChannel)
{
    VerifyOrExit(GetCslChannel() != aChannel);

    mLinks.GetSubMac().SetCslChannel(aChannel);
    mLinks.GetSubMac().SetCslChannelSpecified(aChannel != 0);

    if (IsCslEnabled())
    {
        Get<Mle::Mle>().ScheduleChildUpdateRequest();
    }
exit:
    return;
}

void Mac::SetCslPeriod(uint16_t aPeriod)
{
    mLinks.GetSubMac().SetCslPeriod(aPeriod);

    Get<DataPollSender>().RecalculatePollPeriod();

    if ((GetCslPeriod() == 0) || IsCslEnabled())
    {
        IgnoreError(Get<Radio>().EnableCsl(GetCslPeriod(), Get<Mle::Mle>().GetParent().GetRloc16(),
                                           &Get<Mle::Mle>().GetParent().GetExtAddress()));
    }

    if (IsCslEnabled())
    {
        Get<Mle::Mle>().ScheduleChildUpdateRequest();
    }

    UpdateIdleMode();
}

bool Mac::IsCslEnabled(void) const
{
    return !GetRxOnWhenIdle() && IsCslCapable();
}

bool Mac::IsCslCapable(void) const
{
    return (GetCslPeriod() > 0) && Get<Mle::MleRouter>().IsChild() &&
           Get<Mle::Mle>().GetParent().IsEnhancedKeepAliveSupported();
}

#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
void Mac::ProcessCsl(const RxFrame &aFrame, const Address &aSrcAddr)
{
    const uint8_t *cur   = aFrame.GetHeaderIe(CslIe::kHeaderIeId);
    Child *        child = Get<ChildTable>().FindChild(aSrcAddr, Child::kInStateAnyExceptInvalid);
    const CslIe *  csl;

    VerifyOrExit(cur != nullptr && child != nullptr && aFrame.GetSecurityEnabled());

    csl = reinterpret_cast<const CslIe *>(cur + sizeof(HeaderIe));

    child->SetCslPeriod(csl->GetPeriod());
    // Use ceiling to ensure the the time diff will be within kUsPerTenSymbols
    child->SetCslPhase(csl->GetPhase());
    child->SetCslSynchronized(true);
    child->SetCslLastHeard(TimerMilli::GetNow());
    child->SetLastRxTimestamp(aFrame.GetTimestamp());
    otLogDebgMac("Timestamp=%u Sequence=%u CslPeriod=%hu CslPhase=%hu TransmitPhase=%hu",
                 static_cast<uint32_t>(aFrame.GetTimestamp()), aFrame.GetSequence(), csl->GetPeriod(), csl->GetPhase(),
                 child->GetCslPhase());

    Get<CslTxScheduler>().Update();

exit:
    return;
}
#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
void Mac::ProcessEnhAckProbing(const RxFrame &aFrame, const Neighbor &aNeighbor)
{
    constexpr uint8_t kEnhAckProbingIeMaxLen = 2;

    const HeaderIe *enhAckProbingIe =
        reinterpret_cast<const HeaderIe *>(aFrame.GetThreadIe(ThreadIe::kEnhAckProbingIe));
    const uint8_t *data =
        reinterpret_cast<const uint8_t *>(enhAckProbingIe) + sizeof(HeaderIe) + sizeof(VendorIeHeader);
    uint8_t dataLen = 0;

    VerifyOrExit(enhAckProbingIe != nullptr);

    dataLen = enhAckProbingIe->GetLength() - sizeof(VendorIeHeader);
    VerifyOrExit(dataLen <= kEnhAckProbingIeMaxLen);

    Get<LinkMetrics::LinkMetrics>().ProcessEnhAckIeData(data, dataLen, aNeighbor);
exit:
    return;
}
#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE

} // namespace Mac
} // namespace ot
