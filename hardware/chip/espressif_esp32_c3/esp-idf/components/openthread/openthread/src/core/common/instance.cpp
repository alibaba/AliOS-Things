/*
 *  Copyright (c) 2016-2017, The OpenThread Authors.
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
 *   This file implements the OpenThread Instance class.
 */

#include "instance.hpp"

#include <openthread/platform/misc.h>

#include "common/logging.hpp"
#include "common/new.hpp"
#include "radio/trel_link.hpp"
#include "utils/heap.hpp"

namespace ot {

#if !OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE

// Define the raw storage used for OpenThread instance (in single-instance case).
OT_DEFINE_ALIGNED_VAR(gInstanceRaw, sizeof(Instance), uint64_t);

#endif

#if OPENTHREAD_MTD || OPENTHREAD_FTD
#if !OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE
Utils::Heap Instance::sHeap;
#endif
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
bool Instance::sDnsNameCompressionEnabled = true;
#endif
#endif

#if OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
otLogLevel Instance::sLogLevel = static_cast<otLogLevel>(OPENTHREAD_CONFIG_LOG_LEVEL_INIT);
#endif

Instance::Instance(void)
    : mTimerMilliScheduler(*this)
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    , mTimerMicroScheduler(*this)
#endif
    , mRadio(*this)
#if OPENTHREAD_CONFIG_UPTIME_ENABLE
    , mUptime(*this)
#endif
#if OPENTHREAD_MTD || OPENTHREAD_FTD
    , mNotifier(*this)
    , mTimeTicker(*this)
    , mSettings(*this)
    , mSettingsDriver(*this)
    , mMessagePool(*this)
    , mIp6(*this)
    , mThreadNetif(*this)
#if OPENTHREAD_CONFIG_COAP_API_ENABLE
    , mApplicationCoap(*this)
#endif
#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    , mApplicationCoapSecure(*this, /* aLayerTwoSecurity */ true)
#endif
#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE
    , mPingSender(*this)
#endif
#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
    , mChannelMonitor(*this)
#endif
#if OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE && OPENTHREAD_FTD
    , mChannelManager(*this)
#endif
#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    , mHistoryTracker(*this)
#endif
#if (OPENTHREAD_CONFIG_DATASET_UPDATER_ENABLE || OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE) && OPENTHREAD_FTD
    , mDatasetUpdater(*this)
#endif
#if OPENTHREAD_CONFIG_ANNOUNCE_SENDER_ENABLE
    , mAnnounceSender(*this)
#endif
#if OPENTHREAD_CONFIG_OTNS_ENABLE
    , mOtns(*this)
#endif
#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
    , mRoutingManager(*this)
#endif
#endif // OPENTHREAD_MTD || OPENTHREAD_FTD
#if OPENTHREAD_RADIO || OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    , mLinkRaw(*this)
#endif
#if OPENTHREAD_ENABLE_VENDOR_EXTENSION
    , mExtension(Extension::ExtensionBase::Init(*this))
#endif
#if OPENTHREAD_CONFIG_DIAG_ENABLE
    , mDiags(*this)
#endif
    , mIsInitialized(false)
{
}

#if !OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE

Instance &Instance::InitSingle(void)
{
    Instance *instance = &Get();

    VerifyOrExit(!instance->mIsInitialized);

    instance = new (&gInstanceRaw) Instance();

    instance->AfterInit();

exit:
    return *instance;
}

Instance &Instance::Get(void)
{
    void *instance = &gInstanceRaw;

    return *static_cast<Instance *>(instance);
}

#else // #if !OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE

Instance *Instance::Init(void *aBuffer, size_t *aBufferSize)
{
    Instance *instance = nullptr;

    VerifyOrExit(aBufferSize != nullptr);

    // Make sure the input buffer is big enough
    VerifyOrExit(sizeof(Instance) <= *aBufferSize, *aBufferSize = sizeof(Instance));

    VerifyOrExit(aBuffer != nullptr);

    instance = new (aBuffer) Instance();

    instance->AfterInit();

exit:
    return instance;
}

#endif // OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE

void Instance::Reset(void)
{
    otPlatReset(this);
}

#if OPENTHREAD_RADIO
void Instance::ResetRadioStack(void)
{
    mRadio.Init();
    mLinkRaw.Init();
}
#endif

void Instance::AfterInit(void)
{
    mIsInitialized = true;
#if OPENTHREAD_MTD || OPENTHREAD_FTD

    // Restore datasets and network information

    Get<Settings>().Init();
    IgnoreError(Get<Mle::MleRouter>().Restore());

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    Get<Trel::Link>().AfterInit();
#endif

#endif // OPENTHREAD_MTD || OPENTHREAD_FTD

#if OPENTHREAD_ENABLE_VENDOR_EXTENSION
    Get<Extension::ExtensionBase>().SignalInstanceInit();
#endif
}

void Instance::Finalize(void)
{
    VerifyOrExit(mIsInitialized);

    mIsInitialized = false;

#if OPENTHREAD_MTD || OPENTHREAD_FTD
    IgnoreError(otThreadSetEnabled(this, false));
    IgnoreError(otIp6SetEnabled(this, false));
    IgnoreError(otLinkSetEnabled(this, false));

    Get<Settings>().Deinit();
#endif

    IgnoreError(Get<Mac::SubMac>().Disable());

#if !OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE

    /**
     * Object was created on buffer, so instead of deleting
     * the object we call destructor explicitly.
     */
    this->~Instance();

#endif // !OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE

exit:
    return;
}

#if OPENTHREAD_MTD || OPENTHREAD_FTD
void Instance::FactoryReset(void)
{
    Get<Settings>().Wipe();
    otPlatReset(this);
}

Error Instance::ErasePersistentInfo(void)
{
    Error error = kErrorNone;

    VerifyOrExit(Get<Mle::MleRouter>().IsDisabled(), error = kErrorInvalidState);
    Get<Settings>().Wipe();

exit:
    return error;
}

#endif // OPENTHREAD_MTD || OPENTHREAD_FTD

} // namespace ot
