#!/usr/bin/env python3
#
#  Copyright (c) 2021, The OpenThread Authors.
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

import logging
import unittest

import pktverify
from pktverify import packet_verifier
from pktverify.consts import MA1, MA1g, MA2
import config
import thread_cert

# Test description:
# The purpose of this test is to verify the functionality of ping command.
#
# Topology:
#
#
#  ROUTER_2 ----- ROUTER_1 ---- ROUTER_3
#
#

ROUTER_1 = 1
ROUTER_2 = 2
ROUTER_3 = 3


class TestPing(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        ROUTER_1: {
            'name': 'Router_1',
            'allowlist': [ROUTER_2, ROUTER_3],
        },
        ROUTER_2: {
            'name': 'Router_2',
            'allowlist': [ROUTER_1],
        },
        ROUTER_3: {
            'name': 'Router_3',
            'allowlist': [ROUTER_1],
        },
    }

    def test(self):
        router1 = self.nodes[ROUTER_1]
        router2 = self.nodes[ROUTER_2]
        router3 = self.nodes[ROUTER_3]

        router1.start()
        self.simulator.go(5)
        self.assertEqual('leader', router1.get_state())

        router2.start()
        self.simulator.go(5)
        self.assertEqual('router', router2.get_state())

        router3.start()
        self.simulator.go(5)
        self.assertEqual('router', router3.get_state())

        # 1. ROUTER_1 pings ROUTER_2.
        self.assertTrue(router1.ping(router2.get_ip6_address(config.ADDRESS_TYPE.RLOC)))

        # 2. ROUTER_1 pings ROUTER_2 multiple times.
        self.assertTrue(router1.ping(router2.get_ip6_address(config.ADDRESS_TYPE.RLOC), count=5))

        # 3. ROUTER_2 pings ROUTER_1 from the link-local address to the
        # link-local address.
        self.assertTrue(
            router2.ping(router1.get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL),
                         interface=router2.get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)))

        # 4. ROUTER_2 pings ROUTER_3 using the RLOC.
        self.assertTrue(router2.ping(router3.get_ip6_address(config.ADDRESS_TYPE.RLOC)))

        # 5. ROUTER_2 pings ROUTER_3's link-local address. The ping should fail.
        self.assertFalse(router2.ping(router3.get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)))

        # 6. ROUTER_2 pings ROUTER_3's RLOC from the link-local address. The
        # ping should fail.
        self.assertFalse(
            router2.ping(router3.get_ip6_address(config.ADDRESS_TYPE.RLOC),
                         interface=router2.get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)))

        # 7. ROUTER_2 pings ROUTER_3's RLOC from an non-existent address. The
        # ping command should be rejected by CLI.
        self.assertFalse(router2.ping(router3.get_ip6_address(config.ADDRESS_TYPE.RLOC), interface='1::1'))

        self.collect_ipaddrs()
        self.collect_rloc16s()
        self.collect_rlocs()
        self.collect_extra_vars()

    def verify(self, pv: pktverify.packet_verifier.PacketVerifier):
        pkts = pv.pkts
        vars = pv.vars
        pv.summary.show()

        logging.info(f'vars = {vars}')

        # Ensure the topology is formed correctly
        pv.verify_attached('Router_2', 'Router_1')
        pv.verify_attached('Router_3', 'Router_1')

        # 1. Router_1 pings Router_2.
        _pkt = pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_ipv6_2dsts(vars['Router_2_RLOC'], vars['Router_2_LLA']) \
            .filter_ping_request() \
            .must_next()

        pkts.filter_wpan_src64(vars['Router_2']) \
            .filter_ipv6_dst(_pkt.ipv6.src) \
            .filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier) \
            .must_next()

        # 2. Router_1 pings Router_2 multiple times.
        for i in range(5):
            _pkt = pkts.filter_wpan_src64(vars['Router_1']) \
                .filter_ipv6_2dsts(vars['Router_2_RLOC'], vars['Router_2_LLA']) \
                .filter_ping_request() \
                .must_next()
            pkts.filter_wpan_src64(vars['Router_2']) \
                .filter_ipv6_dst(_pkt.ipv6.src) \
                .filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier) \
                .must_next()

        # 3. Router_2 pings Router_1 from the link-local address to the
        # link-local address.
        _pkt = pkts.filter_wpan_src64(vars['Router_2']) \
            .filter_ipv6_src_dst(vars['Router_2_LLA'], vars['Router_1_LLA']) \
            .filter_ping_request() \
            .must_next()

        pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_ipv6_dst(_pkt.ipv6.src) \
            .filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier) \
            .must_next()

        # 4. Router_2 pings Router_3 using the RLOC.
        _pkt = pkts.filter_wpan_src64(vars['Router_2']) \
            .filter_ipv6_dst(vars['Router_3_RLOC']) \
            .filter_ping_request() \
            .must_next()

        pkts.filter_wpan_src64(vars['Router_3']) \
            .filter_ipv6_dst(_pkt.ipv6.src) \
            .filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier) \
            .must_next()

        # 5. Router_2 pings Router_3's link-local address. The ping should fail.
        _pkt = pkts.filter_wpan_src64(vars['Router_2']) \
            .filter_ipv6_dst(vars['Router_3_LLA']) \
            .filter_ping_request() \
            .must_next()

        pkts.filter_wpan_src64(vars['Router_3']) \
            .filter_ipv6_dst(_pkt.ipv6.src) \
            .filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier) \
            .must_not_next()

        # 5. Router_2 pings Router_3's RLOC from the link-local address. The
        # ping should fail.
        _pkt = pkts.filter_wpan_src64(vars['Router_2']) \
            .filter_ipv6_src_dst(vars['Router_2_LLA'], vars['Router_3_RLOC']) \
            .filter_ping_request() \
            .must_next()


if __name__ == '__main__':
    unittest.main()
