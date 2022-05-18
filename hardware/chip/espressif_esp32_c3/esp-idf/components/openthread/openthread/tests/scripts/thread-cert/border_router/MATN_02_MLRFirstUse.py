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
from pktverify import packet_verifier, packet_filter
from pktverify.consts import MA1, MA1g, MA2, PBBR_ALOC
import config
import thread_cert

# Test description:
# The purpose of this test case is to verify that a Thread device is able to
# register a multicast address with a Primary BBR and that the Primary BBR can
# notify devices on the backbone link of the multicast address. The test also
# verifies that an IPv6 multicast packet originating from the backbone is
# correctly forwarded (in the Thread Network) to the device that registered the
# multicast address.
#
# Topology:
#    ----------------(eth)------------------
#           |                  |     |
#          BR1 (Leader) ----- BR2   HOST
#           |                  |
#           |                  |
#          TD --------(when TD is router)
#

BR_1 = 1
BR_2 = 2
TD = 3
HOST = 4

CHANNEL1 = 18

MLR_NOTIFICATION_TIMEOUT = 3600


class MATN_02_MLRFirstUse(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        BR_1: {
            'name': 'BR_1',
            'is_otbr': True,
            'allowlist': [BR_2, TD],
            'version': '1.2',
        },
        BR_2: {
            'name': 'BR_2',
            'allowlist': [BR_1, TD],
            'is_otbr': True,
            'version': '1.2',
        },
        TD: {
            'name': 'TD',
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
        td = self.nodes[TD]
        host = self.nodes[HOST]

        br1.start()
        self.simulator.go(5)
        self.assertEqual('leader', br1.get_state())
        self.assertTrue(br1.is_primary_backbone_router)

        td.start()
        self.simulator.go(5)
        self.assertEqual('router', td.get_state())

        br2.start()
        self.simulator.go(5)
        self.assertEqual('router', br2.get_state())

        host.start(start_radvd=False)
        self.simulator.go(10)

        # 1. TD registers for multicast address, MA1, at BR_1.
        td.add_ipmaddr(MA1)
        self.simulator.go(5)

        # 7. Host sends a ping packet to the multicast address, MA1. TD should
        # respond to the ping request.
        self.assertTrue(
            host.ping(MA1, backbone=True, ttl=10, interface=host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.simulator.go(5)

        # 11. Host sends a ping packet to the multicast address, MA2. No one
        # should respond.
        self.assertFalse(
            host.ping(MA2, backbone=True, ttl=10, interface=host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.simulator.go(5)

        # 14. Host sends a ping packet to the multicast address, MA1g. No one
        # should respond.
        self.assertFalse(
            host.ping(MA1g, backbone=True, ttl=10, interface=host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.simulator.go(5)

        # 17. Host sends a ping packet to the global unicast address of BR2. BR2
        # should respond.
        self.assertTrue(host.ping(br2.get_ip6_address(config.ADDRESS_TYPE.BACKBONE_GUA), backbone=True, ttl=10))
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
        pv.verify_attached('TD', 'BR_1')
        pv.verify_attached('BR_2')

        # 1. TD Registers for multicast address, MA1, at BR_1.
        # TD unicasts an MLR.req CoAP request to BR_1 as
        # "coap://[<BR_1 RLOC or PBBR ALOC>]:MM/n/mr".
        # The payload contains "IPv6Address TLV: MA1".
        pkts.filter_wpan_src64(vars['TD']) \
            .filter_ipv6_2dsts(vars['BR_1_RLOC'], PBBR_ALOC) \
            .filter_coap_request('/n/mr') \
            .filter(lambda p: p.thread_meshcop.tlv.ipv6_addr == [MA1]) \
            .must_next()

        # 3. BR_1 responds to the multicast registration.
        # BR_1 unicasts an MLR.rsp CoAP response to TD as "2.04 changed".
        # The payload contains "Status TLV: ST_MLR_SUCCESS".
        pkts.copy().filter_wpan_src64(vars['BR_1']) \
            .filter_ipv6_dst(vars['TD_RLOC']) \
            .filter_coap_ack('/n/mr') \
            .filter(lambda p: p.thread_nm.tlv.status == 0) \
            .must_next()

        # 3a. BR_2 does not respond to the multicast registration.
        pkts.filter_wpan_src64(vars['BR_2']) \
            .filter_ipv6_dst(vars['TD_RLOC']) \
            .filter_coap_ack('/n/mr') \
            .must_not_next()

        # 4. BR_1 informs other BBRs on the network of multicast registration.
        # BR_1 multicasts a BMLR.ntf CoAP request to the Backbone Link including
        # to BR_2, as follows
        pkts.filter_eth_src(vars['BR_1_ETH']) \
            .filter_ipv6_dst(config.ALL_NETWORK_BBRS_ADDRESS) \
            .filter_coap_request('/b/bmr') \
            .filter(lambda
                        p: p.thread_meshcop.tlv.ipv6_addr == [MA1] and
                           p.thread_bl.tlv.timeout == MLR_NOTIFICATION_TIMEOUT) \
            .must_next()

        # 6. BR_1 multicasts an MLDv2 message to start listening to MA1.
        # BR_1 multicasts an MLDv2 message of type “Version 2 Multicast Listener
        # Report” (see [RFC 3810] Section 5.2). Where:
        # Nr of Mcast Address at least 1 Records (M): Multicast Address Record
        # The Multicast Address Record contains the following:
        # Record Type: 4 (CHANGE_TO_EXCLUDE_MODE)
        # Number of Sources (N): 0
        # Multicast Address: MA1
        # TODO: Implement this verification

        # 7. Host sends a ping packet to the multicast address, MA1.
        _pkt = pkts.filter_eth_src(vars['Host_ETH']) \
            .filter_ipv6_dst(MA1) \
            .filter_ping_request() \
            .must_next()

        # 8. BR_2 does not forward the ping packet with multicast address MA1 to
        # its Thread Network.
        pkts.filter_wpan_src64(vars['BR_2']) \
            .filter_AMPLFMA(mpl_seed_id=(vars['BR_2_RLOC'])) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_not_next()

        # 9. BR_1 forwards the ping packet to its Thread Network.
        # BR_1 forwards the ping packet with multicast address, MA1, to its
        # Thread Network encapsulated in an MPL packet, where:
        # MPL Seed ID:  If Source outer IP header = BR_1 RLOC, SeedID length = 0
        #               Else, SeedID length = 1, and Seed ID = BR_1 RLOC16
        pkts.filter_wpan_src64(vars['BR_1']) \
            .filter_AMPLFMA(mpl_seed_id=(vars['BR_1_RLOC'])) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_next()

        # 10. TD receives the multicast ping packet and sends a ping response
        # packet back to Host.
        # TD receives the MPL packet containing an encapsulated ping packet to
        # MA1, sent by Host, and unicasts a ping response packet back to Host.
        pkts.filter_wpan_src64(vars['TD']) \
            .filter_ipv6_dst(_pkt.ipv6.src) \
            .filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier) \
            .must_next()

        # 11. Host sends a ping packet to the multicast address, MA2.
        _pkt = pkts.filter_eth_src(vars['Host_ETH']) \
            .filter_ipv6_dst(MA2) \
            .filter_ping_request() \
            .must_next()

        # 12. BR_2 does not forward the ping packet with multicast address, MA2,
        # to the Thread Network in whatever way.
        pkts.filter_wpan_src64(vars['BR_2']) \
            .filter_AMPLFMA(mpl_seed_id=(vars['BR_2_RLOC'])) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_not_next()

        # 13. BR_1 does not forward the ping packet with multicast address, MA2,
        # to the Thread Network in whatever way.
        pkts.filter_wpan_src64(vars['BR_1']) \
            .filter_AMPLFMA(mpl_seed_id=(vars['BR_1_RLOC'])) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_not_next()

        # 14. Host sends a ping packet to the multicast address, MA1g.
        _pkt = pkts.filter_eth_src(vars['Host_ETH']) \
            .filter_ipv6_dst(MA1g) \
            .filter_ping_request() \
            .must_next()

        # 15. BR_2 does not forward the ping packet with multicast address MA1g,
        # to the Thread Network in whatever way.
        pkts.filter_wpan_src64(vars['BR_2']) \
            .filter_AMPLFMA(mpl_seed_id=(vars['BR_2_RLOC'])) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_not_next()

        # 16. BR_1 does not forward the ping packet with multicast address MA1g,
        # to its Thread Network in whatever way.
        pkts.filter_wpan_src64(vars['BR_1']) \
            .filter_AMPLFMA(mpl_seed_id=(vars['BR_1_RLOC'])) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_not_next()

        # 17. Host sends a ping packet to the BR_2's global unicast address, Gg.
        _pkt = pkts.filter_eth_src(vars['Host_ETH']) \
            .filter_ipv6_dst(vars['BR_2_BGUA']) \
            .filter_ping_request() \
            .must_next()

        # 18. BR_2 receives and provides the ping response.
        # BR_2 Must send back the ping response to the Host.
        pkts.filter_eth_src(vars['BR_2_ETH']) \
            .filter_ipv6_dst(vars['Host_BGUA']) \
            .filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier) \
            .must_next()


if __name__ == '__main__':
    unittest.main()
