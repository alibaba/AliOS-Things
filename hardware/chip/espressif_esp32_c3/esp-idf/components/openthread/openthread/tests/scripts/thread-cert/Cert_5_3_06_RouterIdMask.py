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

import config
import thread_cert
from pktverify.consts import MLE_ADVERTISEMENT, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE
from pktverify.packet_verifier import PacketVerifier

DUT_LEADER = 1
ROUTER1 = 2
ROUTER2 = 3

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify that the router ID mask is managed
# correctly, as the connectivity to a router or group of routers is lost and / or
# a new router is added to network.
#
# Test Topology:
# -------------
# Leader(DUT)
#    |
# Router_1
#    |
# Router_2
#
# DUT Types:
# ----------
#  Leader


class Cert_5_3_6_RouterIdMask(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        DUT_LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [DUT_LEADER, ROUTER2]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [ROUTER1]
        },
    }

    def _setUpRouter2(self):
        self.nodes[ROUTER2].add_allowlist(self.nodes[ROUTER1].get_addr64())
        self.nodes[ROUTER2].enable_allowlist()
        self.nodes[ROUTER2].set_router_selection_jitter(1)

    def test(self):
        # 1
        self.nodes[DUT_LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT_LEADER].get_state(), 'leader')

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'router')

        self.collect_rloc16s()

        # Wait DUT_LEADER to establish routing to ROUTER2 via ROUTER1's MLE
        # advertisement.
        self.simulator.go(config.MAX_ADVERTISEMENT_INTERVAL)

        # 2
        self.nodes[ROUTER2].reset()
        self._setUpRouter2()

        # 3 & 4

        self.simulator.go(720)

        # 5

        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'router')

        self.simulator.go(config.MAX_ADVERTISEMENT_INTERVAL)

        # 6
        self.nodes[ROUTER1].reset()
        self.nodes[ROUTER2].reset()

        self.simulator.go(720)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_2 = pv.vars['ROUTER_2']

        leader_rid = pv.vars['LEADER_RLOC16'] >> 10
        router_1_rid = pv.vars['ROUTER_1_RLOC16'] >> 10
        router_2_rid = pv.vars['ROUTER_2_RLOC16'] >> 10

        # Step 1: Ensure topology is formed correctly
        pv.verify_attached('ROUTER_1', 'LEADER')
        pv.verify_attached('ROUTER_2', 'ROUTER_1')

        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p:
                   {1,2,1} == set(p.mle.tlv.route64.cost) and\
                   {leader_rid, router_1_rid, router_2_rid} ==
                   p.mle.tlv.route64.id_mask
                   ).\
            must_next()

        # Step 4: The DUT’s routing cost to Router_2 MUST count to infinity
        #         The DUT MUST remove Router_2 ID from its ID set
        #         Verify route data has settled
        _pkt = pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p: {1,0,1} == set(p.mle.tlv.route64.cost)).\
            must_next()
        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p:
                   {1,1} == set(p.mle.tlv.route64.cost) and\
                   {leader_rid, router_1_rid} ==
                   p.mle.tlv.route64.id_mask
                   ).\
            must_next()

        # Step 5: Re-attach Router_2 to Router_1.
        #         The DUT MUST reset the MLE Advertisement trickle timer and
        #         send an Advertisement
        pv.verify_attached('ROUTER_2', 'ROUTER_1')
        # check trickle timer between the successive advertisements
        with pkts.save_index():
            _pkt = pkts.filter_wpan_src64(LEADER).\
                filter_LLANMA().\
                filter_mle_cmd(MLE_ADVERTISEMENT).\
                must_next()
            pkts.filter_wpan_src64(LEADER).\
                filter_LLANMA().\
                filter_mle_cmd(MLE_ADVERTISEMENT).\
                filter(lambda p: p.sniff_timestamp - _pkt.sniff_timestamp <= 3).\
                must_next()
        # check router cost before and after the re-attach
        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p: {1,0,1} == set(p.mle.tlv.route64.cost)).\
            must_next()
        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p: {1,2,1} == set(p.mle.tlv.route64.cost) and\
                   {leader_rid, router_1_rid, router_2_rid} ==
                   p.mle.tlv.route64.id_mask
                   ).\
            must_next()

        # Step 6: The DUT’s routing cost to Router_1 MUST go directly to
        #         infinity as there is no multi-hop cost for Router_1
        #         The DUT MUST remove Router_1 & Router_2 IDs from its ID set
        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p: {0, 0, 1} == set(p.mle.tlv.route64.cost)).\
            must_next()
        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p: {0, 1} == set(p.mle.tlv.route64.cost)).\
            must_next()
        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p:
                   [1] == p.mle.tlv.route64.cost and\
                   {leader_rid} ==
                   p.mle.tlv.route64.id_mask
                   ).\
            must_next()


if __name__ == '__main__':
    unittest.main()
