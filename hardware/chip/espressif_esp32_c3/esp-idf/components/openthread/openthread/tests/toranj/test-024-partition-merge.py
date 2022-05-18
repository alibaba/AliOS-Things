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

import time
import wpan
from wpan import verify

# -----------------------------------------------------------------------------------------------------------------------
# Test description: Partition formation and merge
#
# Network Topology:
#
#      r1 ---- / ---- r2
#      |       \      |
#      |       /      |
#      c1      \      c2
#
#
# Test covers the following situations:
#
# - r2 forming its own partition when it can no longer hear r1
# - Partitions merging into one once r1 and r2 can talk again
# - Adding on-mesh prefixes on each partition and ensuring after
#   merge the info in combined.
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Utility functions


def verify_prefix(
    node_list,
    prefix,
    prefix_len=64,
    stable=True,
    priority='med',
    on_mesh=False,
    slaac=False,
    dhcp=False,
    configure=False,
    default_route=False,
    preferred=False,
):
    """
    This function verifies that the `prefix` is present on all the nodes in the `node_list`.
    """
    for node in node_list:
        prefixes = wpan.parse_on_mesh_prefix_result(node.get(wpan.WPAN_THREAD_ON_MESH_PREFIXES))
        for p in prefixes:
            if p.prefix == prefix:
                verify(int(p.prefix_len) == prefix_len)
                verify(p.is_stable() == stable)
                verify(p.is_on_mesh() == on_mesh)
                verify(p.is_def_route() == default_route)
                verify(p.is_slaac() == slaac)
                verify(p.is_dhcp() == dhcp)
                verify(p.is_config() == configure)
                verify(p.is_preferred() == preferred)
                verify(p.priority == priority)
                break
        else:
            raise wpan.VerifyError("Did not find prefix {} on node {}".format(prefix, node))


# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 25
wpan.Node.set_time_speedup_factor(speedup)

r1 = wpan.Node()
r2 = wpan.Node()
c1 = wpan.Node()
c2 = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#

r1.form("partition")

r1.allowlist_node(r2)
r2.allowlist_node(r1)
r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)

c1.allowlist_node(r1)
r1.allowlist_node(c1)
c1.join_node(r1, wpan.JOIN_TYPE_END_DEVICE)

c2.allowlist_node(r2)
r2.allowlist_node(c2)
c2.join_node(r2, wpan.JOIN_TYPE_END_DEVICE)

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

short_wait = 6
long_wait = 15  # With speedup of 25, this emulates 375s (~6 min).

prefix1 = "fd00:1234::"
prefix2 = "fd00:abcd::"

time.sleep(1)

verify(r1.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_LEADER)
verify(r2.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_ROUTER)

# Now force the two routers to form their own partition
# by removing them from each other's allowlist table
r1.un_allowlist_node(r2)
r2.un_allowlist_node(r1)

# Add a prefix before r2 realizes it can not longer talk
# to leader (r1).
r2.add_prefix(prefix2)

# Check that r2 forms its own partition


def check_r2_become_leader():
    verify(r2.get(wpan.WPAN_NODE_TYPE) == wpan.NODE_TYPE_LEADER)


wpan.verify_within(check_r2_become_leader, long_wait)

# While we have two partition, add a prefix on r1
r1.add_prefix(prefix1)

# Update allowlist and wait for partitions to merge.
r1.allowlist_node(r2)
r2.allowlist_node(r1)


def check_partition_id_match():
    verify(r1.get(wpan.WPAN_PARTITION_ID) == r2.get(wpan.WPAN_PARTITION_ID))


wpan.verify_within(check_partition_id_match, long_wait)

# Check that partitions merged successfully


def check_r1_r2_roles():
    r1_type = r1.get(wpan.WPAN_NODE_TYPE)
    r2_type = r2.get(wpan.WPAN_NODE_TYPE)
    verify((r1_type == wpan.NODE_TYPE_LEADER and r2_type == wpan.NODE_TYPE_ROUTER) or
           (r2_type == wpan.NODE_TYPE_LEADER and r1_type == wpan.NODE_TYPE_ROUTER))


wpan.verify_within(check_r1_r2_roles, short_wait)

# Verify all nodes have both prefixes


def check_prefixes():
    verify_prefix([r1, r2, c1, c2], prefix1)
    verify_prefix([r1, r2, c1, c2], prefix2)


wpan.verify_within(check_prefixes, short_wait)

# Verify that the children stayed with their parents
verify(len(wpan.parse_list(r1.get(wpan.WPAN_THREAD_CHILD_TABLE))) == 1)
verify(len(wpan.parse_list(r2.get(wpan.WPAN_THREAD_CHILD_TABLE))) == 1)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
