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
import typing
import unittest

import thread_cert

# Test description:
#
#   This test verifies DNS-SD server and DNS client behavior
#   (browsing for services and/or subtype services, resolving an
#   address, or resolving a service). It also indirectly covers the SRP
#   client and server behavior and the interactions of DNS-SD server
#   with SRP server).
#
# Topology:
#    Four nodes, leader acting as SRP and DNS-SD servers, with 3 router
#    nodes acting as SRP and DNS clients.
#

SERVER = 1
CLIENT1 = 2
CLIENT2 = 3
CLIENT3 = 4

DOMAIN = 'default.service.arpa.'
SERVICE = '_ipps._tcp'


class TestDnssd(thread_cert.TestCase):
    SUPPORT_NCP = False
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        SERVER: {
            'mode': 'rdn',
        },
        CLIENT1: {
            'mode': 'rdn',
        },
        CLIENT2: {
            'mode': 'rdn',
        },
        CLIENT3: {
            'mode': 'rdn',
        }
    }

    def test(self):
        server = self.nodes[SERVER]
        client1 = self.nodes[CLIENT1]
        client2 = self.nodes[CLIENT2]
        client3 = self.nodes[CLIENT3]

        #---------------------------------------------------------------
        # Start the server & client devices.

        server.start()
        self.simulator.go(5)
        self.assertEqual(server.get_state(), 'leader')
        server.srp_server_set_enabled(True)

        client1.start()
        client2.start()
        client3.start()
        self.simulator.go(5)
        self.assertEqual(client1.get_state(), 'router')
        self.assertEqual(client2.get_state(), 'router')
        self.assertEqual(client3.get_state(), 'router')

        #---------------------------------------------------------------
        # Register services on clients

        client1_addrs = [client1.get_mleid(), client1.get_rloc()]
        client2_addrs = [client2.get_mleid(), client2.get_rloc()]
        client3_addrs = [client3.get_mleid(), client2.get_rloc()]

        self._config_srp_client_services(client1, server, 'ins1', 'host1', 11111, 1, 1, client1_addrs, ",_s1,_s2")
        self._config_srp_client_services(client2, server, 'ins2', 'host2', 22222, 2, 2, client2_addrs)
        self._config_srp_client_services(client3, server, 'ins3', 'host3', 33333, 3, 3, client3_addrs, ",_s1")

        #---------------------------------------------------------------
        # Resolve address (AAAA records)

        answers = client1.dns_resolve(f"host1.{DOMAIN}", server.get_mleid(), 53)
        self.assertEqual(set(ipaddress.IPv6Address(ip) for ip, _ in answers),
                         set(map(ipaddress.IPv6Address, client1_addrs)))

        answers = client1.dns_resolve(f"host2.{DOMAIN}", server.get_mleid(), 53)
        self.assertEqual(set(ipaddress.IPv6Address(ip) for ip, _ in answers),
                         set(map(ipaddress.IPv6Address, client2_addrs)))

        #---------------------------------------------------------------
        # Browsing for services

        instance1_verify_info = {
            'port': 11111,
            'priority': 1,
            'weight': 1,
            'host': 'host1.default.service.arpa.',
            'address': client1_addrs,
            'txt_data': '',
            'srv_ttl': lambda x: x > 0,
            'txt_ttl': lambda x: x > 0,
            'aaaa_ttl': lambda x: x > 0,
        }

        instance2_verify_info = {
            'port': 22222,
            'priority': 2,
            'weight': 2,
            'host': 'host2.default.service.arpa.',
            'address': client2_addrs,
            'txt_data': '',
            'srv_ttl': lambda x: x > 0,
            'txt_ttl': lambda x: x > 0,
            'aaaa_ttl': lambda x: x > 0,
        }

        instance3_verify_info = {
            'port': 33333,
            'priority': 3,
            'weight': 3,
            'host': 'host3.default.service.arpa.',
            'address': client3_addrs,
            'txt_data': '',
            'srv_ttl': lambda x: x > 0,
            'txt_ttl': lambda x: x > 0,
            'aaaa_ttl': lambda x: x > 0,
        }

        instance4_verify_info = {
            'port': 44444,
            'priority': 4,
            'weight': 4,
            'host': 'host3.default.service.arpa.',
            'address': client3_addrs,
            'txt_data': 'KEY=414243',  # KEY=ABC
            'srv_ttl': lambda x: x > 0,
            'txt_ttl': lambda x: x > 0,
            'aaaa_ttl': lambda x: x > 0,
        }

        # Browse for main service
        service_instances = client1.dns_browse(f'{SERVICE}.{DOMAIN}', server.get_mleid(), 53)
        self.assertEqual({'ins1', 'ins2', 'ins3'}, set(service_instances.keys()))
        self._assert_service_instance_equal(service_instances['ins1'], instance1_verify_info)
        self._assert_service_instance_equal(service_instances['ins2'], instance2_verify_info)
        self._assert_service_instance_equal(service_instances['ins3'], instance3_verify_info)

        # Browse for service sub-type _s1.
        service_instances = client1.dns_browse(f'_s1._sub.{SERVICE}.{DOMAIN}', server.get_mleid(), 53)
        self.assertEqual({'ins1', 'ins3'}, set(service_instances.keys()))
        self._assert_service_instance_equal(service_instances['ins1'], instance1_verify_info)

        # Browse for service sub-type _s2.
        service_instances = client1.dns_browse(f'_s2._sub.{SERVICE}.{DOMAIN}', server.get_mleid(), 53)
        self.assertEqual({'ins1'}, set(service_instances.keys()))
        self._assert_service_instance_equal(service_instances['ins1'], instance1_verify_info)

        #---------------------------------------------------------------
        # Resolve service

        service_instance = client1.dns_resolve_service('ins1', f'{SERVICE}.{DOMAIN}', server.get_mleid(), 53)
        self._assert_service_instance_equal(service_instance, instance1_verify_info)

        service_instance = client1.dns_resolve_service('ins2', f'{SERVICE}.{DOMAIN}', server.get_mleid(), 53)
        self._assert_service_instance_equal(service_instance, instance2_verify_info)

        #---------------------------------------------------------------
        # Add another service with TXT entries to the existing host and
        # verify that it is properly merged.

        client3.srp_client_add_service('ins4', SERVICE + ",_s1", 44444, 4, 4, txt_entries=['KEY=ABC'])
        self.simulator.go(5)

        service_instances = client1.dns_browse(f'{SERVICE}.{DOMAIN}', server.get_mleid(), 53)
        self.assertEqual({'ins1', 'ins2', 'ins3', 'ins4'}, set(service_instances.keys()))
        self._assert_service_instance_equal(service_instances['ins1'], instance1_verify_info)
        self._assert_service_instance_equal(service_instances['ins2'], instance2_verify_info)
        self._assert_service_instance_equal(service_instances['ins3'], instance3_verify_info)
        self._assert_service_instance_equal(service_instances['ins4'], instance4_verify_info)

    def _assert_service_instance_equal(self, instance, info):
        for f in ('port', 'priority', 'weight', 'host', 'txt_data'):
            self.assertEqual(instance[f], info[f], instance)

        verify_addresses = info['address']
        if not isinstance(verify_addresses, typing.Collection):
            verify_addresses = [verify_addresses]
        self.assertIn(ipaddress.IPv6Address(instance['address']), map(ipaddress.IPv6Address, verify_addresses),
                      instance)

        for ttl_f in ('srv_ttl', 'txt_ttl', 'aaaa_ttl'):
            check_ttl = info[ttl_f]
            if not callable(check_ttl):
                check_ttl = lambda x: x == check_ttl

            self.assertTrue(check_ttl(instance[ttl_f]), instance)

    def _config_srp_client_services(self,
                                    client,
                                    server,
                                    instancename,
                                    hostname,
                                    port,
                                    priority,
                                    weight,
                                    addrs,
                                    subtypes=''):
        client.netdata_show()
        srp_server_port = client.get_srp_server_port()

        client.srp_client_start(server.get_mleid(), srp_server_port)
        client.srp_client_set_host_name(hostname)
        client.srp_client_set_host_address(*addrs)
        client.srp_client_add_service(instancename, SERVICE + subtypes, port, priority, weight)

        self.simulator.go(5)
        self.assertEqual(client.srp_client_get_host_state(), 'Registered')


if __name__ == '__main__':
    unittest.main()
