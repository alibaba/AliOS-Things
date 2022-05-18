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
import thread_cert
from pktverify.consts import MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, ADDR_QRY_URI, ADDR_NTF_URI, NL_ML_EID_TLV, NL_RLOC16_TLV, NL_TARGET_EID_TLV, COAP_CODE_POST
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ROUTER1 = 2
DUT_ROUTER2 = 3
ROUTER3 = 4
MED1 = 5
MED1_TIMEOUT = 3

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate that the DUT is able to generate
# Address Query messages and properly respond with Address Notification messages.
#
# Test Topology:
# -------------
# Router_1 - Leader
#            /    \
#     Router_3 - Router_2(DUT)
#                   |
#                  MED
#
# DUT Types:
# ----------
#  Router


class Cert_5_3_3_AddressQuery(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1, DUT_ROUTER2, ROUTER3]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        DUT_ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER, ROUTER3, MED1]
        },
        ROUTER3: {
            'name': 'ROUTER_3',
            'mode': 'rdn',
            'allowlist': [LEADER, DUT_ROUTER2]
        },
        MED1: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'timeout': 3,
            'allowlist': [DUT_ROUTER2]
        },
    }

    def test(self):
        # 1
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER1].start()
        self.nodes[DUT_ROUTER2].start()
        self.nodes[ROUTER3].start()
        self.nodes[MED1].start()
        self.simulator.go(10)

        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')
        self.assertEqual(self.nodes[DUT_ROUTER2].get_state(), 'router')
        self.assertEqual(self.nodes[ROUTER3].get_state(), 'router')
        self.assertEqual(self.nodes[MED1].get_state(), 'child')
        self.collect_ipaddrs()
        self.collect_rlocs()
        self.collect_rloc16s()

        # 2
        router3_mleid = self.nodes[ROUTER3].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        self.assertTrue(self.nodes[MED1].ping(router3_mleid))

        # 3
        # Wait the finish of address resolution traffic triggerred by previous
        # ping.
        self.simulator.go(5)

        med1_mleid = self.nodes[MED1].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        self.assertTrue(self.nodes[ROUTER1].ping(med1_mleid))

        # 4
        # Wait the finish of address resolution traffic triggerred by previous
        # ping.
        self.simulator.go(5)

        self.assertTrue(self.nodes[MED1].ping(router3_mleid))

        # 5
        # Power off ROUTER3 and wait for leader to expire its Router ID.
        # In this topology, ROUTER3 has two neighbors (Leader and DUT_ROUTER2),
        # so the wait time is (MAX_NEIGHBOR_AGE (100s) + worst propagation time (32s * 15) for bad routing +\
        # INFINITE_COST_TIMEOUT (90s) + transmission time + extra redundancy),
        # totally ~700s.
        self.nodes[ROUTER3].stop()
        self.simulator.go(700)

        self.assertFalse(self.nodes[MED1].ping(router3_mleid))

        # 6
        self.nodes[MED1].stop()
        self.simulator.go(MED1_TIMEOUT)

        self.assertFalse(self.nodes[ROUTER1].ping(med1_mleid))
        self.assertFalse(self.nodes[ROUTER1].ping(med1_mleid))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_MLEID = pv.vars['LEADER_MLEID']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_1_RLOC = pv.vars['ROUTER_1_RLOC']
        ROUTER_1_MLEID = pv.vars['ROUTER_1_MLEID']
        ROUTER_2 = pv.vars['ROUTER_2']
        ROUTER_2_RLOC16 = pv.vars['ROUTER_2_RLOC16']
        ROUTER_2_RLOC = pv.vars['ROUTER_2_RLOC']
        ROUTER_2_MLEID = pv.vars['ROUTER_2_MLEID']
        MED = pv.vars['MED']
        MED_RLOC16 = pv.vars['MED_RLOC16']
        MED_MLEID = pv.vars['MED_MLEID']
        ROUTER_3 = pv.vars['ROUTER_3']
        ROUTER_3_MLEID = pv.vars['ROUTER_3_MLEID']
        MM = pv.vars['MM_PORT']

        # Step 1: Build the topology as described
        for i in range(1, 4):
            with pkts.save_index():
                pv.verify_attached('ROUTER_%d' % i)
        pkts.filter_wpan_src64(MED).\
            filter_wpan_dst64(ROUTER_2).\
            filter_mle_cmd(MLE_CHILD_ID_REQUEST).\
            must_next()
        pkts.filter_wpan_src64(ROUTER_2).\
            filter_wpan_dst64(MED).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            must_next()

        # Step 2: MED sends an ICMPv6 Echo Request to Router_3 ML-EID
        #         The DUT MUST generate an Address Query Request on MED’s behalf
        #         to find Router_3 address.
        #         The Address Query Request MUST be sent to the Realm-Local
        #         All-Routers address (FF03::2)
        #             CoAP URI-Path
        #                 - NON POST coap://<FF03::2>
        #             CoAP Payload
        #                 - Target EID TLV
        #         The DUT MUST receive and process the incoming Address Notification
        #         The DUT MUST then forward the ICMPv6 Echo Request from MED and
        #         forward the ICMPv6 Echo Reply to MED

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(MED).\
            filter_ipv6_dst(ROUTER_3_MLEID).\
            must_next()
        pkts.filter_wpan_src64(ROUTER_2).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            filter(lambda p: p.thread_address.tlv.target_eid == ROUTER_3_MLEID).\
            must_next()
        _pkt1 = pkts.filter_wpan_src64(ROUTER_3).\
            filter_ipv6_dst(ROUTER_2_RLOC).\
            filter_coap_request(ADDR_NTF_URI, port=MM).\
            filter(lambda p: {
                              NL_ML_EID_TLV,
                              NL_RLOC16_TLV,
                              NL_TARGET_EID_TLV
                              } == set(p.coap.tlv.type) and\
                   p.thread_address.tlv.target_eid == ROUTER_3_MLEID and\
                   p.coap.code == COAP_CODE_POST
                   ).\
           must_next()
        pkts.filter_ping_request(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_2).\
            filter_ipv6_dst(ROUTER_3_MLEID).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_3).\
            filter_dst16(ROUTER_2_RLOC16).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_2).\
            filter_wpan_dst16(MED_RLOC16).\
            must_next()

        # Step 3: Router_1 sends an ICMPv6 Echo Request to the MED ML-EID address
        #         The DUT MUST respond to the Address Query Request with a properly
        #         formatted Address Notification Message:
        #             CoAP URI-Path
        #                 - CON POST coap://[<Address Query Source>]:MM/a/an
        #             CoAP Payload
        #                 - ML-EID TLV
        #                 - RLOC16 TLV
        #                 - Target EID TLV
        #         The IPv6 Source address MUST be the RLOC of the originator
        #         The IPv6 Destination address MUST be the RLOC of the destination

        pkts.filter_wpan_src64(ROUTER_1).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            filter(lambda p: p.thread_address.tlv.target_eid == MED_MLEID).\
            must_next()
        pkts.filter_ipv6_src_dst(ROUTER_2_RLOC, ROUTER_1_RLOC).\
            filter_coap_request(ADDR_NTF_URI, port=MM).\
            filter(lambda p: {
                              NL_ML_EID_TLV,
                              NL_RLOC16_TLV,
                              NL_TARGET_EID_TLV
                              } <= set(p.coap.tlv.type) and\
                   p.thread_address.tlv.target_eid == MED_MLEID and\
                   p.coap.code == COAP_CODE_POST
                   ).\
            must_next()
        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(ROUTER_1).\
            filter_ipv6_dst(MED_MLEID).\
            must_next()
        pkts.filter_ping_request(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_2).\
            filter_ipv6_dst(MED_MLEID).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(MED).\
            filter_wpan_dst16(ROUTER_2_RLOC16).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_2).\
            filter_ipv6_dst(ROUTER_1_MLEID).\
            must_next()

        # Step 4: MED sends an ICMPv6 Echo Request to the Router_3 ML-EID
        #         The DUT MUST NOT send an Address Query, as the Router_3 address
        #         should be cached
        #         The DUT MUST forward the ICMPv6 Echo Reply to MED

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(MED).\
            filter_ipv6_dst(ROUTER_3_MLEID).\
            must_next()
        lstart = pkts.index
        pkts.filter_ping_request(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_2).\
            filter_ipv6_dst(ROUTER_3_MLEID).\
            must_next()
        lend = pkts.index

        pkts.range(lstart, lend).\
            filter_wpan_src64(ROUTER_2).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI).\
            must_not_next()

        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_3).\
            filter_wpan_dst16(ROUTER_2_RLOC16).\
            must_next()
        _pkt1 = pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_2).\
            filter_wpan_dst16(MED_RLOC16).\
            must_next()

        # Step 5: Power off Router_3 and wait for the Leader to expire its Router ID
        #         Send an ICMPv6 Echo Request from MED to the Router_3 ML-EID address
        #         The DUT MUST update its address cache and removes all entries based
        #         on Router_3’s Router ID.
        #         The DUT MUST send an Address Query to discover Router_3’s RLOC
        #         address.

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(MED).\
            filter_ipv6_dst(ROUTER_3_MLEID).\
            filter(lambda p: p.sniff_timestamp - _pkt1.sniff_timestamp >= 700).\
            must_next()
        pkts.filter_wpan_src64(ROUTER_2).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            filter(lambda p: p.thread_address.tlv.target_eid == ROUTER_3_MLEID and\
                   p.coap.code == COAP_CODE_POST
                   ).\
            must_next()

        # Step 6: Power off MED and wait for the DUT to timeout the child.
        #         Send two ICMPv6 Echo Requests from Router_1 to MED ML-EID
        #         The DUT MUST NOT respond with an Address Notification message

        pkts.filter_ping_request().\
            filter_wpan_src64(ROUTER_1).\
            filter_ipv6_dst(MED_MLEID).\
            must_next()

        pkts.filter_wpan_src64(ROUTER_1).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            filter(lambda p: p.thread_address.tlv.target_eid == MED_MLEID and\
                   p.coap.code == COAP_CODE_POST
                   ).\
            must_next()

        pkts.filter_ipv6_src_dst(ROUTER_2_RLOC, ROUTER_1_RLOC).\
            filter_coap_request(ADDR_NTF_URI).\
            must_not_next()


if __name__ == '__main__':
    unittest.main()
