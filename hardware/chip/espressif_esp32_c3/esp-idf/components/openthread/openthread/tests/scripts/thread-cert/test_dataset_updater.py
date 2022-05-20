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

import unittest

import config
import thread_cert

LEADER = 1
ROUTER = 2
MED = 3
SED = 4


class TestDatasetUpdater(thread_cert.TestCase):
    SUPPORT_NCP = False
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'mode': 'rdn',
            'channel': 11,
        },
        ROUTER: {
            'mode': 'rdn',
            'channel': 11,
        },
        MED: {
            'mode': 'rn',
            'channel': 11,
            'allowlist': [ROUTER],
        },
        SED: {
            'mode': '-',
            'channel': 11,
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER],
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[MED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MED].get_state(), 'child')

        self.nodes[SED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED].get_state(), 'child')

        self.verify_state(11)

        # update initiated by LEADER
        self.nodes[LEADER].start_dataset_updater(channel=12)
        self.simulator.go(120)
        self.verify_state(12)

        # update initiated by ROUTER
        self.nodes[ROUTER].start_dataset_updater(channel=13)
        self.simulator.go(120)
        self.verify_state(13)

        # update initiated by LEADER overridden by ROUTER
        self.nodes[LEADER].start_dataset_updater(channel=14)
        self.simulator.go(20)
        self.nodes[ROUTER].start_dataset_updater(channel=15)
        self.simulator.go(120)
        self.verify_state(15)

        # update initiated by ROUTER overridden by LEADER
        self.nodes[ROUTER].start_dataset_updater(channel=16)
        self.simulator.go(10)
        self.nodes[LEADER].start_dataset_updater(channel=17)
        self.simulator.go(120)
        self.verify_state(17)

    def verify_state(self, channel):
        self.assertEqual(self.nodes[LEADER].get_channel(), channel)
        self.assertEqual(self.nodes[ROUTER].get_channel(), channel)
        self.assertEqual(self.nodes[MED].get_channel(), channel)
        self.assertEqual(self.nodes[SED].get_channel(), channel)


if __name__ == '__main__':
    unittest.main()
