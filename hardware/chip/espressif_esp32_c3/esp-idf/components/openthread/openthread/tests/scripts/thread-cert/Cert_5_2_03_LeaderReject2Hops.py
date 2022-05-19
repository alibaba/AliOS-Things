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

import mle
import network_layer
import thread_cert
from pktverify.consts import MLE_ADVERTISEMENT, MLE_PARENT_RESPONSE, MLE_CHILD_ID_RESPONSE, ADDR_SOL_URI, SOURCE_ADDRESS_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, ADDRESS16_TLV, ROUTE64_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV, NL_MAC_EXTENDED_ADDRESS_TLV, NL_RLOC16_TLV, NL_STATUS_TLV, NL_ROUTER_MASK_TLV, COAP_CODE_ACK, ADDR_SOL_NA, ADDR_SOL_SUCCESS
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

DUT_LEADER = 1
ROUTER_1 = 2
ROUTER_31 = 32
ROUTER_32 = 33

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to show that the DUT will
# only allow 32 active routers on the network and reject the
# Address Solicit Request from a 33rd router - that is
# 2-hops away - with a No Address Available status.
#
# Test Topology:
# -------------
#         Leader[DUT]
#         /    \
# Router_1 --- Router_31
#    |
#    |
# Router_32
#
# DUT Types:
# ----------
# Leader


class Cert_5_2_3_LeaderReject2Hops(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        DUT_LEADER: {
            'name':
                'LEADER',
            'mode':
                'rdn',
            'panid':
                0xface,
            'router_downgrade_threshold':
                33,
            'router_upgrade_threshold':
                32,
            'allowlist': [
                ROUTER_1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
                28, 29, 30, 31, ROUTER_31
            ]
        },
        ROUTER_1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER, ROUTER_32]
        },
        3: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        4: {
            'name': 'ROUTER_3',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        5: {
            'name': 'ROUTER_4',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        6: {
            'name': 'ROUTER_5',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        7: {
            'name': 'ROUTER_6',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        8: {
            'name': 'ROUTER_7',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        9: {
            'name': 'ROUTER_8',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        10: {
            'name': 'ROUTER_9',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        11: {
            'name': 'ROUTER_10',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        12: {
            'name': 'ROUTER_11',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        13: {
            'name': 'ROUTER_12',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        14: {
            'name': 'ROUTER_13',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        15: {
            'name': 'ROUTER_14',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        16: {
            'name': 'ROUTER_15',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        17: {
            'name': 'ROUTER_16',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        18: {
            'name': 'ROUTER_17',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        19: {
            'name': 'ROUTER_18',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        20: {
            'name': 'ROUTER_19',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        21: {
            'name': 'ROUTER_20',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        22: {
            'name': 'ROUTER_21',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        23: {
            'name': 'ROUTER_22',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        24: {
            'name': 'ROUTER_23',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        25: {
            'name': 'ROUTER_24',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        26: {
            'name': 'ROUTER_25',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        27: {
            'name': 'ROUTER_26',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        28: {
            'name': 'ROUTER_27',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        29: {
            'name': 'ROUTER_28',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        30: {
            'name': 'ROUTER_29',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        31: {
            'name': 'ROUTER_30',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        ROUTER_31: {
            'name': 'ROUTER_31',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [DUT_LEADER]
        },
        ROUTER_32: {
            'name': 'ROUTER_32',
            'mode': 'rdn',
            'router_downgrade_threshold': 33,
            'router_upgrade_threshold': 33,
            'allowlist': [ROUTER_1]
        },
    }

    def test(self):
        self.nodes[DUT_LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT_LEADER].get_state(), 'leader')

        for i in range(2, 32):
            self.nodes[i].start()
            self.simulator.go(5)
            self.assertEqual(self.nodes[i].get_state(), 'router')
        self.collect_rlocs()

        self.nodes[ROUTER_31].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER_31].get_state(), 'router')

        self.nodes[ROUTER_32].start()
        self.simulator.go(5)

    def verify(self, pv: PacketVerifier):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        ROUTER_31 = pv.vars['ROUTER_31']
        ROUTER_32 = pv.vars['ROUTER_32']

        # Step 1: Topology is created, the DUT is the Leader of the network
        #         and there is a total of 32 active routers, including the Leader.
        for i in range(1, 32):
            pv.verify_attached('ROUTER_%d' % i)

        # Step 2: Router_31 to attaches to the network and sends an Address
        #         Solicit Request to become an active router.

        _pkt = pkts.filter_wpan_src64(ROUTER_31).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_request(ADDR_SOL_URI).\
            filter(lambda p: {
                              NL_MAC_EXTENDED_ADDRESS_TLV,
                              NL_STATUS_TLV
                              } == set(p.coap.tlv.type)
                  ).\
            must_next()

        # Step 3: Leader sends an Address Solicit Response.
        #         Ensure the Address Solicit Response is properly formatted:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             - Status TLV (value = Success)
        #             - RLOC16 TLV
        #             - Router Mask TLV

        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(_pkt.ipv6.src).\
            filter_coap_ack(ADDR_SOL_URI).\
            filter(lambda p: {
                              NL_STATUS_TLV,
                              NL_RLOC16_TLV,
                              NL_ROUTER_MASK_TLV
                              } == set(p.coap.tlv.type) and\
                   p.coap.code == COAP_CODE_ACK and\
                   p.thread_address.tlv.status == ADDR_SOL_SUCCESS\
                   ).\
            must_next()

        # Step 4: Leader The DUT sends MLE Advertisements.
        #         The MLE Advertisements from the Leader MUST contain
        #         the Route64 TLV with 32 assigned Router IDs.

        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p: {
                              LEADER_DATA_TLV,
                              ROUTE64_TLV,
                              SOURCE_ADDRESS_TLV
                              } == set(p.mle.tlv.type) and\
                   len(p.mle.tlv.route64.cost) == 32 and\
                   p.ipv6.hlim == 255
                  ).\
            must_next()

        # Step 5: Router_32 to attach to any of the active routers, 2-hops
        #         from the leader, and to send an Address Solicit Request
        #         to become an active router.

        _pkt = pkts.filter_wpan_src64(ROUTER_32).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_request(ADDR_SOL_URI).\
            filter(lambda p: {
                              NL_MAC_EXTENDED_ADDRESS_TLV,
                              NL_STATUS_TLV
                              } == set(p.coap.tlv.type)
                  ).\
           must_next()

        # Step 6: Leader sends an Address Solicit Response.
        #         Ensure the Address Solicit Response is properly formatted:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             - Status TLV (value = No Address Available)

        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(_pkt.ipv6.src).\
            filter_coap_ack(ADDR_SOL_URI).\
            filter(lambda p:
                   p.coap.code == COAP_CODE_ACK and\
                   p.thread_address.tlv.status == ADDR_SOL_NA
                   ).\
            must_next()


if __name__ == '__main__':
    unittest.main()
