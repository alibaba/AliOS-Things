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
#   This test verifies SRP client auto-start functionality that SRP client can
#   correctly discover and connect to SRP server.
#
# Topology:
#
#   CLIENT (leader) -- SERVER1 (router)
#      |
#      |
#   SERVER2 (router)
#

CLIENT = 1
SERVER1 = 2
SERVER2 = 3


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
            'mode': 'rdn',
        },
        SERVER2: {
            'name': 'SRP_SERVER2',
            'networkkey': '00112233445566778899aabbccddeeff',
            'mode': 'rdn',
        },
    }

    def test(self):
        client = self.nodes[CLIENT]
        server1 = self.nodes[SERVER1]
        server2 = self.nodes[SERVER2]

        #
        # 0. Start the server & client devices.
        #

        client.srp_server_set_enabled(False)
        client.start()
        self.simulator.go(5)
        self.assertEqual(client.get_state(), 'leader')

        server1.srp_server_set_enabled(True)
        server2.srp_server_set_enabled(False)
        server1.start()
        server2.start()
        self.simulator.go(5)
        self.assertEqual(server1.get_state(), 'router')
        self.assertEqual(server2.get_state(), 'router')

        #
        # 1. Enable auto start mode on client and check that server1 is used.
        #

        self.assertEqual(client.srp_client_get_state(), 'Disabled')
        client.srp_client_enable_auto_start_mode()
        self.assertEqual(client.srp_client_get_auto_start_mode(), 'Enabled')
        self.simulator.go(2)

        self.assertEqual(client.srp_client_get_state(), 'Enabled')
        self.assertTrue(server1.has_ipaddr(client.srp_client_get_server_address()))

        #
        # 2. Disable server1 and check client is stopped/disabled.
        #

        server1.srp_server_set_enabled(False)
        self.simulator.go(5)
        self.assertEqual(client.srp_client_get_state(), 'Disabled')

        #
        # 3. Enable server2 and check client starts again.
        #

        server2.srp_server_set_enabled(True)
        self.simulator.go(5)
        self.assertEqual(client.srp_client_get_state(), 'Enabled')
        server2_address = client.srp_client_get_server_address()

        #
        # 4. Enable both servers and check client stays with server2.
        #

        server1.srp_server_set_enabled(True)
        self.simulator.go(5)
        self.assertEqual(client.srp_client_get_state(), 'Enabled')
        self.assertEqual(client.srp_client_get_server_address(), server2_address)

        #
        # 5. Disable server2 and check client switches to server1.
        #

        server2.srp_server_set_enabled(False)
        self.simulator.go(5)
        self.assertEqual(client.srp_client_get_state(), 'Enabled')
        self.assertNotEqual(client.srp_client_get_server_address(), server2_address)


if __name__ == '__main__':
    unittest.main()
