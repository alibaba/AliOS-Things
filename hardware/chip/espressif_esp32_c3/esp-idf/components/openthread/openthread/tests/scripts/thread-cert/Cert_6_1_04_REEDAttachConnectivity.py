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
from pktverify.consts import MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, ADDR_SOL_URI, SOURCE_ADDRESS_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MLE_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV, NL_MAC_EXTENDED_ADDRESS_TLV, NL_RLOC16_TLV, NL_STATUS_TLV, NL_ROUTER_MASK_TLV, COAP_CODE_ACK
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ROUTER = 2
REED_1 = 3
REED_2 = 4
MTD = 5

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate that the DUT will pick REED_1
# as its parent because of its better connectivity.
#
# Test Topology:
# -------------
#  Router - Leader
#    |    /  |
#  REED_1  REED_2
#      \   /
#       DUT
#
# DUT Types:
# ----------
#  ED
#  SED


class Cert_6_1_4_REEDAttachConnectivity_Base(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER, REED_1, REED_2]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER, REED_1]
        },
        REED_1: {
            'name': 'REED_1',
            'mode': 'rdn',
            'router_upgrade_threshold': 0,
            'allowlist': [LEADER, ROUTER, MTD]
        },
        REED_2: {
            'name': 'REED_2',
            'mode': 'rdn',
            'router_upgrade_threshold': 0,
            'allowlist': [LEADER, MTD]
        },
        MTD: {
            'name': 'DUT',
            'is_mtd': True,
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [REED_1, REED_2]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        for i in (3, 4):
            self.nodes[i].start()
            self.simulator.go(5)
            self.assertEqual(self.nodes[i].get_state(), 'child')

        self.collect_rloc16s()
        self.collect_rlocs()

        self.nodes[MTD].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MTD].get_state(), 'child')
        self.assertEqual(self.nodes[REED_1].get_state(), 'router')

        self.simulator.go(config.DEFAULT_CHILD_TIMEOUT)
        self.collect_ipaddrs()

        dut_addr = self.nodes[MTD].get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)
        self.assertTrue(self.nodes[REED_1].ping(dut_addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        ROUTER = pv.vars['ROUTER']
        REED_1 = pv.vars['REED_1']
        REED_1_RLOC = pv.vars['REED_1_RLOC']
        REED_1_LLA = pv.vars['REED_1_LLA']
        REED_1_RLOC16 = pv.vars['REED_1_RLOC16']
        DUT = pv.vars['DUT']
        DUT_RLOC = pv.vars['DUT_RLOC']
        DUT_LLA = pv.vars['DUT_LLA']

        # Step 1: Setup the topology without the DUT. Ensure all routers and
        #         leader are sending MLE advertisements

        pkts.filter_wpan_src64(LEADER).\
            filter_mle_advertisement('Leader').\
            must_next()

        pv.verify_attached('ROUTER', 'LEADER')

        # Step 2: DUT sends a MLE Parent Request with an IP hop limit of
        #         255 to the Link-Local All Routers multicast address (FF02::2).
        #         The following TLVs MUST be present in the MLE Parent Request:
        #            - Challenge TLV
        #            - Mode TLV
        #            - Scan Mask TLV
        #                Verify that the first one is sent to routers only
        #            -  Version TLV
        #         If the first MLE Parent Request was sent to all Routers and
        #         REED_1S, the test fails.

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
        #                    Verify that it is sent to Routers AND REED_1s
        #            -  Version TLV
        #         If request was not sent to all routers and REED_1S, then the test
        #         has failed.
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

        # Step 3: REED_1 and REED_2 sends no response to Parent Request after
        #         DUT sent the first Parent Request
        for i in (1, 2):
            pkts.range(index1, index2).\
                filter_wpan_src64(pv.vars['REED_%d' %i]).\
                filter_wpan_dst64(DUT).\
                filter_mle_cmd(MLE_PARENT_RESPONSE).\
                must_not_next()

        # Step 5: REED_1 Respond with MLE Parent Response
        for i in (1, 2):
            with pkts.save_index():
                pkts.filter_wpan_src64(pv.vars['REED_%d' %i]).\
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

        # Step 6: DUT sends a MLE Child ID Request to REED_1
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
            filter_wpan_dst64(REED_1).\
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

        # Step 7: REED_1 sends an Address Solicit Request to Leader;
        #         Leader responds with an Address Solicit Response and REED_1
        #         becomes active router;
        #         REED_1 sends Child ID Response with DUT’s new 16-bit Address.

        pkts.filter_wpan_src64(REED_1).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_request(ADDR_SOL_URI).\
            filter(lambda p: {
                              NL_MAC_EXTENDED_ADDRESS_TLV,
                              NL_STATUS_TLV
                              } <= set(p.coap.tlv.type)
                   ).\
            must_next()
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(REED_1_RLOC).\
            filter_coap_ack(ADDR_SOL_URI).\
            filter(lambda p: {
                              NL_STATUS_TLV,
                              NL_RLOC16_TLV,
                              NL_ROUTER_MASK_TLV
                              } == set(p.coap.tlv.type) and\
                   p.thread_address.tlv.status == 0
                   ).\
            must_next()

        pkts.filter_wpan_src64(REED_1).\
            filter_wpan_dst64(DUT).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            filter(lambda p: {
                              ADDRESS16_TLV,
                              LEADER_DATA_TLV,
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.mle.tlv.source_addr != REED_1_RLOC16
                   ).\
            must_next()

        # Step 8: REED_1 verifies connectivity by sending an ICMPv6 Echo Request
        #          to the DUT link local address
        #          DUT responds with ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(REED_1_LLA, DUT_LLA).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(DUT_LLA, REED_1_LLA).\
            must_next()


class Cert_6_1_4_REEDAttachConnectivity_ED(Cert_6_1_4_REEDAttachConnectivity_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_1_4_REEDAttachConnectivity_Base.TOPOLOGY)
    TOPOLOGY[MTD]['mode'] = 'rn'


class Cert_6_1_4_REEDAttachConnectivity_SED(Cert_6_1_4_REEDAttachConnectivity_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_1_4_REEDAttachConnectivity_Base.TOPOLOGY)
    TOPOLOGY[MTD]['mode'] = '-'


del (Cert_6_1_4_REEDAttachConnectivity_Base)

if __name__ == '__main__':
    unittest.main()
