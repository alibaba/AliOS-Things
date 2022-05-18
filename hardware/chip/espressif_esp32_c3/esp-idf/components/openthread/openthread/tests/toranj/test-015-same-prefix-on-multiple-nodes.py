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
# Test description: Adding addresses with same prefix on multiple nodes.
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
sed2 = wpan.Node()

all_nodes = [r1, r2, sed2]

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#
# Two routers r1 and r2 (sed2 is used for quick promotion of r2 to router
# status).

r1.allowlist_node(r2)
r2.allowlist_node(r1)

r2.allowlist_node(sed2)
sed2.allowlist_node(r2)

r1.form("same-prefix")

r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)
sed2.join_node(r2, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)

sed2.set(wpan.WPAN_POLL_INTERVAL, '500')

r2.status()

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

IP6_PREFIX = "fd00:abba::"

IP6_ADDR_1 = IP6_PREFIX + "1"
IP6_ADDR_2 = IP6_PREFIX + "2"

# Add IP6_ADDR_2 to r2.

r2.add_ip6_address_on_interface(IP6_ADDR_2, prefix_len=64)

# Verify (within 5 seconds) that corresponding prefix is seen on both nodes.


def check_prefix():
    for node in [r1, r2]:
        prefixes = wpan.parse_on_mesh_prefix_result(node.get(wpan.WPAN_THREAD_ON_MESH_PREFIXES))
        for p in prefixes:
            if p.prefix == IP6_PREFIX:
                if (p.origin == 'ncp' and p.prefix_len == '64' and p.is_stable() and p.is_on_mesh() and
                        p.is_preferred() and not p.is_def_route() and not p.is_slaac() and not p.is_dhcp() and
                        not p.is_config() and p.priority == "med"):
                    break
        else:  # `for` loop finished without finding the prefix.
            raise wpan.VerifyError('Did not find prefix {} on node {}'.format(IP6_PREFIX, r1))


wpan.verify_within(check_prefix, 5)

# After prefix is seen on r1, add an address with same prefix on r1.
r1.add_ip6_address_on_interface(IP6_ADDR_1, prefix_len=64)

# Verify that the prefix is still seen on both nodes.
wpan.verify_within(check_prefix, 5)

# Remove the address from r2 which should remove the corresponding the prefix as well
# After this since r1 still has the address, the prefix should be present
# on both nodes.
r2.remove_ip6_address_on_interface(IP6_ADDR_2, prefix_len=64)
wpan.verify_within(check_prefix, 5)

# Reset r1 and verify that the prefix is retained correctly (by wpantund).
r1.reset()
wpan.verify_within(check_prefix, 8)

# Remove the address on r1. Verify that prefix list is empty.
r1.remove_ip6_address_on_interface(IP6_ADDR_1, prefix_len=64)


def check_empty_prefix_list():
    for node in [r1, r2]:
        prefixes = wpan.parse_on_mesh_prefix_result(node.get(wpan.WPAN_THREAD_ON_MESH_PREFIXES))
        verify(len(prefixes) == 0)


wpan.verify_within(check_empty_prefix_list, 5)

# Add both addresses back-to-back and check the prefix list to contain the
# prefix.
r1.add_ip6_address_on_interface(IP6_ADDR_1, prefix_len=64)
r2.add_ip6_address_on_interface(IP6_ADDR_2, prefix_len=64)
wpan.verify_within(check_prefix, 5)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
