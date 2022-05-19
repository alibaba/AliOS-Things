/*
 *  Copyright (c) 2018, The OpenThread Authors.
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
 *   This file includes definitions for non-volatile storage of settings.
 */

#include "settings.hpp"

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "meshcop/dataset.hpp"
#include "thread/mle.hpp"

namespace ot {

//---------------------------------------------------------------------------------------------------------------------
// SettingsBase

// LCOV_EXCL_START

#if OPENTHREAD_CONFIG_LOG_UTIL && (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO)

void SettingsBase::NetworkInfo::Log(Action aAction) const
{
    otLogInfoCore(
        "[settings] %s NetworkInfo {rloc:0x%04x, extaddr:%s, role:%s, mode:0x%02x, version:%hu, keyseq:0x%x, ...",
        ActionToString(aAction), GetRloc16(), GetExtAddress().ToString().AsCString(),
        Mle::Mle::RoleToString(static_cast<Mle::DeviceRole>(GetRole())), GetDeviceMode(), GetVersion(),
        GetKeySequence());

    otLogInfoCore("[settings] ... pid:0x%x, mlecntr:0x%x, maccntr:0x%x, mliid:%s}", GetPreviousPartitionId(),
                  GetMleFrameCounter(), GetMacFrameCounter(), GetMeshLocalIid().ToString().AsCString());
}

void SettingsBase::ParentInfo::Log(Action aAction) const
{
    otLogInfoCore("[settings] %s ParentInfo {extaddr:%s, version:%hu}", ActionToString(aAction),
                  GetExtAddress().ToString().AsCString(), GetVersion());
}

#if OPENTHREAD_FTD
void SettingsBase::ChildInfo::Log(Action aAction) const
{
    otLogInfoCore("[settings] %s ChildInfo {rloc:0x%04x, extaddr:%s, timeout:%u, mode:0x%02x, version:%hu}",
                  ActionToString(aAction), GetRloc16(), GetExtAddress().ToString().AsCString(), GetTimeout(), GetMode(),
                  GetVersion());
}
#endif

#if OPENTHREAD_CONFIG_DUA_ENABLE
void SettingsBase::DadInfo::Log(Action aAction) const
{
    otLogInfoCore("[settings] %s DadInfo {DadCounter:%2d}", ActionToString(aAction), GetDadCounter());
}
#endif

#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
void SettingsBase::LogPrefix(Action aAction, Key aKey, const Ip6::Prefix &aPrefix)
{
    otLogInfoCore("[settings] %s %s %s", ActionToString(aAction), KeyToString(aKey), aPrefix.ToString().AsCString());
}
#endif

#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE && OPENTHREAD_CONFIG_SRP_CLIENT_SAVE_SELECTED_SERVER_ENABLE
void SettingsBase::SrpClientInfo::Log(Action aAction) const
{
    otLogInfoCore("[settings] %s SrpClientInfo {Server:[%s]:%u}", ActionToString(aAction),
                  GetServerAddress().ToString().AsCString(), GetServerPort());
}
#endif

#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE && OPENTHREAD_CONFIG_SRP_SERVER_PORT_SWITCH_ENABLE
void SettingsBase::SrpServerInfo::Log(Action aAction) const
{
    otLogInfoCore("[settings] %s SrpServerInfo {port:%u}", ActionToString(aAction), GetPort());
}
#endif

#endif // OPENTHREAD_CONFIG_LOG_UTIL && (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO)

#if OPENTHREAD_CONFIG_LOG_UTIL && (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO)
const char *SettingsBase::ActionToString(Action aAction)
{
    static const char *const kActionStrings[] = {
        "Read",     // (0) kActionRead
        "Saved",    // (1) kActionSave
        "Re-saved", // (2) kActionResave
        "Deleted",  // (3) kActionDelete
#if OPENTHREAD_FTD
        "Added",      // (4) kActionAdd,
        "Removed",    // (5) kActionRemove,
        "Deleted all" // (6) kActionDeleteAll
#endif
    };

    static_assert(0 == kActionRead, "kActionRead value is incorrect");
    static_assert(1 == kActionSave, "kActionSave value is incorrect");
    static_assert(2 == kActionResave, "kActionResave value is incorrect");
    static_assert(3 == kActionDelete, "kActionDelete value is incorrect");
#if OPENTHREAD_FTD
    static_assert(4 == kActionAdd, "kActionAdd value is incorrect");
    static_assert(5 == kActionRemove, "kActionRemove value is incorrect");
    static_assert(6 == kActionDeleteAll, "kActionDeleteAll value is incorrect");
#endif

    return kActionStrings[aAction];
}
#endif // OPENTHREAD_CONFIG_LOG_UTIL && (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO)

#if OPENTHREAD_CONFIG_LOG_UTIL && (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_WARN)
const char *SettingsBase::KeyToString(Key aKey)
{
    static const char *const kKeyStrings[] = {
        "",                  // (0)  (Unused)
        "ActiveDataset",     // (1)  kKeyActiveDataset
        "PendingDataset",    // (2)  kKeyPendingDataset
        "NetworkInfo",       // (3)  kKeyNetworkInfo
        "ParentInfo",        // (4)  kKeyParentInfo
        "ChildInfo",         // (5)  kKeyChildInfo
        "",                  // (6)  kKeyReserved
        "SlaacIidSecretKey", // (7)  kKeySlaacIidSecretKey
        "DadInfo",           // (8)  kKeyDadInfo
        "OmrPrefix",         // (9)  kKeyOmrPrefix
        "OnLinkPrefix",      // (10) kKeyOnLinkPrefix
        "SrpEcdsaKey",       // (11) kKeySrpEcdsaKey
        "SrpClientInfo",     // (12) kKeySrpClientInfo
        "SrpServerInfo",     // (13) kKeySrpServerInfo
    };

    static_assert(1 == kKeyActiveDataset, "kKeyActiveDataset value is incorrect");
    static_assert(2 == kKeyPendingDataset, "kKeyPendingDataset value is incorrect");
    static_assert(3 == kKeyNetworkInfo, "kKeyNetworkInfo value is incorrect");
    static_assert(4 == kKeyParentInfo, "kKeyParentInfo value is incorrect");
    static_assert(5 == kKeyChildInfo, "kKeyChildInfo value is incorrect");
    static_assert(6 == kKeyReserved, "kKeyReserved value is incorrect");
    static_assert(7 == kKeySlaacIidSecretKey, "kKeySlaacIidSecretKey value is incorrect");
    static_assert(8 == kKeyDadInfo, "kKeyDadInfo value is incorrect");
    static_assert(9 == kKeyOmrPrefix, "kKeyOmrPrefix value is incorrect");
    static_assert(10 == kKeyOnLinkPrefix, "kKeyOnLinkPrefix value is incorrect");
    static_assert(11 == kKeySrpEcdsaKey, "kKeySrpEcdsaKey value is incorrect");
    static_assert(12 == kKeySrpClientInfo, "kKeySrpClientInfo value is incorrect");
    static_assert(13 == kKeySrpServerInfo, "kKeySrpServerInfo value is incorrect");

    static_assert(kLastKey == kKeySrpServerInfo, "kLastKey is not valid");

    OT_ASSERT(aKey <= kLastKey);

    return kKeyStrings[aKey];
}
#endif // OPENTHREAD_CONFIG_LOG_UTIL && (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_WARN)

// LCOV_EXCL_STOP

//---------------------------------------------------------------------------------------------------------------------
// Settings

// This array contains critical keys that should be stored in the secure area.
const uint16_t Settings::kCriticalKeys[] = {
    SettingsBase::kKeyActiveDataset,
    SettingsBase::kKeyPendingDataset,
    SettingsBase::kKeySrpEcdsaKey,
};

void Settings::Init(void)
{
    Get<SettingsDriver>().Init();
    Get<SettingsDriver>().SetCriticalKeys(kCriticalKeys, OT_ARRAY_LENGTH(kCriticalKeys));
}

void Settings::Deinit(void)
{
    Get<SettingsDriver>().Deinit();
}

void Settings::Wipe(void)
{
    Get<SettingsDriver>().Wipe();
    otLogInfoCore("[settings] Wiped all info");
}

Error Settings::SaveOperationalDataset(bool aIsActive, const MeshCoP::Dataset &aDataset)
{
    Key   key   = (aIsActive ? kKeyActiveDataset : kKeyPendingDataset);
    Error error = Get<SettingsDriver>().Set(key, aDataset.GetBytes(), aDataset.GetSize());

    Log(kActionSave, error, key);

    return error;
}

Error Settings::ReadOperationalDataset(bool aIsActive, MeshCoP::Dataset &aDataset) const
{
    Error    error  = kErrorNone;
    uint16_t length = MeshCoP::Dataset::kMaxSize;

    SuccessOrExit(error = Get<SettingsDriver>().Get(aIsActive ? kKeyActiveDataset : kKeyPendingDataset,
                                                    aDataset.GetBytes(), &length));
    VerifyOrExit(length <= MeshCoP::Dataset::kMaxSize, error = kErrorNotFound);

    aDataset.SetSize(length);

exit:
    return error;
}

Error Settings::DeleteOperationalDataset(bool aIsActive)
{
    Key   key   = (aIsActive ? kKeyActiveDataset : kKeyPendingDataset);
    Error error = Get<SettingsDriver>().Delete(key);

    Log(kActionDelete, error, key);

    return error;
}

#if OPENTHREAD_FTD
Error Settings::AddChildInfo(const ChildInfo &aChildInfo)
{
    Error error = Get<SettingsDriver>().Add(kKeyChildInfo, &aChildInfo, sizeof(aChildInfo));

    Log(kActionAdd, error, kKeyChildInfo, &aChildInfo);

    return error;
}

Error Settings::DeleteAllChildInfo(void)
{
    Error error = Get<SettingsDriver>().Delete(kKeyChildInfo);

    Log(kActionDeleteAll, error, kKeyChildInfo);

    return error;
}

Settings::ChildInfoIterator::ChildInfoIterator(Instance &aInstance)
    : SettingsBase(aInstance)
    , mIndex(0)
    , mIsDone(false)
{
    Read();
}

void Settings::ChildInfoIterator::Advance(void)
{
    if (!mIsDone)
    {
        mIndex++;
        Read();
    }
}

Error Settings::ChildInfoIterator::Delete(void)
{
    Error error = kErrorNone;

    VerifyOrExit(!mIsDone, error = kErrorInvalidState);
    SuccessOrExit(error = Get<SettingsDriver>().Delete(kKeyChildInfo, mIndex));

exit:
    Log(kActionRemove, error, kKeyChildInfo, &mChildInfo);
    return error;
}

void Settings::ChildInfoIterator::Read(void)
{
    uint16_t length = sizeof(ChildInfo);
    Error    error;

    mChildInfo.Init();
    SuccessOrExit(
        error = Get<SettingsDriver>().Get(kKeyChildInfo, mIndex, reinterpret_cast<uint8_t *>(&mChildInfo), &length));

exit:
    Log(kActionRead, error, kKeyChildInfo, &mChildInfo);
    mIsDone = (error != kErrorNone);
}
#endif // OPENTHREAD_FTD

Error Settings::ReadEntry(Key aKey, void *aValue, uint16_t aMaxLength) const
{
    Error    error;
    uint16_t length = aMaxLength;

    error = Get<SettingsDriver>().Get(aKey, aValue, &length);
    Log(kActionRead, error, aKey, aValue);

    return error;
}

Error Settings::SaveEntry(Key aKey, const void *aValue, void *aPrev, uint16_t aLength)
{
    Error    error      = kErrorNone;
    uint16_t readLength = aLength;
    Action   action     = kActionSave;

    if ((Get<SettingsDriver>().Get(aKey, aPrev, &readLength) == kErrorNone) && (readLength == aLength) &&
        (memcmp(aValue, aPrev, aLength) == 0))
    {
        action = kActionResave;
    }
    else
    {
        error = Get<SettingsDriver>().Set(aKey, aValue, aLength);
    }

    Log(action, error, aKey, aValue);

    return error;
}

Error Settings::DeleteEntry(Key aKey)
{
    Error error = Get<SettingsDriver>().Delete(aKey);

    Log(kActionDelete, error, aKey);

    return error;
}

void Settings::Log(Action aAction, Error aError, Key aKey, const void *aValue)
{
    OT_UNUSED_VARIABLE(aAction);
    OT_UNUSED_VARIABLE(aKey);
    OT_UNUSED_VARIABLE(aError);
    OT_UNUSED_VARIABLE(aValue);

#if OPENTHREAD_CONFIG_LOG_UTIL

    if (aError != kErrorNone)
    {
        // Log error if log level is at "warn" or higher.

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_WARN)
        const char *actionText = "";

        switch (aAction)
        {
        case kActionSave:
        case kActionResave:
            actionText = "saving";
            break;

        case kActionDelete:
            VerifyOrExit(aError != kErrorNotFound);
            actionText = "deleting";
            break;

#if OPENTHREAD_FTD
        case kActionAdd:
            actionText = "adding";
            break;

        case kActionRemove:
            VerifyOrExit(aError != kErrorNotFound);
            actionText = "removing";
            break;

        case kActionDeleteAll:
            VerifyOrExit(aError != kErrorNotFound);
            actionText = "deleting all";
            break;
#endif
        case kActionRead:
            ExitNow();
        }

        otLogWarnCore("[settings] Error %s %s %s", ErrorToString(aError), actionText, KeyToString(aKey));

#endif // #if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_WARN)

        ExitNow();
    }

    // We reach here when `aError` is `kErrorNone`.
    // Log success if log level is at "info" or higher.

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO)
    if (aValue != nullptr)
    {
        switch (aKey)
        {
        case kKeyNetworkInfo:
            reinterpret_cast<const NetworkInfo *>(aValue)->Log(aAction);
            break;

        case kKeyParentInfo:
            reinterpret_cast<const ParentInfo *>(aValue)->Log(aAction);
            break;

#if OPENTHREAD_FTD
        case kKeyChildInfo:
            reinterpret_cast<const ChildInfo *>(aValue)->Log(aAction);
            break;
#endif

#if OPENTHREAD_CONFIG_DUA_ENABLE
        case kKeyDadInfo:
            reinterpret_cast<const DadInfo *>(aValue)->Log(aAction);
            break;
#endif

#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
        case kKeyOmrPrefix:
        case kKeyOnLinkPrefix:
            LogPrefix(aAction, aKey, *reinterpret_cast<const Ip6::Prefix *>(aValue));
            break;
#endif

#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE && OPENTHREAD_CONFIG_SRP_CLIENT_SAVE_SELECTED_SERVER_ENABLE
        case kKeySrpClientInfo:
            reinterpret_cast<const SrpClientInfo *>(aValue)->Log(aAction);
            break;
#endif

#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE && OPENTHREAD_CONFIG_SRP_SERVER_PORT_SWITCH_ENABLE
        case kKeySrpServerInfo:
            reinterpret_cast<const SrpServerInfo *>(aValue)->Log(aAction);
            break;
#endif

        default:
            // For any other keys, we do not want to include the value
            // in the log, so even if it is given we set `aValue` to
            // `nullptr`. This ensures that we just log the action and
            // the key.
            aValue = nullptr;
            break;
        }
    }

    if (aValue == nullptr)
    {
        otLogInfoCore("[settings] %s %s", ActionToString(aAction), KeyToString(aKey));
    }
#endif // (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO)

exit:

#endif // OPENTHREAD_CONFIG_LOG_UTIL
    return;
}

} // namespace ot

//---------------------------------------------------------------------------------------------------------------------
// Default/weak implementation of settings platform APIs

OT_TOOL_WEAK void otPlatSettingsSetCriticalKeys(otInstance *aInstance, const uint16_t *aKeys, uint16_t aKeysLength)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aKeys);
    OT_UNUSED_VARIABLE(aKeysLength);
}
