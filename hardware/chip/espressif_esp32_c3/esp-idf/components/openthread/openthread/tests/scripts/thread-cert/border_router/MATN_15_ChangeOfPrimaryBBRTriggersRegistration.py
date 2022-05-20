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
# The purpose of this test case is to verify that a Thread End Device detects a
# change of Primary BBR device and triggers a re-registration to its multicast
# groups.
#
# Topology:
#
#
#    ----------------(eth)--------------------
#           |
#         BR_1 (Leader) -----BR_2
#           |                  |
#           |                  |
#           |                  |
#         Router_1-------------+
#           |
#           |
#          TD
#

BR_1 = 1
BR_2 = 2
ROUTER_1 = 3
TD = 4

REG_DELAY = 10

NETWORK_ID_TIMEOUT = 120
WAIT_TIME_ALLOWANCE = 60


class MATN_15_ChangeOfPrimaryBBRTriggersRegistration(thread_cert.TestCase):
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
            'allowlist': [BR_1, BR_2, TD],
            'version': '1.2',
            'partition_id': 1,
        },
        TD: {
            'name': 'TD',
            'allowlist': [ROUTER_1],
            'version': '1.2',
            'partition_id': 1,
        },
    }

    def test(self):
        br1 = self.nodes[BR_1]
        br2 = self.nodes[BR_2]
        router1 = self.nodes[ROUTER_1]
        td = self.nodes[TD]

        br1.set_backbone_router(reg_delay=REG_DELAY, mlr_timeout=consts.MLR_TIMEOUT_MIN)
        br1.start()
        self.simulator.go(5)
        self.assertEqual('leader', br1.get_state())
        self.assertTrue(br1.is_primary_backbone_router)

        br2.set_backbone_router(reg_delay=REG_DELAY, mlr_timeout=consts.MLR_TIMEOUT_MIN)
        router1.start()
        self.simulator.go(5)
        self.assertEqual('router', router1.get_state())

        td.start()
        self.simulator.go(5)
        self.assertEqual('router', td.get_state())

        br2.start()
        self.simulator.go(5)
        self.assertEqual('router', br2.get_state())
        self.assertFalse(br2.is_primary_backbone_router)

        # TD registers for a multicast address, MA1.
        td.add_ipmaddr(MA1)
        self.simulator.go(20)

        # 1. Disable BR_1 such that BR_2 becomes the Primary BBR for the Thread
        # Network.
        br1.disable_backbone_router()
        br1.thread_stop()
        br1.interface_down()
        self.simulator.go(NETWORK_ID_TIMEOUT + WAIT_TIME_ALLOWANCE)

        # Make sure that BR_2 becomes the primary BBR
        self.assertEqual('disabled', br1.get_state())
        self.assertFalse(br1.is_primary_backbone_router)
        self.assertTrue(br2.is_primary_backbone_router)

        self.collect_ipaddrs()
        self.collect_rloc16s()
        self.collect_rlocs()
        self.collect_extra_vars()

    def verify(self, pv: pktverify.packet_verifier.PacketVerifier):
        pkts = pv.pkts
        vars = pv.vars
        pv.summary.show()

        # Ensure the topology is formed correctly
        pv.verify_attached('Router_1', 'BR_1')

        # 2. TD automatically detects the Primary BBR change and registers for
        # multicast address, MA1, at BR_2.
        # TD unicasts an MLR.req CoAP request as follows:
        # coap://[<BR_2 RLOC or PBBR ALOC>]:MM/n/mr
        # Where the payload contains:
        # IPv6 Addresses TLV: MA1
        _pkt = pkts.filter_wpan_src64(vars['TD']) \
            .filter_ipv6_2dsts(vars['BR_2_RLOC'], consts.PBBR_ALOC) \
            .filter_coap_request('/n/mr') \
            .filter(lambda p: p.thread_meshcop.tlv.ipv6_addr == [MA1]) \
            .must_next()

        # 3. Router_1 forwards the registration request to BR_2.
        pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_ipv6_2dsts(vars['BR_2_RLOC'], consts.PBBR_ALOC) \
            .filter_coap_request('/n/mr') \
            .filter(lambda
                        p: p.thread_meshcop.tlv.ipv6_addr == [MA1] and
                           p.coap.mid == _pkt.coap.mid) \
            .must_next()

        # 4. BR_2 unicasts an MLR.rsp CoAP response to TD as: 2.04 changed.
        # Where the payload contains:
        # Status TLV: ST_MLR_SUCCESS
        pkts.filter_wpan_src64(vars['BR_2']) \
            .filter_ipv6_dst(_pkt.ipv6.src) \
            .filter_coap_ack('/n/mr') \
            .filter(lambda
                        p: p.coap.mid == _pkt.coap.mid and
                           p.thread_nm.tlv.status == 0) \
            .must_next()

        # 5. Router_1 forwards the response to TD.
        pkts.filter_wpan_src64(vars['Router_1']) \
            .filter_ipv6_dst(_pkt.ipv6.src) \
            .filter_coap_ack('/n/mr') \
            .filter(lambda
                        p: p.coap.mid == _pkt.coap.mid and
                           p.thread_nm.tlv.status == 0) \
            .must_next()


if __name__ == '__main__':
    unittest.main()
