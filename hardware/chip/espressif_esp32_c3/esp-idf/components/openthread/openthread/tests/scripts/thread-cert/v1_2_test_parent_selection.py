#!/usr/bin/env python3
#
#  Copyright (c) 2019, The OpenThread Authors.
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
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS'
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
#

import unittest

import thread_cert
import mle

LEADER_1_2 = 1
ROUTER_1_1 = 2
REED_1_2 = 3
ROUTER_1_2 = 4
REED_1_1 = 5
MED_1_1 = 6
MED_1_2 = 7

# Topology
#               (lq:2)  (pp:1)
#     REED_1_2  ----- ROUTER_1_2
#        |     \    /     |      \
#        |       \/    REED_1_1    \
# (lq:2) |      /  \  /  `router`    \
#        | (lq:2)    \                 \
#        |  /      /   \                 \
#       LEADER_1_2 --- ROUTER_1_1 -- MED_1_2
#                \        |
#                  \      |
#                    \    |
#                       MED_1_1
#
# 1) Bring up LEADER_1_2 and ROUTER_1_1,
# 2) Config link quality (LEADER_1_2->REED_1_2) as 2, bring up REED_1_2 which would attach to ROUTER_1_1
#    due to higher two-way link quality,
# 3) Config link quality(LEADER_1_2->ROUTER_1_2) and link quality(REED_1_2->ROUTER_1_2) as 2, bring up
#    ROUTER_1_2 which would attach to LEADER_1_2 due to active router is preferred,
# 4) Config parent priority as 1 on ROUTER_1_2, bring up REED_1_1 which would attach to ROUTER_1_2 due to
#    higher parent priority,
# 5) Upgrade REED_1_1 to `router` role, bring up MED_1_1 which would attach to LEADER_1_2 which has higher
#    link quality of 3,
# 6) Config parent priority as 1 on ROUTER_1_1, bring up MED_1_2 which would attach to ROUTER_1_2 due to
#    higher version
#


class TestParentSelection(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER_1_2: {
            'version': '1.2',
            'allowlist': [REED_1_2, ROUTER_1_2, REED_1_1, ROUTER_1_1, MED_1_1],
        },
        ROUTER_1_1: {
            'version': '1.1',
            'allowlist': [LEADER_1_2, REED_1_2, MED_1_2, MED_1_1],
        },
        REED_1_2: {
            'version': '1.2',
            'allowlist': [ROUTER_1_2, ROUTER_1_1, LEADER_1_2],
        },
        ROUTER_1_2: {
            'version': '1.2',
            'allowlist': [REED_1_2, MED_1_2, REED_1_1, LEADER_1_2],
        },
        REED_1_1: {
            'version': '1.1',
            'allowlist': [ROUTER_1_2, LEADER_1_2]
        },
        MED_1_1: {
            'mode': 'r',
            'version': '1.1',
            'allowlist': [LEADER_1_2, ROUTER_1_1],
        },
        MED_1_2: {
            'mode': 'r',
            'version': '1.2',
            'allowlist': [ROUTER_1_1, ROUTER_1_2],
        },
    }
    """All nodes are created with default configurations"""

    def test(self):

        self.nodes[LEADER_1_2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER_1_2].get_state(), 'leader')

        self.nodes[ROUTER_1_1].set_router_selection_jitter(1)
        self.nodes[ROUTER_1_1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER_1_1].get_state(), 'router')

        # Mesh Impacting Criteria - Highest Two-way link quality
        # REED_1_2 would attach to ROUTER_1_1
        # Attach to ROUTER_1_1 which has highest two-way link quality

        # Flush relative message queues
        self.flush_nodes([LEADER_1_2, ROUTER_1_1])

        self.nodes[LEADER_1_2].set_link_quality(self.nodes[REED_1_2].get_addr64(), 2)
        self.nodes[REED_1_2].set_router_selection_jitter(1)
        self.nodes[REED_1_2].set_router_upgrade_threshold(1)
        self.nodes[REED_1_2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[REED_1_2].get_state(), 'child')

        # Check Parent Response
        messages = self.simulator.get_messages_sent_by(ROUTER_1_1)
        parent_prefer = messages.next_mle_message(mle.CommandType.PARENT_RESPONSE)
        assert (parent_prefer), "Error: Expected parent response not found"

        messages = self.simulator.get_messages_sent_by(LEADER_1_2)
        parent_cmp = messages.next_mle_message(mle.CommandType.PARENT_RESPONSE)
        assert (parent_cmp), "Error: Expected parent response not found"

        # Known that link margin for link quality 3 is 80 and link quality 2 is 15
        assert ((parent_prefer.get_mle_message_tlv(mle.LinkMargin).link_margin -
                 parent_cmp.get_mle_message_tlv(mle.LinkMargin).link_margin) > 20)

        # Check Child Id Request
        messages = self.simulator.get_messages_sent_by(REED_1_2)
        msg = messages.next_mle_message(mle.CommandType.CHILD_ID_REQUEST)
        msg.assertSentToNode(self.nodes[ROUTER_1_1])

        # Mesh Impacting Criteria - Active Routers over REEDs
        # ROUTER_1_2 would attach to LEADER_1_2
        # Link quality configuration, so that REED_1_2 has the chance to respond

        # Flush relative message queues
        self.flush_nodes([LEADER_1_2, REED_1_2])

        self.nodes[LEADER_1_2].set_link_quality(self.nodes[ROUTER_1_2].get_addr64(), 2)
        self.nodes[REED_1_2].set_link_quality(self.nodes[ROUTER_1_2].get_addr64(), 2)
        self.nodes[ROUTER_1_2].set_router_selection_jitter(1)
        self.nodes[ROUTER_1_2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER_1_2].get_state(), 'router')

        # Check Parent Response
        messages = self.simulator.get_messages_sent_by(LEADER_1_2)

        # Skip first response for first parent request
        assert messages.next_mle_message(mle.CommandType.PARENT_RESPONSE)

        parent_prefer = messages.next_mle_message(mle.CommandType.PARENT_RESPONSE)
        assert (parent_prefer), "Error: Expected parent response not found"

        messages = self.simulator.get_messages_sent_by(REED_1_2)
        parent_cmp = messages.next_mle_message(mle.CommandType.PARENT_RESPONSE)
        assert (parent_cmp), "Error: Expected parent response not found"

        assert (parent_prefer.get_mle_message_tlv(mle.LinkMargin).link_margin == parent_cmp.get_mle_message_tlv(
            mle.LinkMargin).link_margin)

        # Check Child Id Request
        messages = self.simulator.get_messages_sent_by(ROUTER_1_2)
        msg = messages.next_mle_message(mle.CommandType.CHILD_ID_REQUEST)
        msg.assertSentToNode(self.nodes[LEADER_1_2])

        # Mesh Impacting Criteria - Highest Parent Priority value in the Connectivity TLV
        # REED_1_1 would attach to ROUTER_1_2

        # Flush relative message queues
        self.flush_nodes([LEADER_1_2, ROUTER_1_2])

        self.nodes[ROUTER_1_2].set_parent_priority(1)
        self.nodes[REED_1_1].set_router_selection_jitter(1)
        self.nodes[REED_1_1].set_router_upgrade_threshold(1)
        self.nodes[REED_1_1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[REED_1_1].get_state(), 'child')

        # Check Parent Response
        messages = self.simulator.get_messages_sent_by(ROUTER_1_2)
        parent_prefer = messages.next_mle_message(mle.CommandType.PARENT_RESPONSE)
        assert (parent_prefer), "Error: Expected parent response not found"

        messages = self.simulator.get_messages_sent_by(LEADER_1_2)
        parent_cmp = messages.next_mle_message(mle.CommandType.PARENT_RESPONSE)
        assert (parent_cmp), "Error: Expected parent response not found"

        assert (parent_prefer.get_mle_message_tlv(mle.LinkMargin).link_margin == parent_cmp.get_mle_message_tlv(
            mle.LinkMargin).link_margin)

        assert (parent_prefer.get_mle_message_tlv(mle.Connectivity).pp > parent_cmp.get_mle_message_tlv(
            mle.Connectivity).pp)

        # Check Child Id Request
        messages = self.simulator.get_messages_sent_by(REED_1_1)
        msg = messages.next_mle_message(mle.CommandType.CHILD_ID_REQUEST)
        msg.assertSentToNode(self.nodes[ROUTER_1_2])

        # Mesh Impacting Criteria - Router with the most high-quality neighbors
        # (Link Quality 3 field in the Connectivity TLV)
        # MED_1_1 would attach to LEADER_1_2

        self.nodes[REED_1_1].set_state('router')
        self.simulator.go(5)
        self.assertEqual(self.nodes[REED_1_1].get_state(), 'router')

        # Flush relative message queues
        self.flush_nodes([LEADER_1_2, ROUTER_1_1])

        self.nodes[MED_1_1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MED_1_1].get_state(), 'child')

        # Check Parent Response
        messages = self.simulator.get_messages_sent_by(LEADER_1_2)
        parent_prefer = messages.next_mle_message(mle.CommandType.PARENT_RESPONSE)
        assert (parent_prefer), "Error: Expected parent response not found"

        messages = self.simulator.get_messages_sent_by(ROUTER_1_1)
        parent_cmp = messages.next_mle_message(mle.CommandType.PARENT_RESPONSE)
        assert (parent_cmp), "Error: Expected parent response not found"

        assert (parent_prefer.get_mle_message_tlv(mle.LinkMargin).link_margin == parent_cmp.get_mle_message_tlv(
            mle.LinkMargin).link_margin)
        assert (parent_prefer.get_mle_message_tlv(mle.Connectivity).pp == parent_cmp.get_mle_message_tlv(
            mle.Connectivity).pp)
        assert (parent_prefer.get_mle_message_tlv(mle.Connectivity).link_quality_3 > parent_cmp.get_mle_message_tlv(
            mle.Connectivity).link_quality_3)

        # Check Child Id Request
        messages = self.simulator.get_messages_sent_by(MED_1_1)
        msg = messages.next_mle_message(mle.CommandType.CHILD_ID_REQUEST)
        msg.assertSentToNode(self.nodes[LEADER_1_2])

        # Child Impacting Criteria - A Version number in the Version TLV
        # equal to or higher than the version that implements features
        # desirable to the Child MED_1_2 would attach to ROUTER_1_2

        # Flush relative message queues
        self.flush_nodes([ROUTER_1_2, ROUTER_1_1])

        self.nodes[ROUTER_1_1].set_parent_priority(1)
        self.nodes[MED_1_2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MED_1_2].get_state(), 'child')

        # Check Parent Response
        messages = self.simulator.get_messages_sent_by(ROUTER_1_2)
        parent_prefer = messages.next_mle_message(mle.CommandType.PARENT_RESPONSE)
        assert (parent_prefer), "Error: Expected parent response not found"

        messages = self.simulator.get_messages_sent_by(ROUTER_1_1)
        parent_cmp = messages.next_mle_message(mle.CommandType.PARENT_RESPONSE)
        assert (parent_cmp), "Error: Expected parent response not found"

        assert (parent_prefer.get_mle_message_tlv(mle.LinkMargin).link_margin == parent_cmp.get_mle_message_tlv(
            mle.LinkMargin).link_margin)
        assert (parent_prefer.get_mle_message_tlv(mle.Connectivity).pp == parent_cmp.get_mle_message_tlv(
            mle.Connectivity).pp)
        assert (parent_prefer.get_mle_message_tlv(mle.Connectivity).link_quality_3 == parent_cmp.get_mle_message_tlv(
            mle.Connectivity).link_quality_3)
        assert (parent_prefer.get_mle_message_tlv(mle.Version).version > parent_cmp.get_mle_message_tlv(
            mle.Version).version)

        # Check Child Id Request
        messages = self.simulator.get_messages_sent_by(MED_1_2)
        msg = messages.next_mle_message(mle.CommandType.CHILD_ID_REQUEST)
        msg.assertSentToNode(self.nodes[ROUTER_1_2])


if __name__ == '__main__':
    unittest.main()
