#!/usr/bin/env python3
#
#  Copyright (c) 2016, The OpenThread Authors.
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
#

import unittest

import mle
import network_layer
import thread_cert
from pktverify.consts import MLE_PARENT_RESPONSE, MLE_CHILD_ID_RESPONSE, MLE_LINK_REQUEST, ADDR_SOL_URI, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, CONNECTIVITY_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, VERSION_TLV, TLV_REQUEST_TLV, LINK_MARGIN_TLV, SOURCE_ADDRESS_TLV, COAP_CODE_ACK, COAP_CODE_POST, NL_MAC_EXTENDED_ADDRESS_TLV, NL_STATUS_TLV, NL_RLOC16_TLV, NL_ROUTER_MASK_TLV
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ROUTER1 = 2

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify that after deallocating a Router ID,
# Leader as a DUT doesn't reassign the Router ID for at least ID_REUSE_DELAY
# seconds
#
# Test Topology:
# -------------
# Leader
#   |
# Router
#
# DUT Types:
# ----------
# Leader


class Cert_5_1_05_RouterAddressTimeout(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1]
        },
        ROUTER1: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
    }

    def _setUpRouter1(self):
        self.nodes[ROUTER1].add_allowlist(self.nodes[LEADER].get_addr64())
        self.nodes[ROUTER1].enable_allowlist()
        self.nodes[ROUTER1].set_router_selection_jitter(1)

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[ROUTER1].reset()
        self._setUpRouter1()
        self.simulator.go(200)
        self.nodes[ROUTER1].start()
        self.simulator.go(15)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[ROUTER1].reset()
        self._setUpRouter1()
        self.simulator.go(300)
        self.nodes[ROUTER1].start()
        self.simulator.go(15)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']
        MM = pv.vars['MM_PORT']

        # Step 1: Verify topology is formed correctly.

        pv.verify_attached('ROUTER')
        _pkt_as = pkts.filter_wpan_src64(LEADER).\
            filter_coap_ack(ADDR_SOL_URI, port=MM).\
            filter(lambda p: {
                              NL_STATUS_TLV,
                              NL_RLOC16_TLV,
                              NL_ROUTER_MASK_TLV
                              } == set(p.coap.tlv.type) and\
                   p.coap.code == COAP_CODE_ACK and\
                   p.thread_address.tlv.status == 0
                   ).\
            must_next()

        # Step 3: Router automatically sends a link request, reattaches and
        #         requests its original Router ID after reset within the
        #         ID_REUSE_DELAY interval

        pkts.filter_wpan_src64(ROUTER).\
            filter_LLARMA().\
            filter_mle_cmd(MLE_LINK_REQUEST).\
            filter(lambda p: {
                              CHALLENGE_TLV,
                              VERSION_TLV,
                              TLV_REQUEST_TLV,
                              ADDRESS16_TLV,
                              ROUTE64_TLV
                              } <= set(p.mle.tlv.type)\
                   ).\
            must_next()

        # Step 4: Leader MUST send a properly formatted Parent Response and
        #         Child ID Response to Router.
        #         And send Address Solicit Response Message to Router.
        #         The RLOC16 TLV in the Address Solicit Response message MUST
        #         contain a different Router ID then the one allocated in the
        #         original attach because ID_REUSE_DELAY interval has not
        #         timed out.
        #
        #         CoAP Response Code
        #             - 2.04 Changed
        #         CoAP Payload
        #             - Status TLV (value = Success)
        #             - RLOC16 TLV
        #             - Router Mask TLV

        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
            filter_mle_cmd(MLE_PARENT_RESPONSE).\
            filter(lambda p: {
                              CHALLENGE_TLV,
                              CONNECTIVITY_TLV,
                              LEADER_DATA_TLV,
                              LINK_LAYER_FRAME_COUNTER_TLV,
                              LINK_MARGIN_TLV,
                              RESPONSE_TLV,
                              SOURCE_ADDRESS_TLV,
                              VERSION_TLV
                             } <= set(p.mle.tlv.type)
                   ).\
            must_next()

        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            filter(lambda p: {
                              ADDRESS16_TLV,
                              LEADER_DATA_TLV,
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              ROUTE64_TLV
                              } <= set(p.mle.tlv.type) or\
                             {
                              ADDRESS16_TLV,
                              LEADER_DATA_TLV,
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV
                             } <= set(p.mle.tlv.type)\
                   ).\
            must_next()

        _pkt_as2 = pkts.filter_wpan_src64(ROUTER).\
            filter_coap_request(ADDR_SOL_URI, port=MM).\
            filter(lambda p: {
                              NL_MAC_EXTENDED_ADDRESS_TLV,
                              NL_RLOC16_TLV,
                              NL_STATUS_TLV
                              } == set(p.coap.tlv.type) and\
                   p.coap.code == COAP_CODE_POST and\
                   p.thread_address.tlv.rloc16 ==
                   _pkt_as.thread_address.tlv.rloc16
                   ).\
            must_next()

        _pkt_as3 = pkts.filter_wpan_src64(LEADER).\
            filter_coap_ack(ADDR_SOL_URI, port=MM).\
            filter(lambda p: {
                              NL_STATUS_TLV,
                              NL_RLOC16_TLV,
                              NL_ROUTER_MASK_TLV
                              } == set(p.coap.tlv.type) and\
                   p.coap.code == COAP_CODE_ACK and\
                   p.thread_address.tlv.rloc16 !=
                   _pkt_as2.thread_address.tlv.rloc16 and\
                   p.thread_address.tlv.status == 0
                   ).\
            must_next()

        # Step 6: Router automatically sends a link request, reattaches and
        #         requests its most recent Router ID after reset after the
        #         ID_REUSE_DELAY interval

        pkts.filter_wpan_src64(ROUTER).\
            filter_LLARMA().\
            filter_mle_cmd(MLE_LINK_REQUEST).\
            filter(lambda p: {
                              CHALLENGE_TLV,
                              VERSION_TLV,
                              TLV_REQUEST_TLV,
                              ADDRESS16_TLV,
                              ROUTE64_TLV
                              } <= set(p.mle.tlv.type)\
                   ).\
            must_next()

        # Step 7: Leader MUST send a properly formatted Parent Response and
        #         Child ID Response to Router.
        #         And send Address Solicit Response Message to Router.
        #         The RLOC16 TLV in the Address Solicit Response message MUST
        #         contain the requested Router ID
        #
        #         CoAP Response Code
        #             - 2.04 Changed
        #         CoAP Payload
        #             - Status TLV (value = Success)
        #             - RLOC16 TLV
        #             - Router Mask TLV

        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
            filter_mle_cmd(MLE_PARENT_RESPONSE).\
            filter(lambda p: {
                              CHALLENGE_TLV,
                              CONNECTIVITY_TLV,
                              LEADER_DATA_TLV,
                              LINK_LAYER_FRAME_COUNTER_TLV,
                              LINK_MARGIN_TLV,
                              RESPONSE_TLV,
                              SOURCE_ADDRESS_TLV,
                              VERSION_TLV
                             } <= set(p.mle.tlv.type)
                   ).\
            must_next()

        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            filter(lambda p: {
                              ADDRESS16_TLV,
                              LEADER_DATA_TLV,
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              ROUTE64_TLV
                              } <= set(p.mle.tlv.type) or\
                             {
                              ADDRESS16_TLV,
                              LEADER_DATA_TLV,
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV
                             } <= set(p.mle.tlv.type)\
                   ).\
            must_next()

        pkts.filter_wpan_src64(ROUTER).\
            filter_coap_request(ADDR_SOL_URI, port=MM).\
            filter(lambda p: {
                              NL_MAC_EXTENDED_ADDRESS_TLV,
                              NL_RLOC16_TLV,
                              NL_STATUS_TLV
                              } == set(p.coap.tlv.type) and\
                   p.coap.code == COAP_CODE_POST and\
                   p.thread_address.tlv.rloc16 ==
                   _pkt_as3.thread_address.tlv.rloc16
                   ).\
            must_next()

        pkts.filter_wpan_src64(LEADER).\
            filter_coap_ack(ADDR_SOL_URI, port=MM).\
            filter(lambda p: {
                              NL_STATUS_TLV,
                              NL_RLOC16_TLV,
                              NL_ROUTER_MASK_TLV
                              } == set(p.coap.tlv.type) and\
                   p.coap.code == COAP_CODE_ACK and\
                   p.thread_address.tlv.rloc16 ==
                   _pkt_as3.thread_address.tlv.rloc16 and\
                   p.thread_address.tlv.status == 0
                   ).\
            must_next()


if __name__ == '__main__':
    unittest.main()
