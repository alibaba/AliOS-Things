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
import re
import unittest

import thread_cert
from config import MESH_LOCAL_PREFIX_REGEX_PATTERN, ROUTING_LOCATOR_REGEX_PATTERN
from pktverify.packet_verifier import PacketVerifier

# Test description:
# The purpose of this test is to verify ALOC connectivity.
#
# Topology:
#
#
#  PBBR ----- LEADER ---- ROUTER
#
#

LEADER = 1
PBBR = 2
ROUTER = 3


class TestPing(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False
    SUPPORT_THREAD_1_1 = False

    TOPOLOGY = {
        LEADER: {
            'name': 'Router_1',
            'allowlist': [PBBR, ROUTER],
        },
        PBBR: {
            'name': 'Router_2',
            'allowlist': [LEADER],
            'is_bbr': True,
        },
        ROUTER: {
            'name': 'Router_3',
            'allowlist': [LEADER],
        },
    }

    def test(self):
        leader = self.nodes[LEADER]
        pbbr = self.nodes[PBBR]
        router = self.nodes[ROUTER]

        leader.start()
        self.simulator.go(5)
        self.assertEqual('leader', leader.get_state())

        pbbr.enable_backbone_router()
        pbbr.start()
        self.simulator.go(10)
        self.assertEqual('router', pbbr.get_state())
        self.assertTrue(pbbr.is_primary_backbone_router)

        router.start()
        self.simulator.go(10)
        self.assertEqual('router', router.get_state())

        for node in (leader, pbbr):
            for addr in self.get_alocs(node):
                self.assertTrue(router.ping(addr))

    def verify(self, pv: PacketVerifier):
        pkts = pv.pkts
        # Must sure no ADDR_QUERY.qry is ever sent
        pkts.filter_coap_request('/a/aq').must_not_next()

    def get_alocs(self, node):
        mleid = node.get_mleid()
        for ip in node.get_addrs():
            if not re.match(MESH_LOCAL_PREFIX_REGEX_PATTERN, ip):
                continue

            if ip == mleid:
                continue

            self.assertIsNotNone(re.match(ROUTING_LOCATOR_REGEX_PATTERN, ip), ip)
            locator = int(ip.split(':')[-1], 16)
            if locator >= 0xfc00:
                yield ip


if __name__ == '__main__':
    unittest.main()
