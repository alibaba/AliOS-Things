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
from pktverify.consts import MLE_ADVERTISEMENT, MLE_PARENT_REQUEST, MLE_CHILD_ID_RESPONSE, MLE_CHILD_ID_REQUEST, MGMT_ACTIVE_SET_URI, MGMT_ACTIVE_GET_URI, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MODE_TLV, TIMEOUT_TLV, VERSION_TLV, TLV_REQUEST_TLV, CHALLENGE_TLV, SCAN_MASK_TLV, ADDRESS_REGISTRATION_TLV
from pktverify.packet_verifier import PacketVerifier

CHANNEL_INIT = 19
PANID_INIT = 0xface

NETWORK_NAME_FINAL = 'threadCert'
PANID_FINAL = 0xabcd

COMMISSIONER = 1
LEADER = 2
ROUTER1 = 3
ROUTER2 = 4


class Cert_9_2_16_ActivePendingPartition(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        COMMISSIONER: {
            'name': 'COMMISSIONER',
            'active_dataset': {
                'timestamp': 1,
                'panid': PANID_INIT,
                'channel': CHANNEL_INIT
            },
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        LEADER: {
            'name': 'LEADER',
            'active_dataset': {
                'timestamp': 1,
                'panid': PANID_INIT,
                'channel': CHANNEL_INIT
            },
            'mode': 'rdn',
            'partition_id': 0xffffffff,
            'allowlist': [COMMISSIONER, ROUTER1]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'active_dataset': {
                'timestamp': 1,
                'panid': PANID_INIT,
                'channel': CHANNEL_INIT
            },
            'mode': 'rdn',
            'allowlist': [LEADER, ROUTER2]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'active_dataset': {
                'timestamp': 1,
                'panid': PANID_INIT,
                'channel': CHANNEL_INIT
            },
            'mode': 'rdn',
            'allowlist': [ROUTER1]
        },
    }

    def _setUpRouter2(self):
        self.nodes[ROUTER2].add_allowlist(self.nodes[ROUTER1].get_addr64())
        self.nodes[ROUTER2].enable_allowlist()
        self.nodes[ROUTER2].set_router_selection_jitter(1)

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[COMMISSIONER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[COMMISSIONER].get_state(), 'router')
        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(3)

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[COMMISSIONER].send_mgmt_pending_set(
            pending_timestamp=10,
            active_timestamp=10,
            delay_timer=600000,
            mesh_local='fd00:0db9::',
        )
        self.simulator.go(5)

        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'router')

        self.nodes[ROUTER2].reset()
        self._setUpRouter2()
        self.simulator.go(100)

        self.nodes[COMMISSIONER].send_mgmt_pending_set(
            pending_timestamp=20,
            active_timestamp=20,
            delay_timer=200000,
            mesh_local='fd00:0db7::',
            panid=PANID_FINAL,
        )
        self.simulator.go(5)

        self.nodes[COMMISSIONER].send_mgmt_active_set(active_timestamp=15, network_name='threadCert')
        self.simulator.go(100)

        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'router')

        self.assertEqual(self.nodes[COMMISSIONER].get_network_name(), NETWORK_NAME_FINAL)
        self.assertEqual(self.nodes[LEADER].get_network_name(), NETWORK_NAME_FINAL)
        self.assertEqual(self.nodes[ROUTER1].get_network_name(), NETWORK_NAME_FINAL)
        self.assertEqual(self.nodes[ROUTER2].get_network_name(), NETWORK_NAME_FINAL)

        self.simulator.go(100)

        self.assertEqual(self.nodes[COMMISSIONER].get_panid(), PANID_FINAL)
        self.assertEqual(self.nodes[LEADER].get_panid(), PANID_FINAL)
        self.assertEqual(self.nodes[ROUTER1].get_panid(), PANID_FINAL)
        self.assertEqual(self.nodes[ROUTER2].get_panid(), PANID_FINAL)

        ipaddrs = self.nodes[ROUTER2].get_addrs()
        for ipaddr in ipaddrs:
            if ipaddr[0:4] != 'fe80':
                break
        self.assertTrue(self.nodes[LEADER].ping(ipaddr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        COMMISSIONER = pv.vars['COMMISSIONER']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_2 = pv.vars['ROUTER_2']
        _router2_pkts = pkts.filter_wpan_src64(ROUTER_2)

        # Step 1: Ensure the topology is formed correctly
        # Verify Commissioner, Leader and Router_1 are sending MLE advertisements
        pkts.copy().filter_wpan_src64(LEADER).filter_mle_cmd(MLE_ADVERTISEMENT).must_next()
        pkts.filter_wpan_dst64(COMMISSIONER).filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next()
        pkts.copy().filter_wpan_src64(COMMISSIONER).filter_mle_cmd(MLE_ADVERTISEMENT).must_next()
        pkts.filter_wpan_dst64(ROUTER_1).filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next()
        pkts.copy().filter_wpan_src64(ROUTER_1).filter_mle_cmd(MLE_ADVERTISEMENT).must_next()

        # Step 5: Router_2 begins attach process by sending a multicast MLE Parent Request
        # The first MLE Parent Request sent MUST NOT be sent to all routers and REEDS
        _router2_pkts.range(pkts.index).filter_mle_cmd(MLE_PARENT_REQUEST).must_next().must_verify(
            lambda p: {MODE_TLV, CHALLENGE_TLV, SCAN_MASK_TLV, VERSION_TLV} == set(
                p.mle.tlv.type) and p.mle.tlv.scan_mask.r == 1 and p.mle.tlv.scan_mask.e == 0)

        # Step 7: Router_2  MUST send a MLE Child ID Request to Router_1
        _router2_pkts.filter_mle_cmd(MLE_CHILD_ID_REQUEST).must_next().must_verify(lambda p: {
            RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MODE_TLV, TIMEOUT_TLV, VERSION_TLV, TLV_REQUEST_TLV
        } < set(p.mle.tlv.type) and ADDRESS_REGISTRATION_TLV not in p.mle.tlv.type)

        # Step 14: Router_2 begins attach process by sending a multicast MLE Parent Request
        # The first MLE Parent Request sent MUST NOT be sent to all routers and REEDS
        _router2_pkts.filter_mle_cmd(MLE_PARENT_REQUEST).must_next().must_verify(
            lambda p: {MODE_TLV, CHALLENGE_TLV, SCAN_MASK_TLV, VERSION_TLV} == set(
                p.mle.tlv.type) and p.mle.tlv.scan_mask.r == 1 and p.mle.tlv.scan_mask.e == 0)

        # Step 16: Router_2 MUST send a MLE Child ID Request to Router_1
        _router2_pkts.filter_mle_cmd(MLE_CHILD_ID_REQUEST).must_next().must_verify(lambda p: {
            RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MODE_TLV, TIMEOUT_TLV, VERSION_TLV, TLV_REQUEST_TLV
        } < set(p.mle.tlv.type) and ADDRESS_REGISTRATION_TLV not in p.mle.tlv.type)


if __name__ == '__main__':
    unittest.main()
