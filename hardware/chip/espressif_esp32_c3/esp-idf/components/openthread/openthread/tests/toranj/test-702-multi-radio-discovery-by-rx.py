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
# Test description: This test covers behavior of device after TREL network is temporarily disabled
# and rediscovery of TREL by receiving message over that radio from the neighbor.
#
#   r1  ---------- r2
# (15.4+trel)   (15.4+trel)
#
#  On r2 we disable trel temporarily.
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

r1 = wpan.Node(wpan.NODE_15_4_TREL)
r2 = wpan.Node(wpan.NODE_15_4_TREL)
c2 = wpan.Node(wpan.NODE_15_4)

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology
#

r1.allowlist_node(r2)
r2.allowlist_node(r1)

r2.allowlist_node(c2)
c2.allowlist_node(r2)

r1.form("discover-by-rx")

r2.join_node(r1, wpan.JOIN_TYPE_ROUTER)
c2.join_node(r2, wpan.JOIN_TYPE_SLEEPY_END_DEVICE)

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

WAIT_TIME = 5
HIGH_PREFERENCE_THRESHOLD = 220
MIN_PREFERENCE_THRESHOLD = 0

r1_ext_address = r1.get(wpan.WPAN_EXT_ADDRESS)[1:-1]
r1_rloc = int(r1.get(wpan.WPAN_THREAD_RLOC16), 16)
r1_ml_address = r1.get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]
r2_ext_address = r2.get(wpan.WPAN_EXT_ADDRESS)[1:-1]
r2_rloc = int(r2.get(wpan.WPAN_THREAD_RLOC16), 16)
r2_ml_address = r2.get(wpan.WPAN_IP6_MESH_LOCAL_ADDRESS)[1:-1]

# Wait for r2 to become router and r1 establishes a link with it


def check_r1_router_table():
    router_table = wpan.parse_router_table_result(r1.get(wpan.WPAN_THREAD_ROUTER_TABLE))
    verify(len(router_table) == 2)
    for entry in router_table:
        verify(entry.rloc16 == r1_rloc or entry.is_link_established())


wpan.verify_within(check_r1_router_table, WAIT_TIME)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Check that r1 detected both TREL and 15.4 as supported radio by r2

r1_radios = wpan.parse_list(r1.get(wpan.WPAN_OT_SUPPORTED_RADIO_LINKS))
verify(
    len(r1_radios) == 2 and (wpan.RADIO_LINK_IEEE_802_15_4 in r1_radios) and (wpan.RADIO_LINK_TREL_UDP6 in r1_radios))

r2_radios = wpan.parse_list(r2.get(wpan.WPAN_OT_SUPPORTED_RADIO_LINKS))
verify(
    len(r2_radios) == 2 and (wpan.RADIO_LINK_IEEE_802_15_4 in r2_radios) and (wpan.RADIO_LINK_TREL_UDP6 in r2_radios))


def check_r1_sees_r2_has_two_radio_links():
    r1_neighbor_radios = wpan.parse_multi_radio_result(r1.get(wpan.WPAN_OT_NEIGHBOR_TABLE_MULTI_RADIO_INFO))
    verify(len(r1_neighbor_radios) == 1)
    verify(len(r1_neighbor_radios[0].radios) == 2)


wpan.verify_within(check_r1_sees_r2_has_two_radio_links, WAIT_TIME)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Send from r1 to r2 and verify that r1 prefers TREL radio link for sending to r2.

sender = r1.prepare_tx(r1_ml_address, r2_ml_address, "Hi r2 from r1", 5)
recver = r2.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful)
verify(recver.was_successful)

r1_neighbor_radios = wpan.parse_multi_radio_result(r1.get(wpan.WPAN_OT_NEIGHBOR_TABLE_MULTI_RADIO_INFO))
verify(len(r1_neighbor_radios) == 1)
r2_radio_info = r1_neighbor_radios[0]
verify(r2_radio_info.supports(wpan.RADIO_LINK_TREL_UDP6))
verify(r2_radio_info.preference(wpan.RADIO_LINK_TREL_UDP6) >= HIGH_PREFERENCE_THRESHOLD)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Now disable TREL link on r2 and send again. We expect that r1 would quickly learn that trel is
# no longer supported by r2 and prefer 15.4 for tx to r2.

r2.set(wpan.WPAN_OT_TREL_TEST_MODE_ENABLE, 'false')
verify(r2.get(wpan.WPAN_OT_TREL_TEST_MODE_ENABLE) == 'false')

sender = r1.prepare_tx(r1_ml_address, r2_ml_address, "Hi again r2 from r1", 5)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful)


def check_r1_does_not_prefer_trel_for_r2():
    r1_neighbor_radios = wpan.parse_multi_radio_result(r1.get(wpan.WPAN_OT_NEIGHBOR_TABLE_MULTI_RADIO_INFO))
    verify(len(r1_neighbor_radios) == 1)
    r2_radio_info = r1_neighbor_radios[0]
    verify(r2_radio_info.supports(wpan.RADIO_LINK_TREL_UDP6))
    verify(r2_radio_info.preference(wpan.RADIO_LINK_TREL_UDP6) <= MIN_PREFERENCE_THRESHOLD)


wpan.verify_within(check_r1_does_not_prefer_trel_for_r2, WAIT_TIME)

# Check that we can send between r1 and r2 (now all tx should use 15.4)

sender = r1.prepare_tx(r1_ml_address, r2_ml_address, "Hi on 15.4 r2 from r1", 5)
recver = r2.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful)
verify(recver.was_successful)

r1_neighbor_radios = wpan.parse_multi_radio_result(r1.get(wpan.WPAN_OT_NEIGHBOR_TABLE_MULTI_RADIO_INFO))
verify(len(r1_neighbor_radios) == 1)
r2_radio_info = r1_neighbor_radios[0]
verify(r2_radio_info.supports(wpan.RADIO_LINK_TREL_UDP6))
verify(r2_radio_info.preference(wpan.RADIO_LINK_TREL_UDP6) <= MIN_PREFERENCE_THRESHOLD)
verify(r2_radio_info.supports(wpan.RADIO_LINK_IEEE_802_15_4))
verify(r2_radio_info.preference(wpan.RADIO_LINK_IEEE_802_15_4) >= HIGH_PREFERENCE_THRESHOLD)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Enable trel back on r2, start sending traffic from r2 to r1.
# r2 would use probe mehcnasim to discover that trel is enabled again
# r1 should notice new rx on trel and update trel preference for r1

r2.set(wpan.WPAN_OT_TREL_TEST_MODE_ENABLE, 'true')
verify(r2.get(wpan.WPAN_OT_TREL_TEST_MODE_ENABLE) == 'true')

sender = r2.prepare_tx(r2_ml_address, r1_ml_address, "Probing r1 from r2", 80)
recver = r1.prepare_rx(sender)
wpan.Node.perform_async_tx_rx()
verify(sender.was_successful)
verify(recver.was_successful)


def check_r1_again_prefers_trel_for_r2():
    r1_neighbor_radios = wpan.parse_multi_radio_result(r1.get(wpan.WPAN_OT_NEIGHBOR_TABLE_MULTI_RADIO_INFO))
    verify(len(r1_neighbor_radios) == 1)
    r2_radio_info = r1_neighbor_radios[0]
    verify(r2_radio_info.supports(wpan.RADIO_LINK_TREL_UDP6))
    verify(r2_radio_info.preference(wpan.RADIO_LINK_TREL_UDP6) >= HIGH_PREFERENCE_THRESHOLD)


wpan.verify_within(check_r1_again_prefers_trel_for_r2, WAIT_TIME)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
