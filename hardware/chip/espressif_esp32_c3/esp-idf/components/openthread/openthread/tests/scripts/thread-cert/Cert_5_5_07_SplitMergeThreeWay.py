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

import thread_cert
from pktverify.consts import MLE_ADVERTISEMENT, MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ROUTE64_TLV, MODE_TLV, CHALLENGE_TLV, SCAN_MASK_TLV, VERSION_TLV, LINK_LAYER_FRAME_COUNTER_TLV, RESPONSE_TLV, LINK_MARGIN_TLV, CONNECTIVITY_TLV, TIMEOUT_TLV, TLV_REQUEST_TLV, ADDRESS16_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV
from pktverify.packet_verifier import PacketVerifier

LEADER1 = 1
ROUTER1 = 2
ROUTER2 = 3
ROUTER3 = 4


class Cert_5_5_7_SplitMergeThreeWay(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER1: {
            'name': 'LEADER_1',
            'mode': 'rdn',
            'allowlist': [ROUTER1, ROUTER2, ROUTER3]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER1]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER1]
        },
        ROUTER3: {
            'name': 'ROUTER_3',
            'mode': 'rdn',
            'allowlist': [LEADER1]
        },
    }

    def _setUpLeader1(self):
        self.nodes[LEADER1].add_allowlist(self.nodes[ROUTER1].get_addr64())
        self.nodes[LEADER1].add_allowlist(self.nodes[ROUTER2].get_addr64())
        self.nodes[LEADER1].add_allowlist(self.nodes[ROUTER3].get_addr64())
        self.nodes[LEADER1].enable_allowlist()
        self.nodes[LEADER1].set_router_selection_jitter(1)

    def test(self):
        self.nodes[LEADER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER1].get_state(), 'leader')

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'router')

        self.nodes[ROUTER3].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER3].get_state(), 'router')

        self.nodes[LEADER1].reset()
        self._setUpLeader1()
        self.simulator.go(140)

        self.nodes[LEADER1].start()
        self.simulator.go(30)

        addrs = self.nodes[LEADER1].get_addrs()
        for addr in addrs:
            if addr[0:4] != 'fe80':
                self.assertTrue(self.nodes[ROUTER1].ping(addr))

        addrs = self.nodes[ROUTER2].get_addrs()
        for addr in addrs:
            if addr[0:4] != 'fe80':
                self.assertTrue(self.nodes[ROUTER1].ping(addr))

        addrs = self.nodes[ROUTER3].get_addrs()
        for addr in addrs:
            if addr[0:4] != 'fe80':
                self.assertTrue(self.nodes[ROUTER1].ping(addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER_1']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_2 = pv.vars['ROUTER_2']
        ROUTER_3 = pv.vars['ROUTER_3']
        leader_pkts = pkts.filter_wpan_src64(LEADER)
        router1_pkts = pkts.filter_wpan_src64(ROUTER_1)
        router2_pkts = pkts.filter_wpan_src64(ROUTER_2)
        router3_pkts = pkts.filter_wpan_src64(ROUTER_3)

        # Step 2: The Leader and Router_1 MUST send properly formatted MLE Advertisements
        leader_pkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next()
        _lpkts = leader_pkts.copy()
        _lpkts.filter_mle_cmd(MLE_ADVERTISEMENT).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ROUTE64_TLV} == set(p.mle.tlv.type))

        router1_pkts.range(leader_pkts.index).filter_mle_cmd(MLE_ADVERTISEMENT).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ROUTE64_TLV} == set(p.mle.tlv.type))

        # Step 4: Each router forms a partition with the lowest possible partition ID
        # Step 5: Router_1 MUST send MLE Parent Requests and MUST make two separate attempts
        router1_pkts.filter_mle_cmd(MLE_PARENT_REQUEST).must_next().must_verify(
            lambda p: {MODE_TLV, CHALLENGE_TLV, SCAN_MASK_TLV, VERSION_TLV} == set(
                p.mle.tlv.type) and p.mle.tlv.scan_mask.r == 1 and p.mle.tlv.scan_mask.e == 1)
        lreset_start = router1_pkts.index
        router1_pkts.filter_mle_cmd(MLE_PARENT_REQUEST).must_next().must_verify(
            lambda p: {MODE_TLV, CHALLENGE_TLV, SCAN_MASK_TLV, VERSION_TLV} == set(
                p.mle.tlv.type) and p.mle.tlv.scan_mask.r == 1 and p.mle.tlv.scan_mask.e == 1)

        # Step 7: Router_1 MUST attempt to attach to any other Partition
        # within range by sending a MLE Parent Request.
        router1_pkts.filter_mle_cmd(MLE_PARENT_REQUEST).must_next().must_verify(
            lambda p: {MODE_TLV, CHALLENGE_TLV, SCAN_MASK_TLV, VERSION_TLV} == set(p.mle.tlv.type))
        lreset_stop = router1_pkts.index

        # Step 3: The Leader MUST stop sending MLE advertisements.
        leader_pkts.range(lreset_start, lreset_stop).filter_mle_cmd(MLE_ADVERTISEMENT).must_not_next()

        # Step 6: The Leader does not respond to the Parent Requests
        leader_pkts.range(lreset_start, lreset_stop).filter_mle_cmd(MLE_PARENT_RESPONSE).must_not_next()

        # Step 8: Router_1 take over leader role of a new Partition and begin transmitting
        # MLE Advertisements
        router1_pkts.copy().filter_mle_cmd(MLE_ADVERTISEMENT).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ROUTE64_TLV} == set(p.mle.tlv.type))

        # Step 9: The Leader MUST send properly formatted MLE Parent Requests to the
        # All-Routers multicast address
        _lpkts.filter_mle_cmd(MLE_PARENT_REQUEST).must_next().must_verify(
            lambda p: {MODE_TLV, CHALLENGE_TLV, SCAN_MASK_TLV, VERSION_TLV} == set(p.mle.tlv.type))

        # Step 10: Router_1 MUST send an MLE Parent Response
        router1_pkts.filter_mle_cmd(MLE_PARENT_RESPONSE).must_next().must_verify(
            lambda p: {
                SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, LINK_LAYER_FRAME_COUNTER_TLV, RESPONSE_TLV, CHALLENGE_TLV,
                LINK_MARGIN_TLV, CONNECTIVITY_TLV, VERSION_TLV
            } <= set(p.mle.tlv.type))

        # Step 11: Leader send MLE Child ID Request
        _lpkts.filter_mle_cmd(MLE_CHILD_ID_REQUEST).must_next().must_verify(
            lambda p: {
                RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MODE_TLV, TIMEOUT_TLV, VERSION_TLV, TLV_REQUEST_TLV,
                ADDRESS16_TLV, NETWORK_DATA_TLV, ROUTE64_TLV, ACTIVE_TIMESTAMP_TLV
            } <= set(p.mle.tlv.type))

        # Step 12: DUT (Router or Leader) MUST respond with a ICMPv6 Echo Reply
        _lpkts.filter_ping_reply().must_next()
        router2_pkts.filter_ping_reply().must_next()
        router3_pkts.filter_ping_reply().must_next()


if __name__ == '__main__':
    unittest.main()
