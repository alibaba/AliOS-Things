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
from ipaddress import IPv6Network

import config
import thread_cert

# Test description:
#   This test verifies Vicarious Router Solicitation.
#
# Topology:
#    -------------(eth)----------------------------
#           |               |           |
#          BR1             BR2         HOST
#           |               |
#        ROUTER1         ROUTER2
#
#     Thread Net1       Thread Net2
#

BR1 = 1
ROUTER1 = 2
BR2 = 3
ROUTER2 = 4
HOST = 5

CHANNEL1 = 18
CHANNEL2 = 19


class MultiThreadNetworks(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        BR1: {
            'name': 'BR_1',
            'allowlist': [ROUTER1],
            'is_otbr': True,
            'version': '1.2',
            'channel': CHANNEL1,
            'router_selection_jitter': 1,
        },
        ROUTER1: {
            'name': 'Router_1',
            'allowlist': [BR1],
            'version': '1.2',
            'channel': CHANNEL1,
            'router_selection_jitter': 1,
        },
        BR2: {
            'name': 'BR_2',
            'allowlist': [ROUTER2],
            'is_otbr': True,
            'version': '1.2',
            'channel': CHANNEL2,
            'router_selection_jitter': 1,
        },
        ROUTER2: {
            'name': 'Router_2',
            'allowlist': [BR2],
            'version': '1.2',
            'channel': CHANNEL2,
            'router_selection_jitter': 1,
        },
        HOST: {
            'name': 'Host',
            'is_host': True
        },
    }

    def test(self):
        ON_LINK_PREFIX = 'fd00::/64'
        br1 = self.nodes[BR1]
        router1 = self.nodes[ROUTER1]
        br2 = self.nodes[BR2]
        router2 = self.nodes[ROUTER2]
        host = self.nodes[HOST]

        host.start(start_radvd=True, prefix=ON_LINK_PREFIX, slaac=True)
        self.simulator.go(5)

        br1.start()
        self.simulator.go(5)
        self.assertEqual('leader', br1.get_state())

        router1.start()
        self.simulator.go(5)
        self.assertEqual('router', router1.get_state())

        self.simulator.go(10)
        self.collect_ipaddrs()

        logging.info("BR1     addrs: %r", br1.get_addrs())
        logging.info("ROUTER1 addrs: %r", router1.get_addrs())
        logging.info("HOST    addrs: %r", host.get_addrs())

        self.assertEqual(len(br1.get_routes()), 1)
        br1_on_link_prefix = br1.get_routes()[0].split(' ')[0]
        self.assertEqual(IPv6Network(br1_on_link_prefix), IPv6Network(ON_LINK_PREFIX))

        host_on_link_addr = host.get_matched_ula_addresses(br1_on_link_prefix)[0]
        self.assertTrue(router1.ping(host_on_link_addr))
        self.assertTrue(
            host.ping(router1.get_ip6_address(config.ADDRESS_TYPE.OMR)[0], backbone=True, interface=host_on_link_addr))

        # Force kill the radvd host so that BR2 will start advertising its own on-link prefix.
        host.kill_radvd_service()

        br2.start()
        self.simulator.go(5)
        self.assertEqual('leader', br2.get_state())

        router2.start()
        self.simulator.go(5)
        self.assertEqual('router', router2.get_state())

        self.simulator.go(25)
        self.collect_ipaddrs()

        logging.info("BR1     addrs: %r", br1.get_addrs())
        logging.info("ROUTER1 addrs: %r", router1.get_addrs())
        logging.info("BR2     addrs: %r", br2.get_addrs())
        logging.info("ROUTER2 addrs: %r", router2.get_addrs())
        logging.info("HOST    addrs: %r", host.get_addrs())

        self.assertTrue(len(br1.get_prefixes()) == 1)
        self.assertTrue(len(router1.get_prefixes()) == 1)
        self.assertTrue(len(br2.get_prefixes()) == 1)
        self.assertTrue(len(router2.get_prefixes()) == 1)

        br1_omr_prefix = br1.get_omr_prefix()
        br2_omr_prefix = br2.get_omr_prefix()
        self.assertNotEqual(br1_omr_prefix, br2_omr_prefix)

        # Verify that the Border Routers starts advertsing new on-link prefix
        # but don't remove the external routes for the radvd on-link prefix
        # immediately, because the SLAAC addresses are still valid.
        self.assertEqual(len(br1.get_routes()), 3)
        self.assertEqual(len(router1.get_routes()), 3)
        self.assertEqual(len(br2.get_routes()), 2)
        self.assertEqual(len(router2.get_routes()), 2)

        br1_external_routes = [route.split(' ')[0] for route in br1.get_routes()]
        br2_external_routes = [route.split(' ')[0] for route in br2.get_routes()]

        on_link_prefixes = list(set(br1_external_routes).intersection(br2_external_routes))
        self.assertEqual(len(on_link_prefixes), 1)
        self.assertEqual(IPv6Network(on_link_prefixes[0]), IPv6Network(br2.get_on_link_prefix()))

        router1_omr_addr = router1.get_ip6_address(config.ADDRESS_TYPE.OMR)[0]
        router2_omr_addr = router2.get_ip6_address(config.ADDRESS_TYPE.OMR)[0]

        # Make sure that addresses of both the deprecated radvd `ON_LINK_PREFIX`
        # and preferred Border Router on-link prefix can be reached by Thread
        # devices in network of Border Router 1.
        for host_on_link_addr in [
                host.get_matched_ula_addresses(on_link_prefixes[0])[0],
                host.get_matched_ula_addresses(ON_LINK_PREFIX)[0]
        ]:
            self.assertTrue(router1.ping(host_on_link_addr))
            self.assertTrue(host.ping(router1_omr_addr, backbone=True, interface=host_on_link_addr))

        host_on_link_addr = host.get_matched_ula_addresses(ON_LINK_PREFIX)[0]

        # Make sure that addresses of the deprecated radvd `ON_LINK_PREFIX`
        # can't be reached by Thread devices in network of Border Router 2.
        self.assertFalse(router2.ping(host_on_link_addr))
        self.assertFalse(host.ping(router2_omr_addr, backbone=True, interface=host_on_link_addr))

        # Wait 30 seconds for the radvd `ON_LINK_PREFIX` to be invalidated
        # and make sure that Thread devices in both networks can't reach
        # the on-link address.
        self.simulator.go(30)  # Valid Lifetime of radvd PIO is set to 60 seconds.
        self.assertEqual(len(host.get_matched_ula_addresses(ON_LINK_PREFIX)), 0)
        self.assertFalse(router1.ping(host_on_link_addr))
        self.assertFalse(host.ping(router1_omr_addr, backbone=True, interface=host_on_link_addr))
        self.assertFalse(router2.ping(host_on_link_addr))
        self.assertFalse(host.ping(router2_omr_addr, backbone=True, interface=host_on_link_addr))

        # Verify connectivity between the two networks.
        self.assertTrue(router1.ping(router2.get_ip6_address(config.ADDRESS_TYPE.OMR)[0]))
        self.assertTrue(router2.ping(router1.get_ip6_address(config.ADDRESS_TYPE.OMR)[0]))


if __name__ == '__main__':
    unittest.main()
