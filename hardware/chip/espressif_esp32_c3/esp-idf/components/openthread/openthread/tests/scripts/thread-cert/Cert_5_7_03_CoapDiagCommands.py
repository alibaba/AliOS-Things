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
from pktverify.consts import DIAG_GET_QRY_URI, DIAG_GET_ANS_URI, DG_MAC_EXTENDED_ADDRESS_TLV, DG_MAC_ADDRESS_TLV, DG_MODE_TLV, DG_CONNECTIVITY_TLV, DG_ROUTE64_TLV, DG_LEADER_DATA_TLV, DG_NETWORK_DATA_TLV, DG_IPV6_ADDRESS_LIST_TLV, DG_CHANNEL_PAGES_TLV, DG_TYPE_LIST_TLV, DG_MAC_COUNTERS_TLV, DG_TIMEOUT_TLV, DG_CHILD_TABLE_TLV, REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS
from pktverify.packet_verifier import PacketVerifier
from pktverify.utils import colon_hex

LEADER = 1
ROUTER1 = 2
SED1 = 3
MED1 = 4
FED1 = 5

MTDS = [MED1, SED1]

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify functionality of commands
# Diagnostic_Get.query and Diagnostic_Get.ans. Thread Diagnostic commands
# MUST be supported by FTDs.
#
# Test Topology:
# -------------
#        Leader
#          |
#  FED - Router
#        /    \
#      MED    SED
#
# DUT Types:
# ----------
#  Router
#  FED


class Cert_5_7_03_CoapDiagCommands_Base(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1],
        },
        ROUTER1: {
            'mode': 'rdn',
            'allowlist': [LEADER, SED1, MED1, FED1],
        },
        SED1: {
            'name': 'SED',
            'is_mtd': True,
            'mode': '-',
            'allowlist': [ROUTER1],
            'timeout': config.DEFAULT_CHILD_TIMEOUT
        },
        MED1: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [ROUTER1]
        },
        FED1: {
            'allowlist': [ROUTER1],
            'router_upgrade_threshold': 0
        },
    }

    def test(self):
        # 1 - Form topology
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        for i in range(3, 6):
            self.nodes[i].start()
            self.simulator.go(10)
            self.assertEqual(self.nodes[i].get_state(), 'child')

        self.simulator.go(config.MAX_ADVERTISEMENT_INTERVAL)

        self.collect_rlocs()
        self.collect_rloc16s()

        tlv_types = [
            TlvType.EXT_ADDRESS, TlvType.ADDRESS16, TlvType.MODE, TlvType.CONNECTIVITY, TlvType.ROUTE64,
            TlvType.LEADER_DATA, TlvType.NETWORK_DATA, TlvType.IPV6_ADDRESS_LIST, TlvType.CHILD_TABLE,
            TlvType.CHANNEL_PAGES
        ]
        if self.TOPOLOGY[FED1]['name'] == 'DUT':
            tlv_types = [
                TlvType.EXT_ADDRESS, TlvType.ADDRESS16, TlvType.MODE, TlvType.LEADER_DATA, TlvType.NETWORK_DATA,
                TlvType.IPV6_ADDRESS_LIST, TlvType.CHANNEL_PAGES
            ]

        # 2 - Leader sends DIAG_GET.query
        self.nodes[LEADER].send_network_diag_get(REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS, tlv_types)
        self.simulator.go(2)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        DUT = pv.vars['DUT']
        DUT_RLOC16 = pv.vars['DUT_RLOC16']
        SED = pv.vars['SED']

        dut_addr16 = "%04x" % DUT_RLOC16

        # Step 1: Ensure topology is formed correctly
        if self.TOPOLOGY[ROUTER1]['name'] == 'DUT':
            FED = pv.vars['FED']
            pv.verify_attached('DUT', 'LEADER')
            pv.verify_attached('SED', 'DUT', 'MTD')
            pv.verify_attached('MED', 'DUT', 'MTD')
            pv.verify_attached('FED', 'DUT', 'FTD-ED')
        else:
            ROUTER = pv.vars['ROUTER']
            pv.verify_attached('ROUTER', 'LEADER')
            pv.verify_attached('SED', 'ROUTER', 'MTD')
            pv.verify_attached('MED', 'ROUTER', 'MTD')
            pv.verify_attached('DUT', 'ROUTER', 'FTD-ED')

        # Step 2: Leader sends DIAG_GET.qry to the Realm-Local All-Thread-Nodes
        #         multicast address containing the requested diagnostic TLVs:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             TLV Type 0 - MAC Extended Address (64- bit)
        #             TLV Type 1 - MAC Address (16-bit)
        #             TLV Type 2 - Mode (Capability information)
        #             TLV Type 6 – Leader Data
        #             TLV Type 7 – Network Data
        #             TLV Type 8 – IPv6 address list
        #             TLV Type 17 – Channel Pagesi
        #
        #         if DUT is Router, contianing the following as well:
        #             TLV Type 4 – Connectivity
        #             TLV Type 5 – Route64
        #             TLV Type 16 – Child Table
        _qr_pkt = pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS).\
            filter_coap_request(DIAG_GET_QRY_URI).\
            must_next()
        dut_payload_tlvs = {
            DG_TYPE_LIST_TLV, DG_MAC_EXTENDED_ADDRESS_TLV, DG_MAC_ADDRESS_TLV, DG_MODE_TLV, DG_LEADER_DATA_TLV,
            DG_NETWORK_DATA_TLV, DG_IPV6_ADDRESS_LIST_TLV, DG_CHANNEL_PAGES_TLV
        }
        if self.TOPOLOGY[ROUTER1]['name'] == 'DUT':
            dut_payload_tlvs.update({DG_CONNECTIVITY_TLV, DG_ROUTE64_TLV, DG_CHILD_TABLE_TLV})
            _qr_pkt.must_verify(lambda p: dut_payload_tlvs == set(p.thread_diagnostic.tlv.type))

            # Step 3: The DUT automatically responds with a DIAG_GET.ans response
            #         MUST contain the requested diagnostic TLVs:
            #             TLV Type 0 - MAC Extended Address (64- bit)
            #             TLV Type 1 - MAC Address (16-bit)
            #             TLV Type 2 - Mode (Capability information)
            #             TLV Type 4 – Connectivity
            #             TLV Type 5 – Route64
            #             TLV Type 6 – Leader Data
            #             TLV Type 7 – Network Data
            #             TLV Type 8 – IPv6 address list
            #             TLV Type 16 – Child Table
            #             TLV Type 17 – Channel Pages
            dut_payload_tlvs.remove(DG_TYPE_LIST_TLV)
            pkts.filter_wpan_src64(DUT).\
                filter_ipv6_dst(LEADER_RLOC).\
                filter_coap_request(DIAG_GET_ANS_URI).\
                filter(lambda p:
                       dut_payload_tlvs == set(p.thread_diagnostic.tlv.type) and\
                       {str(p.wpan.src64), colon_hex(dut_addr16, 2), '0f'}
                       < set(p.thread_diagnostic.tlv.general)
                      ).\
                must_next()

            # Step 4: The DUT MUST use IEEE 802.15.4 indirect transmissions to forward
            #         the DIAG_GET.query to SED
            dut_payload_tlvs.add(DG_TYPE_LIST_TLV)
            pkts.filter_wpan_src64(DUT).\
                filter_ipv6_dst(REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS).\
                filter_coap_request(DIAG_GET_QRY_URI).\
                filter(lambda p:
                       dut_payload_tlvs == set(p.thread_diagnostic.tlv.type)
                      ).\
                must_next()

        else:
            # Step 5: The DUT automatically responds with a DIAG_GET.ans response
            #         MUST contain the requested diagnostic TLVs:
            #             TLV Type 0 - MAC Extended Address (64- bit)
            #             TLV Type 1 - MAC Address (16-bit)
            #             TLV Type 2 - Mode (Capability information)
            #             TLV Type 6 – Leader Data
            #             TLV Type 7 – Network Data
            #             TLV Type 8 – IPv6 address list
            #             TLV Type 17 – Channel Pages
            dut_payload_tlvs.remove(DG_TYPE_LIST_TLV)
            pkts.filter_wpan_src64(DUT).\
                filter_ipv6_dst(LEADER_RLOC).\
                filter_coap_request(DIAG_GET_ANS_URI).\
                filter(lambda p:
                       dut_payload_tlvs == set(p.thread_diagnostic.tlv.type) and\
                       {str(p.wpan.src64), colon_hex(dut_addr16, 2), '0f'}
                       < set(p.thread_diagnostic.tlv.general)
                      ).\
                must_next()


class Cert_5_7_03_CoapDiagCommands_Base_ROUTER(Cert_5_7_03_CoapDiagCommands_Base):
    TOPOLOGY = copy.deepcopy(Cert_5_7_03_CoapDiagCommands_Base.TOPOLOGY)
    TOPOLOGY[ROUTER1]['name'] = 'DUT'
    TOPOLOGY[FED1]['name'] = 'FED'


class Cert_5_7_03_CoapDiagCommands_Base_FED(Cert_5_7_03_CoapDiagCommands_Base):
    TOPOLOGY = copy.deepcopy(Cert_5_7_03_CoapDiagCommands_Base.TOPOLOGY)
    TOPOLOGY[ROUTER1]['name'] = 'ROUTER'
    TOPOLOGY[FED1]['name'] = 'DUT'


del (Cert_5_7_03_CoapDiagCommands_Base)

if __name__ == '__main__':
    unittest.main()
