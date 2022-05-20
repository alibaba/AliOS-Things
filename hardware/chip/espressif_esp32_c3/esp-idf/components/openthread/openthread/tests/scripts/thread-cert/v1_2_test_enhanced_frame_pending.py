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
"""This test case verifies Thread 1.2 enhanced frame pending feature.

Topology

    Leader ----- SED_1

1. Set up topology a Leader and a SED.
2. SED pings leader so a data poll is just sent.
3. Leader sends a UDP packet to SED to put a pending frame in queue.
4. Wait for half polling period, verify no packet received by SED.
5. SED sends a UDP packet to Leader to solicit an ACK with frame pending set.
   * Verify Leader receives a UDP packet
   * Verify SED sends a data poll
   * Verify SED receives a UDP packet
"""

import unittest
import pexpect

import thread_cert
import common

LEADER = 1
SED_1 = 2

CHILD_TIMEOUT = 30
UDP_BYTES_COUNT = 73

DEFAULT_POLL_PERIOD = CHILD_TIMEOUT - 4
"""The default poll period calculated from child timeout."""


class SED_EnhancedFramePending(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'version': '1.2'
        },
        SED_1: {
            'mode': '-',
            'version': '1.2',
        },
    }

    def test(self):
        self.nodes[SED_1].set_timeout(CHILD_TIMEOUT)

        # 1 - Set up topology
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[SED_1].start()
        self.simulator.go(7)
        self.assertEqual(self.nodes[SED_1].get_state(), 'child')

        self.nodes[LEADER].udp_start('::', common.UDP_TEST_PORT)
        self.nodes[SED_1].udp_start('::', common.UDP_TEST_PORT)

        # 2 - Ping Leader
        self.assertTrue(self.nodes[SED_1].ping(self.nodes[LEADER].get_rloc(), timeout=CHILD_TIMEOUT))

        self.flush_all()

        # 3 - Send to SED
        self.nodes[LEADER].udp_send(UDP_BYTES_COUNT, self.nodes[SED_1].get_rloc(), common.UDP_TEST_PORT)

        # 4 - Wait for half polling period
        self.simulator.go(DEFAULT_POLL_PERIOD // 2)
        with self.assertRaises(pexpect.TIMEOUT):
            self.nodes[SED_1].udp_check_rx(UDP_BYTES_COUNT)

        # 5 - Send to Leader
        self.nodes[SED_1].udp_send(UDP_BYTES_COUNT, self.nodes[LEADER].get_rloc(), common.UDP_TEST_PORT)
        self.simulator.go(1)
        self.nodes[LEADER].udp_check_rx(UDP_BYTES_COUNT)
        sed_messages = self.simulator.get_messages_sent_by(SED_1)
        self.assertNotEqual(sed_messages.next_data_poll(), None)
        self.nodes[SED_1].udp_check_rx(UDP_BYTES_COUNT)


if __name__ == '__main__':
    unittest.main()
