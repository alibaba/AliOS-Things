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
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS'
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
# This test verifies PBBR's ND Proxy feature:
#   1. Sends ICMPv6 Neighbor Advertisement (NA) for ND Proxy
#   2. Receive and handle ICMPv6 Neighbor Solicitation (NS) and sends NA accordingly.
#
import ipaddress
import unittest

import config
import thread_cert
from pktverify.packet_verifier import PacketVerifier
from pktverify.consts import DUA_RECENT_TIME

PBBR = 1
SBBR = 2
ROUTER1 = 3
HOST = 4

REREG_DELAY = 5  # Seconds
MLR_TIMEOUT = 300  # Seconds
WAIT_REDUNDANCE = 3


class TestNdProxy(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    # Topology:
    #    ---- -(eth)------------
    #       |        |     |
    #      PBBR----SBBR   HOST
    #        \     /
    #        Router1
    #
    TOPOLOGY = {
        PBBR: {
            'name': 'PBBR',
            'allowlist': [SBBR, ROUTER1],
            'is_otbr': True,
            'version': '1.2',
        },
        SBBR: {
            'name': 'SBBR',
            'allowlist': [PBBR, ROUTER1],
            'is_otbr': True,
            'version': '1.2',
        },
        ROUTER1: {
            'name': 'ROUTER1',
            'allowlist': [PBBR, SBBR],
            'version': '1.2',
        },
        HOST: {
            'name': 'HOST',
            'is_host': True
        },
    }

    def test(self):
        # Bring up HOST
        self.nodes[HOST].start()

        # Bring up PBBR
        self.nodes[PBBR].start()
        self.simulator.go(5)
        self.assertEqual('leader', self.nodes[PBBR].get_state())
        self.wait_node_state(PBBR, 'leader', 5)

        self.nodes[PBBR].set_backbone_router(reg_delay=REREG_DELAY, mlr_timeout=MLR_TIMEOUT)
        self.nodes[PBBR].enable_backbone_router()
        self.nodes[PBBR].set_domain_prefix(config.DOMAIN_PREFIX, 'prosD')
        self.simulator.go(5)
        self.assertTrue(self.nodes[PBBR].is_primary_backbone_router)
        self.assertIsNotNone(self.nodes[PBBR].get_ip6_address(config.ADDRESS_TYPE.DUA))

        # Bring up SBBR
        self.nodes[SBBR].start()
        self.simulator.go(5)
        self.assertEqual('router', self.nodes[SBBR].get_state())

        self.nodes[SBBR].set_backbone_router(reg_delay=REREG_DELAY, mlr_timeout=MLR_TIMEOUT)
        self.nodes[SBBR].enable_backbone_router()
        self.simulator.go(5)
        self.assertFalse(self.nodes[SBBR].is_primary_backbone_router)
        self.assertIsNotNone(self.nodes[SBBR].get_ip6_address(config.ADDRESS_TYPE.DUA))

        # Bring up ROUTER1
        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual('router', self.nodes[ROUTER1].get_state())
        self.assertIsNotNone(self.nodes[ROUTER1].get_ip6_address(config.ADDRESS_TYPE.DUA))

        self.collect_ipaddrs()
        self.collect_rloc16s()

        # Wait for DUA_RECENT_TIME
        self.simulator.go(DUA_RECENT_TIME + WAIT_REDUNDANCE)

        self._test_neighbor_solicitation(PBBR)
        self._test_neighbor_solicitation(SBBR)
        self._test_neighbor_solicitation(ROUTER1)

    def _test_neighbor_solicitation(self, nodeid):
        dua = self.nodes[nodeid].get_ip6_address(config.ADDRESS_TYPE.DUA)
        print('Node DUA: ', dua)
        self.nodes[HOST].ip_neighbors_flush()
        self.nodes[HOST].ping(dua, backbone=True)
        self.simulator.go(WAIT_REDUNDANCE)

    def verify(self, pv: PacketVerifier):
        pkts = pv.pkts
        pv.add_common_vars()
        pv.summary.show()

        PBBR_DUA = pv.vars['PBBR_DUA']
        SBBR_DUA = pv.vars['SBBR_DUA']
        ROUTER1_DUA = pv.vars['ROUTER1_DUA']
        PBBR_ETH = pv.vars['PBBR_ETH']
        SBBR_ETH = pv.vars['SBBR_ETH']
        HOST_ETH = pv.vars['HOST_ETH']
        HOST_BGUA = pv.vars['HOST_BGUA']

        # SBBR must not send NA for any DUA
        pkts.filter_eth_src(SBBR_ETH).filter_LLANMA().filter_icmpv6_nd_na(PBBR_DUA).must_not_next()
        pkts.filter_eth_src(SBBR_ETH).filter_LLANMA().filter_icmpv6_nd_na(SBBR_DUA).must_not_next()
        pkts.filter_eth_src(SBBR_ETH).filter_LLANMA().filter_icmpv6_nd_na(ROUTER1_DUA).must_not_next()

        # PBBR must send unsolicited NA for PBBR's DUA
        pkts.filter_eth_src(PBBR_ETH).filter_LLANMA().filter_icmpv6_nd_na(PBBR_DUA).filter(
            'icmpv6.nd.na.flag.s == 0').must_next().must_verify("""
            icmpv6.opt.linkaddr == {PBBR_ETH}
            and icmpv6.nd.na.flag.r == 1
            and icmpv6.nd.na.flag.o == 1
            and icmpv6.nd.na.flag.rsv == 0
        """,
                                                                PBBR_ETH=PBBR_ETH)

        # PBBR must send unsolicited NA for SBBR's DUA
        pkts.filter_eth_src(PBBR_ETH).filter_LLANMA().filter_icmpv6_nd_na(SBBR_DUA).filter(
            'icmpv6.nd.na.flag.s == 0').must_next().must_verify("""
            icmpv6.opt.linkaddr == {PBBR_ETH}
            and icmpv6.nd.na.flag.r == 1
            and icmpv6.nd.na.flag.o == 1
            and icmpv6.nd.na.flag.rsv == 0
        """,
                                                                PBBR_ETH=PBBR_ETH)

        # PBBR must send unsolicited NA for ROUTER1's DUA
        pkts.filter_eth_src(PBBR_ETH).filter_LLANMA().filter_icmpv6_nd_na(ROUTER1_DUA).filter(
            'icmpv6.nd.na.flag.s == 0').must_next().must_verify("""
            icmpv6.opt.linkaddr == {PBBR_ETH}
            and icmpv6.nd.na.flag.r == 1
            and icmpv6.nd.na.flag.o == 1
            and icmpv6.nd.na.flag.rsv == 0
        """,
                                                                PBBR_ETH=PBBR_ETH)

        # HOST should send NS for PBBR's DUA
        pkts.filter_eth_src(HOST_ETH).filter_icmpv6_nd_ns(PBBR_DUA).must_next()
        # PBBR must send solicited NA for PBBR's DUA
        pkts.filter_eth_src(PBBR_ETH).filter_ipv6_dst(HOST_BGUA).filter_icmpv6_nd_na(PBBR_DUA).filter(
            'icmpv6.nd.na.flag.s == 1').must_next().must_verify("""
            icmpv6.opt.linkaddr == {PBBR_ETH}
            and icmpv6.nd.na.flag.r == 1
            and icmpv6.nd.na.flag.o == 0
            and icmpv6.nd.na.flag.rsv == 0
        """,
                                                                PBBR_ETH=PBBR_ETH)

        # Host should send NS for SBBR's DUA
        pkts.filter_eth_src(HOST_ETH).filter_icmpv6_nd_ns(SBBR_DUA).must_next()
        # PBBR must send solicited NA for SBBR's DUA
        pkts.filter_eth_src(PBBR_ETH).filter_ipv6_dst(HOST_BGUA).filter_icmpv6_nd_na(SBBR_DUA).filter(
            'icmpv6.nd.na.flag.s == 1').must_next().must_verify("""
            icmpv6.opt.linkaddr == {PBBR_ETH}
            and icmpv6.nd.na.flag.r == 1
            and icmpv6.nd.na.flag.o == 0
            and icmpv6.nd.na.flag.rsv == 0
        """,
                                                                PBBR_ETH=PBBR_ETH)

        # Host should send NS for ROUTER1's DUA
        pkts.filter_eth_src(HOST_ETH).filter_icmpv6_nd_ns(ROUTER1_DUA).must_next()
        # PBBR must send solicited NA for ROUTER1's DUA
        pkts.filter_eth_src(PBBR_ETH).filter_ipv6_dst(HOST_BGUA).filter_icmpv6_nd_na(ROUTER1_DUA).filter(
            'icmpv6.nd.na.flag.s == 1').must_next().must_verify("""
            icmpv6.opt.linkaddr == {PBBR_ETH}
            and icmpv6.nd.na.flag.r == 1
            and icmpv6.nd.na.flag.o == 0
            and icmpv6.nd.na.flag.rsv == 0
        """,
                                                                PBBR_ETH=PBBR_ETH)


if __name__ == '__main__':
    unittest.main()
