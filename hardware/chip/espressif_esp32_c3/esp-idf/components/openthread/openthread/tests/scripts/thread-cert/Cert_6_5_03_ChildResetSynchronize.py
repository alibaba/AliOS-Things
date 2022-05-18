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
from pktverify.consts import MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, MLE_CHILD_UPDATE_REQUEST, MLE_CHILD_UPDATE_RESPONSE, MODE_TLV
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ED = 2


class Cert_6_5_3_ChildResetSynchronize(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ED]
        },
        ED: {
            'name': 'ED',
            'is_mtd': True,
            'mode': 'rn',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [LEADER]
        },
    }

    def _setUpEd(self):
        self.nodes[ED].add_allowlist(self.nodes[LEADER].get_addr64())
        self.nodes[ED].enable_allowlist()

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED].get_state(), 'child')

        self.nodes[ED].reset()
        self._setUpEd()
        self.simulator.go(1)

        self.nodes[ED].start()
        self.simulator.go(1)
        self.assertEqual(self.nodes[ED].get_state(), 'child')

        addrs = self.nodes[ED].get_addrs()
        for addr in addrs:
            if addr[0:4] == 'fe80':
                self.assertTrue(self.nodes[LEADER].ping(addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ED = pv.vars['ED']
        _leader_pkts = pkts.filter_wpan_src64(LEADER)
        _ed_pkts = pkts.filter_wpan_src64(ED)

        # Step 2: Reset the DUT for a time shorter than
        # the Child Timeout Duration.
        # Step 3: Send MLE Child Update Request to Leader
        _ed_pkts.filter_mle_cmd(MLE_CHILD_ID_REQUEST).must_next()
        _leader_pkts.range(_ed_pkts.index).filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next()
        _ed_pkts.filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).must_next().must_verify(
            lambda p: {MODE_TLV} < set(p.mle.tlv.type))

        # Step 4: Leader send an MLE Child Update Response
        _leader_pkts.range(_ed_pkts.index).filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).must_next()
        _ed_pkts.range(_leader_pkts.index).filter_mle_cmd(MLE_CHILD_ID_REQUEST).must_not_next()

        # Step 5: The DUT MUST respond with ICMPv6 Echo Reply
        _ed_pkts.filter_ping_reply().filter(lambda p: p.wpan.src64 == ED and p.wpan.dst64 == LEADER).must_next()


if __name__ == '__main__':
    unittest.main()
