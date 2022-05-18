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

import unittest

import config
import thread_cert

ROUTER1 = 1
ROUTER2 = 2
ROUTER3 = 3
ROUTER4 = 4
ROUTER5 = 5

TEST_PREFIX = '2001:dead:beef:cafe::/64'
TEST_UDP_BYTES = 16
TEST_UDP_PORT = 12345


class TestAnycast(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        ROUTER1: {
            'mode': 'rdn',
            'allowlist': [ROUTER2],
        },
        ROUTER2: {
            'mode': 'rdn',
            'allowlist': [ROUTER1, ROUTER3],
        },
        ROUTER3: {
            'mode': 'rdn',
            'allowlist': [ROUTER2, ROUTER4],
        },
        ROUTER4: {
            'mode': 'rdn',
            'allowlist': [ROUTER3, ROUTER5],
        },
        ROUTER5: {
            'mode': 'rdn',
            'allowlist': [ROUTER4],
        },
    }

    def test(self):
        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'leader')
        self.nodes[ROUTER1].udp_start('::', TEST_UDP_PORT)

        for i in range(ROUTER2, ROUTER5 + 1):
            self.nodes[i].start()
            self.simulator.go(5)
            self.assertEqual(self.nodes[i].get_state(), 'router')
            self.nodes[i].udp_start('::', TEST_UDP_PORT)

        self.__test('ds')  # DHCPv6 Agent (IPv6 Address)
        self.__test('cs')  # DHCPv6 Agent (Other)
        self.__test('ns')  # Neighbor Discovery Agent

    def __test(self, borderRouterFlags):
        # Single Server
        self.nodes[ROUTER2].add_prefix(TEST_PREFIX, borderRouterFlags)
        self.nodes[ROUTER2].register_netdata()
        self.simulator.go(5)
        print(self.nodes[ROUTER2].get_ip6_address(config.ADDRESS_TYPE.ALOC))
        self.assertTrue(len(self.nodes[ROUTER2].get_ip6_address(config.ADDRESS_TYPE.ALOC)) == 1)

        self.nodes[ROUTER3].udp_send(TEST_UDP_BYTES, self.nodes[ROUTER2].get_ip6_address(config.ADDRESS_TYPE.ALOC)[0],
                                     TEST_UDP_PORT)
        self.simulator.go(1)
        self.nodes[ROUTER2].udp_check_rx(TEST_UDP_BYTES)

        # Multiple Servers
        self.nodes[ROUTER5].add_prefix(TEST_PREFIX, borderRouterFlags)
        self.nodes[ROUTER5].register_netdata()
        self.simulator.go(5)
        self.assertTrue(len(self.nodes[ROUTER5].get_ip6_address(config.ADDRESS_TYPE.ALOC)) == 1)

        self.nodes[ROUTER3].udp_send(TEST_UDP_BYTES, self.nodes[ROUTER2].get_ip6_address(config.ADDRESS_TYPE.ALOC)[0],
                                     TEST_UDP_PORT)
        self.simulator.go(1)
        self.nodes[ROUTER2].udp_check_rx(TEST_UDP_BYTES)

        self.nodes[ROUTER4].udp_send(TEST_UDP_BYTES, self.nodes[ROUTER2].get_ip6_address(config.ADDRESS_TYPE.ALOC)[0],
                                     TEST_UDP_PORT)
        self.simulator.go(1)
        self.nodes[ROUTER5].udp_check_rx(TEST_UDP_BYTES)

        # Single Server
        self.nodes[ROUTER2].remove_prefix(TEST_PREFIX)
        self.nodes[ROUTER2].register_netdata()
        self.simulator.go(5)
        self.assertTrue(len(self.nodes[ROUTER2].get_ip6_address(config.ADDRESS_TYPE.ALOC)) == 0)

        self.nodes[ROUTER3].udp_send(TEST_UDP_BYTES, self.nodes[ROUTER5].get_ip6_address(config.ADDRESS_TYPE.ALOC)[0],
                                     TEST_UDP_PORT)
        self.simulator.go(1)
        self.nodes[ROUTER5].udp_check_rx(TEST_UDP_BYTES)

        self.nodes[ROUTER4].udp_send(TEST_UDP_BYTES, self.nodes[ROUTER5].get_ip6_address(config.ADDRESS_TYPE.ALOC)[0],
                                     TEST_UDP_PORT)
        self.simulator.go(1)
        self.nodes[ROUTER5].udp_check_rx(TEST_UDP_BYTES)

        self.nodes[ROUTER5].remove_prefix(TEST_PREFIX)
        self.nodes[ROUTER5].register_netdata()
        self.simulator.go(5)
        self.assertTrue(len(self.nodes[ROUTER5].get_ip6_address(config.ADDRESS_TYPE.ALOC)) == 0)


if __name__ == '__main__':
    unittest.main()
