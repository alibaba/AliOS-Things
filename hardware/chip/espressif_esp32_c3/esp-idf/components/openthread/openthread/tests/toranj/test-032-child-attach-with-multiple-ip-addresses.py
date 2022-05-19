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

import time
import wpan
from wpan import verify

# -----------------------------------------------------------------------------------------------------------------------
# Test description:
#
# This test covers the situation for SED child (re)attaching to a parent with multiple IPv6 addresses present on the
# child.
#
# Network topology
#
#   leader ---- parent
#                 |
#                 |
#              child (sleepy)
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Utility functions


def verify_address(node_list, prefix):
    """
    This function verifies that all nodes in the `node_list` contain an IPv6 address with the given `prefix`.
    """
    for node in node_list:
        all_addrs = wpan.parse_list(node.get(wpan.WPAN_IP6_ALL_ADDRESSES))
        verify(any([addr.startswith(prefix[:-1]) for addr in all_addrs]))


# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

leader = wpan.Node()
parent = wpan.Node()
child = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology

leader.form('multi-addr-test')

leader.allowlist_node(parent)
parent.allowlist_node(leader)
parent.join_node(leader, wpan.JOIN_TYPE_ROUTER)

parent.allowlist_node(child)
child.allowlist_node(parent)

child.join_node(parent, node_type=wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
child.set(wpan.WPAN_POLL_INTERVAL, '400')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

WAIT_TIME = 5
CHILD_SUPERVISION_CHECK_TIMEOUT = 1

prefix1 = 'fd00:1::'
prefix2 = 'fd00:2::'
prefix3 = 'fd00:3::'
prefix4 = 'fd00:4::'

# Add 4 prefixes (all with SLAAC bit set).
leader.add_prefix(prefix1, on_mesh=True, slaac=True, configure=True)
leader.add_prefix(prefix2, on_mesh=True, slaac=True, configure=True)
leader.add_prefix(prefix3, on_mesh=True, slaac=True, configure=True)
leader.add_prefix(prefix4, on_mesh=True, slaac=True, configure=True)

# Verify that the sleepy child gets all 4 SLAAC addresses.


def check_addresses_on_child():
    verify_address([child], prefix1)
    verify_address([child], prefix2)
    verify_address([child], prefix3)
    verify_address([child], prefix4)


wpan.verify_within(check_addresses_on_child, WAIT_TIME)

# Remove child from parent's allowlist
parent.remove(wpan.WPAN_MAC_ALLOWLIST_ENTRIES, child.get(wpan.WPAN_EXT_ADDRESS)[1:-1])

# Enable supervision check on child, this ensures that child is detached soon.
child.set(
    wpan.WPAN_CHILD_SUPERVISION_CHECK_TIMEOUT,
    str(CHILD_SUPERVISION_CHECK_TIMEOUT),
)

# Wait for child to get detached.


def check_child_is_detached():
    verify(not child.is_associated())


wpan.verify_within(check_child_is_detached, WAIT_TIME)

# Now reset parent and wait for it to be associated.
parent.reset()


def check_parent_is_associated():
    verify(parent.is_associated())


wpan.verify_within(check_parent_is_associated, WAIT_TIME)

# Now verify that child is indeed getting attached back.


def check_child_is_associated():
    verify(child.is_associated())


wpan.verify_within(check_child_is_associated, WAIT_TIME)

# Any finally check that we see all the child addresses in the parent's
# child table.


def check_child_addresses_on_parent():
    child_addrs = parent.get(wpan.WPAN_THREAD_CHILD_TABLE_ADDRESSES)
    verify(child_addrs.find(prefix1) > 0)
    verify(child_addrs.find(prefix2) > 0)
    verify(child_addrs.find(prefix3) > 0)
    verify(child_addrs.find(prefix4) > 0)


wpan.verify_within(check_child_addresses_on_parent, WAIT_TIME)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Check the child recovery after a parent reset using quick re-attach
# ("Child Update" exchange).

# Disable supervision check on the child.
child.set(wpan.WPAN_CHILD_SUPERVISION_CHECK_TIMEOUT, '1000')
child.set(wpan.WPAN_POLL_INTERVAL, '10000')
time.sleep(0.1)

# We use the "stat:ncp" wpantund property to verify that child does not
# get detached.
child_num_state_changes = len(wpan.parse_list(child.get("stat:ncp")))

# Reset parent and wait for it to be associated.
parent.reset()
wpan.verify_within(check_parent_is_associated, WAIT_TIME)

child.set(wpan.WPAN_POLL_INTERVAL, '100')

# Verify that we again see all the child addresses in the parent's child table.
# Note that child should register its addresses using "Child Update
# Request" exchange.
wpan.verify_within(check_child_addresses_on_parent, WAIT_TIME)

# Verify that there was no state change on child.
verify(child_num_state_changes == len(wpan.parse_list(child.get("stat:ncp"))))

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
