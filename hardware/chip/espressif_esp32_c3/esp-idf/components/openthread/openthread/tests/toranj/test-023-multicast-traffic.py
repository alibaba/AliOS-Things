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
# Test description: Multicast traffic
#
# Network topology
#
#     r1 ---- r2 ---- r3 ---- r4
#             |               |
#             |               |
#            fed             sed
#
# Test covers the following multicast traffic:
#
# - r2  =>> link-local all-nodes.   Expected to receive on [r1, r2, r3, fed].
# - r3  =>> mesh-local all-nodes.   Expected to receive on [r1, r2, r3, r4, fed].
# - r3  =>> link-local all-routers. Expected to receive on [r2, r3, r4].
# - r3  =>> mesh-local all-routers. Expected to receive on all routers.
# - r1  =>> link-local all-thread.  Expected to receive on [r1, r2].
# - fed =>> mesh-local all-thread.  Expected to receive on all nodes.
# - r1  =>> specific address (on r2 and sed). Expected to receive on [r2, sed].
# - Check behavior with different multicast hop limit values (1-hop up to 4-hops).
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Utility functions


def send_mcast(
    src_node,
    src_addr,
    mcast_addr,
    recving_nodes,
    non_recving_nodes=[],
    msg_len=30,
    mcast_hops=5,
):
    """
    Send a multicast message with given `len` from `src_node` using `src_addr` to the multicast address `mcast_addr`.
    Verify that the message is received on all nodes in `recving_nodes` list and that it is not received on all
    nodes in `non_recving_nodes` list.
    """
    sender = src_node.prepare_tx(src_addr, mcast_addr, msg_len, mcast_hops=mcast_hops)
    recvers = [node.prepare_rx(sender) for node in recving_nodes]
    listeners = [node.prepare_listener(sender.dst_port, timeout=0.5) for node in non_recving_nodes]

    wpan.Node.perform_async_tx_rx()

    verify(sender.was_successful)
    for recvr in recvers:
        verify(recvr.was_successful)
    for lsnr in listeners:
        # `all_rx_msg` contains a list of (msg_content, (src_addr, src_port)).
        verify(
            len(lsnr.all_rx_msg) == 0 or
            all([msg[1][0] != sender.src_addr and msg[1][1] != sender.src_port for msg in lsnr.all_rx_msg]))


# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

r1 = wpan.Node()
r2 = wpan.Node()
r3 = wpan.Node()
r4 = wpan.Node()
fed = wpan.Node()
sed = wpan.Node()

all_routers = [r1, r2, r3, r4]
all_nodes = all_routers + [fed, sed]

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#
# Test topology:
#
#    r1 ---- r2 ---- r3 ---- r4
#            |               |
#            |               |
#           fed             sed
#

r1.form("mcast-traffic")

r1.allowlist_node(r2)
r2.allowlist_node(r1)
r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)

r2.allowlist_node(fed)
fed.allowlist_node(r2)
fed.join_node(r2, wpan.JOIN_TYPE_END_DEVICE)

r2.allowlist_node(r3)
r3.allowlist_node(r2)
r3.join_node(r2, wpan.JOIN_TYPE_ROUTER)

r3.allowlist_node(r4)
r4.allowlist_node(r3)
r4.join_node(r3, wpan.JOIN_TYPE_ROUTER)

r4.allowlist_node(sed)
sed.allowlist_node(r4)
sed.join_node(r4, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
sed.set(wpan.WPAN_POLL_INTERVAL, '600')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

ml1 = r1.get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]
ll1 = r1.get(wpan.WPAN_IP6_LINK_LOCAL_ADDRESS)[1:-1]

ml2 = r2.get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]
ll2 = r2.get(wpan.WPAN_IP6_LINK_LOCAL_ADDRESS)[1:-1]

ml3 = r3.get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]
ll3 = r3.get(wpan.WPAN_IP6_LINK_LOCAL_ADDRESS)[1:-1]

ml4 = r4.get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]
ll4 = r4.get(wpan.WPAN_IP6_LINK_LOCAL_ADDRESS)[1:-1]

ml_fed = fed.get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]
ll_fed = fed.get(wpan.WPAN_IP6_LINK_LOCAL_ADDRESS)[1:-1]

# Multicast addresses

ll_all_nodes = "ff02::1"
ml_all_nodes = "ff03::1"
ml_all_mlp_fwder_nodes = "ff03::fc"

ll_all_routers = "ff02::2"
ml_all_routers = "ff03::2"

ml_prefix = r1.get(wpan.WPAN_IP6_MESH_LOCAL_PREFIX)[1:-1].split('/')[0]
ll_all_thread_nodes_addr = 'ff32:40:' + ml_prefix + '1'
ml_all_thread_nodes_addr = 'ff33:40:' + ml_prefix + '1'

#
#    r1 ---- r2 ---- r3 ---- r4
#            |               |
#            |               |
#           fed             sed
#

# r2 =>> link-local all-nodes.
send_mcast(r2, ll2, ll_all_nodes, [r1, r2, r3, fed], [r4, sed])

# r3 =>> mesh-local all-nodes.
send_mcast(r3, ml3, ml_all_nodes, [r1, r2, r3, r4, fed])

# r3 =>> link-local all-routers.
send_mcast(r3, ml3, ll_all_routers, [r2, r3, r4], [r1, fed, sed])

# r3 =>> mesh-local all-routers.
send_mcast(r3, ml3, ml_all_routers, all_routers, [sed])

# r1 =>> link-local all-thread.
send_mcast(r1, ll1, ll_all_thread_nodes_addr, [r1, r2], [fed, r3, r4, sed])

# fed =>> mesh-local all-thread.
send_mcast(fed, ml_fed, ml_all_thread_nodes_addr, all_nodes)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Send a large multicast message (requiring MAC level fragmentations)

send_mcast(r3, ml3, ml_all_thread_nodes_addr, all_nodes, msg_len=400)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Check the hop limit behavior

# r1 =>> mesh-local all-thread (one hop)
send_mcast(
    r1,
    ml1,
    ml_all_thread_nodes_addr,
    [r1, r2],
    [fed, r3, r4, sed],
    mcast_hops=1,
)

# r1 =>> mesh-local all-thread (two hops)
send_mcast(
    r1,
    ml1,
    ml_all_thread_nodes_addr,
    [r1, r2, fed, r3],
    [r4, sed],
    mcast_hops=2,
)

# r1 =>> mesh-local all-thread (three hops)
send_mcast(
    r1,
    ml1,
    ml_all_thread_nodes_addr,
    [r1, r2, fed, r3, r4],
    [sed],
    mcast_hops=3,
)

# r1 =>> mesh-local all-thread (four hops)
send_mcast(r1, ml1, ml_all_thread_nodes_addr, [r1, r2, fed, r3, r4, sed], mcast_hops=4)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Subscribe to a specific multicast address on r2 and sed

mcast_addr = "ff03::114"
r2.add(wpan.WPAN_IP6_MULTICAST_ADDRESSES, mcast_addr)
sed.add(wpan.WPAN_IP6_MULTICAST_ADDRESSES, mcast_addr)
time.sleep(1)

# r1 =>> specific address
send_mcast(r1, ml1, mcast_addr, [r2, sed], [r1, r3, r4, fed])

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
