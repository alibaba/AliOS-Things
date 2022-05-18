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

import config
import mle
import thread_cert
from pktverify.consts import MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_UPDATE_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, SOURCE_ADDRESS_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MLE_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
ED = 2

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate that the DUT is able to successfully
# attach to a network
#
# Test Topology:
# -------------
#  Leader
#    |
#     DUT
#
# DUT Types:
# ----------
#  ED
#  SED


class Cert_6_1_1_RouterAttach_Base(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ED]
        },
        ED: {
            'name': 'DUT',
            'is_mtd': True,
            'allowlist': [LEADER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED].get_state(), 'child')

        self.collect_ipaddrs()

        ed_addr = self.nodes[ED].get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)
        self.assertTrue(self.nodes[LEADER].ping(ed_addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_LLA = pv.vars['LEADER_LLA']
        DUT = pv.vars['DUT']
        DUT_LLA = pv.vars['DUT_LLA']

        # Step 1: Ensure the Leader is sending MLE Advertisements.

        pkts.filter_wpan_src64(LEADER).\
            filter_mle_advertisement('Leader').\
            must_next()

        # Step 2: DUT sends a MLE Parent Request with an IP hop limit of
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
        #         In securing the first three messages of the attaching process,
        #         the full four-byte key sequence number MUST be included in
        #         the Auxiliary Security Header used for MLE security.
        #
        #         To send the full four-byte key sequence number, the Key
        #         Identifier Mode of the Security Control Field SHALL be set to
        #         ‘0x02’, indicating the presence of a four-byte Key Source,
        #         which SHALL contain the four-byte key sequence number in
        #         network byte order.

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

        # Step 3: Leader responds with a MLE Parent Response.
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

        pkts.filter_wpan_src64(LEADER).\
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

        # Step 4: Router sends a MLE Child ID Request.
        #         The following TLVs MUST be present in the MLE Child ID Request:
        #             - Link-layer Frame Counter TLV
        #             - Address Registration TLV
        #             - Mode TLV
        #             - Response TLV
        #             - Timeout TLV
        #             - TLV Request TLV
        #             - Version TLV
        #             - MLE Frame Counter TLV (optional)

        pkts.filter_wpan_src64(DUT).\
            filter_wpan_dst64(LEADER).\
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

        # Step 5: Leader responds with a Child ID Response.

        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(DUT).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            filter(lambda p: {
                              ADDRESS16_TLV,
                              LEADER_DATA_TLV,
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV
                              } <= set(p.mle.tlv.type)
                   ).\
                   must_next()

        # Step 6: Verify connectivity by sending an ICMPv6 Echo Request
        #         to the DUT link local address
        #         DUT responds with ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(LEADER_LLA, DUT_LLA).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(DUT_LLA, LEADER_LLA).\
            must_next()


class Cert_6_1_1_RouterAttach_ED(Cert_6_1_1_RouterAttach_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_1_1_RouterAttach_Base.TOPOLOGY)
    TOPOLOGY[ED]['mode'] = 'rn'


class Cert_6_1_1_RouterAttach_SED(Cert_6_1_1_RouterAttach_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_1_1_RouterAttach_Base.TOPOLOGY)
    TOPOLOGY[ED]['mode'] = '-'
    TOPOLOGY[ED]['timeout'] = config.DEFAULT_CHILD_TIMEOUT


del (Cert_6_1_1_RouterAttach_Base)

if __name__ == '__main__':
    unittest.main()
