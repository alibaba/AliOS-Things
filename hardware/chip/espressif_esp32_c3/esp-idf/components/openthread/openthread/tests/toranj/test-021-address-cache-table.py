#!/usr/bin/env python3
#
#  Copyright (c) 2018, The OpenThread Authors.
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

import wpan
from wpan import verify

# -----------------------------------------------------------------------------------------------------------------------
# Test description: Address Cache Table
#
# This test verifies the behavior of `AddressResolver` module and entries in
# address cache table. It also tests the behavior of nodes when there are
# topology changes in the network (e.g., a child switches parent). In
# particular, the test covers the address cache update through snooping, i.e.,
# the logic which inspects forwarded frames to update address cache table if
# source RLOC16 on a received frame differs from an existing entry in the
# address cache table.

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
c1 = wpan.Node()
c2 = wpan.Node()
c3 = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#
#     r1 ---- r2 ---- r3
#     |       |       |
#     |       |       |
#     c1      c2(s)   c3
#
# c1 and c3 are FED children, c2 is an SED which is first attached to r2 and
# then forced to switch to r3.

PREFIX = "fd00:1234::"
POLL_INTERVAL = 400

r1.form("addr-cache-tbl")

r1.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=True, preferred=True)

r1.allowlist_node(c1)
c1.allowlist_node(r1)
c1.join_node(r1, wpan.JOIN_TYPE_END_DEVICE)

r1.allowlist_node(r2)
r2.allowlist_node(r1)
r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)

c2.allowlist_node(r2)
r2.allowlist_node(c2)
c2.join_node(r2, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
c2.set(wpan.WPAN_POLL_INTERVAL, str(POLL_INTERVAL))

r2.allowlist_node(r3)
r3.allowlist_node(r2)
r3.join_node(r2, wpan.JOIN_TYPE_ROUTER)

c3.allowlist_node(r3)
r3.allowlist_node(c3)
c3.join_node(r3, wpan.JOIN_TYPE_END_DEVICE)

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation
#

ROUTER_TABLE_WAIT_TIME = 30 / speedup + 5

INVALID_ROUTER_ID = 63

verify(r1.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_LEADER)
verify(r2.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_ROUTER)
verify(r3.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_ROUTER)
verify(c1.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_END_DEVICE)
verify(c2.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_SLEEPY_END_DEVICE)
verify(c3.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_END_DEVICE)

r2_rloc = int(r2.get(wpan.WPAN_THREAD_RLOC16), 16)
r3_rloc = int(r3.get(wpan.WPAN_THREAD_RLOC16), 16)
c3_rloc = int(c3.get(wpan.WPAN_THREAD_RLOC16), 16)

# Wait till we have a valid "next hop" route on r1 towards r3


def check_r1_router_table():
    router_table = wpan.parse_router_table_result(r1.get(wpan.WPAN_THREAD_ROUTER_TABLE))
    verify(len(router_table) == 3)
    for entry in router_table:
        if entry.rloc16 == r3_rloc:
            verify(entry.next_hop != INVALID_ROUTER_ID)


wpan.verify_within(check_r1_router_table, ROUTER_TABLE_WAIT_TIME)

r1_address = r1.find_ip6_address_with_prefix(PREFIX)
c1_address = c1.find_ip6_address_with_prefix(PREFIX)
c2_address = c2.find_ip6_address_with_prefix(PREFIX)
c3_address = c3.find_ip6_address_with_prefix(PREFIX)

# Send a single UDP message from r1 to c2

sender = r1.prepare_tx(r1_address, c2_address, "Hi from r1 to c2")
recver = c2.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful and recver.was_successful)

# Send a single UDP message from r1 to c3

sender = r1.prepare_tx(r1_address, c3_address, "Hi from r1 to c3")
recver = c3.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful and recver.was_successful)

# The address cache table on r1 should contain two entries for
# c2 and c3 addresses.

addr_cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))
verify(len(addr_cache_table) == 2)

for entry in addr_cache_table:
    if entry.address == c2_address:
        # Entry for c2 should point towards its parent r2.
        verify(entry.rloc16 == r2_rloc)
    elif entry.address == c3_address:
        # Entry for c3 should point towards c3 itself.
        verify(entry.rloc16 == c3_rloc)
    else:
        raise (wpan.VerifyError("Unknown entry in the address cache table"))

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Force c2 to switch its parent from r2 to r3

CHILD_SUPERVISION_CHECK_TIMEOUT = 2
PARENT_SUPERVISION_INTERVAL = 1

REATTACH_WAIT_TIME = CHILD_SUPERVISION_CHECK_TIMEOUT / speedup + 6

c2.set(
    wpan.WPAN_CHILD_SUPERVISION_CHECK_TIMEOUT,
    str(CHILD_SUPERVISION_CHECK_TIMEOUT),
)
r3.set(wpan.WPAN_CHILD_SUPERVISION_INTERVAL, str(PARENT_SUPERVISION_INTERVAL))

r2.un_allowlist_node(c2)
r3.allowlist_node(c2)
c2.allowlist_node(r3)

# Wait for c2 to detach from r2 and attach to r3.
#
# Upon re-attach, previous parent r2 is notified and should remove c2 from
# its child table.


def check_c2_is_removed_from_r2_child_table():
    child_table = wpan.parse_list(r2.get(wpan.WPAN_THREAD_CHILD_TABLE))
    verify(len(child_table) == 0)


wpan.verify_within(check_c2_is_removed_from_r2_child_table, REATTACH_WAIT_TIME)

# Verify that both c2, c3 are children of r3

child_table = wpan.parse_list(r3.get(wpan.WPAN_THREAD_CHILD_TABLE))
verify(len(child_table) == 2)

# New network topology
#
#     r1 ---- r2 ---- r3
#     |               /\
#     |              /  \
#     c1           c2(s) c3

# From r1 send again to c2 (which is now a child of r3).
#
# Note that r1 still has r2 as the destination for c2's address in its address
# cache table.  But since r2 is aware that c2 is no longer its child, when it
# receives the IPv6 message with c2's address, r2 itself would do an address
# query for the address and forward the IPv6 message.

sender = r1.prepare_tx(r1_address, c2_address, "Hi again c2")
recver = c2.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful and recver.was_successful)

# The address cache table on r1 should have c2's address removed.

addr_cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))
verify(len(addr_cache_table) == 1)

for entry in addr_cache_table:
    if entry.address == c3_address:
        # Entry for c3 should still point towards c3
        verify(entry.rloc16 == c3_rloc)
    else:
        raise (wpan.VerifyError("Unknown entry in the address cache table"))

# Send a UDP message from r1 to c2.

sender = r1.prepare_tx(r1_address, c2_address, "Hi again c2")
recver = c2.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful and recver.was_successful)

# The address cache table on r1 should have both c1 and c2.

addr_cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))
verify(len(addr_cache_table) == 2)

for entry in addr_cache_table:
    if entry.address == c2_address:
        # Entry for c2 should point towards r3
        verify(entry.rloc16 == r3_rloc)
    elif entry.address == c3_address:
        # Entry for c3 should still point towards c3
        verify(entry.rloc16 == c3_rloc)
    else:
        raise (wpan.VerifyError("Unknown entry in the address cache table"))

# Force c2 to switch its parent from r3 to r2

c2.set(
    wpan.WPAN_CHILD_SUPERVISION_CHECK_TIMEOUT,
    str(CHILD_SUPERVISION_CHECK_TIMEOUT),
)
r2.set(wpan.WPAN_CHILD_SUPERVISION_INTERVAL, str(PARENT_SUPERVISION_INTERVAL))

r3.un_allowlist_node(c2)
r2.allowlist_node(c2)
c2.allowlist_node(r2)

# Wait for c2 to detach from r3 and attach to r2.
#
# Upon re-attach, previous parent r3 is notified and should remove c2 from
# its child table.


def check_c2_is_removed_from_r3_child_table():
    child_table = wpan.parse_list(r3.get(wpan.WPAN_THREAD_CHILD_TABLE))
    verify(len(child_table) == 1)


wpan.verify_within(check_c2_is_removed_from_r3_child_table, REATTACH_WAIT_TIME)

# Verify that both c2 is a child of r2

child_table = wpan.parse_list(r2.get(wpan.WPAN_THREAD_CHILD_TABLE))
verify(len(child_table) == 1)

# New network topology
#
#     r1 ---- r2 ---- r3
#     |       |       |
#     |       |       |
#     c1      c2(s)   c3

# Send a UDP message from c2 to c1.
# This message will be forwarded by r1 to its FED child c1.

sender = c2.prepare_tx(c2_address, c1_address, "Hi c1 child of r1")
recver = c1.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful and recver.was_successful)

# r1 upon receiving and forwarding the message from c2 (through r2 now) should
# update its address cache table for c2 (address cache update through snooping).
#
# verify that the address cache table is updated correctly.

addr_cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))
verify(len(addr_cache_table) == 2)

for entry in addr_cache_table:
    if entry.address == c2_address:
        # Entry for c2's address should now point to r2
        verify(entry.rloc16 == r2_rloc)
    elif entry.address == c3_address:
        # Entry for c3's address should still point to c3
        verify(entry.rloc16 == c3_rloc)
    else:
        raise (wpan.VerifyError("Unknown entry in the address cache table"))

# Force c2 to switch its parent from r2 to r3

CHILD_SUPERVISION_CHECK_TIMEOUT = 2
PARENT_SUPERVISION_INTERVAL = 1

REATTACH_WAIT_TIME = CHILD_SUPERVISION_CHECK_TIMEOUT / speedup + 6

c2.set(
    wpan.WPAN_CHILD_SUPERVISION_CHECK_TIMEOUT,
    str(CHILD_SUPERVISION_CHECK_TIMEOUT),
)
r3.set(wpan.WPAN_CHILD_SUPERVISION_INTERVAL, str(PARENT_SUPERVISION_INTERVAL))

r2.un_allowlist_node(c2)
r3.allowlist_node(c2)
c2.allowlist_node(r3)

# Wait for c2 to detach from r2 and attach to r3.
#
# Upon re-attach, previous parent r2 is notified and should remove c2 from
# its child table.


def check_c2_is_removed_from_r2_child_table():
    child_table = wpan.parse_list(r2.get(wpan.WPAN_THREAD_CHILD_TABLE))
    verify(len(child_table) == 0)


wpan.verify_within(check_c2_is_removed_from_r2_child_table, REATTACH_WAIT_TIME)

# Verify that both c2, c3 are children of r3

child_table = wpan.parse_list(r3.get(wpan.WPAN_THREAD_CHILD_TABLE))
verify(len(child_table) == 2)

# New network topology
#
#     r1 ---- r2 ---- r3
#     |               /\
#     |              /  \
#     c1           c2(s) c3

# Send a UDP message from c2 to c1.
# This message will be forwarded by r1 to its FED child c1.

sender = c2.prepare_tx(c2_address, c1_address, "Hi c1 child of r1")
recver = c1.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful and recver.was_successful)

# r1 upon receiving and forwarding the message from c2 (through r2 now) should
# update its address cache table for c2 (address cache update through snooping).
#
# verify that the address cache table is updated correctly.

addr_cache_table = wpan.parse_address_cache_table_result(r1.get(wpan.WPAN_THREAD_ADDRESS_CACHE_TABLE))
verify(len(addr_cache_table) == 2)

for entry in addr_cache_table:
    if entry.address == c2_address:
        # Entry for c2's address should now point to r3
        verify(entry.rloc16 == r3_rloc)
    elif entry.address == c3_address:
        # Entry for c3's address should still point to c3
        verify(entry.rloc16 == c3_rloc)
    else:
        raise (wpan.VerifyError("Unknown entry in the address cache table"))

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
