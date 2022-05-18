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
from pktverify.consts import MLE_CHILD_ID_RESPONSE, MLE_DATA_REQUEST, MLE_CHILD_UPDATE_REQUEST, LEADER_DATA_TLV, ADDRESS_REGISTRATION_TLV, MODE_TLV, TIMEOUT_TLV, TLV_REQUEST_TLV, NETWORK_DATA_TLV
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ED = 2


class Cert_6_3_2_NetworkDataUpdate(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ED]
        },
        ED: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'timeout': 10,
            'allowlist': [LEADER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED].get_state(), 'child')

        self.nodes[LEADER].add_prefix('2001:2:0:1::/64', 'paros')
        self.nodes[LEADER].register_netdata()

        # Set lowpan context of sniffer
        self.simulator.set_lowpan_context(1, '2001:2:0:1::/64')

        self.simulator.go(5)

        addrs = self.nodes[ED].get_addrs()
        self.assertTrue(any('2001:2:0:1' in addr[0:10] for addr in addrs))
        for addr in addrs:
            if addr[0:10] == '2001:2:0:1':
                self.assertTrue(self.nodes[LEADER].ping(addr))

        self.nodes[LEADER].remove_allowlist(self.nodes[ED].get_addr64())
        self.nodes[ED].remove_allowlist(self.nodes[LEADER].get_addr64())

        self.nodes[LEADER].add_prefix('2001:2:0:2::/64', 'paros')
        self.nodes[LEADER].register_netdata()

        # Set lowpan context of sniffer
        self.simulator.set_lowpan_context(2, '2001:2:0:2::/64')

        self.simulator.go(5)

        self.nodes[LEADER].add_allowlist(self.nodes[ED].get_addr64())
        self.nodes[ED].add_allowlist(self.nodes[LEADER].get_addr64())
        self.simulator.go(10)

        addrs = self.nodes[ED].get_addrs()
        self.assertTrue(any('2001:2:0:1' in addr[0:10] for addr in addrs))
        self.assertTrue(any('2001:2:0:2' in addr[0:10] for addr in addrs))
        for addr in addrs:
            if addr[0:10] == '2001:2:0:1' or addr[0:10] == '2001:2:0:2':
                self.assertTrue(self.nodes[LEADER].ping(addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        MED = pv.vars['MED']
        _epkts = pkts.filter_wpan_src64(MED)

        # Step 1: Ensure the topology is formed correctly
        pkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).filter_wpan_src64(LEADER).must_next()

        # Step 3: The DUT MUST send a MLE Child Update Request to the Leader
        _epkts.range(pkts.index).filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).must_next().must_verify(
            lambda p: p.wpan.dst64 == LEADER and {LEADER_DATA_TLV, ADDRESS_REGISTRATION_TLV, MODE_TLV, TIMEOUT_TLV
                                                 } < set(p.mle.tlv.type))

        # Step 10: The DUT MUST send a MLE Data Request frame to
        # request the updated Network Data
        _epkts.filter_mle_cmd(MLE_DATA_REQUEST).must_next().must_verify(
            lambda p: {TLV_REQUEST_TLV, NETWORK_DATA_TLV} < set(p.mle.tlv.type))

        # Step 12: The DUT MUST send a MLE Child Update Request to the Leader
        _epkts.filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).filter_wpan_dst64(LEADER).must_next().must_verify(
            lambda p: {ADDRESS_REGISTRATION_TLV, MODE_TLV, TIMEOUT_TLV} < set(p.mle.tlv.type))


if __name__ == '__main__':
    unittest.main()
