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

import copy
import unittest

from mle import TlvType
from pktverify import consts
from pktverify.packet_verifier import PacketVerifier

import thread_cert

LEADER = 1
ROUTER_1 = 2
REED_1 = 3
DUT = 4


class LowPower_6_1_07_PreferringARouterOverAReed_Base(thread_cert.TestCase):

    TOPOLOGY = {
        LEADER: {
            'version': '1.2',
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER_1, REED_1]
        },
        ROUTER_1: {
            'version': '1.1',
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER, DUT],
        },
        REED_1: {
            'version': '1.2',
            'name': 'REED_1',
            'mode': 'rdn',
            'router_upgrade_threshold': 0,
            'allowlist': [LEADER, DUT],
        },
        DUT: {
            'version': '1.2',
            'name': 'DUT',
            'is_mtd': True,
            'allowlist': [ROUTER_1, REED_1],
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER_1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER_1].get_state(), 'router')

        self.nodes[REED_1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[REED_1].get_state(), 'child')

        self.simulator.go(5)  # wait for advertisements

        # Step 2 - set outbound link quality of ROUTER_1 and REED_1 to 2 (medium)
        self.nodes[ROUTER_1].set_outbound_link_quality(2)
        self.nodes[REED_1].set_outbound_link_quality(2)

        # Step 3 - DUT Automatically begins attach process by sending a multicast MLE Parent Request
        self.nodes[DUT].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT].get_state(), 'child')

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()
        LEADER = pv.vars['LEADER']
        ROUTER_1 = pv.vars['ROUTER_1']
        REED_1 = pv.vars['REED_1']
        DUT = pv.vars['DUT']

        # Step 1 - Ensure all routers and leader are sending MLE advertisements
        pkts.filter_wpan_src64(LEADER) \
            .filter_mle_cmd(consts.MLE_ADVERTISEMENT) \
            .must_next()
        pkts.filter_wpan_src64(ROUTER_1) \
            .filter_mle_cmd(consts.MLE_ADVERTISEMENT) \
            .must_next()

        # Step 3 - DUT begins attach process by sending a multicast MLE Parent Request
        pkts.filter_wpan_src64(DUT) \
            .filter_mle_cmd(consts.MLE_PARENT_REQUEST) \
            .filter_mle_has_tlv(TlvType.CHALLENGE, TlvType.MODE) \
            .filter(lambda p: p.mle.tlv.scan_mask.r == 1 and p.mle.tlv.scan_mask.e == 0) \
            .filter(lambda p: p.mle.tlv.version == 3) \
            .filter(lambda p: p.ipv6.hlim == 255) \
            .filter_LLARMA() \
            .must_next()

        # Step 4 - Router_1 automatically Respond with MLE Parent Response
        pkts.filter_wpan_src64(ROUTER_1) \
            .filter_wpan_dst64(DUT) \
            .filter_mle_cmd(consts.MLE_PARENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.version == 2) \
            .must_next()

        # Step 5 - DUT sends another multicast MLE Parent Request to the all-routers multicast with the Scan Mask TLV
        # set for all routers and all REEDs
        pkts.filter_wpan_src64(DUT) \
            .filter_mle_cmd(consts.MLE_PARENT_REQUEST) \
            .filter_mle_has_tlv(TlvType.CHALLENGE, TlvType.MODE) \
            .filter(lambda p: p.mle.tlv.scan_mask.r == 1 and p.mle.tlv.scan_mask.e == 1) \
            .filter(lambda p: p.mle.tlv.version == consts.THREAD_VERSION_1_2) \
            .filter(lambda p: p.ipv6.hlim == 255) \
            .filter_LLARMA() \
            .must_next()

        # Step 6 - REED_1 sends MLE Parent Response to the DUT
        pkts.filter_wpan_src64(REED_1) \
            .filter_wpan_dst64(DUT) \
            .filter_mle_cmd(consts.MLE_PARENT_RESPONSE) \
            .must_next()

        # # Step 7 - DUT sends a Child ID Request to REED_1 due to its better link quality
        pkts.filter_wpan_src64(DUT) \
            .filter_mle_cmd(consts.MLE_CHILD_ID_REQUEST) \
            .filter_wpan_dst64(ROUTER_1) \
            .filter_mle_has_tlv(TlvType.ADDRESS_REGISTRATION, TlvType.LINK_LAYER_FRAME_COUNTER, TlvType.MODE, TlvType.RESPONSE, TlvType.TIMEOUT, TlvType.TLV_REQUEST) \
            .filter(lambda p: p.mle.tlv.version == consts.THREAD_VERSION_1_2) \
            .must_next()


class LowPower_6_1_07_PreferringARouterOverAReed_MED(LowPower_6_1_07_PreferringARouterOverAReed_Base):
    TOPOLOGY = copy.deepcopy(LowPower_6_1_07_PreferringARouterOverAReed_Base.TOPOLOGY)
    TOPOLOGY[DUT]['mode'] = 'rn'


class LowPower_6_1_07_PreferringARouterOverAReed_SED(LowPower_6_1_07_PreferringARouterOverAReed_Base):
    TOPOLOGY = copy.deepcopy(LowPower_6_1_07_PreferringARouterOverAReed_Base.TOPOLOGY)
    TOPOLOGY[DUT]['mode'] = '-'


del (LowPower_6_1_07_PreferringARouterOverAReed_Base)

if __name__ == '__main__':
    unittest.main()
