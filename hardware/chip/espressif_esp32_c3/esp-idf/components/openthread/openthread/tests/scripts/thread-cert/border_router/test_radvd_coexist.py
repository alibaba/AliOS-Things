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

import config
import pktverify
from pktverify import packet_verifier, packet_filter, consts
import thread_cert

# Test description:
#   This test verifies that the Border Router will use the same RA parameters
#   when there is already a running radvd service on the same host.
#
# Topology:
#    ----------------(eth)--------------------
#           |                 |
#          BR (Leader)      HOST
#           |
#        ROUTER
#

BR = 1
ROUTER = 2
HOST = 3

ON_LINK_PREFIX = 'fd00::/64'


class SingleBorderRouter(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        BR: {
            'name': 'BR',
            'allowlist': [ROUTER],
            'is_otbr': True,
            'version': '1.2',
        },
        ROUTER: {
            'name': 'Router',
            'allowlist': [BR],
            'version': '1.2',
        },
        HOST: {
            'name': 'Host',
            'is_host': True
        },
    }

    def test(self):
        br = self.nodes[BR]
        router = self.nodes[ROUTER]
        host = self.nodes[HOST]

        host.start(start_radvd=False)
        self.simulator.go(5)

        br.start_radvd_service(prefix=ON_LINK_PREFIX, slaac=True)
        self.simulator.go(5)

        br.start()
        self.simulator.go(5)
        self.assertEqual('leader', br.get_state())

        router.start()
        self.simulator.go(5)
        self.assertEqual('router', router.get_state())

        self.simulator.go(10)
        self.collect_ipaddrs()

        logging.info("BR      addrs: %r", br.get_addrs())
        logging.info("ROUTER  addrs: %r", router.get_addrs())
        logging.info("HOST    addrs: %r", host.get_addrs())

        self.assertEqual(len(br.get_prefixes()), 1)
        self.assertEqual(len(router.get_prefixes()), 1)
        self.assertEqual(len(br.get_routes()), 1)
        self.assertEqual(len(router.get_routes()), 1)

        self.assertEqual(len(br.get_ip6_address(config.ADDRESS_TYPE.OMR)), 1)
        self.assertEqual(len(router.get_ip6_address(config.ADDRESS_TYPE.OMR)), 1)

        # radvd doesn't deprecates the PIO so that the Border Router will not
        # advertise its own on-link prefix immediately.
        self.assertEqual(len(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)), 1)

        self.assertTrue(router.ping(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.assertTrue(host.ping(router.get_ip6_address(config.ADDRESS_TYPE.OMR)[0], backbone=True))

        # Stop the radvd service and wait for the Border Router
        # to reset the RA parameters.
        br.stop_radvd_service()
        self.simulator.go(15)

        self.assertEqual(len(br.get_prefixes()), 1)
        self.assertEqual(len(router.get_prefixes()), 1)
        self.assertEqual(len(br.get_routes()), 1)
        self.assertEqual(len(router.get_routes()), 1)

        self.assertEqual(len(br.get_ip6_address(config.ADDRESS_TYPE.OMR)), 1)
        self.assertEqual(len(router.get_ip6_address(config.ADDRESS_TYPE.OMR)), 1)
        self.assertEqual(len(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)), 1)

        self.assertTrue(router.ping(host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.assertTrue(host.ping(router.get_ip6_address(config.ADDRESS_TYPE.OMR)[0], backbone=True))

        self.collect_ipaddrs()
        self.collect_rloc16s()
        self.collect_rlocs()
        self.collect_extra_vars()

    def verify(self, pv: pktverify.packet_verifier.PacketVerifier):
        RA_OPT_TYPE_PIO = 3
        RA_OPT_TYPE_RIO = 24

        pkts = pv.pkts
        vars = pv.vars
        pv.summary.show()
        logging.info(f'vars = {vars}')

        # Ensure the topology is formed correctly
        pv.verify_attached('Router', 'BR')

        # verify that radvd sends RA messages with PIO.
        radvd_ra = pkts.filter_eth_src(vars['BR_ETH']) \
            .filter_icmpv6_nd_ra() \
            .filter(lambda p: RA_OPT_TYPE_PIO in p.icmpv6.opt.type) \
            .must_next()

        # Verify that the BR sends RA messages with the same parameters.
        br_ra = pkts.filter_eth_src(vars['BR_ETH']) \
            .filter_icmpv6_nd_ra() \
            .filter(lambda p: RA_OPT_TYPE_RIO in p.icmpv6.opt.type) \
            .filter(lambda p: p.icmpv6.nd.ra.router_lifetime == radvd_ra.icmpv6.nd.ra.router_lifetime) \
            .filter(lambda p: p.icmpv6.nd.ra.retrans_timer == radvd_ra.icmpv6.nd.ra.retrans_timer) \
            .filter(lambda p: p.icmpv6.nd.ra.reachable_time == radvd_ra.icmpv6.nd.ra.reachable_time) \
            .must_next()

        # Verify that radvd sends at lease one RA message with zero Router Lifetime
        # when it is stopped.
        pkts.filter_eth_src(vars['BR_ETH']) \
            .filter_icmpv6_nd_ra() \
            .filter(lambda p: RA_OPT_TYPE_PIO in p.icmpv6.opt.type) \
            .filter(lambda p: p.icmpv6.nd.ra.router_lifetime == 0) \
            .must_next()

        # Verify that the BR forgets radvd's RA parameters.
        pkts.filter_eth_src(vars['BR_ETH']) \
            .filter_icmpv6_nd_ra() \
            .filter(lambda p: RA_OPT_TYPE_RIO in p.icmpv6.opt.type) \
            .filter(lambda p: p.icmpv6.nd.ra.router_lifetime == 0) \
            .filter(lambda p: p.icmpv6.nd.ra.retrans_timer == 0) \
            .filter(lambda p: p.icmpv6.nd.ra.reachable_time == 0) \
            .must_next()

        # TODO: verify that the BR forgets the radvd's RA parameters in
        # a period after radvd is force killed.


if __name__ == '__main__':
    unittest.main()
