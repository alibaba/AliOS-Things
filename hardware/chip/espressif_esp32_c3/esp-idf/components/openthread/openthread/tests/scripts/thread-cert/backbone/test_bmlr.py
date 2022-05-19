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
# This test verifies that PBBR sends BMLR.ntf correctly when multicast addresses are registered.
#
# Topology:
#    ---- -(eth)-------
#       |        |
#      PBBR----SBBR
#        \     /
#        Router1---Commissioner
#

import unittest

import thread_cert
from pktverify.packet_verifier import PacketVerifier

PBBR = 1
SBBR = 2
ROUTER1 = 3
COMMISSIONER = 4

REREG_DELAY = 4  # Seconds
MLR_TIMEOUT = 300  # Seconds
CUSTOM_MLR_TIMEOUT = 1000  # Seconds

MA1 = 'ff04::1'
MA2 = 'ff04::2'
MA3 = 'ff04::3'
MA4 = 'ff04::4'
MA5 = 'ff04::5'


class BBR_5_11_01(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        PBBR: {
            'name': 'PBBR',
            'allowlist': [SBBR, ROUTER1],
            'is_otbr': True,
            'version': '1.2',
        },
        SBBR: {
            'name': 'SBBR',
            'allowlist': [PBBR, ROUTER1],
            'is_otbr': True,
            'version': '1.2',
        },
        ROUTER1: {
            'name': 'ROUTER1',
            'allowlist': [PBBR, SBBR, COMMISSIONER],
            'version': '1.2',
        },
        COMMISSIONER: {
            'name': 'COMMISSIONER',
            'allowlist': [ROUTER1],
            'version': '1.2',
        }
    }

    def test(self):
        self.nodes[PBBR].start()
        self.wait_node_state(PBBR, 'leader', 5)
        self.nodes[PBBR].set_backbone_router(reg_delay=REREG_DELAY, mlr_timeout=MLR_TIMEOUT)
        self.nodes[PBBR].enable_backbone_router()
        self.wait_until(lambda: self.nodes[PBBR].is_primary_backbone_router, 5)

        self.nodes[SBBR].start()
        self.wait_node_state(SBBR, 'router', 5)
        self.nodes[SBBR].set_backbone_router(reg_delay=REREG_DELAY, mlr_timeout=MLR_TIMEOUT)
        self.nodes[SBBR].enable_backbone_router()
        self.simulator.go(5)
        self.assertFalse(self.nodes[SBBR].is_primary_backbone_router)

        self.nodes[ROUTER1].start()
        self.wait_node_state(ROUTER1, 'router', 5)

        self.nodes[COMMISSIONER].start()
        self.wait_node_state(COMMISSIONER, 'router', 5)

        self.wait_route_established(COMMISSIONER, PBBR)

        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(10)
        self.assertEqual('active', self.nodes[COMMISSIONER].commissioner_state())

        self.nodes[PBBR].add_ipmaddr(MA1)
        self.simulator.go(REREG_DELAY)
        self.nodes[ROUTER1].add_ipmaddr(MA2)
        self.simulator.go(REREG_DELAY)

        # Commissioner registers MA3 with default timeout
        self.assertEqual((0, []), self.nodes[COMMISSIONER].register_multicast_listener(MA3, timeout=None))
        # Commissioner registers MA4 with a custom timeout
        self.assertEqual((0, []), self.nodes[COMMISSIONER].register_multicast_listener(MA4,
                                                                                       timeout=CUSTOM_MLR_TIMEOUT))
        # Commissioner unregisters MA5
        self.assertEqual((0, []), self.nodes[COMMISSIONER].register_multicast_listener(MA5, timeout=0))

        self.collect_ipaddrs()
        self.collect_rloc16s()

    def verify(self, pv: PacketVerifier):
        pkts = pv.pkts
        pv.add_common_vars()
        pv.summary.show()
        pv.verify_attached('ROUTER1')

        ROUTER1 = pv.vars['ROUTER1']
        COMMISSIONER = pv.vars['COMMISSIONER']
        PBBR_ETH = pv.vars['PBBR_ETH']
        SBBR_ETH = pv.vars['SBBR_ETH']

        # Verify SBBR must not send `/b/bmr` during the test.
        pkts.filter_eth_src(SBBR_ETH).filter_coap_request('/b/bmr').must_not_next()

        # Verify PBBR sends `/b/bmr` on the Backbone link for MA1 with default timeout.
        pkts.filter_eth_src(PBBR_ETH).filter_coap_request('/b/bmr').must_next().must_verify(f"""
            thread_meshcop.tlv.ipv6_addr == ['{MA1}']
            and thread_bl.tlv.timeout == {MLR_TIMEOUT}
            and ipv6.src.is_link_local
        """)

        # Router registers MA2 with default timeout
        pkts.filter_wpan_src64(ROUTER1).filter_coap_request('/n/mr').must_next().must_verify(f"""
            thread_meshcop.tlv.ipv6_addr == ['{MA2}']
            and thread_bl.tlv.timeout is null
        """)
        # Verify PBBR sends `/b/bmr` on the Backbone link for MA2 with default timeout.
        pkts.filter_eth_src(PBBR_ETH).filter_coap_request('/b/bmr').must_next().must_verify(f"""
            thread_meshcop.tlv.ipv6_addr == ['{MA2}']
            and thread_bl.tlv.timeout == {MLR_TIMEOUT}
            and ipv6.src.is_link_local
        """)

        # Commissioner registers MA3 with deafult timeout
        pkts.filter_wpan_src64(COMMISSIONER).filter_coap_request('/n/mr').must_next().must_verify(f"""
            thread_meshcop.tlv.ipv6_addr == ['{MA3}']
            and thread_bl.tlv.timeout is null
        """)
        # Verify PBBR sends `/b/bmr` on the Backbone link for MA3 with default timeout.
        pkts.filter_eth_src(PBBR_ETH).filter_coap_request('/b/bmr').must_next().must_verify(f"""
            thread_meshcop.tlv.ipv6_addr == ['{MA3}']
            and thread_bl.tlv.timeout == {MLR_TIMEOUT}
            and ipv6.src.is_link_local
        """)

        # Commissioner registers MA4 with custom timeout
        pkts.filter_wpan_src64(COMMISSIONER).filter_coap_request('/n/mr').must_next().must_verify(f"""
            thread_meshcop.tlv.ipv6_addr == ['{MA4}']
            and thread_nm.tlv.timeout == {CUSTOM_MLR_TIMEOUT}
        """)
        # Verify PBBR sends `/b/bmr` on the Backbone link for MA4 with custom timeout.
        pkts.filter_eth_src(PBBR_ETH).filter_coap_request('/b/bmr').must_next().must_verify(f"""
            thread_meshcop.tlv.ipv6_addr == ['{MA4}']
            and thread_bl.tlv.timeout == {CUSTOM_MLR_TIMEOUT}
            and ipv6.src.is_link_local
        """)

        # Commissioner unregisters MA5
        pkts.filter_wpan_src64(COMMISSIONER).filter_coap_request('/n/mr').must_next().must_verify(f"""
            thread_meshcop.tlv.ipv6_addr == ['{MA5}']
            and thread_nm.tlv.timeout == 0
        """)
        # Verify PBBR not sends `/b/bmr` on the Backbone link for MA5.
        pkts.filter_eth_src(PBBR_ETH).filter_coap_request('/b/bmr').filter(f"""
            thread_meshcop.tlv.ipv6_addr == ['{MA5}']
            and ipv6.src.is_link_local
        """).must_not_next()


if __name__ == '__main__':
    unittest.main()
