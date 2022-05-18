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
from pktverify.consts import MLE_CHILD_ID_REQUEST, MLE_PARENT_REQUEST, MLE_CHILD_ID_RESPONSE, MLE_ANNOUNCE, CHANNEL_TLV, PAN_ID_TLV, ACTIVE_TIMESTAMP_TLV, LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS
from pktverify.packet_verifier import PacketVerifier

LEADER1 = 1
ROUTER1 = 2
LEADER2 = 3
MED = 4

DATASET1_TIMESTAMP = 20
DATASET1_CHANNEL = 11
DATASET1_PANID = 0xface

DATASET2_TIMESTAMP = 10
DATASET2_CHANNEL = 12
DATASET2_PANID = 0xafce


class Cert_9_2_12_Announce(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER1: {
            'name': 'LEADER_1',
            'active_dataset': {
                'timestamp': DATASET1_TIMESTAMP,
                'panid': DATASET1_PANID,
                'channel': DATASET1_CHANNEL
            },
            'mode': 'rdn',
            'allowlist': [ROUTER1]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'active_dataset': {
                'timestamp': DATASET1_TIMESTAMP,
                'panid': DATASET1_PANID,
                'channel': DATASET1_CHANNEL
            },
            'mode': 'rdn',
            'allowlist': [LEADER1, LEADER2]
        },
        LEADER2: {
            'name': 'LEADER_2',
            'active_dataset': {
                'timestamp': DATASET2_TIMESTAMP,
                'panid': DATASET2_PANID,
                'channel': DATASET2_CHANNEL
            },
            'mode': 'rdn',
            'allowlist': [MED, ROUTER1]
        },
        MED: {
            'name': 'MED',
            'channel': DATASET2_CHANNEL,
            'is_mtd': True,
            'mode': 'rn',
            'panid': DATASET2_PANID,
            'allowlist': [LEADER2]
        },
    }

    def test(self):
        self.nodes[LEADER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER1].get_state(), 'leader')
        self.nodes[LEADER1].commissioner_start()
        self.simulator.go(3)

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[LEADER2].start()
        self.nodes[LEADER2].set_state('leader')
        self.assertEqual(self.nodes[LEADER2].get_state(), 'leader')

        self.nodes[MED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MED].get_state(), 'child')

        ipaddrs = self.nodes[ROUTER1].get_addrs()
        for ipaddr in ipaddrs:
            if ipaddr[0:4] != 'fe80':
                break

        self.nodes[LEADER1].announce_begin(0x1000, 1, 1000, ipaddr)
        self.simulator.go(30)
        self.assertEqual(self.nodes[LEADER2].get_state(), 'router')
        self.assertEqual(self.nodes[MED].get_state(), 'child')

        self.collect_rlocs()
        ipaddrs = self.nodes[MED].get_addrs()
        for ipaddr in ipaddrs:
            if ipaddr[0:4] != 'fe80':
                self.assertTrue(self.nodes[LEADER1].ping(ipaddr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER_1 = pv.vars['LEADER_1']
        ROUTER_1 = pv.vars['ROUTER_1']
        LEADER_2 = pv.vars['LEADER_2']
        LEADER_1_RLOC = pv.vars['LEADER_1_RLOC']
        MED = pv.vars['MED']
        MED_RLOC = pv.vars['MED_RLOC']

        # Step 1: Ensure the topology is formed correctly
        pkts.filter_wpan_src64(LEADER_1).filter_wpan_dst64(ROUTER_1).filter_mle_cmd(
            MLE_CHILD_ID_RESPONSE).must_next().must_verify(lambda p: p.wpan.dst_pan == DATASET1_PANID)
        pkts.copy().filter_wpan_src64(LEADER_2).filter_wpan_dst64(MED).filter_mle_cmd(
            MLE_CHILD_ID_RESPONSE).must_next().must_verify(lambda p: p.wpan.dst_pan == DATASET2_PANID)

        # Step 4: Leader_2 MUST send a MLE Child ID Request on its new channel to Router_1
        # LEADER_2 MUST send a MLE Announce Message
        # The Destination PAN ID (0xFFFF) in the IEEE 802.15.4 MAC and MUST be secured using Key ID Mode 2.
        pkts.filter_wpan_src64(LEADER_2).filter_wpan_dst64(ROUTER_1).filter_mle_cmd(
            MLE_CHILD_ID_REQUEST).must_next().must_verify(lambda p: p.wpan.dst_pan == DATASET1_PANID)

        pkts.filter_wpan_src64(LEADER_2).filter_ipv6_dst(LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS).filter_mle_cmd(
            MLE_ANNOUNCE).must_next().must_verify(
                lambda p: {CHANNEL_TLV, PAN_ID_TLV, ACTIVE_TIMESTAMP_TLV} == set(p.mle.tlv.type) and p.wpan.dst_pan ==
                0xffff and p.wpan.aux_sec.key_id_mode == 0x2 and p.wpan.aux_sec.key_source == 0x00000000ffffffff)

        # Step 5: MED MUST send a MLE Child ID Request on its new channel
        # MED MUST send a MLE Announce Message
        # The Destination PAN ID (0xFFFF) in the IEEE 802.15.4 MAC and MUST be secured using Key ID Mode 2.
        pkts.filter_wpan_src64(MED).filter_wpan_dst64(LEADER_2).filter_mle_cmd(
            MLE_CHILD_ID_REQUEST).must_next().must_verify(lambda p: p.wpan.dst_pan == DATASET1_PANID)

        pkts.filter_wpan_src64(MED).filter_ipv6_dst(LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS).filter_mle_cmd(
            MLE_ANNOUNCE).must_next().must_verify(
                lambda p: {CHANNEL_TLV, PAN_ID_TLV, ACTIVE_TIMESTAMP_TLV} == set(p.mle.tlv.type) and p.wpan.dst_pan ==
                0xffff and p.wpan.aux_sec.key_id_mode == 0x2 and p.wpan.aux_sec.key_source == 0x00000000ffffffff)

        # Step 6: MED MUST respond with an ICMPv6 Echo Reply
        pkts.filter_ping_reply().filter_ipv6_src_dst(MED_RLOC, LEADER_1_RLOC).must_next()


if __name__ == '__main__':
    unittest.main()
