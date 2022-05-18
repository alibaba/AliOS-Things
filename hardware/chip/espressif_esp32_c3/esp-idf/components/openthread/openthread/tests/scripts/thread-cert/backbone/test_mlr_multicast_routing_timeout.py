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
# This test verifies that the MLR timeout works for multicast routing.
#
import unittest

import config
import thread_cert
from pktverify.packet_verifier import PacketVerifier

PBBR = 1
ROUTER = 2
HOST = 3

MA1 = 'ff05::1234:777a:1'

BBR_REGISTRATION_JITTER = 1
REG_DELAY = 10
MLR_TIMEOUT = 300
WAIT_REDUNDANCE = 3


class TestMlrTimeout(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    # Topology:
    #   --------(eth)---------
    #       |            |
    #     PBBR(Leader)  HOST
    #      |
    #    ROUTER
    #
    TOPOLOGY = {
        PBBR: {
            'name': 'PBBR',
            'allowlist': [ROUTER],
            'is_otbr': True,
            'version': '1.2',
            'bbr_registration_jitter': BBR_REGISTRATION_JITTER,
        },
        ROUTER: {
            'name': 'ROUTER',
            'allowlist': [PBBR],
            'version': '1.2',
        },
        HOST: {
            'name': 'Host',
            'is_host': True
        },
    }

    def _bootstrap(self):
        # Bring up Host
        self.nodes[HOST].start()

        # Bring up PBBR
        self.nodes[PBBR].start()
        self.simulator.go(5)
        self.assertEqual('leader', self.nodes[PBBR].get_state())
        self.nodes[PBBR].enable_backbone_router()
        self.nodes[PBBR].set_backbone_router(reg_delay=REG_DELAY, mlr_timeout=MLR_TIMEOUT)
        self.simulator.go(10)
        self.assertTrue(self.nodes[PBBR].is_primary_backbone_router)
        self.nodes[PBBR].add_prefix(config.DOMAIN_PREFIX, "parosD")
        self.nodes[PBBR].register_netdata()

        # Bring up Router
        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual('router', self.nodes[ROUTER].get_state())

        self.collect_ipaddrs()

    def test(self):
        self._bootstrap()

        self.assertFalse(self.nodes[HOST].ping(MA1, backbone=True, ttl=10))

        # Router subscribes MA
        self.nodes[ROUTER].add_ipmaddr(MA1)
        self.simulator.go(WAIT_REDUNDANCE)

        # Verify MA is reachable from Host
        self.assertTrue(self.nodes[HOST].ping(MA1, backbone=True, ttl=10))
        self.simulator.go(WAIT_REDUNDANCE)

        # Router unsubscribes MA
        self.nodes[ROUTER].del_ipmaddr(MA1)
        self.simulator.go(WAIT_REDUNDANCE)

        # Verify MA is not reachable from Host after unsubscribed
        # But PBBR should still forward the Ping Requests to Thread network
        self.assertFalse(self.nodes[HOST].ping(MA1, backbone=True, ttl=10))

        self.simulator.go(MLR_TIMEOUT + WAIT_REDUNDANCE)

        # Verify MA is not reachable from Host after MLR timeout
        # PBBR should not forward the Ping Requests to Thread network
        self.assertFalse(self.nodes[HOST].ping(MA1, backbone=True, ttl=10))

    def verify(self, pv: PacketVerifier):
        pkts = pv.pkts
        pv.add_common_vars()
        pv.summary.show()

        ROUTER = pv.vars['ROUTER']
        PBBR = pv.vars['PBBR']
        MM = pv.vars['MM_PORT']
        HOST_ETH = pv.vars['Host_ETH']
        HOST_BGUA = pv.vars['Host_BGUA']

        start = pkts.index

        # Verify that Router sends MLR.req for MA1
        pkts.filter_wpan_src64(ROUTER).filter_coap_request('/n/mr', port=MM).must_next().must_verify(
            'thread_meshcop.tlv.ipv6_addr == {ipv6_addr}', ipv6_addr=[MA1])

        mr_index = pkts.index

        # Host pings Router before MLR.reg
        before_mr_pkts = pkts.range(start, mr_index, cascade=False)
        ping = before_mr_pkts.filter_eth_src(HOST_ETH).filter_ipv6_dst(MA1).filter_ping_request().must_next()

        # PBBR should not forward this ping request to Thread network
        before_mr_pkts.filter_wpan_src64(PBBR).filter_ipv6_dst(MA1).filter_ping_request(
            identifier=ping.icmpv6.echo.identifier).must_not_next()

        # Host pings Router after MLR.req
        ping = pkts.filter_eth_src(HOST_ETH).filter_ipv6_dst(MA1).filter_ping_request().must_next()
        # PBBR should forward this ping request to Thread network
        pkts.filter_wpan_src64(PBBR).filter_AMPLFMA().filter_ping_request(
            identifier=ping.icmpv6.echo.identifier).must_next()
        # Router should reply
        pkts.filter_wpan_src64(ROUTER).filter_ipv6_dst(HOST_BGUA).filter_ping_reply(
            identifier=ping.icmpv6.echo.identifier).must_next()

        # Host pings Router after Router unsubscribed MA
        ping = pkts.filter_eth_src(HOST_ETH).filter_ipv6_dst(MA1).filter_ping_request().must_next()
        # PBBR should forward this ping request to Thread network because MA doesn't expire yet
        pkts.filter_wpan_src64(PBBR).filter_AMPLFMA().filter_ping_request(
            identifier=ping.icmpv6.echo.identifier).must_next()
        # Router should NOT reply because it has unsubcribed the multicast address
        pkts.filter_wpan_src64(ROUTER).filter_ipv6_dst(HOST_BGUA).filter_ping_reply(
            identifier=ping.icmpv6.echo.identifier).must_not_next()

        # Host pings Router after MLR timeout
        ping = pkts.filter_eth_src(HOST_ETH).filter_ipv6_dst(MA1).filter_ping_request().must_next()
        # PBBR should NOT forward this ping request to Thread network because MLR has timeout
        pkts.filter_wpan_src64(PBBR).filter_AMPLFMA().filter_ping_request(
            identifier=ping.icmpv6.echo.identifier).must_not_next()


if __name__ == '__main__':
    unittest.main()
