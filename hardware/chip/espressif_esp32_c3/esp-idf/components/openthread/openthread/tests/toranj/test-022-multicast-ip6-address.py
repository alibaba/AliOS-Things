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
# Test description: Multicast addresses on routers, FEDs, and SEDs
#
# Test topology:
#
#       router
#         /\
#        /  \
#      fed  sed
#
# - Verify all nodes subscribe to:
#   * Link-local all nodes, realm-local all nodes,
#   * Link-local all Thread nodes, realm-local all Thread nodes,
#   * All MPL forwarder realm-local.
# - Verify routers also subscribe to:
#   * Link-local all routers, realm-local all nodes.
# - Verify adding/removing of multicast addresses on devices.
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

router = wpan.Node()
fed = wpan.Node()
sed = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Utility functions


def check_multicast_addresses(node, mcast_addr_list):
    addrs = wpan.parse_list(node.get(wpan.WPAN_IP6_MULTICAST_ADDRESSES))
    for addr in mcast_addr_list:
        verify(addr in addrs)


# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#

router.form("mcast-addr")

fed.join_node(router, wpan.JOIN_TYPE_END_DEVICE)
sed.join_node(router, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)

sed.set(wpan.WPAN_POLL_INTERVAL, '800')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# Get the mesh-local prefix (remove the "/64" at the end of the string)
ml_prefix = router.get(wpan.WPAN_IP6_MESH_LOCAL_PREFIX)[1:-1].split('/')[0]

# Derive the link-local/realm-local all thread nodes multicast addresses
ll_all_thread_nodes_addr = 'ff32:40:' + ml_prefix + '1'
rl_all_thread_nodes_addr = 'ff33:40:' + ml_prefix + '1'

# List of multicast addresses subscribed by all nodes
mcast_addrs = [
    "ff02::1",  # All nodes link-local
    "ff03::1",  # All nodes realm-local
    "ff03::fc",  # All MPL forwarder realm-local
    ll_all_thread_nodes_addr,
    rl_all_thread_nodes_addr,
]

# List of multicast addresses subscribed by routers only
router_mcast_addrs = mcast_addrs + [
    "ff02::2",  # All routers link-local
    "ff03::2",  # All routers realm-local
]

check_multicast_addresses(router, router_mcast_addrs)
check_multicast_addresses(fed, router_mcast_addrs)
check_multicast_addresses(sed, mcast_addrs)

# Add a multicast address

MCAST_ADDR = "ff02::114"

for node in [router, fed, sed]:
    node.add(wpan.WPAN_IP6_MULTICAST_ADDRESSES, MCAST_ADDR)
    addrs = wpan.parse_list(node.get(wpan.WPAN_IP6_MULTICAST_ADDRESSES))
    verify(MCAST_ADDR in addrs)

    node.remove(wpan.WPAN_IP6_MULTICAST_ADDRESSES, MCAST_ADDR)
    addrs = wpan.parse_list(node.get(wpan.WPAN_IP6_MULTICAST_ADDRESSES))
    verify(MCAST_ADDR not in addrs)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
