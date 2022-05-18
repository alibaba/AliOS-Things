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

import thread_cert

# Test description:
#
#   This test verifies the behavior of the network when a zero length external route prefix "::/0" is added.
#
# Topology:
#
#    leader --- router1
#       \         /
#        \       /
#         router2
#

LEADER = 1
ROUTER1 = 2
ROUTER2 = 3


class ZeroLengthExternalRoute(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
        },
        ROUTER1: {
            'name': 'ROUTER2',
            'mode': 'rdn',
        },
        ROUTER2: {
            'name': 'ROUTER2',
            'mode': 'rdn',
        },
    }

    def test(self):
        leader = self.nodes[LEADER]
        router1 = self.nodes[ROUTER1]
        router2 = self.nodes[ROUTER2]

        # Start the nodes and form the network.

        leader.start()
        self.simulator.go(5)
        self.assertEqual(leader.get_state(), 'leader')

        router1.start()
        router2.start()
        self.simulator.go(5)
        self.assertEqual(router1.get_state(), 'router')
        self.assertEqual(router2.get_state(), 'router')

        # Add an on-mesh prefix with SLAAC on router2 (without default
        # route flag).

        router2.add_prefix('fd00:1234::/64', 'paos')
        router2.register_netdata()

        # Add a zero length external route on router1. Also add an IPv6
        # address on router1.

        router1.add_route('::/0', stable=True)
        router1.add_ipaddr('fd00:abcd::1')
        router1.register_netdata()
        self.simulator.go(5)

        # Ping from leader the address added on router1. The zero
        # length external route should ensure the message is routed
        # to router1.

        self.assertTrue(leader.ping('fd00:abcd::1'))

        # Change the on-mesh prefix on router2 to now also have
        # the default route flag.

        router2.remove_prefix('fd00:1234::/64')
        router2.add_prefix('fd00:5678::/64', 'paros')
        router2.register_netdata()
        self.simulator.go(5)

        # Again ping from leader the same address. The explicit
        # external route (even with zero length) on router1 should
        # still be preferred over the default route flag of the
        # on-mesh prefix from router2.

        self.assertTrue(leader.ping('fd00:abcd::1'))

        # Remove the external route on router1.

        router1.remove_route("::/0")
        router1.register_netdata()
        self.simulator.go(5)

        # Now the ping should fail since the message would be routed
        # to router2 (due to its on-mesh prefix with default route
        # flag).

        self.assertFalse(leader.ping('fd00:abcd::1'))

        # Remove the address from router1 and add it on router2 and
        # ping it again from leader to verify that the message is
        # being routed to router2.

        router1.del_ipaddr('fd00:abcd::1')
        router2.add_ipaddr('fd00:abcd::1')
        self.simulator.go(5)
        self.assertTrue(leader.ping('fd00:abcd::1'))


if __name__ == '__main__':
    unittest.main()
