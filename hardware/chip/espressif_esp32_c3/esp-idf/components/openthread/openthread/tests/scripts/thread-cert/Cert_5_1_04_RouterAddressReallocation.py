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
from pktverify.consts import MLE_ADVERTISEMENT, MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_ID_RESPONSE, ADDR_SOL_URI, MODE_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, CONNECTIVITY_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, SCAN_MASK_TLV, VERSION_TLV, LINK_MARGIN_TLV, SOURCE_ADDRESS_TLV, COAP_CODE_ACK, NL_STATUS_TLV, NL_RLOC16_TLV, NL_ROUTER_MASK_TLV
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ROUTER1 = 2
ROUTER2 = 3

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify that when the original Leader is
# removed from the network, the DUT will create a new partition as Leader and
# will assign a router ID if a specific ID is requested.
#
# Test Topology:
# -------------
#        Leader
#       /     \
# Router_1  -  Router_2
# [DUT]
#
# DUT Types:
# ----------
# Router


class Cert_5_1_04_RouterAddressReallocation(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'partition_id': 1,
            'allowlist': [ROUTER1, ROUTER2]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER, ROUTER2]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER, ROUTER1]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'router')

        self.nodes[ROUTER2].set_network_id_timeout(200)
        self.nodes[LEADER].stop()
        self.simulator.go(220)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'leader')
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'router')

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_2 = pv.vars['ROUTER_2']
        MM = pv.vars['MM_PORT']

        # Step 3: Verify topology is formed correctly.

        pv.verify_attached('ROUTER_1')
        _pkt_pt = pkts.filter_wpan_src64(ROUTER_1).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            must_next()
        with pkts.save_index():
            _pkt_id = pkts.filter_wpan_src64(ROUTER_1).\
                filter_wpan_dst64(ROUTER_2).\
                filter_mle_cmd(MLE_PARENT_RESPONSE).\
                must_next()

        pv.verify_attached('ROUTER_2')
        _pkt_as = pkts.filter_wpan_src64(LEADER).\
            filter_coap_ack(ADDR_SOL_URI, port=MM).\
            must_next()

        # Step 5: Router_1 MUST attempt to reattach to its original partition
        #         by sending a MLE Parent Request with a hop limit of 255 to
        #         the All-Routers multicast address (FF02::2).
        #         The following TLVs MUST be present in the MLE Parent Request:
        #            - Challenge TLV
        #            - Mode TLV
        #            - Scan Mask TLV (MUST have E and R flags set)
        #            - Version TLV
        #         The DUT MUST make two separate attempts to reconnect to its
        #         original partition in this manner

        with pkts.save_index():
            for i in range(1, 3):
                pkts.filter_wpan_src64(ROUTER_1).\
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
                           p.mle.tlv.scan_mask.e == 1
                           ).\
                    must_next()

            # Step 6: Router_1 MUST attempt to attach to any other partition
            #         within range by sending a MLE Parent Request.
            #         The following TLVs MUST be present in the MLE Parent Request:
            #            - Challenge TLV
            #            - Mode TLV
            #            - Scan Mask TLV
            #            - Version TLV

            pkts.filter_wpan_src64(ROUTER_1).\
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

        # Step 7: Router_1 MUST create a new partition and update the following
        #         values randomly:
        #             - Partition ID
        #             - Initial VN_version & VN_stable_version
        #             - Initial ID sequence number
        #  Notes: Considerring the randomly created VN_version & VN_stable_version
        #         could equal the previous versions, checking one of them to
        #         reduce case failures

        pkts.filter_wpan_src64(ROUTER_1).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p:\
                   p.mle.tlv.leader_data.partition_id !=
                   _pkt_pt.mle.tlv.leader_data.partition_id and\
                   (p.mle.tlv.leader_data.data_version !=
                    _pkt_pt.mle.tlv.leader_data.data_version or\
                    p.mle.tlv.leader_data.stable_data_version !=
                    _pkt_pt.mle.tlv.leader_data.stable_data_version)
                   ).\
            must_next()

        # Step 9: Router_1 MUST send a properly formatted Parent Response and
        #         Child ID Response to Router_2.

        pkts.filter_wpan_src64(ROUTER_1).\
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
                   p.mle.tlv.conn.id_seq != _pkt_id.mle.tlv.conn.id_seq
                   ).\
            must_next()

        pkts.filter_wpan_src64(ROUTER_1).\
            filter_wpan_dst64(ROUTER_2).\
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
                             } <= set(p.mle.tlv.type)\
                   ).\
            must_next()

        # Step 10: Router_1 MUST send a properly-formatted Address Solicit
        #          Response Message to Router_2.
        #          If a specific router ID is requested, the DUT MUST provide this
        #          router ID:
        #          CoAP Response Code
        #              - 2.04 Changed
        #          CoAP Payload
        #              - Status TLV (value = Success)
        #              - RLOC16 TLV
        #              - Router Mask TLV

        pkts.filter_wpan_src64(ROUTER_1).\
            filter_coap_ack(ADDR_SOL_URI, port=MM).\
            filter(lambda p: {
                              NL_STATUS_TLV,
                              NL_RLOC16_TLV,
                              NL_ROUTER_MASK_TLV
                              } == set(p.coap.tlv.type) and\
                   p.coap.code == COAP_CODE_ACK and\
                   p.thread_address.tlv.rloc16 ==
                   _pkt_as.thread_address.tlv.rloc16 and\
                   p.thread_address.tlv.status == 0
                   ).\
            must_next()


if __name__ == '__main__':
    unittest.main()
