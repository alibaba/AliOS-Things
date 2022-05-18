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

import ipaddress
import config
import thread_cert

# Test description:
#   This test verifies that when external routes are changed in Network Data, a
#   border router will make according updates to routes at the linux kernel.
#
# Topology:
#    ----------------(eth)----------------------
#           |                  |          |
#          BR1 (Leader) ----- BR2       HOST
#           |                  |
#         ROUTER1             ROUTER2

BR1 = 1
BR2 = 2
ROUTER1 = 3
ROUTER2 = 4
HOST = 5

ROUTE1 = '2402:1234:1234:1234::/64'


class ExternalRoutes(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        BR1: {
            'name': 'BR_1',
            'allowlist': [BR2, ROUTER1],
            'is_otbr': True,
            'version': '1.2',
        },
        BR2: {
            'name': 'BR_2',
            'allowlist': [BR1, ROUTER2],
            'is_otbr': True,
            'version': '1.2',
        },
        ROUTER1: {
            'name': 'Router_1',
            'allowlist': [BR1],
            'version': '1.2',
        },
        ROUTER2: {
            'name': 'Router_2',
            'allowlist': [BR2],
            'version': '1.2',
        },
        HOST: {
            'name': 'Host',
            'is_host': True,
        }
    }

    def test(self):
        br1 = self.nodes[BR1]
        br2 = self.nodes[BR2]
        router1 = self.nodes[ROUTER1]
        router2 = self.nodes[ROUTER2]
        host = self.nodes[HOST]

        br1.start()
        self.simulator.go(5)
        self.assertEqual('leader', br1.get_state())

        br2.start()
        self.simulator.go(5)
        self.assertEqual('router', br2.get_state())

        router1.start()
        router2.start()
        host.start()
        self.simulator.go(5)
        self.assertEqual('router', router1.get_state())
        self.assertEqual('router', router2.get_state())

        # Manually add ROUTE1 at BR1
        br1.add_route(ROUTE1)
        br1.register_netdata()

        # BRs has installed all external routes in their kernels respectively
        netdata = br1.get_netdata()
        br1_kernel_routes = self.get_routes_from_kernel(br1)
        for route in self.get_routes_from_netdata(netdata, br1.get_addr16()):
            self.assertIn(route, br1_kernel_routes)
        br2_kernel_routes = self.get_routes_from_kernel(br2)
        for route in self.get_routes_from_netdata(netdata, br2.get_addr16()):
            self.assertIn(route, br2_kernel_routes)

        # ROUTE1 has been installed in BR2 but not BR1
        self.assertNotIn(ipaddress.IPv6Network(ROUTE1), br1_kernel_routes)
        self.assertIn(ipaddress.IPv6Network(ROUTE1), br2_kernel_routes)

        # Remove ROUTE1
        br1.remove_route(ROUTE1)
        br1.register_netdata()

        # Verify that external routes are still in kernel routes
        netdata = br1.get_netdata()
        br1_kernel_routes = self.get_routes_from_kernel(br1)
        for route in self.get_routes_from_netdata(netdata, br1.get_addr16()):
            self.assertIn(route, br1_kernel_routes)
        br2_kernel_routes = self.get_routes_from_kernel(br2)
        for route in self.get_routes_from_netdata(netdata, br2.get_addr16()):
            self.assertIn(route, br2_kernel_routes)

        # ROUTE1 has been removed from kernel routes
        self.assertNotIn(ipaddress.IPv6Network(ROUTE1), br1_kernel_routes)
        self.assertNotIn(ipaddress.IPv6Network(ROUTE1), br2_kernel_routes)

        br2.bash('ip link set eth0 down')
        self.simulator.go(10)

        # HOST pings BR2's OMR, the ping should succeed
        self.assertTrue(host.ping_ether(br2.get_ip6_address(address_type=config.ADDRESS_TYPE.OMR)[0]))

    def get_routes_from_netdata(self, netdata, exclude_rloc16):
        routes = []
        for entry in netdata['Routes']:
            items = entry.split()
            prefix = items[0]
            rloc16 = int(items[-1], 16)
            if rloc16 != exclude_rloc16:
                routes.append(ipaddress.IPv6Network(prefix))
        return routes

    def get_routes_from_kernel(self, br):
        routes = []
        for entry in br.bash('ip -6 -d route list dev wpan0'):
            routes.append(ipaddress.IPv6Network(entry.split()[1]))
        return routes


if __name__ == '__main__':
    unittest.main()
