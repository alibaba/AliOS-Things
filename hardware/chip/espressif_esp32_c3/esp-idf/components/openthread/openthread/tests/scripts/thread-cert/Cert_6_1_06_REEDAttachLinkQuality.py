#!/usr/bin/env python3
#
#  Copyright (c) 2019, The OpenThread Authors.
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
import copy
import mle
import thread_cert
from pktverify.consts import MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, SOURCE_ADDRESS_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MLE_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
REED = 2
ROUTER = 3
MTD = 4

# Test Purpose and Description:
# -----------------------------
# The purpose of this test is to verify that the DUT sends a second Parent Request
# to the all-routers and all-reeds multicast address if it gets a reply from the
# first Parent Request to the all-routers address with a bad link quality.
#
# Test Topology:
# -------------
#     Leader
#     /    \
#  REED   ROUTER
#      \  /
#       DUT
#
# DUT Types:
# ----------
#  ED
#  SED


class Cert_6_1_6_REEDAttachLinkQuality_Base(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [REED, ROUTER]
        },
        REED: {
            'name': 'REED',
            'mode': 'rdn',
            'router_upgrade_threshold': 0,
            'allowlist': [LEADER, MTD]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER, (MTD, -85)]
        },
        MTD: {
            'name': 'DUT',
            'is_mtd': True,
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [REED, ROUTER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[REED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[REED].get_state(), 'child')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[MTD].start()
        self.simulator.go(10)
        self.assertEqual(self.nodes[MTD].get_state(), 'child')
        self.assertEqual(self.nodes[REED].get_state(), 'router')

        self.collect_ipaddrs()
        self.collect_rlocs()

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        REED = pv.vars['REED']
        ROUTER = pv.vars['ROUTER']
        DUT = pv.vars['DUT']

        # Step 1: Setup the topology without the DUT. Ensure all routers and leader
        #         are sending MLE advertisements
        pkts.filter_wpan_src64(LEADER).\
            filter_mle_advertisement('Leader').\
            must_next()

        pv.verify_attached('REED', 'LEADER')
        pv.verify_attached('ROUTER', 'LEADER')

        # Step 3: DUT sends a MLE Parent Request with an IP hop limit of
        #         255 to the Link-Local All Routers multicast address (FF02::2).
        #         The following TLVs MUST be present in the MLE Parent Request:
        #            - Challenge TLV
        #            - Mode TLV
        #            - Scan Mask TLV
        #                Verify sent to routers only
        #            -  Version TLV
        #         If the first Request was sent to all routers and REEDS, then
        #         the test has failed.
        pkts.filter_wpan_src64(DUT).\
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
                   p.mle.tlv.scan_mask.e == 0 and\
                   p.wpan.aux_sec.key_id_mode == 0x2
                   ).\
            must_next()

        # Step 4: Router responds with MLE Parent Response
        pkts.filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(DUT).\
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
                               } <= set(p.mle.tlv.type)).\
            must_next()

        # Step 5: DUT sends a MLE Parent Request with an IP hop limit of
        #         255 to the Link-Local All Routers multicast address (FF02::2).
        #         The following TLVs MUST be present in the MLE Parent Request:
        #            - Challenge TLV
        #            - Mode TLV
        #            - Scan Mask TLV
        #                    Verify that it is sent to Routers AND REEDs
        #            -  Version TLV
        pkts.filter_wpan_src64(DUT).\
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
        index2 = pkts.index

        # Step 6: REED sends MLE Parent Response to DUT
        pkts.filter_wpan_src64(REED).\
            filter_wpan_dst64(DUT).\
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
                               } <= set(p.mle.tlv.type)).\
            must_next()

        # Step 6: DUT sends a MLE Child ID Request to REED
        #         The following TLVs MUST be present in the MLE Child ID Request:
        #             - Address Registration TLV
        #             - Link-layer Frame Counter TLV
        #             - Mode TLV
        #             - Response TLV
        #             - Timeout TLV
        #             - TLV Request TLV
        #             - Version TLV
        #             - MLE Frame Counter TLV (optional)
        pkts.filter_wpan_src64(DUT).\
            filter_wpan_dst64(REED).\
            filter_mle_cmd(MLE_CHILD_ID_REQUEST).\
            filter(lambda p: {
                              ADDRESS_REGISTRATION_TLV,
                              LINK_LAYER_FRAME_COUNTER_TLV,
                              MODE_TLV,
                              RESPONSE_TLV,
                              TIMEOUT_TLV,
                              TLV_REQUEST_TLV,
                              VERSION_TLV
                             } <= set(p.mle.tlv.type) and\
                   p.wpan.aux_sec.key_id_mode == 0x2
                   ).\
             must_next()


class Cert_6_1_6_REEDAttachLinkQuality_ED(Cert_6_1_6_REEDAttachLinkQuality_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_1_6_REEDAttachLinkQuality_Base.TOPOLOGY)
    TOPOLOGY[MTD]['mode'] = 'rn'


class Cert_6_1_6_REEDAttachLinkQuality_SED(Cert_6_1_6_REEDAttachLinkQuality_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_1_6_REEDAttachLinkQuality_Base.TOPOLOGY)
    TOPOLOGY[MTD]['mode'] = '-'


del (Cert_6_1_6_REEDAttachLinkQuality_Base)

if __name__ == '__main__':
    unittest.main()
