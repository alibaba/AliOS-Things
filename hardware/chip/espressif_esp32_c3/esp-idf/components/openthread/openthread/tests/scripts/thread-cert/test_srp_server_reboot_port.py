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
import logging
import unittest

import command
import thread_cert

# Test description:
#   This test verifies when an SRP server restarts, it will listen to a UDP port
#   that wasn't used in the last time.
#
# Topology:
#
#   LEADER (SRP client) -- ROUTER (SRP server)
#

CLIENT = 1
SERVER = 2

REBOOT_TIMES = 25


class SrpServerRebootPort(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        CLIENT: {
            'name': 'SRP_CLIENT',
            'mode': 'rdn',
        },
        SERVER: {
            'name': 'SRP_SERVER',
            'mode': 'rdn',
        },
    }

    def test(self):
        client = self.nodes[CLIENT]
        server = self.nodes[SERVER]

        #
        # 0. Start the server & client devices.
        #

        client.srp_server_set_enabled(False)
        client.start()
        self.simulator.go(5)
        self.assertEqual(client.get_state(), 'leader')

        server.srp_server_set_enabled(True)
        server.start()
        self.simulator.go(5)
        self.assertEqual(server.get_state(), 'router')

        #
        # 1. Enable auto start mode on client and check that server is used.
        #

        self.assertEqual(client.srp_client_get_state(), 'Disabled')
        client.srp_client_enable_auto_start_mode()
        self.assertEqual(client.srp_client_get_auto_start_mode(), 'Enabled')
        self.simulator.go(2)
        self.assertEqual(client.srp_client_get_state(), 'Enabled')
        self.assertTrue(server.has_ipaddr(client.srp_client_get_server_address()))

        #
        # 2. Reboot the server without any service registered. The server should
        # listen to the same port after the reboot.
        #
        old_port = server.get_srp_server_port()
        server.srp_server_set_enabled(False)
        self.simulator.go(5)
        server.srp_server_set_enabled(True)
        self.simulator.go(5)
        self.assertEqual(old_port, server.get_srp_server_port())

        #
        # 3. Register a service
        #
        client.srp_client_set_host_name('my-host')
        client.srp_client_set_host_address('2001::1')
        client.srp_client_add_service('my-service', '_ipps._tcp', 12345, 0, 0, ['abc', 'def=', 'xyz=XYZ'])
        self.simulator.go(5)
        self.check_host_and_service(server, client, '2001::1')

        ports = [server.get_srp_server_port()]

        # Reboot the SRP server several times
        for i in range(REBOOT_TIMES):
            #
            # 4. Disable server and check client is stopped/disabled.
            #
            old_port = server.get_srp_server_port()
            server.srp_server_set_enabled(False)
            self.simulator.go(5)

            #
            # 5. Enable server and check client starts again. Verify that the
            # server is using a different port, and the service have been
            # re-registered.
            #
            server.srp_server_set_enabled(True)
            self.simulator.go(5)
            self.assertEqual(client.srp_client_get_state(), 'Enabled')
            self.assertEqual(client.srp_client_get_server_address(), server.get_mleid())
            self.assertNotEqual(old_port, server.get_srp_server_port())
            self.check_host_and_service(server, client, '2001::1')
            ports.append(server.get_srp_server_port())
        logging.info(f'ports = {ports}')

    def check_host_and_service(self, server, client, host_addr):
        # Check that we have properly registered host and service instance.
        # Originally used in test_srp_register_single_service.py.

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
