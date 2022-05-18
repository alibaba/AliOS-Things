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
from pktverify.consts import LINK_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
DUT_ROUTER1 = 2
FRAGMENTED_DATA_LEN = 256

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate the Link-Local addresses
# that the DUT auto-configures.
#
# Test Topology:
# -------------
# Leader
#    |
# Router(DUT)
#
# DUT Types:
# ----------
#  Router


class Cert_5_3_1_LinkLocal(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
        },
        DUT_ROUTER1: {
            'name': 'ROUTER',
            'mode': 'rdn',
        },
    }

    def test(self):
        # 1
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[DUT_ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT_ROUTER1].get_state(), 'router')

        self.collect_rlocs()
        self.collect_ipaddrs()

        # 2 & 3
        link_local = self.nodes[DUT_ROUTER1].get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)
        self.assertTrue(self.nodes[LEADER].ping(link_local, size=FRAGMENTED_DATA_LEN))
        self.assertTrue(self.nodes[LEADER].ping(link_local))

        # 4 & 5
        self.assertTrue(self.nodes[LEADER].ping(config.LINK_LOCAL_ALL_NODES_ADDRESS, size=FRAGMENTED_DATA_LEN))
        self.assertTrue(self.nodes[LEADER].ping(config.LINK_LOCAL_ALL_NODES_ADDRESS))

        # 6 & 7
        self.assertTrue(self.nodes[LEADER].ping(config.LINK_LOCAL_ALL_ROUTERS_ADDRESS, size=FRAGMENTED_DATA_LEN))
        self.assertTrue(self.nodes[LEADER].ping(config.LINK_LOCAL_ALL_ROUTERS_ADDRESS))

        # 8
        self.assertTrue(self.nodes[LEADER].ping(config.LINK_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_LLA = pv.vars['LEADER_LLA']
        ROUTER_LLA = pv.vars['ROUTER_LLA']

        # Step 1: Build the topology as described
        pv.verify_attached('ROUTER')

        # Step 2: Leader sends a Fragmented ICMPv6 Echo Request to DUT’s
        #         MAC extended address based Link-Local address
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(LEADER_LLA, ROUTER_LLA).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(ROUTER_LLA, LEADER_LLA).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()

        # Step 3: Leader sends an Unfragmented ICMPv6 Echo Request to DUT’s
        #         MAC extended address based Link-Local address
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(LEADER_LLA, ROUTER_LLA).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(ROUTER_LLA, LEADER_LLA).\
            must_next()

        # Step 4: Leader sends a Fragmented ICMPv6 Echo Request to the
        #         Link-Local All Nodes multicast address (FF02::1)
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(ROUTER_LLA, LEADER_LLA).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()

        # Step 5: Leader sends an Unfragmented ICMPv6 Echo Request to the
        #         Link-Local All Nodes multicast address (FF02::1)
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(ROUTER_LLA, LEADER_LLA).\
            must_next()

        # Step 6: Leader sends a Fragmented ICMPv6 Echo Request to the
        #         Link-Local All Routers multicast address (FF02::2)
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_LLARMA().\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(ROUTER_LLA, LEADER_LLA).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()

        # Step 7: Leader sends an Unfragmented ICMPv6 Echo Request to the
        #         Link-Local All Routers multicast address (FF02::2)
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_LLARMA().\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(ROUTER_LLA, LEADER_LLA).\
            must_next()

        # Step 8: Leader sends an Unfragmented ICMPv6 Echo Request to the
        #         Link-Local All Thread Nodes multicast address
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(LEADER_LLA,
                                LINK_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(ROUTER_LLA, LEADER_LLA).\
            must_next()


if __name__ == '__main__':
    unittest.main()
