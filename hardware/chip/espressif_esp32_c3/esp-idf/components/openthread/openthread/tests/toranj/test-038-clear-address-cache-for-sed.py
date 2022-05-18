#!/usr/bin/env python3
#
#  Copyright (c) 2019, The OpenThread Authors.
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
# This test verifies that address cache entry associated with a SED child
# addresses is removed from new parent node ensuring we would not have a
# routing loop.

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
c = wpan.Node()
c3 = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#
#   r3 ---- r1 ---- r2
#   |               |
#   |               |
#   c3              c
#
# c is initially attached to r2 but it switches parent during test to r1 and then r3
# c3 is just added to make sure r3 become router quickly (not involved in test)

PREFIX = "fd00:1234::"
POLL_INTERVAL = 400

r1.form("addr-cache")

r1.add_prefix(PREFIX, stable=True, on_mesh=True, slaac=True, preferred=True)

r1.allowlist_node(r2)
r2.allowlist_node(r1)
r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)

c.set(wpan.WPAN_POLL_INTERVAL, str(POLL_INTERVAL))
c.allowlist_node(r2)
r2.allowlist_node(c)
c.join_node(r2, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)

r3.allowlist_node(r1)
r1.allowlist_node(r3)
r3.join_node(r1, wpan.JOIN_TYPE_ROUTER)

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
verify(c.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_SLEEPY_END_DEVICE)
verify(c3.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_END_DEVICE)

r1_address = r1.find_ip6_address_with_prefix(PREFIX)
r2_address = r2.find_ip6_address_with_prefix(PREFIX)
c_address = c.find_ip6_address_with_prefix(PREFIX)

# Send a single UDP message from r1 to c
#
# This adds an address cache entry on r1 for c pointing to r2 (the current parent of c).

sender = r1.prepare_tx(r1_address, c_address, "Hi from r1 to c")
recver = c.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful and recver.was_successful)

# Force c to switch its parent from r2 to r1
#
#   r3 ---- r1 ---- r2
#   |       |
#   |       |
#   c3      c

CHILD_SUPERVISION_CHECK_TIMEOUT = 2
PARENT_SUPERVISION_INTERVAL = 1

REATTACH_WAIT_TIME = CHILD_SUPERVISION_CHECK_TIMEOUT / speedup + 6

c.set(wpan.WPAN_CHILD_SUPERVISION_CHECK_TIMEOUT, str(CHILD_SUPERVISION_CHECK_TIMEOUT))
r2.set(wpan.WPAN_CHILD_SUPERVISION_INTERVAL, str(PARENT_SUPERVISION_INTERVAL))
r1.set(wpan.WPAN_CHILD_SUPERVISION_INTERVAL, str(PARENT_SUPERVISION_INTERVAL))
r3.set(wpan.WPAN_CHILD_SUPERVISION_INTERVAL, str(PARENT_SUPERVISION_INTERVAL))

r2.un_allowlist_node(c)
r1.allowlist_node(c)
c.allowlist_node(r1)

# Wait for c to detach from r2 and attach to r1.


def check_c_is_removed_from_r2_child_table():
    child_table = wpan.parse_list(r2.get(wpan.WPAN_THREAD_CHILD_TABLE))
    verify(len(child_table) == 0)


wpan.verify_within(check_c_is_removed_from_r2_child_table, REATTACH_WAIT_TIME)

# check that c is now a child of r1
child_table = wpan.parse_list(r1.get(wpan.WPAN_THREAD_CHILD_TABLE))
verify(len(child_table) == 1)

# Send a single UDP message from r2 to c
#
# This adds an address cache entry on r2 for c pointing to r1 (the current parent of c).

sender = r2.prepare_tx(r2_address, c_address, "Hi from r2 to c")
recver = c.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful and recver.was_successful)

# Force c to switch its parent from r1 to r3
#
#   r3 ---- r1 ---- r2
#   | \
#   |  \
#   c3  c

r1.un_allowlist_node(c)
r3.allowlist_node(c)
c.allowlist_node(r3)

# Wait for c to detach from r1 and attach to r3.


def check_c_is_removed_from_r1_child_table():
    child_table = wpan.parse_list(r1.get(wpan.WPAN_THREAD_CHILD_TABLE))
    verify(len(child_table) == 0)


wpan.verify_within(check_c_is_removed_from_r1_child_table, REATTACH_WAIT_TIME)

# check that c is now a child of r3 (r3 should have two child, c and c3)
child_table = wpan.parse_list(r3.get(wpan.WPAN_THREAD_CHILD_TABLE))
verify(len(child_table) == 2)

# Send a single UDP message from r1 to c
#
# If the r1 address cache entry is not cleared when c attached to r1,
# r1 will still have an entry pointing to r2, and r2 will have an entry
# pointing to r1, thus creating a loop (the msg will not be delivered to r3)

sender = r1.prepare_tx(r1_address, c_address, "Hi from r1 to c")
recver = c.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful and recver.was_successful)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
