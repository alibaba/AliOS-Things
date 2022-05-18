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

import config
import unittest
import thread_cert

LEADER = 1
ROUTER = 2
ED = 3
SED = 4


class TestReset(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'mode': 'rdn',
            'allowlist': [LEADER, ED]
        },
        ED: {
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [ROUTER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(7)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[ED].start()
        self.simulator.go(7)
        self.assertEqual(self.nodes[ED].get_state(), 'child')

        for i in range(0, 1010):
            self.assertTrue(self.nodes[ED].ping(self.nodes[LEADER].get_ip6_address(config.ADDRESS_TYPE.RLOC)))
        self.simulator.go(1)

        self.nodes[LEADER].reset()
        self.nodes[LEADER].start()
        self.simulator.go(7)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].reset()
        self.nodes[ROUTER].start()
        self.simulator.go(7)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[ED].reset()
        self.nodes[LEADER].add_allowlist(self.nodes[ROUTER].get_addr64())
        self.nodes[LEADER].enable_allowlist()
        self.nodes[ED].start()
        self.simulator.go(7)
        self.assertEqual(self.nodes[ED].get_state(), 'child')
        self.assertTrue(self.nodes[ED].ping(self.nodes[LEADER].get_ip6_address(config.ADDRESS_TYPE.RLOC)))


if __name__ == '__main__':
    unittest.main()
