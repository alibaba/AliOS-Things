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
from pktverify.consts import MLE_ADVERTISEMENT, MLE_PARENT_REQUEST, MLE_CHILD_ID_RESPONSE, MLE_ANNOUNCE, CHANNEL_TLV, PAN_ID_TLV, ACTIVE_TIMESTAMP_TLV
from pktverify.packet_verifier import PacketVerifier

CHANNEL1 = 11
CHANNEL2 = 18
CHANNEL_MASK = 1 << 18
PANID_INIT = 0xface

LEADER1 = 1
LEADER2 = 2
ED1 = 3


class Cert_9_2_17_Orphan(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER1: {
            'name': 'LEADER_1',
            'active_dataset': {
                'timestamp': 10,
                'panid': PANID_INIT,
                'channel': CHANNEL1,
                'channel_mask': CHANNEL_MASK
            },
            'mode': 'rdn',
            'allowlist': [ED1]
        },
        LEADER2: {
            'name': 'LEADER_2',
            'active_dataset': {
                'timestamp': 20,
                'panid': PANID_INIT,
                'channel': CHANNEL2,
                'channel_mask': CHANNEL_MASK
            },
            'mode': 'rdn',
        },
        ED1: {
            'name': 'ED',
            'channel': CHANNEL1,
            'is_mtd': True,
            'mode': 'rn',
            'panid': PANID_INIT,
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [LEADER1]
        },
    }

    def test(self):
        self.nodes[LEADER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER1].get_state(), 'leader')

        self.nodes[LEADER2].start()
        self.nodes[LEADER2].set_state('leader')
        self.assertEqual(self.nodes[LEADER2].get_state(), 'leader')

        self.nodes[ED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED1].get_state(), 'child')

        self.nodes[LEADER1].stop()
        self.nodes[LEADER2].add_allowlist(self.nodes[ED1].get_addr64())
        self.nodes[ED1].add_allowlist(self.nodes[LEADER2].get_addr64())
        self.simulator.go(20)

        self.assertEqual(self.nodes[ED1].get_state(), 'child')
        self.assertEqual(self.nodes[ED1].get_channel(), CHANNEL2)

        self.collect_ipaddrs()
        ipaddrs = self.nodes[ED1].get_addrs()
        for ipaddr in ipaddrs:
            self.assertTrue(self.nodes[LEADER2].ping(ipaddr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER_1 = pv.vars['LEADER_1']
        LEADER_2 = pv.vars['LEADER_2']
        ED = pv.vars['ED']

        # Step 1: Ensure the topology is formed correctly
        # Verify that Leader_1 & Leader_2 are sending MLE Advertisements on separate channels.
        pkts.filter_wpan_src64(LEADER_1).filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next().must_verify(
            lambda p: p.wpan.dst64 == ED and p.thread_meshcop.tlv.channel == [CHANNEL1])
        pkts.filter_wpan_src64(LEADER_1).filter_mle_cmd(MLE_ADVERTISEMENT).must_next()
        pkts.copy().filter_wpan_src64(LEADER_2).filter_mle_cmd(MLE_ADVERTISEMENT).must_next()

        # Step 4: powers-down Leader_1 and enables connectivity between the ED and Leader_2
        # ED MUST send a MLE Parent Request
        _epkts = pkts.filter_wpan_src64(ED)
        _epkts.filter_mle_cmd(MLE_PARENT_REQUEST).must_next()

        # Step 6: ED MUST send a MLE Announce Message
        # The Destination PAN ID (0xFFFF) in the IEEE 802.15.4 MAC and MUST be secured using Key ID Mode 2.
        _epkts.filter_mle_cmd(MLE_ANNOUNCE).must_next().must_verify(
            lambda p: {CHANNEL_TLV, PAN_ID_TLV, ACTIVE_TIMESTAMP_TLV} == set(
                p.mle.tlv.type) and p.wpan.dst_pan == 0xffff and p.wpan.aux_sec.key_id_mode == 0x2)

        # Step 8: ED MUST attempt to attach on the Secondary channel,
        # with the new PAN ID it received in the MLE Announce message from Leader_2
        pkts.range(_epkts.index).filter_mle_cmd(MLE_ANNOUNCE).filter_wpan_src64(LEADER_2).filter_wpan_dst64(
            ED).must_next().must_verify(lambda p: p.mle.tlv.channel == CHANNEL2)
        _epkts.range(pkts.index).filter_mle_cmd(MLE_PARENT_REQUEST).must_next()

        # Step 9: ED MUST respond with an ICMPv6 Echo Reply
        _epkts.filter('ipv6.dst == {LEADER_2_MLEID} and ipv6.src == {ED_MLEID}',
                      **pv.vars).filter_ping_reply().must_next()


if __name__ == '__main__':
    unittest.main()
