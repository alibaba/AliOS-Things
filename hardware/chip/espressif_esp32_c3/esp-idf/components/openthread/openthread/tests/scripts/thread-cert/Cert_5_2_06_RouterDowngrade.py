#!/usr/bin/env python3
#
#  Copyright (c) 2016, The OpenThread Authors.
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

import command
import config
import mle
import thread_cert
import thread_cert
from pktverify.consts import MLE_PARENT_REQUEST, MLE_CHILD_ID_REQUEST, ADDR_REL_URI, SOURCE_ADDRESS_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV, NL_MAC_EXTENDED_ADDRESS_TLV, NL_RLOC16_TLV, COAP_CODE_ACK
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
DUT_ROUTER1 = 2
ROUTER2 = 3
ROUTER23 = 24

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify that the DUT will downgrade
# to a REED when the network becomes too dense and the Router Downgrade
# Threshold conditions are met.
#
# Test Topology:
# -------------
#           Leader
#         /         \
# Router_1 [DUT] ... Router_23
#
# DUT Types:
# ----------
#  Router


class Cert_5_2_06_RouterDowngrade(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        DUT_ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        4: {
            'name': 'ROUTER_3',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        5: {
            'name': 'ROUTER_4',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        6: {
            'name': 'ROUTER_5',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        7: {
            'name': 'ROUTER_6',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        8: {
            'name': 'ROUTER_7',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        9: {
            'name': 'ROUTER_8',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        10: {
            'name': 'ROUTER_9',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        11: {
            'name': 'ROUTER_10',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        12: {
            'name': 'ROUTER_11',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        13: {
            'name': 'ROUTER_12',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        14: {
            'name': 'ROUTER_13',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        15: {
            'name': 'ROUTER_14',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        16: {
            'name': 'ROUTER_15',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        17: {
            'name': 'ROUTER_16',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        18: {
            'name': 'ROUTER_17',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        19: {
            'name': 'ROUTER_18',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        20: {
            'name': 'ROUTER_19',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        21: {
            'name': 'ROUTER_20',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        22: {
            'name': 'ROUTER_21',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        23: {
            'name': 'ROUTER_22',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
        ROUTER23: {
            'name': 'ROUTER_23',
            'mode': 'rdn',
            'router_downgrade_threshold': 32,
            'router_upgrade_threshold': 32
        },
    }

    def test(self):
        # 1 Ensure topology is formed correctly without ROUTER23.
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        for i in range(2, 24):
            self.nodes[i].start()
            self.simulator.go(5)
            self.assertEqual(self.nodes[i].get_state(), 'router')
        self.collect_rloc16s()

        # All reference testbed devices have been configured with downgrade threshold as 32 except DUT_ROUTER1,
        # so we don't need to ensure ROUTER23 has a better link quality on
        # posix.
        self.nodes[ROUTER23].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER23].get_state(), 'router')

        self.simulator.go(10)
        self.assertEqual(self.nodes[DUT_ROUTER1].get_state(), 'child')
        self.collect_rlocs()

        router1_rloc = self.nodes[DUT_ROUTER1].get_ip6_address(config.ADDRESS_TYPE.RLOC)
        self.assertTrue(self.nodes[LEADER].ping(router1_rloc))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_1_RLOC = pv.vars['ROUTER_1_RLOC']
        ROUTER_1_RLOC16 = pv.vars['ROUTER_1_RLOC16']

        # Step 1: Ensure topology is formed correctly

        for i in range(1, 24):
            pv.verify_attached('ROUTER_%d' % i)

        # Step 3: Allow enough time for the DUT to get Network Data Updates
        #         and resign its Router ID.
        #         The DUT MUST first reconnect to the network as a Child by
        #         sending properly formatted Parent Request and Child ID Request
        #         messages.
        #         Once the DUT attaches as a Child, it MUST send an Address
        #         Release Message to the Leader:
        #         CoAP Request URI
        #             coap://[<leader address>]:MM/a/ar
        #         CoAP Payload
        #             MAC Extended Address TLV
        #             RLOC16 TLV

        pkts.filter_wpan_src64(ROUTER_1).\
            filter_LLARMA().\
            filter_mle_cmd(MLE_PARENT_REQUEST).\
            filter(lambda p: {
                              CHALLENGE_TLV,
                              MODE_TLV,
                              SCAN_MASK_TLV,
                              VERSION_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.ipv6.hlim == 255 and\
                   p.mle.tlv.scan_mask.r == 1 and\
                   p.mle.tlv.scan_mask.e == 0
                  ).\
            must_next()

        _pkt = pkts.filter_wpan_src64(ROUTER_1).\
            filter_mle_cmd(MLE_CHILD_ID_REQUEST).\
            filter(lambda p: {
                              LINK_LAYER_FRAME_COUNTER_TLV,
                              MODE_TLV,
                              RESPONSE_TLV,
                              TIMEOUT_TLV,
                              TLV_REQUEST_TLV,
                              ADDRESS16_TLV,
                              NETWORK_DATA_TLV,
                              VERSION_TLV
                    } <= set(p.mle.tlv.type) and\
                   p.mle.tlv.addr16 is nullField and\
                   p.thread_nwd.tlv.type is nullField
                  ).\
            must_next()
        _pkt.must_not_verify(lambda p: (ADDRESS_REGISTRATION_TLV) in p.mle.tlv.type)

        pkts.filter_wpan_src64(ROUTER_1).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_request(ADDR_REL_URI).\
            filter(lambda p: {
                              NL_MAC_EXTENDED_ADDRESS_TLV,
                              NL_RLOC16_TLV
                              } == set(p.coap.tlv.type) and\
                   p.thread_address.tlv.rloc16 == ROUTER_1_RLOC16
                   ).\
           must_next()

        # Step 4: Leader receives Address Release messages and sends a2.04
        #         Changed CoAP response.

        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(ROUTER_1_RLOC).\
            filter_coap_ack(ADDR_REL_URI).\
            must_next()

        # Step 5: ROUTER_1 responds with ICMPv6 Echo Reply
        _pkt = pkts.filter_ipv6_src_dst(LEADER_RLOC, ROUTER_1_RLOC).\
                    filter_ping_request().\
                    must_next()
        pkts.filter_ipv6_src_dst(ROUTER_1_RLOC, LEADER_RLOC).\
            filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            must_next()


if __name__ == '__main__':
    unittest.main()
