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
import pktverify
from pktverify import packet_verifier
from pktverify.consts import MA1, PBBR_ALOC
import unittest

import config
import thread_cert

# Test description:
# The purpose of this test case is to verify that a Primary BBR ignores a
# Timeout TLV if included in an MLR.req that does not contain a Commissioner
# Session ID TLV nor is signed by a Commissioner.
#
# Topology:
#    ----------------(eth)------------------
#           |                  |     |
#          BR1 (Leader) ----- BR2   HOST
#           |                  |
#           |                  |
#          Router_1------------+
#

BR_1 = 1
BR_2 = 2
ROUTER_1 = 3
HOST = 4


class MATN_03_InvalidCommissionerDeregistration(thread_cert.TestCase):
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
            'allowlist': [BR_1, ROUTER_1],
            'is_otbr': True,
            'version': '1.2',
        },
        ROUTER_1: {
            'name': 'Router_1',
            'allowlist': [BR_1, BR_2],
            'version': '1.2',
        },
        HOST: {
            'name': 'Host',
            'is_host': True
        },
    }

    def test(self):
        br1 = self.nodes[BR_1]
        br2 = self.nodes[BR_2]
        router = self.nodes[ROUTER_1]
        host = self.nodes[HOST]

        br1.start()
        self.simulator.go(5)
        self.assertEqual('leader', br1.get_state())
        self.assertTrue(br1.is_primary_backbone_router)

        router.start()
        self.simulator.go(5)
        self.assertEqual('router', router.get_state())

        br2.start()
        self.simulator.go(5)
        self.assertEqual('router', router.get_state())
        self.assertFalse(br2.is_primary_backbone_router)

        host.start(start_radvd=False)
        self.simulator.go(5)

        router.add_ipmaddr(MA1)
        self.simulator.go(5)

        # Ensure that router can receive a multicast ping packet sent to MA1.
        self.assertTrue(
            host.ping(MA1, backbone=True, ttl=10, interface=host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.simulator.go(5)

        # 1. Router attempts to deregister for multicast address, MA1, at BR_1.
        router.register_multicast_listener(MA1, timeout=0)

        # 3. Host sends a ping packet to the multicast address, MA1.
        self.assertTrue(
            host.ping(MA1, backbone=True, ttl=10, interface=host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.simulator.go(5)

        self.collect_ipaddrs()
        self.collect_rloc16s()
        self.collect_rlocs()
        self.collect_leader_aloc(BR_1)
        self.collect_extra_vars()

    def verify(self, pv: pktverify.packet_verifier.PacketVerifier):
        pkts = pv.pkts
        vars = pv.vars
        pv.summary.show()

        # Ensure the topology is formed correctly
        pv.verify_attached('Router_1', 'BR_1')
        pv.verify_attached('BR_2')

        # 1. Router attempts to deregister for multicast address, MA1, at BR_1.
        # Router unicasts an MLR.req CoAP request to BR_1 as follows:
        # coap://[<BR_1 RLOC>]:MM/n/mr
        # Where the payload contains:
        # IPv6 Addresses TLV: MA1
        # Timeout TLV: 0
        pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_ipv6_2dsts(vars['BR_1_RLOC'], PBBR_ALOC) \
            .filter_coap_request('/n/mr') \
            .filter(lambda p: p.thread_meshcop.tlv.ipv6_addr == [MA1] and
                              p.thread_nm.tlv.timeout == 0) \
            .must_next()

        # 2. BR_1 responds to the multicast registration successfully,
        # ignoring the timeout value.
        pkts.filter_wpan_src64(vars['BR_1']) \
            .filter_ipv6_dst(vars['Router_1_RLOC']) \
            .filter_coap_ack('/n/mr') \
            .filter(lambda p: p.thread_nm.tlv.status == 0) \
            .must_next()

        # 3. Host sends a ping packet to the multicast address, MA1.
        _pkt = pkts.filter_eth_src(vars['Host_ETH']) \
            .filter_ipv6_dst(MA1) \
            .filter_ping_request() \
            .must_next()

        # 4. BR_1 forwards the ping packet with multicast address, MA1, to its
        # Thread Network encapsulated in an MPL packet.
        pkts.filter_wpan_src64(vars['BR_1']) \
            .filter_AMPLFMA(mpl_seed_id=vars['BR_1_RLOC']) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_next()

        # 5. Router receives the MPL packet containing an encapsulated ping
        # packet to MA1, sent by Host, and unicasts a ping response packet back
        # to Host.
        pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_ipv6_dst(_pkt.ipv6.src) \
            .filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier) \
            .must_next()


if __name__ == '__main__':
    unittest.main()
