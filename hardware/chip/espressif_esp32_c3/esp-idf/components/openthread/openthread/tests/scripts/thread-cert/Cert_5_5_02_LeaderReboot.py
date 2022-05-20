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
from pktverify.consts import MLE_ADVERTISEMENT, MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_UPDATE_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, MLE_LINK_REQUEST, MLE_LINK_ACCEPT_AND_REQUEST, ADDR_SOL_URI, SOURCE_ADDRESS_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MLE_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV, ACTIVE_TIMESTAMP_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
ROUTER = 2
ED = 3


class Cert_5_5_2_LeaderReboot(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER, ED]
        },
        ED: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [ROUTER]
        },
    }

    def _setUpLeader(self):
        self.nodes[LEADER].add_allowlist(self.nodes[ROUTER].get_addr64())
        self.nodes[LEADER].enable_allowlist()
        self.nodes[LEADER].set_router_selection_jitter(1)

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[ED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED].get_state(), 'child')

        self.nodes[LEADER].reset()
        self._setUpLeader()
        self.simulator.go(140)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'leader')

        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'router')

        addrs = self.nodes[ED].get_addrs()
        for addr in addrs:
            self.assertTrue(self.nodes[ROUTER].ping(addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']
        MED = pv.vars['MED']
        leader_pkts = pkts.filter_wpan_src64(LEADER)
        _rpkts = pkts.filter_wpan_src64(ROUTER)

        # Step 2: The DUT MUST send properly formatted MLE Advertisements
        _rpkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next()
        _lpkts = leader_pkts.range(_rpkts.index)
        _lpkts.filter_mle_cmd(MLE_ADVERTISEMENT).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ROUTE64_TLV} == set(p.mle.tlv.type))

        _rpkts.filter_mle_cmd(MLE_ADVERTISEMENT).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ROUTE64_TLV} == set(p.mle.tlv.type))

        # Step 4: Router_1 MUST attempt to reattach to its original partition by
        # sending MLE Parent Requests to the All-Routers multicast
        # address (FFxx::xx) with a hop limit of 255. MUST make two separate attempts
        for i in range(1, 3):
            _rpkts.filter_mle_cmd(MLE_PARENT_REQUEST).must_next().must_verify(
                lambda p: {MODE_TLV, CHALLENGE_TLV, SCAN_MASK_TLV, VERSION_TLV} == set(
                    p.mle.tlv.type) and p.mle.tlv.scan_mask.r == 1 and p.mle.tlv.scan_mask.e == 1)
        lreset_start = _rpkts.index

        # Step 6:Router_1 MUST attempt to attach to any other Partition
        # within range by sending a MLE Parent Request.
        _rpkts.filter_mle_cmd(MLE_PARENT_REQUEST).must_next().must_verify(
            lambda p: {MODE_TLV, CHALLENGE_TLV, SCAN_MASK_TLV, VERSION_TLV} == set(p.mle.tlv.type))
        lreset_stop = _rpkts.index

        # Step 3: The Leader MUST stop sending MLE advertisements.
        leader_pkts.range(lreset_start, lreset_stop).filter_mle_cmd(MLE_ADVERTISEMENT).must_not_next()

        # Step 5: Leader MUST NOT respond to the MLE Parent Requests
        leader_pkts.range(lreset_start, lreset_stop).filter_mle_cmd(MLE_PARENT_RESPONSE).must_not_next()

        # Step 7: Take over leader role of a new Partition and
        # begin transmitting MLE Advertisements
        with _rpkts.save_index():
            _rpkts.filter_mle_cmd(MLE_ADVERTISEMENT).must_next().must_verify(
                lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ROUTE64_TLV} == set(p.mle.tlv.type))

        # Step 8: Router_1 MUST respond with an MLE Child Update Response,
        # with the updated TLVs of the new partition
        _rpkts.filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, MODE_TLV, LEADER_DATA_TLV, ADDRESS_REGISTRATION_TLV} < set(p.mle.tlv.type))

        # Step 9: The Leader MUST send properly formatted MLE Parent
        # Requests to the All-Routers multicast address
        _lpkts.range(lreset_stop).filter_mle_cmd(MLE_PARENT_REQUEST).must_next().must_verify(
            lambda p: {MODE_TLV, CHALLENGE_TLV, SCAN_MASK_TLV, VERSION_TLV} == set(p.mle.tlv.type))

        # Step 10: Router_1 MUST send an MLE Parent Response
        _rpkts.filter_mle_cmd(MLE_PARENT_RESPONSE).must_next().must_verify(
            lambda p: {
                SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, LINK_LAYER_FRAME_COUNTER_TLV, RESPONSE_TLV, CHALLENGE_TLV,
                LINK_MARGIN_TLV, CONNECTIVITY_TLV, VERSION_TLV
            } < set(p.mle.tlv.type))

        # Step 11: Leader send MLE Child ID Request
        _lpkts.filter_mle_cmd(MLE_CHILD_ID_REQUEST).must_next().must_verify(
            lambda p: {
                RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MODE_TLV, TIMEOUT_TLV, VERSION_TLV, TLV_REQUEST_TLV,
                ADDRESS16_TLV, NETWORK_DATA_TLV, ROUTE64_TLV, ACTIVE_TIMESTAMP_TLV
            } < set(p.mle.tlv.type))

        #Step 12: Router_1 send MLE Child ID Response
        _rpkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ADDRESS16_TLV, NETWORK_DATA_TLV, ROUTE64_TLV} < set(
                p.mle.tlv.type))

        #Step 13: Leader send an Address Solicit Request
        _lpkts.filter_coap_request(ADDR_SOL_URI).must_next().must_verify(
            lambda p: p.thread_address.tlv.ext_mac_addr and p.thread_address.tlv.status != 0)

        #Step 14: Router_1 send an Address Solicit Response
        _rpkts.filter_coap_ack(ADDR_SOL_URI).must_next().must_verify(
            lambda p: p.thread_address.tlv.router_mask_assigned and p.thread_address.tlv.rloc16 is not nullField and p.
            thread_address.tlv.status == 0)

        #Step 15: Leader Send a Multicast Link Request
        _lpkts.filter_mle_cmd(MLE_LINK_REQUEST).must_next().must_verify(
            lambda p: {VERSION_TLV, TLV_REQUEST_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, CHALLENGE_TLV} < set(
                p.mle.tlv.type))

        #Step 16: Router_1 send a Unicast Link Accept
        _rpkts.filter_mle_cmd(MLE_LINK_ACCEPT_AND_REQUEST).must_next().must_verify(lambda p: {
            VERSION_TLV, SOURCE_ADDRESS_TLV, RESPONSE_TLV, MLE_FRAME_COUNTER_TLV, LINK_MARGIN_TLV, LEADER_DATA_TLV
        } < set(p.mle.tlv.type))

        #Step 17: Router_1 MUST respond with an ICMPv6 Echo Reply
        _rpkts.filter_ping_request().filter_wpan_dst64(MED).must_next()


if __name__ == '__main__':
    unittest.main()
