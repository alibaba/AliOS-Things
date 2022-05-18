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
# This test verifies that the MLR timeout configured by Commissioner works for multicast routing.
#
import unittest

from pktverify.consts import NM_COMMISSIONER_SESSION_ID_TLV
from pktverify.packet_verifier import PacketVerifier
from test_mlr_multicast_routing_timeout import ROUTER, HOST
from test_mlr_multicast_routing_timeout import TestMlrTimeout, MLR_TIMEOUT, WAIT_REDUNDANCE, MA1

COMMISSIONER_MLR_TIMEOUT = 360

assert COMMISSIONER_MLR_TIMEOUT > MLR_TIMEOUT + WAIT_REDUNDANCE * 3, "COMMISSIONER_MLR_TIMEOUT should be significantly larger than MLR_TIMEOUT"


class TestMlrCommissionerTimeout(TestMlrTimeout):

    # Topology (same as TestMlrTimeout):
    #   --------(eth)---------
    #       |            |
    #     PBBR(Leader)  HOST
    #      |
    #    ROUTER
    #

    def test(self):
        self._bootstrap()

        # Ping 1: PBBR should not forward to Thread network
        self.assertFalse(self.nodes[HOST].ping(MA1, backbone=True, ttl=10))

        # Router starts Commissioner
        self.nodes[ROUTER].commissioner_start()
        self.simulator.go(10)
        self.assertEqual('active', self.nodes[ROUTER].commissioner_state())

        # Router (Commissioner) registers MA1
        self.nodes[ROUTER].register_multicast_listener(MA1, timeout=COMMISSIONER_MLR_TIMEOUT)
        self.simulator.go(WAIT_REDUNDANCE)

        # Ping 2: PBBR should forward to Thread network
        self.assertFalse(self.nodes[HOST].ping(MA1, backbone=True, ttl=10))

        self.simulator.go(MLR_TIMEOUT + WAIT_REDUNDANCE)

        # Ping 3: PBBR should forward to Thread network
        self.assertFalse(self.nodes[HOST].ping(MA1, backbone=True, ttl=10))

        self.simulator.go(COMMISSIONER_MLR_TIMEOUT - MLR_TIMEOUT)

        # Ping 4: PBBR should NOT forward to Thread network
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

        # Verify that Router sends MLR.req for MA1 with Commissioner timeout
        pkts.filter_wpan_src64(ROUTER).filter_coap_request('/n/mr', port=MM).must_next().must_verify(
            """
                thread_meshcop.tlv.ipv6_addr == {ipv6_addr}
                and thread_nm.tlv.timeout == {timeout}
                and {NM_COMMISSIONER_SESSION_ID_TLV} in thread_nm.tlv.type
            """,
            ipv6_addr=[MA1],
            timeout=COMMISSIONER_MLR_TIMEOUT,
            NM_COMMISSIONER_SESSION_ID_TLV=NM_COMMISSIONER_SESSION_ID_TLV)

        mr_index = pkts.index

        # Ping 1: Host pings Router before MLR.reg
        before_mr_pkts = pkts.range(start, mr_index, cascade=False)
        ping1 = before_mr_pkts.filter_eth_src(HOST_ETH).filter_ipv6_dst(MA1).filter_ping_request().must_next()

        # PBBR should not forward Ping 1 to Thread network
        before_mr_pkts.filter_wpan_src64(PBBR).filter_ipv6_dst(MA1).filter_ping_request(
            identifier=ping1.icmpv6.echo.identifier).must_not_next()

        # Ping 2: Host pings Router after MLR.req
        ping2 = pkts.filter_eth_src(HOST_ETH).filter_ipv6_dst(MA1).filter_ping_request().must_next()
        # PBBR should forward this ping request to Thread network
        pkts.filter_wpan_src64(PBBR).filter_AMPLFMA().filter_ping_request(
            identifier=ping2.icmpv6.echo.identifier).must_next()

        # Ping 3: Host pings Router after delayed MLR Timeout
        ping3 = pkts.filter_eth_src(HOST_ETH).filter_ipv6_dst(MA1).filter_ping_request().must_next()
        # PBBR should forward this ping request to Thread network because MA doesn't expire yet
        pkts.filter_wpan_src64(PBBR).filter_AMPLFMA().filter_ping_request(
            identifier=ping3.icmpv6.echo.identifier).must_next()

        # Ping 4: Host pings Router after delayed Commissioner MLR Timeout
        ping4 = pkts.filter_eth_src(HOST_ETH).filter_ipv6_dst(MA1).filter_ping_request().must_next()
        # PBBR should NOT forward this ping request to Thread network because MLR has timeout
        pkts.filter_wpan_src64(PBBR).filter_AMPLFMA().filter_ping_request(
            identifier=ping4.icmpv6.echo.identifier).must_not_next()


del TestMlrTimeout

if __name__ == '__main__':
    unittest.main()
