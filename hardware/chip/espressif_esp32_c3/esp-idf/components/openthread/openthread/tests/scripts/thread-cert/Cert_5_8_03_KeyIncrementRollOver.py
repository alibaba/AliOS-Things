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
from pktverify.consts import MLE_ADVERTISEMENT, MLE_CHILD_ID_RESPONSE
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ROUTER = 2


class Cert_5_8_3_KeyIncrementRollOver(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'key_sequence_counter': 127,
            'key_switch_guardtime': 0,
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'name': 'ROUTER',
            'key_switch_guardtime': 0,
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(4)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.collect_ipaddrs()
        addrs = self.nodes[ROUTER].get_addrs()
        for addr in addrs:
            if addr[0:4] != 'fe80':
                self.assertTrue(self.nodes[LEADER].ping(addr))

        key_sequence_counter = self.nodes[LEADER].get_key_sequence_counter()
        self.nodes[LEADER].set_key_sequence_counter(key_sequence_counter + 1)

        addrs = self.nodes[ROUTER].get_addrs()
        for addr in addrs:
            if addr[0:4] != 'fe80':
                self.assertTrue(self.nodes[LEADER].ping(addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']
        leader_pkts = pkts.filter_wpan_src64(LEADER)
        router_pkts = pkts.filter_wpan_src64(ROUTER)

        # Step 1: The DUT must start the network using
        # thrKeySequenceCounter = 127
        _lpkts = leader_pkts.filter_mle_cmd(MLE_ADVERTISEMENT).must_next()
        self.assertTrue(_lpkts.wpan.aux_sec.key_source == 127)

        # Step 2: Verify that the topology described above is created.
        # MLE Auxiliary security header shall contain Key Source = 127,
        # KeyIndex = 128, KeyID Mode = 2
        leader_pkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next()
        _lpkts = leader_pkts.copy()
        _rpkts = router_pkts.range(leader_pkts.index)

        _rpkts.filter_mle_cmd(
            MLE_ADVERTISEMENT).must_next().must_verify(lambda p: p.wpan.aux_sec.key_index == 128 and p.wpan.aux_sec.
                                                       key_id_mode == 2 and p.wpan.aux_sec.key_source == 127)

        # Step 3: Leader send an ICMPv6 Echo Request to Router_1.
        # The MAC Auxiliary security header must contain
        # KeyIndex = 128, KeyID Mode = 1
        leader_mleid = pv.vars['LEADER_MLEID']
        router_mleid = pv.vars['ROUTER_MLEID']
        _lpkts.filter(lambda p: p.ipv6.dst == router_mleid).filter_ping_request().must_next().must_verify(
            lambda p: p.wpan.aux_sec.key_index == 128 and p.wpan.aux_sec.key_id_mode == 1)

        # Step 4: Router_1 send an ICMPv6 Echo Reply to Leader.
        # The MAC Auxiliary security header must contain
        # KeyIndex = 128, KeyID Mode = 1
        _rpkts.filter(lambda p: p.ipv6.dst == leader_mleid).filter_ping_reply().must_next().must_verify(
            lambda p: p.wpan.aux_sec.key_index == 128 and p.wpan.aux_sec.key_id_mode == 1)

        # Step 5: The DUT MUST implementation specific means increment
        # thrKeySequenceCounter by 1 to force a key switch
        # Step 6: Leader Send an ICMPv6 Echo Request to Router_1.
        # The MAC Auxiliary security header must contain
        # KeyIndex = 1, KeyID Mode = 1
        _lpkts.filter(lambda p: p.ipv6.dst == router_mleid).filter_ping_request().must_next().must_verify(
            lambda p: p.wpan.aux_sec.key_index == 1 and p.wpan.aux_sec.key_id_mode == 1)

        # Step 7: Router_1 send an ICMPv6 Echo Reply to Leader.
        # The MAC Auxiliary security header must contain
        # KeyIndex = 1, KeyID Mode = 1
        _rpkts.filter(lambda p: p.ipv6.dst == leader_mleid).filter_ping_reply().must_next().must_verify(
            lambda p: p.wpan.aux_sec.key_index == 1 and p.wpan.aux_sec.key_id_mode == 1)


if __name__ == '__main__':
    unittest.main()
