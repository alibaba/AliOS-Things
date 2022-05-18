/*
 *  Copyright (c) 2020, The OpenThread Authors.
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
 *   This file includes definitions for Multicast Listeners Table.
 */

#ifndef MULTICAST_LISTENERS_TABLE_HPP
#define MULTICAST_LISTENERS_TABLE_HPP

#include "openthread-core-config.h"

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE

#include <openthread/backbone_router_ftd.h>

#include "common/non_copyable.hpp"
#include "common/notifier.hpp"
#include "common/time.hpp"
#include "net/ip6_address.hpp"

namespace ot {

namespace BackboneRouter {

/**
 * This class implements the definitions for Multicast Listeners Table.
 *
 */
class MulticastListenersTable : public InstanceLocator, private NonCopyable
{
    class IteratorBuilder;

public:
    /**
     * This class represents a Multicast Listener entry.
     *
     */
    class Listener : public Clearable<Listener>
    {
        friend class MulticastListenersTable;

    public:
        /**
         * This constructor initializes the `Listener` object.
         *
         */
        Listener(void) { Clear(); }

        /**
         * This method returns the Multicast Listener address.
         *
         * @returns The Multicast Listener address.
         *
         */
        const Ip6::Address &GetAddress(void) const { return mAddress; }

        /**
         * This method returns the expire time of the Multicast Listener.
         *
         * @returns The Multicast Listener expire time.
         *
         */
        const TimeMilli GetExpireTime(void) const { return mExpireTime; }

    private:
        void SetAddress(const Ip6::Address &aAddress) { mAddress = aAddress; }
        void SetExpireTime(TimeMilli aExpireTime) { mExpireTime = aExpireTime; }

        bool operator<(const Listener &aOther) const { return GetExpireTime() < aOther.GetExpireTime(); }

        Ip6::Address mAddress;
        TimeMilli    mExpireTime;
    };

    /**
     * This constructor initializes the Multicast Listeners Table.
     *
     * @param[in] aInstance  A reference to the OpenThread instance.
     *
     */
    explicit MulticastListenersTable(Instance &aInstance)
        : InstanceLocator(aInstance)
        , mNumValidListeners(0)
        , mCallback(nullptr)
        , mCallbackContext(nullptr)
    {
    }

    /**
     * This method adds a Multicast Listener with given address and expire time.
     *
     * @param[in] aAddress     The Multicast Listener address.
     * @param[in] aExpireTime  The Multicast Listener expire time.
     *
     * @retval kErrorNone         If the Multicast Listener was successfully added.
     * @retval kErrorInvalidArgs  If the Multicast Listener address was invalid.
     * @retval kErrorNoBufs       No space available to save the Multicast Listener.
     *
     */
    Error Add(const Ip6::Address &aAddress, TimeMilli aExpireTime);

    /**
     * This method removes a given Multicast Listener.
     *
     * @param[in] aAddress  The Multicast Listener address.
     *
     */
    void Remove(const Ip6::Address &aAddress);

    /**
     * This method removes expired Multicast Listeners.
     *
     */
    void Expire(void);

    /**
     * This method counts the number of valid Multicast Listeners.
     *
     * @returns The number of valid Multicast Listeners.
     *
     */
    uint16_t Count(void) const { return mNumValidListeners; }

    /**
     * This method enables range-based `for` loop iteration over all Multicast Listeners.
     *
     * This method should be used as follows:
     *
     *     for (MulticastListenersTable::Listener &listener : Get<MulticastListenersTable>().Iterate())
     *
     * @returns An IteratorBuilder instance.
     *
     */
    IteratorBuilder Iterate(void) { return IteratorBuilder(GetInstance()); }

    /**
     * This method removes all the Multicast Listeners.
     *
     */
    void Clear(void);

    /**
     * This method sets the Multicast Listener callback.
     *
     * @param[in] aCallback  The callback function.
     * @param[in] aContext   A user context pointer.
     *
     */
    void SetCallback(otBackboneRouterMulticastListenerCallback aCallback, void *aContext);

    /**
     * This method gets the next Multicast Listener.
     *
     * @param[in] aIterator       A pointer to the Multicast Listener Iterator.
     * @param[out] aListenerInfo  A pointer to where the Multicast Listener info is placed.
     *
     * @retval kErrorNone         Successfully found the next Multicast Listener info.
     * @retval kErrorNotFound     No subsequent Multicast Listener was found.
     *
     */
    Error GetNext(otBackboneRouterMulticastListenerIterator &aIterator,
                  otBackboneRouterMulticastListenerInfo &    aListenerInfo);

private:
    static constexpr uint16_t kMulticastListenersTableSize = OPENTHREAD_CONFIG_MAX_MULTICAST_LISTENERS;

    static_assert(
        kMulticastListenersTableSize >= 75,
        "Thread 1.2 Conformance requires the Multicast Listener Table size to be larger than or equal to 75.");

    class IteratorBuilder : InstanceLocator
    {
    public:
        explicit IteratorBuilder(Instance &aInstance)
            : InstanceLocator(aInstance)
        {
        }

        Listener *begin(void);
        Listener *end(void);
    };

    void LogMulticastListenersTable(const char *        aAction,
                                    const Ip6::Address &aAddress,
                                    TimeMilli           aExpireTime,
                                    Error               aError);

    void FixHeap(uint16_t aIndex);
    bool SiftHeapElemDown(uint16_t aIndex);
    void SiftHeapElemUp(uint16_t aIndex);
    void CheckInvariants(void) const;

    Listener mListeners[kMulticastListenersTableSize];
    uint16_t mNumValidListeners;

    otBackboneRouterMulticastListenerCallback mCallback;
    void *                                    mCallbackContext;
};

} // namespace BackboneRouter

/**
 * @}
 */

} // namespace ot

#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE

#endif // MULTICAST_LISTENERS_TABLE_HPP
