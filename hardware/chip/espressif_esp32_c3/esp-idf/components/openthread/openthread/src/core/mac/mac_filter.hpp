/*
 *  Copyright (c) 2017, The OpenThread Authors.
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
 *   This file includes definitions for IEEE 802.15.4 frame filtering based on MAC address.
 */

#ifndef MAC_FILTER_HPP_
#define MAC_FILTER_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_MAC_FILTER_ENABLE

#include <stdint.h>

#include "common/non_copyable.hpp"
#include "mac/mac_frame.hpp"

namespace ot {
namespace Mac {

/**
 * @addtogroup core-mac
 *
 * @{
 *
 */

/**
 * This class implements Mac Filter on IEEE 802.15.4 frames.
 *
 */
class Filter : private NonCopyable
{
public:
    /**
     * This structure represents a Mac Filter entry (used during iteration).
     *
     */
    typedef otMacFilterEntry Entry;

    /**
     * This type represents an iterator used to iterate through filter entries.
     *
     * See `GetNextAddress()` and `GetNextRssIn()`.
     *
     */
    typedef otMacFilterIterator Iterator;

    /**
     * This enumeration type represents the MAC Filter mode.
     *
     */
    enum Mode : uint8_t
    {
        kModeRssInOnly = OT_MAC_FILTER_ADDRESS_MODE_DISABLED,  ///< No address filtering. RSS-In update only.
        kModeAllowlist = OT_MAC_FILTER_ADDRESS_MODE_ALLOWLIST, ///< Enable allowlist address filter mode.
        kModeDenylist  = OT_MAC_FILTER_ADDRESS_MODE_DENYLIST,  ///< Enable denylist address filter mode.
    };

    static constexpr int8_t kFixedRssDisabled = OT_MAC_FILTER_FIXED_RSS_DISABLED; ///< Value when no fixed RSS is set.

    /**
     * This constructor initializes the filter.
     *
     */
    Filter(void);

    /**
     * This method gets the MAC Filter mode.
     *
     * @returns  the Filter mode.
     *
     */
    Mode GetMode(void) const { return mMode; }

    /**
     * This method sets the address mode of the filter.
     *
     * @param[in]  aMode  The new Filter mode.
     *
     */
    void SetMode(Mode aMode) { mMode = aMode; }

    /**
     * This method adds an Extended Address to filter.
     *
     * @param[in]  aExtAddress  A reference to the Extended Address.
     *
     * @retval kErrorNone          Successfully added @p aExtAddress to the filter.
     * @retval kErrorNoBufs        No available entry exists.
     *
     */
    Error AddAddress(const ExtAddress &aExtAddress);

    /**
     * This method removes an Extended Address from the filter.
     *
     * No action is performed if there is no existing entry in the filter list matching the given Extended Address.
     *
     * @param[in]  aExtAddress  A reference to the Extended Address to remove.
     *
     */
    void RemoveAddress(const ExtAddress &aExtAddress);

    /**
     * This method clears all Extended Addresses from the filter.
     *
     */
    void ClearAddresses(void);

    /**
     * This method iterates through filter entries.
     *
     * @param[inout]  aIterator  A reference to the MAC filter iterator context.
     *                           To get the first in-use address filter, set it to OT_MAC_FILTER_ITERATOR_INIT.
     * @param[out]     aEntry    A reference to where the information is placed.
     *
     * @retval kErrorNone      Successfully retrieved the next address filter entry.
     * @retval kErrorNotFound  No subsequent entry exists.
     *
     */
    Error GetNextAddress(Iterator &aIterator, Entry &aEntry) const;

    /**
     * This method adds a fixed received signal strength entry for the messages from a given Extended Address.
     *
     * @param[in]  aExtAddress  An Extended Address
     * @param[in]  aRss         The received signal strength to set.
     *
     * @retval kErrorNone    Successfully set @p aRss for @p aExtAddress.
     * @retval kErrorNoBufs  No available entry exists.
     *
     */
    Error AddRssIn(const ExtAddress &aExtAddress, int8_t aRss);

    /**
     * This method removes a fixed received signal strength entry for a given Extended Address.
     *
     * No action is performed if there is no existing entry in the filter list matching the given Extended Address.
     *
     * @param[in]  aExtAddress   A Extended Address.
     *
     */
    void RemoveRssIn(const ExtAddress &aExtAddress);

    /**
     * This method sets the default received signal strength.
     *
     * The default RSS value is used for all received frames from addresses for which there is no explicit RSS-IN entry
     * in the Filter list (added using `AddRssIn()`).
     *
     * @param[in]  aRss  The default received signal strength to set.
     *
     */
    void SetDefaultRssIn(int8_t aRss) { mDefaultRssIn = aRss; }

    /**
     * This method clears the default received signal strength.
     *
     */
    void ClearDefaultRssIn(void) { mDefaultRssIn = kFixedRssDisabled; }

    /**
     * This method clears all the received signal strength settings (including the default RSS-In).
     *
     */
    void ClearAllRssIn(void);

    /**
     * This method iterates through RssIn filter entry.
     *
     * @param[inout]  aIterator  A reference to the MAC filter iterator context. To get the first in-use RssIn
     *                           filter entry, it should be set to OT_MAC_FILTER_ITERATOR_INIT.
     * @param[out]    aEntry     A reference to where the information is placed. The last entry would have the
     *                           Extended Address as all 0xff to indicate the default received signal strength
     *                           if it was set.
     *
     * @retval kErrorNone      Successfully retrieved the next RssIn filter entry.
     * @retval kErrorNotFound  No subsequent entry exists.
     *
     */
    Error GetNextRssIn(Iterator &aIterator, Entry &aEntry);

    /**
     * This method applies the filter rules on a given Extended Address.
     *
     * @param[in]  aExtAddress  A reference to the Extended Address.
     * @param[out] aRss         A reference to where the received signal strength to be placed.
     *
     * @retval kErrorNone             Successfully applied the filter rules on @p aExtAddress.
     * @retval kErrorAddressFiltered  Address filter (allowlist or denylist) is enabled and @p aExtAddress is filtered.
     *
     */
    Error Apply(const ExtAddress &aExtAddress, int8_t &aRss);

private:
    static constexpr uint16_t kMaxEntries = OPENTHREAD_CONFIG_MAC_FILTER_SIZE;

    struct FilterEntry
    {
        bool       mFiltered;   // Indicates whether or not this entry is filtered (allowlist/denylist modes).
        int8_t     mRssIn;      // The RssIn value for this entry or `kFixedRssDisabled`.
        ExtAddress mExtAddress; // IEEE 802.15.4 Extended Address.

        bool IsInUse(void) const { return mFiltered || (mRssIn != kFixedRssDisabled); }
    };

    FilterEntry *FindAvailableEntry(void);
    FilterEntry *FindEntry(const ExtAddress &aExtAddress);

    Mode        mMode;
    int8_t      mDefaultRssIn;
    FilterEntry mFilterEntries[kMaxEntries];
};

/**
 * @}
 *
 */

} // namespace Mac
} // namespace ot

#endif // OPENTHREAD_CONFIG_MAC_FILTER_ENABLE

#endif // MAC_FILTER_HPP_
