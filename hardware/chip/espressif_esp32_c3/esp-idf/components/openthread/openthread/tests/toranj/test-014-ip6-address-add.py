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
# Test description: Adding/Removing IPv6 addresses on routers and SEDs on network interface.
#
# Test topology:
#
#     r1 ---- r2
#     |       |
#     |       |
#    fed1    sed2
#
# IPv6 addresses are added as follows:
# - On `r2`   add `IP6_ADDR_1` with prefix `IP6_PREFIX_1`
# - On `fed1` add `IP6_ADDR_2` with prefix `IP6_PREFIX_2`
# - On `sed2` add `IP6_ADDR_3` with prefix `IP6_PREFIX_3`
#
# The test then covers the following:
# - Verify that the addresses are present in "IPv6:AllAddresses" wpantund property on the corresponding node.
# - Verify that all prefixes are present in network data with correct configuration flags (on all nodes).
# - Verify that `sed2`'s address is present in `r2` (its parent) "Thread:ChildTable:Addresses".
# - Verify that addresses/prefixes are retained by wpantund over NCP reset.
# - Verify that when an IPv6 address is removed from network interface, its corresponding prefix is also removed from
#   all nodes.

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

r1 = wpan.Node()
fed1 = wpan.Node()
r2 = wpan.Node()
sed2 = wpan.Node()

all_nodes = [r1, fed1, r2, sed2]

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#
#   r1 ---- r2
#   |       |
#   |       |
#  fed1    sed2

r1.allowlist_node(r2)
r2.allowlist_node(r1)

r1.allowlist_node(fed1)
fed1.allowlist_node(r1)

r2.allowlist_node(sed2)
sed2.allowlist_node(r2)

r1.form("ip-addr")
r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)

fed1.join_node(r1, wpan.JOIN_TYPE_END_DEVICE)
sed2.join_node(r2, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)

sed2.set(wpan.WPAN_POLL_INTERVAL, '300')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

IP6_PREFIX_1 = "fd00:c0de::"
IP6_PREFIX_2 = "fd00:deed::"
IP6_PREFIX_3 = "fd00:beef::"

IP6_ADDR_1 = IP6_PREFIX_1 + "1"
IP6_ADDR_2 = IP6_PREFIX_2 + "2"
IP6_ADDR_3 = IP6_PREFIX_3 + "3"

# On `r2`   add `IP6_ADDR_1` with prefix `IP6_PREFIX_1`
# On `fed1` add `IP6_ADDR_2` with prefix `IP6_PREFIX_2`
# On `sed2` add `IP6_ADDR_3` with prefix `IP6_PREFIX_3`

r2.add_ip6_address_on_interface(IP6_ADDR_1, prefix_len=64)
fed1.add_ip6_address_on_interface(IP6_ADDR_2, prefix_len=64)
sed2.add_ip6_address_on_interface(IP6_ADDR_3, prefix_len=64)


def check_addresses_and_prefixes():
    # Verify that the addresses are present in "IPv6:AllAddresses" wpantund
    # property on the corresponding node.
    verify(r2.find_ip6_address_with_prefix(IP6_PREFIX_1) == IP6_ADDR_1)
    verify(fed1.find_ip6_address_with_prefix(IP6_PREFIX_2) == IP6_ADDR_2)
    verify(sed2.find_ip6_address_with_prefix(IP6_PREFIX_3) == IP6_ADDR_3)

    # Verify that all prefixes are present in network data on all nodes (with
    # correct flags).
    for prefix in [IP6_PREFIX_1, IP6_PREFIX_2, IP6_PREFIX_3]:
        for node in all_nodes:
            prefixes = wpan.parse_on_mesh_prefix_result(node.get(wpan.WPAN_THREAD_ON_MESH_PREFIXES))
            for p in prefixes:
                if p.prefix == prefix:
                    verify(p.prefix_len == '64')
                    verify(p.is_stable())
                    verify(p.is_on_mesh())
                    verify(p.is_preferred())
                    verify(p.is_def_route() is False)
                    verify(p.is_slaac() is False)
                    verify(p.is_dhcp() is False)
                    verify(p.is_config() is False)
                    verify(p.priority == "med")
                    break
            else:  # `for` loop finished without finding the prefix.
                raise wpan.VerifyError('Did not find prefix {} on node {}'.format(prefix, node))

    # Verify that IPv6 address of `sed2` is present on `r2` (its parent)
    # "Thread:ChildTable:Addresses".
    addr_str = r2.get(wpan.WPAN_THREAD_CHILD_TABLE_ADDRESSES)
    # search for index on address in the `addr_str` and ensure it is
    # non-negative.
    verify(addr_str.find(IP6_ADDR_3) >= 0)


# Check the addresses and prefixes (wait time 20 seconds)
wpan.verify_within(check_addresses_and_prefixes, 15)

# Reset the nodes and verify that all addresses/prefixes are preserved.
fed1.reset()
sed2.reset()
wpan.verify_within(check_addresses_and_prefixes, 20)

# Remove address from `r2`
r2.remove_ip6_address_on_interface(IP6_ADDR_1, prefix_len=64)


def check_address_prefix_removed():
    # Verify that address is removed from r2
    verify(r2.find_ip6_address_with_prefix(IP6_PREFIX_1) == '')
    # Verify that the related prefix is also removed on all nodes
    for node in all_nodes:
        prefixes = wpan.parse_on_mesh_prefix_result(node.get(wpan.WPAN_THREAD_ON_MESH_PREFIXES))
        for p in prefixes:
            verify(p.prefix != IP6_PREFIX_1)


# Check the addresses and prefixes (wait time 15 seconds)
wpan.verify_within(check_address_prefix_removed, 15)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
