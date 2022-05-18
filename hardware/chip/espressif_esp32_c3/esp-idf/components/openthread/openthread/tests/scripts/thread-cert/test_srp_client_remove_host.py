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
#   This test verifies SRP client `RemoveHostAndServices()` behavior when host info is not yet registered.
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


class SrpRemoveHost(thread_cert.TestCase):
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

        #-------------------------------------------------------------------------------------
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

        #-------------------------------------------------------------------------------------
        # Register a single service and verify that it worked.

        client.srp_client_set_host_name('host')
        client.srp_client_set_host_address('2001::1')
        client.srp_client_add_service('ins', '_srv._udp', 1977)
        self.simulator.go(2)

        client_services = client.srp_client_get_services()
        self.assertEqual(len(client_services), 1)
        client_service = client_services[0]
        self.assertEqual(client_service['instance'], 'ins')
        self.assertEqual(client_service['name'], '_srv._udp')
        self.assertEqual(int(client_service['port']), 1977)
        self.assertEqual(int(client_service['priority']), 0)
        self.assertEqual(int(client_service['weight']), 0)
        self.assertEqual(client_service['state'], 'Registered')

        server_hosts = server.srp_server_get_hosts()
        self.assertEqual(len(server_hosts), 1)
        server_host = server_hosts[0]
        self.assertEqual(server_host['fullname'], 'host.default.service.arpa.')
        self.assertEqual(server_host['deleted'], 'false')
        self.assertEqual(len(server_host['addresses']), 1)
        self.assertIn('2001:0:0:0:0:0:0:1', server_host['addresses'])

        server_services = server.srp_server_get_services()
        self.assertEqual(len(server_services), 1)
        server_service = server_services[0]
        self.assertEqual(server_service['fullname'], 'ins._srv._udp.default.service.arpa.')
        self.assertEqual(server_service['instance'], 'ins')
        self.assertEqual(server_service['name'], '_srv._udp')
        self.assertEqual(server_service['deleted'], 'false')
        self.assertEqual(int(server_service['port']), 1977)
        self.assertEqual(int(server_service['priority']), 0)
        self.assertEqual(int(server_service['weight']), 0)
        self.assertEqual(server_service['host'], 'host')

        #-------------------------------------------------------------------------------------
        # Clear the info on client, and verify that it is still present on server.

        client.srp_client_clear_host()
        self.simulator.go(2)

        client_services = client.srp_client_get_services()
        self.assertEqual(len(client_services), 0)
        self.assertIsNone(client.srp_client_get_host_name())
        self.assertIsNone(client.srp_client_get_host_address())

        server_services = server.srp_server_get_services()
        self.assertEqual(len(server_services), 1)

        #-------------------------------------------------------------------------------------
        # Now set the host name and request "host remove" requiring that client updates server
        # even if host is not yet registered. Verify that client updates the server by checking
        # that the host and service entries are marked as "deleted" on the server (i.e. deleted
        # by the name (and associated key) are retained).

        client.srp_client_set_host_name('host')
        client.srp_client_remove_host(send_unreg_to_server=True)
        self.simulator.go(2)

        self.assertIsNone(client.srp_client_get_host_name())

        server_hosts = server.srp_server_get_hosts()
        self.assertEqual(len(server_hosts), 1)
        server_host = server_hosts[0]
        self.assertEqual(server_host['fullname'], 'host.default.service.arpa.')
        self.assertEqual(server_host['deleted'], 'true')

        server_services = server.srp_server_get_services()
        self.assertEqual(len(server_services), 1)
        server_service = server_services[0]
        self.assertEqual(server_service['fullname'], 'ins._srv._udp.default.service.arpa.')
        self.assertEqual(server_service['deleted'], 'true')

        #-------------------------------------------------------------------------------------
        # Again request "host remove" but this time request `remove_key`. Verify that entries
        # on the server are fully removed .

        client.srp_client_set_host_name('host')
        client.srp_client_remove_host(remove_key=True, send_unreg_to_server=True)
        self.simulator.go(2)

        self.assertIsNone(client.srp_client_get_host_name())
        self.assertEqual(len(server.srp_server_get_services()), 0)
        self.assertEqual(len(server.srp_server_get_hosts()), 0)


if __name__ == '__main__':
    unittest.main()
