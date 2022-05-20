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
# Test description: Network Data (on-mesh prefix) timeout and entry removal
#
# Network topology
#
#   r1 ----- r2
#            |
#            |
#            c2 (sleepy)
#
#
# Test covers the following steps:
#
# - Every node adds a unique on-mesh prefix.
# - Every node also adds a common on-mesh prefix (with different flags).
# - Verify that all the unique and common prefixes are present on all nodes are associated with correct RLOC16.
# - Remove `r2` from network (which removes `c2` as well) from Thread partition created by `r1`.
# - Verify that all on-mesh prefixes added by `r2` or `c2` (unique and common) are removed on `r1`.
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
    preferred=True,
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

r1 = wpan.Node()
r2 = wpan.Node()
c2 = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#
# Test topology:
#
#   r1 ----- r2
#            |
#            |
#            c2

r1.form("net-data")

r1.allowlist_node(r2)
r2.allowlist_node(r1)
r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)

r2.allowlist_node(c2)
c2.allowlist_node(r2)
c2.join_node(r2, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
c2.set(wpan.WPAN_POLL_INTERVAL, '400')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

common_prefix = "fd00:cafe::"
prefix1 = "fd00:1::"
prefix2 = "fd00:2::"
prefix3 = "fd00:3::"

wait_time = 15

# Each node adds its own prefix.
r1.add_prefix(prefix1, on_mesh=True, preferred=True, stable=True)
r2.add_prefix(prefix2, on_mesh=True, preferred=True, stable=True)
c2.add_prefix(prefix3, on_mesh=True, preferred=True, stable=True)

# `common_prefix` is added by all three nodes (with different flags)
r1.add_prefix(common_prefix, on_mesh=True, preferred=True, stable=False)
r2.add_prefix(common_prefix, on_mesh=True, preferred=True, stable=True)
c2.add_prefix(common_prefix, on_mesh=True, preferred=False, stable=True)

r1_rloc = int(r1.get(wpan.WPAN_THREAD_RLOC16), 0)
r2_rloc = int(r2.get(wpan.WPAN_THREAD_RLOC16), 0)
c2_rloc = int(c2.get(wpan.WPAN_THREAD_RLOC16), 0)


def check_prefixes():
    # Verify that all three `prefix1`, 'prefix2', and `prefix3` are present on all nodes
    # and respectively associated with `r1`, r2, and `r3` nodes.
    verify_prefix(
        [r1, r2, c2],
        prefix1,
        r1_rloc,
        on_mesh=True,
        preferred=True,
        stable=True,
    )
    verify_prefix(
        [r1, r2, c2],
        prefix2,
        r2_rloc,
        on_mesh=True,
        preferred=True,
        stable=True,
    )
    verify_prefix(
        [r1, r2, c2],
        prefix3,
        c2_rloc,
        on_mesh=True,
        preferred=True,
        stable=True,
    )

    # Verify the presence of `common_prefix` associated with each node (with
    # correct flags).
    verify_prefix(
        [r1, r2, c2],
        common_prefix,
        r1_rloc,
        on_mesh=True,
        preferred=True,
        stable=False,
    )
    verify_prefix(
        [r1, r2, c2],
        common_prefix,
        r2_rloc,
        on_mesh=True,
        preferred=True,
        stable=True,
    )
    verify_prefix(
        [r1, r2, c2],
        common_prefix,
        c2_rloc,
        on_mesh=True,
        preferred=False,
        stable=True,
    )


wpan.verify_within(check_prefixes, wait_time)

# Remove `r2`. This should trigger all the prefixes added by it or its
# child to timeout and be removed.
r2.leave()


def check_prefixes_on_r1_after_r2_leave():
    # Verify that entries added by r1 are still present
    verify_prefix([r1], prefix1, r1_rloc, on_mesh=True, preferred=True, stable=True)
    verify_prefix(
        [r1],
        common_prefix,
        r1_rloc,
        on_mesh=True,
        preferred=True,
        stable=False,
    )

    # Verify all entries added by `r2` or `c2` are removed
    verify_no_prefix([r1], prefix2, r2_rloc)
    verify_no_prefix([r1], prefix3, c2_rloc)
    verify_no_prefix([r1], common_prefix, r2_rloc)
    verify_no_prefix([r1], common_prefix, c2_rloc)


wpan.verify_within(check_prefixes_on_r1_after_r2_leave, wait_time)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
