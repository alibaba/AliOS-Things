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
 *   This file implements common methods for manipulating MeshCoP Datasets.
 *
 */

#include "dataset.hpp"

#include <stdio.h>

#include "common/code_utils.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "mac/mac_types.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "thread/mle_tlvs.hpp"

namespace ot {
namespace MeshCoP {

Error Dataset::Info::GenerateRandom(Instance &aInstance)
{
    Error            error;
    Mac::ChannelMask supportedChannels = aInstance.Get<Mac::Mac>().GetSupportedChannelMask();
    Mac::ChannelMask preferredChannels(aInstance.Get<Radio>().GetPreferredChannelMask());

    // If the preferred channel mask is not empty, select a random
    // channel from it, otherwise choose one from the supported
    // channel mask.

    preferredChannels.Intersect(supportedChannels);

    if (preferredChannels.IsEmpty())
    {
        preferredChannels = supportedChannels;
    }

    Clear();

    mActiveTimestamp = 1;
    mChannel         = preferredChannels.ChooseRandomChannel();
    mChannelMask     = supportedChannels.GetMask();
    mPanId           = Mac::GenerateRandomPanId();
    static_cast<SecurityPolicy &>(mSecurityPolicy).SetToDefault();

    SuccessOrExit(error = static_cast<NetworkKey &>(mNetworkKey).GenerateRandom());
    SuccessOrExit(error = static_cast<Pskc &>(mPskc).GenerateRandom());
    SuccessOrExit(error = Random::Crypto::FillBuffer(mExtendedPanId.m8, sizeof(mExtendedPanId.m8)));
    SuccessOrExit(error = static_cast<Ip6::NetworkPrefix &>(mMeshLocalPrefix).GenerateRandomUla());

    snprintf(mNetworkName.m8, sizeof(mNetworkName), "OpenThread-%04x", mPanId);

    mComponents.mIsActiveTimestampPresent = true;
    mComponents.mIsNetworkKeyPresent      = true;
    mComponents.mIsNetworkNamePresent     = true;
    mComponents.mIsExtendedPanIdPresent   = true;
    mComponents.mIsMeshLocalPrefixPresent = true;
    mComponents.mIsPanIdPresent           = true;
    mComponents.mIsChannelPresent         = true;
    mComponents.mIsPskcPresent            = true;
    mComponents.mIsSecurityPolicyPresent  = true;
    mComponents.mIsChannelMaskPresent     = true;

exit:
    return error;
}

bool Dataset::Info::IsSubsetOf(const Info &aOther) const
{
    bool isSubset = false;

    if (IsNetworkKeyPresent())
    {
        VerifyOrExit(aOther.IsNetworkKeyPresent() && GetNetworkKey() == aOther.GetNetworkKey());
    }

    if (IsNetworkNamePresent())
    {
        VerifyOrExit(aOther.IsNetworkNamePresent() && GetNetworkName() == aOther.GetNetworkName());
    }

    if (IsExtendedPanIdPresent())
    {
        VerifyOrExit(aOther.IsExtendedPanIdPresent() && GetExtendedPanId() == aOther.GetExtendedPanId());
    }

    if (IsMeshLocalPrefixPresent())
    {
        VerifyOrExit(aOther.IsMeshLocalPrefixPresent() && GetMeshLocalPrefix() == aOther.GetMeshLocalPrefix());
    }

    if (IsPanIdPresent())
    {
        VerifyOrExit(aOther.IsPanIdPresent() && GetPanId() == aOther.GetPanId());
    }

    if (IsChannelPresent())
    {
        VerifyOrExit(aOther.IsChannelPresent() && GetChannel() == aOther.GetChannel());
    }

    if (IsPskcPresent())
    {
        VerifyOrExit(aOther.IsPskcPresent() && GetPskc() == aOther.GetPskc());
    }

    if (IsSecurityPolicyPresent())
    {
        VerifyOrExit(aOther.IsSecurityPolicyPresent() && GetSecurityPolicy() == aOther.GetSecurityPolicy());
    }

    if (IsChannelMaskPresent())
    {
        VerifyOrExit(aOther.IsChannelMaskPresent() && GetChannelMask() == aOther.GetChannelMask());
    }

    isSubset = true;

exit:
    return isSubset;
}

Dataset::Dataset(void)
    : mUpdateTime(0)
    , mLength(0)
{
    memset(mTlvs, 0, sizeof(mTlvs));
}

void Dataset::Clear(void)
{
    mLength = 0;
}

bool Dataset::IsValid(void) const
{
    bool       rval = true;
    const Tlv *end  = GetTlvsEnd();

    for (const Tlv *cur = GetTlvsStart(); cur < end; cur = cur->GetNext())
    {
        VerifyOrExit(!cur->IsExtended() && (cur + 1) <= end && cur->GetNext() <= end && Tlv::IsValid(*cur),
                     rval = false);
    }

exit:
    return rval;
}

const Tlv *Dataset::GetTlv(Tlv::Type aType) const
{
    return Tlv::FindTlv(mTlvs, mLength, aType);
}

void Dataset::ConvertTo(Info &aDatasetInfo) const
{
    aDatasetInfo.Clear();

    for (const Tlv *cur = GetTlvsStart(); cur < GetTlvsEnd(); cur = cur->GetNext())
    {
        switch (cur->GetType())
        {
        case Tlv::kActiveTimestamp:
            aDatasetInfo.SetActiveTimestamp(static_cast<const ActiveTimestampTlv *>(cur)->GetTimestamp().GetSeconds());
            break;

        case Tlv::kChannel:
            aDatasetInfo.SetChannel(static_cast<const ChannelTlv *>(cur)->GetChannel());
            break;

        case Tlv::kChannelMask:
        {
            uint32_t mask = static_cast<const ChannelMaskTlv *>(cur)->GetChannelMask();

            if (mask != 0)
            {
                aDatasetInfo.SetChannelMask(mask);
            }

            break;
        }

        case Tlv::kDelayTimer:
            aDatasetInfo.SetDelay(static_cast<const DelayTimerTlv *>(cur)->GetDelayTimer());
            break;

        case Tlv::kExtendedPanId:
            aDatasetInfo.SetExtendedPanId(static_cast<const ExtendedPanIdTlv *>(cur)->GetExtendedPanId());
            break;

        case Tlv::kMeshLocalPrefix:
            aDatasetInfo.SetMeshLocalPrefix(static_cast<const MeshLocalPrefixTlv *>(cur)->GetMeshLocalPrefix());
            break;

        case Tlv::kNetworkKey:
            aDatasetInfo.SetNetworkKey(static_cast<const NetworkKeyTlv *>(cur)->GetNetworkKey());
            break;

        case Tlv::kNetworkName:
            aDatasetInfo.SetNetworkName(static_cast<const NetworkNameTlv *>(cur)->GetNetworkName());
            break;

        case Tlv::kPanId:
            aDatasetInfo.SetPanId(static_cast<const PanIdTlv *>(cur)->GetPanId());
            break;

        case Tlv::kPendingTimestamp:
            aDatasetInfo.SetPendingTimestamp(
                static_cast<const PendingTimestampTlv *>(cur)->GetTimestamp().GetSeconds());
            break;

        case Tlv::kPskc:
            aDatasetInfo.SetPskc(static_cast<const PskcTlv *>(cur)->GetPskc());
            break;

        case Tlv::kSecurityPolicy:
        {
            const SecurityPolicyTlv *tlv = static_cast<const SecurityPolicyTlv *>(cur);

            aDatasetInfo.SetSecurityPolicy(tlv->GetSecurityPolicy());
            break;
        }

        default:
            break;
        }
    }
}

void Dataset::ConvertTo(otOperationalDatasetTlvs &aDataset) const
{
    memcpy(aDataset.mTlvs, mTlvs, mLength);
    aDataset.mLength = static_cast<uint8_t>(mLength);
}

void Dataset::Set(Type aType, const Dataset &aDataset)
{
    memcpy(mTlvs, aDataset.mTlvs, aDataset.mLength);
    mLength = aDataset.mLength;

    if (aType == kActive)
    {
        RemoveTlv(Tlv::kPendingTimestamp);
        RemoveTlv(Tlv::kDelayTimer);
    }

    mUpdateTime = aDataset.GetUpdateTime();
}

void Dataset::SetFrom(const otOperationalDatasetTlvs &aDataset)
{
    mLength = aDataset.mLength;
    memcpy(mTlvs, aDataset.mTlvs, mLength);
}

Error Dataset::SetFrom(const Info &aDatasetInfo)
{
    Error error = kErrorNone;

    if (aDatasetInfo.IsActiveTimestampPresent())
    {
        Timestamp timestamp;

        timestamp.Clear();
        timestamp.SetSeconds(aDatasetInfo.GetActiveTimestamp());
        IgnoreError(SetTlv(Tlv::kActiveTimestamp, timestamp));
    }

    if (aDatasetInfo.IsPendingTimestampPresent())
    {
        Timestamp timestamp;

        timestamp.Clear();
        timestamp.SetSeconds(aDatasetInfo.GetPendingTimestamp());
        IgnoreError(SetTlv(Tlv::kPendingTimestamp, timestamp));
    }

    if (aDatasetInfo.IsDelayPresent())
    {
        IgnoreError(SetTlv(Tlv::kDelayTimer, aDatasetInfo.GetDelay()));
    }

    if (aDatasetInfo.IsChannelPresent())
    {
        ChannelTlv tlv;
        tlv.Init();
        tlv.SetChannel(aDatasetInfo.GetChannel());
        IgnoreError(SetTlv(tlv));
    }

    if (aDatasetInfo.IsChannelMaskPresent())
    {
        ChannelMaskTlv tlv;
        tlv.Init();
        tlv.SetChannelMask(aDatasetInfo.GetChannelMask());
        IgnoreError(SetTlv(tlv));
    }

    if (aDatasetInfo.IsExtendedPanIdPresent())
    {
        IgnoreError(SetTlv(Tlv::kExtendedPanId, aDatasetInfo.GetExtendedPanId()));
    }

    if (aDatasetInfo.IsMeshLocalPrefixPresent())
    {
        IgnoreError(SetTlv(Tlv::kMeshLocalPrefix, aDatasetInfo.GetMeshLocalPrefix()));
    }

    if (aDatasetInfo.IsNetworkKeyPresent())
    {
        IgnoreError(SetTlv(Tlv::kNetworkKey, aDatasetInfo.GetNetworkKey()));
    }

    if (aDatasetInfo.IsNetworkNamePresent())
    {
        Mac::NameData nameData = aDatasetInfo.GetNetworkName().GetAsData();

        IgnoreError(SetTlv(Tlv::kNetworkName, nameData.GetBuffer(), nameData.GetLength()));
    }

    if (aDatasetInfo.IsPanIdPresent())
    {
        IgnoreError(SetTlv(Tlv::kPanId, aDatasetInfo.GetPanId()));
    }

    if (aDatasetInfo.IsPskcPresent())
    {
        IgnoreError(SetTlv(Tlv::kPskc, aDatasetInfo.GetPskc()));
    }

    if (aDatasetInfo.IsSecurityPolicyPresent())
    {
        SecurityPolicyTlv tlv;

        tlv.Init();
        tlv.SetSecurityPolicy(aDatasetInfo.GetSecurityPolicy());
        IgnoreError(SetTlv(tlv));
    }

    mUpdateTime = TimerMilli::GetNow();

    return error;
}

Error Dataset::GetTimestamp(Type aType, Timestamp &aTimestamp) const
{
    Error error = kErrorNone;

    if (aType == kActive)
    {
        const ActiveTimestampTlv *tlv = GetTlv<ActiveTimestampTlv>();

        VerifyOrExit(tlv != nullptr, error = kErrorNotFound);
        aTimestamp = tlv->GetTimestamp();
    }
    else
    {
        const PendingTimestampTlv *tlv = GetTlv<PendingTimestampTlv>();

        VerifyOrExit(tlv != nullptr, error = kErrorNotFound);
        aTimestamp = tlv->GetTimestamp();
    }

exit:
    return error;
}

void Dataset::SetTimestamp(Type aType, const Timestamp &aTimestamp)
{
    IgnoreError(SetTlv((aType == kActive) ? Tlv::kActiveTimestamp : Tlv::kPendingTimestamp, aTimestamp));
}

Error Dataset::SetTlv(Tlv::Type aType, const void *aValue, uint8_t aLength)
{
    Error    error          = kErrorNone;
    uint16_t bytesAvailable = sizeof(mTlvs) - mLength;
    Tlv *    old            = GetTlv(aType);
    Tlv      tlv;

    if (old != nullptr)
    {
        bytesAvailable += sizeof(Tlv) + old->GetLength();
    }

    VerifyOrExit(sizeof(Tlv) + aLength <= bytesAvailable, error = kErrorNoBufs);

    if (old != nullptr)
    {
        RemoveTlv(old);
    }

    tlv.SetType(aType);
    tlv.SetLength(aLength);
    memcpy(mTlvs + mLength, &tlv, sizeof(Tlv));
    mLength += sizeof(Tlv);

    memcpy(mTlvs + mLength, aValue, aLength);
    mLength += aLength;

    mUpdateTime = TimerMilli::GetNow();

exit:
    return error;
}

Error Dataset::SetTlv(const Tlv &aTlv)
{
    return SetTlv(aTlv.GetType(), aTlv.GetValue(), aTlv.GetLength());
}

Error Dataset::Set(const Message &aMessage, uint16_t aOffset, uint8_t aLength)
{
    Error error = kErrorInvalidArgs;

    SuccessOrExit(aMessage.Read(aOffset, mTlvs, aLength));
    mLength = aLength;

    mUpdateTime = TimerMilli::GetNow();
    error       = kErrorNone;

exit:
    return error;
}

void Dataset::RemoveTlv(Tlv::Type aType)
{
    Tlv *tlv;

    VerifyOrExit((tlv = GetTlv(aType)) != nullptr);
    RemoveTlv(tlv);

exit:
    return;
}

Error Dataset::AppendMleDatasetTlv(Type aType, Message &aMessage) const
{
    Error          error = kErrorNone;
    Mle::Tlv       tlv;
    Mle::Tlv::Type type;

    VerifyOrExit(mLength > 0);

    type = (aType == kActive ? Mle::Tlv::kActiveDataset : Mle::Tlv::kPendingDataset);

    tlv.SetType(type);
    tlv.SetLength(static_cast<uint8_t>(mLength) - sizeof(Tlv) - sizeof(Timestamp));
    SuccessOrExit(error = aMessage.Append(tlv));

    for (const Tlv *cur = GetTlvsStart(); cur < GetTlvsEnd(); cur = cur->GetNext())
    {
        if (((aType == kActive) && (cur->GetType() == Tlv::kActiveTimestamp)) ||
            ((aType == kPending) && (cur->GetType() == Tlv::kPendingTimestamp)))
        {
            ; // skip Active or Pending Timestamp TLV
        }
        else if (cur->GetType() == Tlv::kDelayTimer)
        {
            uint32_t      elapsed = TimerMilli::GetNow() - mUpdateTime;
            DelayTimerTlv delayTimer(static_cast<const DelayTimerTlv &>(*cur));

            if (delayTimer.GetDelayTimer() > elapsed)
            {
                delayTimer.SetDelayTimer(delayTimer.GetDelayTimer() - elapsed);
            }
            else
            {
                delayTimer.SetDelayTimer(0);
            }

            SuccessOrExit(error = delayTimer.AppendTo(aMessage));
        }
        else
        {
            SuccessOrExit(error = cur->AppendTo(aMessage));
        }
    }

exit:
    return error;
}

void Dataset::RemoveTlv(Tlv *aTlv)
{
    uint8_t *start  = reinterpret_cast<uint8_t *>(aTlv);
    uint16_t length = sizeof(Tlv) + aTlv->GetLength();

    memmove(start, start + length, mLength - (static_cast<uint8_t>(start - mTlvs) + length));
    mLength -= length;
}

Error Dataset::ApplyConfiguration(Instance &aInstance, bool *aIsNetworkKeyUpdated) const
{
    Mac::Mac &  mac        = aInstance.Get<Mac::Mac>();
    KeyManager &keyManager = aInstance.Get<KeyManager>();
    Error       error      = kErrorNone;

    VerifyOrExit(IsValid(), error = kErrorParse);

    if (aIsNetworkKeyUpdated)
    {
        *aIsNetworkKeyUpdated = false;
    }

    for (const Tlv *cur = GetTlvsStart(); cur < GetTlvsEnd(); cur = cur->GetNext())
    {
        switch (cur->GetType())
        {
        case Tlv::kChannel:
        {
            uint8_t channel = static_cast<uint8_t>(static_cast<const ChannelTlv *>(cur)->GetChannel());

            error = mac.SetPanChannel(channel);

            if (error != kErrorNone)
            {
                otLogWarnMeshCoP("DatasetManager::ApplyConfiguration() Failed to set channel to %d (%s)", channel,
                                 ErrorToString(error));
                ExitNow();
            }

            break;
        }

        case Tlv::kPanId:
            mac.SetPanId(static_cast<const PanIdTlv *>(cur)->GetPanId());
            break;

        case Tlv::kExtendedPanId:
            mac.SetExtendedPanId(static_cast<const ExtendedPanIdTlv *>(cur)->GetExtendedPanId());
            break;

        case Tlv::kNetworkName:
            IgnoreError(mac.SetNetworkName(static_cast<const NetworkNameTlv *>(cur)->GetNetworkName()));
            break;

        case Tlv::kNetworkKey:
        {
            const NetworkKeyTlv *key = static_cast<const NetworkKeyTlv *>(cur);
            NetworkKey           networkKey;

            keyManager.GetNetworkKey(networkKey);

            if (aIsNetworkKeyUpdated && (key->GetNetworkKey() != networkKey))
            {
                *aIsNetworkKeyUpdated = true;
            }

            keyManager.SetNetworkKey(key->GetNetworkKey());
            break;
        }

#if OPENTHREAD_FTD

        case Tlv::kPskc:
            keyManager.SetPskc(static_cast<const PskcTlv *>(cur)->GetPskc());
            break;

#endif

        case Tlv::kMeshLocalPrefix:
            aInstance.Get<Mle::MleRouter>().SetMeshLocalPrefix(
                static_cast<const MeshLocalPrefixTlv *>(cur)->GetMeshLocalPrefix());
            break;

        case Tlv::kSecurityPolicy:
        {
            const SecurityPolicyTlv *securityPolicy = static_cast<const SecurityPolicyTlv *>(cur);
            keyManager.SetSecurityPolicy(securityPolicy->GetSecurityPolicy());
            break;
        }

        default:
            break;
        }
    }

exit:
    return error;
}

void Dataset::ConvertToActive(void)
{
    RemoveTlv(Tlv::kPendingTimestamp);
    RemoveTlv(Tlv::kDelayTimer);
}

const char *Dataset::TypeToString(Type aType)
{
    return (aType == kActive) ? "Active" : "Pending";
}

} // namespace MeshCoP
} // namespace ot
