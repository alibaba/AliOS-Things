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
# This test verifies that PBBR sets DUA routes correctly.
#
import ipaddress
import unittest

import config
import thread_cert
from pktverify.packet_verifier import PacketVerifier

# Use two channels
CH1 = 11
CH2 = 22

PBBR = 1
SBBR = 2
ROUTER1 = 3
HOST = 4
PBBR2 = 5
ROUTER2 = 6

REREG_DELAY = 5  # Seconds
MLR_TIMEOUT = 300  # Seconds
WAIT_REDUNDANCE = 3


class TestNdProxy(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    # Topology:
    #    ------(eth)----------------------
    #       |        |       |      |
    #      PBBR----SBBR    HOST   PBBR2
    #        \ CH1 /               | CH2
    #        ROUTER1             ROUTER2
    #
    # PBBR2 is in the secondary channel
    #
    TOPOLOGY = {
        PBBR: {
            'name': 'PBBR',
            'allowlist': [SBBR, ROUTER1],
            'is_otbr': True,
            'version': '1.2',
            'channel': CH1,
        },
        SBBR: {
            'name': 'SBBR',
            'allowlist': [PBBR, ROUTER1],
            'is_otbr': True,
            'version': '1.2',
            'channel': CH1,
        },
        ROUTER1: {
            'name': 'ROUTER1',
            'allowlist': [PBBR, SBBR],
            'version': '1.2',
            'channel': CH1,
        },
        HOST: {
            'name': 'HOST',
            'is_host': True
        },
        PBBR2: {
            'name': 'PBBR2',
            'is_otbr': True,
            'version': '1.2',
            'channel': CH2,
        },
        ROUTER2: {
            'name': 'ROUTER2',
            'version': '1.2',
            'channel': CH2,
        },
    }

    def _bootstrap(self):
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

        # Bring up PBBR2
        self.nodes[PBBR2].start()
        self.simulator.go(5)
        self.assertEqual('leader', self.nodes[PBBR2].get_state())
        self.wait_node_state(PBBR2, 'leader', 5)

        self.nodes[PBBR2].set_backbone_router(reg_delay=REREG_DELAY, mlr_timeout=MLR_TIMEOUT)
        self.nodes[PBBR2].enable_backbone_router()
        self.nodes[PBBR2].set_domain_prefix(config.DOMAIN_PREFIX, 'prosD')
        self.simulator.go(5)
        self.assertTrue(self.nodes[PBBR2].is_primary_backbone_router)
        self.assertIsNotNone(self.nodes[PBBR2].get_ip6_address(config.ADDRESS_TYPE.DUA))

        # Bring up ROUTER2
        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual('router', self.nodes[ROUTER2].get_state())
        self.assertIsNotNone(self.nodes[ROUTER2].get_ip6_address(config.ADDRESS_TYPE.DUA))

    def test(self):
        self._bootstrap()

        self.collect_ipaddrs()
        self.collect_rloc16s()

        self.simulator.go(WAIT_REDUNDANCE)

        # Step 1: Host pings PBBR's DUA
        self.assertTrue(self.nodes[HOST].ping(self.nodes[PBBR].get_ip6_address(config.ADDRESS_TYPE.DUA),
                                              backbone=True))

        self.simulator.go(WAIT_REDUNDANCE)

        # Step 2: Host pings SBBR's DUA
        self.assertTrue(self.nodes[HOST].ping(self.nodes[SBBR].get_ip6_address(config.ADDRESS_TYPE.DUA),
                                              backbone=True))

        self.simulator.go(WAIT_REDUNDANCE)

        # Step 3: Host pings ROUTER1's DUA
        self.assertTrue(self.nodes[HOST].ping(self.nodes[ROUTER1].get_ip6_address(config.ADDRESS_TYPE.DUA),
                                              backbone=True))

        self.simulator.go(WAIT_REDUNDANCE)

        # Step 4: ROUTER2 pings ROUTER1 which should succeed
        ROUTER1_DUA = self.nodes[ROUTER1].get_ip6_address(config.ADDRESS_TYPE.DUA)
        self.assertTrue(self.nodes[ROUTER2].ping(ROUTER1_DUA))

        self.simulator.go(WAIT_REDUNDANCE)

    def _get_mliid(self, nodeid: int) -> str:
        mleid = self.nodes[nodeid].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        mliid = ipaddress.IPv6Address(mleid).packed[-8:]
        return ''.join(['%02x' % b for b in mliid])

    def verify(self, pv: PacketVerifier):
        pkts = pv.pkts
        pv.add_common_vars()
        pv.summary.show()

        PBBR = pv.vars['PBBR']
        PBBR2 = pv.vars['PBBR2']
        ROUTER1 = pv.vars['ROUTER1']
        ROUTER2 = pv.vars['ROUTER2']
        SBBR = pv.vars['SBBR']
        PBBR_DUA = pv.vars['PBBR_DUA']
        SBBR_DUA = pv.vars['SBBR_DUA']
        ROUTER1_DUA = pv.vars['ROUTER1_DUA']
        ROUTER2_DUA = pv.vars['ROUTER2_DUA']
        HOST_BGUA = pv.vars['HOST_BGUA']
        PBBR_ETH = pv.vars['PBBR_ETH']
        PBBR2_ETH = pv.vars['PBBR2_ETH']

        # Verify that SBBR should not foward any Ping Request to the Thread network.
        # Use `ipv6.hlim == 63` to avoid false fails because SBBR might still forward Ping Request from PBBR to ROUTER1
        pkts.filter_wpan_src64(SBBR).filter_ping_request().filter('ipv6.hlim == 63').must_not_next()

        # Step 1: Host pings PBBR's DUA
        # PBBR should not forward the Ping Request to Thread network
        ping1_pkts = pkts.filter_ipv6_src_dst(HOST_BGUA, PBBR_DUA).filter_ping_request()
        ping1_pkts.filter_eth().must_next()
        ping1_pkts.filter_wpan().must_not_next()

        # Step 2: Host pings SBBR's DUA
        # PBBR should forward the Ping Request to Thread network according to DUA routes
        ping2_pkts = pkts.filter_ipv6_src_dst(HOST_BGUA, SBBR_DUA).filter_ping_request()
        ping2_pkts.filter_eth().must_next()
        ping2_pkts.filter_wpan().must_next()

        # Step 3: Host pings ROUTER1's DUA
        # PBBR should forward the Ping Request to Thread network according to DUA routes
        ping3_pkts = pkts.filter_ipv6_src_dst(HOST_BGUA, ROUTER1_DUA).filter_ping_request()
        ping3_pkts.filter_eth().must_next()
        ping3_pkts.filter_wpan().must_next()

        # Step 4: ROUTER2 pings ROUTER1
        # Verify Ping Request: ROUTER2 -> PBBR2 -> PBBR -> ROUTER1
        ping4_request_pkts = pkts.filter_ipv6_src_dst(ROUTER2_DUA, ROUTER1_DUA)
        ping4_id = ping4_request_pkts.filter_wpan_src64(
            ROUTER2).filter_ping_request().must_next().icmpv6.echo.identifier
        ping4_request_pkts.filter_eth_src(PBBR2_ETH).filter_ping_request(identifier=ping4_id).must_next()
        ping4_request_pkts.filter_wpan_src64(PBBR).filter_ping_request(identifier=ping4_id).must_next()
        # Verify Ping Reply: ROUTER1 -> PBBR -> PBBR2 -> ROUTER2
        ping4_reply_pkts = pkts.filter_ipv6_src_dst(ROUTER1_DUA, ROUTER2_DUA).filter_ping_reply(identifier=ping4_id)
        ping4_reply_pkts.filter_eth_src(PBBR_ETH).must_next()


if __name__ == '__main__':
    unittest.main()
