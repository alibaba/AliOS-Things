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
# This test verifies that the basic MLR feature works.
#
import unittest

import config
import thread_cert
from pktverify.packet_verifier import PacketVerifier

CH1 = 11
CH2 = 22

PBBR1 = 1
ROUTER1 = 2
PBBR2 = 3
ROUTER2 = 4
HOST = 5

MA1 = 'ff05::1234:777a:1'
MA2 = 'ff05::1234:777a:2'

BBR_REGISTRATION_JITTER = 1
WAIT_REDUNDANCE = 3


class TestMlr(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    # Topology:
    #   --------(eth)-----------
    #       |     |      |
    #     PBBR  HOST   PBBR2
    #      |            |
    #    ROUTER1      ROUTER2
    #
    TOPOLOGY = {
        PBBR1: {
            'name': 'PBBR1',
            'allowlist': [ROUTER1],
            'is_otbr': True,
            'version': '1.2',
            'bbr_registration_jitter': BBR_REGISTRATION_JITTER,
            'channel': CH1,
        },
        ROUTER1: {
            'name': 'ROUTER1',
            'allowlist': [PBBR1],
            'version': '1.2',
            'channel': CH1,
        },
        PBBR2: {
            'name': 'PBBR2',
            'allowlist': [ROUTER2],
            'is_otbr': True,
            'version': '1.2',
            'bbr_registration_jitter': BBR_REGISTRATION_JITTER,
            'channel': CH2,
        },
        ROUTER2: {
            'name': 'ROUTER2',
            'allowlist': [PBBR2],
            'version': '1.2',
            'channel': CH2,
        },
        HOST: {
            'name': 'Host',
            'is_host': True
        },
    }

    def test(self):
        # Bring up Host
        self.nodes[HOST].start()
        self.nodes[HOST].add_ipmaddr(MA2, backbone=True)

        # Bring up PBBR1
        self.nodes[PBBR1].start()
        self.simulator.go(5)
        self.assertEqual('leader', self.nodes[PBBR1].get_state())

        self.nodes[PBBR1].enable_backbone_router()
        self.simulator.go(10)
        self.assertTrue(self.nodes[PBBR1].is_primary_backbone_router)
        self.nodes[PBBR1].add_prefix(config.DOMAIN_PREFIX, "parosD")
        self.nodes[PBBR1].register_netdata()
        self.simulator.go(WAIT_REDUNDANCE)

        # Bring up ROUTER1
        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual('router', self.nodes[ROUTER1].get_state())
        self.nodes[ROUTER1].add_ipmaddr(MA1)
        self.simulator.go(WAIT_REDUNDANCE)

        # Bring up PBBR2
        self.nodes[PBBR2].start()
        self.simulator.go(5)
        self.assertEqual('leader', self.nodes[PBBR2].get_state())

        self.nodes[PBBR2].enable_backbone_router()
        self.simulator.go(10)
        self.assertTrue(self.nodes[PBBR2].is_primary_backbone_router)

        self.nodes[PBBR2].add_prefix(config.DOMAIN_PREFIX, "parosD")
        self.nodes[PBBR2].register_netdata()
        self.simulator.go(WAIT_REDUNDANCE)

        # Bring up ROUTER2
        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual('router', self.nodes[ROUTER1].get_state())
        self.nodes[ROUTER2].add_ipmaddr(MA1)
        self.nodes[ROUTER2].add_ipmaddr(MA2)
        self.simulator.go(WAIT_REDUNDANCE)

        self.collect_ipaddrs()
        self.collect_rloc16s()

        # ping MA1 from Host could get replied from R1 and R2
        self.assertTrue(self.nodes[HOST].ping(MA1, backbone=True, ttl=5))
        self.simulator.go(WAIT_REDUNDANCE)

        # ping MA2 from R1 could get replied from Host and R2
        self.assertTrue(self.nodes[ROUTER1].ping(MA2))
        self.simulator.go(WAIT_REDUNDANCE)

    def verify(self, pv: PacketVerifier):
        pkts = pv.pkts
        pv.add_common_vars()
        pv.summary.show()

        PBBR1 = pv.vars['PBBR1']
        PBBR1_ETH = pv.vars['PBBR1_ETH']
        PBBR2 = pv.vars['PBBR2']
        PBBR2_ETH = pv.vars['PBBR2_ETH']
        ROUTER1 = pv.vars['ROUTER1']
        ROUTER2 = pv.vars['ROUTER2']
        HOST_ETH = pv.vars['Host_ETH']
        HOST_BGUA = pv.vars['Host_BGUA']

        ROUTER1_DUA = pv.vars['ROUTER1_DUA']
        ROUTER2_DUA = pv.vars['ROUTER2_DUA']

        ROUTER1_RLOC16 = pv.vars['ROUTER1_RLOC16']

        #
        # Verify Host ping MA1 to R1 and R2
        #

        # Host should send ping MA1 in the Backbone link
        ping_ma1 = pkts.filter_eth_src(HOST_ETH).filter_ipv6_src_dst(HOST_BGUA, MA1).filter_ping_request().must_next()

        with pkts.save_index():
            # PBBR1 should forward ping reply to the Backbone link
            pkts.filter_eth_src(PBBR1_ETH).filter_ipv6_src_dst(
                ROUTER1_DUA, HOST_BGUA).filter_ping_reply(identifier=ping_ma1.icmpv6.echo.identifier).must_next()

        # PBBR2 should forward ping reply to the Backbone link
        pkts.filter_eth_src(PBBR2_ETH).filter_ipv6_src_dst(
            ROUTER2_DUA, HOST_BGUA).filter_ping_reply(identifier=ping_ma1.icmpv6.echo.identifier).must_next()

        #
        # Verify R1 pings MA2 to Host and R2
        #

        # ROUTER1 should send the multicast ping request
        ping_ma2 = pkts.filter_wpan_src64(ROUTER1).filter_AMPLFMA(
            mpl_seed_id=ROUTER1_RLOC16).filter_ping_request().must_next()

        # PBBR1 should forward the multicast ping request to the Backbone link
        pkts.filter_eth_src(PBBR1_ETH).filter_ipv6_src_dst(
            ROUTER1_DUA, MA2).filter_ping_request(identifier=ping_ma2.icmpv6.echo.identifier).must_next()

        with pkts.save_index():
            # Host should send ping reply to Router1
            pkts.filter_eth_src(HOST_ETH).filter_ipv6_dst(ROUTER1_DUA).filter_ping_reply(
                identifier=ping_ma2.icmpv6.echo.identifier).must_next()

        # PBBR2 should forward the multicast ping request to Thread network at CH2
        pkts.filter_wpan_src64(PBBR2).filter_AMPLFMA().filter_ping_request(
            identifier=ping_ma2.icmpv6.echo.identifier).must_next()

        # ROUTER2 should send ping reply back to ROUTER1
        pkts.filter_wpan_src64(ROUTER2).filter_ipv6_src_dst(
            ROUTER2_DUA, ROUTER1_DUA).filter_ping_reply(identifier=ping_ma2.icmpv6.echo.identifier).must_next()


if __name__ == '__main__':
    unittest.main()
