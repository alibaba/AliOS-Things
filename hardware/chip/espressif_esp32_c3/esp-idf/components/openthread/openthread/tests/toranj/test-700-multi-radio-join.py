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
# Test description: This test covers joining of nodes with different radio links
#
# Parent node with trel and 15.4 with 3 children.
#
#        parent
#    (trel + 15.4)
#      /    |    \
#     /     |     \
#    /      |      \
#   c1     c2       c3
# (15.4)  (trel)  (trel+15.4)

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

parent = wpan.Node(wpan.NODE_15_4_TREL)
c1 = wpan.Node(wpan.NODE_15_4)
c2 = wpan.Node(wpan.NODE_TREL)
c3 = wpan.Node(wpan.NODE_15_4_TREL)

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

WAIT_TIME = 5

# Verify that each node supports the correct radio links:

parent_radios = wpan.parse_list(parent.get(wpan.WPAN_OT_SUPPORTED_RADIO_LINKS))
verify(
    len(parent_radios) == 2 and (wpan.RADIO_LINK_IEEE_802_15_4 in parent_radios) and
    (wpan.RADIO_LINK_TREL_UDP6 in parent_radios))

c1_radios = wpan.parse_list(c1.get(wpan.WPAN_OT_SUPPORTED_RADIO_LINKS))
verify(len(c1_radios) == 1 and wpan.RADIO_LINK_IEEE_802_15_4 in c1_radios)

c2_radios = wpan.parse_list(c2.get(wpan.WPAN_OT_SUPPORTED_RADIO_LINKS))
verify(len(c2_radios) == 1 and wpan.RADIO_LINK_TREL_UDP6 in c2_radios)

c3_radios = wpan.parse_list(c3.get(wpan.WPAN_OT_SUPPORTED_RADIO_LINKS))
verify(
    len(c3_radios) == 2 and (wpan.RADIO_LINK_IEEE_802_15_4 in c3_radios) and (wpan.RADIO_LINK_TREL_UDP6 in c3_radios))

parent.form("multi-radio")

c1.join_node(parent, wpan.JOIN_TYPE_END_DEVICE)
c2.join_node(parent, wpan.JOIN_TYPE_END_DEVICE)
c3.join_node(parent, wpan.JOIN_TYPE_END_DEVICE)

# Verify that parent correctly knows about all the children and their supported radio links

radio_info_entries = wpan.parse_multi_radio_result(parent.get(wpan.WPAN_OT_NEIGHBOR_TABLE_MULTI_RADIO_INFO))
verify(len(radio_info_entries) == 3)

for node in [c1, c2, c3]:
    # Find the entry matching node ext address
    address = node.get(wpan.WPAN_EXT_ADDRESS)[1:-1]
    filtred_list = [entry for entry in radio_info_entries if entry.ext_address == address]
    verify(len(filtred_list) == 1)
    entry = filtred_list[0]

    # Verify that the multi radio info matches the radio links supported by the node
    node_radios = wpan.parse_list(node.get(wpan.WPAN_OT_SUPPORTED_RADIO_LINKS))
    verify(len(node_radios) == len(entry.radios))
    for radio in node_radios:
        verify(entry.supports(radio))

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Reset the parent and check that all children are attached back successfully

parent.reset()


def check_all_children_are_attached():
    neighbor_table = wpan.parse_neighbor_table_result(parent.get(wpan.WPAN_THREAD_NEIGHBOR_TABLE))
    verify(len(neighbor_table) == 3)


wpan.verify_within(check_all_children_are_attached, WAIT_TIME)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
