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
#   This test verifies basic SRP function that a service can be registered to
#   and unregistered from the SRP server.
#
# Topology:
#     LEADER (SRP server)
#       |
#       |
#     ROUTER (SRP client)
#

SERVER = 1
CLIENT = 2


class SrpRegisterSingleService(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        SERVER: {
            'name': 'SRP_SERVER',
            'networkkey': '00112233445566778899aabbccddeeff',
            'mode': 'rdn',
        },
        CLIENT: {
            'name': 'SRP_CLIENT',
            'networkkey': '00112233445566778899aabbccddeeff',
            'mode': 'rdn',
        },
    }

    def test(self):
        server = self.nodes[SERVER]
        client = self.nodes[CLIENT]

        #
        # 0. Start the server & client devices.
        #

        server.srp_server_set_enabled(True)
        server.start()
        self.simulator.go(5)
        self.assertEqual(server.get_state(), 'leader')
        self.simulator.go(5)

        client.srp_server_set_enabled(False)
        client.start()
        self.simulator.go(5)
        self.assertEqual(client.get_state(), 'router')

        #
        # 1. Register a single service and verify that it works.
        #

        client.srp_client_set_host_name('my-host')
        client.srp_client_set_host_address('2001::1')
        client.srp_client_start(server.get_addrs()[0], client.get_srp_server_port())
        client.srp_client_add_service('my-service', '_ipps._tcp', 12345, 0, 0, ['abc', 'def=', 'xyz=XYZ'])
        self.simulator.go(2)

        self.check_host_and_service(server, client, '2001::1')

        #
        # 2. Unregister a service but retain the name. The service name should be
        #    retained on the server.
        #

        client.srp_client_remove_service('my-service', '_ipps._tcp')
        self.simulator.go(2)

        client_services = client.srp_client_get_services()
        print(client_services)
        self.assertEqual(len(client_services), 0)

        server_services = server.srp_server_get_services()
        print(server_services)
        self.assertEqual(len(server_services), 1)
        server_service = server_services[0]

        # Verify that the service has been successfully removed on the server side.
        self.assertEqual(server_service['deleted'], 'true')

        server_hosts = server.srp_server_get_hosts()
        print(server_hosts)
        self.assertEqual(len(server_hosts), 1)
        server_host = server_hosts[0]

        # The registered host should not be touched.
        self.assertEqual(server_host['deleted'], 'false')
        self.assertEqual(server_host['name'], 'my-host')
        self.assertEqual(len(server_host['addresses']), 1)
        self.assertEqual(ipaddress.ip_address(server_host['addresses'][0]), ipaddress.ip_address('2001::1'))

        #
        # 3. Register the same service again. It should succeed and the name should be
        #    reused.
        #

        client.srp_client_add_service('my-service', '_ipps._tcp', 12345, 0, 0, ['abc', 'def=', 'xyz=XYZ'])
        self.simulator.go(2)

        self.check_host_and_service(server, client, '2001::1')

        #
        # 4. Update the SRP host address and make sure it will succeed.
        #

        client.srp_client_set_host_address('2001::2')
        self.simulator.go(2)

        self.check_host_and_service(server, client, '2001::2')

        #
        # 5. Fully remove the host and all its service instances.
        #

        client.srp_client_remove_host(remove_key=True)
        self.simulator.go(2)

        client_services = client.srp_client_get_services()
        print(client_services)
        self.assertEqual(len(client_services), 0)

        print(client.srp_client_get_host_state())

        server_services = server.srp_server_get_services()
        print(server_services)
        self.assertEqual(len(server_services), 0)

        server_hosts = server.srp_server_get_hosts()
        print(server_hosts)
        self.assertEqual(len(server_hosts), 0)

    def check_host_and_service(self, server, client, host_addr):
        """Check that we have properly registered host and service instance.
        """

        client_services = client.srp_client_get_services()
        print(client_services)
        self.assertEqual(len(client_services), 1)
        client_service = client_services[0]

        # Verify that the client possesses correct service resources.
        self.assertEqual(client_service['instance'], 'my-service')
        self.assertEqual(client_service['name'], '_ipps._tcp')
        self.assertEqual(int(client_service['port']), 12345)
        self.assertEqual(int(client_service['priority']), 0)
        self.assertEqual(int(client_service['weight']), 0)

        # Verify that the client received a SUCCESS response for the server.
        self.assertEqual(client_service['state'], 'Registered')

        server_services = server.srp_server_get_services()
        self.assertEqual(len(server_services), 1)
        server_service = server_services[0]

        # Verify that the server accepted the SRP registration and stores
        # the same service resources.
        self.assertEqual(server_service['deleted'], 'false')
        self.assertEqual(server_service['instance'], client_service['instance'])
        self.assertEqual(server_service['name'], client_service['name'])
        self.assertEqual(server_service['subtypes'], '(null)')
        self.assertEqual(int(server_service['port']), int(client_service['port']))
        self.assertEqual(int(server_service['priority']), int(client_service['priority']))
        self.assertEqual(int(server_service['weight']), int(client_service['weight']))
        # We output value of TXT entry as HEX string.
        print(server_service['TXT'])
        self.assertEqual(server_service['TXT'], ['abc', 'def=', 'xyz=58595a'])
        self.assertEqual(server_service['host'], 'my-host')

        server_hosts = server.srp_server_get_hosts()
        print(server_hosts)
        self.assertEqual(len(server_hosts), 1)
        server_host = server_hosts[0]

        self.assertEqual(server_host['deleted'], 'false')
        self.assertEqual(server_host['fullname'], server_service['host_fullname'])
        self.assertEqual(len(server_host['addresses']), 1)
        self.assertEqual(ipaddress.ip_address(server_host['addresses'][0]), ipaddress.ip_address(host_addr))


if __name__ == '__main__':
    unittest.main()
