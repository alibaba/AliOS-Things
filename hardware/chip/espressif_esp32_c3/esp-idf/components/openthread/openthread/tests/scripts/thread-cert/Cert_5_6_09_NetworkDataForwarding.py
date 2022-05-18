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
from pktverify.consts import MLE_ADVERTISEMENT, MLE_DATA_RESPONSE, MLE_CHILD_ID_RESPONSE, MLE_CHILD_UPDATE_REQUEST, MLE_CHILD_UPDATE_RESPONSE, SOURCE_ADDRESS_TLV, MODE_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, ADDRESS_REGISTRATION_TLV, NWD_COMMISSIONING_DATA_TLV, NWD_PREFIX_TLV, NWD_BORDER_ROUTER_TLV, NWD_6LOWPAN_ID_TLV, NWD_HAS_ROUTER_TLV, LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS
from pktverify.packet_verifier import PacketVerifier
from pktverify.addrs import Ipv6Addr

LEADER = 1
ROUTER1 = 2
ROUTER2 = 3
ED = 4
SED = 5

MTDS = [ED, SED]


class Cert_5_6_9_NetworkDataForwarding(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1, ROUTER2]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER, ED, SED]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        ED: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [ROUTER1]
        },
        SED: {
            'name': 'SED',
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER1]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(4)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'router')

        self.nodes[ED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED].get_state(), 'child')

        self.nodes[SED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED].get_state(), 'child')

        self.collect_rloc16s()
        self.collect_ipaddrs()
        self.nodes[LEADER].add_prefix('2001:2:0:1::/64', 'aros', 'med')
        self.nodes[LEADER].add_route('2001:2:0:2::/64', stable=True, prf='med')
        self.nodes[LEADER].register_netdata()

        # Set lowpan context of sniffer
        self.simulator.set_lowpan_context(1, '2001:2:0:1::/64')

        self.simulator.go(10)

        self.nodes[ROUTER2].add_prefix('2001:2:0:1::/64', 'aos', 'med')
        self.nodes[ROUTER2].add_route('2001:2:0:2::/64', stable=True, prf='high')
        self.nodes[ROUTER2].register_netdata()
        self.simulator.go(15)

        self.assertFalse(self.nodes[SED].ping('2001:2:0:2::1', timeout=10))

        self.assertFalse(self.nodes[SED].ping('2007::1', timeout=10))

        self.nodes[ROUTER2].remove_prefix('2001:2:0:1::/64')
        self.nodes[ROUTER2].add_prefix('2001:2:0:1::/64', 'paros', 'high')
        self.nodes[ROUTER2].register_netdata()
        self.simulator.go(15)

        self.assertFalse(self.nodes[SED].ping('2007::1', timeout=10))

        self.nodes[ROUTER2].remove_prefix('2001:2:0:1::/64')
        self.nodes[ROUTER2].add_prefix('2001:2:0:1::/64', 'paros', 'med')
        self.nodes[ROUTER2].register_netdata()
        self.simulator.go(15)

        self.assertFalse(self.nodes[SED].ping('2007::1', timeout=10))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        ROUTER_1 = pv.vars['ROUTER_1']
        MED = pv.vars['MED']
        SED = pv.vars['SED']
        _rpkts = pkts.filter_wpan_src64(ROUTER_1)

        # Step 1: Ensure the topology is formed correctly
        _rpkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).filter_wpan_dst64(SED).must_next()

        # Step 4: The DUT MUST send a multicast MLE Data Response with
        # the new network information
        _rpkts.filter_mle_cmd(MLE_DATA_RESPONSE).filter_ipv6_dst(LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS).must_next(
        ).must_verify(lambda p: {Ipv6Addr('2001:2:0:1::'), Ipv6Addr('2001:2:0:2::')} == set(
            p.thread_nwd.tlv.prefix) and p.thread_nwd.tlv.border_router.flag.p == [0] and p.thread_nwd.tlv.
                      border_router.flag.s == [1] and p.thread_nwd.tlv.border_router.flag.r == [1] and p.thread_nwd.tlv
                      .border_router.flag.o == [1] and p.thread_nwd.tlv.stable == [0, 1, 1, 1, 1, 1])

        # Step 5: The DUT MUST send a unicast MLE Child Update
        # Request to SED_1
        _rpkts.filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).filter_wpan_dst64(SED).must_next(
        ).must_verify(lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV} == set(
            p.mle.tlv.type
        ) and {NWD_PREFIX_TLV, NWD_BORDER_ROUTER_TLV, NWD_6LOWPAN_ID_TLV, NWD_PREFIX_TLV, NWD_HAS_ROUTER_TLV} == set(
            p.thread_nwd.tlv.type) and {Ipv6Addr('2001:2:0:1::'), Ipv6Addr('2001:2:0:2::')} == set(
                p.thread_nwd.tlv.prefix) and {0xFFFE, 0xFFFE} == set(p.thread_nwd.tlv.border_router_16))

        # Step 6: The DUT MUST forward the SED_1 ICMPv6 Echo Request to Router_2
        # due to higher preference
        router1_rloc16 = pv.vars['ROUTER_1_RLOC16']
        leader_rloc16 = pv.vars['LEADER_RLOC16']
        _rpkts.filter_ping_request().filter_ipv6_dst('2001:2:0:2::1').must_next().must_verify(
            lambda p: p.wpan.dst16 == leader_rloc16 and p.wpan.src16 == router1_rloc16)

        # Step 7: The DUT MUST forward the MED_1 ICMPv6 Echo Request to the
        # Leader due to default route
        _rpkts.filter_ping_request().filter_ipv6_dst('2007::1').must_next().must_verify(
            lambda p: p.wpan.dst16 == leader_rloc16 and p.wpan.src16 == router1_rloc16)

        # Step 9: The DUT MUST send a multicast MLE Data Response with
        # the new network information
        _rpkts.filter_mle_cmd(MLE_DATA_RESPONSE).filter_ipv6_dst(
            LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS).must_next().must_verify(
                lambda p: {
                    NWD_COMMISSIONING_DATA_TLV, NWD_PREFIX_TLV, NWD_BORDER_ROUTER_TLV, NWD_6LOWPAN_ID_TLV,
                    NWD_PREFIX_TLV, NWD_HAS_ROUTER_TLV
                } == set(p.thread_nwd.tlv.type) and {
                    Ipv6Addr('2001:2:0:1::'), Ipv6Addr('2001:2:0:2::')
                } == set(p.thread_nwd.tlv.prefix) and p.thread_nwd.tlv.border_router.flag.p == [0, 1] and p.thread_nwd.
                tlv.border_router.flag.s == [1, 1] and p.thread_nwd.tlv.border_router.flag.r == [1, 1] and p.thread_nwd
                .tlv.border_router.flag.o == [1, 1] and p.thread_nwd.tlv.stable == [0, 1, 1, 1, 1, 1])

        # Step 10: The DUT MUST send a unicast MLE Child Update Request to SED_1
        _rpkts.filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).filter_wpan_dst64(SED).must_next(
        ).must_verify(lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV} == set(
            p.mle.tlv.type
        ) and {NWD_PREFIX_TLV, NWD_BORDER_ROUTER_TLV, NWD_6LOWPAN_ID_TLV, NWD_PREFIX_TLV, NWD_HAS_ROUTER_TLV} == set(
            p.thread_nwd.tlv.type) and {Ipv6Addr('2001:2:0:1::'), Ipv6Addr('2001:2:0:2::')} == set(
                p.thread_nwd.tlv.prefix) and {0xFFFE, 0xFFFE} == set(p.thread_nwd.tlv.border_router_16))

        # Step 11: The DUT MUST forward the SED_1 ICMPv6 Echo Request to Router_2
        # due to higher preference
        _rpkts.filter_ping_request().filter_ipv6_dst('2007::1').must_next().must_verify(
            lambda p: p.wpan.dst16 == leader_rloc16 and p.wpan.src16 == router1_rloc16)

        # Step 13: The DUT MUST send a multicast MLE Data Response with
        # the new network information
        _rpkts.filter_mle_cmd(MLE_DATA_RESPONSE).filter_ipv6_dst(
            LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS).must_next().must_verify(
                lambda p: {
                    NWD_COMMISSIONING_DATA_TLV, NWD_PREFIX_TLV, NWD_BORDER_ROUTER_TLV, NWD_6LOWPAN_ID_TLV,
                    NWD_PREFIX_TLV, NWD_HAS_ROUTER_TLV
                } == set(p.thread_nwd.tlv.type) and {Ipv6Addr('2001:2:0:1::'),
                                                     Ipv6Addr('2001:2:0:2::')} == set(p.thread_nwd.tlv.prefix))

        # Step 14: The DUT MUST send a unicast MLE Child Update Request to SED_1
        _rpkts.filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).filter_wpan_dst64(SED).must_next(
        ).must_verify(lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV} == set(
            p.mle.tlv.type
        ) and {NWD_PREFIX_TLV, NWD_BORDER_ROUTER_TLV, NWD_6LOWPAN_ID_TLV, NWD_PREFIX_TLV, NWD_HAS_ROUTER_TLV} == set(
            p.thread_nwd.tlv.type) and {Ipv6Addr('2001:2:0:1::'), Ipv6Addr('2001:2:0:2::')} == set(
                p.thread_nwd.tlv.prefix) and {0xFFFE, 0xFFFE} == set(p.thread_nwd.tlv.border_router_16))

        # Step 15: The DUT MUST forward the SED_1 ICMPv6 Echo Request to Router_2
        # due to higher preference
        _rpkts.filter_ping_request().filter_ipv6_dst('2007::1').must_next().must_verify(
            lambda p: p.wpan.dst16 == leader_rloc16 and p.wpan.src16 == router1_rloc16)


if __name__ == '__main__':
    unittest.main()
