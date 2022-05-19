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
from pktverify.consts import MA1, MA2
import config
import thread_cert

# Test description:
# The purpose of this test case is to verify that a Primary BBR correctly
# decrements the Hop Limit field by 1 if packet is forwarded with MPL, and if
# forwarded from Thread Network (using MPL) to LAN. This test also verifies that
# the BR drops a packet with Hop Limit 0. It also checks the use of IPv6 packets
# that span multiple 6LoWPAN fragments.
#
# Topology:
#    ----------------(eth)------------------
#           |                        |
#         BR_1 (Leader)              |
#           |                       HOST
#           |
#          ROUTER_1
#

BR_1 = 1
ROUTER_1 = 2
HOST = 3
ICMP_HEADER_LEN = 8


class MATN_12_HopLimitProcessing(thread_cert.TestCase):
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

        br1.start()
        self.simulator.go(5)
        self.assertEqual('leader', br1.get_state())
        self.assertTrue(br1.is_primary_backbone_router)

        router1.start()
        self.simulator.go(5)
        self.assertEqual('router', router1.get_state())

        host.start(start_radvd=False)
        self.simulator.go(10)

        # Router_1 registers a multicast address, MA1.
        router1.add_ipmaddr(MA1)
        self.simulator.go(5)

        # 1. Host multicasts a ping packet to the multicast address, MA1, with
        # the IPv6 Hop Limit field set to 59. The size of the payload is 130
        # bytes.
        self.assertTrue(
            host.ping(MA1,
                      backbone=True,
                      ttl=59,
                      size=130,
                      interface=host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.simulator.go(5)

        # 4. Host multicasts a ping packet to the multicast address, MA1, with
        # the IPv6 Hop Limit field set to 1. The size of the payload is 130
        # bytes.
        self.assertFalse(
            host.ping(MA1,
                      backbone=True,
                      ttl=1,
                      size=130,
                      interface=host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))
        self.simulator.go(5)

        # 6. Router_1 sends a ping packet encapsulated in an MPL packet to the
        # multicast address, MA2, with the Hop Limit field of the inner packet
        # set to 159. The size of the payload is 130 bytes.
        self.assertFalse(router1.ping(MA2, hoplimit=159, size=130))
        self.simulator.go(5)

        # 8. Router_1 sends a ping packet encapsulated in an MPL multicast
        # packet to the multicast address, MA2, with the Hop Limit field of
        # the inner (encapsulated) packet set to 2. The size of the payload is
        # 130 bytes.
        self.assertFalse(router1.ping(MA2, hoplimit=2, size=130))
        self.simulator.go(5)

        # 10. Router_1 sends a ping packet encapsulated in an MPL multicast
        # packet to the multicast address, MA2, with the Hop Limit field of the
        # inner packet set to 1. The size of the payload is 130 bytes.
        self.assertFalse(router1.ping(MA2, hoplimit=1, size=130))
        self.simulator.go(5)

        # 11. Router_1 sends a ping packet encapsulated in an MPL multicast
        # packet to the multicast address, MA2, with the Hop Limit field of the
        # inner packet set to 0.
        self.assertFalse(router1.ping(MA2, hoplimit=0, size=130))
        self.simulator.go(5)

        self.collect_ipaddrs()
        self.collect_rloc16s()
        self.collect_rlocs()
        self.collect_extra_vars()

    def verify(self, pv: pktverify.packet_verifier.PacketVerifier):
        pkts = pv.pkts
        vars = pv.vars
        pv.summary.show()
        logging.info(f'vars = {vars}')

        # Ensure the topology is formed correctly
        pv.verify_attached('Router_1', 'BR_1')

        # 1. Host multicasts a ping packet to the multicast address, MA1, with
        # the IPv6 Hop Limit field set to 59. The size of the payload is 130
        # bytes.
        _pkt = pkts.filter_eth_src(vars['Host_ETH']) \
            .filter_ipv6_dst(MA1) \
            .filter_ping_request() \
            .filter(lambda
                        p: p.ipv6.hlim == 59 and p.ipv6.plen == 130 + ICMP_HEADER_LEN) \
            .must_next()

        # 2. BR_1 forwards the ping packet to Router_1 as an MPL packet
        # encapsulating the IPv6 packet with the Hop Limit field of the inner
        # packet set to 58.
        _pkt2 = pkts.filter_wpan_src64(vars['BR_1']) \
            .filter_AMPLFMA(mpl_seed_id=vars['BR_1_RLOC']) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .filter(lambda
                        p: p.ipv6inner.hlim == 58 and p.ipv6inner.plen == 130 + ICMP_HEADER_LEN) \
            .must_next()

        # 3. Router_1 receives the multicast ping packet.
        pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_ipv6_dst(_pkt.ipv6.src) \
            .filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier) \
            .must_next()

        # 4. Host multicasts a ping packet to the multicast address, MA1, with
        # the IPv6 Hop Limit field set to 1. The size of the payload is 130
        # bytes.
        _pkt = pkts.filter_eth_src(vars['Host_ETH']) \
            .filter_ipv6_dst(MA1) \
            .filter_ping_request() \
            .filter(
            lambda
                p: p.ipv6.hlim == 1 and p.ipv6.plen == 130 + ICMP_HEADER_LEN) \
            .must_next()

        # 5. BR_1 does not forward the ping packet.
        pkts.filter_wpan_src64(vars['BR_1']) \
            .filter_AMPLFMA(mpl_seed_id=vars['BR_1']) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_not_next()

        # 6. Router_1 sends a ping packet encapsulated in an MPL packet to the
        # multicast address, MA2, with the Hop Limit field of the inner
        # packet set to 159. The size of the payload is 130 bytes.
        _pkt = pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_AMPLFMA(mpl_seed_id=vars['Router_1_RLOC']) \
            .filter_ping_request() \
            .filter(lambda p: p.ipv6inner.dst == MA2 and
                              p.ipv6inner.hlim == 159 and
                              p.ipv6inner.plen == 130 + ICMP_HEADER_LEN) \
            .must_next()

        # 7. BR_1 forwards the multicast ping packet to the LAN with the Hop
        # Limit field set to 158.
        pkts.filter_eth_src(vars['BR_1_ETH']) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .filter(lambda p: p.ipv6.hlim == 158) \
            .must_next()

        # 8. Router_1 sends a ping packet encapsulated in an MPL multicast
        # packet to the multicast address, MA2, with the Hop Limit field of the
        # inner packet set to 2. The size of the payload is 130 bytes
        _pkt = pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_AMPLFMA(mpl_seed_id=vars['Router_1_RLOC']) \
            .filter_ping_request() \
            .filter(lambda p: p.ipv6inner.dst == MA2 and
                              p.ipv6inner.hlim == 2 and
                              p.ipv6inner.plen == 130 + ICMP_HEADER_LEN) \
            .must_next()

        # 9. BR_1 forwards the multicast packet to the LAN with the Hop Limit
        # field set to 1.
        pkts.filter_eth_src(vars['BR_1_ETH']) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .filter(lambda p: p.ipv6.hlim == 1) \
            .must_next()

        # 10. Router_1 sends a ping packet encapsulated in an MPL multicast
        # packet to the multicast address, MA2, with the Hop Limit field of the
        # inner packet set to 1. The size of the payload is 130 bytes.
        _pkt = pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_AMPLFMA(mpl_seed_id=vars['Router_1_RLOC']) \
            .filter_ping_request() \
            .filter(lambda p: p.ipv6inner.dst == MA2 and
                              p.ipv6inner.hlim == 1 and
                              p.ipv6inner.plen == 130 + ICMP_HEADER_LEN) \
            .must_next()

        # 11. BR_1 does not forward the ping packet to the LAN.
        pkts.filter_eth_src(vars['BR_1_ETH']) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_not_next()

        # 12. Router_1 sends a ping packet encapsulated in an MPL multicast
        # packet to the multicast address, MA2, with the Hop Limit field of the
        # inner packet set to 0.
        _pkt = pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_AMPLFMA(mpl_seed_id=vars['Router_1_RLOC']) \
            .filter_ping_request() \
            .filter(lambda p: p.ipv6inner.dst == MA2 and
                              p.ipv6inner.hlim == 0 and
                              p.ipv6inner.plen == 130 + ICMP_HEADER_LEN) \
            .must_next()

        # 13. BR_1 does not forward the ping packet to the LAN.
        pkts.filter_eth_src(vars['BR_1_ETH']) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_not_next()


if __name__ == '__main__':
    unittest.main()
