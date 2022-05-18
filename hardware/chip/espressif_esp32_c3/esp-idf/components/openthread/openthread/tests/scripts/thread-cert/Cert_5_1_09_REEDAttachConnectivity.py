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

import config
import mle
import thread_cert
from pktverify.consts import MLE_ADVERTISEMENT, MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_ID_REQUEST, SOURCE_ADDRESS_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
ROUTER1 = 2
REED1 = 3
REED2 = 4
ROUTER2 = 5

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify that the DUT will
# pick REED_1 as its parent because of its better connectivity
#
# Test Topology:
# -------------
#     Leader--Router1
#       /  \     /
# REED2  REED1
#       \  /
#      Router2[DUT]
#
# DUT Types:
# ----------
#  Router


class Cert_5_1_09_REEDAttachConnectivity(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1, REED1, REED2]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER, REED1]
        },
        REED1: {
            'name': 'REED_1',
            'mode': 'rdn',
            'router_upgrade_threshold': 0,
            'allowlist': [LEADER, ROUTER1, ROUTER2]
        },
        REED2: {
            'name': 'REED_2',
            'mode': 'rdn',
            'router_upgrade_threshold': 0,
            'allowlist': [LEADER, ROUTER2]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [REED1, REED2]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        for i in range(2, 5):
            self.nodes[i].start()

        self.simulator.go(5)

        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')
        self.assertEqual(self.nodes[REED1].get_state(), 'child')
        self.assertEqual(self.nodes[REED2].get_state(), 'child')

        self.simulator.go(config.MAX_ADVERTISEMENT_INTERVAL)

        self.nodes[ROUTER2].start()
        self.simulator.go(10)
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'router')
        self.assertEqual(self.nodes[REED1].get_state(), 'router')

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        REED_1 = pv.vars['REED_1']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_2 = pv.vars['ROUTER_2']

        # Step 1: Verify ROUTER_1 and Leader are sending MLE advertisements.
        pkts.filter_wpan_src64(LEADER).\
                   filter_mle_cmd(MLE_ADVERTISEMENT).\
                   must_next()
        pv.verify_attached('ROUTER_1')

        # Step 3: The DUT sends a MLE Parent Request with an IP hop limit of
        #         255 to the Link-Local All Routers multicast address (FF02::2).
        #         The following TLVs MUST be present in the MLE Parent Request:
        #            - Challenge TLV
        #            - Mode TLV
        #            - Scan Mask TLV
        #                If the DUT sends multiple MLE Parent Requests
        #                    - The first one MUST be sent only to all Routers
        #                    - Subsequent ones MAY be sent to all Routers and REEDS
        #            -  Version TLV
        #         If the first MLE Parent Request was sent to all Routers and
        #         REEDS, the test fails.

        pkts.filter_wpan_src64(ROUTER_2).\
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
        lstart = pkts.index

        # Step 5: The DUT MUST send a MLE Parent Request with an IP hop limit of
        #         255 to the Link-Local All Routers multicast address (FF02::2).
        #         The following TLVs MUST be present in the MLE Parent Request:
        #            - Challenge TLV
        #            - Mode TLV
        #            - Scan Mask TLV
        #               - The Scan Mask TLV MUST be sent to Routers And REEDs
        #            -  Version TLV
        #
        #         In securing the first three messages of the attaching process,
        #         the full four-byte key sequence number MUST be included in
        #         the Auxiliary Security Header used for MLE security.
        #
        #         To send the full four-byte key sequence number, the Key
        #         Identifier Mode of the Security Control Field SHALL be set to
        #         ‘0x02’, indicating the presence of a four-byte Key Source,
        #         which SHALL contain the four-byte key sequence number in
        #         network byte order.

        pkts.filter_wpan_src64(ROUTER_2).\
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
                   p.mle.tlv.scan_mask.e == 1 and\
                   p.wpan.aux_sec.key_id_mode == 0x2
                   ).\
            must_next()
        lend = pkts.index

        # Step 4: REED_1 and REED_2 no response to Parent Request meant for all routers.

        for i in (1, 2):
            pkts.range(lstart, lend).\
                filter_wpan_src64(pv.vars['REED_%d' % i]).\
                filter_wpan_dst64(ROUTER_2).\
                filter_mle_cmd(MLE_PARENT_RESPONSE).\
                must_not_next()

        # Step 6: REED_1 and REED_2 respond with a MLE Parent Response.
        #         The following TLVs MUST be present in the MLE Parent Response:
        #             - Challenge TLV
        #             - Connectivity TLV
        #             - Leader Data TLV
        #             - Link-layer Frame Counter TLV
        #             - Link Margin TLV
        #             - Response TLV
        #             - Source Address
        #             - Version TLV
        #             - MLE Frame Counter TLV (optional)

        for i in (1, 2):
            with pkts.save_index():
                pkts.filter_wpan_src64(pv.vars['REED_%d' % i]).\
                    filter_wpan_dst64(ROUTER_2).\
                    filter_mle_cmd(MLE_PARENT_RESPONSE).\
                    filter(lambda p: {
                                      CHALLENGE_TLV,
                                      CONNECTIVITY_TLV,
                                      LEADER_DATA_TLV,
                                      LINK_LAYER_FRAME_COUNTER_TLV,
                                      LINK_MARGIN_TLV,
                                      RESPONSE_TLV,
                                      SOURCE_ADDRESS_TLV,
                                      VERSION_TLV
                                    } <= set(p.mle.tlv.type) and\
                           p.wpan.aux_sec.key_id_mode == 0x2
                           ).\
                    must_next()

        # Step 7: DUT sends a MLE Child ID Request to REED_1.
        #         The following TLVs MUST be present in the MLE Child ID Request:
        #             - Link-layer Frame Counter TLV
        #             - Mode TLV
        #             - Response TLV
        #             - Timeout TLV
        #             - TLV Request TLV
        #                 - Address16 TLV
        #                 - Network Data TLV
        #                 - Route64 TLV (optional)
        #             - Version TLV
        #             - MLE Frame Counter TLV (optional)
        #         The following TLV MUST NOT be present in the MLE Child ID Request:
        #             - Address Registration TLV

        _pkt = pkts.filter_wpan_src64(ROUTER_2).\
            filter_wpan_dst64(REED_1).\
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
                   p.thread_nwd.tlv.type is nullField and\
                   p.wpan.aux_sec.key_id_mode == 0x2
                   ).\
            must_next()
        _pkt.must_not_verify(lambda p: (ADDRESS_REGISTRATION_TLV) in p.mle.tlv.type)


if __name__ == '__main__':
    unittest.main()
