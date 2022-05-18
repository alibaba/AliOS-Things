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
from pktverify.consts import MLE_ADVERTISEMENT, MLE_LINK_REQUEST, MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_UPDATE_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MODE_TLV, TIMEOUT_TLV, VERSION_TLV, TLV_REQUEST_TLV, ADDRESS16_TLV, NETWORK_DATA_TLV, ROUTE64_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ADDRESS_REGISTRATION_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.addrs import Ipv6Addr

LEADER = 1
ROUTER = 2
ED1 = 3
SED1 = 4

MTDS = [ED1, SED1]


class Cert_5_6_1_NetworkDataLeaderAsBr(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER, ED1, SED1]
        },
        ED1: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [ROUTER]
        },
        SED1: {
            'name': 'SED',
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(4)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[LEADER].add_prefix('2001:2:0:1::/64', 'paros')
        self.nodes[LEADER].add_prefix('2001:2:0:2::/64', 'paro')
        self.nodes[LEADER].register_netdata()

        # Set lowpan context of sniffer
        self.simulator.set_lowpan_context(1, '2001:2:0:1::/64')
        self.simulator.set_lowpan_context(2, '2001:2:0:2::/64')

        self.simulator.go(5)

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[ED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED1].get_state(), 'child')

        self.nodes[SED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED1].get_state(), 'child')

        self.collect_rloc16s()
        addrs = self.nodes[ED1].get_addrs()
        self.assertTrue(any('2001:2:0:1' in addr[0:10] for addr in addrs))
        self.assertTrue(any('2001:2:0:2' in addr[0:10] for addr in addrs))
        for addr in addrs:
            if addr[0:10] == '2001:2:0:1' or addr[0:10] == '2001:2:0:2':
                self.assertTrue(self.nodes[LEADER].ping(addr))

        addrs = self.nodes[SED1].get_addrs()
        self.assertTrue(any('2001:2:0:1' in addr[0:10] for addr in addrs))
        self.assertFalse(any('2001:2:0:2' in addr[0:10] for addr in addrs))
        for addr in addrs:
            if addr[0:10] == '2001:2:0:1' or addr[0:10] == '2001:2:0:2':
                self.assertTrue(self.nodes[LEADER].ping(addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        ROUTER = pv.vars['ROUTER']
        MED = pv.vars['MED']
        SED = pv.vars['SED']
        _rpkts = pkts.filter_wpan_src64(ROUTER)
        _mpkts = pkts.filter_wpan_src64(MED)
        _spkts = pkts.filter_wpan_src64(SED)

        # Step 3: The DUT MUST request the Network Data TLV during the
        # attaching procedure when sending MLE Child ID Request frame
        _rpkts.filter_mle_cmd(MLE_CHILD_ID_REQUEST).must_next().must_verify(
            lambda p: {
                RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MODE_TLV, TIMEOUT_TLV, VERSION_TLV, TLV_REQUEST_TLV,
                ADDRESS16_TLV, NETWORK_DATA_TLV, ROUTE64_TLV
            } < set(p.mle.tlv.type))
        _rpkts_med = _rpkts.copy()
        _rpkts_sed = _rpkts.copy()

        # Step 6: The DUT MUST send an MLE Child ID Response to SED_1,
        # containing only stable Network Data
        _rpkts_sed.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).filter_wpan_dst64(SED).must_next().must_verify(
            lambda p: {MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV} == set(p.thread_nwd.tlv.type) and
            {Ipv6Addr('2001:2:0:1::')} == set(p.thread_nwd.tlv.prefix) and p.thread_nwd.tlv.border_router.flag.p ==
            [1] and p.thread_nwd.tlv.border_router.flag.s == [1] and p.thread_nwd.tlv.border_router.flag.r == [1] and p
            .thread_nwd.tlv.border_router.flag.o == [1] and p.thread_nwd.tlv.stable == [1, 1, 1])

        # Step 8: The DUT MUST send a MLE Child ID Response to MED_1,
        # containing the full Network Data
        _rpkts_med.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).filter_wpan_dst64(MED).must_next().must_verify(
            lambda p: {Ipv6Addr('2001:2:0:1::'), Ipv6Addr('2001:2:0:2::')} == set(
                p.thread_nwd.tlv.prefix) and p.thread_nwd.tlv.border_router.flag.p == [1, 1] and p.thread_nwd.tlv.
            border_router.flag.s == [1, 1] and p.thread_nwd.tlv.border_router.flag.r == [1, 1] and p.thread_nwd.tlv.
            border_router.flag.o == [1, 1] and p.thread_nwd.tlv.stable == [0, 1, 1, 1, 0, 0, 0])

        # Step 10: The DUT MUST send a unicast MLE Child Update
        # Response to each of MED_1 and SED_1
        _rpkts_med.filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).must_next().must_verify(
            lambda p: p.wpan.dst64 == MED and
            {SOURCE_ADDRESS_TLV, MODE_TLV, LEADER_DATA_TLV, ADDRESS_REGISTRATION_TLV} < set(p.mle.tlv.type))
        _rpkts_sed.filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).must_next().must_verify(
            lambda p: p.wpan.dst64 == SED and
            {SOURCE_ADDRESS_TLV, MODE_TLV, LEADER_DATA_TLV, ADDRESS_REGISTRATION_TLV} < set(p.mle.tlv.type))

        # Step 11: MED_1 and SED_1 MUST respond to each ICMPv6 Echo Request
        # with an ICMPv6 Echo Reply
        med_rloc16 = pv.vars['MED_RLOC16']
        sed_rloc16 = pv.vars['SED_RLOC16']
        router_rloc16 = pv.vars['ROUTER_RLOC16']
        _mpkts.range(_rpkts_med.index).filter(
            lambda p: p.wpan.src16 == med_rloc16 and p.wpan.dst16 == router_rloc16).filter_ping_reply().must_next()
        _spkts.range(_rpkts_sed.index).filter(
            lambda p: p.wpan.src16 == sed_rloc16 and p.wpan.dst16 == router_rloc16).filter_ping_reply().must_next()


if __name__ == '__main__':
    unittest.main()
