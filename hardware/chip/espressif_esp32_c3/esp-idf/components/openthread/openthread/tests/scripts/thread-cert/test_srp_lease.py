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
#   This test verifies the SRP server and client properly handle SRP host
#   and service instance lease.
#
# Topology:
#     LEADER (SRP server)
#       |
#       |
#     ROUTER (SRP client)
#

SERVER = 1
CLIENT = 2
LEASE = 10  # Seconds
KEY_LEASE = 20  # Seconds


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
        # 0. Start the server and client devices.
        #

        server.srp_server_set_enabled(True)
        server.srp_server_set_lease_range(LEASE, LEASE, KEY_LEASE, KEY_LEASE)
        server.start()
        self.simulator.go(5)
        self.assertEqual(server.get_state(), 'leader')
        self.simulator.go(5)

        client.srp_server_set_enabled(False)
        client.start()
        self.simulator.go(10)
        self.assertEqual(client.get_state(), 'router')

        #
        # 1. Register a single service and verify that it works.
        #

        client.srp_client_set_host_name('my-host')
        client.srp_client_set_host_address('2001::1')
        client.srp_client_start(server.get_addrs()[0], client.get_srp_server_port())
        client.srp_client_add_service('my-service', '_ipps._tcp', 12345)
        self.simulator.go(2)

        self.check_host_and_service(server, client)

        #
        # 2. Stop the client and wait for the service instance LEASE to expire.
        #

        client.srp_client_stop()
        self.simulator.go(LEASE + 1)

        # The SRP server should remove the host and service but retain their names
        # since the the KEY LEASE hasn't expired yet.
        self.assertEqual(server.srp_server_get_host('my-host')['deleted'], 'true')
        self.assertEqual(server.srp_server_get_service('my-service', '_ipps._tcp')['deleted'], 'true')

        # Start the client again, the same service should be successfully registered.
        client.srp_client_start(server.get_addrs()[0], client.get_srp_server_port())
        self.simulator.go(2)

        self.check_host_and_service(server, client)

        #
        # 3. Stop the client and wait for the KEY LEASE to expire.
        #    The host and service instance should be fully removed by the SRP server.
        #

        client.srp_client_stop()
        self.simulator.go(KEY_LEASE + 1)

        # The host and service are expected to be fully removed.
        self.assertEqual(len(server.srp_server_get_hosts()), 0)
        self.assertEqual(len(server.srp_server_get_services()), 0)

        # Start the client again, the same service should be successfully registered.
        client.srp_client_start(server.get_addrs()[0], client.get_srp_server_port())
        self.simulator.go(2)

        self.check_host_and_service(server, client)

        #
        # 4. Clear the first service, shorten the lease time and register a second service.
        #    Verify that the lease time of the first service is not affected by new SRP
        #    registrations.
        #

        client.srp_client_clear_service('my-service', '_ipps._tcp')
        server.srp_server_set_lease_range(50 * LEASE, 50 * LEASE, 50 * KEY_LEASE, 50 * KEY_LEASE)
        client.srp_client_add_service('my-service2', '_ipps._tcp', 12345)

        # Wait for the first service to expire.
        self.simulator.go(LEASE + 2)
        self.assertEqual(server.srp_server_get_service('my-service', '_ipps._tcp')['deleted'], 'true')

        # Wait for the first service to be fully removed.
        self.simulator.go(KEY_LEASE - LEASE + 2)
        self.assertEqual(len(server.srp_server_get_services()), 1)
        self.assertEqual(len(server.srp_server_get_hosts()), 1)

        #
        # 5. Clear the second service, lengthen the lease time and register a third service.
        #    Verify that the lease time of the second service is not affected by new SRP
        #    registrations.
        #

        client.srp_client_clear_service('my-service2', '_ipps._tcp')
        server.srp_server_set_lease_range(LEASE, LEASE, KEY_LEASE, KEY_LEASE)
        client.srp_client_add_service('my-service3', '_ipps._tcp', 12345)

        # The second service has lease time of 50 * LEASE and should not expire.
        self.simulator.go(LEASE + 2)
        self.assertEqual(server.srp_server_get_service('my-service2', '_ipps._tcp')['deleted'], 'false')

        # The second service has key-lease time of 50 * KEY_LEASE and should not expire.
        self.simulator.go(KEY_LEASE - LEASE + 2)
        self.assertEqual(len(server.srp_server_get_services()), 2)
        self.assertEqual(len(server.srp_server_get_hosts()), 1)

    def check_host_and_service(self, server, client):
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

        # Wait for a KEY LEASE period to make sure that the client has refreshed
        # the host and service instance.
        self.simulator.go(KEY_LEASE + 1)

        server_services = server.srp_server_get_services()
        print(server_services)
        self.assertEqual(len(server_services), 1)
        server_service = server_services[0]

        # Verify that the server accepted the SRP registration and stored
        # the same service resources.
        self.assertEqual(server_service['deleted'], 'false')
        self.assertEqual(server_service['instance'], client_service['instance'])
        self.assertEqual(server_service['name'], client_service['name'])
        self.assertEqual(int(server_service['port']), int(client_service['port']))
        self.assertEqual(int(server_service['priority']), int(client_service['priority']))
        self.assertEqual(int(server_service['weight']), int(client_service['weight']))
        self.assertEqual(server_service['host'], 'my-host')

        server_hosts = server.srp_server_get_hosts()
        print(server_hosts)
        self.assertEqual(len(server_hosts), 1)
        server_host = server_hosts[0]

        self.assertEqual(server_host['deleted'], 'false')
        self.assertEqual(server_host['fullname'], server_service['host_fullname'])
        self.assertEqual(len(server_host['addresses']), 1)
        self.assertEqual(ipaddress.ip_address(server_host['addresses'][0]), ipaddress.ip_address('2001::1'))


if __name__ == '__main__':
    unittest.main()
