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
from pktverify.consts import MLE_CHILD_ID_REQUEST, MLE_ADVERTISEMENT, MLE_CHILD_UPDATE_REQUEST, SOURCE_ADDRESS_TLV, MODE_TLV, LEADER_DATA_TLV
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ROUTER1 = 2
ROUTER2 = 3
ED = 4


class Cert_6_2_2_NewPartition(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1, ROUTER2]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER, ROUTER2, ED]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'network_id_timeout': 110,
            'allowlist': [LEADER, ROUTER1]
        },
        ED: {
            'name': 'ED',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [ROUTER1]
        },
    }

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

        self.nodes[ED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED].get_state(), 'child')

        self.nodes[LEADER].stop()
        self.simulator.go(140)
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'leader')
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')
        self.assertEqual(self.nodes[ED].get_state(), 'child')

        self.collect_ipaddrs()
        addrs = self.nodes[ED].get_addrs()
        for addr in addrs:
            self.assertTrue(self.nodes[ROUTER1].ping(addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        ROUTER_1 = pv.vars['ROUTER_1']
        ED = pv.vars['ED']
        _router1_pkts = pkts.filter_wpan_src64(ROUTER_1)
        _ed_pkts = pkts.filter_wpan_src64(ED)

        # Step 1: Ensure that the DUT successfully attached to Router_1
        _ed_pkts.filter_mle_cmd(MLE_CHILD_ID_REQUEST).filter_wpan_dst64(ROUTER_1).must_next()

        # Step 2-6 : remove Leader and make ROUTER_2 as new Leader
        # Step 7: The DUT MUST send a MLE Child Update Request to Router_1
        _ed_pkts.filter_mle_cmd(MLE_ADVERTISEMENT).must_not_next()
        _ed_pkts.filter_mle_cmd(MLE_CHILD_ID_REQUEST).must_not_next()
        _ed_pkts.filter_wpan_dst64(ROUTER_1).filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).must_next().must_verify(
            lambda p: {MODE_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV} < set(p.mle.tlv.type))

        # Step 8: The DUT MUST respond with ICMPv6 Echo Reply
        _ed_pkts.filter('ipv6.dst == {ROUTER_1_MLEID} and ipv6.src == {ED_MLEID}',
                        **pv.vars).filter_ping_reply().must_next()


if __name__ == '__main__':
    unittest.main()
