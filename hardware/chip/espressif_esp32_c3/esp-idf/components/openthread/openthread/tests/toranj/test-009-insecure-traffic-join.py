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
# Test description: Check insecure data transmission during joining.

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

node1.form("insec-join-test")

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

insecure_port = 1234
NUM_MSGS = 4

# Make node1 joinable and set the insecure port
node1.permit_join(duration_sec='100', port=str(insecure_port))
verify(node1.get(wpan.WPAN_NETWORK_ALLOW_JOIN) == 'true')

# Join node1 network from node2 without setting the key
node2.join_node(node1, should_set_key=False)

verify(node2.get(wpan.WPAN_STATE) == wpan.STATE_CREDENTIALS_NEEDED)
verify(node2.get(wpan.WPAN_NAME) == node1.get(wpan.WPAN_NAME))
verify(node2.get(wpan.WPAN_PANID) == node1.get(wpan.WPAN_PANID))
verify(node2.get(wpan.WPAN_XPANID) == node1.get(wpan.WPAN_XPANID))

ll1 = node1.get(wpan.WPAN_IP6_LINK_LOCAL_ADDRESS)[1:-1]
ll2 = node2.get(wpan.WPAN_IP6_LINK_LOCAL_ADDRESS)[1:-1]

# Send insecure traffic from node2 to node1 using link-local IP address
# for src/dst and insecure port number

sender = node2.prepare_tx(ll2, (ll1, insecure_port), "Hi (insecure)", NUM_MSGS)
recver = node1.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful)
verify(recver.was_successful)

# Get the random src port number used by node1 and ensure node2 allows
# insecure rx traffic on that port

rx_port = recver.all_rx_msg[0][1][1]
node2.permit_join(duration_sec='100', port=str(rx_port))

# Send insecure reply from node1 to node2

sender2 = node1.prepare_tx((ll1, insecure_port), (ll2, rx_port), "Hi back! (insecure)", NUM_MSGS)
recver2 = node2.prepare_rx(sender2)
wpan.Node.perform_async_tx_rx()
verify(sender2.was_successful)
verify(recver2.was_successful)

# Now node2 fully joins the network (set the network key), check all
# secure traffic exchange between the nodes

node2.set(wpan.WPAN_KEY, node1.get(wpan.WPAN_KEY)[1:-1], binary_data=True)
verify(node2.is_associated())

node1.permit_join('0')

sender = node2.prepare_tx(ll2, (ll1, insecure_port), "Hi (now secure)", NUM_MSGS)
recver = node1.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful)
verify(recver.was_successful)

node2.permit_join('0')

sender2 = node1.prepare_tx((ll1, insecure_port), (ll2, rx_port), "Hi back! (secure now)", NUM_MSGS)
recver2 = node2.prepare_rx(sender2)
wpan.Node.perform_async_tx_rx()
verify(sender2.was_successful)
verify(recver2.was_successful)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
