#!/usr/bin/env python3
#
#  Copyright (c) 2018, The OpenThread Authors.
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


class TestCoaps(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'mode': 'rdn',
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

        mleid = self.nodes[LEADER].get_ip6_address(config.ADDRESS_TYPE.ML_EID)

        self.nodes[LEADER].coaps_start_psk('psk', 'pskIdentity')
        self.nodes[LEADER].coaps_set_resource_path('test')
        self.nodes[ROUTER].coaps_start_psk('psk', 'pskIdentity')
        self.nodes[ROUTER].coaps_connect(mleid)
        self.nodes[ROUTER].coaps_get()
        self.nodes[ROUTER].coaps_disconnect()
        self.nodes[ROUTER].coaps_stop()
        self.nodes[LEADER].coaps_stop()

        self.nodes[LEADER].coaps_start_x509()
        # self.nodes[LEADER].coaps_set_resource_path('test')
        self.nodes[ROUTER].coaps_start_x509()
        self.nodes[ROUTER].coaps_connect(mleid)
        self.nodes[ROUTER].coaps_get()
        self.nodes[ROUTER].coaps_disconnect()
        self.nodes[ROUTER].coaps_stop()
        self.nodes[LEADER].coaps_stop()


if __name__ == '__main__':
    unittest.main()
