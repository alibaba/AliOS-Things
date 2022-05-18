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
 *   This file implements the Notifier class.
 */

#include "notifier.hpp"

#include "border_router/routing_manager.hpp"
#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"

namespace ot {

Notifier::Notifier(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mTask(aInstance, Notifier::EmitEvents)
{
    for (ExternalCallback &callback : mExternalCallbacks)
    {
        callback.mHandler = nullptr;
        callback.mContext = nullptr;
    }
}

Error Notifier::RegisterCallback(otStateChangedCallback aCallback, void *aContext)
{
    Error             error          = kErrorNone;
    ExternalCallback *unusedCallback = nullptr;

    VerifyOrExit(aCallback != nullptr);

    for (ExternalCallback &callback : mExternalCallbacks)
    {
        if (callback.mHandler == nullptr)
        {
            if (unusedCallback == nullptr)
            {
                unusedCallback = &callback;
            }

            continue;
        }

        VerifyOrExit((callback.mHandler != aCallback) || (callback.mContext != aContext), error = kErrorAlready);
    }

    VerifyOrExit(unusedCallback != nullptr, error = kErrorNoBufs);

    unusedCallback->mHandler = aCallback;
    unusedCallback->mContext = aContext;

exit:
    return error;
}

void Notifier::RemoveCallback(otStateChangedCallback aCallback, void *aContext)
{
    VerifyOrExit(aCallback != nullptr);

    for (ExternalCallback &callback : mExternalCallbacks)
    {
        if ((callback.mHandler == aCallback) && (callback.mContext == aContext))
        {
            callback.mHandler = nullptr;
            callback.mContext = nullptr;
        }
    }

exit:
    return;
}

void Notifier::Signal(Event aEvent)
{
    mEventsToSignal.Add(aEvent);
    mSignaledEvents.Add(aEvent);
    mTask.Post();
}

void Notifier::SignalIfFirst(Event aEvent)
{
    if (!HasSignaled(aEvent))
    {
        Signal(aEvent);
    }
}

void Notifier::EmitEvents(Tasklet &aTasklet)
{
    aTasklet.Get<Notifier>().EmitEvents();
}

void Notifier::EmitEvents(void)
{
    Events events;

    VerifyOrExit(!mEventsToSignal.IsEmpty());

    // Note that the callbacks may signal new events, so we create a
    // copy of `mEventsToSignal` and then clear it.

    events = mEventsToSignal;
    mEventsToSignal.Clear();

    LogEvents(events);

    // Emit events to core internal modules

    Get<Mle::Mle>().HandleNotifierEvents(events);
    Get<EnergyScanServer>().HandleNotifierEvents(events);
#if OPENTHREAD_FTD
    Get<MeshCoP::JoinerRouter>().HandleNotifierEvents(events);
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    Get<BackboneRouter::Manager>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
    Get<Utils::ChildSupervisor>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_CONFIG_DATASET_UPDATER_ENABLE || OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE
    Get<MeshCoP::DatasetUpdater>().HandleNotifierEvents(events);
#endif
#endif // OPENTHREAD_FTD
#if OPENTHREAD_FTD || OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    Get<NetworkData::Notifier>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_CONFIG_ANNOUNCE_SENDER_ENABLE
    Get<AnnounceSender>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
    Get<MeshCoP::BorderAgent>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_CONFIG_MLR_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE)
    Get<MlrManager>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_CONFIG_DUA_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_DUA_ENABLE)
    Get<DuaManager>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    Get<TimeSync>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
    Get<Utils::Slaac>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE
    Get<Utils::JamDetector>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_CONFIG_OTNS_ENABLE
    Get<Utils::Otns>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    Get<Utils::HistoryTracker>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_ENABLE_VENDOR_EXTENSION
    Get<Extension::ExtensionBase>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
    Get<BorderRouter::RoutingManager>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
    Get<Srp::Client>().HandleNotifierEvents(events);
#endif
#if OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE
    // The `NetworkData::Publisher` is notified last (e.g., after SRP
    // client) to allow other modules to request changes to what is
    // being published (if needed).
    Get<NetworkData::Publisher>().HandleNotifierEvents(events);
#endif

    for (ExternalCallback &callback : mExternalCallbacks)
    {
        if (callback.mHandler != nullptr)
        {
            callback.mHandler(events.GetAsFlags(), callback.mContext);
        }
    }

exit:
    return;
}

// LCOV_EXCL_START

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_CORE == 1)

void Notifier::LogEvents(Events aEvents) const
{
    Events::Flags                  flags    = aEvents.GetAsFlags();
    bool                           addSpace = false;
    bool                           didLog   = false;
    String<kFlagsStringBufferSize> string;

    for (uint8_t bit = 0; bit < sizeof(Events::Flags) * CHAR_BIT; bit++)
    {
        VerifyOrExit(flags != 0);

        if (flags & (1 << bit))
        {
            if (string.GetLength() >= kFlagsStringLineLimit)
            {
                otLogInfoCore("Notifier: StateChanged (0x%08x) %s%s ...", aEvents.GetAsFlags(), didLog ? "... " : "[",
                              string.AsCString());
                string.Clear();
                didLog   = true;
                addSpace = false;
            }

            string.Append("%s%s", addSpace ? " " : "", EventToString(static_cast<Event>(1 << bit)));
            addSpace = true;

            flags ^= (1 << bit);
        }
    }

exit:
    otLogInfoCore("Notifier: StateChanged (0x%08x) %s%s]", aEvents.GetAsFlags(), didLog ? "... " : "[",
                  string.AsCString());
}

const char *Notifier::EventToString(Event aEvent) const
{
    const char *retval = "(unknown)";

    // To ensure no clipping of flag names in the logs, the returned
    // strings from this method should have shorter length than
    // `kMaxFlagNameLength` value.
    static const char *const kEventStrings[] = {
        "Ip6+",              // kEventIp6AddressAdded                  (1 << 0)
        "Ip6-",              // kEventIp6AddressRemoved                (1 << 1)
        "Role",              // kEventThreadRoleChanged                (1 << 2)
        "LLAddr",            // kEventThreadLinkLocalAddrChanged       (1 << 3)
        "MLAddr",            // kEventThreadMeshLocalAddrChanged       (1 << 4)
        "Rloc+",             // kEventThreadRlocAdded                  (1 << 5)
        "Rloc-",             // kEventThreadRlocRemoved                (1 << 6)
        "PartitionId",       // kEventThreadPartitionIdChanged         (1 << 7)
        "KeySeqCntr",        // kEventThreadKeySeqCounterChanged       (1 << 8)
        "NetData",           // kEventThreadNetdataChanged             (1 << 9)
        "Child+",            // kEventThreadChildAdded                 (1 << 10)
        "Child-",            // kEventThreadChildRemoved               (1 << 11)
        "Ip6Mult+",          // kEventIp6MulticastSubscribed           (1 << 12)
        "Ip6Mult-",          // kEventIp6MulticastUnsubscribed         (1 << 13)
        "Channel",           // kEventThreadChannelChanged             (1 << 14)
        "PanId",             // kEventThreadPanIdChanged               (1 << 15)
        "NetName",           // kEventThreadNetworkNameChanged         (1 << 16)
        "ExtPanId",          // kEventThreadExtPanIdChanged            (1 << 17)
        "NetworkKey",        // kEventNetworkKeyChanged                (1 << 18)
        "PSKc",              // kEventPskcChanged                      (1 << 19)
        "SecPolicy",         // kEventSecurityPolicyChanged            (1 << 20)
        "CMNewChan",         // kEventChannelManagerNewChannelChanged  (1 << 21)
        "ChanMask",          // kEventSupportedChannelMaskChanged      (1 << 22)
        "CommissionerState", // kEventCommissionerStateChanged         (1 << 23)
        "NetifState",        // kEventThreadNetifStateChanged          (1 << 24)
        "BbrState",          // kEventThreadBackboneRouterStateChanged (1 << 25)
        "BbrLocal",          // kEventThreadBackboneRouterLocalChanged (1 << 26)
        "JoinerState",       // kEventJoinerStateChanged               (1 << 27)
        "ActDset",           // kEventActiveDatasetChanged             (1 << 28)
        "PndDset",           // kEventPendingDatasetChanged            (1 << 29)
    };

    for (uint8_t index = 0; index < OT_ARRAY_LENGTH(kEventStrings); index++)
    {
        if (static_cast<uint32_t>(aEvent) == (1U << index))
        {
            retval = kEventStrings[index];
            break;
        }
    }

    return retval;
}

#else // #if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_CORE == 1)

void Notifier::LogEvents(Events) const
{
}

const char *Notifier::EventToString(Event) const
{
    return "";
}

#endif // #if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_CORE == 1)

// LCOV_EXCL_STOP

} // namespace ot
