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

import command
import thread_cert

# Test description:
#
#   This test verifies the behavior of DNS client feature where the SRP
#   server address (from SRP client auto-start feature) is used as the
#   default IPv6 address for DNS resolver config when user does not
#   explicitly set a default address
#
# Topology: two nodes
#
#   leader (SRP server & client & DNS client) --- router (SRP client & DNS client)
#

LEADER = 1
ROUTER = 2

DEFAULT_ADDRESS = 'fd00:1:2:3:4:5:6:7'


class DnsClientConfigAutoStart(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
        },
    }

    def test(self):
        leader = self.nodes[LEADER]
        router = self.nodes[ROUTER]

        # Form network.

        leader.start()
        self.simulator.go(5)
        self.assertEqual(leader.get_state(), 'leader')

        router.start()
        self.simulator.go(5)
        self.assertEqual(router.get_state(), 'router')

        # On leader set DNS config explicitly.

        leader.dns_set_config(DEFAULT_ADDRESS)
        config = leader.dns_get_config()
        self.assertEqual(config['Server'], '[{}]:53'.format(DEFAULT_ADDRESS))

        # Start leader to act as SRP server.

        leader.srp_server_set_enabled(True)

        # Enable SRP client auto start on both nodes.

        leader.srp_client_enable_auto_start_mode()
        router.srp_client_enable_auto_start_mode()
        self.simulator.go(5)

        # Verify that on router the default DNS config is changed
        # to the same address being used by SRP client.

        srp_server_address = router.srp_client_get_server_address()
        self.assertEqual(leader.srp_client_get_server_address(), srp_server_address)

        config = router.dns_get_config()
        self.assertEqual(config['Server'], '[{}]:53'.format(srp_server_address))

        # Verify that on leader the default DNS config remains
        # as before (the address explicitly set earlier).

        config = leader.dns_get_config()
        self.assertEqual(config['Server'], '[{}]:53'.format(DEFAULT_ADDRESS))

        # On leader clear DNS config (the explicitly set address)
        # and verify that it adopts the SRP server address.

        leader.dns_set_config("::")
        config = leader.dns_get_config()
        self.assertEqual(config['Server'], '[{}]:53'.format(srp_server_address))

        # On leader set DNS config explicitly again.

        leader.dns_set_config(DEFAULT_ADDRESS)
        config = leader.dns_get_config()
        self.assertEqual(config['Server'], '[{}]:53'.format(DEFAULT_ADDRESS))

        # Stop SRP server on leader and start it on router.

        leader.srp_server_set_enabled(False)
        router.srp_server_set_enabled(True)
        self.simulator.go(5)

        # Verify that SRP client on router switched to new SRP server.

        self.assertNotEqual(srp_server_address, router.srp_client_get_server_address())
        self.assertEqual(router.srp_client_get_server_address(), leader.srp_client_get_server_address())

        # Verify that config on router gets changed while on leader
        # it remains unchanged.

        config = router.dns_get_config()
        srp_server_address = router.srp_client_get_server_address()
        self.assertEqual(config['Server'], '[{}]:53'.format(srp_server_address))

        config = leader.dns_get_config()
        self.assertEqual(config['Server'], '[{}]:53'.format(DEFAULT_ADDRESS))


if __name__ == '__main__':
    unittest.main()
