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
# Test description: Neighbor table
#
# - Network with NUM_ROUTERS(= 2) routers, all within range of each other.
# - The first router has NUM_CHILDREN(= 1) sleepy children attached to it.
# - The test verifies that all children and routers are observed in the "Thread:NeighborTable" of the first router.
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

NUM_ROUTERS = 2
NUM_CHILDREN = 1

routers = []
for num in range(NUM_ROUTERS):
    routers.append(wpan.Node())

children = []
for num in range(NUM_CHILDREN):
    children.append(wpan.Node())

# end-device per router used for quick promotion to router role
ed = [0]
for num in range(1, NUM_ROUTERS):
    ed.append(wpan.Node())

all_nodes = routers + children + ed

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#

# allowlist all routers with one another
for i in range(NUM_ROUTERS):
    for j in range(NUM_ROUTERS):
        if i != j:
            routers[i].allowlist_node(routers[j])

# All children should attach to routers[0]
for num in range(NUM_CHILDREN):
    children[num].allowlist_node(routers[0])
    routers[0].allowlist_node(children[num])

# allowlist the end-device ed with its corresponding router
for num in range(1, NUM_ROUTERS):
    ed[num].allowlist_node(routers[num])
    routers[num].allowlist_node(ed[num])

routers[0].form("neigh-table")

for router in routers[1:]:
    router.join_node(routers[0], wpan.JOIN_TYPE_ROUTER)

for num in range(1, NUM_ROUTERS):
    ed[num].join_node(routers[num], wpan.JOIN_TYPE_END_DEVICE)

for num in range(NUM_CHILDREN):
    children[num].join_node(routers[0], wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
    children[num].set(wpan.WPAN_POLL_INTERVAL, '300')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

for router in routers[1:]:
    verify(router.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_ROUTER)

# Get and parse the neighbor table on routers[0].
neighbor_table = wpan.parse_neighbor_table_result(routers[0].get(wpan.WPAN_THREAD_NEIGHBOR_TABLE))

verify(len(neighbor_table) == NUM_ROUTERS - 1 + NUM_CHILDREN)

# Verify that all children are seen in the neighbor table
for child in children:
    ext_addr = child.get(wpan.WPAN_EXT_ADDRESS)[1:-1]
    for entry in neighbor_table:
        if entry.ext_address == ext_addr:
            break
    else:
        raise wpan.VerifyError('Failed to find a child entry for extended address {} in table'.format(ext_addr))

    verify(int(entry.rloc16, 16) == int(child.get(wpan.WPAN_THREAD_RLOC16), 16))
    verify(entry.is_rx_on_when_idle() is False)
    verify(entry.is_ftd() is False)
    verify(entry.is_child())

# Verify that all other routers are seen in the neighbor table
for router in routers[1:]:
    ext_addr = router.get(wpan.WPAN_EXT_ADDRESS)[1:-1]
    for entry in neighbor_table:
        if entry.ext_address == ext_addr:
            break
    else:
        raise wpan.VerifyError('Failed to find a router entry for extended address {} in table'.format(ext_addr))

    verify(int(entry.rloc16, 16) == int(router.get(wpan.WPAN_THREAD_RLOC16), 16))
    verify(entry.is_rx_on_when_idle())
    verify(entry.is_ftd())
    verify(entry.is_child() is False)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
