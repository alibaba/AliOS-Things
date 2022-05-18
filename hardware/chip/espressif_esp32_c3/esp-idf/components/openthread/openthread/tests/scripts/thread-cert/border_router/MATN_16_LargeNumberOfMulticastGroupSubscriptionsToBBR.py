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
from pktverify.consts import MAS, MA2
import config
import thread_cert

# Test description:
# The purpose of this test case is to verify that the Primary BBR can handle a
# large number (75) of subscriptions to different multicast groups. Multicast
# registrations are performed each time with 15 multicast addresses per
# registration request.
#
# Topology:
#
#
#    ----------------(eth)--------------------
#           |                     |
#         BR_1 (Leader)           |
#           |                   Host
#           |
#           |
#         Router_1
#
#
#

BR_1 = 1
ROUTER_1 = 2
HOST = 3

REG_DELAY = 10


class MATN_16_LargeNumberOfMulticastGroupSubscriptionsToBBR(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        BR_1: {
            'name': 'BR_1',
            'is_otbr': True,
            'allowlist': [ROUTER_1],
            'version': '1.2',
        },
        ROUTER_1: {
            'name': 'Router_1',
            'allowlist': [BR_1],
            'version': '1.2',
        },
        HOST: {
            'name': 'Host',
            'is_host': True,
        },
    }

    def test(self):
        br1 = self.nodes[BR_1]
        router1 = self.nodes[ROUTER_1]
        host = self.nodes[HOST]

        br1.set_backbone_router(reg_delay=REG_DELAY, mlr_timeout=consts.MLR_TIMEOUT_MIN)
        br1.start()
        self.simulator.go(5)
        self.assertEqual('leader', br1.get_state())
        self.assertTrue(br1.is_primary_backbone_router)

        router1.start()
        self.simulator.go(5)
        self.assertEqual('router', router1.get_state())

        host.start(start_radvd=False)
        self.simulator.go(5)

        # Loop steps 1 to 4 with i := 1 to 5.
        for i in range(1, 6):
            # 1. Router_1 registers for 15 multicast addresses, MASi.
            self.assertEqual((0, []), router1.register_multicast_listener(*(MAS[i][1:]),
                                                                          timeout=consts.MLR_TIMEOUT_MIN))
        self.simulator.go(5)

        # Loop steps 5 to 6 with i := 1 to 5.
        for i in range(1, 6):
            # 5. Host sends a ping packet to the multicast address,
            # MASi[ 3 * i - 1], on the backbone link.
            self.assertFalse(
                host.ping(MAS[i][i * 3 - 1],
                          backbone=True,
                          ttl=64,
                          interface=host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))

            self.simulator.go(3)

        # 7. Host multicasts a packet to the multicast addresses, MA2.
        self.assertFalse(
            host.ping(MA2, backbone=True, ttl=64, interface=host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.simulator.go(3)

        self.collect_ipaddrs()
        self.collect_rloc16s()
        self.collect_rlocs()
        self.collect_extra_vars()
        self.collect_leader_aloc(BR_1)

    def verify(self, pv: pktverify.packet_verifier.PacketVerifier):
        pkts = pv.pkts
        vars = pv.vars
        pv.summary.show()

        # Ensure the topology is formed correctly
        pv.verify_attached('Router_1', 'BR_1')

        # Loop steps 1 to 4 with i := 1 to 5.
        for i in range(1, 6):
            # 1. Router_1 registers for 15 multicast addresses, MASi.
            # [Automatic result:]
            # Unicasts an MLR.req CoAP request to BR_1 as follows:
            # coap://[<BR_1 RLOC> or <PBBR ALOC>]:MM/n/mr
            # Where the payload contains:
            # IPv6 Addresses TLV: MASi (15 addresses)
            _pkt = pkts.filter_wpan_src64(vars['Router_1']) \
                .filter_ipv6_2dsts(vars['BR_1_RLOC'], consts.PBBR_ALOC) \
                .filter_coap_request('/n/mr') \
                .filter(lambda p: set(p.thread_meshcop.tlv.ipv6_addr) == set(MAS[i][1:])) \
                .must_next()

            # 2. BR_1 Responds to the multicast registration.
            # BR_1 unicasts an MLR.rsp CoAP response to Router_1 as follows:
            # 2.04 changed
            # Where the payload contains:
            # Status TLV: ST_MLR_SUCCESS
            pkts.copy().filter_wpan_src64(vars['BR_1']) \
                .filter_ipv6_dst(_pkt.ipv6.src) \
                .filter_coap_ack('/n/mr') \
                .filter(lambda p: p.coap.mid == _pkt.coap.mid and p.thread_nm.tlv.status == 0) \
                .must_next()

            # 3. BR_1 informs other BBRs on the network of the multicast
            # registrations.
            # BR_1 multicasts a BMLR.ntf CoAP request to the Backbone Link,
            # as follows:
            # coap://[<All network BBRs multicast>]:BB/b/bmr
            # Where the payload contains:
            # IPv6 Addresses TLV:  MASi (15 addresses)
            # Timeout TLV:  default MLR timeout of BR_1
            pkts.filter_eth_src(vars['BR_1_ETH']) \
                .filter_ipv6_dst(config.ALL_NETWORK_BBRS_ADDRESS) \
                .filter_coap_request('/b/bmr') \
                .filter(
                lambda p: set(p.thread_meshcop.tlv.ipv6_addr) == set(MAS[i][1:]) and
                          p.thread_bl.tlv.timeout == consts.MLR_TIMEOUT_MIN) \
                .must_next()

            # 4. BR_1 multicasts an MLDv2 message of type “Version 2
            # Multicast Listener Report” (see [RFC 3810] Section 5.2).
            # Where:
            # Nr of Mcast Address Records(M) >= 15
            # Multicast Address Record[j]:
            # Each of the j := 1 ... 15 Multicast Address Record containing an
            # address of the set MASi contains the following:
            # Record Type: 4 (CHANGE_TO_EXCLUDE_MODE)
            # Number of Sources (N): 0
            # Multicast Address: MASi[j]
            # Alternatively, the DUT may also send multiple of above messages
            # each with a portion of the 15 addresses MASi.
            # In this case the Nr of Mcast Address Records can be < 15
            # but the sum over all messages MUST be >= 15.
            # TODO

        # Loop steps 5 to 6 with i := 1 to 5.
        for i in range(1, 6):
            # 5. Host sends a ping packet to the multicast address,
            # MASi[ 3 * i - 1], on the backbone link.
            _pkt = pkts.filter_eth_src(vars['Host_ETH']) \
                .filter_ipv6_dst(MAS[i][3 * i - 1]) \
                .filter_ping_request() \
                .must_next()

            # 6. BR_1 forwards the ping packet of the previous step to its
            # Thread Network encapsulated in an MPL packet.
            pkts.copy().filter_wpan_src64(vars['BR_1']) \
                .filter_AMPLFMA(mpl_seed_id=vars['BR_1_RLOC']) \
                .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
                .must_next()

        # 7. Host multicasts a packet to the multicast addresses, MA2.
        _pkt = pkts.filter_eth_src(vars['Host_ETH']) \
            .filter_ipv6_dst(MA2) \
            .filter_ping_request() \
            .must_next()

        # 8. BR_1 does not forward the packet to its Thread Network.
        pkts.filter_wpan_src64(vars['BR_1']) \
            .filter_AMPLFMA() \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_not_next()


if __name__ == '__main__':
    unittest.main()
