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
import copy

import config
import thread_cert
from pktverify.consts import LINK_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
MTD = 2
FRAGMENTED_DATA_LEN = 256

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate the Link-Local addresses
# that the DUT auto-configures.
#
# Test Topology:
# -------------
#  Leader
#    |
#   DUT
#
# DUT Types:
# ----------
#  ED
#  SED


class Cert_6_4_1_LinkLocal_Base(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [MTD]
        },
        MTD: {
            'name': 'DUT',
            'is_mtd': True,
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [LEADER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[MTD].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MTD].get_state(), 'child')

        self.collect_ipaddrs()

        dut_addr = self.nodes[MTD].get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)
        self.assertTrue(self.nodes[LEADER].ping(dut_addr, size=FRAGMENTED_DATA_LEN))
        self.simulator.go(1)
        self.assertTrue(self.nodes[LEADER].ping(dut_addr))
        self.simulator.go(1)

        self.assertTrue(self.nodes[LEADER].ping(config.LINK_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS,
                                                size=FRAGMENTED_DATA_LEN))
        self.simulator.go(1)
        self.assertTrue(self.nodes[LEADER].ping(config.LINK_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS))
        self.simulator.go(1)

        if self.TOPOLOGY[MTD]['mode'] == 'rn':
            self.assertTrue(self.nodes[LEADER].ping(config.LINK_LOCAL_ALL_NODES_ADDRESS, size=FRAGMENTED_DATA_LEN))
            self.simulator.go(1)
            self.assertTrue(self.nodes[LEADER].ping(config.LINK_LOCAL_ALL_NODES_ADDRESS))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_LLA = pv.vars['LEADER_LLA']
        DUT_LLA = pv.vars['DUT_LLA']

        # Step 1: Ensure topology is formed correctly
        pv.verify_attached('DUT', 'LEADER', 'MTD')

        # Step 2: Leader sends a Fragmented ICMPv6 Echo Request to DUT’s
        #         MAC extended address based Link-Local address
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(LEADER_LLA, DUT_LLA).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(DUT_LLA, LEADER_LLA).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()

        # Step 3: Leader sends an Unfragmented ICMPv6 Echo Request to DUT’s
        #         MAC extended address based Link-Local address
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(LEADER_LLA, DUT_LLA).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(DUT_LLA, LEADER_LLA).\
            must_next()

        # Step 4: Leader sends a Fragmented ICMPv6 Echo Request to the
        #         Link-Local All thread nodes multicast address
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_LLATNMA().\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(DUT_LLA, LEADER_LLA).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()

        # Step 5: Leader sends an Unfragmented ICMPv6 Echo Request to the
        #         Link-Local All thread nodes multicast address
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_LLATNMA().\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(DUT_LLA, LEADER_LLA).\
            must_next()

        if self.TOPOLOGY[MTD]['mode'] == 'rn':
            # Step 6: Leader sends a Fragmented ICMPv6 Echo Request to the
            #         Link-Local All Nodes multicast address (FF02::1)
            #         The DUT MUST respond with an ICMPv6 Echo Reply

            _pkt = pkts.filter_ping_request().\
                filter_wpan_src64(LEADER).\
                filter_LLANMA().\
                filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
                must_next()
            pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
                filter_ipv6_src_dst(DUT_LLA, LEADER_LLA).\
                filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
                must_next()

            # Step 7: Leader sends an Unfragmented ICMPv6 Echo Request to the
            #         Link-Local All Nodes multicast address (FF02::1)
            #         The DUT MUST respond with an ICMPv6 Echo Reply

            _pkt = pkts.filter_ping_request().\
                filter_wpan_src64(LEADER).\
                filter_LLANMA().\
                must_next()
            pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
                filter_ipv6_src_dst(DUT_LLA, LEADER_LLA).\
                must_next()


class Cert_6_4_1_LinkLocal_ED(Cert_6_4_1_LinkLocal_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_4_1_LinkLocal_Base.TOPOLOGY)
    TOPOLOGY[MTD]['mode'] = 'rn'


class Cert_6_4_1_LinkLocal_SED(Cert_6_4_1_LinkLocal_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_4_1_LinkLocal_Base.TOPOLOGY)
    TOPOLOGY[MTD]['mode'] = '-'


del (Cert_6_4_1_LinkLocal_Base)

if __name__ == '__main__':
    unittest.main()
