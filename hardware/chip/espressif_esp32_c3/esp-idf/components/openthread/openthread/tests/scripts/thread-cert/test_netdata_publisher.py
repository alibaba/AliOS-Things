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
#   This test verifies network data publisher behavior with DNS/SRP service entries and on-mesh prefix and external
#   route entries.
#
# Topology:
#
#   1 leader, 5 routers and 5 end-devices all connected.
#

LEADER = 1
ROUTER1 = 2
ROUTER2 = 3
ROUTER3 = 4
ROUTER4 = 5
ROUTER5 = 6
END_DEV1 = 7
END_DEV2 = 8
END_DEV3 = 9
END_DEV4 = 10
END_DEV5 = 11

WAIT_TIME = 55

ON_MESH_PREFIX = 'fd00:1234::/64'
ON_MESH_FLAGS = 'paso'

EXTERNAL_ROUTE = 'fd00:abce::/64'
EXTERNAL_FLAGS = 's'

ANYCAST_SEQ_NUM = 4

DNSSRP_ADDRESS = 'fd00::cdef'
DNSSRP_PORT = 49152

# The desired number of entries (based on related config).
DESIRED_NUM_DNSSRP_ANYCAST = 8
DESIRED_NUM_DNSSRP_UNCIAST = 2
DESIRED_NUM_ON_MESH_PREFIX = 3
DESIRED_NUM_EXTERNAL_ROUTE = 10

THREAD_ENTERPRISE_NUMBER = 44970
ANYCAST_SERVICE_NUM = 0x5c
UNICAST_SERVICE_NUM = 0x5d


class NetDataPublisher(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
        },
        ROUTER1: {
            'name': 'ROUTER1',
            'mode': 'rdn',
        },
        ROUTER2: {
            'name': 'ROUTER2',
            'mode': 'rdn',
        },
        ROUTER3: {
            'name': 'ROUTER3',
            'mode': 'rdn',
        },
        ROUTER4: {
            'name': 'ROUTER4',
            'mode': 'rdn',
        },
        ROUTER5: {
            'name': 'ROUTER5',
            'mode': 'rdn',
        },
        END_DEV1: {
            'name': 'END_DEV1',
            'mode': 'rn',
        },
        END_DEV2: {
            'name': 'END_DEV2',
            'mode': 'rn',
        },
        END_DEV3: {
            'name': 'END_DEV3',
            'mode': 'rn',
        },
        END_DEV4: {
            'name': 'END_DEV4',
            'mode': 'rn',
        },
        END_DEV5: {
            'name': 'END_DEV5',
            'mode': 'rn',
        },
    }

    def verify_anycast_service(self, service):
        # Verify the data in a single anycast `service` from `get_services()`
        # Example of `service`: ['44970', '5c04', '', 's', 'bc00']
        self.assertEqual(int(service[0]), THREAD_ENTERPRISE_NUMBER)
        # Check service data
        service_data = bytes.fromhex(service[1])
        self.assertTrue(len(service_data) >= 2)
        self.assertEqual(service_data[0], ANYCAST_SERVICE_NUM)
        self.assertEqual(service_data[1], int(ANYCAST_SEQ_NUM))
        # Verify that it stable
        self.assertEqual(service[3], 's')

    def verify_anycast_services(self, services):
        # Verify a list of anycast `services` from `get_services()`
        for service in services:
            self.verify_anycast_service(service)

    def verify_unicast_service(self, service):
        # Verify the data in a single unicast `service` from `get_services()`
        # Example of `service`: ['44970', '5d', 'fd000db800000000c6b0e5ee81f940e8223d', 's', '7000']
        self.assertEqual(int(service[0]), THREAD_ENTERPRISE_NUMBER)
        # Check service data
        service_data = bytes.fromhex(service[1])
        self.assertTrue(len(service_data) >= 1)
        self.assertEqual(service_data[0], UNICAST_SERVICE_NUM)
        # Verify that it stable
        self.assertEqual(service[3], 's')

    def verify_unicast_services(self, services):
        # Verify a list of unicast `services` from `get_services()`
        for service in services:
            self.verify_unicast_service(service)

    def check_num_of_prefixes(self, prefixes, num_low, num_med, num_high):
        # Check and validate the prefix entries in network data (from
        # `prefixes`) based on number of published prefix entries at
        # different preference levels given by `num_low`, `num_med`,
        # `num_high`. Prefixes is a list of the format
        # 'fd00:1234:0:0::/64 paos low a802'.
        self.assertEqual(len(prefixes), min(num_high + num_med + num_low, DESIRED_NUM_ON_MESH_PREFIX))
        prfs = [prefix.split(' ')[2] for prefix in prefixes]
        self.assertEqual(prfs.count('high'), min(num_high, DESIRED_NUM_ON_MESH_PREFIX))
        self.assertEqual(prfs.count('med'), min(num_med, max(0, DESIRED_NUM_ON_MESH_PREFIX - num_high)))
        self.assertEqual(prfs.count('low'), min(num_low, max(0, DESIRED_NUM_ON_MESH_PREFIX - num_high - num_med)))

    def check_num_of_routes(self, routes, num_low, num_med, num_high):
        # Check and validate the prefix entries in network data (from
        # `routes`) based on number of published prefix entries at
        # different preference levels given by `num_low`, `num_med`,
        # `num_high`. Prefixes is a list of the format
        # 'fd00:abce:0:0::/64 s med 6c01'.
        self.assertEqual(len(routes), min(num_high + num_med + num_low, DESIRED_NUM_EXTERNAL_ROUTE))
        prfs = [route.split(' ')[2] for route in routes]
        self.assertEqual(prfs.count('high'), min(num_high, DESIRED_NUM_EXTERNAL_ROUTE))
        self.assertEqual(prfs.count('med'), min(num_med, max(0, DESIRED_NUM_EXTERNAL_ROUTE - num_high)))
        self.assertEqual(prfs.count('low'), min(num_low, max(0, DESIRED_NUM_EXTERNAL_ROUTE - num_high - num_med)))

    def test(self):
        leader = self.nodes[LEADER]
        router1 = self.nodes[ROUTER1]
        router2 = self.nodes[ROUTER2]
        router3 = self.nodes[ROUTER3]
        router4 = self.nodes[ROUTER4]
        router5 = self.nodes[ROUTER5]
        end_dev1 = self.nodes[END_DEV1]
        end_dev2 = self.nodes[END_DEV2]
        end_dev3 = self.nodes[END_DEV3]
        end_dev4 = self.nodes[END_DEV4]
        end_dev5 = self.nodes[END_DEV5]

        nodes = self.nodes.values()
        routers = [router1, router2, router3, router4, router5]
        end_devs = [end_dev1, end_dev2, end_dev3, end_dev4, end_dev5]

        # Start the nodes

        leader.start()
        self.simulator.go(5)
        self.assertEqual(leader.get_state(), 'leader')

        for router in routers:
            router.start()
            self.simulator.go(5)
            self.assertEqual(router.get_state(), 'router')

        for end_dev in end_devs:
            end_dev.start()
            self.simulator.go(5)
            self.assertEqual(end_dev.get_state(), 'child')

        #---------------------------------------------------------------------------------
        # DNS/SRP anycast entries

        # Publish DNS/SRP anycast on leader and all routers (6 nodes).

        leader.netdata_publish_dnssrp_anycast(ANYCAST_SEQ_NUM)
        for node in routers:
            node.netdata_publish_dnssrp_anycast(ANYCAST_SEQ_NUM)
        self.simulator.go(WAIT_TIME)

        # Check all entries are present in the network data

        services = leader.get_services()
        self.assertEqual(len(services), min(1 + len(routers), DESIRED_NUM_DNSSRP_ANYCAST))
        self.verify_anycast_services(services)

        # Publish same entry on all end-devices (5 nodes).

        for node in end_devs:
            node.netdata_publish_dnssrp_anycast(ANYCAST_SEQ_NUM)
            print(node.name)
        self.simulator.go(WAIT_TIME)

        # Check number of entries in the network data is limited to
        # the desired number (8 entries).

        services = leader.get_services()
        self.assertEqual(len(leader.get_services()), min(len(nodes), DESIRED_NUM_DNSSRP_ANYCAST))
        self.verify_anycast_services(services)

        # Unpublish the entry from nodes one by one starting from leader
        # and check that number of entries is correct in each step.

        num = len(nodes)
        for node in nodes:
            node.netdata_unpublish_dnssrp()
            self.simulator.go(WAIT_TIME)
            num -= 1
            services = leader.get_services()
            self.assertEqual(len(services), min(num, DESIRED_NUM_DNSSRP_ANYCAST))
            self.verify_anycast_services(services)

        #---------------------------------------------------------------------------------
        # DNS/SRP unicast entries

        # Publish DNS/SRP unicast address on all routers, first using
        # MLE-EID address, then change to use specific address. Verify
        # that number of entries in network data is correct in each step
        # and that entries are switched correctly.
        num = 0
        for node in routers:
            node.netdata_publish_dnssrp_unicast_mleid(DNSSRP_PORT)
            self.simulator.go(WAIT_TIME)
            num += 1
            services = leader.get_services()
            self.assertEqual(len(services), min(num, DESIRED_NUM_DNSSRP_UNCIAST))
            self.verify_unicast_services(services)

        for node in routers:
            node.netdata_publish_dnssrp_unicast(DNSSRP_ADDRESS, DNSSRP_PORT)
            self.simulator.go(WAIT_TIME)
            services = leader.get_services()
            self.assertEqual(len(services), min(num, DESIRED_NUM_DNSSRP_UNCIAST))
            self.verify_unicast_services(services)

        for node in routers:
            node.srp_server_set_enabled(True)
            self.simulator.go(WAIT_TIME)
        self.assertEqual(sum(node.srp_server_get_state() == 'running' for node in routers),
                         min(len(routers), DESIRED_NUM_DNSSRP_UNCIAST))
        self.assertEqual(sum(node.srp_server_get_state() == 'stopped' for node in routers),
                         max(len(routers) - DESIRED_NUM_DNSSRP_UNCIAST, 0))

        for node in routers:
            node.netdata_unpublish_dnssrp()
            self.simulator.go(WAIT_TIME)
            num -= 1
            services = leader.get_services()
            self.assertEqual(len(services), min(num, DESIRED_NUM_DNSSRP_UNCIAST))
            self.verify_unicast_services(services)
        for node in routers:
            node.srp_server_set_enabled(False)
            self.assertEqual(node.srp_server_get_state(), 'disabled')

        #---------------------------------------------------------------------------------
        # DNS/SRP entries: Verify publisher preference when removing
        # entries.
        #
        # Publish DNS/SRP anycast on 8 nodes: leader, router1,
        # router2, and all 5 end-devices. Afterwards, manually add
        # the same service entry in Network Data on router3, router4,
        # and router5 and at each step check that entry from one of
        # the end-devices is removed (publisher prefers
        # entries from routers over the ones from end-devices).

        num = 0
        test_routers = [leader, router1, router2]
        for node in test_routers + end_devs:
            node.netdata_publish_dnssrp_anycast(ANYCAST_SEQ_NUM)
            self.simulator.go(WAIT_TIME)
            num += 1
            services = leader.get_services()
            self.assertEqual(len(services), num)
            self.verify_anycast_services(services)

        self.assertEqual(num, DESIRED_NUM_DNSSRP_ANYCAST)

        service_data = '%02x%02x' % (ANYCAST_SERVICE_NUM, int(ANYCAST_SEQ_NUM))
        for node in [router3, router4, router5]:
            node.add_service(str(THREAD_ENTERPRISE_NUMBER), service_data, '00')
            node.register_netdata()
            self.simulator.go(WAIT_TIME)

            services = leader.get_services()
            self.assertEqual(len(services), num)
            self.verify_anycast_services(services)

            service_rlocs = [int(service[4], 16) for service in services]
            test_routers.append(node)

            for router in test_routers:
                self.assertIn(router.get_addr16(), service_rlocs)

        #---------------------------------------------------------------------------------
        # On-mesh prefix

        # Publish the same on-mesh prefix on different nodes (low
        # preference on end-devices, medium preference on routers, and
        # high on leader) one by one and then unpublish them one by one.
        # Verify that at each step the entries in the network data are
        # correct. Particularly verify that that higher preference
        # entries replace lower preference ones even when there are
        # already desired number in network data.

        num_low = 0
        num_med = 0
        num_high = 0

        for node in end_devs:
            node.netdata_publish_prefix(ON_MESH_PREFIX, ON_MESH_FLAGS, 'low')
            self.simulator.go(WAIT_TIME)
            num_low += 1
            prefixes = leader.get_prefixes()
            self.check_num_of_prefixes(prefixes, num_low, num_med, num_high)

        # Now add the entry as 'med' on routers and check that we see those in the list.
        for node in routers:
            node.netdata_publish_prefix(ON_MESH_PREFIX, ON_MESH_FLAGS, 'med')
            self.simulator.go(WAIT_TIME)
            num_med += 1
            prefixes = leader.get_prefixes()
            self.check_num_of_prefixes(prefixes, num_low, num_med, num_high)

        leader.netdata_publish_prefix(ON_MESH_PREFIX, ON_MESH_FLAGS, 'high')
        self.simulator.go(WAIT_TIME)
        num_high += 1
        prefixes = leader.get_prefixes()
        self.check_num_of_prefixes(prefixes, num_low, num_med, num_high)

        for node in routers:
            node.netdata_unpublish_prefix(ON_MESH_PREFIX)
            self.simulator.go(WAIT_TIME)
            num_med -= 1
            prefixes = leader.get_prefixes()
            self.check_num_of_prefixes(prefixes, num_low, num_med, num_high)

        leader.netdata_unpublish_prefix(ON_MESH_PREFIX)
        self.simulator.go(WAIT_TIME)
        num_high -= 1
        prefixes = leader.get_prefixes()
        self.check_num_of_prefixes(prefixes, num_low, num_med, num_high)

        for node in end_devs:
            node.netdata_unpublish_prefix(ON_MESH_PREFIX)
            self.simulator.go(WAIT_TIME)
            num_low -= 1
            prefixes = leader.get_prefixes()
            self.check_num_of_prefixes(prefixes, num_low, num_med, num_high)

        #- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        # Verify that when removing extra entries, non-preferred entries
        # are removed first over preferred ones. Entries from routers are
        # preferred over similar entries from end-devices.

        # Publish prefix entry on `end_dev1` and verify that it is added.

        end_dev1.netdata_publish_prefix(ON_MESH_PREFIX, ON_MESH_FLAGS, 'med')
        self.simulator.go(WAIT_TIME)
        prefixes = leader.get_prefixes()
        self.check_num_of_prefixes(prefixes, 0, 1, 0)

        # Publish same prefix on all routers (again as `med` preference).
        # Verify that we reach the desired number of prefix entries in network
        # data and that the entry from `end_dev1` is present in network data.

        for node in routers:
            node.netdata_publish_prefix(ON_MESH_PREFIX, ON_MESH_FLAGS, 'med')
            self.simulator.go(WAIT_TIME)
        prefixes = leader.get_prefixes()
        self.check_num_of_prefixes(prefixes, 0, 1 + len(routers), 0)
        self.assertTrue(1 + len(routers) >= DESIRED_NUM_ON_MESH_PREFIX)
        # `prefixes` is a list of format 'fd00:1234:0:0::/64 paos low a802'
        rlocs = [int(prefix.split(' ')[3], 16) for prefix in prefixes]
        self.assertTrue(rlocs.count(end_dev1.get_addr16()) == 1)

        # Publish same prefix now with `high` preference on leader.
        # Since it is `high` preference, it is added to network data
        # which leads to total number of entries to go above the desired
        # number temporarily and trigger other nodes to try to remove
        # their entry. The entries from routers should be preferred over
        # the one from `end_dev1` so that is the one we expect to be
        # removed. We check that this is the case (i.e., the entry from
        # `end_dev1` is no longer present in network data).

        leader.netdata_publish_prefix(ON_MESH_PREFIX, ON_MESH_FLAGS, 'high')
        self.simulator.go(WAIT_TIME)
        prefixes = leader.get_prefixes()
        self.check_num_of_prefixes(prefixes, 0, 1 + len(routers), 1)
        rlocs = [int(prefix.split(' ')[3], 16) for prefix in prefixes]
        self.assertTrue(rlocs.count(end_dev1.get_addr16()) == 0)

        #---------------------------------------------------------------------------------
        # External route

        num = 0
        for node in nodes:
            node.netdata_publish_route(EXTERNAL_ROUTE, EXTERNAL_FLAGS, 'low')
            self.simulator.go(WAIT_TIME)
            num += 1
            routes = leader.get_routes()
            self.check_num_of_routes(routes, num, 0, 0)

        leader.netdata_unpublish_prefix(EXTERNAL_ROUTE)
        leader.netdata_publish_route(EXTERNAL_ROUTE, EXTERNAL_FLAGS, 'high')
        self.simulator.go(WAIT_TIME)
        routes = leader.get_routes()
        self.check_num_of_routes(routes, num - 1, 0, 1)


if __name__ == '__main__':
    unittest.main()
