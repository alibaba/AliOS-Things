#!/usr/bin/env python3
#
#  Copyright (c) 2020, The OpenThread Authors.
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

LEADER = 1
ROUTER = 2
ED1 = 3
SED1 = 4

MTDS = [ED1, SED1]


class Test_OnMeshPrefix(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'mode': 'rdn',
            'allowlist': [LEADER, ED1, SED1]
        },
        ED1: {
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [ROUTER]
        },
        SED1: {
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(4)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[ED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED1].get_state(), 'child')

        self.nodes[SED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED1].get_state(), 'child')

        self.nodes[ROUTER].add_prefix('2001:2:0:1::/64', 'paros')
        self.nodes[ROUTER].add_prefix('2001:2:0:2::/64', 'paro')
        self.nodes[ROUTER].register_netdata()

        # Set lowpan context of sniffer
        self.simulator.set_lowpan_context(1, '2001:2:0:1::/64')
        self.simulator.set_lowpan_context(2, '2001:2:0:2::/64')

        self.simulator.go(10)

        addrs = self.nodes[ED1].get_addrs()
        self.assertTrue(any('2001:2:0:1' in addr[0:10] for addr in addrs))
        self.assertTrue(any('2001:2:0:2' in addr[0:10] for addr in addrs))
        for addr in addrs:
            if addr[0:3] == '200':
                self.assertTrue(self.nodes[LEADER].ping(addr))

        addrs = self.nodes[SED1].get_addrs()
        self.assertTrue(any('2001:2:0:1' in addr[0:10] for addr in addrs))
        self.assertFalse(any('2001:2:0:2' in addr[0:10] for addr in addrs))
        for addr in addrs:
            if addr[0:3] == '200':
                self.assertTrue(self.nodes[LEADER].ping(addr))

        self.nodes[ROUTER].add_prefix('2002:2:0:3::/64', 'pars')
        self.nodes[ROUTER].register_netdata()

        # Set lowpan context of sniffer
        self.simulator.set_lowpan_context(3, '2002:2:0:3::/64')

        self.simulator.go(10)

        addrs = self.nodes[ED1].get_addrs()
        self.assertTrue(any('2001:2:0:1' in addr[0:10] for addr in addrs))
        self.assertTrue(any('2001:2:0:2' in addr[0:10] for addr in addrs))
        self.assertTrue(any('2002:2:0:3' in addr[0:10] for addr in addrs))
        for addr in addrs:
            if addr[0:4] == '2001':
                self.assertTrue(self.nodes[LEADER].ping(addr))
            elif addr[0:4] == '2002':
                self.assertFalse(self.nodes[LEADER].ping(addr))

        addrs = self.nodes[SED1].get_addrs()
        self.assertTrue(any('2001:2:0:1' in addr[0:10] for addr in addrs))
        self.assertFalse(any('2001:2:0:2' in addr[0:10] for addr in addrs))
        self.assertTrue(any('2002:2:0:3' in addr[0:10] for addr in addrs))
        for addr in addrs:
            if addr[0:4] == '2001':
                self.assertTrue(self.nodes[LEADER].ping(addr))
            elif addr[0:4] == '2002':
                self.assertFalse(self.nodes[LEADER].ping(addr))


if __name__ == '__main__':
    unittest.main()
