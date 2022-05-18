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
from pktverify.consts import MLE_ADVERTISEMENT, MLE_LINK_REQUEST, MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, ADDR_SOL_URI, VERSION_TLV, TLV_REQUEST_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, CHALLENGE_TLV, LINK_MARGIN_TLV, NL_MAC_EXTENDED_ADDRESS_TLV, NL_STATUS_TLV
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ROUTER1 = 2
ROUTER2 = 3
ROUTER3 = 4
ROUTER15 = 16
REED1 = 17


class Cert_5_5_5_SplitMergeREED(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER2, ROUTER3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, ROUTER15]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [ROUTER3]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER, REED1]
        },
        ROUTER3: {
            'name': 'ROUTER_3',
            'mode': 'rdn',
            'allowlist': [LEADER, ROUTER1]
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
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        for i in range(ROUTER2, ROUTER15 + 1):
            self.nodes[i].start()
            self.simulator.go(5)
            self.assertEqual(self.nodes[i].get_state(), 'router')

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[REED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[REED1].get_state(), 'child')

        self.nodes[ROUTER1].add_allowlist(self.nodes[REED1].get_addr64())
        self.nodes[REED1].add_allowlist(self.nodes[ROUTER1].get_addr64())

        self.nodes[ROUTER3].stop()
        self.simulator.go(140)

        self.assertEqual(self.nodes[ROUTER1].get_state(), 'child')
        self.assertEqual(self.nodes[REED1].get_state(), 'router')

        self.collect_ipaddrs()

        addrs = self.nodes[LEADER].get_addrs()
        for addr in addrs:
            if addr[0:4] != 'fe80':
                self.assertTrue(self.nodes[ROUTER1].ping(addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_2 = pv.vars['ROUTER_2']
        ROUTER_3 = pv.vars['ROUTER_3']
        REED = pv.vars['REED']
        router1_pkts = pkts.filter_wpan_src64(ROUTER_1)
        router2_pkts = pkts.filter_wpan_src64(ROUTER_2)
        reed_pkts = pkts.filter_wpan_src64(REED)

        router2_pkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).filter_wpan_dst64(REED).must_next()
        _start_idx = router2_pkts.index
        reed_pkts.index = _start_idx

        # Step 3: Remove Router_3 from the network
        # Router_1 attempt to re-attach to partition.
        # Sends multicast Parent Request to Routers and REEDs
        router1_pkts.range(_start_idx).must_next()
        router1_pkts.filter_mle_cmd(MLE_PARENT_REQUEST).must_next()

        # filter MLE_ADVERTISEMENT with 15 routing table entry:
        # 1 byte ID Sequence + 8 bytes ID Mask + 15 bytes Routing Table Entry =
        # 24 (Router64 tlv length)
        pkts.range(_start_idx).filter_mle_cmd(MLE_ADVERTISEMENT).filter(lambda p: 24 in p.mle.tlv.len).must_next()
        _end_idx = pkts.index

        # Step 2: The DUT MUST NOT attempt to become an active router by sending an Address Solicit Request
        reed_pkts.range(_start_idx, _end_idx).filter_coap_request(ADDR_SOL_URI).must_not_next()

        # Step 4: The DUT send a Parent Response to Router_1
        reed_pkts.filter_mle_cmd(MLE_PARENT_RESPONSE).must_next().must_verify(lambda p: p.wpan.dst64 == ROUTER_1)

        # Step 5: Router_1 Send MLE Child ID Request to the DUT
        router1_pkts.range(reed_pkts.index).filter_mle_cmd(MLE_CHILD_ID_REQUEST).must_next()

        # Step 6: DUT send an Address Solicit Request to Leader,
        # receives short address and becomes a router
        pkts.range(_end_idx).filter_wpan_src64(REED).filter_coap_request(ADDR_SOL_URI).must_next().must_verify(
            lambda p: {NL_MAC_EXTENDED_ADDRESS_TLV, NL_STATUS_TLV} <= set(p.coap.tlv.type))

        # Step 7: DUT send a Multicast Link Request
        pkts.filter_wpan_src64(REED).filter_mle_cmd(MLE_LINK_REQUEST).must_next().must_verify(lambda p: {
            VERSION_TLV, TLV_REQUEST_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, CHALLENGE_TLV, LINK_MARGIN_TLV
        } <= set(p.mle.tlv.type))

        # Step 8: DUT send Child ID Response to Router_1
        reed_pkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next().must_verify(lambda p: p.wpan.dst64 == ROUTER_1)

        # Step 9: The DUT MUST route the ICMPv6 Echo request to the Leader.
        # The DUT MUST route the ICMPv6 Echo reply back to Router_1
        leader_mleid = pv.vars['LEADER_MLEID']
        router1_mleid = pv.vars['ROUTER_1_MLEID']
        reed_pkts.filter_ipv6_src_dst(router1_mleid, leader_mleid).filter_ping_request().must_next()
        reed_pkts.filter_ipv6_src_dst(leader_mleid, router1_mleid).filter_ping_reply().must_next()


if __name__ == '__main__':
    unittest.main()
