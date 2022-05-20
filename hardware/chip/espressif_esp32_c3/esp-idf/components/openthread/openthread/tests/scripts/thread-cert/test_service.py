#!/usr/bin/env python3
#
#  Copyright (c) 2017, The OpenThread Authors.
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

import re
import unittest

import config
import thread_cert

LEADER = 1
ROUTER1 = 2
ROUTER2 = 3

SRV_0_ID = 0
SRV_0_ENT_NUMBER = '123'
SRV_0_SERVICE_DATA = '123'
SRV_0_SERVER_DATA = 'abc'

SRV_1_ID = 1
SRV_1_ENT_NUMBER = '234'
SRV_1_SERVICE_DATA = '456'
SRV_1_SERVER_DATA = 'def'


class Test_Service(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'channel': 12,
            'mode': 'rdn',
            'network_name': 'OpenThread',
            'allowlist': [ROUTER1, ROUTER2]
        },
        ROUTER1: {
            'channel': 12,
            'mode': 'rdn',
            'network_name': 'OpenThread',
            'allowlist': [LEADER, ROUTER2]
        },
        ROUTER2: {
            'channel': 12,
            'mode': 'rdn',
            'network_name': 'OpenThread',
            'allowlist': [LEADER, ROUTER1]
        },
    }

    def hasAloc(self, node_id, service_id):
        for addr in self.nodes[node_id].get_ip6_address(config.ADDRESS_TYPE.ALOC):
            m = re.match('.*:fc(..)$', addr, re.I)
            if m is not None:
                if m.group(1) == str(service_id + 10):  # for service_id=3 look for '...:fc13'
                    return True

        return False

    def pingFromAll(self, addr):
        for n in list(self.nodes.values()):
            self.assertTrue(n.ping(addr))

    def failToPingFromAll(self, addr):
        for n in list(self.nodes.values()):
            self.assertFalse(n.ping(addr, timeout=3))

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER1].start()
        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'router')

        self.assertEqual(self.hasAloc(LEADER, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(LEADER, SRV_1_ID), False)
        self.assertEqual(self.hasAloc(ROUTER1, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(ROUTER1, SRV_1_ID), False)
        self.assertEqual(self.hasAloc(ROUTER2, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(ROUTER2, SRV_1_ID), False)

        self.nodes[ROUTER1].add_service(SRV_0_ENT_NUMBER, SRV_0_SERVICE_DATA, SRV_0_SERVER_DATA)
        self.nodes[ROUTER1].register_netdata()
        self.simulator.go(2)

        self.assertEqual(self.hasAloc(LEADER, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(LEADER, SRV_1_ID), False)
        self.assertEqual(self.hasAloc(ROUTER1, SRV_0_ID), True)
        self.assertEqual(self.hasAloc(ROUTER1, SRV_1_ID), False)
        self.assertEqual(self.hasAloc(ROUTER2, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(ROUTER2, SRV_1_ID), False)

        aloc0 = self.nodes[ROUTER1].get_ip6_address(config.ADDRESS_TYPE.ALOC)[0]
        self.pingFromAll(aloc0)

        self.nodes[LEADER].add_service(SRV_0_ENT_NUMBER, SRV_0_SERVICE_DATA, SRV_0_SERVER_DATA)
        self.nodes[LEADER].register_netdata()
        self.simulator.go(2)

        self.assertEqual(self.hasAloc(LEADER, SRV_0_ID), True)
        self.assertEqual(self.hasAloc(LEADER, SRV_1_ID), False)
        self.assertEqual(self.hasAloc(ROUTER1, SRV_0_ID), True)
        self.assertEqual(self.hasAloc(ROUTER1, SRV_1_ID), False)
        self.assertEqual(self.hasAloc(ROUTER2, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(ROUTER2, SRV_1_ID), False)

        self.pingFromAll(aloc0)

        self.nodes[ROUTER2].add_service(SRV_1_ENT_NUMBER, SRV_1_SERVICE_DATA, SRV_1_SERVER_DATA)
        self.nodes[ROUTER2].register_netdata()
        self.simulator.go(2)

        self.assertEqual(self.hasAloc(LEADER, SRV_0_ID), True)
        self.assertEqual(self.hasAloc(LEADER, SRV_1_ID), False)
        self.assertEqual(self.hasAloc(ROUTER1, SRV_0_ID), True)
        self.assertEqual(self.hasAloc(ROUTER1, SRV_1_ID), False)
        self.assertEqual(self.hasAloc(ROUTER2, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(ROUTER2, SRV_1_ID), True)

        aloc1 = self.nodes[ROUTER2].get_ip6_address(config.ADDRESS_TYPE.ALOC)[0]
        self.pingFromAll(aloc0)
        self.pingFromAll(aloc1)

        self.nodes[ROUTER1].remove_service(SRV_0_ENT_NUMBER, SRV_0_SERVICE_DATA)
        self.nodes[ROUTER1].register_netdata()
        self.simulator.go(2)

        self.assertEqual(self.hasAloc(LEADER, SRV_0_ID), True)
        self.assertEqual(self.hasAloc(LEADER, SRV_1_ID), False)
        self.assertEqual(self.hasAloc(ROUTER1, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(ROUTER1, SRV_1_ID), False)
        self.assertEqual(self.hasAloc(ROUTER2, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(ROUTER2, SRV_1_ID), True)

        self.pingFromAll(aloc0)
        self.pingFromAll(aloc1)

        self.nodes[LEADER].remove_service(SRV_0_ENT_NUMBER, SRV_0_SERVICE_DATA)
        self.nodes[LEADER].register_netdata()
        self.simulator.go(2)

        self.assertEqual(self.hasAloc(LEADER, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(LEADER, SRV_1_ID), False)
        self.assertEqual(self.hasAloc(ROUTER1, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(ROUTER1, SRV_1_ID), False)
        self.assertEqual(self.hasAloc(ROUTER2, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(ROUTER2, SRV_1_ID), True)

        self.failToPingFromAll(aloc0)
        self.pingFromAll(aloc1)

        self.nodes[ROUTER2].remove_service(SRV_1_ENT_NUMBER, SRV_1_SERVICE_DATA)
        self.nodes[ROUTER2].register_netdata()
        self.simulator.go(2)

        self.assertEqual(self.hasAloc(LEADER, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(LEADER, SRV_1_ID), False)
        self.assertEqual(self.hasAloc(ROUTER1, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(ROUTER1, SRV_1_ID), False)
        self.assertEqual(self.hasAloc(ROUTER2, SRV_0_ID), False)
        self.assertEqual(self.hasAloc(ROUTER2, SRV_1_ID), False)

        self.failToPingFromAll(aloc0)
        self.failToPingFromAll(aloc1)


if __name__ == '__main__':
    unittest.main()
