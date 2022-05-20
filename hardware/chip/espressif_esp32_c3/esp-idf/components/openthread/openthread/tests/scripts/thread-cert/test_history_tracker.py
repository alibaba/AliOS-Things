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
import sys
import thread_cert

# Test description:
#   This test verifies History Tracker behavior.
#
# Topology:
#
#     LEADER
#       |
#       |
#     CHILD
#

LEADER = 1
CHILD = 2

SHORT_WAIT = 5
ONE_DAY = 24 * 60 * 60
MAX_AGE_IN_DAYS = 49


class TestHistoryTracker(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'name': 'Leader',
            'mode': 'rdn',
        },
        CHILD: {
            'name': 'Child',
            'mode': 'n',
        },
    }

    def test(self):
        leader = self.nodes[LEADER]
        child = self.nodes[CHILD]

        # Start the leader and verify that 'netinfo' history
        # is updated correctly.

        leader.start()
        self.simulator.go(SHORT_WAIT)
        self.assertEqual(leader.get_state(), 'leader')

        netinfo = leader.history_netinfo()
        self.assertEqual(len(netinfo), 2)
        self.assertEqual(netinfo[0]['role'], 'leader')
        self.assertEqual(netinfo[0]['mode'], 'rdn')
        self.assertEqual(int(netinfo[0]['rloc16'], 16), leader.get_addr16())
        self.assertEqual(netinfo[0]['partition-id'], leader.get_partition_id())
        self.assertEqual(netinfo[1]['role'], 'detached')

        # Stop the leader

        leader.thread_stop()
        leader.interface_down()
        self.simulator.go(SHORT_WAIT)
        netinfo = leader.history_netinfo(2)
        self.assertEqual(len(netinfo), 2)
        self.assertEqual(netinfo[0]['role'], 'disabled')
        self.assertEqual(netinfo[1]['role'], 'leader')

        # Wait for one day, two days, then up to max age and verify that
        # `netinfo` entry age is updated correctly.
        #
        # Since we want to wait for long duration (49 days), to speed up
        # the simulation time, we disable leader to avoid the need to
        # to simulate all the message/events (e.g. MLE adv) while thread
        # is operational.

        self.simulator.go(ONE_DAY)
        netinfo = leader.history_netinfo(1)
        self.assertTrue(netinfo[0]['age'].startswith('1 day'))

        self.simulator.go(ONE_DAY)
        netinfo = leader.history_netinfo(1)
        self.assertTrue(netinfo[0]['age'].startswith('2 days'))

        self.simulator.go((MAX_AGE_IN_DAYS - 3) * ONE_DAY)
        netinfo = leader.history_netinfo(1)
        self.assertTrue(netinfo[0]['age'].startswith('{} days'.format(MAX_AGE_IN_DAYS - 1)))

        self.simulator.go(ONE_DAY)
        netinfo = leader.history_netinfo(1)
        self.assertTrue(netinfo[0]['age'].startswith('more than {} days'.format(MAX_AGE_IN_DAYS)))

        self.simulator.go(2 * ONE_DAY)
        netinfo = leader.history_netinfo(1)
        self.assertTrue(netinfo[0]['age'].startswith('more than {} days'.format(MAX_AGE_IN_DAYS)))

        # Start leader and child

        leader.start()
        self.simulator.go(SHORT_WAIT)
        self.assertEqual(leader.get_state(), 'leader')

        child.start()
        self.simulator.go(SHORT_WAIT)
        self.assertEqual(child.get_state(), 'child')

        child_rloc16 = child.get_addr16()
        leader_rloc16 = leader.get_addr16()

        # Verify the `netinfo` history on child

        netinfo = child.history_netinfo(2)
        self.assertEqual(len(netinfo), 2)
        self.assertEqual(netinfo[0]['role'], 'child')
        self.assertEqual(netinfo[0]['mode'], 'n')
        self.assertEqual(int(netinfo[0]['rloc16'], 16), child_rloc16)
        self.assertEqual(netinfo[0]['partition-id'], leader.get_partition_id())
        self.assertEqual(netinfo[1]['role'], 'detached')

        # Change the child mode and verify that `netinfo` history
        # records this change.

        child.set_mode('rn')
        self.simulator.go(SHORT_WAIT)
        netinfo = child.history_netinfo(1)
        self.assertEqual(len(netinfo), 1)
        self.assertEqual(netinfo[0]['mode'], 'rn')

        # Ping from leader to child and check the RX and TX history
        # on child and leader.

        child_mleid = child.get_mleid()
        leader_mleid = leader.get_mleid()

        ping_sizes = [10, 100, 1000]
        num_msgs = len(ping_sizes)

        for size in ping_sizes:
            leader.ping(child_mleid, size=size)

        leader_tx = leader.history_tx(num_msgs)
        leader_rx = leader.history_rx(num_msgs)
        child_tx = child.history_tx(num_msgs)
        child_rx = child.history_rx(num_msgs)

        for index in range(num_msgs):
            self.assertEqual(leader_tx[index]['type'], 'ICMP6(EchoReqst)')
            self.assertEqual(leader_tx[index]['sec'], 'yes')
            self.assertEqual(leader_tx[index]['prio'], 'norm')
            self.assertEqual(leader_tx[index]['tx-success'], 'yes')
            self.assertEqual(leader_tx[index]['radio'], '15.4')
            self.assertEqual(int(leader_tx[index]['to'], 16), child_rloc16)
            self.assertEqual(leader_tx[index]['src'][1:-3], leader_mleid)
            self.assertEqual(leader_tx[index]['dst'][1:-3], child_mleid)

            self.assertEqual(child_rx[index]['type'], 'ICMP6(EchoReqst)')
            self.assertEqual(child_rx[index]['sec'], 'yes')
            self.assertEqual(child_rx[index]['prio'], 'norm')
            self.assertEqual(child_rx[index]['radio'], '15.4')
            self.assertEqual(int(child_rx[index]['from'], 16), leader_rloc16)
            self.assertEqual(child_rx[index]['src'][1:-3], leader_mleid)
            self.assertEqual(child_rx[index]['dst'][1:-3], child_mleid)

            self.assertEqual(leader_rx[index]['type'], 'ICMP6(EchoReply)')
            self.assertEqual(child_tx[index]['type'], 'ICMP6(EchoReply)')

            self.assertEqual(leader_tx[index]['len'], child_rx[index]['len'])
            self.assertEqual(leader_rx[index]['len'], child_tx[index]['len'])


if __name__ == '__main__':
    #  FIXME: We skip the test under distcheck build (the simulation
    #  under this build for some reason cannot seem to handle longer
    #  wait times - days up to 50 days in this test). We return error
    #  code 77 which indicates that this test case was skipped (in
    #  automake).

    if os.getenv('DISTCHECK_BUILD') == '1':
        sys.exit(77)

    unittest.main()
