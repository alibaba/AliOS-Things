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
import ipaddress
import logging
import unittest

import config
import thread_cert

# Test description:
#   This test verifies the basic functionality of advertising proxy.
#
# Topology:
#    ----------------(eth)--------------------
#           |                   |
#          BR (Leader)    HOST (mDNS Browser)
#           |
#        ROUTER
#

BR = 1
ROUTER = 2
HOST = 3
LEASE = 10  # Seconds
KEY_LEASE = 20  # Seconds


class SingleHostAndService(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        BR: {
            'name': 'BR_1',
            'allowlist': [ROUTER],
            'is_otbr': True,
            'version': '1.2',
        },
        ROUTER: {
            'name': 'Router_1',
            'allowlist': [BR],
            'version': '1.2',
        },
        HOST: {
            'name': 'Host',
            'is_host': True
        },
    }

    def test(self):
        host = self.nodes[HOST]
        server = self.nodes[BR]
        client = self.nodes[ROUTER]

        server.srp_server_set_enabled(False)
        host.start(start_radvd=False)
        self.simulator.go(5)

        self.assertEqual(server.srp_server_get_state(), 'disabled')
        server.srp_server_set_enabled(True)
        server.srp_server_set_lease_range(LEASE, LEASE, KEY_LEASE, KEY_LEASE)
        server.start()
        self.simulator.go(10)
        self.assertEqual('leader', server.get_state())
        self.assertEqual(server.srp_server_get_state(), 'running')

        client.start()
        self.simulator.go(5)
        self.assertEqual('router', client.get_state())

        #
        # 1. Register a single service.
        #

        client.srp_client_set_host_name('my-host')
        client.srp_client_set_host_address('2001::1')
        client.srp_client_add_service('my-service', '_ipps._tcp', 12345)
        client.srp_client_add_service('my-service-1', '_ipps._tcp', 12345)
        client.srp_client_enable_auto_start_mode()
        self.simulator.go(2)

        self.check_host_and_service(server, client, '2001::1', 'my-service')
        self.check_host_and_service(server, client, '2001::1', 'my-service-1')

        #
        # 2. Discover the service by the HOST on the ethernet. This makes sure
        #    the Advertising Proxy multicasts the same service on ethernet.
        #

        self.host_check_mdns_service(host, '2001::1', 'my-service')
        self.host_check_mdns_service(host, '2001::1', 'my-service-1')

        #
        # 3. Check if the Advertising Proxy removes the service from ethernet
        #    when the SRP client removes it.
        #

        client.srp_client_remove_host()
        self.simulator.go(2)

        self.assertIsNone(host.discover_mdns_service('my-service', '_ipps._tcp', 'my-host'))
        self.assertIsNone(host.discover_mdns_service('my-service-1', '_ipps._tcp', 'my-host'))

        #
        # 4. Check if we can discover the mDNS service again when re-registering the
        #    service from the SRP client.
        #

        client.srp_client_set_host_name('my-host')
        client.srp_client_set_host_address('2001::1')
        client.srp_client_add_service('my-service', '_ipps._tcp', 12345)
        client.srp_client_add_service('my-service-1', '_ipps._tcp', 12345)
        self.simulator.go(2)

        self.check_host_and_service(server, client, '2001::1', 'my-service')
        self.check_host_and_service(server, client, '2001::1', 'my-service-1')
        self.host_check_mdns_service(host, '2001::1', 'my-service')
        self.host_check_mdns_service(host, '2001::1', 'my-service-1')

        #
        # 5. Update the SRP host address and make sure the Advertising Proxy
        #    will follow it.
        #

        client.srp_client_set_host_address('2001::2')
        self.simulator.go(8)

        self.check_host_and_service(server, client, '2001::2', 'my-service')
        self.check_host_and_service(server, client, '2001::2', 'my-service-1')
        self.host_check_mdns_service(host, '2001::2', 'my-service')
        self.host_check_mdns_service(host, '2001::2', 'my-service-1')

        #
        # 6. Check if the service is removed by the Advertising Proxy when the SRP server is stopped.
        #

        server.srp_server_set_enabled(False)
        self.simulator.go(5)

        self.assertEqual(len(server.srp_server_get_hosts()), 0)
        self.assertEqual(len(server.srp_server_get_services()), 0)
        self.assertIsNone(host.discover_mdns_service('my-service', '_ipps._tcp', 'my-host'))
        self.assertIsNone(host.discover_mdns_service('my-service-1', '_ipps._tcp', 'my-host'))

        server.srp_server_set_enabled(True)
        self.simulator.go(LEASE)

        self.check_host_and_service(server, client, '2001::2', 'my-service')
        self.check_host_and_service(server, client, '2001::2', 'my-service-1')
        self.host_check_mdns_service(host, '2001::2', 'my-service')
        self.host_check_mdns_service(host, '2001::2', 'my-service-1')

        #
        # 7. Remove a single service and verify that the remaining one can still
        #    be discovered.
        #

        client.srp_client_remove_service('my-service-1', '_ipps._tcp')
        self.simulator.go(5)

        self.check_host_and_service(server, client, '2001::2', 'my-service')
        self.host_check_mdns_service(host, '2001::2', 'my-service')
        self.assertIsNone(host.discover_mdns_service('my-service-1', '_ipps._tcp', 'my-host'))

        #
        # 8. Update both the host and the service in a loop and make sure the
        #    Advertising Proxy can follow.
        #

        for host_address_suffix, service_port in ((1, 12341), (2, 12342), (3, 12343), (2, 12345)):
            host_address = f'2001::{host_address_suffix}'
            client.srp_client_clear_service('my-service', '_ipps._tcp')
            client.srp_client_set_host_address(host_address)
            client.srp_client_add_service('my-service', '_ipps._tcp', service_port)
            self.simulator.go(5)

            self.check_host_and_service(server, client, host_address, 'my-service', service_port)
            self.host_check_mdns_service(host, host_address, 'my-service', service_port)

        #
        # 9. Check if the expired service is removed by the Advertising Proxy.
        #

        client.srp_client_stop()
        self.simulator.go(LEASE + 2)

        self.assertIsNone(host.discover_mdns_service('my-service', '_ipps._tcp', 'my-host'))
        self.assertIsNone(host.discover_mdns_service('my-service-1', '_ipps._tcp', 'my-host'))

    def host_check_mdns_service(self, host, host_addr, service_instance, service_port=12345):
        service = host.discover_mdns_service(service_instance, '_ipps._tcp', 'my-host')
        self.assertIsNotNone(service)
        self.assertEqual(service['instance'], service_instance)
        self.assertEqual(service['name'], '_ipps._tcp')
        self.assertEqual(service['port'], service_port)
        self.assertEqual(service['priority'], 0)
        self.assertEqual(service['weight'], 0)
        self.assertEqual(service['host'], 'my-host')
        self.assertEqual(ipaddress.ip_address(service['addresses'][0]), ipaddress.ip_address(host_addr))
        self.assertEqual(len(service['addresses']), 1)

    def check_host_and_service(self, server, client, host_addr, service_instance, service_port=12345):
        """Check that we have properly registered host and service instance.
        """

        client_services = client.srp_client_get_services()
        print(client_services)
        client_services = [service for service in client_services if service['instance'] == service_instance]
        self.assertEqual(len(client_services), 1)
        client_service = client_services[0]

        # Verify that the client possesses correct service resources.
        self.assertEqual(client_service['instance'], service_instance)
        self.assertEqual(client_service['name'], '_ipps._tcp')
        self.assertEqual(int(client_service['port']), service_port)
        self.assertEqual(int(client_service['priority']), 0)
        self.assertEqual(int(client_service['weight']), 0)

        # Verify that the client received a SUCCESS response for the server.
        self.assertEqual(client_service['state'], 'Registered')

        server_services = server.srp_server_get_services()
        print(server_services)
        server_services = [service for service in server_services if service['instance'] == service_instance]
        self.assertEqual(len(server_services), 1)
        server_service = server_services[0]

        # Verify that the server accepted the SRP registration and stores
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
        self.assertEqual(ipaddress.ip_address(server_host['addresses'][0]), ipaddress.ip_address(host_addr))


if __name__ == '__main__':
    unittest.main()
