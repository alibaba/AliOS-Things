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
from pktverify.consts import MLE_PARENT_REQUEST, MLE_CHILD_ID_REQUEST, MLE_CHILD_UPDATE_REQUEST, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, MODE_TLV
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ROUTER = 2
ED = 3


class Cert_6_3_1_OrphanReattach(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER, ED]
        },
        ED: {
            'name': 'ED',
            'is_mtd': True,
            'mode': 'rn',
            'timeout': 10,
            'allowlist': [ROUTER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[ED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED].get_state(), 'child')

        self.collect_ipaddrs()
        self.nodes[ROUTER].stop()
        self.nodes[LEADER].add_allowlist(self.nodes[ED].get_addr64())
        self.nodes[ED].add_allowlist(self.nodes[LEADER].get_addr64())
        self.simulator.go(20)

        self.assertEqual(self.nodes[ED].get_state(), 'child')

        addrs = self.nodes[ED].get_addrs()
        for addr in addrs:
            self.assertTrue(self.nodes[LEADER].ping(addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']
        ED = pv.vars['ED']
        _epkts = pkts.filter_wpan_src64(ED)

        _epkts.filter_mle_cmd(MLE_CHILD_ID_REQUEST).must_next()

        # Step 2: Remove Router from the network
        # Step 3: The DUT MUST send three MLE Child Update Requests to its parent
        for i in range(1, 3):
            _epkts.filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).filter_wpan_dst64(ROUTER).must_next().must_verify(
                lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, MODE_TLV} <= set(p.mle.tlv.type))

        # Step 5: The DUT MUST perform the attach procedure with the Leader
        _epkts.filter_mle_cmd(MLE_PARENT_REQUEST).must_next()
        _epkts.filter_mle_cmd(MLE_CHILD_ID_REQUEST).filter_wpan_dst64(LEADER).must_next()

        # Step 6: The DUT MUST respond with ICMPv6 Echo Reply
        _epkts.filter('ipv6.src == {ED_MLEID} and ipv6.dst == {LEADER_MLEID}',
                      **pv.vars).filter_ping_reply().must_next()


if __name__ == '__main__':
    unittest.main()
