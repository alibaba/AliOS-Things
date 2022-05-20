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
from pktverify.consts import MLE_ADVERTISEMENT, MLE_CHILD_ID_RESPONSE, MLE_DATA_REQUEST, MLE_DATA_RESPONSE, TLV_REQUEST_TLV, NETWORK_DATA_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NWD_PREFIX_TLV, NWD_BORDER_ROUTER_TLV, NWD_6LOWPAN_ID_TLV
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ROUTER1 = 2
ROUTER2 = 3
ROUTER3 = 4
ROUTER15 = 16
REED1 = 17


class Cert_5_6_7_NetworkDataRequestREED(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1, ROUTER2, ROUTER3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, ROUTER15]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER, REED1]
        },
        ROUTER3: {
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        5: {
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        6: {
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        7: {
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        8: {
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        9: {
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        10: {
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        11: {
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        12: {
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        13: {
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        14: {
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        15: {
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        ROUTER15: {
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        REED1: {
            'name': 'REED',
            'mode': 'rdn',
            'allowlist': [ROUTER2]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(4)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        for i in range(ROUTER1, ROUTER15 + 1):
            self.nodes[i].start()
            self.simulator.go(5)
            self.assertEqual(self.nodes[i].get_state(), 'router')

        self.nodes[REED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[REED1].get_state(), 'child')

        self.nodes[ROUTER2].remove_allowlist(self.nodes[REED1].get_addr64())
        self.nodes[REED1].remove_allowlist(self.nodes[ROUTER2].get_addr64())

        self.nodes[LEADER].add_prefix('2001:2:0:3::/64', 'paros')
        self.nodes[LEADER].register_netdata()

        # Set lowpan context of sniffer
        self.simulator.set_lowpan_context(1, '2001:2:0:3::/64')

        self.simulator.go(2)

        self.nodes[ROUTER2].add_allowlist(self.nodes[REED1].get_addr64())
        self.nodes[REED1].add_allowlist(self.nodes[ROUTER2].get_addr64())

        self.simulator.go(30)
        self.simulator.go(600)

        addrs = self.nodes[REED1].get_addrs()
        self.assertTrue(any('2001:2:0:3' in addr[0:10] for addr in addrs))
        for addr in addrs:
            if addr[0:10] == '2001:2:0:3':
                self.assertTrue(self.nodes[LEADER].ping(addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        REED = pv.vars['REED']

        # Step 4: Leader multicasts a MLE Data Response with the new information
        _pkt = pkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).filter_wpan_dst64(REED).must_next()
        pkts.filter_wpan_src64(LEADER).filter_mle_cmd(MLE_DATA_RESPONSE).must_next().must_verify(
            lambda p: {NWD_PREFIX_TLV, NWD_BORDER_ROUTER_TLV, NWD_6LOWPAN_ID_TLV} <= set(p.thread_nwd.tlv.type))

        # Step 8: REED1 MUST send a MLE Data Request to its parent to get the new Network Dataset.
        # MLE Data Request includes a TLV Request TLV for Network Data TLV
        pkts.filter_wpan_src64(REED).filter_mle_cmd(MLE_DATA_REQUEST).must_next().must_verify(
            lambda p: {TLV_REQUEST_TLV, NETWORK_DATA_TLV} <= set(p.mle.tlv.type))

        # Step 10: REED1 send MLE Advertisement
        pkts.filter_wpan_src64(REED).filter_mle_cmd(MLE_ADVERTISEMENT).must_next().must_verify(
            lambda p: p.mle.tlv.leader_data.data_version == (_pkt.mle.tlv.leader_data.data_version + 1) % 256)


if __name__ == '__main__':
    unittest.main()
