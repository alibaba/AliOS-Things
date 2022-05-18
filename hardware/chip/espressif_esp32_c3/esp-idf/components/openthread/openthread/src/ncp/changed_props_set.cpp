/*
 *    Copyright (c) 2016-2017, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "changed_props_set.hpp"

#include <limits.h>

#include "common/code_utils.hpp"

namespace ot {
namespace Ncp {

// ----------------------------------------------------------------------------
// MARK: ChangedPropsSet class
// ----------------------------------------------------------------------------

// Defines the list of properties that can support unsolicited update.
//
// Note that {`SPINEL_PROP_LAST_STATUS`, `SPINEL_STATUS_RESET_UNKNOWN`} should be first entry to ensure that RESET is
// reported before any other property update.
//
// Since a `uint64_t` is used as bit-mask to track which entries are in the changed set, we should ensure that the
// number of entries in the list is always less than or equal to 64.
//
const ChangedPropsSet::Entry ChangedPropsSet::mSupportedProps[] = {
    // Spinel property , Status (if prop is `LAST_STATUS`),  IsFilterable?

    {SPINEL_PROP_LAST_STATUS, SPINEL_STATUS_RESET_UNKNOWN, false},
    {SPINEL_PROP_STREAM_DEBUG, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_IPV6_LL_ADDR, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_IPV6_ML_ADDR, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_IPV6_ADDRESS_TABLE, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_NET_ROLE, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_NET_PARTITION_ID, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_NET_KEY_SEQUENCE_COUNTER, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_THREAD_LEADER_NETWORK_DATA, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_THREAD_CHILD_TABLE, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_THREAD_ON_MESH_NETS, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_THREAD_OFF_MESH_ROUTES, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_NET_STACK_UP, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_NET_REQUIRE_JOIN_EXISTING, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_LAST_STATUS, SPINEL_STATUS_NOMEM, true},
    {SPINEL_PROP_LAST_STATUS, SPINEL_STATUS_DROPPED, true},
#if OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE
    {SPINEL_PROP_JAM_DETECTED, SPINEL_STATUS_OK, true},
#endif
#if OPENTHREAD_CONFIG_LEGACY_ENABLE
    {SPINEL_PROP_NEST_LEGACY_ULA_PREFIX, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_NEST_LEGACY_LAST_NODE_JOINED, SPINEL_STATUS_OK, true},
#endif
    {SPINEL_PROP_LAST_STATUS, SPINEL_STATUS_JOIN_FAILURE, false},
    {SPINEL_PROP_MAC_SCAN_STATE, SPINEL_STATUS_OK, false},
    {SPINEL_PROP_IPV6_MULTICAST_ADDRESS_TABLE, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_PHY_CHAN, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_MAC_15_4_PANID, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_NET_NETWORK_NAME, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_NET_XPANID, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_NET_NETWORK_KEY, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_NET_PSKC, SPINEL_STATUS_OK, true},
    {SPINEL_PROP_PHY_CHAN_SUPPORTED, SPINEL_STATUS_OK, true},
#if OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE
    {SPINEL_PROP_CHANNEL_MANAGER_NEW_CHANNEL, SPINEL_STATUS_OK, true},
#endif
#if OPENTHREAD_CONFIG_JOINER_ENABLE
    {SPINEL_PROP_LAST_STATUS, SPINEL_STATUS_JOIN_NO_PEERS, false},
    {SPINEL_PROP_LAST_STATUS, SPINEL_STATUS_JOIN_SECURITY, false},
    {SPINEL_PROP_LAST_STATUS, SPINEL_STATUS_JOIN_RSP_TIMEOUT, false},
    {SPINEL_PROP_LAST_STATUS, SPINEL_STATUS_JOIN_SUCCESS, false},
#endif
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    {SPINEL_PROP_THREAD_NETWORK_TIME, SPINEL_STATUS_OK, false},
#endif
    {SPINEL_PROP_PARENT_RESPONSE_INFO, SPINEL_STATUS_OK, true},
};

uint8_t ChangedPropsSet::GetNumEntries(void) const
{
    static_assert(OT_ARRAY_LENGTH(mSupportedProps) <= sizeof(mChangedSet) * CHAR_BIT,
                  "Changed set size is smaller than number of entries in `mSupportedProps[]` array");

    return OT_ARRAY_LENGTH(mSupportedProps);
}

void ChangedPropsSet::Add(spinel_prop_key_t aPropKey, spinel_status_t aStatus)
{
    uint8_t      numEntries;
    const Entry *entry;

    entry = GetSupportedEntries(numEntries);

    for (uint8_t index = 0; index < numEntries; index++, entry++)
    {
        if ((entry->mPropKey == aPropKey) && (entry->mStatus == aStatus))
        {
            if (!IsEntryFiltered(index))
            {
                SetBit(mChangedSet, index);
            }

            break;
        }
    }
}

otError ChangedPropsSet::EnablePropertyFilter(spinel_prop_key_t aPropKey, bool aEnable)
{
    uint8_t      numEntries;
    const Entry *entry;
    bool         didFind = false;

    entry = GetSupportedEntries(numEntries);

    for (uint8_t index = 0; index < numEntries; index++, entry++)
    {
        if (entry->mFilterable && (entry->mPropKey == aPropKey))
        {
            if (aEnable)
            {
                SetBit(mFilterSet, index);

                // If filter is enabled for a property, the `mChangedSet` is cleared
                // for the same property so to ensure a pending update is also filtered.

                ClearBit(mChangedSet, index);
            }
            else
            {
                ClearBit(mFilterSet, index);
            }

            didFind = true;

            // Continue the search only if the prop key is `LAST_STATUS`, as
            // we have multiple filterable `LAST_STATUS` entries in the table
            // with different error status (DROPPED and NOMEM).

            if (aPropKey != SPINEL_PROP_LAST_STATUS)
            {
                break;
            }
        }
    }

    return didFind ? OT_ERROR_NONE : OT_ERROR_INVALID_ARGS;
}

bool ChangedPropsSet::IsPropertyFiltered(spinel_prop_key_t aPropKey) const
{
    bool         isFiltered = false;
    uint8_t      numEntries;
    const Entry *entry;

    entry = GetSupportedEntries(numEntries);

    for (uint8_t index = 0; index < numEntries; index++, entry++)
    {
        if (entry->mFilterable && (entry->mPropKey == aPropKey))
        {
            isFiltered = IsEntryFiltered(index);

            break;
        }
    }

    return isFiltered;
}

} // namespace Ncp
} // namespace ot
