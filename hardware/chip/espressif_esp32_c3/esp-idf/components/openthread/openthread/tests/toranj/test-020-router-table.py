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
# Test description: Router table
#
# Verify router table entries on a network with 4 routers:
# {r1, r2, r3} forming a loop with r4 connecting to r3.
#

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
r4 = wpan.Node()
c4 = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#
#
#     r1 ------ r2
#      \        /
#       \      /
#        \    /
#          r3 _____ r4
#
#

r1.form("route-table")

r1.allowlist_node(r2)
r2.allowlist_node(r1)
r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)

r2.allowlist_node(r3)
r3.allowlist_node(r2)
r3.join_node(r2, wpan.JOIN_TYPE_ROUTER)

r3.allowlist_node(r1)
r1.allowlist_node(r3)

r3.allowlist_node(r4)
r4.allowlist_node(r3)
r4.join_node(r3, wpan.JOIN_TYPE_ROUTER)

# c4 is attached to r4 so that it quickly gets promoted to a router role.
c4.allowlist_node(r4)
r4.allowlist_node(c4)
c4.join_node(r4, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
c4.set(wpan.WPAN_POLL_INTERVAL, '2000')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation
#

verify(r1.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_LEADER)
verify(r2.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_ROUTER)
verify(r3.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_ROUTER)
verify(r4.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_ROUTER)

r1_id = int(r1.get(wpan.WPAN_THREAD_ROUTER_ID), 0)
r2_id = int(r2.get(wpan.WPAN_THREAD_ROUTER_ID), 0)
r3_id = int(r3.get(wpan.WPAN_THREAD_ROUTER_ID), 0)
r4_id = int(r4.get(wpan.WPAN_THREAD_ROUTER_ID), 0)

r1_ext_addr = r1.get(wpan.WPAN_EXT_ADDRESS)[1:-1]
r2_ext_addr = r2.get(wpan.WPAN_EXT_ADDRESS)[1:-1]
r3_ext_addr = r3.get(wpan.WPAN_EXT_ADDRESS)[1:-1]
r4_ext_addr = r4.get(wpan.WPAN_EXT_ADDRESS)[1:-1]

r1_rloc = int(r1.get(wpan.WPAN_THREAD_RLOC16), 16)
r2_rloc = int(r2.get(wpan.WPAN_THREAD_RLOC16), 16)
r3_rloc = int(r3.get(wpan.WPAN_THREAD_RLOC16), 16)
r4_rloc = int(r4.get(wpan.WPAN_THREAD_RLOC16), 16)

WAIT_TIME = 30 / speedup + 5

INVALID_ROUTER_ID = 63


def check_r1_router_table():
    router_table = wpan.parse_router_table_result(r1.get(wpan.WPAN_THREAD_ROUTER_TABLE))
    verify(len(router_table) == 4)
    for entry in router_table:
        if entry.rloc16 == r1_rloc:
            pass
        elif entry.rloc16 == r2_rloc:
            # r1 should be directly connected to r2.
            verify(entry.is_link_established())
            verify(entry.ext_address == r2_ext_addr)
        elif entry.rloc16 == r3_rloc:
            # r1 should be directly connected to r3.
            verify(entry.is_link_established())
            verify(entry.ext_address == r3_ext_addr)
        elif entry.rloc16 == r4_rloc:
            # r1's next hop towards r4 should be through r3.
            verify(not entry.is_link_established())
            verify(entry.next_hop == r3_id)
        else:
            raise (wpan.VerifyError("unknown entry in the router table of r1"))


wpan.verify_within(check_r1_router_table, WAIT_TIME)


def check_r3_router_table():
    router_table = wpan.parse_router_table_result(r3.get(wpan.WPAN_THREAD_ROUTER_TABLE))
    verify(len(router_table) == 4)
    for entry in router_table:
        if entry.rloc16 == r1_rloc:
            # r3 should be directly connected to r1.
            verify(entry.is_link_established())
            verify(entry.ext_address == r1_ext_addr)
        elif entry.rloc16 == r2_rloc:
            # r3 should be directly connected to r2.
            verify(entry.is_link_established())
            verify(entry.ext_address == r2_ext_addr)
        elif entry.rloc16 == r3_rloc:
            pass
        elif entry.rloc16 == r4_rloc:
            # r3 should be directly connected to r4.
            verify(entry.is_link_established())
            verify(entry.ext_address == r4_ext_addr)
        else:
            raise (wpan.VerifyError("unknown entry in the router table of r3"))


wpan.verify_within(check_r3_router_table, WAIT_TIME)


def check_r4_router_table():
    router_table = wpan.parse_router_table_result(r4.get(wpan.WPAN_THREAD_ROUTER_TABLE))
    verify(len(router_table) == 4)
    for entry in router_table:
        if entry.rloc16 == r1_rloc:
            # r4's next hop towards r1 should be through r3.
            verify(not entry.is_link_established())
            verify(entry.next_hop == r3_id)
        elif entry.rloc16 == r2_rloc:
            # r4's next hop towards r2 should be through r3.
            verify(not entry.is_link_established())
            verify(entry.next_hop == r3_id)
        elif entry.rloc16 == r3_rloc:
            # r4 should be directly connected to r3.
            verify(entry.is_link_established())
            verify(entry.ext_address == r3_ext_addr)
        elif entry.rloc16 == r4_rloc:
            pass
        else:
            raise (wpan.VerifyError("unknown entry in the router table of r4"))


wpan.verify_within(check_r4_router_table, WAIT_TIME)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
