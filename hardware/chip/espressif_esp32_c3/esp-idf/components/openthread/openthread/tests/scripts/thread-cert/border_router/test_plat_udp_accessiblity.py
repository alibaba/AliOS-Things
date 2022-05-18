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
import unittest

import thread_cert
import config

# Test description:
#   This test verifies UDP servers can be accessible using RLOC/ALOC/MLEID/LINK-LOCAL/OMR when PLAT_UDP is enabled.
#   This test uses SRP server for convenience.
#
# Topology:
#    -----------(eth)------
#           |
#          BR (Leader)
#           |
#        ROUTER
#

BR = 1
ROUTER = 2


class TestPlatUdpAccessibility(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        BR: {
            'name': 'BR',
            'is_otbr': True,
            'version': '1.2',
        },
        ROUTER: {
            'name': 'Router',
            'version': '1.2',
        },
    }

    def test(self):
        br = self.nodes[BR]
        router = self.nodes[ROUTER]

        br.start()
        self.simulator.go(5)
        self.assertEqual('leader', br.get_state())
        br.srp_server_set_enabled(True)

        router.start()
        self.simulator.go(5)
        self.assertEqual('router', router.get_state())

        # Router1 can ping to/from the Host on infra link.
        self.assertTrue(router.ping(br.get_rloc()))

        server_port = br.get_srp_server_port()

        self._test_srp_server(router, br.get_mleid(), server_port)
        self._test_srp_server(router, br.get_linklocal(), server_port)
        self._test_srp_server(router, br.get_ip6_address(config.ADDRESS_TYPE.OMR)[0], server_port)
        self._test_srp_server(router, br.get_rloc(), server_port)
        for server_aloc in br.get_ip6_address(config.ADDRESS_TYPE.ALOC):
            self._test_srp_server(router, server_aloc, server_port)

        self._testDhcp6ClientAfterReset(br, br, br)

    def _testDhcp6ClientAfterReset(self, server, client, reset_device):
        DHCP6_PREFIX = '2001::/64'

        # Configure DHCP6 server
        server.add_prefix(DHCP6_PREFIX, 'pdros')
        self.simulator.go(3)
        server.register_netdata()
        self.simulator.go(10)

        # Verify DHCP6 client works
        self.assertTrue(client.get_addr(DHCP6_PREFIX))
        self.simulator.go(3)

        reset_device.reset()
        reset_device.start()
        self.simulator.go(5)
        self.assertIn(reset_device.get_state(), ['leader', 'router'])
        self.simulator.go(5)

        if reset_device == server:
            # Reconfigure DHCP6 server if necessary
            server.add_prefix(DHCP6_PREFIX, 'pdros')
            self.simulator.go(3)
            server.register_netdata()

        self.simulator.go(10)

        # Verify DHCP6 client works after reset
        self.assertTrue(client.get_addr(DHCP6_PREFIX))
        self.simulator.go(3)

    def _test_srp_server(self, client, server_addr, server_port):
        print(f'Testing SRP server: {server_addr}:{server_port}')

        # check if the SRP client can register to the SRP server
        client.srp_client_start(server_addr, server_port)
        client.srp_client_set_host_name('host1')
        client.srp_client_set_host_address(client.get_rloc())
        client.srp_client_add_service('ins1', '_ipp._tcp', 11111)
        self.simulator.go(3)
        self.assertEqual(client.srp_client_get_host_state(), 'Registered')

        # check if the SRP client can remove from the SRP server
        client.srp_client_remove_host()
        self.simulator.go(3)
        self.assertEqual(client.srp_client_get_host_state(), 'Removed')

        # stop the SRP client for the next round
        client.srp_client_stop()
        self.simulator.go(3)


if __name__ == '__main__':
    unittest.main()
