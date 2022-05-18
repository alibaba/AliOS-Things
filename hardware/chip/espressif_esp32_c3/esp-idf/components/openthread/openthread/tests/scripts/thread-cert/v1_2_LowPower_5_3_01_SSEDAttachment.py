#!/usr/bin/env python3
#
#  Copyright (c) 2020, The OpenThread Authors.
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

from config import ADDRESS_TYPE
from mac802154 import MacHeader
from pktverify import consts
from pktverify.packet_verifier import PacketVerifier

import thread_cert

LEADER = 1
ROUTER = 2
SSED_1 = 3


class LowPower_5_3_01_SSEDAttachment(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'version': '1.2',
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER, SSED_1]
        },
        ROUTER: {
            'version': '1.2',
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER],
        },
        SSED_1: {
            'version': '1.2',
            'name': 'SSED_1',
            'is_mtd': True,
            'mode': '-',
            'allowlist': [LEADER],
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[SSED_1].set_csl_period(consts.CSL_DEFAULT_PERIOD)
        self.nodes[SSED_1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SSED_1].get_state(), 'child')

        self.simulator.go(5)

        self.collect_rloc16s()
        self.collect_rlocs()

        # Verify connectivity by sending an ICMP Echo Request from the Router to SSED_1 mesh-local address via the DUT
        self.assertTrue(self.nodes[ROUTER].ping(self.nodes[SSED_1].get_ip6_address(ADDRESS_TYPE.RLOC)))

        # Verify fragmented CSL transmission
        self.assertTrue(self.nodes[ROUTER].ping(self.nodes[SSED_1].get_ip6_address(ADDRESS_TYPE.RLOC), size=128))

        self.simulator.go(5)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()
        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']
        SSED_1 = pv.vars['SSED_1']

        # Step 1: ensure the Leader is sending MLE Advertisements
        pkts.filter_wpan_src64(LEADER) \
            .filter_mle_cmd(consts.MLE_ADVERTISEMENT) \
            .must_next()

        # Step 2: Verify that the frame version is IEEE Std 802.15.4-2015 of the MLE Child Update Response from the
        # DUT to SSED_1
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(SSED_1) \
            .filter_mle_cmd(consts.MLE_CHILD_UPDATE_RESPONSE) \
            .filter_wpan_version(consts.MAC_FRAME_VERSION_2015) \
            .must_next()

        # Step 3:
        # Verify that SSED_1 does not send a MAC Data Request prior to receiving the ICMP Echo Request from the Leader.
        # Verify that the frame version is IEEE Std 802.15.4-2015 of the ICMP Echo Request sent to SSED_1 from the DUT
        # Verify that the Router successfully receives an Echo Response from SSED_1
        ssed_rloc16 = pv.vars['SSED_1_RLOC16']
        ssed_rloc = pv.vars['SSED_1_RLOC']
        leader_rloc16 = pv.vars['LEADER_RLOC16']
        router_rloc16 = pv.vars['ROUTER_RLOC16']
        router_rloc = pv.vars['ROUTER_RLOC']
        pkts.filter_wpan_src64(LEADER) \
            .filter('wpan.dst16 == {rloc16}', rloc16 = ssed_rloc16) \
            .filter_ipv6_dst(ssed_rloc) \
            .filter_ping_request() \
            .filter_wpan_version(consts.MAC_FRAME_VERSION_2015) \
            .must_next()
        idx1 = pkts.index
        pkts.filter_wpan_src64(SSED_1) \
            .filter('wpan.dst16 == {rloc16}', rloc16 = leader_rloc16) \
            .filter_ipv6_dst(router_rloc) \
            .filter_ping_reply() \
            .filter_wpan_version(consts.MAC_FRAME_VERSION_2015) \
            .must_next() \
            .must_verify(lambda p: (p.wpan.ie_present == 1) and (consts.CSL_IE_ID in p.wpan.header_ie.id))
        idx2 = pkts.index
        pkts.range(idx1, idx2).filter(lambda p: p.wpan.cmd == MacHeader.CommandIdentifier.DATA_REQUEST) \
            .must_not_next()


if __name__ == '__main__':
    unittest.main()
