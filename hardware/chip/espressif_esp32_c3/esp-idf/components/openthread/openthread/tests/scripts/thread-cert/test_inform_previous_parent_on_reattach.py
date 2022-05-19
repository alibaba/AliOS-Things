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

import os
import unittest

import config
import thread_cert

from pktverify.packet_filter import PacketFilter

# Test description:
# The purpose of this test is to verify a MED will inform its previous parent when re-attaches to another parent.
#
# Initial Topology:
#
#  LEADER ----- ROUTER
#    |
#   MED
#

LEADER = 1
ROUTER = 2
MED = 3

LONG_CHILD_TIMEOUT = 120


class TestReset(thread_cert.TestCase):
    SUPPORT_NCP = False
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER, MED]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        MED: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [LEADER],
            'timeout': LONG_CHILD_TIMEOUT,
        },
    }

    def test(self):
        if 'posix' in os.getenv('OT_CLI_PATH', ''):
            self.skipTest("skip for posix tests")

        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(7)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[MED].start()
        self.simulator.go(7)
        self.assertEqual(self.nodes[MED].get_state(), 'child')

        self.assertIsChildOf(MED, LEADER)

        self.nodes[LEADER].remove_allowlist(self.nodes[MED].get_addr64())
        self.nodes[MED].remove_allowlist(self.nodes[LEADER].get_addr64())

        self.nodes[ROUTER].add_allowlist(self.nodes[MED].get_addr64())
        self.nodes[MED].add_allowlist(self.nodes[ROUTER].get_addr64())

        self.nodes[MED].set_timeout(config.DEFAULT_CHILD_TIMEOUT)

        self.simulator.go(config.DEFAULT_CHILD_TIMEOUT * 2)
        self.assertIsChildOf(MED, ROUTER)

        # Verify MED is not in the LEADER's Child Table.
        med_extaddr = self.nodes[MED].get_addr64()
        self.assertFalse(any(info['extaddr'] == med_extaddr for info in self.nodes[LEADER].get_child_table().values()))

        self.collect_ipaddrs()
        self.collect_rlocs()

    def verify(self, pv):
        pkts: PacketFilter = pv.pkts
        pv.summary.show()

        MED = pv.vars['MED']
        LEADER_RLOC = pv.vars['LEADER_RLOC']

        # MED should attach to LEADER first.
        pv.verify_attached('MED', 'LEADER', child_type='MTD')
        # MED should re-attach to ROUTER.
        pv.verify_attached('MED', 'ROUTER', child_type='MTD')
        # MED should send empty IPv6 message to inform previous parent (LEADER).
        pkts.filter_wpan_src64(MED).filter('lowpan.dst == {LEADER_RLOC} and lowpan.next == 0x3b',
                                           LEADER_RLOC=LEADER_RLOC).must_next()

    def assertIsChildOf(self, childid, parentid):
        childRloc16 = self.nodes[childid].get_addr16()
        parentRloc16 = self.nodes[parentid].get_addr16()
        self.assertEqual(parentRloc16 & 0xfc00, parentRloc16)
        self.assertEqual(childRloc16 & 0xfc00, parentRloc16)

        child_extaddr = self.nodes[childid].get_addr64()
        self.assertTrue(
            any(info['extaddr'] == child_extaddr for info in self.nodes[parentid].get_child_table().values()))


if __name__ == '__main__':
    unittest.main()
