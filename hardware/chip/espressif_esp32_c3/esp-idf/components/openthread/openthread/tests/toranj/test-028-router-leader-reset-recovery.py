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
# Test description: Verify sequential reset recovery of a router and leader
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Utility functions


def verify_neighbor_table(node, neighbors):
    """
    This function verifies that the neighbor table of a given `node` contains the node in the `neighbors` list.
    """
    neighbor_table = wpan.parse_neighbor_table_result(node.get(wpan.WPAN_THREAD_NEIGHBOR_TABLE))
    for neighbor in neighbors:
        ext_addr = neighbor.get(wpan.WPAN_EXT_ADDRESS)[1:-1]
        for entry in neighbor_table:
            if entry.ext_address == ext_addr:
                break
        else:
            raise wpan.VerifyError('Failed to find a neighbor entry for extended address {} in table'.format(ext_addr))


# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

r1 = wpan.Node()
r2 = wpan.Node()
c2 = wpan.Node()
# c2 is used to force r2 becoming router

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#

r1.form("RtrLderRst")

r1.allowlist_node(r2)
r2.allowlist_node(r1)
r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)

c2.allowlist_node(r2)
r2.allowlist_node(c2)
c2.join_node(r2, wpan.JOIN_TYPE_END_DEVICE)
c2.set(wpan.WPAN_POLL_INTERVAL, '8000')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

WAIT_INTERVAL = 6

# Check that r1 and r2 are present in each other's neighbor table


def check_neighbor_tables():
    verify_neighbor_table(r1, [r2])
    verify_neighbor_table(r2, [r1])


wpan.verify_within(check_neighbor_tables, WAIT_INTERVAL)

verify(r1.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_LEADER)
verify(r2.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_ROUTER)

# Reset r2 and wait for it to be associated.
r2.reset()


def check_r2_neighbor_table():
    verify(r2.is_associated())
    verify_neighbor_table(r2, [r1])


wpan.verify_within(check_r2_neighbor_table, WAIT_INTERVAL)
verify(r1.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_LEADER)
verify(r2.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_ROUTER)

# Now reset r1 and check that everything recover correctly.
r1.reset()


def check_r1_neighbor_table():
    verify(r1.is_associated())
    verify_neighbor_table(r1, [r2])


wpan.verify_within(check_r1_neighbor_table, WAIT_INTERVAL)
verify(r1.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_LEADER)
verify(r2.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_ROUTER)

wpan.verify_within(check_r2_neighbor_table, WAIT_INTERVAL)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
