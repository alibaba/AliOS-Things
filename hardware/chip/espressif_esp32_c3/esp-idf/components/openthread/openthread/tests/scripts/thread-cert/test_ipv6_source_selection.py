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

import ipv6
import thread_cert

LEADER = 1
ROUTER = 2


class TestIPv6SourceSelection(thread_cert.TestCase):
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

        leader_aloc = self.nodes[LEADER].get_addr_leader_aloc()
        leader_mleid = self.nodes[LEADER].get_mleid()
        leader_rloc = self.nodes[LEADER].get_rloc()
        leader_linklocal = self.nodes[LEADER].get_linklocal()
        multicast_addr = 'ff03::1'
        external_addr = '2007::1'

        router_rloc = self.nodes[ROUTER].get_rloc()
        router_linklocal = self.nodes[ROUTER].get_linklocal()
        router_mleid = self.nodes[ROUTER].get_mleid()

        # Source check - RLOC source for RLOC destination
        self.assertTrue(self.nodes[ROUTER].ping(leader_rloc))
        router_msgs = self.simulator.get_messages_sent_by(ROUTER)
        msg = router_msgs.get_icmp_message(ipv6.ICMP_ECHO_REQUEST)
        msg.assertSentFromSourceAddress(router_rloc)

        # Source check - ML-EID source for ALOC destination
        self.assertTrue(self.nodes[ROUTER].ping(leader_aloc))
        router_msgs = self.simulator.get_messages_sent_by(ROUTER)
        msg = router_msgs.get_icmp_message(ipv6.ICMP_ECHO_REQUEST)
        msg.assertSentFromSourceAddress(router_mleid)

        # Source check - ML-EID source for ML-EID destination
        self.assertTrue(self.nodes[ROUTER].ping(leader_mleid))
        router_msgs = self.simulator.get_messages_sent_by(ROUTER)
        msg = router_msgs.get_icmp_message(ipv6.ICMP_ECHO_REQUEST)
        msg.assertSentFromSourceAddress(router_mleid)

        # Source check - link local source source for link local address
        self.assertTrue(self.nodes[ROUTER].ping(leader_linklocal))
        router_msgs = self.simulator.get_messages_sent_by(ROUTER)
        msg = router_msgs.get_icmp_message(ipv6.ICMP_ECHO_REQUEST)
        msg.assertSentFromSourceAddress(router_linklocal)

        # Source check - ML-EID source for realmlocal multicast destination
        self.assertTrue(self.nodes[ROUTER].ping(multicast_addr))
        router_msgs = self.simulator.get_messages_sent_by(ROUTER)
        msg = router_msgs.get_icmp_message(ipv6.ICMP_ECHO_REQUEST)
        msg.assertSentFromSourceAddress(router_mleid)

        # GUA and default gateway
        self.nodes[LEADER].add_prefix('2001::/64', 'paros')
        self.nodes[LEADER].register_netdata()
        self.simulator.go(5)

        # Set lowpan context of sniffer
        self.simulator.set_lowpan_context(1, '2001::/64')

        # Flushes message queue before next ping
        router_msgs = self.simulator.get_messages_sent_by(ROUTER)

        # Source check - GUA source for GUA destination
        leader_gua = self.nodes[LEADER].get_addr("2001::/64")
        router_gua = self.nodes[ROUTER].get_addr("2001::/64")
        self.assertTrue(leader_gua is not None)
        self.assertTrue(router_gua is not None)
        self.assertTrue(self.nodes[ROUTER].ping(leader_gua))
        router_msgs = self.simulator.get_messages_sent_by(ROUTER)
        msg = router_msgs.get_icmp_message(ipv6.ICMP_ECHO_REQUEST)
        msg.assertSentFromSourceAddress(router_gua)

        # Source check - GUA source for external address (default route)
        self.nodes[ROUTER].ping(external_addr)
        router_msgs = self.simulator.get_messages_sent_by(ROUTER)
        msg = router_msgs.get_icmp_message(ipv6.ICMP_ECHO_REQUEST)
        msg.assertSentFromSourceAddress(router_gua)


if __name__ == '__main__':
    unittest.main()
