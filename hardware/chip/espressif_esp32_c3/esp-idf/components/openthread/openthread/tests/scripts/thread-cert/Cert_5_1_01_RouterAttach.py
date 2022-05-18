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
import network_layer
import thread_cert
from pktverify.consts import MLE_ADVERTISEMENT, MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_UPDATE_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, MLE_LINK_REQUEST, MLE_LINK_ACCEPT_AND_REQUEST, ADDR_SOL_URI, SOURCE_ADDRESS_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MLE_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV, NL_MAC_EXTENDED_ADDRESS_TLV, NL_RLOC16_TLV, NL_STATUS_TLV, NL_ROUTER_MASK_TLV, COAP_CODE_ACK
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
ROUTER = 2

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to show that the Leader is able to form
# a network and the Router attaches to it with proper steps.
#
# Test Topology:
# -------------
# Leader
#    |
# Router
#
# DUT Types:
# ----------
#  Leader
#  Router


class Cert_5_1_01_RouterAttach(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(7)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.collect_rloc16s()

        leader_addr = self.nodes[LEADER].get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)
        self.assertTrue(self.nodes[ROUTER].ping(leader_addr))
        self.simulator.go(5)

        router_addr = self.nodes[ROUTER].get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)
        self.assertTrue(self.nodes[LEADER].ping(router_addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC16 = pv.vars['LEADER_RLOC16']
        ROUTER = pv.vars['ROUTER']

        # Step 1: Leader is sending properly formatted MLE Advertisements.
        #         Advertisements MUST be sent with an IP hop limit of 255 to
        #         the Link-Local All Nodes multicast address (FF02::1).
        #         The following TLVs MUST be present in the MLE Advertisements:
        #             - Leader Data TLV
        #             - Route64 TLV
        #             - Source Address TLV

        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p: {
                              LEADER_DATA_TLV,
                              ROUTE64_TLV,
                              SOURCE_ADDRESS_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.ipv6.hlim == 255).\
            must_next()

        # Step 2: Router sends a MLE Parent Request with an IP hop limit of
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

        pkts.filter_wpan_src64(ROUTER).\
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
            filter_wpan_dst64(ROUTER).\
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

        _pkt = pkts.filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(LEADER).\
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
                   p.thread_nwd.tlv.type is nullField).\
                   must_next()
        _pkt.must_not_verify(lambda p: (ADDRESS_REGISTRATION_TLV) in p.mle.tlv.type)

        # Step 5: Leader responds with a Child ID Response.
        #         The following TLVs MUST be present in the Child ID Response:
        #             - Address16 TLV
        #             - Leader Data TLV
        #             - Network Data TLV
        #             - Source Address TLV
        #             - Route64 TLV (if requested)

        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
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
                               } <= set(p.mle.tlv.type)).\
                   must_next()

        # Step 6: Router sends an Address Solicit Request.
        #         Ensure the Address Solicit Request is properly formatted:
        #         CoAP Request URI
        #             coap://<leader address>:MM/a/as
        #         CoAP Payload
        #             - MAC Extended Address TLV
        #             - Status TLV

        _pkt = pkts.filter_wpan_src64(ROUTER).\
            filter_wpan_dst16(LEADER_RLOC16).\
            filter_coap_request(ADDR_SOL_URI).\
            filter(lambda p: {
                              NL_MAC_EXTENDED_ADDRESS_TLV,
                              NL_STATUS_TLV
                              } == set(p.coap.tlv.type)\
                   ).\
           must_next()

        # Step 7: Leader sends an Address Solicit Response.
        #         Ensure the Address Solicit Response is properly formatted:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             - Status TLV (value = Success)
        #             - RLOC16 TLV
        #             - Router Mask TLV

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

        # Step 8: Router Sends a Link Request Message.
        #         The Link Request Message MUST be multicast and contain
        #         the following TLVs:
        #             - Challenge TLV
        #             - Leader Data TLV
        #             - Source Address TLV
        #             - Version TLV
        #             - TLV Request TLV: Link Margin

        pkts.filter_wpan_src64(ROUTER).\
            filter_LLARMA().\
            filter_mle_cmd(MLE_LINK_REQUEST).\
            filter(lambda p: {
                              CHALLENGE_TLV,
                              LEADER_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              VERSION_TLV,
                              TLV_REQUEST_TLV,
                              LINK_MARGIN_TLV
                              } <= set(p.mle.tlv.type)\
                   ).\
            must_next()

        # Step 9: Leader sends a Unicast Link Accept and Request Message.
        #         The Message MUST be unicast to Router
        #         The Message MUST contain the following TLVs:
        #             - Leader Data TLV
        #             - Link-layer Frame Counter TLV
        #             - Link Margin TLV
        #             - Response TLV
        #             - Source Address TLV
        #             - Version TLV
        #             - Challenge TLV (optional)
        #             - MLE Frame Counter TLV (optional)

        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
            filter_mle_cmd(MLE_LINK_ACCEPT_AND_REQUEST).\
            filter(lambda p: {
                              LEADER_DATA_TLV,
                              LINK_LAYER_FRAME_COUNTER_TLV,
                              LINK_MARGIN_TLV,
                              RESPONSE_TLV,
                              SOURCE_ADDRESS_TLV,
                              VERSION_TLV
                               } <= set(p.mle.tlv.type)).\
                   must_next()

        # Step 10: Router is sending properly formatted MLE Advertisements.
        #          MLE Advertisements MUST be sent with an IP Hop Limit of
        #          255 to the Link-Local All Nodes multicast address (FF02::1).
        #          The following TLVs MUST be present in the MLE Advertisements:
        #              - Leader Data TLV
        #              - Route64 TLV
        #              - Source Address TLV

        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p: {
                              LEADER_DATA_TLV,
                              ROUTE64_TLV,
                              SOURCE_ADDRESS_TLV
                              } == set(p.mle.tlv.type) and\
                   p.ipv6.hlim == 255).\
            must_next()

        # Step 11: DUT responds with ICMPv6 Echo Reply

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(LEADER).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
            must_next()

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(LEADER).\
            must_next()


if __name__ == '__main__':
    unittest.main()
