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
from pktverify.consts import MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, REALM_LOCAL_ALL_NODES_ADDRESS, REALM_LOCAL_ALL_ROUTERS_ADDRESS, REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ROUTER1 = 2
DUT_ROUTER2 = 3
SED1 = 4
FRAGMENTED_DATA_LEN = 256

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate the Realm-Local addresses
# that the DUT auto-configures.
#
# Test Topology:
# -------------
# Leader
#    |
# Router_1 - Router_2(DUT)
#              |
#             SED
#
# DUT Types:
# ----------
#  Router


class Cert_5_3_2_RealmLocal(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER, DUT_ROUTER2]
        },
        DUT_ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [ROUTER1, SED1]
        },
        SED1: {
            'name': 'SED',
            'is_mtd': True,
            'mode': 'n',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [DUT_ROUTER2]
        },
    }

    def test(self):
        # 1
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[DUT_ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT_ROUTER2].get_state(), 'router')

        self.nodes[SED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED1].get_state(), 'child')

        self.collect_ipaddrs()
        self.collect_rloc16s()

        # 2 & 3
        mleid = self.nodes[DUT_ROUTER2].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        self.assertTrue(self.nodes[LEADER].ping(mleid, size=FRAGMENTED_DATA_LEN))
        self.simulator.go(2)
        self.assertTrue(self.nodes[LEADER].ping(mleid))
        self.simulator.go(2)

        # 4 & 5
        self.assertTrue(self.nodes[LEADER].ping('ff03::1', num_responses=2, size=FRAGMENTED_DATA_LEN))
        self.simulator.go(5)
        self.assertTrue(self.nodes[LEADER].ping('ff03::1', num_responses=2))
        self.simulator.go(5)

        # 6 & 7
        self.assertTrue(self.nodes[LEADER].ping('ff03::2', num_responses=2, size=FRAGMENTED_DATA_LEN))
        self.simulator.go(5)
        self.assertTrue(self.nodes[LEADER].ping('ff03::2', num_responses=2))
        self.simulator.go(5)

        # 8
        self.assertTrue(self.nodes[LEADER].ping(
            config.REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS,
            num_responses=3,
            size=FRAGMENTED_DATA_LEN,
        ))
        self.simulator.go(5)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_MLEID = pv.vars['LEADER_MLEID']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_2 = pv.vars['ROUTER_2']
        ROUTER_2_RLOC16 = pv.vars['ROUTER_2_RLOC16']
        ROUTER_2_MLEID = pv.vars['ROUTER_2_MLEID']
        SED = pv.vars['SED']
        SED_RLOC16 = pv.vars['SED_RLOC16']

        # Step 1: Build the topology as described
        pv.verify_attached('ROUTER_1', 'LEADER')
        pv.verify_attached('ROUTER_2', 'ROUTER_1')
        pv.verify_attached('SED', 'ROUTER_2', 'MTD')

        # Step 2: Leader sends a Fragmented ICMPv6 Echo Request to
        #         DUT's ML-EID
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(LEADER_MLEID, ROUTER_2_MLEID).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(ROUTER_2_MLEID, LEADER_MLEID).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()

        # Step 3: Leader sends a Unfragmented ICMPv6 Echo Request to
        #         DUT’s ML-EID
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(LEADER_MLEID, ROUTER_2_MLEID).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(ROUTER_2_MLEID, LEADER_MLEID).\
            must_next()

        # Step 4: Leader sends a Fragmented ICMPv6 Echo Request to the
        #         Realm-Local All Nodes multicast address (FF03::1)
        #         The DUT MUST respond with an ICMPv6 Echo Reply
        #         The DUT MUST NOT forward the ICMPv6 Echo Request to SED

        _pkt1 = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(REALM_LOCAL_ALL_NODES_ADDRESS).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()
        with pkts.save_index():
            pkts.filter_ping_reply(identifier=_pkt1.icmpv6.echo.identifier).\
                filter_ipv6_src_dst(ROUTER_2_MLEID, LEADER_MLEID).\
                filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
                must_next()
        pkts.filter_ping_request(identifier=_pkt1.icmpv6.echo.identifier).\
            filter_wpan_src16_dst16(ROUTER_2_RLOC16, SED_RLOC16).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_not_next()

        # Step 5: Leader sends an Unfragmented ICMPv6 Echo Request to the
        #         Realm-Local All Nodes multicast address (FF03::1)
        #         The DUT MUST respond with an ICMPv6 Echo Reply
        #         The DUT MUST NOT forward the ICMPv6 Echo Request to SED

        _pkt2 = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(REALM_LOCAL_ALL_NODES_ADDRESS).\
            filter(lambda p: p.icmpv6.echo.sequence_number !=
                   _pkt1.icmpv6.echo.sequence_number
                   ).\
            must_next()
        with pkts.save_index():
            pkts.filter_ping_reply(identifier=_pkt2.icmpv6.echo.identifier).\
                filter_ipv6_src_dst(ROUTER_2_MLEID, LEADER_MLEID).\
                must_next()
        pkts.filter_ping_request(identifier = _pkt2.icmpv6.echo.identifier).\
            filter_wpan_src16_dst16(ROUTER_2_RLOC16, SED_RLOC16).\
            must_not_next()

        # Step 6: Leader sends a Fragmented ICMPv6 Echo Request to the
        #         Realm-Local All Routers multicast address (FF03::2)
        #         The DUT MUST respond with an ICMPv6 Echo Reply
        #         The DUT MUST NOT forward the ICMPv6 Echo Request to SED

        _pkt1 = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(REALM_LOCAL_ALL_ROUTERS_ADDRESS).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()
        with pkts.save_index():
            pkts.filter_ping_reply(identifier=_pkt1.icmpv6.echo.identifier).\
                filter_ipv6_src_dst(ROUTER_2_MLEID, LEADER_MLEID).\
                filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
                must_next()
        pkts.filter_ping_request(identifier=_pkt1.icmpv6.echo.identifier).\
            filter_wpan_src16_dst16(ROUTER_2_RLOC16, SED_RLOC16).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_not_next()

        # Step 7: Leader sends an Unfragmented ICMPv6 Echo Request to the
        #         Realm-Local All Routers multicast address (FF03::2)
        #         The DUT MUST respond with an ICMPv6 Echo Reply
        #         The DUT MUST NOT forward the ICMPv6 Echo Request to SED

        _pkt2 = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(REALM_LOCAL_ALL_ROUTERS_ADDRESS).\
            filter(lambda p: p.icmpv6.echo.sequence_number !=
                   _pkt1.icmpv6.echo.sequence_number
                   ).\
            must_next()
        with pkts.save_index():
            pkts.filter_ping_reply(identifier=_pkt2.icmpv6.echo.identifier).\
                filter_ipv6_src_dst(ROUTER_2_MLEID, LEADER_MLEID).\
                must_next()
        pkts.filter_ping_request(identifier=_pkt2.icmpv6.echo.identifier).\
            filter_wpan_src16_dst16(ROUTER_2_RLOC16, SED_RLOC16).\
            must_not_next()

        # Step 8: Leader sends a Fragmented ICMPv6 Echo Request to the
        #         Realm-Local All Thread Nodes multicast address
        #         The DUT MUST respond with an ICMPv6 Echo Reply
        #         The Realm-Local All Thread Nodes multicast address
        #         MUST be a realm-local Unicast Prefix-Based Multicast
        #         Address [RFC 3306], with:
        #             - flgs set to 3 (P = 1 and T = 1)
        #             - scop set to 3
        #             - plen set to the Mesh Local Prefix length
        #             - network prefix set to the Mesh Local Prefix
        #             - group ID set to 1
        #         The DUT MUST use IEEE 802.15.4 indirect transmissions
        #         to forward packet to SED

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()
        with pkts.save_index():
            pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
                filter_ipv6_src_dst(ROUTER_2_MLEID, LEADER_MLEID).\
                filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
                must_next()
        pkts.filter_ping_request(identifier = _pkt.icmpv6.echo.identifier).\
            filter_wpan_src16_dst16(ROUTER_2_RLOC16, SED_RLOC16).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(SED).\
            filter_ipv6_dst(LEADER_MLEID).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()


if __name__ == '__main__':
    unittest.main()
