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

import config
import copy
import mle
import network_diag
import network_layer
import thread_cert
from network_diag import TlvType
from pktverify.consts import DIAG_RST_URI, DIAG_GET_URI, DIAG_GET_QRY_URI, DIAG_GET_ANS_URI, DG_MAC_EXTENDED_ADDRESS_TLV, DG_MAC_ADDRESS_TLV, DG_MODE_TLV, DG_CONNECTIVITY_TLV, DG_ROUTE64_TLV, DG_LEADER_DATA_TLV, DG_NETWORK_DATA_TLV, DG_IPV6_ADDRESS_LIST_TLV, DG_CHANNEL_PAGES_TLV, DG_TYPE_LIST_TLV, DG_MAC_COUNTERS_TLV, DG_TIMEOUT_TLV, DG_BATTERY_LEVEL_TLV, DG_SUPPLY_VOLTAGE_TLV, DG_CHILD_TABLE_TLV, DG_CHILD_TABLE_TLV, REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField
from pktverify.utils import colon_hex

LEADER = 1
ROUTER15 = 16
REED = 17

# Test Purpose and Description:
# -----------------------------
# This test case exercises the Diagnostic Get Query and Answer commands
# as part of the Network Management. This test case topology is specific
# to REED DUTs.
#
# Test Topology:
# -------------
#         Leader
#        /      \
# Router1   ..  Router15
#                    |
#                  REED
#
# DUT Types:
# ----------
#  REED


class Cert_5_7_02_CoapDiagCommands(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, ROUTER15]
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
        ROUTER15: {
            'name': 'ROUTER_15',
            'mode': 'rdn',
            'allowlist': [LEADER, REED]
        },
        REED: {
            'name': 'DUT',
            'mode': 'rdn',
            'allowlist': [ROUTER15]
        },
    }

    def test(self):
        # 1 - Form topology
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        for i in range(2, 17):
            self.nodes[i].start()
            self.simulator.go(10)
            self.assertEqual(self.nodes[i].get_state(), 'router')

        self.nodes[REED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[REED].get_state(), 'child')

        self.simulator.go(config.MAX_ADVERTISEMENT_INTERVAL)

        self.collect_rlocs()
        self.collect_rloc16s()

        dut_rloc = self.nodes[REED].get_ip6_address(config.ADDRESS_TYPE.RLOC)

        # 2 - Leader sends DIAG_GET.req
        tlv_types = [
            TlvType.EXT_ADDRESS, TlvType.ADDRESS16, TlvType.MODE, TlvType.CONNECTIVITY, TlvType.ROUTE64,
            TlvType.LEADER_DATA, TlvType.IPV6_ADDRESS_LIST, TlvType.CHANNEL_PAGES
        ]
        self.nodes[LEADER].send_network_diag_get(dut_rloc, tlv_types)
        self.simulator.go(2)

        # 3 - Leader sends DIAG_GET.req (MAC Counters TLV type included)
        self.nodes[LEADER].send_network_diag_get(dut_rloc, [TlvType.MAC_COUNTERS])
        self.simulator.go(2)

        # 4 - Leader sends DIAG_GET.req (Timeout/Polling Period TLV type included)
        self.nodes[LEADER].send_network_diag_get(dut_rloc, [TlvType.POLLING_PERIOD, TlvType.CHILD_TABLE])
        self.simulator.go(2)

        # 5 - Leader sends DIAG_GET.req (Battery Level and Supply Voltage TLV types included)
        self.nodes[LEADER].send_network_diag_get(dut_rloc, [TlvType.BATTERY_LEVEL, TlvType.SUPPLY_VOLTAGE])
        self.simulator.go(20)

        # 6 - Leader sends DIAG_RST.ntf (MAC Counters TLV type included)
        self.nodes[LEADER].send_network_diag_reset(dut_rloc, [TlvType.MAC_COUNTERS])
        self.simulator.go(2)

        # 7 - Leader Sends DIAG_GET.req (MAC Counters TLV type included)
        self.nodes[LEADER].send_network_diag_get(dut_rloc, [TlvType.MAC_COUNTERS])
        self.simulator.go(2)

        # 8 - Leader sends DIAG_GET.query
        self.nodes[LEADER].send_network_diag_get(REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS, tlv_types)
        self.simulator.go(2)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        DUT = pv.vars['DUT']
        DUT_RLOC = pv.vars['DUT_RLOC']
        DUT_RLOC16 = pv.vars['DUT_RLOC16']
        MM = pv.vars['MM_PORT']

        dut_addr16 = "%04x" % DUT_RLOC16
        dut_payload_tlvs = {
            DG_TYPE_LIST_TLV, DG_MAC_EXTENDED_ADDRESS_TLV, DG_MAC_ADDRESS_TLV, DG_MODE_TLV, DG_CONNECTIVITY_TLV,
            DG_ROUTE64_TLV, DG_LEADER_DATA_TLV, DG_IPV6_ADDRESS_LIST_TLV, DG_CHANNEL_PAGES_TLV
        }

        # Step 1: Ensure topology is formed correctly
        for i in range(1, 16):
            with pkts.save_index():
                pv.verify_attached('ROUTER_%d' % i, 'LEADER')
        pv.verify_attached('DUT', 'ROUTER_15')

        # Step 2: Leader to send DIAG_GET.req to DUT’s RLOC.
        #         The DUT MUST respond with a DIAG_GET.rsp response containing
        #         the requested diagnostic TLVs:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             TLV Type 0 - MAC Extended Address (64- bit)
        #             TLV Type 1 - MAC Address (16-bit)
        #             TLV Type 2 - Mode (Capability information)
        #             TLV Type 4 – Connectivity
        #             TLV Type 5 – Route64(optional)
        #             TLV Type 6 – Leader Data
        #             TLV Type 8 – IPv6 address list
        #             TLV Type 17 – Channel Pages
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(DUT_RLOC).\
            filter_coap_request(DIAG_GET_URI).\
            filter(lambda p: dut_payload_tlvs == set(p.thread_diagnostic.tlv.type)).\
            must_next()
        dut_payload_tlvs.remove(DG_TYPE_LIST_TLV)
        dut_payload_tlvs.remove(DG_ROUTE64_TLV)
        pkts.filter_wpan_src64(DUT).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_ack(DIAG_GET_URI).\
            filter(lambda p:
                   dut_payload_tlvs <= set(p.thread_diagnostic.tlv.type) and\
                   {str(p.wpan.src64), colon_hex(dut_addr16, 2), '0f'}
                   < set(p.thread_diagnostic.tlv.general)
                  ).\
            must_next()

        # Step 3: Leader to send DIAG_GET.req to DUT’s RLOC.
        #         The DUT MUST respond with a DIAG_GET.rsp response containing
        #         the requested diagnostic TLVs:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             TLV Type 9 - MAC Counters
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(DUT_RLOC).\
            filter_coap_request(DIAG_GET_URI).\
            filter(lambda p: {
                              DG_TYPE_LIST_TLV,
                              DG_MAC_COUNTERS_TLV
                              } == set(p.thread_diagnostic.tlv.type)
                   ).\
            must_next()
        pkts.filter_wpan_src64(DUT).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_ack(DIAG_GET_URI).\
            filter(lambda p: {
                              DG_MAC_COUNTERS_TLV
                              } == set(p.thread_diagnostic.tlv.type)
                   ).\
            must_next()

        # Step 4: Leader to send DIAG_GET.req to DUT’s RLOC.
        #         The DUT MUST respond with a DIAG_GET.rsp response containing
        #         the requested diagnostic TLVs:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             TLV Type 3 - Timeout MUST be omitted from the response
        #             TLV Type 16 – Child Table TLV
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(DUT_RLOC).\
            filter_coap_request(DIAG_GET_URI).\
            filter(lambda p: {
                              DG_TYPE_LIST_TLV,
                              DG_TIMEOUT_TLV,
                              DG_CHILD_TABLE_TLV
                              } == set(p.thread_diagnostic.tlv.type)
                   ).\
            must_next()
        pkts.filter_wpan_src64(DUT).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_ack(DIAG_GET_URI).\
            must_next()

        # Step 5: Leader to send DIAG_GET.req to DUT’s RLOC.
        #         The DUT MUST respond with a DIAG_GET.rsp response containing
        #         the requested diagnostic TLVs:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             TLV Type 14 – Battery Level (optional)
        #             TLV Type 15 – Supply Voltage (optional)
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(DUT_RLOC).\
            filter_coap_request(DIAG_GET_URI).\
            filter(lambda p: {
                              DG_TYPE_LIST_TLV,
                              DG_BATTERY_LEVEL_TLV,
                              DG_SUPPLY_VOLTAGE_TLV
                              } == set(p.thread_diagnostic.tlv.type)
                   ).\
            must_next()
        pkts.filter_wpan_src64(DUT).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_ack(DIAG_GET_URI).\
            must_next()

        # Step 6: Leader to send DIAG_RST.req to DUT’s RLOC for the following diagnostic
        #         TLV type:
        #             TLV Type 9 - MAC Counters
        #         The DUT MUST respond with a CoAP response
        #         CoAP Response Code
        #             2.04 Changed
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(DUT_RLOC).\
            filter_coap_request(DIAG_RST_URI).\
            filter(lambda p: {
                              DG_TYPE_LIST_TLV,
                              DG_MAC_COUNTERS_TLV
                              } == set(p.thread_diagnostic.tlv.type)
                   ).\
            must_next()
        pkts.filter_wpan_src64(DUT).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_ack(DIAG_RST_URI).\
            must_next()

        # Step 7: Leader to send DIAG_GET.req to DUT’s RLOC.
        #         The DUT MUST respond with a DIAG_GET.rsp response containing
        #         the requested diagnostic TLVs:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             TLV Type 9 - MAC Counters
        #         TLV Type 9 - MAC Counters MUST contain a list of MAC Counters
        #         with 0 value or less than value returned in step 3.
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(DUT_RLOC).\
            filter_coap_request(DIAG_GET_URI).\
            filter(lambda p: {
                              DG_TYPE_LIST_TLV,
                              DG_MAC_COUNTERS_TLV
                              } == set(p.thread_diagnostic.tlv.type)
                   ).\
            must_next()
        pkts.filter_wpan_src64(DUT).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_ack(DIAG_GET_URI).\
            filter(lambda p: {
                              DG_MAC_COUNTERS_TLV
                              } == set(p.thread_diagnostic.tlv.type)
                   ).\
            must_next()

        # Step 8: Leader sends DIAG_GET.qry to the Realm-Local All-Thread-Nodes
        #         multicast address containing the requested diagnostic TLVs:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             TLV Type 0 - MAC Extended Address (64- bit)
        #             TLV Type 1 - MAC Address (16-bit)
        #             TLV Type 2 - Mode (Capability information)
        #             TLV Type 4 – Connectivity
        #             TLV Type 5 – Route64(optional)
        #             TLV Type 6 – Leader Data
        #             TLV Type 7 – Network Data
        #             TLV Type 8 – IPv6 address list
        #             TLV Type 17 – Channel Pagesi
        dut_payload_tlvs.add(DG_TYPE_LIST_TLV)
        dut_payload_tlvs.add(DG_ROUTE64_TLV)
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS).\
            filter_coap_request(DIAG_GET_QRY_URI).\
            filter(lambda p: dut_payload_tlvs == set(p.thread_diagnostic.tlv.type)).\
            must_next()

        dut_payload_tlvs.remove(DG_TYPE_LIST_TLV)
        dut_payload_tlvs.remove(DG_ROUTE64_TLV)
        pkts.filter_wpan_src64(DUT).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_request(DIAG_GET_ANS_URI).\
            filter(lambda p:
                   dut_payload_tlvs <= set(p.thread_diagnostic.tlv.type) and\
                   {str(p.wpan.src64), colon_hex(dut_addr16, 2), '0f'}
                   < set(p.thread_diagnostic.tlv.general)
                  ).\
            must_next()


if __name__ == '__main__':
    unittest.main()
