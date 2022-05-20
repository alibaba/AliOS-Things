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
# Adding on-mesh prefix and `config-gateway` command
#
#  - Verifies adding an on-mesh prefix using `config-gateway`, `add-prefix` commands.
#  - Verifies prefixes with different flags/priorities added on routers and/or end-devices.
#  - Verifies `wpantund` adding SLAAC based IPv6 address based on prefix.
#  - Verifies `wpantund` retaining user-added prefixes and adding them back after an NCP reset.
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
    preferred=True,
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
            raise wpan.VerifyError('Did not find prefix {} on node {}'.format(prefix, node))


# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

r1 = wpan.Node()
r2 = wpan.Node()
sc1 = wpan.Node()
sc2 = wpan.Node()

all_nodes = [r1, r2, sc1, sc2]

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology

r1.allowlist_node(r2)
r2.allowlist_node(r1)

r1.allowlist_node(sc1)
r2.allowlist_node(sc2)

r1.form('config-gtway')
r2.join_node(r1, node_type=wpan.JOIN_TYPE_ROUTER)
sc1.join_node(r1, node_type=wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
sc2.join_node(r2, node_type=wpan.JOIN_TYPE_SLEEPY_END_DEVICE)

sc1.set(wpan.WPAN_POLL_INTERVAL, '200')
sc2.set(wpan.WPAN_POLL_INTERVAL, '200')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

WAIT_TIME = 5

prefix1 = 'fd00:abba:cafe::'
prefix2 = 'fd00:1234::'
prefix3 = 'fd00:deed::'
prefix4 = 'fd00:abcd::'

# Add on-mesh prefix1 on router r1
r1.config_gateway(prefix1)

# Verify that the prefix1 and its corresponding address are present on all
# nodes


def check_prefix1_on_all_nodes():
    verify_prefix(all_nodes, prefix1, stable=True, on_mesh=True, slaac=True)
    verify_address(all_nodes, prefix1)


wpan.verify_within(check_prefix1_on_all_nodes, WAIT_TIME)

# Now add prefix2 with priority `high` on router r2 and check all nodes
# for the new prefix/address
r2.config_gateway(prefix2, default_route=True, priority='1')


def check_prefix2_on_all_nodes():
    verify_prefix(
        all_nodes,
        prefix2,
        stable=True,
        on_mesh=True,
        slaac=True,
        default_route=True,
        priority='high',
    )
    verify_address(all_nodes, prefix2)


wpan.verify_within(check_prefix2_on_all_nodes, WAIT_TIME)

# Add prefix3 on sleepy end-device and check for it on all nodes
sc1.config_gateway(prefix3, priority='-1')


def check_prefix3_on_all_nodes():
    verify_prefix(
        all_nodes,
        prefix3,
        stable=True,
        on_mesh=True,
        slaac=True,
        priority='low',
    )
    verify_address(all_nodes, prefix3)


wpan.verify_within(check_prefix3_on_all_nodes, WAIT_TIME)

# Verify that prefix1 is retained by `wpantund` and pushed to NCP after a reset
r1.reset()


def check_r1_is_associated():
    verify(r1.is_associated())


# Wait for r1 to recover after reset
wpan.verify_within(check_r1_is_associated, WAIT_TIME)

# Wait for on-mesh prefix to be updated
wpan.verify_within(check_prefix1_on_all_nodes, WAIT_TIME)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Test `add-prefix` and `remove-prefix`

r1.add_prefix(
    prefix4,
    48,
    priority="1",
    stable=False,
    on_mesh=True,
    slaac=False,
    dhcp=True,
    configure=False,
    default_route=True,
    preferred=False,
)


def check_prefix4_on_r1():
    verify_prefix(
        all_nodes,
        prefix4,
        48,
        priority="high",
        stable=False,
        on_mesh=True,
        slaac=False,
        dhcp=True,
        configure=False,
        default_route=True,
        preferred=False,
    )


wpan.verify_within(check_prefix4_on_r1, WAIT_TIME)

# Remove prefix and verify that it is removed from list
r1.remove_prefix(prefix4, 48)


def check_prefix4_removed_from_r1():
    verify(r1.get(wpan.WPAN_THREAD_ON_MESH_PREFIXES).find(prefix4) < 0)


wpan.verify_within(check_prefix4_removed_from_r1, WAIT_TIME)

r1.add_prefix(
    prefix4,
    64,
    priority="-1",
    stable=True,
    on_mesh=False,
    slaac=True,
    dhcp=False,
    configure=True,
    default_route=False,
    preferred=True,
)
verify_prefix(
    [r1],
    prefix4,
    64,
    priority="low",
    stable=True,
    on_mesh=False,
    slaac=True,
    dhcp=False,
    configure=True,
    default_route=False,
    preferred=True,
)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
