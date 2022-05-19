#!/usr/bin/env python3
#
#  Copyright (c) 2019, The OpenThread Authors.
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

import common
import config
import thread_cert

LEADER = 1
ROUTER = 2


class TestIPv6Fragmentation(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'mode': 'rdn',
            'panid': 0xcafe,
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'mode': 'rdn',
            'panid': 0xcafe,
            'allowlist': [LEADER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        mleid_leader = self.nodes[LEADER].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        mleid_router = self.nodes[ROUTER].get_ip6_address(config.ADDRESS_TYPE.ML_EID)

        self.nodes[LEADER].udp_start("::", common.UDP_TEST_PORT)
        self.nodes[ROUTER].udp_start("::", common.UDP_TEST_PORT)

        self.nodes[LEADER].udp_send(1952, mleid_router, common.UDP_TEST_PORT)
        self.simulator.go(5)
        self.nodes[ROUTER].udp_check_rx(1952)

        self.nodes[ROUTER].udp_send(1831, mleid_leader, common.UDP_TEST_PORT)
        self.simulator.go(5)
        self.nodes[LEADER].udp_check_rx(1831)

        self.nodes[ROUTER].udp_stop()
        self.nodes[LEADER].udp_stop()


if __name__ == '__main__':
    unittest.main()
