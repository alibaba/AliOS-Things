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

import command
import config
import mle
import thread_cert
from command import CheckType
from pktverify.consts import MLE_PARENT_REQUEST, MLE_CHILD_ID_REQUEST, ADDR_SOL_URI, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, ADDRESS16_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV, NL_MAC_EXTENDED_ADDRESS_TLV, NL_RLOC16_TLV, NL_STATUS_TLV, NL_ROUTER_MASK_TLV, COAP_CODE_POST, COAP_CODE_ACK
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
ROUTER1 = 2

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify that when the Leader
# de-allocates a router ID, the DUT, as a router, re-attaches.
#
# Test Topology:
# -------------
# Leader
#    |
# Router
#
# DUT Types:
# ----------
# Router


class Cert_5_1_06_RemoveRouterId(thread_cert.TestCase):
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

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')
        rloc16 = self.nodes[ROUTER1].get_addr16()
        self.collect_rloc16s()

        router_lladdr = self.nodes[ROUTER1].get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)
        self.assertTrue(self.nodes[LEADER].ping(router_lladdr))

        self.nodes[LEADER].release_router_id(rloc16 >> 10)
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.assertTrue(self.nodes[LEADER].ping(router_lladdr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC16 = pv.vars['LEADER_RLOC16']
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

        # Step 3: Router send a properly formatted
        #         MLE Parent Request,
        #         MLE Child ID Request,
        #         and Address Solicit Request
        #         messages to the Leader.

        pkts.filter_wpan_src64(ROUTER).\
            filter_LLARMA().\
            filter_mle_cmd(MLE_PARENT_REQUEST).\
            filter(lambda p: {
                              CHALLENGE_TLV,
                              MODE_TLV,
                              SCAN_MASK_TLV,
                              VERSION_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.ipv6.hlim == 255 and\
                   p.mle.tlv.scan_mask.r == 1 and\
                   p.mle.tlv.scan_mask.e == 0).\
            must_next()

        _pkt = pkts.filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(LEADER).\
            filter_mle_cmd(MLE_CHILD_ID_REQUEST).\
            filter(lambda p: {
                              LINK_LAYER_FRAME_COUNTER_TLV,
                              MODE_TLV,
                              RESPONSE_TLV,
                              TIMEOUT_TLV,
                              TLV_REQUEST_TLV,
                              ADDRESS16_TLV,
                              NETWORK_DATA_TLV,
                              VERSION_TLV
                    } <= set(p.mle.tlv.type) and\
                   p.mle.tlv.addr16 is nullField and\
                   p.thread_nwd.tlv.type is nullField).\
                   must_next()
        _pkt.must_not_verify(lambda p: (ADDRESS_REGISTRATION_TLV) in p.mle.tlv.type)

        pkts.filter_wpan_src64(ROUTER).\
            filter_wpan_dst16(LEADER_RLOC16).\
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

        pkts.filter_wpan_src64(LEADER).\
            filter_coap_ack(ADDR_SOL_URI, port=MM).\
            filter(lambda p: {
                              NL_STATUS_TLV,
                              NL_RLOC16_TLV,
                              NL_ROUTER_MASK_TLV
                              } == set(p.coap.tlv.type) and\
                   p.coap.code == COAP_CODE_ACK and\
                   p.thread_address.tlv.rloc16 !=
                   _pkt_as.thread_address.tlv.rloc16 and\
                   p.thread_address.tlv.status == 0
                   ).\
            must_next()

        # Step 4: DUT responds with ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(LEADER).\
            must_next()


if __name__ == '__main__':
    unittest.main()
