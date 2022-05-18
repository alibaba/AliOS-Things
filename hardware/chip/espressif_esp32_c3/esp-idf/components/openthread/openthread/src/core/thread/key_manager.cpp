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
 *   This file implements Thread security material generation.
 */

#include "key_manager.hpp"

#include "common/code_utils.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/timer.hpp"
#include "crypto/hkdf_sha256.hpp"
#include "crypto/storage.hpp"
#include "thread/mle_router.hpp"
#include "thread/thread_netif.hpp"

namespace ot {

const uint8_t KeyManager::kThreadString[] = {
    'T', 'h', 'r', 'e', 'a', 'd',
};

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
const uint8_t KeyManager::kHkdfExtractSaltString[] = {'T', 'h', 'r', 'e', 'a', 'd', 'S', 'e', 'q', 'u', 'e', 'n',
                                                      'c', 'e', 'M', 'a', 's', 't', 'e', 'r', 'K', 'e', 'y'};

const uint8_t KeyManager::kTrelInfoString[] = {'T', 'h', 'r', 'e', 'a', 'd', 'O', 'v', 'e',
                                               'r', 'I', 'n', 'f', 'r', 'a', 'K', 'e', 'y'};
#endif

void SecurityPolicy::SetToDefault(void)
{
    mRotationTime = kDefaultKeyRotationTime;
    SetToDefaultFlags();
}

void SecurityPolicy::SetToDefaultFlags(void)
{
    mObtainNetworkKeyEnabled        = true;
    mNativeCommissioningEnabled     = true;
    mRoutersEnabled                 = true;
    mExternalCommissioningEnabled   = true;
    mBeaconsEnabled                 = true;
    mCommercialCommissioningEnabled = false;
    mAutonomousEnrollmentEnabled    = false;
    mNetworkKeyProvisioningEnabled  = false;
    mTobleLinkEnabled               = true;
    mNonCcmRoutersEnabled           = false;
    mVersionThresholdForRouting     = 0;
}

void SecurityPolicy::SetFlags(const uint8_t *aFlags, uint8_t aFlagsLength)
{
    OT_ASSERT(aFlagsLength > 0);

    SetToDefaultFlags();

    mObtainNetworkKeyEnabled        = aFlags[0] & kObtainNetworkKeyMask;
    mNativeCommissioningEnabled     = aFlags[0] & kNativeCommissioningMask;
    mRoutersEnabled                 = aFlags[0] & kRoutersMask;
    mExternalCommissioningEnabled   = aFlags[0] & kExternalCommissioningMask;
    mBeaconsEnabled                 = aFlags[0] & kBeaconsMask;
    mCommercialCommissioningEnabled = (aFlags[0] & kCommercialCommissioningMask) == 0;
    mAutonomousEnrollmentEnabled    = (aFlags[0] & kAutonomousEnrollmentMask) == 0;
    mNetworkKeyProvisioningEnabled  = (aFlags[0] & kNetworkKeyProvisioningMask) == 0;

    VerifyOrExit(aFlagsLength > sizeof(aFlags[0]));
    mTobleLinkEnabled           = aFlags[1] & kTobleLinkMask;
    mNonCcmRoutersEnabled       = (aFlags[1] & kNonCcmRoutersMask) == 0;
    mVersionThresholdForRouting = aFlags[1] & kVersionThresholdForRoutingMask;

exit:
    return;
}

void SecurityPolicy::GetFlags(uint8_t *aFlags, uint8_t aFlagsLength) const
{
    OT_ASSERT(aFlagsLength > 0);

    memset(aFlags, 0, aFlagsLength);

    if (mObtainNetworkKeyEnabled)
    {
        aFlags[0] |= kObtainNetworkKeyMask;
    }

    if (mNativeCommissioningEnabled)
    {
        aFlags[0] |= kNativeCommissioningMask;
    }

    if (mRoutersEnabled)
    {
        aFlags[0] |= kRoutersMask;
    }

    if (mExternalCommissioningEnabled)
    {
        aFlags[0] |= kExternalCommissioningMask;
    }

    if (mBeaconsEnabled)
    {
        aFlags[0] |= kBeaconsMask;
    }

    if (!mCommercialCommissioningEnabled)
    {
        aFlags[0] |= kCommercialCommissioningMask;
    }

    if (!mAutonomousEnrollmentEnabled)
    {
        aFlags[0] |= kAutonomousEnrollmentMask;
    }

    if (!mNetworkKeyProvisioningEnabled)
    {
        aFlags[0] |= kNetworkKeyProvisioningMask;
    }

    VerifyOrExit(aFlagsLength > sizeof(aFlags[0]));

    if (mTobleLinkEnabled)
    {
        aFlags[1] |= kTobleLinkMask;
    }

    if (!mNonCcmRoutersEnabled)
    {
        aFlags[1] |= kNonCcmRoutersMask;
    }

    aFlags[1] |= kReservedMask;
    aFlags[1] |= mVersionThresholdForRouting;

exit:
    return;
}

KeyManager::KeyManager(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mKeySequence(0)
    , mMleFrameCounter(0)
    , mStoredMacFrameCounter(0)
    , mStoredMleFrameCounter(0)
    , mHoursSinceKeyRotation(0)
    , mKeySwitchGuardTime(kDefaultKeySwitchGuardTime)
    , mKeySwitchGuardEnabled(false)
    , mKeyRotationTimer(aInstance, KeyManager::HandleKeyRotationTimer)
    , mKekFrameCounter(0)
    , mIsPskcSet(false)
{
    IgnoreError(otPlatCryptoInit());

#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    {
        NetworkKey networkKey;

        mNetworkKeyRef = Crypto::Storage::kInvalidKeyRef;
        mPskcRef       = Crypto::Storage::kInvalidKeyRef;

        IgnoreError(networkKey.GenerateRandom());
        StoreNetworkKey(networkKey, /* aOverWriteExisting */ false);
    }
#else
    IgnoreError(mNetworkKey.GenerateRandom());
    mPskc.Clear();
#endif

    mMacFrameCounters.Reset();
}

void KeyManager::Start(void)
{
    mKeySwitchGuardEnabled = false;
    StartKeyRotationTimer();
}

void KeyManager::Stop(void)
{
    mKeyRotationTimer.Stop();
}

void KeyManager::SetPskc(const Pskc &aPskc)
{
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    if (Crypto::Storage::IsKeyRefValid(mPskcRef))
    {
        Pskc pskc;

        GetPskc(pskc);
        VerifyOrExit(aPskc != pskc, Get<Notifier>().SignalIfFirst(kEventPskcChanged));
    }

    StorePskc(aPskc);
    Get<Notifier>().Signal(kEventPskcChanged);
#else
    SuccessOrExit(Get<Notifier>().Update(mPskc, aPskc, kEventPskcChanged));
#endif

exit:
    mIsPskcSet = true;
}

void KeyManager::ResetFrameCounters(void)
{
    Router *parent;

    // reset parent frame counters
    parent = &Get<Mle::MleRouter>().GetParent();
    parent->SetKeySequence(0);
    parent->GetLinkFrameCounters().Reset();
    parent->SetLinkAckFrameCounter(0);
    parent->SetMleFrameCounter(0);

#if OPENTHREAD_FTD
    // reset router frame counters
    for (Router &router : Get<RouterTable>().Iterate())
    {
        router.SetKeySequence(0);
        router.GetLinkFrameCounters().Reset();
        router.SetLinkAckFrameCounter(0);
        router.SetMleFrameCounter(0);
    }

    // reset child frame counters
    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateAnyExceptInvalid))
    {
        child.SetKeySequence(0);
        child.GetLinkFrameCounters().Reset();
        child.SetLinkAckFrameCounter(0);
        child.SetMleFrameCounter(0);
    }
#endif
}

void KeyManager::SetNetworkKey(const NetworkKey &aNetworkKey)
{
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    if (Crypto::Storage::IsKeyRefValid(mNetworkKeyRef))
    {
        NetworkKey networkKey;

        GetNetworkKey(networkKey);
        VerifyOrExit(networkKey != aNetworkKey, Get<Notifier>().SignalIfFirst(kEventNetworkKeyChanged));
    }

    StoreNetworkKey(aNetworkKey, /* aOverWriteExisting */ true);
    Get<Notifier>().Signal(kEventNetworkKeyChanged);
#else
    SuccessOrExit(Get<Notifier>().Update(mNetworkKey, aNetworkKey, kEventNetworkKeyChanged));
#endif

    Get<Notifier>().Signal(kEventThreadKeySeqCounterChanged);

    mKeySequence = 0;
    UpdateKeyMaterial();
    ResetFrameCounters();

exit:
    return;
}

void KeyManager::ComputeKeys(uint32_t aKeySequence, HashKeys &aHashKeys)
{
    Crypto::HmacSha256 hmac;
    uint8_t            keySequenceBytes[sizeof(uint32_t)];
    Crypto::Key        cryptoKey;

#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    cryptoKey.SetAsKeyRef(mNetworkKeyRef);
#else
    cryptoKey.Set(mNetworkKey.m8, NetworkKey::kSize);
#endif

    hmac.Start(cryptoKey);

    Encoding::BigEndian::WriteUint32(aKeySequence, keySequenceBytes);
    hmac.Update(keySequenceBytes);
    hmac.Update(kThreadString);

    hmac.Finish(aHashKeys.mHash);
}

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
void KeyManager::ComputeTrelKey(uint32_t aKeySequence, Mac::Key &aKey)
{
    Crypto::HkdfSha256 hkdf;
    uint8_t            salt[sizeof(uint32_t) + sizeof(kHkdfExtractSaltString)];
    Crypto::Key        cryptoKey;

#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    cryptoKey.SetAsKeyRef(mNetworkKeyRef);
#else
    cryptoKey.Set(mNetworkKey.m8, NetworkKey::kSize);
#endif

    Encoding::BigEndian::WriteUint32(aKeySequence, salt);
    memcpy(salt + sizeof(uint32_t), kHkdfExtractSaltString, sizeof(kHkdfExtractSaltString));

    hkdf.Extract(salt, sizeof(salt), cryptoKey);
    hkdf.Expand(kTrelInfoString, sizeof(kTrelInfoString), aKey.m8, Mac::Key::kSize);
}
#endif

void KeyManager::UpdateKeyMaterial(void)
{
    HashKeys hashKeys;

    ComputeKeys(mKeySequence, hashKeys);

    mMleKey.SetFrom(hashKeys.GetMleKey());

#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
    {
        Mac::KeyMaterial curKey;
        Mac::KeyMaterial prevKey;
        Mac::KeyMaterial nextKey;

        curKey.SetFrom(hashKeys.GetMacKey());

        ComputeKeys(mKeySequence - 1, hashKeys);
        prevKey.SetFrom(hashKeys.GetMacKey());

        ComputeKeys(mKeySequence + 1, hashKeys);
        nextKey.SetFrom(hashKeys.GetMacKey());

        Get<Mac::SubMac>().SetMacKey(Mac::Frame::kKeyIdMode1, (mKeySequence & 0x7f) + 1, prevKey, curKey, nextKey);
    }
#endif

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    {
        Mac::Key key;

        ComputeTrelKey(mKeySequence, key);
        mTrelKey.SetFrom(key);
    }
#endif
}

void KeyManager::SetCurrentKeySequence(uint32_t aKeySequence)
{
    VerifyOrExit(aKeySequence != mKeySequence, Get<Notifier>().SignalIfFirst(kEventThreadKeySeqCounterChanged));

    if ((aKeySequence == (mKeySequence + 1)) && mKeyRotationTimer.IsRunning())
    {
        if (mKeySwitchGuardEnabled)
        {
            // Check if the guard timer has expired if key rotation is requested.
            VerifyOrExit(mHoursSinceKeyRotation >= mKeySwitchGuardTime);
            StartKeyRotationTimer();
        }

        mKeySwitchGuardEnabled = true;
    }

    mKeySequence = aKeySequence;
    UpdateKeyMaterial();

    SetAllMacFrameCounters(0);
    mMleFrameCounter = 0;

    Get<Notifier>().Signal(kEventThreadKeySeqCounterChanged);

exit:
    return;
}

const Mle::KeyMaterial &KeyManager::GetTemporaryMleKey(uint32_t aKeySequence)
{
    HashKeys hashKeys;

    ComputeKeys(aKeySequence, hashKeys);
    mTemporaryMleKey.SetFrom(hashKeys.GetMleKey());

    return mTemporaryMleKey;
}

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
const Mac::KeyMaterial &KeyManager::GetTemporaryTrelMacKey(uint32_t aKeySequence)
{
    Mac::Key key;

    ComputeTrelKey(aKeySequence, key);
    mTemporaryTrelKey.SetFrom(key);

    return mTemporaryTrelKey;
}
#endif

void KeyManager::SetAllMacFrameCounters(uint32_t aMacFrameCounter)
{
    mMacFrameCounters.SetAll(aMacFrameCounter);

#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
    Get<Mac::SubMac>().SetFrameCounter(aMacFrameCounter);
#endif
}

#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
void KeyManager::MacFrameCounterUsed(uint32_t aMacFrameCounter)
{
    // This is callback from `SubMac` to indicate that a frame
    // counter value is used for tx. We ensure to handle it
    // even if it is called out of order.

    VerifyOrExit(mMacFrameCounters.Get154() <= aMacFrameCounter);

    mMacFrameCounters.Set154(aMacFrameCounter + 1);

    if (mMacFrameCounters.Get154() >= mStoredMacFrameCounter)
    {
        IgnoreError(Get<Mle::MleRouter>().Store());
    }

exit:
    return;
}
#else
void KeyManager::MacFrameCounterUsed(uint32_t)
{
}
#endif

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
void KeyManager::IncrementTrelMacFrameCounter(void)
{
    mMacFrameCounters.IncrementTrel();

    if (mMacFrameCounters.GetTrel() >= mStoredMacFrameCounter)
    {
        IgnoreError(Get<Mle::MleRouter>().Store());
    }
}
#endif

void KeyManager::IncrementMleFrameCounter(void)
{
    mMleFrameCounter++;

    if (mMleFrameCounter >= mStoredMleFrameCounter)
    {
        IgnoreError(Get<Mle::MleRouter>().Store());
    }
}

void KeyManager::SetKek(const Kek &aKek)
{
    mKek.SetFrom(aKek, /* aIsExportable */ true);
    mKekFrameCounter = 0;
}

void KeyManager::SetSecurityPolicy(const SecurityPolicy &aSecurityPolicy)
{
    OT_ASSERT(aSecurityPolicy.mRotationTime >= SecurityPolicy::kMinKeyRotationTime);

    IgnoreError(Get<Notifier>().Update(mSecurityPolicy, aSecurityPolicy, kEventSecurityPolicyChanged));
}

void KeyManager::StartKeyRotationTimer(void)
{
    mHoursSinceKeyRotation = 0;
    mKeyRotationTimer.Start(kOneHourIntervalInMsec);
}

void KeyManager::HandleKeyRotationTimer(Timer &aTimer)
{
    aTimer.Get<KeyManager>().HandleKeyRotationTimer();
}

void KeyManager::HandleKeyRotationTimer(void)
{
    mHoursSinceKeyRotation++;

    // Order of operations below is important. We should restart the timer (from
    // last fire time for one hour interval) before potentially calling
    // `SetCurrentKeySequence()`. `SetCurrentKeySequence()` uses the fact that
    // timer is running to decide to check for the guard time and to reset the
    // rotation timer (and the `mHoursSinceKeyRotation`) if it updates the key
    // sequence.

    mKeyRotationTimer.StartAt(mKeyRotationTimer.GetFireTime(), kOneHourIntervalInMsec);

    if (mHoursSinceKeyRotation >= mSecurityPolicy.mRotationTime)
    {
        SetCurrentKeySequence(mKeySequence + 1);
    }
}

void KeyManager::GetNetworkKey(NetworkKey &aNetworkKey) const
{
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    if (Crypto::Storage::IsKeyRefValid(mNetworkKeyRef))
    {
        Error  error = kErrorNone;
        size_t keyLen;

        error = Crypto::Storage::ExportKey(mNetworkKeyRef, aNetworkKey.m8, NetworkKey::kSize, keyLen);
        OT_ASSERT(error == kErrorNone);
        OT_ASSERT(keyLen == NetworkKey::kSize);
        OT_UNUSED_VARIABLE(error);
    }
    else
    {
        aNetworkKey.Clear();
    }
#else
    aNetworkKey = mNetworkKey;
#endif
}

void KeyManager::GetPskc(Pskc &aPskc) const
{
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    if (Crypto::Storage::IsKeyRefValid(mPskcRef))
    {
        Error  error = kErrorNone;
        size_t keyLen;

        error = Crypto::Storage::ExportKey(mPskcRef, aPskc.m8, Pskc::kSize, keyLen);
        OT_ASSERT(error == kErrorNone);
        OT_ASSERT(keyLen == Pskc::kSize);
        OT_UNUSED_VARIABLE(error);
    }
    else
    {
        aPskc.Clear();
    }
#else
    aPskc = mPskc;
#endif
}

#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE

void KeyManager::StoreNetworkKey(const NetworkKey &aNetworkKey, bool aOverWriteExisting)
{
    Error         error;
    NetworkKeyRef keyRef;

    keyRef = kNetworkKeyPsaItsOffset;

    if (!aOverWriteExisting)
    {
        // Check if there is already a network key stored in ITS. If
        // stored, and we are not overwriting the existing key,
        // return without doing anything.
        if (Crypto::Storage::HasKey(keyRef))
        {
            ExitNow();
        }
    }

    Crypto::Storage::DestroyKey(keyRef);

    error = Crypto::Storage::ImportKey(keyRef, Crypto::Storage::kKeyTypeHmac, Crypto::Storage::kKeyAlgorithmHmacSha256,
                                       Crypto::Storage::kUsageSignHash | Crypto::Storage::kUsageExport,
                                       Crypto::Storage::kTypePersistent, aNetworkKey.m8, NetworkKey::kSize);
    OT_ASSERT(error == kErrorNone);

exit:
    if (mNetworkKeyRef != keyRef)
    {
        Crypto::Storage::DestroyKey(mNetworkKeyRef);
    }

    mNetworkKeyRef = keyRef;
}

void KeyManager::StorePskc(const Pskc &aPskc)
{
    PskcRef keyRef = kPskcPsaItsOffset;
    Error   error  = kErrorNone;

    Crypto::Storage::DestroyKey(keyRef);

    error = Crypto::Storage::ImportKey(keyRef, Crypto::Storage::kKeyTypeRaw, Crypto::Storage::kKeyAlgorithmVendor,
                                       Crypto::Storage::kUsageExport, Crypto::Storage::kTypePersistent, aPskc.m8,
                                       Pskc::kSize);
    OT_ASSERT(error == kErrorNone);
    OT_UNUSED_VARIABLE(error);

    if (mPskcRef != keyRef)
    {
        Crypto::Storage::DestroyKey(mPskcRef);
    }

    mPskcRef = keyRef;
}

void KeyManager::SetPskcRef(PskcRef aKeyRef)
{
    VerifyOrExit(mPskcRef != aKeyRef, Get<Notifier>().SignalIfFirst(kEventPskcChanged));

    Crypto::Storage::DestroyKey(mPskcRef);

    mPskcRef = aKeyRef;
    Get<Notifier>().Signal(kEventPskcChanged);

exit:
    mIsPskcSet = true;
}

void KeyManager::SetNetworkKeyRef(otNetworkKeyRef aKeyRef)
{
    VerifyOrExit(mNetworkKeyRef != aKeyRef, Get<Notifier>().SignalIfFirst(kEventNetworkKeyChanged));

    Crypto::Storage::DestroyKey(mNetworkKeyRef);

    mNetworkKeyRef = aKeyRef;
    Get<Notifier>().Signal(kEventNetworkKeyChanged);
    Get<Notifier>().Signal(kEventThreadKeySeqCounterChanged);
    mKeySequence = 0;
    UpdateKeyMaterial();
    ResetFrameCounters();

exit:
    return;
}

#endif // OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE

} // namespace ot
