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

import config
import ipaddress
import unittest

import command
import thread_cert

# Test description:
#   This test verifies Anycast Locator functionality
#
# Topology:
#
#   LEADER -- ROUTER1 -- ROUTER2 -- ROUTER3 -- ROUTER4
#

LEADER = 1
ROUTER1 = 2
ROUTER2 = 3
ROUTER3 = 4
ROUTER4 = 5

SRV_ENT_NUMBER = '44970'
SRV_SERVICE_DATA = '571234'
SRV_SERVER_DATA = '00'


class AnycastLocator(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1]
        },
        ROUTER1: {
            'name': 'ROUTER1',
            'mode': 'rdn',
            'allowlist': [LEADER, ROUTER2]
        },
        ROUTER2: {
            'name': 'ROUTER2',
            'mode': 'rdn',
            'allowlist': [ROUTER1, ROUTER3]
        },
        ROUTER3: {
            'name': 'ROUTER3',
            'mode': 'rdn',
            'allowlist': [ROUTER2, ROUTER4]
        },
        ROUTER4: {
            'name': 'ROUTER4',
            'mode': 'rdn',
            'allowlist': [ROUTER3]
        },
    }

    def test(self):
        leader = self.nodes[LEADER]
        router1 = self.nodes[ROUTER1]
        router2 = self.nodes[ROUTER2]
        router3 = self.nodes[ROUTER3]
        router4 = self.nodes[ROUTER4]
        nodes = [leader, router1, router2, router3, router4]

        #
        # 0. Start the leader and routers
        #

        leader.start()
        self.simulator.go(5)
        self.assertEqual(leader.get_state(), 'leader')

        for node in nodes[1:]:
            node.start()
            self.simulator.go(5)
            self.assertEqual(node.get_state(), 'router')

        #
        # 1. Locate leader ALOC from all nodes
        #

        leader_aloc = leader.get_addr_leader_aloc()

        for node in nodes:
            result = node.locate(leader_aloc)
            self.assertEqual(result[0], leader.get_mleid())
            self.assertEqual(int(result[1], 0), leader.get_addr16())

        #
        # 2. Add a service on router4 and locate its ALOC
        #

        router4.add_service(SRV_ENT_NUMBER, SRV_SERVICE_DATA, SRV_SERVER_DATA)
        router4.register_netdata()
        self.simulator.go(5)
        services = leader.get_services()
        self.assertEqual(len(services), 1)

        service_aloc = router4.get_ip6_address(config.ADDRESS_TYPE.ALOC)[0]

        for node in nodes:
            result = node.locate(service_aloc)
            self.assertEqual(result[0], router4.get_mleid())
            self.assertEqual(int(result[1], 0), router4.get_addr16())

        #
        # 3. Add same service on leader and ensure we locate the closest ALOC destination
        #

        leader.add_service(SRV_ENT_NUMBER, SRV_SERVICE_DATA, SRV_SERVER_DATA)
        leader.register_netdata()
        self.simulator.go(5)
        services = leader.get_services()
        self.assertEqual(len(services), 2)

        # leader and router1 should locate leader as closest service ALOC,
        # router3 and router4 should locate router4. router2 is in middle
        # and can locate either one.
        for node in [leader, router1]:
            result = node.locate(service_aloc)
            self.assertEqual(result[0], leader.get_mleid())
            self.assertEqual(int(result[1], 0), leader.get_addr16())

        for node in [router3, router4]:
            result = node.locate(service_aloc)
            self.assertEqual(result[0], router4.get_mleid())
            self.assertEqual(int(result[1], 0), router4.get_addr16())

        result = router2.locate(service_aloc)
        self.assertTrue(result[0] in [leader.get_mleid(), router4.get_mleid()])
        self.assertTrue(int(result[1], 0) in [leader.get_addr16(), router4.get_addr16()])


if __name__ == '__main__':
    unittest.main()
