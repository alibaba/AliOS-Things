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
# Test description:
#
# Traffic over multi-hop in a network with chain topology
#
#       r1 ----- r2 ---- r3 ----- r4
#       /\       |       |        /\
#      /  \      |       |       /  \
#    fed1 sed1  sed2    sed3   sed4 fed4
#
#
# Traffic flow:
#  - From first router to last router
#  - From SED child of last router to SED child of first router
#  - From FED child of first router to FED child of last router
#
# The test verifies the following:
# - Verifies Address Query process to routers and FEDs.
# - Verifies Mesh Header frame forwarding over multiple routers.
# - Verifies forwarding of large IPv6 messages (1000 bytes) requiring lowpan fragmentation.

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

NUM_ROUTERS = 4

routers = []
sed_children = []
fed_children = []

for index in range(NUM_ROUTERS):
    routers.append(wpan.Node())
    sed_children.append(wpan.Node())

fed_children.append(wpan.Node())
fed_children.append(wpan.Node())

all_nodes = routers + sed_children + fed_children

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#
#       r1 ----- r2 ---- r3 ----- r4
#       /\       |       |        /\
#      /  \      |       |       /  \
#    fed1 sed1  sed2    sed3   sed4 fed4

# Allowlist routers with their corresponding sleepy children

for index in range(0, NUM_ROUTERS):
    routers[index].allowlist_node(sed_children[index])
    sed_children[index].allowlist_node(routers[index])

# Allowlist a FED child for the first and last routers

routers[0].allowlist_node(fed_children[0])
fed_children[0].allowlist_node(routers[0])

routers[-1].allowlist_node(fed_children[-1])
fed_children[-1].allowlist_node(routers[-1])

# While list routers at [index-1] and [index]

for index in range(1, NUM_ROUTERS):
    routers[index - 1].allowlist_node(routers[index])
    routers[index].allowlist_node(routers[index - 1])

routers[0].form("multi-hop")
sed_children[0].join_node(routers[0], wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
sed_children[0].set(wpan.WPAN_POLL_INTERVAL, '500')

for index in range(1, NUM_ROUTERS):
    routers[index].join_node(routers[index - 1], wpan.JOIN_TYPE_ROUTER)
    sed_children[index].join_node(routers[index], wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
    sed_children[index].set(wpan.WPAN_POLL_INTERVAL, '500')

fed_children[0].join_node(routers[0], wpan.JOIN_TYPE_END_DEVICE)
fed_children[-1].join_node(routers[-1], wpan.JOIN_TYPE_END_DEVICE)

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

NUM_MSGS = 3
MSG_LENS = [40, 100, 400, 800, 1000]

# Wait till first router has either established a link or
# has a valid "next hop" towards all other routers.

ROUTER_TABLE_WAIT_TIME = 60 / speedup + 5
INVALID_ROUTER_ID = 63

r1_rloc = int(routers[0].get(wpan.WPAN_THREAD_RLOC16), 16)


def check_r1_router_table():
    router_table = wpan.parse_router_table_result(routers[0].get(wpan.WPAN_THREAD_ROUTER_TABLE))
    verify(len(router_table) == NUM_ROUTERS)
    for entry in router_table:
        verify(entry.rloc16 == r1_rloc or entry.is_link_established() or entry.next_hop != INVALID_ROUTER_ID)


wpan.verify_within(check_r1_router_table, ROUTER_TABLE_WAIT_TIME)

# Send from the first router in the chain to the last one.

src = routers[0].get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]
dst = routers[-1].get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]

for msg_length in MSG_LENS:
    sender = routers[0].prepare_tx(src, dst, msg_length, NUM_MSGS)
    recver = routers[-1].prepare_rx(sender)
    wpan.Node.perform_async_tx_rx()
    verify(sender.was_successful)
    verify(recver.was_successful)

# Send from the SED child of the last router to the SED child of the first
# router.

src = sed_children[-1].get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]
dst = sed_children[0].get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]

for msg_length in MSG_LENS:
    sender = sed_children[-1].prepare_tx(src, dst, msg_length, NUM_MSGS)
    recver = sed_children[0].prepare_rx(sender)
    wpan.Node.perform_async_tx_rx()
    verify(sender.was_successful)
    verify(recver.was_successful)

# Send from the FED child of the first router to the FED child of the last
# router.

src = fed_children[0].get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]
dst = fed_children[-1].get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]

for msg_length in MSG_LENS:
    sender = fed_children[0].prepare_tx(src, dst, msg_length, NUM_MSGS)
    recver = fed_children[-1].prepare_rx(sender)
    wpan.Node.perform_async_tx_rx()
    verify(sender.was_successful)
    verify(recver.was_successful)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
