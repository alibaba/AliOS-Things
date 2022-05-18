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
from pktverify.consts import DIAG_RST_URI, DIAG_GET_URI, DG_MAC_EXTENDED_ADDRESS_TLV, DG_MAC_ADDRESS_TLV, DG_MODE_TLV, DG_CONNECTIVITY_TLV, DG_ROUTE64_TLV, DG_LEADER_DATA_TLV, DG_NETWORK_DATA_TLV, DG_IPV6_ADDRESS_LIST_TLV, DG_CHANNEL_PAGES_TLV, DG_TYPE_LIST_TLV, DG_MAC_COUNTERS_TLV, DG_TIMEOUT_TLV, DG_BATTERY_LEVEL_TLV, DG_SUPPLY_VOLTAGE_TLV, DG_CHILD_TABLE_TLV, DG_CHILD_TABLE_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
ROUTER1 = 2
REED1 = 3
SED1 = 4
MED1 = 5
FED1 = 6

MTDS = [MED1, SED1]

# Test Purpose and Description:
# -----------------------------
# These cases test the Diagnostic Get and Reset Commands as a part of the
# Network Management
#
# Test Topology:
# -------------
#        Leader
#          |
#  FED - Router - REED
#        /    \
#      MED    SED
#
# DUT Types:
# ----------
#  Router
#  FED


class Cert_5_7_01_CoapDiagCommands_Base(thread_cert.TestCase):
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
            'allowlist': [LEADER, REED1, SED1, MED1, FED1],
        },
        REED1: {
            'name': 'REED',
            'mode': 'rdn',
            'allowlist': [ROUTER1],
            'router_upgrade_threshold': 0
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

        for i in range(3, 7):
            self.nodes[i].start()
            self.simulator.go(10)
            self.assertEqual(self.nodes[i].get_state(), 'child')

        self.simulator.go(config.MAX_ADVERTISEMENT_INTERVAL)

        self.collect_rlocs()
        self.collect_rloc16s()
        self.collect_ipaddrs()

        DUT = ROUTER1
        if self.TOPOLOGY[FED1]['name'] == 'DUT':
            DUT = FED1

        dut_rloc = self.nodes[DUT].get_ip6_address(config.ADDRESS_TYPE.RLOC)

        # 2 - Leader sends DIAG_GET.req
        tlv_types = [
            TlvType.EXT_ADDRESS, TlvType.ADDRESS16, TlvType.MODE, TlvType.CONNECTIVITY, TlvType.ROUTE64,
            TlvType.LEADER_DATA, TlvType.NETWORK_DATA, TlvType.IPV6_ADDRESS_LIST, TlvType.CHANNEL_PAGES
        ]
        self.nodes[LEADER].send_network_diag_get(dut_rloc, tlv_types)
        self.simulator.go(2)

        # 3 - Leader sends DIAG_GET.req (MAC Counters TLV type included)
        self.nodes[LEADER].send_network_diag_get(dut_rloc, [TlvType.MAC_COUNTERS])
        self.simulator.go(2)

        # 4 - Leader sends DIAG_GET.req (Timeout/Polling Period TLV type included)
        self.nodes[LEADER].send_network_diag_get(dut_rloc, [TlvType.POLLING_PERIOD])
        self.simulator.go(2)

        # 5 - Leader sends DIAG_GET.req (Battery Level and Supply Voltage TLV types included)
        self.nodes[LEADER].send_network_diag_get(dut_rloc, [TlvType.BATTERY_LEVEL, TlvType.SUPPLY_VOLTAGE])
        self.simulator.go(2)

        # 6 - Leader sends DIAG_GET.req (Child Table TLV types included)
        self.nodes[LEADER].send_network_diag_get(dut_rloc, [TlvType.CHILD_TABLE])
        self.simulator.go(2)

        # 7 - Leader sends DIAG_RST.ntf (MAC Counters TLV type included)
        self.nodes[LEADER].send_network_diag_reset(dut_rloc, [TlvType.MAC_COUNTERS])
        self.simulator.go(2)

        # 8 - Leader Sends DIAG_GET.req (MAC Counters TLV type included)
        self.nodes[LEADER].send_network_diag_get(dut_rloc, [TlvType.MAC_COUNTERS])
        self.simulator.go(2)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        DUT = pv.vars['DUT']
        DUT_RLOC = pv.vars['DUT_RLOC']
        DUT_RLOC16 = pv.vars['DUT_RLOC16']
        REED = pv.vars['REED']
        REED_RLOC = pv.vars['REED_RLOC']
        MED = pv.vars['MED']
        MED_RLOC = pv.vars['MED_RLOC']
        SED = pv.vars['SED']
        SED_RLOC = pv.vars['SED_RLOC']
        MM = pv.vars['MM_PORT']

        #DUT_IPADDR =

        # Step 1: Ensure topology is formed correctly
        if self.TOPOLOGY[ROUTER1]['name'] == 'DUT':
            FED = pv.vars['FED']
            pv.verify_attached('DUT', 'LEADER')
            pv.verify_attached('REED', 'DUT')
            pv.verify_attached('SED', 'DUT', 'MTD')
            pv.verify_attached('MED', 'DUT', 'MTD')
            pv.verify_attached('FED', 'DUT', 'FTD-ED')
        else:
            ROUTER = pv.vars['ROUTER']
            pv.verify_attached('ROUTER', 'LEADER')
            pv.verify_attached('REED', 'ROUTER')
            pv.verify_attached('SED', 'ROUTER', 'MTD')
            pv.verify_attached('MED', 'ROUTER', 'MTD')
            pv.verify_attached('DUT', 'ROUTER', 'FTD-ED')

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
        #             TLV Type 5 – Route64
        #             TLV Type 6 – Leader Data
        #             TLV Type 7 – Network Data
        #             TLV Type 8 – IPv6 address list
        #             TLV Type 17 – Channel Pages
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(DUT_RLOC).\
            filter_coap_request(DIAG_GET_URI).\
            filter(lambda p: {
                              DG_TYPE_LIST_TLV,
                              DG_MAC_EXTENDED_ADDRESS_TLV,
                              DG_MAC_ADDRESS_TLV,
                              DG_MODE_TLV,
                              DG_CONNECTIVITY_TLV,
                              DG_ROUTE64_TLV,
                              DG_LEADER_DATA_TLV,
                              DG_NETWORK_DATA_TLV,
                              DG_IPV6_ADDRESS_LIST_TLV,
                              DG_CHANNEL_PAGES_TLV
                              } == set(p.thread_diagnostic.tlv.type)
                   ).\
            must_next()
        pkts.filter_wpan_src64(DUT).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_ack(DIAG_GET_URI).\
            filter(lambda p: {
                              DG_MAC_EXTENDED_ADDRESS_TLV,
                              DG_MAC_ADDRESS_TLV,
                              DG_MODE_TLV,
                              DG_CONNECTIVITY_TLV,
                              DG_ROUTE64_TLV,
                              DG_LEADER_DATA_TLV,
                              DG_NETWORK_DATA_TLV,
                              DG_IPV6_ADDRESS_LIST_TLV,
                              DG_CHANNEL_PAGES_TLV
                              } == set(p.thread_diagnostic.tlv.type)
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
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(DUT_RLOC).\
            filter_coap_request(DIAG_GET_URI).\
            filter(lambda p: {
                              DG_TYPE_LIST_TLV,
                              DG_TIMEOUT_TLV
                              } == set(p.thread_diagnostic.tlv.type)
                   ).\
            must_next()
        pkts.filter_wpan_src64(DUT).\
            filter_ipv6_dst(LEADER_RLOC).\
            filter_coap_ack(DIAG_GET_URI).\
            filter(lambda p: p.thread_diagnostic.tlv.type is nullField).\
            must_next()

        # Step 5: Leader to send DIAG_GET.req to DUT’s RLOC.
        #         The DUT MUST respond with a DIAG_GET.rsp response containing
        #         the requested diagnostic TLVs:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             TLV Type 3 - Timeout MUST be omitted from the response
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
            filter(lambda p: p.thread_diagnostic.tlv.type is nullField).\
            must_next()

        # Step 6: Leader to send DIAG_GET.req to DUT’s RLOC.
        #         The DUT MUST respond with a DIAG_GET.rsp response containing
        #         the requested diagnostic TLVs:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             TLV Type 16 - Child Table (is empty if FED is DUT)
        #
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(DUT_RLOC).\
            filter_coap_request(DIAG_GET_URI).\
            filter(lambda p: {
                              DG_TYPE_LIST_TLV,
                              DG_CHILD_TABLE_TLV
                              } == set(p.thread_diagnostic.tlv.type)
                   ).\
            must_next()

        if self.TOPOLOGY[ROUTER1]['name'] == 'DUT':
            _pkt = pkts.filter_wpan_src64(DUT).\
                filter_ipv6_dst(LEADER_RLOC).\
                filter_coap_ack(DIAG_GET_URI).\
                filter(lambda p: {
                                  DG_CHILD_TABLE_TLV
                                  } == set(p.thread_diagnostic.tlv.type)
                       ).\
                must_next()
        else:
            pkts.filter_wpan_src64(DUT).\
                filter_ipv6_dst(LEADER_RLOC).\
                filter_coap_ack(DIAG_GET_URI).\
                filter(lambda p: p.thread_diagnostic.tlv.type is nullField).\
                must_next()

        # Step 7: Leader to send DIAG_RST.req to DUT’s RLOC for the following diagnostic
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

        # Step 8: Leader to send DIAG_GET.req to DUT’s RLOC.
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


class Cert_5_7_01_CoapDiagCommands_Base_ROUTER(Cert_5_7_01_CoapDiagCommands_Base):
    TOPOLOGY = copy.deepcopy(Cert_5_7_01_CoapDiagCommands_Base.TOPOLOGY)
    TOPOLOGY[ROUTER1]['name'] = 'DUT'
    TOPOLOGY[FED1]['name'] = 'FED'


class Cert_5_7_01_CoapDiagCommands_Base_FED(Cert_5_7_01_CoapDiagCommands_Base):
    TOPOLOGY = copy.deepcopy(Cert_5_7_01_CoapDiagCommands_Base.TOPOLOGY)
    TOPOLOGY[ROUTER1]['name'] = 'ROUTER'
    TOPOLOGY[FED1]['name'] = 'DUT'


del (Cert_5_7_01_CoapDiagCommands_Base)

if __name__ == '__main__':
    unittest.main()
