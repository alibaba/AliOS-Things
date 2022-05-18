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
# Test description: Traffic between router and end-device (link-local and
# mesh-local IPv6 addresses)

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

node1 = wpan.Node()
node2 = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology

# Two-node network (node1 leader/router, node2 end-device)

node1.form('test-PAN')
node2.join_node(node1, node_type=wpan.JOIN_TYPE_END_DEVICE)

verify(node2.get(wpan.WPAN_STATE) == wpan.STATE_ASSOCIATED)
verify(node2.get(wpan.WPAN_NAME) == node1.get(wpan.WPAN_NAME))
verify(node2.get(wpan.WPAN_PANID) == node1.get(wpan.WPAN_PANID))
verify(node2.get(wpan.WPAN_XPANID) == node1.get(wpan.WPAN_XPANID))

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# Get the link local addresses
ll1 = node1.get(wpan.WPAN_IP6_LINK_LOCAL_ADDRESS)[1:-1]
ll2 = node2.get(wpan.WPAN_IP6_LINK_LOCAL_ADDRESS)[1:-1]

# Get the mesh-local addresses
ml1 = node1.get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]
ml2 = node2.get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]

NUM_MSGS = 3
MSG_LENS = [40, 100, 400, 800, 1000]
PORT = 1234

# all src and dst configuration (link-local and mesh-local)
for src, dst in [(ll1, ll2), (ll1, ml2), (ml1, ll2), (ml1, ml2)]:

    for msg_length in MSG_LENS:
        sender = node1.prepare_tx(src, dst, msg_length, NUM_MSGS)
        recver = node2.prepare_rx(sender)

        wpan.Node.perform_async_tx_rx()

        verify(sender.was_successful)
        verify(recver.was_successful)

    # Send and receive at the same time on same port number

    s1 = node1.prepare_tx((src, PORT), (dst, PORT), 'Hi there!', NUM_MSGS)
    r1 = node2.prepare_rx(s1)
    s2 = node2.prepare_tx((dst, PORT), (src, PORT), 'Hello back to you!', NUM_MSGS)
    r2 = node1.prepare_rx(s2)

    wpan.Node.perform_async_tx_rx()

    verify(s1.was_successful and r1.was_successful)
    verify(s2.was_successful and r2.was_successful)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
