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
import network_layer

LEADER = 1
ROUTER_1 = 2


class Router_5_1_01(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'version': '1.2'
        },
        ROUTER_1: {
            'version': '1.2'
        },
    }
    """All nodes are created with default configurations"""

    def test(self):
        self.nodes[ROUTER_1].set_router_selection_jitter(1)

        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER_1].start()
        self.simulator.go(7)
        self.assertEqual(self.nodes[ROUTER_1].get_state(), 'router')

        leader_messages = self.simulator.get_messages_sent_by(LEADER)
        router_messages = self.simulator.get_messages_sent_by(ROUTER_1)

        # 1 - Leader transmits MLE advertisements
        msg = leader_messages.next_mle_message(mle.CommandType.ADVERTISEMENT)
        msg.assertSentWithHopLimit(255)
        msg.assertSentToDestinationAddress('ff02::1')
        msg.assertMleMessageContainsTlv(mle.SourceAddress)
        msg.assertMleMessageContainsTlv(mle.LeaderData)
        msg.assertMleMessageContainsTlv(mle.Route64)

        # 2 - Router_1 begins attach process by sending a multicast MLE Parent Request
        msg = router_messages.next_mle_message(mle.CommandType.PARENT_REQUEST)
        msg.assertSentWithHopLimit(255)
        msg.assertSentToDestinationAddress('ff02::2')
        msg.assertMleMessageContainsTlv(mle.Mode)
        msg.assertMleMessageContainsTlv(mle.Challenge)
        msg.assertMleMessageContainsTlv(mle.ScanMask)
        msg.assertMleMessageContainsTlv(mle.Version)
        assert msg.get_mle_message_tlv(mle.Version).version == 3

        scan_mask_tlv = msg.get_mle_message_tlv(mle.ScanMask)
        self.assertEqual(1, scan_mask_tlv.router)
        self.assertEqual(0, scan_mask_tlv.end_device)

        # 3 - Leader sends a MLE Parent Response
        msg = leader_messages.next_mle_message(mle.CommandType.PARENT_RESPONSE)
        msg.assertSentToNode(self.nodes[ROUTER_1])
        msg.assertMleMessageContainsTlv(mle.SourceAddress)
        msg.assertMleMessageContainsTlv(mle.LeaderData)
        msg.assertMleMessageContainsTlv(mle.LinkLayerFrameCounter)
        msg.assertMleMessageContainsOptionalTlv(mle.MleFrameCounter)
        msg.assertMleMessageContainsTlv(mle.Response)
        msg.assertMleMessageContainsTlv(mle.Challenge)
        msg.assertMleMessageContainsTlv(mle.LinkMargin)
        msg.assertMleMessageContainsTlv(mle.Connectivity)
        msg.assertMleMessageContainsTlv(mle.Version)
        assert msg.get_mle_message_tlv(mle.Version).version == 3

        # 4 - Router_1 receives the MLE Parent Response and sends a Child ID Request
        msg = router_messages.next_mle_message(mle.CommandType.CHILD_ID_REQUEST)
        msg.assertSentToNode(self.nodes[LEADER])
        msg.assertMleMessageContainsTlv(mle.Response)
        msg.assertMleMessageContainsTlv(mle.LinkLayerFrameCounter)
        msg.assertMleMessageContainsOptionalTlv(mle.MleFrameCounter)
        msg.assertMleMessageContainsTlv(mle.Mode)
        msg.assertMleMessageContainsTlv(mle.Timeout)
        msg.assertMleMessageContainsTlv(mle.Version)
        msg.assertMleMessageContainsTlv(mle.TlvRequest)
        msg.assertMleMessageDoesNotContainTlv(mle.AddressRegistration)
        assert msg.get_mle_message_tlv(mle.Version).version == 3

        # 5 - Leader responds with a Child ID Response
        msg = leader_messages.next_mle_message(mle.CommandType.CHILD_ID_RESPONSE)
        msg.assertSentToNode(self.nodes[ROUTER_1])
        msg.assertMleMessageContainsTlv(mle.SourceAddress)
        msg.assertMleMessageContainsTlv(mle.LeaderData)
        msg.assertMleMessageContainsTlv(mle.Address16)
        msg.assertMleMessageContainsOptionalTlv(mle.NetworkData)
        msg.assertMleMessageContainsOptionalTlv(mle.Route64)
        msg.assertMleMessageContainsOptionalTlv(mle.AddressRegistration)

        # 6 - Router_1 sends an Address Solicit Request
        msg = router_messages.next_coap_message('0.02')
        msg.assertCoapMessageRequestUriPath('/a/as')
        msg.assertCoapMessageContainsTlv(network_layer.MacExtendedAddress)
        msg.assertCoapMessageContainsTlv(network_layer.Status)

        # 7 - Leader sends an Address Solicit Response
        msg = leader_messages.next_coap_message('2.04')
        msg.assertCoapMessageContainsTlv(network_layer.Status)
        msg.assertCoapMessageContainsOptionalTlv(network_layer.RouterMask)

        status_tlv = msg.get_coap_message_tlv(network_layer.Status)
        self.assertEqual(network_layer.StatusValues.SUCCESS, status_tlv.status)

        # 8 - Router_1 sends a multicast Link Request Message
        msg = router_messages.next_mle_message(mle.CommandType.LINK_REQUEST)
        msg.assertMleMessageContainsTlv(mle.SourceAddress)
        msg.assertMleMessageContainsTlv(mle.LeaderData)
        msg.assertMleMessageContainsTlv(mle.Challenge)
        msg.assertMleMessageContainsTlv(mle.Version)
        msg.assertMleMessageContainsTlv(mle.TlvRequest)
        assert msg.get_mle_message_tlv(mle.Version).version >= 3

        tlv_request = msg.get_mle_message_tlv(mle.TlvRequest)
        self.assertIn(mle.TlvType.LINK_MARGIN, tlv_request.tlvs)

        # 9 - Leader sends a Unicast Link Accept
        msg = leader_messages.next_mle_message(mle.CommandType.LINK_ACCEPT_AND_REQUEST)
        msg.assertMleMessageContainsTlv(mle.SourceAddress)
        msg.assertMleMessageContainsTlv(mle.LeaderData)
        msg.assertMleMessageContainsTlv(mle.Response)
        msg.assertMleMessageContainsTlv(mle.LinkLayerFrameCounter)
        msg.assertMleMessageContainsTlv(mle.Version)
        msg.assertMleMessageContainsTlv(mle.LinkMargin)
        msg.assertMleMessageContainsOptionalTlv(mle.MleFrameCounter)
        msg.assertMleMessageContainsOptionalTlv(mle.Challenge)
        assert msg.get_mle_message_tlv(mle.Version).version >= 3

        # 10 - Router_1 Transmit MLE advertisements
        msg = router_messages.next_mle_message(mle.CommandType.ADVERTISEMENT)
        msg.assertSentWithHopLimit(255)
        msg.assertSentToDestinationAddress('ff02::1')
        msg.assertMleMessageContainsTlv(mle.SourceAddress)
        msg.assertMleMessageContainsTlv(mle.LeaderData)
        msg.assertMleMessageContainsTlv(mle.Route64)

        # 11 - Verify connectivity by sending an ICMPv6 Echo Request to the DUT link local address
        self.assertTrue(self.nodes[LEADER].ping(self.nodes[ROUTER_1].get_linklocal()))
        self.assertTrue(self.nodes[ROUTER_1].ping(self.nodes[LEADER].get_linklocal()))


if __name__ == '__main__':
    unittest.main()
