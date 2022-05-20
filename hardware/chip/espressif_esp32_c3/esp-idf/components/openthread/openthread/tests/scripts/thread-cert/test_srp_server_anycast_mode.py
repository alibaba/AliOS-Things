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

import config
import thread_cert

# Test description:
#
#   This test covers the SRP server's behavior using different address
#   modes (unicast or anycast). The address mode indicates how the SRP
#   server determines its address and port and how this info is
#   published in the Thread Network Data. When using anycast address
#   mode, the SRP server and DNS server/resolver will both listen on port
#   53 and they re-use the same socket. This test verifies the behavior of
#   both modules in such a situation.
#
#
# Topology:
#
#   One leader and two router nodes, all connected. The leader acts as SRP
#   server and DNS resolver. One router acts as SRP client, and the
#   other acts as DNS client browsing for registered service names.
#

SERVER = 1
CLIENT = 2
BROWSER = 3

DOMAIN = 'default.service.arpa.'
HOST = 'host'
INSTANCE = 'ins1'
SERVICE = '_srv._udp'
SERVICE_PORT = 1313

SRP_SERVER_ANYCAST_PORT = 53
SRP_SERVER_ANYCAST_SEQ_NUM = 17
DNS_RESOLVER_PORT = 53

THREAD_ENTERPRISE_NUMBER = 44970
ANYCAST_SERVICE_NUM = 0x5c
UNICAST_SERVICE_NUM = 0x5d


class TestSrpServerAnycastMode(thread_cert.TestCase):
    SUPPORT_NCP = False
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        SERVER: {
            'mode': 'rdn',
        },
        CLIENT: {
            'mode': 'rdn',
        },
        BROWSER: {
            'mode': 'rdn',
        },
    }

    def test(self):
        server = self.nodes[SERVER]
        client = self.nodes[CLIENT]
        browser = self.nodes[BROWSER]

        #-------------------------------------------------------------------
        # Form the network.

        server.start()
        self.simulator.go(5)
        self.assertEqual(server.get_state(), 'leader')

        client.start()
        browser.start()
        self.simulator.go(5)
        self.assertEqual(client.get_state(), 'router')
        self.assertEqual(browser.get_state(), 'router')

        #-------------------------------------------------------------------
        # Go through the entire test twice, first time using anycast address
        # mode and second time using unicast address mode.

        for addr_mode in ['anycast', 'unicast']:

            #---------------------------------------------------------------
            # Set the SRP server address mode and start the SRP server.

            server.srp_server_set_addr_mode(addr_mode)

            if addr_mode == 'anycast':
                server.srp_server_set_anycast_seq_num(SRP_SERVER_ANYCAST_SEQ_NUM)
                self.assertEqual(server.srp_server_get_anycast_seq_num(), SRP_SERVER_ANYCAST_SEQ_NUM)

            self.assertEqual(server.srp_server_get_addr_mode(), addr_mode)
            server.srp_server_set_enabled(True)
            self.simulator.go(5)

            #---------------------------------------------------------------
            # Verify the published SRP server info in the Network Data.

            netdata_services = client.get_services()
            self.assertEqual(len(netdata_services), 1)
            netdata_service = netdata_services[0]

            self.assertEqual(int(netdata_service[0]), THREAD_ENTERPRISE_NUMBER)
            data = bytes.fromhex(netdata_service[1])
            self.assertEqual(netdata_service[3], 's')

            if addr_mode == 'anycast':
                self.assertTrue(len(data) >= 2)
                self.assertEqual(data[0], ANYCAST_SERVICE_NUM)
                self.assertEqual(data[1], SRP_SERVER_ANYCAST_SEQ_NUM)
            else:
                self.assertTrue(len(data) >= 1)
                self.assertEqual(data[0], UNICAST_SERVICE_NUM)
                self.assertEqual(netdata_service[3], 's')

            #---------------------------------------------------------------
            # Enable auto-start on SRP client. Verify that it does find the
            # server and uses the proper address and port number.

            client.srp_client_enable_auto_start_mode()
            self.simulator.go(5)

            if addr_mode == 'anycast':
                server_alocs = server.get_ip6_address(config.ADDRESS_TYPE.ALOC)
                self.assertEqual(client.srp_client_get_state(), 'Enabled')
                self.assertIn(client.srp_client_get_server_address(), server_alocs)
                self.assertEqual(client.srp_client_get_server_port(), SRP_SERVER_ANYCAST_PORT)
            else:
                self.assertIn(client.srp_client_get_server_address(), server.get_mleid())
                self.assertEqual(client.srp_client_get_server_port(), server.get_srp_server_port())

            #---------------------------------------------------------------
            # Add a service on the SRP client and verify its successful
            # registration with SRP server.

            client.srp_client_set_host_name(HOST)
            client.srp_client_set_host_address(client.get_mleid())
            client.srp_client_add_service(INSTANCE, SERVICE, SERVICE_PORT)
            self.simulator.go(5)

            client_services = client.srp_client_get_services()
            self.assertEqual(len(client_services), 1)
            client_service = client_services[0]
            self.assertEqual(client_service['instance'], INSTANCE)
            self.assertEqual(client_service['name'], SERVICE)
            self.assertEqual(int(client_service['port']), SERVICE_PORT)
            self.assertEqual(client_service['state'], 'Registered')

            #---------------------------------------------------------------
            # Browse for a matching service name and verify that the registered
            # service is successfully found.

            service_instances = browser.dns_browse(f'{SERVICE}.{DOMAIN}', server.get_mleid(), DNS_RESOLVER_PORT)
            self.assertEqual({INSTANCE}, set(service_instances.keys()))

            service_instance = service_instances[INSTANCE]
            self.assertEqual(service_instance['host'], f'{HOST}.{DOMAIN}')
            self.assertEqual(int(service_instance['port']), SERVICE_PORT)
            self.assertEqual(service_instance['address'], client.get_mleid())

            #---------------------------------------------------------------
            # Stop SRP client and server and clear host (and service) on the
            # client.

            client.srp_client_clear_host()
            client.srp_client_stop()
            server.srp_server_set_enabled(False)


if __name__ == '__main__':
    unittest.main()
