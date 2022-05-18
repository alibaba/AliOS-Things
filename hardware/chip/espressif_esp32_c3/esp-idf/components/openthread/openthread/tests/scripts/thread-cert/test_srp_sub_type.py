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
#   This test verifies SRP client and server support for registering sub-type services.
#
# Topology:
#
#     LEADER (SRP server)
#       |
#       |
#     ROUTER (SRP client)
#

SERVER = 1
CLIENT = 2


class SrpSubType(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        SERVER: {
            'name': 'SRP_SERVER',
            'mode': 'rdn',
        },
        CLIENT: {
            'name': 'SRP_CLIENT',
            'mode': 'rdn',
        },
    }

    def test(self):
        server = self.nodes[SERVER]
        client = self.nodes[CLIENT]

        # Start the server & client devices.

        server.start()
        self.simulator.go(5)
        self.assertEqual(server.get_state(), 'leader')

        client.start()
        self.simulator.go(5)
        self.assertEqual(client.get_state(), 'router')

        server.srp_server_set_enabled(True)
        client.srp_client_enable_auto_start_mode()
        self.simulator.go(5)

        # Register a single service with 3 subtypes and verify that it worked.

        client.srp_client_set_host_name('host1')
        client.srp_client_set_host_address('2001::1')
        client.srp_client_add_service('ins1', '_srv._udp,_s1,_s2,_s3', 1977)
        self.simulator.go(2)
        self.check_service_on_client_and_server(server, client)

        # Remove the service on client

        client.srp_client_remove_service('ins1', '_srv._udp')
        self.simulator.go(2)

        client_services = client.srp_client_get_services()
        self.assertEqual(len(client_services), 0)
        server_services = server.srp_server_get_services()
        self.assertEqual(len(server_services), 1)
        server_service = server_services[0]
        self.assertEqual(server_service['fullname'], 'ins1._srv._udp.default.service.arpa.')
        self.assertEqual(server_service['instance'], 'ins1')
        self.assertEqual(server_service['name'], '_srv._udp')
        self.assertEqual(server_service['deleted'], 'true')

        # Register the same service again.

        client.srp_client_add_service('ins1', '_srv._udp,_s1,_s2,_s3', 1977)
        self.simulator.go(2)
        self.check_service_on_client_and_server(server, client)

    def check_service_on_client_and_server(self, server, client):
        # Check the service on client
        client_services = client.srp_client_get_services()
        self.assertEqual(len(client_services), 1)
        client_service = client_services[0]
        self.assertEqual(client_service['instance'], 'ins1')
        self.assertEqual(client_service['name'], '_srv._udp,_s1,_s2,_s3')
        self.assertEqual(int(client_service['port']), 1977)
        self.assertEqual(int(client_service['priority']), 0)
        self.assertEqual(int(client_service['weight']), 0)
        self.assertEqual(client_service['state'], 'Registered')

        # Check the service on server
        server_services = server.srp_server_get_services()
        self.assertEqual(len(server_services), 1)
        server_service = server_services[0]
        self.assertEqual(server_service['fullname'], 'ins1._srv._udp.default.service.arpa.')
        self.assertEqual(server_service['instance'], 'ins1')
        self.assertEqual(server_service['name'], '_srv._udp')
        self.assertEqual(server_service['deleted'], 'false')
        self.assertEqual(set(server_service['subtypes'].split(',')), {'_s1', '_s2', '_s3'})
        self.assertEqual(int(server_service['port']), 1977)
        self.assertEqual(int(server_service['priority']), 0)
        self.assertEqual(int(server_service['weight']), 0)
        self.assertEqual(server_service['host'], 'host1')


if __name__ == '__main__':
    unittest.main()
