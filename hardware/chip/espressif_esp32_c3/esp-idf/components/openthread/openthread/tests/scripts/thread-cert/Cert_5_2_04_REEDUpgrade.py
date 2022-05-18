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
from pktverify.consts import MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, MLE_LINK_REQUEST, ADDR_SOL_URI, SOURCE_ADDRESS_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MLE_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV, NL_MAC_EXTENDED_ADDRESS_TLV, NL_RLOC16_TLV, NL_STATUS_TLV, NL_ROUTER_MASK_TLV, COAP_CODE_ACK
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
ROUTER = 16
DUT_REED = 17
MED = 18
REED_ADVERTISEMENT_INTERVAL = 570
REED_ADVERTISEMENT_MAX_JITTER = 60
ROUTER_SELECTION_JITTER = 1

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to:
#   1) Verify that the DUT does not attempt to become a router if there
#      are already 16 active routers on the Thread network AND it is not
#      bringing children
#   2) Verify that the DUT transmits MLE Advertisement messages every
#      REED_ADVERTISEMENT_INTERVAL (+REED_ADVERTISEMENT_MAX_JITTER) seconds
#   3) Verify that the DUT upgrades to a router by sending an Address Solicit
#      Request when a child attempts to attach to it.
#
# Test Topology:
# -------------
#  Router_15 - Leader
#      ...    /     \
#         Router_n  Router_1(DUT)
#          |
#      REED(DUT)
#          |
#         MED
#
# DUT Types:
# ----------
#  REED


class Cert_5_2_4_REEDUpgrade(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, ROUTER]
        },
        2: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        3: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        4: {
            'name': 'ROUTER_3',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        5: {
            'name': 'ROUTER_4',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        6: {
            'name': 'ROUTER_5',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        7: {
            'name': 'ROUTER_6',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        8: {
            'name': 'ROUTER_7',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        9: {
            'name': 'ROUTER_8',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        10: {
            'name': 'ROUTER_9',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        11: {
            'name': 'ROUTER_10',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        12: {
            'name': 'ROUTER_11',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        13: {
            'name': 'ROUTER_12',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        14: {
            'name': 'ROUTER_13',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        15: {
            'name': 'ROUTER_14',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        ROUTER: {
            'name': 'ROUTER_15',
            'mode': 'rdn',
            'allowlist': [LEADER, DUT_REED]
        },
        DUT_REED: {
            'name': 'REED',
            'mode': 'rdn',
            'allowlist': [ROUTER, MED]
        },
        MED: {
            'name': 'MED',
            'mode': 'rdn',
            'allowlist': [DUT_REED]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        for i in range(2, 17):
            self.nodes[i].start()
            self.simulator.go(5)
            self.assertEqual(self.nodes[i].get_state(), 'router')

        self.nodes[DUT_REED].start()
        self.simulator.go(5)
        self.simulator.go(ROUTER_SELECTION_JITTER)

        self.collect_rloc16s()
        self.collect_rlocs()

        self.simulator.go(REED_ADVERTISEMENT_INTERVAL + REED_ADVERTISEMENT_MAX_JITTER)

        self.nodes[MED].start()
        self.simulator.go(5)
        self.collect_ipaddrs()

        mleid = self.nodes[LEADER].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        self.assertTrue(self.nodes[MED].ping(mleid))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER_RLOC16 = pv.vars['LEADER_RLOC16']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        LEADER_MLEID = pv.vars['LEADER_MLEID']
        REED = pv.vars['REED']
        REED_RLOC16 = pv.vars['REED_RLOC16']
        MED = pv.vars['MED']
        MED_RLOC16 = pv.vars['MED_RLOC16']
        MED_MLEID = pv.vars['MED_MLEID']

        # Step 1: Verify topology is formed correctly except REED.

        with pkts.save_index():
            for i in range(1, 16):
                pv.verify_attached('ROUTER_%d' % i)

        # Step 2: REED attaches to the network with 2-hops from the Leader
        #         and MUST NOT attempt to become an active router by sending
        #         an Address Solicit Request

        pv.verify_attached('REED')
        lstart = pkts.index

        # Step 3: REED MUST send properly formatted MLE Advertisements.
        #         MLE Advertisements MUST be sent with an IP Hop Limit of
        #         255 to the Link-Local All Nodes multicast address (FF02::1).
        #         The following TLVs MUST be present in the MLE Advertisements:
        #             - Leader Data TLV
        #             - Source Address TLV
        #         The following TLV MUST NOT be present in the MLE Advertisements:
        #             - Route64 TLV

        _pkt = pkts.filter_wpan_src64(REED).\
            filter_mle_advertisement('REED').\
            must_next()
        lend = pkts.index

        pkts.range(lstart, lend).filter_wpan_src64(REED).\
            filter_coap_request(ADDR_SOL_URI).\
            filter(lambda p: {
                              NL_MAC_EXTENDED_ADDRESS_TLV,
                              NL_STATUS_TLV
                              } == set(p.coap.tlv.type)
                  ).\
            must_not_next()

        # Step 5: REED MUST send a second MLE Advertisement after
        #         REED_ADVERTISEMENT_INTERVAL+JITTER where
        #         JITTER <= REED_ADVERTISEMENT_MAX_JITTER

        pkts.filter_wpan_src64(REED).\
            filter_mle_advertisement('REED').\
            filter(lambda p:
                   REED_ADVERTISEMENT_INTERVAL <
                   p.sniff_timestamp - _pkt.sniff_timestamp <=
                   REED_ADVERTISEMENT_INTERVAL +
                   REED_ADVERTISEMENT_MAX_JITTER
                   ).\
            must_next()

        # Step 6: MED sends multicast MLE Parent Request
        #         First one is to all routers, the second one is to
        #         all routers and reeds

        pkts.filter_wpan_src64(MED).\
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
        pkts.filter_wpan_src64(MED).\
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
                   p.mle.tlv.scan_mask.e == 1).\
            must_next()

        # Step 7: REED MUST reply with a properly formatted MLE Parent Response

        pkts.filter_wpan_src64(REED).\
            filter_wpan_dst64(MED).\
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

        # Step 8: MED sends MLE Child ID Request to REED

        _pkt = pkts.filter_wpan_src64(MED).\
            filter_wpan_dst64(REED).\
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

        # Step 9: REED sends an Address Solicit Request to the Leader
        #         Ensure the Address Solicit Request is properly formatted:
        #         CoAP Request URI
        #             coap://<leader address>:MM/a/as
        #         CoAP Payload
        #             - MAC Extended Address TLV
        #             - Status TLV

        pkts.filter_wpan_src64(REED).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_request(ADDR_SOL_URI).\
            filter(lambda p: {
                              NL_MAC_EXTENDED_ADDRESS_TLV,
                              NL_STATUS_TLV
                              } <= set(p.coap.tlv.type)
                   ).\
            must_next()

        # Step 10: REED Sends a Link Request Message.
        #          The Link Request Message MUST be multicast and contain
        #          the following TLVs:
        #              - Challenge TLV
        #              - Leader Data TLV
        #              - Source Address TLV
        #              - TLV Request TLV: Link Margin
        #              - Version TLV

        pkts.filter_wpan_src64(REED).\
            filter_LLARMA().\
            filter_mle_cmd(MLE_LINK_REQUEST).\
            filter(lambda p: {
                              CHALLENGE_TLV,
                              LEADER_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              VERSION_TLV,
                              TLV_REQUEST_TLV,
                              LINK_MARGIN_TLV
                              } <= set(p.mle.tlv.type)
                   ).\
            must_next()

        # Step 11: The REED MLE Child ID Response MUST be properly
        #          formatted with MED_1’s new 16-bit address.

        pkts.filter_wpan_src64(REED).\
            filter_wpan_dst64(MED).\
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
                              } <= set(p.mle.tlv.type) and\
                   p.mle.tlv.source_addr != REED_RLOC16 and\
                   p.mle.tlv.addr16 != MED_RLOC16
                   ).\
                   must_next()

        # Step 12: The Leader MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ipv6_src_dst(MED_MLEID, LEADER_MLEID).\
                    filter_ping_request().\
                    must_next()
        pkts.filter_ipv6_src_dst(LEADER_MLEID, MED_MLEID).\
            filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            must_next()


if __name__ == '__main__':
    unittest.main()
