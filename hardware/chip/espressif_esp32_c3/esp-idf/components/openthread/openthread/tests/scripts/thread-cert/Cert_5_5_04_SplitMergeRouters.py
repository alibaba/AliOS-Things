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

import thread_cert
from pktverify.consts import MLE_ADVERTISEMENT, MLE_PARENT_REQUEST, MLE_CHILD_ID_RESPONSE, SOURCE_ADDRESS_TLV, ROUTE64_TLV, LEADER_DATA_TLV
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ROUTER1 = 2
ROUTER2 = 3
ROUTER3 = 4
ROUTER4 = 5


class Cert_5_5_4_SplitMergeRouters(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1, ROUTER2]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER, ROUTER3]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER, ROUTER4]
        },
        ROUTER3: {
            'name': 'ROUTER_3',
            'mode': 'rdn',
            'allowlist': [ROUTER1]
        },
        ROUTER4: {
            'name': 'ROUTER_4',
            'mode': 'rdn',
            'allowlist': [ROUTER2]
        },
    }

    def _setUpLeader(self):
        self.nodes[LEADER].add_allowlist(self.nodes[ROUTER1].get_addr64())
        self.nodes[LEADER].add_allowlist(self.nodes[ROUTER2].get_addr64())
        self.nodes[LEADER].enable_allowlist()
        self.nodes[LEADER].set_router_selection_jitter(1)

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'router')

        self.nodes[ROUTER3].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER3].get_state(), 'router')

        self.nodes[ROUTER4].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER4].get_state(), 'router')

        self.nodes[LEADER].reset()
        self._setUpLeader()
        self.simulator.go(150)

        self.nodes[LEADER].start()
        self.simulator.go(50)

        self.assertEqual(self.nodes[LEADER].get_state(), 'router')

        self.collect_ipaddrs()
        addrs = self.nodes[ROUTER4].get_addrs()
        for addr in addrs:
            if addr[0:4] != 'fe80':
                self.assertTrue(self.nodes[ROUTER3].ping(addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_3 = pv.vars['ROUTER_3']
        ROUTER_4 = pv.vars['ROUTER_4']
        leader_pkts = pkts.filter_wpan_src64(LEADER)
        router1_pkts = pkts.filter_wpan_src64(ROUTER_1)

        # Step 2: The Leader  MUST send properly formatted MLE Advertisements
        router1_pkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next()
        leader_pkts.range(router1_pkts.index).filter_mle_cmd(MLE_ADVERTISEMENT).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, ROUTE64_TLV, LEADER_DATA_TLV} == set(p.mle.tlv.type))

        router1_pkts.filter_mle_cmd(MLE_PARENT_REQUEST).must_next()
        lreset_start = router1_pkts.index

        router1_pkts.filter_mle_cmd(MLE_ADVERTISEMENT).must_next()
        lreset_stop = router1_pkts.index

        # Step 3: The Leader MUST stop sending MLE advertisements.
        leader_pkts.range(lreset_start, lreset_stop).filter_mle_cmd(MLE_ADVERTISEMENT).must_not_next()

        # Step 5: Router_4 MUST send an ICMPv6 Echo Reply to Router_3
        router3_mleid = pv.vars['ROUTER_3_MLEID']
        router4_mleid = pv.vars['ROUTER_4_MLEID']
        pkts.filter_wpan_src64(ROUTER_3).filter(
            lambda p: p.ipv6.src == router3_mleid and p.ipv6.dst == router4_mleid).filter_ping_request().must_next()
        pkts.filter_wpan_src64(ROUTER_4).filter(
            lambda p: p.ipv6.src == router4_mleid and p.ipv6.dst == router3_mleid).filter_ping_reply().must_next()


if __name__ == '__main__':
    unittest.main()
