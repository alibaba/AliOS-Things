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
from pktverify.consts import MLE_ADVERTISEMENT, MLE_DATA_RESPONSE, MLE_CHILD_ID_RESPONSE, MLE_CHILD_UPDATE_REQUEST, SVR_DATA_URI, MLE_CHILD_UPDATE_RESPONSE, MODE_TLV, LEADER_DATA_TLV, ROUTE64_TLV, SOURCE_ADDRESS_TLV, ACTIVE_TIMESTAMP_TLV, ADDRESS16_TLV, NETWORK_DATA_TLV, ADDRESS_REGISTRATION_TLV, LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS
from pktverify.packet_verifier import PacketVerifier
from pktverify.addrs import Ipv6Addr

LEADER = 1
ROUTER = 2
ED1 = 3
SED1 = 4

MTDS = [ED1, SED1]


class Cert_5_6_5_NetworkDataRegisterAfterAttachRouter(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER, ED1, SED1]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        ED1: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [LEADER]
        },
        SED1: {
            'name': 'SED',
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
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

        self.nodes[ED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED1].get_state(), 'child')

        self.nodes[SED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED1].get_state(), 'child')

        self.nodes[ROUTER].add_prefix('2001:2:0:1::/64', 'paros')
        self.nodes[ROUTER].add_prefix('2001:2:0:2::/64', 'paro')
        self.nodes[ROUTER].add_prefix('2001:2:0:3::/64', 'paos')
        self.nodes[ROUTER].register_netdata()

        # Set lowpan context of sniffer
        self.simulator.set_lowpan_context(1, '2001:2:0:1::/64')
        self.simulator.set_lowpan_context(2, '2001:2:0:2::/64')
        self.simulator.set_lowpan_context(3, '2001:2:0:3::/64')

        self.simulator.go(10)

        addrs = self.nodes[ED1].get_addrs()
        self.assertTrue(any('2001:2:0:1' in addr[0:10] for addr in addrs))
        self.assertTrue(any('2001:2:0:2' in addr[0:10] for addr in addrs))
        self.assertTrue(any('2001:2:0:3' in addr[0:10] for addr in addrs))
        for addr in addrs:
            if addr[0:3] == '200':
                self.assertTrue(self.nodes[LEADER].ping(addr))

        addrs = self.nodes[SED1].get_addrs()
        self.assertTrue(any('2001:2:0:1' in addr[0:10] for addr in addrs))
        self.assertFalse(any('2001:2:0:2' in addr[0:10] for addr in addrs))
        self.assertTrue(any('2001:2:0:3' in addr[0:10] for addr in addrs))
        for addr in addrs:
            if addr[0:3] == '200':
                self.assertTrue(self.nodes[LEADER].ping(addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        MED = pv.vars['MED']
        SED = pv.vars['SED']
        _lpkts = pkts.filter_wpan_src64(LEADER)

        # Step 1: Ensure the topology is formed correctly
        _lpkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).filter_wpan_dst64(SED).must_next()

        # Step 3: The DUT Automatically sends a CoAP Response frame and
        # MLE Data Response message
        _lpkts.copy().filter_coap_ack(SVR_DATA_URI).must_next()

        _lpkts_med = _lpkts.copy()
        _lpkts_sed = _lpkts.copy()

        # Step 4: The DUT MUST send a multicast MLE Data Response with
        # the new network information collected from Router_1
        _lpkts.filter_mle_cmd(MLE_DATA_RESPONSE).filter_ipv6_dst(LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS).must_next(
        ).must_verify(lambda p: {Ipv6Addr('2001:2:0:1::'),
                                 Ipv6Addr('2001:2:0:2::'),
                                 Ipv6Addr('2001:2:0:3::')} == set(p.thread_nwd.tlv.prefix) and p.thread_nwd.tlv.
                      border_router.flag.p == [1, 1, 1] and p.thread_nwd.tlv.border_router.flag.s == [1, 1, 1] and p.
                      thread_nwd.tlv.border_router.flag.r == [1, 1, 0] and p.thread_nwd.tlv.border_router.flag.o ==
                      [1, 1, 1] and p.thread_nwd.tlv.stable == [0, 1, 1, 1, 0, 0, 0, 1, 1, 1])

        # Step 7: The DUT MUST send a unicast MLE Child Update
        # Response to MED_1
        _lpkts_med.filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).filter_wpan_dst64(MED).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, MODE_TLV, LEADER_DATA_TLV, ADDRESS_REGISTRATION_TLV} < set(p.mle.tlv.type))

        # Step 8: The DUT MUST send a unicast MLE Child Update
        # Request to SED_1
        _lpkts_sed.filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).filter_wpan_dst64(SED).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV} == set(
                p.mle.tlv.type) and {Ipv6Addr('2001:2:0:1::'), Ipv6Addr('2001:2:0:3::')} == set(
                    p.thread_nwd.tlv.prefix) and p.thread_nwd.tlv.border_router.flag.p == [1, 1] and p.thread_nwd.tlv.
            border_router.flag.s == [1, 1] and p.thread_nwd.tlv.border_router.flag.r == [1, 0] and p.thread_nwd.tlv.
            border_router.flag.o == [1, 1] and p.thread_nwd.tlv.stable == [1, 1, 1, 1, 1, 1])

        # Step 10: The DUT MUST send a unicast MLE Child Update
        # Response to SED_1
        _lpkts_sed.filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).filter_wpan_dst64(SED).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, MODE_TLV, LEADER_DATA_TLV, ADDRESS_REGISTRATION_TLV} < set(p.mle.tlv.type))


if __name__ == '__main__':
    unittest.main()
