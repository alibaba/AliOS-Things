#!/usr/bin/env python3
#
#  Copyright (c) 2018, The OpenThread Authors.
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
import copy

import thread_cert
import config
from pktverify.consts import WPAN_DATA_REQUEST, WPAN_ACK, MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_UPDATE_REQUEST, MLE_CHILD_UPDATE_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, ADDR_SOL_URI, SOURCE_ADDRESS_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MLE_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV, NL_MAC_EXTENDED_ADDRESS_TLV, NL_RLOC16_TLV, NL_STATUS_TLV, NL_ROUTER_MASK_TLV, COAP_CODE_ACK
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
REED = 2
MTD = 3

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate that the DUT is able to successfully
# attach to a network as an End Device through a REED.
#
# Test Topology:
# -------------
#  Leader
#    |
#  REED
#    |
#   DUT
#
# DUT Types:
# ----------
#  ED
#  SED


class Cert_6_1_2_REEDAttach_Base(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [REED]
        },
        REED: {
            'name': 'REED',
            'mode': 'rdn',
            'router_upgrade_threshold': 0,
            'allowlist': [LEADER, MTD]
        },
        MTD: {
            'name': 'DUT',
            'is_mtd': True,
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [REED]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[REED].start()
        self.simulator.go(config.MAX_ADVERTISEMENT_INTERVAL)
        self.assertEqual(self.nodes[REED].get_state(), 'child')
        self.collect_rloc16s()

        self.nodes[MTD].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MTD].get_state(), 'child')
        self.assertEqual(self.nodes[REED].get_state(), 'router')
        self.collect_ipaddrs()
        self.collect_rlocs()

        self.simulator.go(config.DEFAULT_CHILD_TIMEOUT)

        dut_addr = self.nodes[MTD].get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)
        self.assertTrue(self.nodes[REED].ping(dut_addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        REED = pv.vars['REED']
        REED_RLOC = pv.vars['REED_RLOC']
        REED_LLA = pv.vars['REED_LLA']
        REED_RLOC16 = pv.vars['REED_RLOC16']
        DUT = pv.vars['DUT']
        DUT_RLOC = pv.vars['DUT_RLOC']
        DUT_LLA = pv.vars['DUT_LLA']

        # Step 1: Ensure the Leader is sending MLE Advertisements and is connected to REED

        pkts.filter_wpan_src64(LEADER).\
            filter_mle_advertisement('Leader').\
            must_next()

        pv.verify_attached('REED', 'LEADER')

        # Step 2: DUT sends a MLE Parent Request with an IP hop limit of
        #         255 to the Link-Local All Routers multicast address (FF02::2).
        #         The following TLVs MUST be present in the MLE Parent Request:
        #            - Challenge TLV
        #            - Mode TLV
        #            - Scan Mask TLV
        #                Verify that the first one is sent to routers only
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
        index1 = pkts.index

        # Step 4: DUT sends a MLE Parent Request with an IP hop limit of
        #         255 to the Link-Local All Routers multicast address (FF02::2).
        #         The following TLVs MUST be present in the MLE Parent Request:
        #            - Challenge TLV
        #            - Mode TLV
        #            - Scan Mask TLV
        #                    Verify that it is sent to Routers AND REEDs
        #            -  Version TLV
        #         If request was not sent to all routers and REEDS, then the test
        #         has failed.
        #         In securing the first three messages of the attaching process,
        #         the full four-byte key sequence number MUST be included in
        #         the Auxiliary Security Header used for MLE security.
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
                   p.mle.tlv.scan_mask.e == 1 and\
                   p.wpan.aux_sec.key_id_mode == 0x2
                   ).\
            must_next()
        index2 = pkts.index

        # Step 3: REED doesn't response to the first Parent Request

        pkts.range(index1, index2).\
            filter_wpan_src64(REED).\
            filter_wpan_dst64(DUT).\
            filter_mle_cmd(MLE_PARENT_RESPONSE).\
            must_not_next()

        # Step 5: REED responds with MLE Parent Response for the second Parent Request

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

        # Step 6: DUT sends a MLE Child ID Request.
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

        # Step 7: REED sends an Address Solicit Request to Leader;
        #         Leader responds with an Address Solicit Response and REED
        #         becomes active router;
        #         REED sends Child ID Response with DUT’s new 16-bit Address.

        _pkt = pkts.filter_wpan_src64(REED).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_request(ADDR_SOL_URI).\
            filter(lambda p: {
                              NL_MAC_EXTENDED_ADDRESS_TLV,
                              NL_STATUS_TLV
                              } <= set(p.coap.tlv.type)
                   ).\
            must_next()
        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst16(_pkt.wpan.src16).\
            filter_coap_ack(ADDR_SOL_URI).\
            filter(lambda p: {
                              NL_STATUS_TLV,
                              NL_RLOC16_TLV,
                              NL_ROUTER_MASK_TLV
                              } == set(p.coap.tlv.type) and\
                   p.coap.code == COAP_CODE_ACK and\
                   p.thread_address.tlv.status == 0\
                   ).\
            must_next()
        _pkt = pkts.filter_wpan_src64(REED).\
            filter_wpan_dst64(DUT).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            filter(lambda p: {
                              ADDRESS16_TLV,
                              LEADER_DATA_TLV,
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.mle.tlv.source_addr != REED_RLOC16
                   ).\
            must_next()

        if self.TOPOLOGY[MTD]['mode'] == 'rn':
            # Step 8: DUT sends periodic Child Update messages as part of the
            #         keep-alive message
            #         The DUT MUST send MLE Child Update messages containing
            #         the following TLVs:
            #             - Leader Data TLV
            #             - Mode TLV
            #             - Source Address TLV
            pkts.filter_wpan_src64(DUT).\
                filter_wpan_dst64(REED).\
                filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).\
                filter(lambda p: {
                                  SOURCE_ADDRESS_TLV,
                                  MODE_TLV,
                                  LEADER_DATA_TLV
                                 } < set(p.mle.tlv.type)
                       ).\
                must_next()

            # Step 9: REED Respond to Child Update messages with a MLE Update
            #         Response.
            pkts.filter_wpan_src64(REED).\
                filter_wpan_dst64(DUT).\
                filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).\
                filter(lambda p: {
                                  SOURCE_ADDRESS_TLV,
                                  MODE_TLV,
                                  LEADER_DATA_TLV
                                 } < set(p.mle.tlv.type)
                       ).\
                must_next()

            # Step 10: Go to Step 12

        else:
            # Step 11: DUT sends periodic 802.15.4 Data Request messages as part
            #          of the keep-alive message
            #          The DUT must send a 802.15.4 Data Request command to the
            #          parent device and receive an ACK message in response
            _pkt2 = pkts.filter_wpan_src64(DUT).\
                filter_wpan_dst16(_pkt.mle.tlv.source_addr).\
                filter_wpan_cmd(WPAN_DATA_REQUEST).\
                must_next()

            pkts.filter(lambda p:
                        p.wpan.seq_no == _pkt2.wpan.seq_no and\
                        p.wpan.frame_type == WPAN_ACK
                        ).\
                must_next()

        # Step 12: REED verifies connectivity by sending an ICMPv6 Echo Request
        #          to the DUT link local address
        #          DUT responds with ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(REED_LLA, DUT_LLA).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(DUT_LLA, REED_LLA).\
                must_next()


class Cert_6_1_2_REEDAttach_ED(Cert_6_1_2_REEDAttach_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_1_2_REEDAttach_Base.TOPOLOGY)
    TOPOLOGY[MTD]['mode'] = 'rn'


class Cert_6_1_2_REEDAttach_SED(Cert_6_1_2_REEDAttach_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_1_2_REEDAttach_Base.TOPOLOGY)
    TOPOLOGY[MTD]['mode'] = '-'


del (Cert_6_1_2_REEDAttach_Base)

if __name__ == '__main__':
    unittest.main()
