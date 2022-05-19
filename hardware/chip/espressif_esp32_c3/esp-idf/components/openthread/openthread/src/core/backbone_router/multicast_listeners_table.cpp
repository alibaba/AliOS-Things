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
 *   This file implements the Multicast Listeners Table.
 */

#include "multicast_listeners_table.hpp"

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"
#include "thread/mle_types.hpp"
#include "thread/thread_netif.hpp"
#include "thread/uri_paths.hpp"

namespace ot {

namespace BackboneRouter {

Error MulticastListenersTable::Add(const Ip6::Address &aAddress, Time aExpireTime)
{
    Error error = kErrorNone;

    VerifyOrExit(aAddress.IsMulticastLargerThanRealmLocal(), error = kErrorInvalidArgs);

    for (uint16_t i = 0; i < mNumValidListeners; i++)
    {
        Listener &listener = mListeners[i];

        if (listener.GetAddress() == aAddress)
        {
            listener.SetExpireTime(aExpireTime);
            FixHeap(i);
            ExitNow();
        }
    }

    VerifyOrExit(mNumValidListeners < OT_ARRAY_LENGTH(mListeners), error = kErrorNoBufs);

    mListeners[mNumValidListeners].SetAddress(aAddress);
    mListeners[mNumValidListeners].SetExpireTime(aExpireTime);
    mNumValidListeners++;

    FixHeap(mNumValidListeners - 1);

    if (mCallback != nullptr)
    {
        mCallback(mCallbackContext, OT_BACKBONE_ROUTER_MULTICAST_LISTENER_ADDED, &aAddress);
    }

exit:
    LogMulticastListenersTable("Add", aAddress, aExpireTime, error);
    CheckInvariants();
    return error;
}

void MulticastListenersTable::Remove(const Ip6::Address &aAddress)
{
    Error error = kErrorNotFound;

    for (uint16_t i = 0; i < mNumValidListeners; i++)
    {
        Listener &listener = mListeners[i];

        if (listener.GetAddress() == aAddress)
        {
            mNumValidListeners--;

            if (i != mNumValidListeners)
            {
                listener = mListeners[mNumValidListeners];
                FixHeap(i);
            }

            if (mCallback != nullptr)
            {
                mCallback(mCallbackContext, OT_BACKBONE_ROUTER_MULTICAST_LISTENER_REMOVED, &aAddress);
            }

            ExitNow(error = kErrorNone);
        }
    }

exit:
    LogMulticastListenersTable("Remove", aAddress, TimeMilli(0), error);
    CheckInvariants();
}

void MulticastListenersTable::Expire(void)
{
    TimeMilli    now = TimerMilli::GetNow();
    Ip6::Address address;

    while (mNumValidListeners > 0 && now >= mListeners[0].GetExpireTime())
    {
        LogMulticastListenersTable("Expire", mListeners[0].GetAddress(), mListeners[0].GetExpireTime(), kErrorNone);
        address = mListeners[0].GetAddress();

        mNumValidListeners--;

        if (mNumValidListeners > 0)
        {
            mListeners[0] = mListeners[mNumValidListeners];
            FixHeap(0);
        }

        if (mCallback != nullptr)
        {
            mCallback(mCallbackContext, OT_BACKBONE_ROUTER_MULTICAST_LISTENER_REMOVED, &address);
        }
    }

    CheckInvariants();
}

void MulticastListenersTable::LogMulticastListenersTable(const char *        aAction,
                                                         const Ip6::Address &aAddress,
                                                         TimeMilli           aExpireTime,
                                                         Error               aError)
{
    OT_UNUSED_VARIABLE(aAction);
    OT_UNUSED_VARIABLE(aAddress);
    OT_UNUSED_VARIABLE(aExpireTime);
    OT_UNUSED_VARIABLE(aError);

    otLogDebgBbr("MulticastListenersTable: %s %s expire %u: %s", aAction, aAddress.ToString().AsCString(),
                 aExpireTime.GetValue(), ErrorToString(aError));
}

void MulticastListenersTable::FixHeap(uint16_t aIndex)
{
    if (!SiftHeapElemDown(aIndex))
    {
        SiftHeapElemUp(aIndex);
    }
}

void MulticastListenersTable::CheckInvariants(void) const
{
#if OPENTHREAD_EXAMPLES_SIMULATION && OPENTHREAD_CONFIG_ASSERT_ENABLE
    for (uint16_t child = 1; child < mNumValidListeners; ++child)
    {
        uint16_t parent = (child - 1) / 2;

        OT_ASSERT(!(mListeners[child] < mListeners[parent]));
    }
#endif
}

bool MulticastListenersTable::SiftHeapElemDown(uint16_t aIndex)
{
    uint16_t index = aIndex;
    Listener saveElem;

    OT_ASSERT(aIndex < mNumValidListeners);

    saveElem = mListeners[aIndex];

    for (;;)
    {
        uint16_t child = 2 * index + 1;

        if (child >= mNumValidListeners || child <= index) // child <= index after int overflow
        {
            break;
        }

        if (child + 1 < mNumValidListeners && mListeners[child + 1] < mListeners[child])
        {
            child++;
        }

        if (!(mListeners[child] < saveElem))
        {
            break;
        }

        mListeners[index] = mListeners[child];

        index = child;
    }

    if (index > aIndex)
    {
        mListeners[index] = saveElem;
    }

    return index > aIndex;
}

void MulticastListenersTable::SiftHeapElemUp(uint16_t aIndex)
{
    uint16_t index = aIndex;
    Listener saveElem;

    OT_ASSERT(aIndex < mNumValidListeners);

    saveElem = mListeners[aIndex];

    for (;;)
    {
        uint16_t parent = (index - 1) / 2;

        if (index == 0 || !(saveElem < mListeners[parent]))
        {
            break;
        }

        mListeners[index] = mListeners[parent];

        index = parent;
    }

    if (index < aIndex)
    {
        mListeners[index] = saveElem;
    }
}

MulticastListenersTable::Listener *MulticastListenersTable::IteratorBuilder::begin(void)
{
    return &Get<MulticastListenersTable>().mListeners[0];
}

MulticastListenersTable::Listener *MulticastListenersTable::IteratorBuilder::end(void)
{
    return &Get<MulticastListenersTable>().mListeners[Get<MulticastListenersTable>().mNumValidListeners];
}

void MulticastListenersTable::Clear(void)
{
    if (mCallback != nullptr)
    {
        for (uint16_t i = 0; i < mNumValidListeners; i++)
        {
            mCallback(mCallbackContext, OT_BACKBONE_ROUTER_MULTICAST_LISTENER_REMOVED, &mListeners[i].GetAddress());
        }
    }

    mNumValidListeners = 0;

    CheckInvariants();
}

void MulticastListenersTable::SetCallback(otBackboneRouterMulticastListenerCallback aCallback, void *aContext)
{
    mCallback        = aCallback;
    mCallbackContext = aContext;

    if (mCallback != nullptr)
    {
        for (uint16_t i = 0; i < mNumValidListeners; i++)
        {
            mCallback(mCallbackContext, OT_BACKBONE_ROUTER_MULTICAST_LISTENER_ADDED, &mListeners[i].GetAddress());
        }
    }
}

Error MulticastListenersTable::GetNext(otBackboneRouterMulticastListenerIterator &aIterator,
                                       otBackboneRouterMulticastListenerInfo &    aListenerInfo)
{
    Error     error = kErrorNone;
    TimeMilli now;

    VerifyOrExit(aIterator < mNumValidListeners, error = kErrorNotFound);

    now = TimerMilli::GetNow();

    aListenerInfo.mAddress = mListeners[aIterator].mAddress;
    aListenerInfo.mTimeout =
        Time::MsecToSec(mListeners[aIterator].mExpireTime > now ? mListeners[aIterator].mExpireTime - now : 0);

    aIterator++;

exit:
    return error;
}

} // namespace BackboneRouter

} // namespace ot

#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE
