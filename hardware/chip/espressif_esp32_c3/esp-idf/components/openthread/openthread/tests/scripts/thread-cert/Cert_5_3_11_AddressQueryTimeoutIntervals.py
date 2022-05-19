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

import unittest

import command
import config
import thread_cert
from pktverify.consts import ADDR_QRY_URI, ADDR_NTF_URI, NL_ML_EID_TLV, NL_RLOC16_TLV, NL_TARGET_EID_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.bytes import Bytes
from pktverify.addrs import Ipv6Addr

LEADER = 1
DUT_ROUTER1 = 2
MED1 = 3
X = 'fd00:db8:0000:0000:aa55:aa55:aa55:aa55'

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate the way AQ_TIMEOUT and
# AQ_RETRY_TIMEOUT intervals are used in the Address Query transmission
# algorithm.
#
# Test Topology:
# -------------
#  Leader
#    |
#  Router(DUT)
#    |
#   MED
#
# DUT Types:
# ----------
#  Router


class Cert_5_3_11_AddressQueryTimeoutIntervals(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [DUT_ROUTER1]
        },
        DUT_ROUTER1: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER, MED1]
        },
        MED1: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [DUT_ROUTER1]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[DUT_ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT_ROUTER1].get_state(), 'router')

        self.nodes[MED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MED1].get_state(), 'child')

        self.collect_rlocs()

        # 2 MED1: MED1 sends an ICMPv6 Echo Request to a non-existent
        # mesh-local address X
        self.assertFalse(self.nodes[MED1].ping(X))

        self.simulator.go(config.AQ_TIMEOUT)

        # 3 MED1: MED1 sends an ICMPv6 Echo Request to a non-existent mesh-local address X before
        #         ADDRESS_QUERY_INITIAL_RETRY_DELAY timeout expires
        self.assertFalse(self.nodes[MED1].ping(X))

        self.simulator.go(config.ADDRESS_QUERY_INITIAL_RETRY_DELAY)

        # 4 MED1: MED1 sends an ICMPv6 Echo Request to a non-existent mesh-local address X after
        #         ADDRESS_QUERY_INITIAL_RETRY_DELAY timeout expired
        self.assertFalse(self.nodes[MED1].ping(X))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']
        ROUTER_RLOC = pv.vars['ROUTER_RLOC']
        MED = pv.vars['MED']
        MM = pv.vars['MM_PORT']

        # Step 1: Build the topology as described

        pv.verify_attached('ROUTER', 'LEADER')
        pv.verify_attached('MED', 'ROUTER', 'MTD')

        # Step 2: MED sends an ICMPv6 Echo Request to a nonexistent mesh-local
        #         address X
        #         The DUT MUST generate an Address Query Request on MED’s behalf
        #         The Address Query Request MUST be sent to the Realm-Local
        #         All-Routers address (FF03::2)
        #             CoAP URI-Path
        #                 - NON POST coap://<FF03::2>
        #             CoAP Payload
        #                 - Target EID TLV – non-existent mesh-local
        #         An Address Query Notification MUST NOT be received within
        #         AQ_TIMEOUT interval.

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(MED).\
            filter_ipv6_dst(Ipv6Addr(X)).\
            must_next()
        step2_start = pkts.index

        pkts.filter_wpan_src64(ROUTER).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            filter(lambda p: p.thread_address.tlv.target_eid == Ipv6Addr(X)).\
            must_next()

        # Step 3: MED sends an ICMPv6 Echo Request to a nonexistent mesh-local
        #         address X before ADDRESS_QUERY_INITIAL_RETRY_DELAY timeout
        #         expired
        #         The DUT MUST NOT initiate a new Address Query frame

        pkts.filter_ping_request().\
            filter_wpan_src64(MED).\
            filter_ipv6_dst(Ipv6Addr(X)).\
            must_next()
        step2_end = pkts.index

        pkts.range(step2_start, step2_end).filter_ipv6_dst(ROUTER_RLOC).\
            filter_coap_request(ADDR_NTF_URI, port=MM).\
            must_not_next()

        # Step 4: MED sends an ICMPv6 Echo Request to a nonexistent mesh-local
        #         address X after ADDRESS_QUERY_INITIAL_RETRY_DELAY timeout
        #         expired
        #         The DUT MUST generate an Address Query Request on MED’s behalf
        #         The Address Query Request MUST be sent to the Realm-Local
        #         All-Routers address (FF03::2)
        #             CoAP URI-Path
        #                 - NON POST coap://<FF03::2>
        #             CoAP Payload
        #                 - Target EID TLV – non-existent mesh-local

        pkts.filter_ping_request().\
            filter_wpan_src64(MED).\
            filter_ipv6_dst(Ipv6Addr(X)).\
            must_next()
        step3_end = pkts.index
        pkts.range(step2_end, step3_end).filter_wpan_src64(ROUTER).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            must_not_next()
        pkts.filter_wpan_src64(ROUTER).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            filter(lambda p: p.thread_address.tlv.target_eid == Ipv6Addr(X)).\
            must_next()


if __name__ == '__main__':
    unittest.main()
