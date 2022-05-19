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

import logging
import unittest

import pktverify
from pktverify import packet_verifier, packet_filter, consts
from pktverify.consts import MA1
import config
import thread_cert

# Test description:
# The purpose of this test case is to verify that a Secondary BBR can take over
# forwarding of outbound multicast transmissions from the Thread Network when
# the Primary BBR disconnects. The Secondary in that case becomes Primary.
#
# Topology:
#    ----------------(eth)------------------
#           |                  |
#         BR_1 (Leader) ---- BR_2
#           |                  |
#           |                  |
#          ROUTER_1 -----------+
#
from pktverify.null_field import nullField

BR_1 = 1
BR_2 = 2
ROUTER_1 = 3


class MATN_09_FailureOfPrimaryBBROutboundMulticast(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        BR_1: {
            'name': 'BR_1',
            'is_otbr': True,
            'allowlist': [BR_2, ROUTER_1],
            'version': '1.2',
        },
        BR_2: {
            'name': 'BR_2',
            'is_otbr': True,
            'allowlist': [BR_1, ROUTER_1],
            'version': '1.2',
        },
        ROUTER_1: {
            'name': 'Router_1',
            'allowlist': [BR_1, BR_2],
            'version': '1.2',
            'partition_id': 1,
            'network_id_timeout': 150,
        },
    }

    def test(self):
        br1 = self.nodes[BR_1]
        br2 = self.nodes[BR_2]
        router1 = self.nodes[ROUTER_1]

        br1.start()
        self.simulator.go(5)
        self.assertEqual('leader', br1.get_state())
        self.assertTrue(br1.is_primary_backbone_router)

        router1.start()
        self.simulator.go(10)
        self.assertEqual('router', router1.get_state())

        br2.start()
        self.simulator.go(5)
        self.assertEqual('router', br2.get_state())
        self.assertFalse(br2.is_primary_backbone_router)

        # 1. Router sends a ping packet to the multicast address, MA1,
        # encapsulated in an MPL packet
        self.assertFalse(router1.ping(MA1))
        self.simulator.go(5)

        # 4a. Switch off BR_1
        br1.disable_backbone_router()
        br1.thread_stop()
        br1.interface_down()
        self.simulator.go(180)

        # 4b. BR_2 detects the missing Primary BBR and becomes the the Leader of
        # the Thread Network.
        self.assertEqual('disabled', br1.get_state())
        self.assertEqual('leader', br2.get_state())
        self.assertEqual('router', router1.get_state())
        self.assertFalse(br1.is_primary_backbone_router)
        self.assertTrue(br2.is_primary_backbone_router)

        # 5. Router_1 sends a ping packet to the multicast address, MA1,
        # encapsulated in an MPL packet.
        self.assertFalse(router1.ping(MA1))

        self.collect_ipaddrs()
        self.collect_rloc16s()
        self.collect_rlocs()
        self.collect_leader_aloc(BR_2)
        self.collect_extra_vars()

    def verify(self, pv: pktverify.packet_verifier.PacketVerifier):
        pkts = pv.pkts
        vars = pv.vars
        pv.summary.show()
        logging.info(f'vars = {vars}')

        # Ensure the topology is formed correctly
        pv.verify_attached('Router_1', 'BR_1')
        pv.verify_attached('BR_2')

        # 1. Router_1 sends a ping packet to the multicast address, MA1,
        # encapsulated in an MPL packet.
        _pkt = pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_AMPLFMA(mpl_seed_id=vars['Router_1_RLOC']) \
            .filter_ping_request() \
            .must_next()

        initial_identifier = _pkt.icmpv6.echo.identifier

        # 2. BR_1 forwards the multicast ping packet with multicast address,
        # MA1, to the LAN.
        pkts.filter_eth_src(vars['BR_1_ETH']) \
            .filter_ipv6_dst(MA1) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_next()

        with pkts.save_index():
            pv.verify_attached('Router_1', 'BR_2')

        # 4b. BR_2 detects the missing Primary BBR and becomes the Leader of the
        # Thread Network, distributing its BBR dataset.
        # Verify that Router_1 has received the new BBR Dataset from BR_2,
        # where:
        # • RLOC16 in Server TLV == The RLOC16 of BR_2
        # All fields in the Service TLV contain valid values.
        pkts.filter_wpan_src64(vars['BR_2']) \
            .filter_mle() \
            .filter(
            lambda p: p.thread_nwd.tlv.server_16 is not nullField and
                      vars['BR_2_RLOC16'] in p.thread_nwd.tlv.server_16) \
            .must_next()

        # 5.Router_1 sends a ping packet to the multicast address, MA1,
        # encapsulated in an MPL packet.
        _pkt = pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_AMPLFMA(mpl_seed_id=vars['Router_1_RLOC']) \
            .filter_ping_request() \
            .filter(lambda p: p.icmpv6.echo.identifier != initial_identifier) \
            .must_next()

        # 6. BR_2 forwards the multicast ping packet with multicast address,
        # MA1, to the LAN.
        pkts.filter_eth_src(vars['BR_2_ETH']) \
            .filter_ipv6_dst(MA1) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_next()


if __name__ == '__main__':
    unittest.main()
