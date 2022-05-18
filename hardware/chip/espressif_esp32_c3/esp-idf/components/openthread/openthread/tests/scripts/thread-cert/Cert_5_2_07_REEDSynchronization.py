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

import copy
import unittest

import command
import config
import ipv6
import mle
import thread_cert
from pktverify.consts import ADDR_SOL_URI, MLE_LINK_REQUEST, MLE_LINK_ACCEPT, MLE_LINK_ACCEPT_AND_REQUEST, SOURCE_ADDRESS_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, LEADER_DATA_TLV, VERSION_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
DUT_ROUTER1 = 2
ROUTER2 = 3
DUT_REED = 17

MLE_MIN_LINKS = 3

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate the REED’s Synchronization
# procedure after attaching to a network with multiple Routers. A REED
# MUST process incoming Advertisements and perform a one-way frame-counter
# synchronization with at least 3 neighboring Routers. When Router receives
# unicast MLE Link Request from REED, it replies with MLE Link Accept.
#
# Test Topology:
# -------------
#  Router_15 - Leader
#      ...    /     \
#         Router_n  Router_1(DUT)
#          |
#      REED(DUT)
#
# DUT Types:
# ----------
#  Router
#  REED


class Cert_5_2_7_REEDSynchronization_Base(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
        },
        DUT_ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
        },
        4: {
            'name': 'ROUTER_3',
            'mode': 'rdn',
        },
        5: {
            'name': 'ROUTER_4',
            'mode': 'rdn',
        },
        6: {
            'name': 'ROUTER_5',
            'mode': 'rdn',
        },
        7: {
            'name': 'ROUTER_6',
            'mode': 'rdn',
        },
        8: {
            'name': 'ROUTER_7',
            'mode': 'rdn',
        },
        9: {
            'name': 'ROUTER_8',
            'mode': 'rdn',
        },
        10: {
            'name': 'ROUTER_9',
            'mode': 'rdn',
        },
        11: {
            'name': 'ROUTER_10',
            'mode': 'rdn',
        },
        12: {
            'name': 'ROUTER_11',
            'mode': 'rdn',
        },
        13: {
            'name': 'ROUTER_12',
            'mode': 'rdn',
        },
        14: {
            'name': 'ROUTER_13',
            'mode': 'rdn',
        },
        15: {
            'name': 'ROUTER_14',
            'mode': 'rdn',
        },
        16: {
            'name': 'ROUTER_15',
            'mode': 'rdn',
        },
        DUT_REED: {
            'name': 'REED',
            'mode': 'rdn',
        },
    }

    DUT = 0

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        for i in range(2, 17):
            self.nodes[i].start()
        self.simulator.go(10)

        for i in range(2, 17):
            self.assertEqual(self.nodes[i].get_state(), 'router')

        # Avoid DUT_REED attach to DUT_ROUTER1.
        if self.DUT == DUT_REED:
            self.nodes[DUT_REED].\
                add_allowlist(self.nodes[DUT_ROUTER1].get_addr64(),
                              config.RSSI['LINK_QULITY_1'])

        if self.DUT == DUT_ROUTER1:
            self.nodes[DUT_REED].\
                add_allowlist(self.nodes[DUT_ROUTER1].get_addr64(),
                              config.RSSI['LINK_QULITY_1'])
            self.nodes[DUT_REED].add_allowlist(self.nodes[ROUTER2].get_addr64())
            self.nodes[DUT_REED].enable_allowlist()

        self.nodes[DUT_REED].start()
        self.simulator.go(config.MAX_ADVERTISEMENT_INTERVAL)
        self.assertEqual(self.nodes[DUT_REED].get_state(), 'child')

        self.simulator.go(config.MAX_ADVERTISEMENT_INTERVAL)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER_1 = pv.vars['ROUTER_1']
        REED = pv.vars['REED']

        # Step 1: Verify topology is formed correctly except REED.

        for i in range(1, 16):
            with pkts.save_index():
                pv.verify_attached('ROUTER_%d' % i)

        # Step 2: REED attaches to the network and MUST NOT attempt to become
        #         an active router by sending an Address Solicit Request

        pv.verify_attached('REED')
        pkts.filter_wpan_src64(REED).\
            filter_coap_request(ADDR_SOL_URI).\
            filter(lambda p: {
                              NL_MAC_EXTENDED_ADDRESS_TLV,
                              NL_STATUS_TLV
                              } == set(p.coap.tlv.type)
                  ).\
            must_not_next()

        # Step 3: REED sends a unicast Link Request message to at lease 3 Routers
        #
        #         The Link Request Message MUST contain the following TLVs:
        #             - Challenge TLV
        #             - Leader Data TLV
        #             - Source Address TLV
        #             - Version TLV

        if self.DUT == DUT_REED:
            for i in range(0, MLE_MIN_LINKS):
                pkts.filter_wpan_src64(REED).\
                    filter_mle_cmd(MLE_LINK_REQUEST).\
                    filter(lambda p: {
                                      CHALLENGE_TLV,
                                      LEADER_DATA_TLV,
                                      SOURCE_ADDRESS_TLV,
                                      VERSION_TLV
                                      } <= set(p.mle.tlv.type)
                           ).\
                    must_next()

        # Step 4: Router_1 sends Link Accept message
        #         The following TLVs MUST be present in the Link Accept message:
        #             - Link-layer Frame Counter TLV
        #             - Source Address TLV
        #             - Response TLV
        #             - Version TLV
        #             - MLE Frame Counter TLV (optional)
        #
        #         The recipient does not make any change to its local state and
        #         MUST NOT reply with a Link Accept And Request message.

        if self.DUT == DUT_ROUTER1:
            pkts.filter_wpan_dst64(REED).\
                filter_wpan_src64(ROUTER_1).\
                filter_mle_cmd(MLE_LINK_ACCEPT).\
                filter(lambda p: {
                                  LINK_LAYER_FRAME_COUNTER_TLV,
                                  RESPONSE_TLV,
                                  SOURCE_ADDRESS_TLV,
                                  VERSION_TLV
                                   } <= set(p.mle.tlv.type)
                       ).\
                must_next()
            pkts.filter_wpan_src64(REED).\
                filter_mle_cmd(MLE_LINK_ACCEPT_AND_REQUEST).\
                must_not_next()


class Cert_5_2_7_REEDSynchronization_REED(Cert_5_2_7_REEDSynchronization_Base):
    DUT = DUT_REED


class Cert_5_2_7_REEDSynchronization_ROUTER(Cert_5_2_7_REEDSynchronization_Base):
    DUT = DUT_ROUTER1


if __name__ == '__main__':
    unittest.main()
