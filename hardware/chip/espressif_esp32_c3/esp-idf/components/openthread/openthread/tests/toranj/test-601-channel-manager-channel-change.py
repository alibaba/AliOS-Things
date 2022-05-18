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
# Test description: verifies `ChannelManager` channel change process

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))


def verify_channel(nodes, new_channel, wait_time=20):
    """
    This function checks the channel on a given list of `nodes` and verifies that all nodes
    switch to a given `new_channel` (as int) within certain `wait_time` (int and in seconds)
    """
    start_time = time.time()

    while not all([(new_channel == int(node.get(wpan.WPAN_CHANNEL), 0)) for node in nodes]):
        if time.time() - start_time > wait_time:
            print('Took too long to switch to channel {} ({}>{} sec)'.format(new_channel,
                                                                             time.time() - start_time, wait_time))
            exit(1)
        time.sleep(0.1)


# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

r1 = wpan.Node()
r2 = wpan.Node()
r3 = wpan.Node()
sc1 = wpan.Node()
ec1 = wpan.Node()
sc2 = wpan.Node()
sc3 = wpan.Node()

all_nodes = [r1, r2, r3, sc1, ec1, sc2, sc3]

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology

for node in all_nodes:
    node.set(wpan.WPAN_OT_LOG_LEVEL, '0')

r1.allowlist_node(r2)
r2.allowlist_node(r1)
r1.allowlist_node(r3)
r3.allowlist_node(r1)

r1.allowlist_node(sc1)
r1.allowlist_node(ec1)
r2.allowlist_node(sc2)
r3.allowlist_node(sc3)

r1.form('channel-manager', channel=12)
r2.join_node(r1, node_type=wpan.JOIN_TYPE_ROUTER)
r3.join_node(r1, node_type=wpan.JOIN_TYPE_ROUTER)
sc1.join_node(r1, node_type=wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
ec1.join_node(r1, node_type=wpan.JOIN_TYPE_END_DEVICE)
sc2.join_node(r2, node_type=wpan.JOIN_TYPE_SLEEPY_END_DEVICE)
sc3.join_node(r3, node_type=wpan.JOIN_TYPE_SLEEPY_END_DEVICE)

sc1.set(wpan.WPAN_POLL_INTERVAL, '500')
sc2.set(wpan.WPAN_POLL_INTERVAL, '500')
sc3.set(wpan.WPAN_POLL_INTERVAL, '500')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# The channel manager delay is set from "openthread-core-toranj-config.h".
# Verify that it is 2 seconds.

verify(int(r1.get(wpan.WPAN_CHANNEL_MANAGER_DELAY), 0) == 2)

verify_channel(all_nodes, 12)

# Request a channel change to channel 13 from router r1

r1.set(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL, '13')
verify(int(r1.get(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL), 0) == 13)
verify_channel(all_nodes, 13)

# Request same channel change on multiple routers at the same time

r1.set(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL, '14')
r2.set(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL, '14')
r3.set(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL, '14')
verify_channel(all_nodes, 14)

# Request different channel changes from same router (router r1).

r1.set(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL, '15')
verify_channel(all_nodes, 14)
r1.set(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL, '16')
verify_channel(all_nodes, 16)

# Request different channels from two routers (r1 and r2)

r1.set(wpan.WPAN_CHANNEL_MANAGER_DELAY, '20')  # increase the time to ensure r1 change is in process
r1.set(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL, '17')
time.sleep(10.5 / speedup)
verify_channel(all_nodes, 16)
r2.set(wpan.WPAN_CHANNEL_MANAGER_NEW_CHANNEL, '18')
verify_channel(all_nodes, 18)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
