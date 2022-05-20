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
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
DUT_ROUTER1 = 2
ROUTER2 = 3
ROUTER3 = 4

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to ensure that the DUT routes traffic properly
# when link qualities between the nodes are adjusted.
#
# Test Topology:
# -------------
#            Leader
#            /    \
#     Router_2 - Router_1(DUT)
#                   |
#                Router_3
#
# DUT Types:
# ----------
#  Router


class Cert_5_3_5_RoutingLinkQuality(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [DUT_ROUTER1, ROUTER2]
        },
        DUT_ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER, ROUTER2, ROUTER3]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER, DUT_ROUTER1]
        },
        ROUTER3: {
            'name': 'ROUTER_3',
            'mode': 'rdn',
            'allowlist': [DUT_ROUTER1]
        },
    }

    def test(self):
        # 1
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        for router in range(DUT_ROUTER1, ROUTER3 + 1):
            self.nodes[router].start()
        self.simulator.go(10)

        for router in range(DUT_ROUTER1, ROUTER3 + 1):
            self.assertEqual(self.nodes[router].get_state(), 'router')

        self.collect_rlocs()
        self.collect_rloc16s()

        # 2 & 3
        leader_rloc = self.nodes[LEADER].get_ip6_address(config.ADDRESS_TYPE.RLOC)
        self.assertTrue(self.nodes[ROUTER3].ping(leader_rloc))

        # 4 & 5
        self.nodes[LEADER].add_allowlist(self.nodes[DUT_ROUTER1].get_addr64(), config.RSSI['LINK_QULITY_1'])
        self.nodes[DUT_ROUTER1].add_allowlist(self.nodes[LEADER].get_addr64(), config.RSSI['LINK_QULITY_1'])
        self.simulator.go(3 * config.MAX_ADVERTISEMENT_INTERVAL)

        self.assertTrue(self.nodes[ROUTER3].ping(leader_rloc))

        # 6 & 7
        self.nodes[LEADER].add_allowlist(self.nodes[DUT_ROUTER1].get_addr64(), config.RSSI['LINK_QULITY_2'])
        self.nodes[DUT_ROUTER1].add_allowlist(self.nodes[LEADER].get_addr64(), config.RSSI['LINK_QULITY_2'])
        self.simulator.go(3 * config.MAX_ADVERTISEMENT_INTERVAL)

        self.assertTrue(self.nodes[ROUTER3].ping(leader_rloc))

        # 8 & 9
        self.nodes[LEADER].add_allowlist(self.nodes[DUT_ROUTER1].get_addr64(), config.RSSI['LINK_QULITY_0'])
        self.nodes[DUT_ROUTER1].add_allowlist(self.nodes[LEADER].get_addr64(), config.RSSI['LINK_QULITY_0'])
        self.simulator.go(3 * config.MAX_ADVERTISEMENT_INTERVAL)

        self.assertTrue(self.nodes[ROUTER3].ping(leader_rloc))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        LEADER_RLOC16 = pv.vars['LEADER_RLOC16']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_1_RLOC = pv.vars['ROUTER_1_RLOC']
        ROUTER_1_RLOC16 = pv.vars['ROUTER_1_RLOC16']
        ROUTER_2 = pv.vars['ROUTER_2']
        ROUTER_2_RLOC16 = pv.vars['ROUTER_2_RLOC16']
        ROUTER_2_RLOC = pv.vars['ROUTER_2_RLOC']
        ROUTER_3 = pv.vars['ROUTER_3']
        ROUTER_3_RLOC = pv.vars['ROUTER_3_RLOC']
        MM = pv.vars['MM_PORT']

        # Step 1: Ensure topology is formed correctly
        for i in range(1, 4):
            with pkts.save_index():
                pv.verify_attached('ROUTER_%d' % i)

        # Step 2: Modify the link quality between the DUT and the Leader to be 3
        # Step 3: Router_3 sends an ICMPv6 Echo Request to the Leader
        #         The ICMPv6 Echo Request MUST take the shortest path:
        #             Router_3 -> DUT -> Leader
        #         The hopsLft field of the 6LoWPAN Mesh Header MUST be greater than
        #         the route cost to the destination

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(ROUTER_3).\
            filter_ipv6_dst(LEADER_RLOC).\
            must_next()
        _pkt.must_verify(lambda p: p.lowpan.mesh.hops > 2)
        pkts.filter_ping_request(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_1).\
            filter_wpan_dst16(LEADER_RLOC16).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(ROUTER_3_RLOC).\
            must_next()

        # Step 4: Modify the link quality between the DUT and the Leader to be 1
        # Step 5: Router_3 sends an ICMPv6 Echo Request to the Leader
        #         The ICMPv6 Echo Request MUST take the shortest path:
        #             Router_3 -> DUT -> Router_2 -> Leader
        #         The hopsLft field of the 6LoWPAN Mesh Header MUST be greater than
        #         the route cost to the destination

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(ROUTER_3).\
            filter_ipv6_dst(LEADER_RLOC).\
            must_next()
        _pkt.must_verify(lambda p: p.lowpan.mesh.hops > 3)
        pkts.filter_ping_request(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_1).\
            filter_wpan_dst16(ROUTER_2_RLOC16).\
            must_next()
        pkts.filter_ping_request(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_2).\
            filter_wpan_dst16(LEADER_RLOC16).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(ROUTER_3_RLOC).\
            must_next()

        # Step 6: Modify the link quality between the DUT and the Leader to be 2
        # Step 7: Router_3 sends an ICMPv6 Echo Request to the Leader
        #         The ICMPv6 Echo Request MUST take the shortest path:
        #             Router_3 -> DUT -> Leader
        #         The hopsLft field of the 6LoWPAN Mesh Header MUST be greater than
        #         the route cost to the destination

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(ROUTER_3).\
            filter_ipv6_dst(LEADER_RLOC).\
            must_next()
        _pkt.must_verify(lambda p: p.lowpan.mesh.hops > 2)
        pkts.filter_ping_request(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_1).\
            filter_wpan_dst16(LEADER_RLOC16).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(ROUTER_3_RLOC).\
            must_next()

        # Step 8: Modify the link quality between the DUT and the Leader to be 0
        # Step 9: Router_3 sends an ICMPv6 Echo Request to the Leader
        #         The ICMPv6 Echo Request MUST take the shortest path:
        #             Router_3 -> DUT -> Router_2 -> Leader
        #         The hopsLft field of the 6LoWPAN Mesh Header MUST be greater than
        #         the route cost to the destination

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(ROUTER_3).\
            filter_ipv6_dst(LEADER_RLOC).\
            must_next()
        _pkt.must_verify(lambda p: p.lowpan.mesh.hops > 3)
        pkts.filter_ping_request(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_1).\
            filter_wpan_dst16(ROUTER_2_RLOC16).\
            must_next()
        pkts.filter_ping_request(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_2).\
            filter_wpan_dst16(LEADER_RLOC16).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(ROUTER_3_RLOC).\
            must_next()


if __name__ == '__main__':
    unittest.main()
