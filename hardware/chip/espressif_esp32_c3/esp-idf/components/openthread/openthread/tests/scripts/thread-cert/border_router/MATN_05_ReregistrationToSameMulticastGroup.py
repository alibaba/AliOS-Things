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
from pktverify.consts import MA1, PBBR_ALOC
import config
import thread_cert

# Test description:
# The purpose of this test case is to verify that a Primary BBR (DUT) can manage
# a re-registration of a device on its network to remain receiving multicasts.
# The test also verifies the usage of UDP multicast packets across backbone and
# internal Thread network.
#
# Topology:
#    ----------------(eth)------------------
#           |                  |     |
#          BR_1 (Leader) ---- BR_2  HOST
#           |                  |
#           |                  |
#          Router_1 -----------+
#

BR_1 = 1
BR_2 = 2
ROUTER_1 = 3
HOST = 4

REG_DELAY = 10
UDP_HEADER_LENGTH = 8


class MATN_05_ReregistrationToSameMulticastGroup(thread_cert.TestCase):
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
        router1 = self.nodes[ROUTER_1]
        host = self.nodes[HOST]

        br1.set_backbone_router(reg_delay=REG_DELAY, mlr_timeout=consts.MLR_TIMEOUT_MIN)
        br1.start()
        self.simulator.go(10)
        self.assertEqual('leader', br1.get_state())
        self.assertTrue(br1.is_primary_backbone_router)

        router1.start()
        self.simulator.go(10)
        self.assertEqual('router', router1.get_state())

        br2.start()
        self.simulator.go(10)
        self.assertEqual('router', br2.get_state())
        self.assertFalse(br2.is_primary_backbone_router)

        host.start(start_radvd=False)
        self.simulator.go(10)

        # Router_1 registers for multicast address, MA1, at BR_1.
        router1.add_ipmaddr(MA1)
        self.simulator.go(5)

        # 1. Host sends a ping packet to the multicast address, MA1.
        self.assertTrue(
            host.ping(MA1, backbone=True, ttl=10, interface=host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))

        # Ensure Router_1 renews its multicast registration
        self.simulator.go(consts.MLR_TIMEOUT_MIN - 10)

        # 4. Within MLR_TIMEOUT_MIN seconds, Host sends a ping packet to the
        # multicast address, MA1. The destination port 5683 is used for the UDP
        # Multicast packet transmission.
        host.udp_send_host(data='PING', ipaddr=MA1, port=5683)
        self.simulator.go(5)

        # 6a. By internal means, Router_1 stops listening to the multicast
        # address, MA1.
        router1.del_ipmaddr(MA1)

        # 7. After (MLR_TIMEOUT_MIN+2) seconds, Host multicasts a ping packet to
        # multicast address, MA1, on the backbone link.
        self.simulator.go(consts.MLR_TIMEOUT_MIN + 2)
        self.assertFalse(
            host.ping(MA1, backbone=True, ttl=10, interface=host.get_ip6_address(config.ADDRESS_TYPE.ONLINK_ULA)[0]))

        self.collect_ipaddrs()
        self.collect_rloc16s()
        self.collect_rlocs()
        self.collect_leader_aloc(BR_1)
        self.collect_extra_vars()

    def verify(self, pv: pktverify.packet_verifier.PacketVerifier):
        pkts = pv.pkts
        vars = pv.vars
        pv.summary.show()
        logging.info(f'vars = {vars}')

        # Ensure the topology is formed correctly
        pv.verify_attached('Router_1', 'BR_1')
        pv.verify_attached('BR_2')

        # Initial registration
        # Router_1 registers for multicast address, MA1, at BR_1.
        # Router_1 unicasts an MLR.req CoAP request to BR_1 as
        # "coap://[<BR_1 RLOC or PBBR ALOC>]:MM/n/mr".
        # The payload contains "IPv6Address TLV: MA1".
        initial_registration_pkt = pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_ipv6_2dsts(vars['BR_1_RLOC'], PBBR_ALOC) \
            .filter_coap_request('/n/mr') \
            .filter(lambda p: p.thread_meshcop.tlv.ipv6_addr == [MA1]) \
            .must_next()

        # 1. Host sends a ping packet to the multicast address, MA1.
        _pkt = pkts.filter_eth_src(vars['Host_ETH']) \
            .filter_ipv6_dst(MA1) \
            .filter_ping_request() \
            .must_next()

        # 2. BR_1 forwards the ping packet with multicast address, MA1, to its
        # Thread Network encapsulated in an MPL packet.
        pkts.filter_wpan_src64(vars['BR_1']) \
            .filter_AMPLFMA(mpl_seed_id=vars['BR_1_RLOC']) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_next()

        # 3. Router_1 receives the MPL packet containing an encapsulated ping
        # packet to MA1, sent by Host, and unicasts a ping response packet back
        # to Host.
        pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_ipv6_dst(_pkt.ipv6.src) \
            .filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier) \
            .must_next()

        # 3a. Within MLR_TIMEOUT_MIN seconds of initial registration, Router_1
        # re-registers for multicast address, MA1, at BR_1.
        # Router_1 unicasts an MLR.req CoAP request to BR_1 as
        # "coap://[<BR_1 RLOC or PBBR ALOC>]:MM/n/mr".
        # The payload contains "IPv6Address TLV: MA1".
        pkts.copy() \
            .filter_wpan_src64(vars['Router_1']) \
            .filter_ipv6_2dsts(vars['BR_1_RLOC'], PBBR_ALOC) \
            .filter_coap_request('/n/mr') \
            .filter(lambda p: p.thread_meshcop.tlv.ipv6_addr == [MA1] and
                              p.sniff_timestamp <= initial_registration_pkt.sniff_timestamp + consts.MLR_TIMEOUT_MIN) \
            .must_next()

        # 4. Within MLR_TIMEOUT_MIN seconds, Host sends a ping packet to the
        # multicast address, MA1. The destination port 5683 is used for the UDP
        # Multicast packet transmission.
        _pkt = pkts.filter_eth_src(vars['Host_ETH']) \
            .filter_ipv6_dst(MA1) \
            .filter(lambda p: p.udp.length == UDP_HEADER_LENGTH + len('PING')
                              and p.udp.dstport == 5683) \
            .must_next()

        # 5. BR_1 forwards the UDP ping packet with multicast address, MA1, to
        # its Thread Network encapsulated in an MPL packet.
        pkts.filter_wpan_src64(vars['BR_1']) \
            .filter_AMPLFMA(mpl_seed_id=vars['BR_1_RLOC']) \
            .filter(lambda p: p.udp.length == _pkt.udp.length) \
            .must_next()

        # 6. Router_1 receives the ping packet.
        # Use the port 5683 (CoAP port) to verify that the
        # UDP Multicast packet is received.
        pkts.filter_wpan_src64(vars['Router_1']) \
            .filter(
            lambda p: p.udp.length == _pkt.udp.length and p.udp.dstport == 5683) \
            .must_next()

        # 7. After (MLR_TIMEOUT_MIN+2) seconds, Host multicasts a ping packet to
        # multicast address, MA1, on the backbone link.
        _pkt = pkts.filter_eth_src(vars['Host_ETH']) \
            .filter_ipv6_dst(MA1) \
            .filter_ping_request() \
            .must_next()

        # 8. BR_1 does not forward the ping packet with multicast address, MA1,
        # to its Thread Network.
        pkts.filter_wpan_src64(vars['BR_1']) \
            .filter_AMPLFMA(mpl_seed_id=vars['BR_1_RLOC']) \
            .filter_ping_request(identifier=_pkt.icmpv6.echo.identifier) \
            .must_not_next()


if __name__ == '__main__':
    unittest.main()
