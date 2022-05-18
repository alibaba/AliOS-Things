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

import command
import config
import mle
import thread_cert
from pktverify.consts import MLE_ADVERTISEMENT, MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_ID_RESPONSE, ADDR_SOL_URI, SOURCE_ADDRESS_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, NL_MAC_EXTENDED_ADDRESS_TLV, NL_RLOC16_TLV, NL_STATUS_TLV, NL_ROUTER_MASK_TLV, COAP_CODE_ACK
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
DUT_ROUTER1 = 2
REED1 = 3
MED1 = 4

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to show that the DUT is able to attach
# a REED and forward address solicits two hops away from the Leader.
#
# Test Topology:
# -------------
#  Leader
#    |
# Router_1
#    |
#  REED_1
#    |
#  MED_1
#
# DUT Types:
# ----------
#  Router


class Cert_5_2_01_REEDAttach(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [DUT_ROUTER1]
        },
        DUT_ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER, REED1]
        },
        REED1: {
            'name': 'REED_1',
            'mode': 'rdn',
            'router_upgrade_threshold': 1,
            'allowlist': [DUT_ROUTER1, MED1]
        },
        MED1: {
            'name': 'MED_1',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [REED1]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[DUT_ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT_ROUTER1].get_state(), 'router')

        self.nodes[REED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[REED1].get_state(), 'child')

        self.nodes[MED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MED1].get_state(), 'child')

        self.collect_rloc16s()
        self.collect_ipaddrs()

        self.simulator.go(config.MAX_ADVERTISEMENT_INTERVAL)

        reed_mleid = self.nodes[REED1].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        self.assertTrue(self.nodes[LEADER].ping(reed_mleid))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_MLEID = pv.vars['LEADER_MLEID']
        LEADER_RLOC16 = pv.vars['LEADER_RLOC16']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_1_RLOC16 = pv.vars['ROUTER_1_RLOC16']
        REED_1 = pv.vars['REED_1']
        REED_1_MLEID = pv.vars['REED_1_MLEID']

        # Step 1: Router_1 attaches to Leader and sends properly formatted MLE
        #         advertisements
        #         Advertisements MUST be sent with an IP hop limit of 255 to
        #         the Link-Local All Nodes multicast address (FF02::1).
        #         The following TLVs MUST be present in the MLE Advertisements:
        #             - Leader Data TLV
        #             - Route64 TLV
        #             - Source Address TLV

        pv.verify_attached('ROUTER_1')
        pkts.filter_wpan_src64(ROUTER_1).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p: {
                              LEADER_DATA_TLV,
                              ROUTE64_TLV,
                              SOURCE_ADDRESS_TLV
                              } == set(p.mle.tlv.type) and\
                   p.ipv6.hlim == 255
                   ).\
            must_next()

        # Step 2: Attach REED_1 to Router_1; REED_1 sends MLE Parent Request with
        #         an IP hop limit of 255 to the Link-Local All Routers multicast
        #         address (FF02::2).
        #         The following TLVs MUST be present in the MLE Parent Request:
        #            - Challenge TLV
        #            - Mode TLV
        #            - Scan Mask TLV
        #                If the DUT sends multiple MLE Parent Requests
        #                    - The first one MUST be sent only to all Routers
        #                    - Subsequent ones MAY be sent to all Routers and REEDS
        #            -  Version TLV

        pkts.filter_wpan_src64(REED_1).\
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
                   p.mle.tlv.scan_mask.e == 0).\
            must_next()

        # Step 3: Router_1 must respond with a MLE Parent Response.
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

        pkts.filter_wpan_src64(ROUTER_1).\
            filter_wpan_dst64(REED_1).\
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

        # Step 4: Router_1 must respond with a Child ID Response.
        #         The following TLVs MUST be present in the Child ID Response:
        #             - Address16 TLV
        #             - Leader Data TLV
        #             - Network Data TLV
        #             - Source Address TLV
        #             - Route64 TLV (if requested)

        pkts.filter_wpan_src64(ROUTER_1).\
            filter_wpan_dst64(REED_1).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            filter(lambda p: {
                              ADDRESS16_TLV,
                              LEADER_DATA_TLV,
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              ROUTE64_TLV
                              } <= set(p.mle.tlv.type) or\
                             {
                              ADDRESS16_TLV,
                              LEADER_DATA_TLV,
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV
                              } <= set(p.mle.tlv.type)
                   ).\
                   must_next()

        # Step 7: REED_1 sends an Address Solicit Request to Router_1.
        #         Ensure the Address Solicit Request is properly formatted:
        #         CoAP Request URI
        #             coap://<leader address>:MM/a/as
        #         CoAP Payload
        #             - MAC Extended Address TLV
        #             - Status TLV

        _pkt1 = pkts.filter_wpan_src64(REED_1).\
            filter_wpan_dst16(ROUTER_1_RLOC16).\
            filter_coap_request(ADDR_SOL_URI).\
            filter(lambda p: {
                              NL_MAC_EXTENDED_ADDRESS_TLV,
                              NL_STATUS_TLV
                              } == set(p.coap.tlv.type)\
                   ).\
           must_next()

        # Step 8: Router_1 forward the REED_1's Address Solicit Request to
        #         Leader and Leader's Address Solicit Response to REED_1.
        #         Ensure the Address Solicit Response is properly formatted:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             - Status TLV (value = Success)
        #             - RLOC16 TLV
        #             - Router Mask TLV

        _pkt2 = pkts.filter_wpan_src64(ROUTER_1).\
            filter_wpan_dst16(LEADER_RLOC16).\
            filter_coap_request(ADDR_SOL_URI).\
            filter(lambda p: {
                              NL_MAC_EXTENDED_ADDRESS_TLV,
                              NL_STATUS_TLV
                              } == set(p.coap.tlv.type)\
                   ).\
           must_next()

        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst16(_pkt2.wpan.src16).\
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
        pkts.filter_wpan_src64(ROUTER_1).\
            filter_wpan_dst16(_pkt1.wpan.src16).\
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

        # Step 9: REED_1 responds with ICMPv6 Echo Reply
        _pkt = pkts.filter_ipv6_src_dst(LEADER_MLEID, REED_1_MLEID).\
                    filter_ping_request().\
                    must_next()
        pkts.filter_ipv6_src_dst(REED_1_MLEID, LEADER_MLEID).\
            filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            must_next()


if __name__ == '__main__':
    unittest.main()
