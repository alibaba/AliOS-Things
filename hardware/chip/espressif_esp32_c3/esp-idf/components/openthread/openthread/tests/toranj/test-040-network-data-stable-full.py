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

import wpan
from wpan import verify

# -----------------------------------------------------------------------------------------------------------------------
# Test description: Network Data update and version changes (stable only vs. full version).
#
# Network topology
#
#      leader
#      /  |  \
#     /   |   \
#    /    |    \
#   c1    c2    c3
#
#
# c3 is sleepy-end node and also configured to request stable Network Data only
#
# Test covers the following steps:
# - Adding/removing prefixes (stable or temporary) on c1
# - Verifying that Network Data is updated on all nodes
# - Ensuring correct update to version and stable version

# The above steps are repeated over many different situations:
# - Where the same prefixes are also added by other nodes
# - Or the same prefixes are added as off-mesh routes by other nodes
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))


def verify_prefix(
    node_list,
    prefix,
    rloc16,
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
    This function verifies that the `prefix` is present on all the nodes in the `node_list`. It also verifies that the
    `prefix` is associated with the given `rloc16` (as an integer).
    """
    for node in node_list:
        prefixes = wpan.parse_on_mesh_prefix_result(node.get(wpan.WPAN_THREAD_ON_MESH_PREFIXES))
        for p in prefixes:
            if (p.prefix == prefix and p.origin == "ncp" and int(p.rloc16(), 0) == rloc16):
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


def verify_no_prefix(node_list, prefix, rloc16):
    """
    This function verifies that none of the nodes in `node_list` contains the on-mesh `prefix` associated with the
    given `rloc16`.
    """
    for node in node_list:
        prefixes = wpan.parse_on_mesh_prefix_result(node.get(wpan.WPAN_THREAD_ON_MESH_PREFIXES))
        for p in prefixes:
            if (p.prefix == prefix and p.origin == "ncp" and int(p.rloc16(), 0) == rloc16):
                raise wpan.VerifyError("Did find prefix {} with rloc {} on node {}".format(prefix, hex(rloc16), node))


# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 25
wpan.Node.set_time_speedup_factor(speedup)

leader = wpan.Node()
c1 = wpan.Node()
c2 = wpan.Node()
c3 = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#

leader.form("bloodborne")  # "fear the old blood"

c1.join_node(leader, wpan.JOIN_TYPE_END_DEVICE)
c2.join_node(leader, wpan.JOIN_TYPE_END_DEVICE)
c3.join_node(leader, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
c3.set(wpan.WPAN_POLL_INTERVAL, '400')

# Clear the "full network data" flag on c3.
c3.set(wpan.WPAN_THREAD_DEVICE_MODE, '-')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

WAIT_TIME = 15

prefix1 = "fd00:1::"
prefix2 = "fd00:2::"
prefix3 = "fd00:3::"

leader_rloc = int(leader.get(wpan.WPAN_THREAD_RLOC16), 0)
c1_rloc = int(c1.get(wpan.WPAN_THREAD_RLOC16), 0)
c2_rloc = int(c2.get(wpan.WPAN_THREAD_RLOC16), 0)
no_rloc = 0xfffe


def test_prefix_add_remove():
    # Tests adding and removing stable and temporary prefixes on r1
    # Verifies that all nodes in network do see the updates and that
    # Network Data version and stable version are updated correctly.

    old_version = int(leader.get(wpan.WPAN_THREAD_NETWORK_DATA_VERSION), 0)
    old_stable_version = int(leader.get(wpan.WPAN_THREAD_STABLE_NETWORK_DATA_VERSION), 0)

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Add a stable prefix and check all nodes see the prefix
    c1.add_prefix(prefix1, stable=True)

    def check_prefix1():
        verify_prefix(
            [leader, c1, c2],
            prefix1,
            c1_rloc,
            stable=True,
        )

        verify_prefix(
            [c3],
            prefix1,
            no_rloc,
            stable=True,
        )

    wpan.verify_within(check_prefix1, WAIT_TIME)

    new_version = int(leader.get(wpan.WPAN_THREAD_NETWORK_DATA_VERSION), 0)
    new_stable_version = int(leader.get(wpan.WPAN_THREAD_STABLE_NETWORK_DATA_VERSION), 0)

    verify(new_version == ((old_version + 1) % 256))
    verify(new_stable_version == ((old_stable_version + 1) % 256))

    old_version = new_version
    old_stable_version = new_stable_version

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Add prefix 2 as temp (not stable)

    c1.add_prefix(prefix2, stable=False)

    def check_prefix2():
        verify_prefix(
            [leader, c1, c2],
            prefix2,
            c1_rloc,
            stable=False,
        )

    wpan.verify_within(check_prefix1, WAIT_TIME)
    wpan.verify_within(check_prefix2, WAIT_TIME)

    new_version = int(leader.get(wpan.WPAN_THREAD_NETWORK_DATA_VERSION), 0)
    new_stable_version = int(leader.get(wpan.WPAN_THREAD_STABLE_NETWORK_DATA_VERSION), 0)

    verify(new_version == ((old_version + 1) % 256))
    verify(new_stable_version == old_stable_version)

    old_version = new_version
    old_stable_version = new_stable_version

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Remove prefix 1

    c1.remove_prefix(prefix1)

    def check_no_prefix1():
        verify_no_prefix([leader, c1, c2], prefix1, c1_rloc)

    wpan.verify_within(check_no_prefix1, WAIT_TIME)
    wpan.verify_within(check_prefix2, WAIT_TIME)

    new_version = int(leader.get(wpan.WPAN_THREAD_NETWORK_DATA_VERSION), 0)
    new_stable_version = int(leader.get(wpan.WPAN_THREAD_STABLE_NETWORK_DATA_VERSION), 0)

    verify(new_version == ((old_version + 1) % 256))
    verify(new_stable_version == ((old_stable_version + 1) % 256))

    old_version = new_version
    old_stable_version = new_stable_version

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    # Remove prefix 2

    c1.remove_prefix(prefix2)

    def check_no_prefix2():
        verify_no_prefix([leader, c1, c2], prefix2, c1_rloc)

    wpan.verify_within(check_no_prefix1, WAIT_TIME)
    wpan.verify_within(check_no_prefix2, WAIT_TIME)

    new_version = int(leader.get(wpan.WPAN_THREAD_NETWORK_DATA_VERSION), 0)
    new_stable_version = int(leader.get(wpan.WPAN_THREAD_STABLE_NETWORK_DATA_VERSION), 0)

    verify(new_version == ((old_version + 1) % 256))
    verify(new_stable_version == old_stable_version)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Repeat the `test_prefix_add_remove()` under different situations
# where same prefix is added/removed by other nodes in the network
# or added as an off-mesh route.

num_routes = 0

test_prefix_add_remove()

leader.add_prefix(prefix1, stable=False)
test_prefix_add_remove()

leader.add_prefix(prefix2, stable=True)
test_prefix_add_remove()

leader.remove_prefix(prefix1)
test_prefix_add_remove()

leader.remove_prefix(prefix2)
test_prefix_add_remove()

leader.add_route(prefix1, stable=False)
num_routes = num_routes + 1
test_prefix_add_remove()
verify(len(wpan.parse_list(c2.get(wpan.WPAN_THREAD_OFF_MESH_ROUTES))) == num_routes)

leader.add_route(prefix2, stable=True)
num_routes = num_routes + 1
test_prefix_add_remove()
verify(len(wpan.parse_list(c2.get(wpan.WPAN_THREAD_OFF_MESH_ROUTES))) == num_routes)

leader.add_prefix(prefix3, stable=True)
test_prefix_add_remove()
verify(len(wpan.parse_list(c2.get(wpan.WPAN_THREAD_OFF_MESH_ROUTES))) == num_routes)

leader.remove_route(prefix2)
num_routes = num_routes - 1
test_prefix_add_remove()
verify(len(wpan.parse_list(c2.get(wpan.WPAN_THREAD_OFF_MESH_ROUTES))) == num_routes)

leader.remove_route(prefix1)
num_routes = num_routes - 1
test_prefix_add_remove()
verify(len(wpan.parse_list(c2.get(wpan.WPAN_THREAD_OFF_MESH_ROUTES))) == num_routes)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
