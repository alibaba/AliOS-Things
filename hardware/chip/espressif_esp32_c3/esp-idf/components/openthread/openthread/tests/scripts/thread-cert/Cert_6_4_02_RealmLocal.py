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
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ROUTER = 2
MTD = 3
FRAGMENTED_DATA_LEN = 256

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate the Realm-Local addresses
# that the DUT configures.
#
# Test Topology:
# -------------
#  Leader
#    |
#  Router
#    |
#   DUT
#
# DUT Types:
# ----------
#  ED
#  SED


class Cert_6_4_2_RealmLocal_Base(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER, MTD]
        },
        MTD: {
            'name': 'DUT',
            'is_mtd': True,
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[MTD].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MTD].get_state(), 'child')

        self.collect_ipaddrs()
        self.collect_rloc16s()

        dut_addr = self.nodes[MTD].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        self.assertTrue(self.nodes[LEADER].\
                        ping(dut_addr,
                             size=FRAGMENTED_DATA_LEN))
        self.simulator.go(1)
        self.assertTrue(self.nodes[LEADER].\
                        ping(dut_addr))
        self.simulator.go(1)

        if self.TOPOLOGY[MTD]['mode'] == 'rn':
            self.assertTrue(self.nodes[LEADER].\
                            ping(config.REALM_LOCAL_ALL_NODES_ADDRESS,
                                 num_responses=2,
                                 size=FRAGMENTED_DATA_LEN))
            self.simulator.go(2)
            self.assertTrue(self.nodes[LEADER].\
                            ping(config.REALM_LOCAL_ALL_NODES_ADDRESS,
                                 num_responses=2))
            self.simulator.go(2)

        self.assertTrue(self.nodes[LEADER].\
                        ping(config.REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS,
                             num_responses=2,
                             size=FRAGMENTED_DATA_LEN))
        self.simulator.go(2)
        self.assertTrue(self.nodes[LEADER].\
                        ping(config.REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS,
                             num_responses=2))
        self.simulator.go(2)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_MLEID = pv.vars['LEADER_MLEID']
        ROUTER = pv.vars['ROUTER']
        ROUTER_MLEID = pv.vars['ROUTER_MLEID']
        ROUTER_RLOC16 = pv.vars['ROUTER_RLOC16']
        DUT = pv.vars['DUT']
        DUT_MLEID = pv.vars['DUT_MLEID']
        DUT_RLOC16 = pv.vars['DUT_RLOC16']

        # Step 1: Ensure topology is formed correctly
        pv.verify_attached('ROUTER', 'LEADER')
        pv.verify_attached('DUT', 'ROUTER', 'MTD')

        # Step 2: Leader sends a Fragmented ICMPv6 Echo Request to
        #         DUT's ML-EID
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(LEADER_MLEID, DUT_MLEID).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(DUT_MLEID, LEADER_MLEID).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()

        # Step 3: Leader sends an Unfragmented ICMPv6 Echo Request to
        #         DUT’s ML-EID
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(LEADER_MLEID, DUT_MLEID).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(DUT_MLEID, LEADER_MLEID).\
            must_next()

        if self.TOPOLOGY[MTD]['mode'] == 'rn':
            # Step 4: Leader sends a Fragmented ICMPv6 Echo Request to the
            #         Realm-Local All Nodes multicast address (FF03::1)
            #         The DUT MUST respond with an ICMPv6 Echo Reply

            _pkt1 = pkts.filter_ping_request().\
                filter_wpan_src64(LEADER).\
                filter_RLANMA().\
                filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
                must_next()
            with pkts.save_index():
                pkts.filter_ping_reply(identifier=_pkt1.icmpv6.echo.identifier).\
                    filter_ipv6_src_dst(ROUTER_MLEID, LEADER_MLEID).\
                    filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
                    must_next()
            pkts.filter_ping_request(identifier=_pkt1.icmpv6.echo.identifier).\
                filter_wpan_src16_dst16(ROUTER_RLOC16, DUT_RLOC16).\
                filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
                must_not_next()

            # Step 5: Leader sends an Unfragmented ICMPv6 Echo Request to the
            #         Realm-Local All Nodes multicast address (FF03::1)
            #         The DUT MUST respond with an ICMPv6 Echo Reply

            _pkt2 = pkts.filter_ping_request().\
                filter_wpan_src64(LEADER).\
                filter_RLANMA().\
                filter(lambda p: p.icmpv6.echo.sequence_number !=
                       _pkt1.icmpv6.echo.sequence_number
                       ).\
                must_next()
            with pkts.save_index():
                pkts.filter_ping_reply(identifier=_pkt2.icmpv6.echo.identifier).\
                    filter_ipv6_src_dst(ROUTER_MLEID, LEADER_MLEID).\
                    must_next()
            pkts.filter_ping_request(identifier = _pkt2.icmpv6.echo.identifier).\
                filter_wpan_src16_dst16(ROUTER_RLOC16, DUT_RLOC16).\
                must_not_next()

        # Step 6: Leader sends a Fragmented ICMPv6 Echo Request to the
        #         Realm-Local All Thread Nodes multicast address
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_RLATNMA().\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(DUT).\
            filter_ipv6_dst(LEADER_MLEID).\
            filter(lambda p: p.icmpv6.data.len == FRAGMENTED_DATA_LEN).\
            must_next()

        # Step 7: Leader sends an Unfragmented ICMPv6 Echo Request to the
        #         Realm-Local All Thread Nodes multicast address
        #         The DUT MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_RLATNMA().\
            filter(lambda p: p.icmpv6.data.len != FRAGMENTED_DATA_LEN).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(DUT).\
            filter_ipv6_dst(LEADER_MLEID).\
            must_next()


class Cert_6_4_2_RealmLocal_ED(Cert_6_4_2_RealmLocal_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_4_2_RealmLocal_Base.TOPOLOGY)
    TOPOLOGY[MTD]['mode'] = 'rn'


class Cert_6_4_2_RealmLocal_SED(Cert_6_4_2_RealmLocal_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_4_2_RealmLocal_Base.TOPOLOGY)
    TOPOLOGY[MTD]['mode'] = '-'


del (Cert_6_4_2_RealmLocal_Base)

if __name__ == '__main__':
    unittest.main()
