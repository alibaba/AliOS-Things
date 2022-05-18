#!/usr/bin/env python3
#
#  Copyright (c) 2020, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.

import time
import wpan
from wpan import verify

# -----------------------------------------------------------------------------------------------------------------------
# Test description: Address Cache Table
#
# This test verifies the behavior of `AddressResolver` and how the cache
# table is managed. In particular it verifies behavior query timeout and
# query retry and snoop optimization.

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

r1 = wpan.Node()
r2 = wpan.Node()
r3 = wpan.Node()

c2 = wpan.Node()
c3 = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#
#  r3 ---- r1 ---- r2
#  |               |
#  |               |
#  c3              c2
#

PREFIX = "fd00:1234::"
POLL_INTERVAL = 200

MAX_SNOOPED_NON_EVICTABLE = 2
INITIAL_RETRY_DELAY = 4
MAX_CACHE_ENTRIES = 16

r1.form("sekiro")  # shadows die twice!

r1.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=False, preferred=True)

r1.allowlist_node(r2)
r2.allowlist_node(r1)
r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)

c2.allowlist_node(r2)
r2.allowlist_node(c2)
c2.join_node(r2, wpan.JOIN_TYPE_END_DEVICE)

r1.allowlist_node(r3)
r3.allowlist_node(r1)
r3.join_node(r1, wpan.JOIN_TYPE_ROUTER)

c3.allowlist_node(r3)
r3.allowlist_node(c3)
c3.join_node(r3, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
c3.set(wpan.WPAN_POLL_INTERVAL, str(POLL_INTERVAL))

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation
#

# Add IPv6 addresses on different nodes.

r1_address = PREFIX + "1"
r1.add_ip6_address_on_interface(r1_address)

WAIT_TIME = 10
PORT = 1234

NUM_ADDRESSES = 4  # Number of addresses to add on r2, r3, c2, and c3

for num in range(NUM_ADDRESSES):
    r2.add_ip6_address_on_interface(PREFIX + "2:" + str(num))
    r3.add_ip6_address_on_interface(PREFIX + "3:" + str(num))
    c2.add_ip6_address_on_interface(PREFIX + "c2:" + str(num))
    c3.add_ip6_address_on_interface(PREFIX + "c3:" + str(num))

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# From r1 send msg to a group of addresses (not provided by any nodes in network).

NUM_QUERY_ADDRS = 5
MAX_STAGGER_INTERVAL = 2.5

for num in range(NUM_QUERY_ADDRS):
    sender = r1.prepare_tx((r1_address, PORT), (PREFIX + "800:" + str(num), PORT), "hi nobody!", 1)
    wpan.Node.perform_async_tx_rx()
    verify(sender.was_successful)
    # Wait before next tx to stagger the address queries
    # request ensuring different timeouts
    time.sleep(MAX_STAGGER_INTERVAL / (NUM_QUERY_ADDRS * speedup))

r2_rloc = int(r2.get(wpan.WPAN_THREAD_RLOC16), 16)
c2_rloc = int(c2.get(wpan.WPAN_THREAD_RLOC16), 16)
r3_rloc = int(r3.get(wpan.WPAN_THREAD_RLOC16), 16)

# Verify that we do see entries in cache table for all the addresses and all are in "query" state

addr_cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))

verify(len(addr_cache_table) == NUM_QUERY_ADDRS)
for entry in addr_cache_table:
    verify(entry.state == wpan.ADDRESS_CACHE_ENTRY_STATE_QUERY)
    verify(not entry.can_evict())
    verify(entry.timeout > 0)
    verify(entry.retry_delay == INITIAL_RETRY_DELAY)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Check the retry-query behavior

# Wait till all the address queries time out and verify they enter "retry-query" state.


def check_cache_entry_switch_to_retry_state():
    cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))
    for index in range(NUM_QUERY_ADDRS):
        verify(cache_table[index].state == wpan.ADDRESS_CACHE_ENTRY_STATE_RETRY_QUERY)
        verify(cache_table[index].retry_delay == 2 * INITIAL_RETRY_DELAY)


wpan.verify_within(check_cache_entry_switch_to_retry_state, WAIT_TIME)

# Try sending again to same addresses which are in "retry-delay" state.

for num in range(NUM_QUERY_ADDRS):
    sender = r1.prepare_tx((r1_address, PORT), (PREFIX + "800:" + str(num), PORT), "hi nobody!", 1)
    wpan.Node.perform_async_tx_rx()
    verify(sender.was_successful)

# Make sure the entries stayed in retry-delay as before.

wpan.verify_within(check_cache_entry_switch_to_retry_state, WAIT_TIME)

# Now wait for them to get to zero timeout.

cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))


def check_cache_entry_in_retry_state_to_get_to_zero_timeout():
    cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))
    for index in range(NUM_QUERY_ADDRS):
        verify(cache_table[index].state == wpan.ADDRESS_CACHE_ENTRY_STATE_RETRY_QUERY)
        verify(cache_table[index].timeout == 0)


wpan.verify_within(check_cache_entry_in_retry_state_to_get_to_zero_timeout, WAIT_TIME)

# Now try again using the same addresses.

for num in range(NUM_QUERY_ADDRS):
    sender = r1.prepare_tx((r1_address, PORT), (PREFIX + "800:" + str(num), PORT), "hi again nobody!", 1)
    wpan.Node.perform_async_tx_rx()
    verify(sender.was_successful)

# We expect now after the delay to see retries for same addresses.


def check_cache_entry_switch_to_query_state():
    cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))
    for index in range(NUM_QUERY_ADDRS):
        verify(cache_table[index].state == wpan.ADDRESS_CACHE_ENTRY_STATE_QUERY)
        verify(cache_table[index].can_evict() == True)


wpan.verify_within(check_cache_entry_switch_to_query_state, WAIT_TIME)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Verify snoop optimization.

for num in range(NUM_ADDRESSES):
    sender = r2.prepare_tx((PREFIX + "2:" + str(num), PORT), (r1_address, PORT), "hi r1 from r2 (snoop me)", 1)
    recver = r1.prepare_rx(sender)
    wpan.Node.perform_async_tx_rx()
    verify(sender.was_successful)
    verify(recver.was_successful)

cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))

# We expect to see new "snooped" entries at the top of list.
# Also verify that only MAX_SNOOPED_NON_EVICTABLE of snooped entries are non-evictable.

verify(len(cache_table) >= NUM_ADDRESSES)

for index in range(NUM_ADDRESSES):
    verify(cache_table[index].address == PREFIX + "2:" + str(NUM_ADDRESSES - index - 1))
    verify(cache_table[index].rloc16 == r2_rloc)
    verify(cache_table[index].state == wpan.ADDRESS_CACHE_ENTRY_STATE_SNOOPED)
    if index < NUM_ADDRESSES - MAX_SNOOPED_NON_EVICTABLE:
        verify(cache_table[index].can_evict() == True)
        verify(cache_table[index].timeout == 0)
    else:
        verify(cache_table[index].can_evict() == False)
        verify(cache_table[index].timeout > 0)

# From r1 send to r2 using the addresses from snooped entries:

for num in range(NUM_ADDRESSES):
    sender = r1.prepare_tx((r1_address, PORT), (PREFIX + "2:" + str(num), PORT), "hi back r2 from r1", 1)
    recver = r2.prepare_rx(sender)
    wpan.Node.perform_async_tx_rx()
    verify(sender.was_successful)
    verify(recver.was_successful)

cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))

# We expect to see the entries to be in "cached" state now.

verify(len(cache_table) >= NUM_ADDRESSES)

for index in range(NUM_ADDRESSES):
    verify(cache_table[index].address == PREFIX + "2:" + str(NUM_ADDRESSES - index - 1))
    verify(cache_table[index].rloc16 == r2_rloc)
    verify(cache_table[index].state == wpan.ADDRESS_CACHE_ENTRY_STATE_CACHED)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Check query requests, last transaction time

# Send from r1 to all addresses on r3.

for num in range(NUM_ADDRESSES):
    sender = r1.prepare_tx((r1_address, PORT), (PREFIX + "3:" + str(num), PORT), "hi r3 from r1", 1)
    recver = r3.prepare_rx(sender)
    wpan.Node.perform_async_tx_rx()
    verify(sender.was_successful)
    verify(recver.was_successful)
    cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))

# We expect to see the cache entries for the addresses pointing to r3.

cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))

for index in range(NUM_ADDRESSES):
    verify(cache_table[index].address == PREFIX + "3:" + str(NUM_ADDRESSES - index - 1))
    verify(cache_table[index].rloc16 == r3_rloc)
    verify(cache_table[index].state == wpan.ADDRESS_CACHE_ENTRY_STATE_CACHED)
    verify(cache_table[index].last_trans == 0)

# Send from r1 to all addresses on c3 (sleepy child of r3)

for num in range(NUM_ADDRESSES):
    sender = r1.prepare_tx((r1_address, PORT), (PREFIX + "c3:" + str(num), PORT), "hi c3 from r1", 1)
    recver = c3.prepare_rx(sender)
    wpan.Node.perform_async_tx_rx()
    verify(sender.was_successful)
    verify(recver.was_successful)

# We expect to see the cache entries for c3 addresses pointing to r3.

cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))

for index in range(NUM_ADDRESSES):
    verify(cache_table[index].address == PREFIX + "c3:" + str(NUM_ADDRESSES - index - 1))
    verify(cache_table[index].rloc16 == r3_rloc)
    verify(cache_table[index].state == wpan.ADDRESS_CACHE_ENTRY_STATE_CACHED)
    # SED's keep-alive period (`POLL_PERIOD`) is 200ms, `last_trans` should always be 0 as it is
    # the number of seconds since a keep-alive was last received from the child.
    verify(cache_table[index].last_trans == 0)

# Send again to r2. This should cause the related cache entries to be moved to top of the list:

for num in range(NUM_ADDRESSES):
    sender = r1.prepare_tx((r1_address, PORT), (PREFIX + "2:" + str(num), PORT), "hi again r2 from r1", 1)
    recver = r2.prepare_rx(sender)
    wpan.Node.perform_async_tx_rx()
    verify(sender.was_successful)
    verify(recver.was_successful)

cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))

for index in range(NUM_ADDRESSES):
    verify(cache_table[index].address == PREFIX + "2:" + str(NUM_ADDRESSES - index - 1))
    verify(cache_table[index].rloc16 == r2_rloc)
    verify(cache_table[index].state == wpan.ADDRESS_CACHE_ENTRY_STATE_CACHED)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Check behavior when cache table is full.

verify(len(cache_table) == MAX_CACHE_ENTRIES)

for num in range(NUM_QUERY_ADDRS):
    sender = r1.prepare_tx((r1_address, PORT), (PREFIX + "900:" + str(num), PORT), "hi nobody!", 1)
    wpan.Node.perform_async_tx_rx()
    verify(sender.was_successful)

cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))

verify(len(cache_table) == MAX_CACHE_ENTRIES)

# Send from c2 to r1 and verify that snoop optimization uses at most
# `MAX_SNOOPED_NON_EVICTABLE` entries.

for num in range(NUM_ADDRESSES):
    sender = c2.prepare_tx((PREFIX + "c2:" + str(num), PORT), (r1_address, PORT), "hi r1 from c2 (snoop me)", 1)
    recver = r1.prepare_rx(sender)
    wpan.Node.perform_async_tx_rx()
    verify(sender.was_successful)
    verify(recver.was_successful)

cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))

verify(len(cache_table) == MAX_CACHE_ENTRIES)

snooped_entries = [entry for entry in cache_table if entry.state == wpan.ADDRESS_CACHE_ENTRY_STATE_SNOOPED]
verify(len(snooped_entries) == MAX_SNOOPED_NON_EVICTABLE)

for entry in snooped_entries:
    verify(entry.rloc16 == c2_rloc)
    verify(entry.state == wpan.ADDRESS_CACHE_ENTRY_STATE_SNOOPED)
    verify(entry.can_evict() == False)
    verify(entry.timeout > 0)

# Now send from r1 to c2, some of the snooped entries would be used,
# others would  go through full address query.

for num in range(NUM_ADDRESSES):
    sender = r1.prepare_tx((r1_address, PORT), (PREFIX + "c2:" + str(num), PORT), "hi c2 from r1", 1)
    recver = c2.prepare_rx(sender)
    wpan.Node.perform_async_tx_rx()
    verify(sender.was_successful)
    verify(recver.was_successful)

cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))

verify(len(cache_table) == MAX_CACHE_ENTRIES)

# Verify that c2 entries are now at the top of cache list.

for index in range(NUM_ADDRESSES):
    verify(cache_table[index].address == PREFIX + "c2:" + str(NUM_ADDRESSES - index - 1))
    verify(cache_table[index].rloc16 == c2_rloc)
    verify(cache_table[index].state == wpan.ADDRESS_CACHE_ENTRY_STATE_CACHED)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
