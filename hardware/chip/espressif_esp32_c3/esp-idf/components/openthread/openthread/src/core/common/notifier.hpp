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
 *  This file defines OpenThread Notifier class.
 */

#ifndef NOTIFIER_HPP_
#define NOTIFIER_HPP_

#include "openthread-core-config.h"

#include <stdbool.h>
#include <stdint.h>

#include <openthread/instance.h>
#include <openthread/platform/toolchain.h>

#include "common/error.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/tasklet.hpp"

namespace ot {

/**
 * @addtogroup core-notifier
 *
 * @brief
 *   This module includes definitions for OpenThread Notifier class.
 *
 * @{
 *
 */

/**
 * This enumeration type represents events emitted from OpenThread Notifier.
 *
 */
enum Event : uint32_t
{
    kEventIp6AddressAdded                  = OT_CHANGED_IP6_ADDRESS_ADDED,            ///< IPv6 address was added
    kEventIp6AddressRemoved                = OT_CHANGED_IP6_ADDRESS_REMOVED,          ///< IPv6 address was removed
    kEventThreadRoleChanged                = OT_CHANGED_THREAD_ROLE,                  ///< Role changed
    kEventThreadLinkLocalAddrChanged       = OT_CHANGED_THREAD_LL_ADDR,               ///< Link-local address changed
    kEventThreadMeshLocalAddrChanged       = OT_CHANGED_THREAD_ML_ADDR,               ///< Mesh-local address changed
    kEventThreadRlocAdded                  = OT_CHANGED_THREAD_RLOC_ADDED,            ///< RLOC was added
    kEventThreadRlocRemoved                = OT_CHANGED_THREAD_RLOC_REMOVED,          ///< RLOC was removed
    kEventThreadPartitionIdChanged         = OT_CHANGED_THREAD_PARTITION_ID,          ///< Partition ID changed
    kEventThreadKeySeqCounterChanged       = OT_CHANGED_THREAD_KEY_SEQUENCE_COUNTER,  ///< Key Sequence changed
    kEventThreadNetdataChanged             = OT_CHANGED_THREAD_NETDATA,               ///< Network Data changed
    kEventThreadChildAdded                 = OT_CHANGED_THREAD_CHILD_ADDED,           ///< Child was added
    kEventThreadChildRemoved               = OT_CHANGED_THREAD_CHILD_REMOVED,         ///< Child was removed
    kEventIp6MulticastSubscribed           = OT_CHANGED_IP6_MULTICAST_SUBSCRIBED,     ///< Multicast address added
    kEventIp6MulticastUnsubscribed         = OT_CHANGED_IP6_MULTICAST_UNSUBSCRIBED,   ///< Multicast address removed
    kEventThreadChannelChanged             = OT_CHANGED_THREAD_CHANNEL,               ///< Network channel changed
    kEventThreadPanIdChanged               = OT_CHANGED_THREAD_PANID,                 ///< Network PAN ID changed
    kEventThreadNetworkNameChanged         = OT_CHANGED_THREAD_NETWORK_NAME,          ///< Network name changed
    kEventThreadExtPanIdChanged            = OT_CHANGED_THREAD_EXT_PANID,             ///< Extended PAN ID changed
    kEventNetworkKeyChanged                = OT_CHANGED_NETWORK_KEY,                  ///< Network Key changed
    kEventPskcChanged                      = OT_CHANGED_PSKC,                         ///< PSKc changed
    kEventSecurityPolicyChanged            = OT_CHANGED_SECURITY_POLICY,              ///< Security Policy changed
    kEventChannelManagerNewChannelChanged  = OT_CHANGED_CHANNEL_MANAGER_NEW_CHANNEL,  ///< New Channel (channel-manager)
    kEventSupportedChannelMaskChanged      = OT_CHANGED_SUPPORTED_CHANNEL_MASK,       ///< Channel mask changed
    kEventCommissionerStateChanged         = OT_CHANGED_COMMISSIONER_STATE,           ///< Commissioner state changed
    kEventThreadNetifStateChanged          = OT_CHANGED_THREAD_NETIF_STATE,           ///< Netif state changed
    kEventThreadBackboneRouterStateChanged = OT_CHANGED_THREAD_BACKBONE_ROUTER_STATE, ///< Backbone Router state changed
    kEventThreadBackboneRouterLocalChanged = OT_CHANGED_THREAD_BACKBONE_ROUTER_LOCAL, ///< Local Backbone Router changed
    kEventJoinerStateChanged               = OT_CHANGED_JOINER_STATE,                 ///< Joiner state changed
    kEventActiveDatasetChanged             = OT_CHANGED_ACTIVE_DATASET,               ///< Active Dataset changed
    kEventPendingDatasetChanged            = OT_CHANGED_PENDING_DATASET,              ///< Pending Dataset changed
};

/**
 * This type represents a list of events.
 *
 */
class Events
{
public:
    /**
     * This type represents a bit-field indicating a list of events (with values from `Event`)
     *
     */
    typedef otChangedFlags Flags;

    /**
     * This constructor initializes the `Events` list (as empty).
     *
     */
    Events(void)
        : mEventFlags(0)
    {
    }

    /**
     * This method clears the `Events` list.
     *
     */
    void Clear(void) { mEventFlags = 0; }

    /**
     * This method indicates whether the `Events` list contains a given event.
     *
     * @param[in] aEvent  The event to check.
     *
     * @returns TRUE if the list contains the @p aEvent, FALSE otherwise.
     *
     */
    bool Contains(Event aEvent) const { return (mEventFlags & aEvent) != 0; }

    /**
     * This method indicates whether the `Events` list contains any of a given set of events.
     *
     * @param[in] aEvents  The events set to check (must be a collection of `Event` constants combined using `|`).
     *
     * @returns TRUE if the list contains any of the @p aEvents set, FALSE otherwise.
     *
     */
    bool ContainsAny(Flags aEvents) const { return (mEventFlags & aEvents) != 0; }

    /**
     * This method indicates whether the `Events` list contains all of a given set of events.
     *
     * @param[in] aEvents  The events set to check (must be collection of `Event` constants combined using `|`).
     *
     * @returns TRUE if the list contains all of the @p aEvents set, FALSE otherwise.
     *
     */
    bool ContainsAll(Flags aEvents) const { return (mEventFlags & aEvents) == aEvents; }

    /**
     * This method adds a given event to the `Events` list.
     *
     * @param[in] aEvent  The event to add.
     *
     */
    void Add(Event aEvent) { mEventFlags |= aEvent; }

    /**
     * This method indicates whether the `Events` list is empty.
     *
     * @returns TRUE if the list is empty, FALSE otherwise.
     *
     */
    bool IsEmpty(void) const { return (mEventFlags == 0); }

    /**
     * This method gets the `Events` list as bit-field `Flags` value.
     *
     * @returns The list as bit-field `Flags` value.
     *
     */
    Flags GetAsFlags(void) const { return mEventFlags; }

private:
    Flags mEventFlags;
};

/**
 * This class implements the OpenThread Notifier.
 *
 * For core internal modules, `Notifier` class emits events directly to them by invoking method `HandleNotifierEvents()`
 * on the module instance.
 *
 * A `otStateChangedCallback` callback can be explicitly registered with the `Notifier`. This is mainly intended for use
 * by external users (i.e.provided as an OpenThread public API). Max number of such callbacks that can be registered at
 * the same time is specified by `OPENTHREAD_CONFIG_MAX_STATECHANGE_HANDLERS` configuration parameter.
 *
 */
class Notifier : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This constructor initializes a `Notifier` instance.
     *
     *  @param[in] aInstance     A reference to OpenThread instance.
     *
     */
    explicit Notifier(Instance &aInstance);

    /**
     * This method registers an `otStateChangedCallback` handler.
     *
     * @param[in]  aCallback     A pointer to the handler function that is called to notify of the changes.
     * @param[in]  aContext      A pointer to arbitrary context information.
     *
     * @retval kErrorNone     Successfully registered the callback.
     * @retval kErrorAlready  The callback was already registered.
     * @retval kErrorNoBufs   Could not add the callback due to resource constraints.
     *
     */
    Error RegisterCallback(otStateChangedCallback aCallback, void *aContext);

    /**
     * This method removes/unregisters a previously registered `otStateChangedCallback` handler.
     *
     * @param[in]  aCallback     A pointer to the callback function pointer.
     * @param[in]  aContex       A pointer to arbitrary context information.
     *
     */
    void RemoveCallback(otStateChangedCallback aCallback, void *aContext);

    /**
     * This method schedules signaling of an event.
     *
     * @param[in]  aEvent     The event to signal.
     *
     */
    void Signal(Event aEvent);

    /**
     * This method schedules signaling of am event only if the event has not been signaled before (first time signal).
     *
     * @param[in]  aEvent     The event to signal.
     *
     */
    void SignalIfFirst(Event aEvent);

    /**
     * This method indicates whether or not an event signal callback is pending/scheduled.
     *
     * @returns TRUE if a callback is pending, FALSE otherwise.
     *
     */
    bool IsPending(void) const { return !mEventsToSignal.IsEmpty(); }

    /**
     * This method indicates whether or not an event has been signaled before.
     *
     * @param[in]  aEvent    The event to check.
     *
     * @retval TRUE    The event @p aEvent have been signaled before.
     * @retval FALSE   The event @p aEvent has not been signaled before.
     *
     */
    bool HasSignaled(Event aEvent) const { return mSignaledEvents.Contains(aEvent); }

    /**
     * This template method updates a variable of a type `Type` with a new value and signals the given event.
     *
     * If the variable is already set to the same value, this method returns `kErrorAlready` and the event is
     * signaled using `SignalIfFirst()` (i.e., signal is scheduled only if event has not been signaled before).
     *
     * The template `Type` should support comparison operator `==` and assignment operator `=`.
     *
     * @param[inout] aVariable    A reference to the variable to update.
     * @param[in]    aNewValue    The new value.
     * @param[in]    aEvent       The event to signal.
     *
     * @retval kErrorNone      The variable was update successfully and @p aEvent was signaled.
     * @retval kErrorAlready   The variable was already set to the same value.
     *
     */
    template <typename Type> Error Update(Type &aVariable, const Type &aNewValue, Event aEvent)
    {
        Error error = kErrorNone;

        if (aVariable == aNewValue)
        {
            SignalIfFirst(aEvent);
            error = kErrorAlready;
        }
        else
        {
            aVariable = aNewValue;
            Signal(aEvent);
        }

        return error;
    }

private:
    static constexpr uint16_t kMaxExternalHandlers = OPENTHREAD_CONFIG_MAX_STATECHANGE_HANDLERS;

    // Character limit to divide the log into multiple lines in `LogChangedFlags()`.
    static constexpr uint16_t kFlagsStringLineLimit = 70;

    // Max length for string representation of a flag by `FlagToString()`.
    static constexpr uint8_t kMaxFlagNameLength = 25;

    static constexpr uint16_t kFlagsStringBufferSize = kFlagsStringLineLimit + kMaxFlagNameLength;

    struct ExternalCallback
    {
        otStateChangedCallback mHandler;
        void *                 mContext;
    };

    static void EmitEvents(Tasklet &aTasklet);
    void        EmitEvents(void);

    void        LogEvents(Events aEvents) const;
    const char *EventToString(Event aEvent) const;

    Events           mEventsToSignal;
    Events           mSignaledEvents;
    Tasklet          mTask;
    ExternalCallback mExternalCallbacks[kMaxExternalHandlers];
};

/**
 * @}
 *
 */

} // namespace ot

#endif // NOTIFIER_HPP_
