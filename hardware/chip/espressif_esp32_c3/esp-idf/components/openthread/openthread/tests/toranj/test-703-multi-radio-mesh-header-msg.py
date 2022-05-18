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

import time
import wpan
from wpan import verify

# -----------------------------------------------------------------------------------------------------------------------
# Test description: This test covers behavior of MeshHeader messages (message sent over multi-hop) when the
# underlying devices (in the path) support different radio types with different frame MTU length.
#
#   r1  --------- r2 ---------- r3
# (trel)      (15.4+trel)     (15.4)
#                  |            |
#                  |            |
#               c2 (15.4)     c3 (15.4)
#
# c3 is used for quick router promotion of r3.
#
test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

r1 = wpan.Node(wpan.NODE_TREL)
r2 = wpan.Node(wpan.NODE_15_4_TREL)
r3 = wpan.Node(wpan.NODE_15_4)
c2 = wpan.Node(wpan.NODE_15_4)
c3 = wpan.Node(wpan.NODE_15_4)

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#

r1.allowlist_node(r2)
r2.allowlist_node(r1)

r2.allowlist_node(r3)
r3.allowlist_node(r2)

r2.allowlist_node(c2)
c2.allowlist_node(r2)

r3.allowlist_node(c3)
c3.allowlist_node(r3)

r1.form("mesh-header")

r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)
r3.join_node(r2, wpan.JOIN_TYPE_ROUTER)
c2.join_node(r2, wpan.NODE_TYPE_SLEEPY_END_DEVICE)

c3.join_node(r3, wpan.JOIN_TYPE_END_DEVICE)

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

WAIT_TIME = 5
INVALID_ROUTER_ID = 63
MSG_LEN = 1000
MSG_COUNT = 2
POLL_INTERVAL = 400

c2.set(wpan.WPAN_POLL_INTERVAL, str(POLL_INTERVAL))

# Verify that each node supports the correct radio links.

r1_radios = wpan.parse_list(r1.get(wpan.WPAN_OT_SUPPORTED_RADIO_LINKS))
verify(len(r1_radios) == 1 and (wpan.RADIO_LINK_TREL_UDP6 in r1_radios))

r2_radios = wpan.parse_list(r2.get(wpan.WPAN_OT_SUPPORTED_RADIO_LINKS))
verify(
    len(r2_radios) == 2 and (wpan.RADIO_LINK_IEEE_802_15_4 in r2_radios) and (wpan.RADIO_LINK_TREL_UDP6 in r2_radios))

r3_radios = wpan.parse_list(r3.get(wpan.WPAN_OT_SUPPORTED_RADIO_LINKS))
verify(len(r3_radios) == 1 and (wpan.RADIO_LINK_IEEE_802_15_4 in r3_radios))

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Wait for all router entries are updated

r1_rloc = int(r1.get(wpan.WPAN_THREAD_RLOC16), 16)


def check_r1_router_table():
    router_table = wpan.parse_router_table_result(r1.get(wpan.WPAN_THREAD_ROUTER_TABLE))
    verify(len(router_table) == 3)
    for entry in router_table:
        verify(entry.rloc16 == r1_rloc or entry.is_link_established() or entry.next_hop != INVALID_ROUTER_ID)


wpan.verify_within(check_r1_router_table, WAIT_TIME)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Send large message from r1 to r3. Such a message would be sent as a
# Mesh Header message. Note that the origin r1 is a trel-only device
# whereas the final destination r3 is a 15.4-only device. The originator
# should use smaller MTU size (which then fragment the message into
# multiple frames) otherwise the 15.4 link won't be able to handle it.

r1_ml_address = r1.get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]
r3_ml_address = r3.get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]

sender = r1.prepare_tx(r1_ml_address, r3_ml_address, MSG_LEN, MSG_COUNT)
recver = r3.prepare_rx(sender)

wpan.Node.perform_async_tx_rx()

verify(sender.was_successful)
verify(recver.was_successful)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Send messages from c2 to r1.

c2_ml_address = c2.get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]

sender = c2.prepare_tx(c2_ml_address, r1_ml_address, MSG_LEN, MSG_COUNT)
recver = r1.prepare_rx(sender)

wpan.Node.perform_async_tx_rx()

verify(sender.was_successful)
verify(recver.was_successful)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Send messages from r3 to r1.

sender = r3.prepare_tx(r3_ml_address, r1_ml_address, MSG_LEN, MSG_COUNT)
recver = r1.prepare_rx(sender)

wpan.Node.perform_async_tx_rx()

verify(sender.was_successful)
verify(recver.was_successful)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
