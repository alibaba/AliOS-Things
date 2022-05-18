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

LEADER = 1
SED_1 = 2

CHILD_TIMEOUT = 30

DEFAULT_POLL_PERIOD = CHILD_TIMEOUT - 4
"""The default poll period calculated by ot::Mac::DataPollSender::GetDefaultPollPeriod()."""

USER_POLL_PERIOD = CHILD_TIMEOUT // 3
"""The poll period explicitly set by this test for verifying enhanced keep-alive."""


class SED_EnhancedKeepAlive(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'version': '1.2'
        },
        SED_1: {
            'mode': '-',
            'version': '1.2',
        },
    }
    """All nodes are created with default configurations"""

    def test(self):
        self.nodes[SED_1].set_timeout(CHILD_TIMEOUT)
        self.nodes[SED_1].set_pollperiod(USER_POLL_PERIOD * 1000)

        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[SED_1].start()
        self.simulator.go(7)
        self.assertEqual(self.nodes[SED_1].get_state(), 'child')

        leader_messages = self.simulator.get_messages_sent_by(LEADER)
        sed_messages = self.simulator.get_messages_sent_by(SED_1)

        # 1 - Leader transmits MLE advertisements
        msg = leader_messages.next_mle_message(mle.CommandType.ADVERTISEMENT)
        msg.assertSentWithHopLimit(255)
        msg.assertSentToDestinationAddress('ff02::1')
        msg.assertMleMessageContainsTlv(mle.SourceAddress)
        msg.assertMleMessageContainsTlv(mle.LeaderData)
        msg.assertMleMessageContainsTlv(mle.Route64)

        # 2 - SED_1 begins attach process by sending a multicast MLE Parent Request
        msg = sed_messages.next_mle_message(mle.CommandType.PARENT_REQUEST)
        msg.assertSentWithHopLimit(255)
        msg.assertSentToDestinationAddress('ff02::2')
        msg.assertMleMessageContainsTlv(mle.Mode)
        msg.assertMleMessageContainsTlv(mle.Challenge)
        msg.assertMleMessageContainsTlv(mle.ScanMask)
        msg.assertMleMessageContainsTlv(mle.Version)
        self.assertEqual(msg.get_mle_message_tlv(mle.Version).version, 3)

        scan_mask_tlv = msg.get_mle_message_tlv(mle.ScanMask)
        self.assertEqual(1, scan_mask_tlv.router)
        self.assertEqual(0, scan_mask_tlv.end_device)

        # 3 - Leader sends a MLE Parent Response
        msg = leader_messages.next_mle_message(mle.CommandType.PARENT_RESPONSE)
        msg.assertSentToNode(self.nodes[SED_1])
        msg.assertMleMessageContainsTlv(mle.SourceAddress)
        msg.assertMleMessageContainsTlv(mle.LeaderData)
        msg.assertMleMessageContainsTlv(mle.LinkLayerFrameCounter)
        msg.assertMleMessageContainsOptionalTlv(mle.MleFrameCounter)
        msg.assertMleMessageContainsTlv(mle.Response)
        msg.assertMleMessageContainsTlv(mle.Challenge)
        msg.assertMleMessageContainsTlv(mle.LinkMargin)
        msg.assertMleMessageContainsTlv(mle.Connectivity)
        msg.assertMleMessageContainsTlv(mle.Version)
        self.assertEqual(msg.get_mle_message_tlv(mle.Version).version, 3)

        # 4 - SED_1 receives the MLE Parent Response and sends a Child ID Request
        msg = sed_messages.next_mle_message(mle.CommandType.CHILD_ID_REQUEST)
        msg.assertSentToNode(self.nodes[LEADER])
        msg.assertMleMessageContainsTlv(mle.Response)
        msg.assertMleMessageContainsTlv(mle.LinkLayerFrameCounter)
        msg.assertMleMessageContainsOptionalTlv(mle.MleFrameCounter)
        msg.assertMleMessageContainsTlv(mle.Mode)
        msg.assertMleMessageContainsTlv(mle.Timeout)
        msg.assertMleMessageContainsTlv(mle.Version)
        msg.assertMleMessageContainsTlv(mle.TlvRequest)
        self.assertEqual(msg.get_mle_message_tlv(mle.Version).version, 3)

        # 5 - Leader responds with a Child ID Response
        msg = leader_messages.next_mle_message(mle.CommandType.CHILD_ID_RESPONSE)
        msg.assertSentToNode(self.nodes[SED_1])
        msg.assertMleMessageContainsTlv(mle.SourceAddress)
        msg.assertMleMessageContainsTlv(mle.LeaderData)
        msg.assertMleMessageContainsTlv(mle.Address16)
        msg.assertMleMessageContainsOptionalTlv(mle.NetworkData)
        msg.assertMleMessageContainsOptionalTlv(mle.Route64)
        msg.assertMleMessageContainsOptionalTlv(mle.AddressRegistration)

        leader_aloc = self.nodes[LEADER].get_addr_leader_aloc()
        self.assertTrue(self.nodes[SED_1].ping(leader_aloc, timeout=USER_POLL_PERIOD * 2))

        # 6 - Timeout Child
        self.nodes[LEADER].enable_allowlist()
        self.nodes[SED_1].enable_allowlist()
        self.nodes[SED_1].set_pollperiod(CHILD_TIMEOUT * 1000 * 2)
        self.simulator.go(CHILD_TIMEOUT + 1)
        self.assertEqual(self.nodes[SED_1].get_state(), 'child')
        self.nodes[SED_1].set_pollperiod(USER_POLL_PERIOD * 1000)
        self.nodes[LEADER].disable_allowlist()
        self.nodes[SED_1].disable_allowlist()
        self.assertFalse(self.nodes[SED_1].ping(leader_aloc, timeout=USER_POLL_PERIOD * 2))
        self.flush_all()

        # 7 - Wait SED_1 to re-attach
        self.simulator.go(240)
        leader_messages = self.simulator.get_messages_sent_by(LEADER)
        msg = leader_messages.next_mle_message(mle.CommandType.CHILD_ID_RESPONSE)
        msg.assertSentToNode(self.nodes[SED_1])
        msg.assertMleMessageContainsTlv(mle.SourceAddress)
        msg.assertMleMessageContainsTlv(mle.LeaderData)
        msg.assertMleMessageContainsTlv(mle.Address16)
        msg.assertMleMessageContainsOptionalTlv(mle.NetworkData)
        msg.assertMleMessageContainsOptionalTlv(mle.Route64)
        msg.assertMleMessageContainsOptionalTlv(mle.AddressRegistration)
        self.assertTrue(self.nodes[SED_1].ping(leader_aloc, timeout=USER_POLL_PERIOD * 2))
        self.flush_all()

        # 8 - Verify enhanced keep-alive works
        self.nodes[LEADER].enable_allowlist()
        self.nodes[SED_1].enable_allowlist()
        self.nodes[SED_1].set_pollperiod(CHILD_TIMEOUT * 1000 * 2)
        self.simulator.go(CHILD_TIMEOUT // 2)
        self.assertEqual(self.nodes[SED_1].get_state(), 'child')
        self.nodes[LEADER].disable_allowlist()
        self.nodes[SED_1].disable_allowlist()
        non_exist_addr = leader_aloc.replace('fc00', 'fc12')
        self.assertFalse(self.nodes[SED_1].ping(non_exist_addr))
        self.nodes[LEADER].enable_allowlist()
        self.nodes[SED_1].enable_allowlist()
        self.simulator.go(CHILD_TIMEOUT // 2)
        self.nodes[LEADER].disable_allowlist()
        self.nodes[SED_1].disable_allowlist()
        self.nodes[SED_1].set_pollperiod(USER_POLL_PERIOD * 1000)
        self.assertTrue(self.nodes[SED_1].ping(leader_aloc, timeout=USER_POLL_PERIOD * 2))

        # 9 - Verify child resets keep-alive timer
        self.nodes[SED_1].set_pollperiod(DEFAULT_POLL_PERIOD * 1000)
        self.simulator.go(DEFAULT_POLL_PERIOD // 3 * 2)
        self.flush_all()
        self.nodes[SED_1].ping(leader_aloc, timeout=1)
        self.simulator.go(DEFAULT_POLL_PERIOD // 3 * 2)
        sed_messages = self.simulator.get_messages_sent_by(SED_1)
        self.assertEqual(sed_messages.next_data_poll(), None)


if __name__ == '__main__':
    unittest.main()
