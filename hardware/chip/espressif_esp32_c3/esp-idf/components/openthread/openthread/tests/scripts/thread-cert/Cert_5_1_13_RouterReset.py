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
import thread_cert
from pktverify.consts import MLE_ADVERTISEMENT, MLE_LINK_REQUEST, MLE_LINK_ACCEPT, MLE_LINK_ACCEPT_AND_REQUEST, SOURCE_ADDRESS_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, TLV_REQUEST_TLV, VERSION_TLV, MLE_MAX_RESPONSE_DELAY
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
ROUTER = 2

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate that when a router resets,
# it will synchronize upon returning by using the Router Synchronization
# after Reset procedure.
#
# Test Topology:
# -------------
#     Leader
#       |
#     Router
#
# DUT Types:
# ----------
#  Leader
#  Router


class Cert_5_1_13_RouterReset(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
    }

    def _setUpRouter(self):
        self.nodes[ROUTER].add_allowlist(self.nodes[LEADER].get_addr64())
        self.nodes[ROUTER].enable_allowlist()
        self.nodes[ROUTER].set_router_selection_jitter(1)

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')
        self.collect_rloc16s()

        self.nodes[ROUTER].reset()
        self._setUpRouter()
        self.simulator.go(5)

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']
        ROUTER_RLOC16 = pv.vars['ROUTER_RLOC16']

        # Step 1: Verify topology is formed correctly.

        # Step 2: Devices MUST send properly formatted MLE Advertisements with
        #         an IP Hop Limit of 255 to the Link-Local All Nodes multicast
        #         address (FF02::1).
        #          The following TLVs MUST be present in the MLE Advertisements:
        #              - Leader Data TLV
        #              - Route64 TLV
        #              - Source Address TLV

        pv.verify_attached('ROUTER')
        pkts.filter_wpan_src64(ROUTER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p: {
                              LEADER_DATA_TLV,
                              ROUTE64_TLV,
                              SOURCE_ADDRESS_TLV
                              } == set(p.mle.tlv.type) and\
                   p.ipv6.hlim == 255
                   ).\
            must_next()
        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p: {
                              LEADER_DATA_TLV,
                              ROUTE64_TLV,
                              SOURCE_ADDRESS_TLV
                              } == set(p.mle.tlv.type) and\
                   p.ipv6.hlim == 255
                   ).\
            must_next()

        # Step 4: Router sends multicast Link Request message
        #
        #         The Link Request Message MUST be sent to the Link-Local All Routers
        #         Routers multicast address (FF02::2) and contain the following TLVs:
        #             - Challenge TLV
        #             - TLV Request TLV
        #                 - Address16 TLV
        #                 - Route64 TLV
        #             - Version TLV
        #

        _pkt_lq = pkts.filter_mle_cmd(MLE_LINK_REQUEST).\
                filter_LLARMA().\
                filter(lambda p: {
                                  CHALLENGE_TLV,
                                  VERSION_TLV,
                                  TLV_REQUEST_TLV,
                                  ADDRESS16_TLV,
                                  ROUTE64_TLV
                                  } <= set(p.mle.tlv.type) and\
                       p.mle.tlv.addr16 is nullField and \
                       p.mle.tlv.route64.id_mask is nullField
                       ).\
                must_next()

        # Step 5: Leader replies to Router with Link Accept message
        #         The following TLVs MUST be present in the Link Accept message:
        #             - Leader Data TLV
        #             - Link-layer Frame Counter TLV
        #             - Link Margin TLV
        #             - Response TLV
        #             - Source Address TLV
        #             - Version TLV
        #             - TLV Request TLV: Link Margin
        #             - Challenge TLV (optional)
        #             - MLE Frame Counter TLV (optional)
        #         The Challenge TLV MUST be included if the response is an
        #         Accept and Request message.
        #         Responses to multicast Link Requests MUST be delayed by a
        #         random time of up to MLE_MAX_RESPONSE_DELAY (1 second).

        _pkt = pkts.filter_wpan_src64(LEADER).\
                filter_wpan_dst64(ROUTER).\
                filter_mle_cmd2(MLE_LINK_ACCEPT, MLE_LINK_ACCEPT_AND_REQUEST).\
                filter(lambda p: {
                                  ADDRESS16_TLV,
                                  LEADER_DATA_TLV,
                                  LINK_LAYER_FRAME_COUNTER_TLV,
                                  RESPONSE_TLV,
                                  ROUTE64_TLV,
                                  SOURCE_ADDRESS_TLV,
                                  VERSION_TLV
                                   } <= set(p.mle.tlv.type) and\
                       p.mle.tlv.addr16 is not nullField and \
                       p.mle.tlv.route64.id_mask is not nullField and\
                       p.mle.tlv.addr16 == ROUTER_RLOC16 and\
                       p.sniff_timestamp - _pkt_lq.sniff_timestamp <
                       MLE_MAX_RESPONSE_DELAY + 0.1
                       ).\
                must_next()
        if _pkt.mle.cmd == MLE_LINK_ACCEPT_AND_REQUEST:
            _pkt.must_verify(lambda p: [CHALLENGE_TLV] in p.mle.tlv.type)


if __name__ == '__main__':
    unittest.main()
