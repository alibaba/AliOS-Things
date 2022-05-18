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
#   This test verifies if the SRP server can handle name conflicts correctly.
#
# Topology:
#            LEADER (SRP server)
#           /      \
#          /        \
#         /          \
#     ROUTER1      ROUTER2
#

SERVER = 1
CLIENT1 = 2
CLIENT2 = 3


class SrpNameConflicts(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        SERVER: {
            'name': 'SRP_SERVER',
            'networkkey': '00112233445566778899aabbccddeeff',
            'mode': 'rdn',
        },
        CLIENT1: {
            'name': 'SRP_CLIENT1',
            'networkkey': '00112233445566778899aabbccddeeff',
            'mode': 'rdn',
        },
        CLIENT2: {
            'name': 'SRP_CLIENT2',
            'networkkey': '00112233445566778899aabbccddeeff',
            'mode': 'rdn',
        },
    }

    def test(self):
        server = self.nodes[SERVER]
        client_1 = self.nodes[CLIENT1]
        client_2 = self.nodes[CLIENT2]

        #
        # 0. Start the server & client devices.
        #

        server.srp_server_set_enabled(True)
        server.start()
        self.simulator.go(5)
        self.assertEqual(server.get_state(), 'leader')
        self.simulator.go(5)

        client_1.srp_server_set_enabled(False)
        client_1.start()
        self.simulator.go(5)
        self.assertEqual(client_1.get_state(), 'router')

        client_2.srp_server_set_enabled(False)
        client_2.start()
        self.simulator.go(5)
        self.assertEqual(client_2.get_state(), 'router')

        #
        # 1. Register a single service and verify that it works.
        #

        client_1.srp_client_set_host_name('my-host-1')
        client_1.srp_client_set_host_address('2001::1')
        client_1.srp_client_start(server.get_addrs()[0], client_1.get_srp_server_port())
        client_1.srp_client_add_service('my-service-1', '_ipps._tcp', 12345)
        self.simulator.go(2)

        # Verify that the client possesses correct service resources.
        client_1_service = client_1.srp_client_get_services()[0]
        self.assertEqual(client_1_service['instance'], 'my-service-1')
        self.assertEqual(client_1_service['name'], '_ipps._tcp')
        self.assertEqual(int(client_1_service['port']), 12345)
        self.assertEqual(int(client_1_service['priority']), 0)
        self.assertEqual(int(client_1_service['weight']), 0)

        # Verify that the client receives a SUCCESS response for the server.
        self.assertEqual(client_1_service['state'], 'Registered')

        # Verify that the server accepts the SRP registration and stored
        # the same service resources.
        server_service = server.srp_server_get_services()[0]
        self.assertEqual(server_service['deleted'], 'false')
        self.assertEqual(server_service['instance'], client_1_service['instance'])
        self.assertEqual(server_service['name'], client_1_service['name'])
        self.assertEqual(int(server_service['port']), int(client_1_service['port']))
        self.assertEqual(int(server_service['priority']), int(client_1_service['priority']))
        self.assertEqual(int(server_service['weight']), int(client_1_service['weight']))
        self.assertEqual(server_service['host'], 'my-host-1')

        server_host = server.srp_server_get_hosts()[0]
        self.assertEqual(server_host['deleted'], 'false')
        self.assertEqual(server_host['fullname'], server_service['host_fullname'])
        self.assertEqual(len(server_host['addresses']), 1)
        self.assertEqual(ipaddress.ip_address(server_host['addresses'][0]), ipaddress.ip_address('2001::1'))

        #
        # 2. Register with the same host name from the second client and it should fail.
        #

        client_2.srp_client_set_host_name('my-host-1')
        client_2.srp_client_set_host_address('2001::2')
        client_2.srp_client_start(server.get_addrs()[0], client_2.get_srp_server_port())
        client_2.srp_client_add_service('my-service-2', '_ipps._tcp', 12345)
        self.simulator.go(2)

        # It is expected that the registration will be rejected.
        client_2_service = client_2.srp_client_get_services()[0]
        self.assertNotEqual(client_2_service['state'], 'Registered')
        self.assertNotEqual(client_2.srp_client_get_host_state(), 'Registered')

        self.assertEqual(len(server.srp_server_get_services()), 1)
        self.assertEqual(len(server.srp_server_get_hosts()), 1)

        client_2.srp_client_clear_host()
        client_2.srp_client_stop()

        #
        # 3. Register with the same service name from the second client and it should fail.
        #

        client_2.srp_client_set_host_name('my-host-2')
        client_2.srp_client_set_host_address('2001::2')
        client_2.srp_client_start(server.get_addrs()[0], client_2.get_srp_server_port())
        client_2.srp_client_add_service('my-service-1', '_ipps._tcp', 12345)
        self.simulator.go(2)

        # It is expected that the registration will be rejected.
        client_2_service = client_2.srp_client_get_services()[0]
        self.assertNotEqual(client_2_service['state'], 'Registered')
        self.assertNotEqual(client_2.srp_client_get_host_state(), 'Registered')

        self.assertEqual(len(server.srp_server_get_services()), 1)
        self.assertEqual(len(server.srp_server_get_hosts()), 1)

        client_2.srp_client_clear_host()
        client_2.srp_client_stop()

        #
        # 4. Register with different host & service instance name, it should succeed.
        #

        client_2.srp_client_set_host_name('my-host-2')
        client_2.srp_client_set_host_address('2001::2')
        client_2.srp_client_start(server.get_addrs()[0], client_2.get_srp_server_port())
        client_2.srp_client_add_service('my-service-2', '_ipps._tcp', 12345)
        self.simulator.go(2)

        # It is expected that the registration will be accepted.
        client_2_service = client_2.srp_client_get_services()[0]
        self.assertEqual(client_2_service['state'], 'Registered')
        self.assertEqual(client_2.srp_client_get_host_state(), 'Registered')

        self.assertEqual(len(server.srp_server_get_services()), 2)
        self.assertEqual(len(server.srp_server_get_hosts()), 2)
        self.assertEqual(server.srp_server_get_host('my-host-2')['deleted'], 'false')
        self.assertEqual(server.srp_server_get_service('my-service-2', '_ipps._tcp')['deleted'], 'false')

        # Remove the host and all services registered on the SRP server.
        client_2.srp_client_remove_host(remove_key=True)
        self.simulator.go(2)

        client_2.srp_client_clear_host()
        client_2.srp_client_stop()

        #
        # 5. Register with the same service instance name before its KEY LEASE expires,
        #    it is expected to fail.
        #

        # Remove the service instance from SRP server but retains its name.
        client_1.srp_client_remove_service('my-service-1', '_ipps._tcp')
        self.simulator.go(2)

        client_2.srp_client_set_host_name('my-host-2')
        client_2.srp_client_set_host_address('2001::2')
        client_2.srp_client_start(server.get_addrs()[0], client_2.get_srp_server_port())
        client_2.srp_client_add_service('my-service-1', '_ipps._tcp', 12345)
        self.simulator.go(2)

        # It is expected that the registration will be rejected.
        client_2_service = client_2.srp_client_get_services()[0]
        self.assertNotEqual(client_2_service['state'], 'Registered')
        self.assertNotEqual(client_2.srp_client_get_host_state(), 'Registered')

        # The service 'my-service-1' is removed but its name is retained.
        # This is why we can see the service record on the SRP server.
        self.assertEqual(len(server.srp_server_get_services()), 1)
        self.assertEqual(len(server.srp_server_get_hosts()), 1)
        self.assertEqual(server.srp_server_get_host('my-host-1')['deleted'], 'false')
        self.assertEqual(server.srp_server_get_service('my-service-1', '_ipps._tcp')['deleted'], 'true')

        client_2.srp_client_clear_host()
        client_2.srp_client_stop()

        #
        # 6. The service instance name can be re-used by another client when
        #    the service has been permanently removed (the KEY resource is
        #    removed) from the host.
        #

        # Client 1 adds back the service, it should success.
        client_1.srp_client_add_service('my-service-1', '_ipps._tcp', 12345)
        self.simulator.go(2)
        self.assertEqual(len(server.srp_server_get_services()), 1)
        self.assertEqual(len(server.srp_server_get_hosts()), 1)
        self.assertEqual(server.srp_server_get_host('my-host-1')['deleted'], 'false')
        self.assertEqual(server.srp_server_get_service('my-service-1', '_ipps._tcp')['deleted'], 'false')

        # Permanently removes the service instance.
        client_1.srp_client_remove_host(remove_key=True)
        self.simulator.go(2)
        self.assertEqual(len(server.srp_server_get_services()), 0)
        self.assertEqual(len(server.srp_server_get_hosts()), 0)

        # Client 2 registers the same host & service instance name with Client 1.
        client_2.srp_client_stop()
        client_2.srp_client_clear_host()
        client_2.srp_client_set_host_name('my-host-1')
        client_2.srp_client_set_host_address('2001::2')
        client_2.srp_client_start(server.get_addrs()[0], client_2.get_srp_server_port())
        client_2.srp_client_add_service('my-service-1', '_ipps._tcp', 12345)
        self.simulator.go(2)

        # It is expected that client 2 will success because those names has been
        # released by client 1.
        self.assertEqual(len(server.srp_server_get_services()), 1)
        self.assertEqual(len(server.srp_server_get_hosts()), 1)
        self.assertEqual(server.srp_server_get_host('my-host-1')['deleted'], 'false')
        self.assertEqual(server.srp_server_get_service('my-service-1', '_ipps._tcp')['deleted'], 'false')


if __name__ == '__main__':
    unittest.main()
