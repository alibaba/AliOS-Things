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
from pktverify.consts import MLE_ADVERTISEMENT, MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_UPDATE_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, MLE_LINK_REQUEST, MLE_LINK_ACCEPT, MLE_LINK_ACCEPT_AND_REQUEST, ADDR_SOL_URI, SOURCE_ADDRESS_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MLE_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV, NL_MAC_EXTENDED_ADDRESS_TLV, NL_RLOC16_TLV, NL_STATUS_TLV, NL_ROUTER_MASK_TLV, COAP_CODE_ACK
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
ROUTER1 = 2
ROUTER2 = 3

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate that when the DUT sees a new router for the first time, it will synchronize using the New Router Neighbor Synchronization procedure.
#
# Test Topology:
# -------------
#     Leader
#      /  \
# Router2  Router1[DUT]
#
# DUT Types:
# ----------
#  Router


class Cert_5_1_12_NewRouterSync(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1, ROUTER2]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'router')

        self.simulator.go(10)

        self.nodes[ROUTER1].add_allowlist(self.nodes[ROUTER2].get_addr64())
        self.nodes[ROUTER2].add_allowlist(self.nodes[ROUTER1].get_addr64())

        self.simulator.go(35)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_2 = pv.vars['ROUTER_2']

        # Step 1: Verify topology is formed correctly.

        # Step 2: The DUT MUST send properly formatted MLE Advertisements with
        #         an IP Hop Limit of 255 to the Link-Local All Nodes multicast
        #         address (FF02::1).
        #          The following TLVs MUST be present in the MLE Advertisements:
        #              - Leader Data TLV
        #              - Route64 TLV
        #              - Source Address TLV

        pv.verify_attached('ROUTER_1')
        pkts.filter_wpan_src64(ROUTER_1).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p: {
                              LEADER_DATA_TLV,
                              ROUTE64_TLV,
                              SOURCE_ADDRESS_TLV
                              } == set(p.mle.tlv.type) and\
                   p.ipv6.hlim == 255).\
            must_next()

        pv.verify_attached('ROUTER_2')
        pkts.filter_wpan_src64(ROUTER_2).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p: {
                              LEADER_DATA_TLV,
                              ROUTE64_TLV,
                              SOURCE_ADDRESS_TLV
                              } == set(p.mle.tlv.type) and\
                   p.ipv6.hlim == 255).\
            must_next()

        # Step 4: The DUT and Router_2 exchange unicast Link Request and unicast
        #         Link Accept messages OR Link Accept and Request messages.
        #
        #         The Link Request Message MUST be unicast and contain
        #         the following TLVs:
        #             - Challenge TLV
        #             - Leader Data TLV
        #             - Source Address TLV
        #             - Version TLV
        #             - TLV Request TLV: Link Margin
        #
        #         Link Accept or Link Accept And Request Messages MUST be
        #         Unicast.
        #         The following TLVs MUST be present in the Link Accept or
        #         Link Accept And Request Messages :
        #             - Leader Data TLV
        #             - Link-layer Frame Counter TLV
        #             - Link Margin TLV
        #             - Response TLV
        #             - Source Address TLV
        #             - Version TLV
        #             - TLV Request TLV: Link Margin
        #             - Challenge TLV (optional)
        #             - MLE Frame Counter TLV (optional)
        #         The Challenge TLV and TLV Request TLV MUST be included
        #         if the response is an Accept and Request message.

        lq_src = ROUTER_1
        lq_dst = ROUTER_2
        _pkt = pkts.filter_mle_cmd(MLE_LINK_REQUEST).\
                filter(lambda p: {
                                  CHALLENGE_TLV,
                                  LEADER_DATA_TLV,
                                  SOURCE_ADDRESS_TLV,
                                  VERSION_TLV,
                                  TLV_REQUEST_TLV,
                                  LINK_MARGIN_TLV
                                  } <= set(p.mle.tlv.type) and\
                       p.mle.tlv.link_margin is nullField and\
                       (p.wpan.src64 == ROUTER_1 or\
                        p.wpan.src64 == ROUTER_2)
                       ).\
                must_next()
        if _pkt.wpan.src64 != ROUTER_1:
            _pkt.must_verify(lambda p: p.wpan.dst64 == ROUTER_1)
            lq_src = ROUTER_2
            lq_dst = ROUTER_1

        _pkt = pkts.filter_wpan_src64(lq_dst).\
                filter_wpan_dst64(lq_src).\
                filter_mle_cmd2(MLE_LINK_ACCEPT, MLE_LINK_ACCEPT_AND_REQUEST).\
                filter(lambda p: {
                                  LEADER_DATA_TLV,
                                  LINK_LAYER_FRAME_COUNTER_TLV,
                                  LINK_MARGIN_TLV,
                                  RESPONSE_TLV,
                                  SOURCE_ADDRESS_TLV,
                                  VERSION_TLV
                                  } <= set(p.mle.tlv.type) and\
                       p.mle.tlv.link_margin is not nullField
                       ).\
                       must_next()
        if _pkt.mle.cmd == MLE_LINK_ACCEPT_AND_REQUEST:
            _pkt.must_verify(lambda p: {CHALLENGE_TLV, TLV_REQUEST_TLV} <= set(p.mle.tlv.type))


if __name__ == '__main__':
    unittest.main()
