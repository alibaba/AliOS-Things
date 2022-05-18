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

import ipaddress
import unittest

import command
import thread_cert

# Test description:
#   This test verifies SRP client behavior to save the selected server address info (unicast) by
#   auto-start feature.
#
# Topology:
#
#   4 node (SRP client as leader, 3 SRP servers).
#

CLIENT = 1
SERVER1 = 2
SERVER2 = 3
SERVER3 = 4

WAIT_TIME = 5
MAX_ITER = 5


class SrpAutoStartMode(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        CLIENT: {
            'name': 'SRP_CLIENT',
            'networkkey': '00112233445566778899aabbccddeeff',
            'mode': 'rdn',
        },
        SERVER1: {
            'name': 'SRP_SERVER1',
            'networkkey': '00112233445566778899aabbccddeeff',
            'mode': 'rn',
        },
        SERVER2: {
            'name': 'SRP_SERVER2',
            'networkkey': '00112233445566778899aabbccddeeff',
            'mode': 'rn',
        },
        SERVER3: {
            'name': 'SRP_SERVER3',
            'networkkey': '00112233445566778899aabbccddeeff',
            'mode': 'rn',
        },
    }

    def test(self):
        client = self.nodes[CLIENT]
        server1 = self.nodes[SERVER1]
        server2 = self.nodes[SERVER2]
        server3 = self.nodes[SERVER3]

        # Start the server & client devices.

        client.start()
        self.simulator.go(WAIT_TIME)
        self.assertEqual(client.get_state(), 'leader')

        for node in [server1, server2, server3]:
            node.start()
            self.simulator.go(WAIT_TIME)
            self.assertEqual(node.get_state(), 'child')

        # Enable auto start mode on client and check that server1 is used.

        server1.srp_server_set_enabled(True)
        self.simulator.go(WAIT_TIME)

        client.srp_client_set_host_name('host')
        client.srp_client_set_host_address('2001::1')
        client.srp_client_add_service('my-service', '_ipps._tcp', 12345)

        self.assertEqual(client.srp_client_get_state(), 'Disabled')
        client.srp_client_enable_auto_start_mode()
        self.assertEqual(client.srp_client_get_auto_start_mode(), 'Enabled')
        self.simulator.go(WAIT_TIME)

        self.assertEqual(client.srp_client_get_state(), 'Enabled')
        self.assertTrue(server1.has_ipaddr(client.srp_client_get_server_address()))
        self.assertEqual(client.srp_client_get_host_state(), 'Registered')

        # Enable server2 and server3 and check that server1 is still used.

        server2.srp_server_set_enabled(True)
        server3.srp_server_set_enabled(True)
        self.simulator.go(WAIT_TIME)
        self.assertTrue(server1.has_ipaddr(client.srp_client_get_server_address()))

        # Stop and restart the client (multiple times) and verify that
        # server1 is always picked.

        for iter in range(0, MAX_ITER):
            client.srp_client_stop()
            client.srp_client_enable_auto_start_mode()
            self.simulator.go(WAIT_TIME)
            self.assertTrue(server1.has_ipaddr(client.srp_client_get_server_address()))
            self.assertEqual(client.srp_client_get_host_state(), 'Registered')

        # Stop the client, then stop server1 and restart client and
        # verify that server1 is no longer used.

        client.srp_client_stop()
        server1.srp_server_set_enabled(False)
        self.simulator.go(WAIT_TIME)

        client.srp_client_enable_auto_start_mode()
        self.simulator.go(WAIT_TIME)
        server_address = client.srp_client_get_server_address()
        self.assertFalse(server1.has_ipaddr(server_address))
        self.assertTrue(server2.has_ipaddr(server_address) or server3.has_ipaddr(server_address))
        self.assertEqual(client.srp_client_get_host_state(), 'Registered')

        # Start back server1, then restart client and verify that now we remain
        # with the new saved server info.

        server1.srp_server_set_enabled(True)
        self.simulator.go(WAIT_TIME)

        for iter in range(0, MAX_ITER):
            client.srp_client_stop()
            self.simulator.go(WAIT_TIME)
            client.srp_client_enable_auto_start_mode()
            self.simulator.go(WAIT_TIME)
            self.assertEqual(client.srp_client_get_server_address(), server_address)
            self.assertEqual(client.srp_client_get_host_state(), 'Registered')


if __name__ == '__main__':
    unittest.main()
