#!/usr/bin/env python3
#
#  Copyright (c) 2018, The OpenThread Authors.
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

import copy
import unittest

import command
import config
import mle
import thread_cert
from pktverify.consts import MLE_PARENT_REQUEST, MLE_LINK_REQUEST, MLE_LINK_ACCEPT, MLE_LINK_ACCEPT_AND_REQUEST, SOURCE_ADDRESS_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, TLV_REQUEST_TLV, VERSION_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

DUT_LEADER = 1
DUT_ROUTER1 = 2

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to show that when the Leader is rebooted for a time period shorter than the leader timeout, it does not trigger network partitioning and remains the leader when it reattaches to the network.
#
# Test Topology:
# -------------
#   Leader
#     |
#   Router
#
# DUT Types:
# ----------
#  Leader
#  Router


class Cert_5_5_1_LeaderReboot(thread_cert.TestCase):
    #USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        DUT_LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [DUT_ROUTER1]
        },
        DUT_ROUTER1: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [DUT_LEADER]
        },
    }

    def _setUpLeader(self):
        self.nodes[DUT_LEADER].add_allowlist(self.nodes[DUT_ROUTER1].get_addr64())
        self.nodes[DUT_LEADER].enable_allowlist()

    def test(self):
        self.nodes[DUT_LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT_LEADER].get_state(), 'leader')

        self.nodes[DUT_ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT_ROUTER1].get_state(), 'router')

        leader_rloc = self.nodes[DUT_LEADER].get_ip6_address(config.ADDRESS_TYPE.RLOC)

        leader_rloc16 = self.nodes[DUT_LEADER].get_addr16()
        self.nodes[DUT_LEADER].reset()
        self.assertFalse(self.nodes[DUT_ROUTER1].ping(leader_rloc))
        self._setUpLeader()

        self.simulator.go(config.MAX_ADVERTISEMENT_INTERVAL)

        self.nodes[DUT_LEADER].start()

        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT_LEADER].get_state(), 'leader')
        self.assertEqual(self.nodes[DUT_LEADER].get_addr16(), leader_rloc16)

        router1_link_local_address = self.nodes[DUT_ROUTER1].get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)
        self.assertTrue(self.nodes[DUT_LEADER].ping(router1_link_local_address))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']

        # Step 1: Verify topology is formed correctly.
        pv.verify_attached('ROUTER', 'LEADER')

        # Step 2: The DUT MUST send properly formatted MLE Advertisements with
        #         an IP Hop Limit of 255 to the Link-Local All Nodes multicast
        #         address (FF02::1).
        #          The following TLVs MUST be present in the MLE Advertisements:
        #              - Leader Data TLV
        #              - Route64 TLV
        #              - Source Address TLV
        with pkts.save_index():
            pkts.filter_wpan_src64(LEADER).\
                filter_mle_advertisement('Leader').\
                must_next()
        pkts.filter_wpan_src64(ROUTER).\
            filter_mle_advertisement('Router').\
            must_next()

        pkts.filter_ping_request().\
            filter_wpan_src64(ROUTER).\
            must_next()
        lstart = pkts.index

        # Step 4: The Leader MUST send a multicast Link Request
        #         The following TLVs MUST be present in the Link Request:
        #             - Challenge TLV
        #             - Version TLV
        #             - TLV Request TLV: Address16 TLV, Route64 TLV
        pkts.filter_wpan_src64(LEADER).\
            filter_LLARMA().\
            filter_mle_cmd(MLE_LINK_REQUEST).\
            filter(lambda p: {
                              CHALLENGE_TLV,
                              VERSION_TLV,
                              TLV_REQUEST_TLV,
                              ADDRESS16_TLV,
                              ROUTE64_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.mle.tlv.addr16 is nullField and\
                   p.mle.tlv.route64.id_mask is nullField
                   ).\
            must_next()
        lend = pkts.index

        # Step 3: Reset Leader. The Leader MUST stop sending MLE advertisements.
        #         The Leader reboot time MUST be less than Leader Timeout value
        pkts.range(lstart, lend).\
            filter_wpan_src64(LEADER).\
            filter_mle_advertisement('Leader').\
            must_not_next()

        # Step 5: Router MUST reply with a Link Accept
        #         The following TLVs MUST be present in the Link Accept:
        #             - Leader Data TLV
        #             - Link-layer Frame Counter TLV
        #             - Response TLV
        #             - Source Address TLV
        #             - Address16 TLV
        #             - Route64 TLV
        #             - Version TLV
        #             - Challenge TLV (situational)
        #             - MLE Frame Counter TLV (optional)
        #         The Challenge TLV MUST be included
        #         if the response is an Accept and Request message.
        _pkt = pkts.filter_wpan_src64(ROUTER).\
                filter_wpan_dst64(LEADER).\
                filter_mle_cmd2(MLE_LINK_ACCEPT, MLE_LINK_ACCEPT_AND_REQUEST).\
                filter(lambda p: {
                                  LEADER_DATA_TLV,
                                  LINK_LAYER_FRAME_COUNTER_TLV,
                                  RESPONSE_TLV,
                                  SOURCE_ADDRESS_TLV,
                                  ADDRESS16_TLV,
                                  ROUTE64_TLV,
                                  VERSION_TLV
                                   } <= set(p.mle.tlv.type) and\
                       p.mle.tlv.addr16 is not nullField and\
                       p.mle.tlv.route64.id_mask is not nullField
                       ).\
                       must_next()
        if _pkt.mle.cmd == MLE_LINK_ACCEPT_AND_REQUEST:
            _pkt.must_verify(lambda p: {CHALLENGE_TLV} <= set(p.mle.tlv.type))

        # Step 7: Router_1 MUST respond with an ICMPv6 Echo Reply
        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
            must_next()
        lend2 = pkts.index
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(LEADER).\
            must_next()

        # Step 6: The Leader MUST NOT send a Parent Request after it is re-enabled.
        pkts.range(lend, lend2).\
            filter_wpan_src64(LEADER).\
            filter_mle_cmd(MLE_PARENT_REQUEST).\
            must_not_next()


if __name__ == '__main__':
    unittest.main()
