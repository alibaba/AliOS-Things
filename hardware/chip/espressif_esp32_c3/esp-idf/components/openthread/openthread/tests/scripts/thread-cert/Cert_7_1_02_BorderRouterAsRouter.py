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
from pktverify.consts import MLE_ADVERTISEMENT, MLE_CHILD_ID_REQUEST, MLE_DATA_RESPONSE, MLE_CHILD_ID_RESPONSE, MLE_CHILD_UPDATE_REQUEST, MLE_CHILD_UPDATE_RESPONSE, SVR_DATA_URI, SOURCE_ADDRESS_TLV, MODE_TLV, ADDRESS_REGISTRATION_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, ROUTE64_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.addrs import Ipv6Addr

LEADER = 1
ROUTER = 2
ED2 = 3
SED2 = 4

MTDS = [ED2, SED2]


class Cert_7_1_2_BorderRouterAsRouter(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER, ED2, SED2]
        },
        ED2: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [ROUTER]
        },
        SED2: {
            'name': 'SED',
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
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

        self.nodes[ROUTER].add_prefix('2001:2:0:1::/64', 'paros')
        self.nodes[ROUTER].add_prefix('2001:2:0:2::/64', 'paro')
        self.nodes[ROUTER].register_netdata()

        # Set lowpan context of sniffer
        self.simulator.set_lowpan_context(1, '2001:2:0:1::/64')
        self.simulator.set_lowpan_context(2, '2001:2:0:2::/64')

        self.nodes[ED2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED2].get_state(), 'child')

        self.nodes[SED2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED2].get_state(), 'child')

        self.collect_rloc16s()
        addrs = self.nodes[ED2].get_addrs()
        self.assertTrue(any('2001:2:0:1' in addr[0:10] for addr in addrs))
        self.assertTrue(any('2001:2:0:2' in addr[0:10] for addr in addrs))
        for addr in addrs:
            if addr[0:10] == '2001:2:0:1' or addr[0:10] == '2001:2:0:2':
                self.assertTrue(self.nodes[LEADER].ping(addr))

        addrs = self.nodes[SED2].get_addrs()
        self.assertTrue(any('2001:2:0:1' in addr[0:10] for addr in addrs))
        self.assertFalse(any('2001:2:0:2' in addr[0:10] for addr in addrs))
        for addr in addrs:
            if addr[0:10] == '2001:2:0:1' or addr[0:10] == '2001:2:0:2':
                self.assertTrue(self.nodes[LEADER].ping(addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']
        MED = pv.vars['MED']
        SED = pv.vars['SED']
        _rpkts = pkts.filter_wpan_src64(ROUTER)

        # Step 2: The DUT MUST send properly formatted MLE Advertisements
        # The DUT MUST send a CoAP Server Data Notification message
        # with the server’s information (Prefix, Border Router) to the Leader
        _rpkts.filter_mle_cmd(MLE_CHILD_ID_REQUEST).must_next()
        _rpkts.filter_mle_cmd(MLE_ADVERTISEMENT).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ROUTE64_TLV} == set(p.mle.tlv.type))
        _rpkts.filter_coap_request(SVR_DATA_URI).must_next().must_verify(
            lambda p: p.wpan.dst16 == pv.vars['LEADER_RLOC16'] and {
                Ipv6Addr('2001:2:0:1::'), Ipv6Addr('2001:2:0:2::')
            } == set(p.thread_nwd.tlv.prefix) and p.thread_nwd.tlv.border_router.flag.p == [1, 1] and p.thread_nwd.tlv.
            border_router.flag.s == [1, 1] and p.thread_nwd.tlv.border_router.flag.r == [1, 1] and p.thread_nwd.tlv.
            border_router.flag.o == [1, 1] and p.thread_nwd.tlv.stable == [1, 1, 0, 0])

        # Step 3: Automatically transmits a 2.04 Changed CoAP response to the DUT
        # Step 4: The DUT MUST multicast the MLE Data Response message sent by the Leader
        _rpkts.filter_mle_cmd(MLE_DATA_RESPONSE).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV} == set(p.mle.tlv.
                                                                                                           type))

        # Step 5: MED_1 to attach to DUT and request complete network data
        # Step 6: The DUT MUST send an MLE Child ID Response to MED_1
        p = _rpkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next()
        p.must_verify(lambda p: p.wpan.dst64 == MED and {Ipv6Addr('2001:2:0:1::'),
                                                         Ipv6Addr('2001:2:0:2::')} == set(p.thread_nwd.tlv.prefix))
        _rpkts_med = _rpkts.copy()

        # Step 7: SED_1 to attach to DUT and request only stable data
        # Step 8: The DUT MUST send an MLE Child Response to SED_1
        _rpkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next().must_verify(
            lambda p: p.wpan.dst64 == SED and {Ipv6Addr('2001:2:0:1::')} == set(
                p.thread_nwd.tlv.prefix) and p.thread_nwd.tlv.border_router_16 == [0xFFFE])
        _rpkts_sed = _rpkts.copy()

        # Step 9: SED_1 and MED_1 send its configured global address to the DUT
        # Step 10: The DUT MUST send a Child Update Response to MED_1 & SED_1
        _med_pkt = pkts.range(
            _rpkts_med.index).filter_wpan_src64(MED).filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).must_next()
        _sed_pkt = pkts.range(
            _rpkts_sed.index).filter_wpan_src64(SED).filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).must_next()
        _rpkts_med.filter_wpan_dst64(MED).filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, MODE_TLV, ADDRESS_REGISTRATION_TLV} <= set(p.mle.tlv.type) and p.wpan.dst64
            == MED and set(p.mle.tlv.addr_reg_iid) < set(_med_pkt.mle.tlv.addr_reg_iid))
        _rpkts_sed.filter_wpan_dst64(SED).filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, MODE_TLV, ADDRESS_REGISTRATION_TLV} <= set(p.mle.tlv.type) and set(
                p.mle.tlv.addr_reg_iid) < set(_sed_pkt.mle.tlv.addr_reg_iid))


if __name__ == '__main__':
    unittest.main()
