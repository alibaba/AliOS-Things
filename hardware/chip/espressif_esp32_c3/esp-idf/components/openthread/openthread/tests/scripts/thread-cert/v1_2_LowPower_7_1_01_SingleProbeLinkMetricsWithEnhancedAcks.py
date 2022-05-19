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
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS'
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

from config import ADDRESS_TYPE
from pktverify import consts

import thread_cert

LEADER = 1
SED_1 = 2
SSED_1 = 3

POLL_PERIOD = 3000  # 3s


class LowPower_7_1_01(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'version': '1.2',
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [SED_1, SSED_1],
        },
        SED_1: {
            'version': '1.2',
            'name': 'SED_1',
            'mode': '-',
            'allowlist': [LEADER],
        },
        SSED_1: {
            'version': '1.2',
            'name': 'SSED_1',
            'mode': '-',
            'allowlist': [LEADER],
        }
    }
    """All nodes are created with default configurations"""

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[SED_1].set_pollperiod(POLL_PERIOD)
        self.nodes[SED_1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED_1].get_state(), 'child')

        self.nodes[SSED_1].set_csl_period(consts.CSL_DEFAULT_PERIOD)
        self.nodes[SSED_1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SSED_1].get_state(), 'child')

        leader_addr = self.nodes[LEADER].get_ip6_address(ADDRESS_TYPE.LINK_LOCAL)

        # Step 3 - Verify connectivity by instructing each device to send an ICMPv6 Echo Request to the DUT
        self.assertTrue(self.nodes[SED_1].ping(leader_addr, timeout=POLL_PERIOD * 2 / 1000))
        self.assertTrue(self.nodes[SSED_1].ping(leader_addr))
        self.simulator.go(5)

        # Step 4 - SED_1 enables IEEE 802.15.4-2015 Enhanced ACK based Probing by sending a Link Metrics Management
        # Request to the DUT
        # MLE Link Metrics Management TLV Payload:
        # - Enhanced ACK Configuration Sub-TLV
        # -- Enh-ACK Flags = 1 (register a configuration)
        # -- Concatenation of Link Metric Type ID Flags = 0x00:
        # --- Item1: (0)(0)(001)(010) = 0x0a
        # ---- E = 0
        # ---- L = 0
        # ---- Type/Average Enum = 1 (Exponential Moving Avg)
        # ---- Metrics Enum = 2 (Link Margin)
        self.nodes[SED_1].link_metrics_mgmt_req_enhanced_ack_based_probing(leader_addr, True, 'm')
        self.simulator.go(5)

        # Step 6 - SSED_1 enables IEEE 802.15.4-2015 Enhanced ACK based Probing by sending a Link Metrics Management
        # Request to the DUT
        # MLE Link Metrics Management TLV Payload:
        # - Enhanced ACK Configuration Sub-TLV
        # -- Enh-ACK Flags = 1 (register a configuration)
        # -- Concatenation of Link Metric Type ID Flags = 0x00:
        # --- Item1: (0)(0)(001)(010) = 0x0a
        # --- Item2: (0)(0)(001)(011) = 0x0b
        # ---- E = 0
        # ---- L = 0
        # ---- Type/Average Enum = 1 (Exponential Moving Avg)
        # ---- Metrics Enum = 2 (Link Margin)
        # ---- Metrics Enum = 3 (RSSI)
        self.nodes[SSED_1].link_metrics_mgmt_req_enhanced_ack_based_probing(leader_addr, True, 'mr')
        self.simulator.go(5)

        # Step 8 - SSED_1 sends an MLE Data Message with setting Frame Version subfield within the MAC header of the
        # message to 0b10
        self.nodes[SSED_1].send_mac_emptydata()
        self.simulator.go(5)

        # Step 10 - SED_1 sends an MLE Data Message with setting Frame Version subfield within the MAC header of the
        # message # to 0b10
        self.nodes[SED_1].send_mac_emptydata()
        self.simulator.go(5)

        # Step 12 - SSED_1 clears its Enhanced ACK link metrics configuration by # sending a Link Metrics Management
        # Request to the DUT
        # Enh-ACK Flags = 0 (clear enhanced ACK link metric config)
        self.nodes[SSED_1].link_metrics_mgmt_req_enhanced_ack_based_probing(leader_addr, False, '')
        self.simulator.go(5)

        # Step 14 - SSED_1 Sends a MLE Data Message with setting Frame Version subfield within the MAC header of the
        # message to 0b10
        self.nodes[SSED_1].send_mac_emptydata()
        self.simulator.go(5)

        # Step 16 - This step verifies that Enhanced ACKs cannot be enabled while requesting 3 metric types by
        # instructing the device to send the following Link Metrics Management Request to the DUT
        # MLE Link Metrics Management TLV Payload:
        # - Enhanced ACK Configuration Sub-TLV
        # -- Enh-ACK Flags = 1 (register a configuration)
        # -- Concatenation of Link Metric Type ID Flags = 0x00:
        # --- Item1: (0)(0)(001)(001) = 0x09
        # --- Item2: (0)(0)(001)(010) = 0x0a
        # --- Item3: (0)(0)(001)(011) = 0x0b
        # ---- E = 0
        # ---- L = 0
        # ---- Type/Average Enum = 1 (Exponential Moving Avg)
        # ---- Metrics Enum = 1 (Layer 2 LQI)
        # ---- Metrics Enum = 2 (Link Margin)
        # ---- Metrics Enum = 3 (RSSI)
        self.nodes[SSED_1].link_metrics_mgmt_req_enhanced_ack_based_probing(leader_addr, True, 'qmr')
        self.simulator.go(5)

        # Step 18 - This step verifies that Enhanced ACKs cannot be enabled while requesting a reserved Type/Average
        # enum of value 2 by instructing the device to send the following Link Metrics Management Request to the DUT
        # MLE Link Metrics Management TLV Payload:
        # - Enhanced ACK Configuration Sub-TLV
        # -- Enh-ACK Flags = 1 (register a configuration)
        # -- Concatenation of Link Metric Type ID Flags = 0x00:
        # --- Item1: (0)(0)(010)(010) = 0x12
        # ---- E = 0
        # ---- L = 0
        # ---- Type/Average Enum = 2 (Reserved)
        # ---- Metrics Enum = 2 (Link Margin)
        self.nodes[SSED_1].link_metrics_mgmt_req_enhanced_ack_based_probing(leader_addr, True, 'm', 'r')
        self.simulator.go(5)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()
        LEADER = pv.vars['LEADER']
        SED_1 = pv.vars['SED_1']
        SSED_1 = pv.vars['SSED_1']

        # Step 3 - The DUT MUST send ICMPv6 Echo Responses to both SED1 & SSED1
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(SED_1) \
            .filter_ping_reply() \
            .must_next()
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(SSED_1) \
            .filter_ping_reply() \
            .must_next()

        # Step 4 - SED_1 enables IEEE 802.15.4-2015 Enhanced ACK based Probing by sending a Link Metrics Management
        # Request to the DUT
        # MLE Link Metrics Management TLV Payload:
        # - Enhanced ACK Configuration Sub-TLV
        # -- Enh-ACK Flags = 1 (register a configuration)
        # -- Concatenation of Link Metric Type ID Flags = 0x00:
        # --- Item1: (0)(0)(001)(010) = 0x0a
        # ---- E = 0
        # ---- L = 0
        # ---- Type/Average Enum = 1 (Exponential Moving Avg)
        # ---- Metrics Enum = 2 (Link Margin)
        pkts.filter_wpan_src64(SED_1) \
            .filter_wpan_dst64(LEADER) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_REQUEST) \
            .filter(lambda p: p.mle.tlv.link_sub_tlv == consts.LM_ENHANCED_ACK_CONFIGURATION_SUB_TLV) \
            .filter(lambda p: p.mle.tlv.link_enh_ack_flags == consts.LINK_METRICS_ENH_ACK_PROBING_REGISTER) \
            .filter(lambda p: p.mle.tlv.link_requested_type_id_flags == '0a') \
            .must_next()

        # Step 5 - The DUT MUST send a Link Metrics Management Response to SED_1 containing the following TLVs:
        # - MLE LInk Metrics Management TLV
        # -- Link Metrics Status Sub-TLV = 0 (Success)
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(SED_1) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_SUCCESS) \
            .must_next()

        # Step 6 - SSED_1 enables IEEE 802.15.4-2015 Enhanced ACK based Probing by sending a Link Metrics Management
        # Request to the DUT
        # MLE Link Metrics Management TLV Payload:
        # - Enhanced ACK Configuration Sub-TLV
        # -- Enh-ACK Flags = 1 (register a configuration)
        # -- Concatenation of Link Metric Type ID Flags = 0x00:
        # --- Item1: (0)(0)(001)(010) = 0x0a
        # --- Item2: (0)(0)(001)(011) = 0x0b
        # ---- E = 0
        # ---- L = 0
        # ---- Type/Average Enum = 1 (Exponential Moving Avg)
        # ---- Metrics Enum = 2 (Link Margin)
        # ---- Metrics Enum = 3 (RSSI)
        pkts.filter_wpan_src64(SSED_1) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_REQUEST) \
            .filter(lambda p: p.mle.tlv.link_sub_tlv == consts.LM_ENHANCED_ACK_CONFIGURATION_SUB_TLV) \
            .filter(lambda p: p.mle.tlv.link_enh_ack_flags == consts.LINK_METRICS_ENH_ACK_PROBING_REGISTER) \
            .filter(lambda p: p.mle.tlv.link_requested_type_id_flags == '0a0b') \
            .must_next()

        # Step 7 - The DUT MUST send a Link Metrics Management Response to SSED_1
        pkts.filter_wpan_src64(LEADER) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_SUCCESS) \
            .must_next()

        # Step 8 - SSED_1 sends an MLE Data Message with setting Frame Version subfield within the MAC header of the
        # message to 0b10
        pkt = pkts.filter_wpan_src64(SSED_1) \
            .filter_wpan_data() \
            .filter_wpan_version(consts.MAC_FRAME_VERSION_2015) \
            .must_next()
        ack_seq_no = pkt.wpan.seq_no

        # Step 9 - The DUT MUST reply to SSED_1 with an Enhanced ACK containing the following:
        # - Frame Control Field
        # -- Security Enabled = True
        # - Header IE
        # -- Element ID = 0x00
        # --- Vendor CID = 0xEAB89B  (Thread Group)
        # --- Vendor Specific Information
        # ---- 1st byte = 0 (Enhanced ACK Link Metrics)
        # ---- 2nd byte ... Link Margin data
        # ---- 3rd byte ... RSSI data
        pkts.filter_wpan_ack() \
            .filter_wpan_seq(ack_seq_no) \
            .filter(lambda p: p.wpan.payload_ie.vendor.oui == consts.THREAD_IEEE_802154_COMPANY_ID) \
            .must_next()

        # Step 10 - SED_1 sends an MLE Data Message with setting Frame Version subfield within the MAC header of the
        # message to 0b10
        pkt = pkts.filter_wpan_src64(SED_1) \
            .filter_wpan_data() \
            .filter_wpan_version(consts.MAC_FRAME_VERSION_2015) \
            .must_next()
        ack_seq_no = pkt.wpan.seq_no

        # Step 11 - The DUT MUST reply to SED_1 with an Enhanced ACK containing the following:
        # - Frame Control Field
        # -- Security Enabled = True
        # - Header IE
        # -- Element ID = 0x00
        # --- Vendor CID = 0xEAB89B  (Thread Group)
        # --- Vendor Specific Information
        # ---- 1st byte = 0 (Enhanced ACK Link Metrics)
        # ---- 2nd byte ... Link Margin data
        pkts.filter_wpan_ack() \
            .filter_wpan_seq(ack_seq_no) \
            .filter(lambda p: p.wpan.payload_ie.vendor.oui == consts.THREAD_IEEE_802154_COMPANY_ID) \
            .must_next()

        # Step 12 - SSED_1 clears enhanced ACK link metrics configuration by instructing it to send a Link Metrics
        # Management Request to the DUT
        # - MLE Link Metrics Management TLV Payload
        # -- Enhanced ACK Configuration Sub-TLV
        # -- Enh-ACK Flags = 0 (clear enhanced ACK link metric config)
        pkts.filter_wpan_src64(SSED_1) \
            .filter_wpan_dst64(LEADER) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_REQUEST) \
            .filter(lambda p: p.mle.tlv.link_sub_tlv == consts.LM_ENHANCED_ACK_CONFIGURATION_SUB_TLV) \
            .filter(lambda p: p.mle.tlv.link_enh_ack_flags == consts.LINK_METRICS_ENH_ACK_PROBING_CLEAR) \
            .must_next()

        # Step 13 - The DUT MUST send Link Metrics Management Response to SSED_1 containing the following:
        # - MLE Link Metrics Management TLV
        # -- Link Metrics Status Sub-TLV = 0 (Success)
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(SSED_1) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_SUCCESS) \
            .must_next()

        # Step 14 - SSED_1 sends an MLE Data Message with setting Frame Version subfield within the MAC header of the
        # message to 0b10
        pkt = pkts.filter_wpan_src64(SSED_1) \
            .filter_wpan_data() \
            .filter_wpan_version(consts.MAC_FRAME_VERSION_2015) \
            .must_next()
        ack_seq_no = pkt.wpan.seq_no

        # Step 15 - The DUT MUST NOT include a Link Metrics Report in the ACK
        pkts.filter_wpan_ack() \
            .filter_wpan_seq(ack_seq_no) \
            .filter_wpan_ie_not_present() \
            .must_next()

        # Step 16 - This step verifies that Enhanced ACKS cannot be enabled while requesting 3 metric types by
        # instructing the device to send the following Link Metrics Management Request to the DUT:
        # MLE Link Metrics Management TLV Payload:
        # - Enhanced ACK Configuration Sub-TLV
        # -- Enh-ACK Flags = 1 (register a configuration)
        # -- Concatenation of Link Metric Type ID Flags = 0x00:
        # --- Item1: (0)(0)(001)(001) = 0x09
        # --- Item2: (0)(0)(001)(010) = 0x0a
        # --- Item3: (0)(0)(001)(011) = 0x0b
        # ---- E = 0
        # ---- L = 0
        # ---- Type/Average Enum = 1 (Exponential Moving Avg)
        # ---- Metrics Enum = 1 (Layer 2 LQI)
        # ---- Metrics Enum = 2 (Link Margin)
        # ---- Metrics Enum = 3 (RSSI)
        pkts.filter_wpan_src64(SSED_1) \
            .filter_wpan_dst64(LEADER) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_REQUEST) \
            .filter(lambda p: p.mle.tlv.link_sub_tlv == consts.LM_ENHANCED_ACK_CONFIGURATION_SUB_TLV) \
            .filter(lambda p: p.mle.tlv.link_enh_ack_flags == consts.LINK_METRICS_ENH_ACK_PROBING_REGISTER) \
            .filter(lambda p: p.mle.tlv.link_requested_type_id_flags == '090a0b') \
            .must_next()

        # Step 17 - Leader automatically responds to the invalid query from SSED_1 with a failure
        # The DUT MUST send Link Metrics Management Response to SSED_1containing the following:
        # - MLE Link Metrics Management TLV
        # -- Link Metrics Status Sub-TLV = 254 (Failure)
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(SSED_1) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_OTHER_ERROR) \
            .must_next()

        # Step 18 - This step verifies that Enhanced ACKs cannot be enabled while requesting a reserved Type/Average
        # enum of value 2 by instructing the device to send the following Link Metrics Management Request to the DUT
        # MLE Link Metrics Management TLV Payload:
        # - Enhanced ACK Configuration Sub-TLV
        # -- Enh-ACK Flags = 1 (register a configuration)
        # -- Concatenation of Link Metric Type ID Flags = 0x00:
        # --- Item1: (0)(0)(010)(010) = 0x12
        # ---- E = 0
        # ---- L = 0
        # ---- Type/Average Enum = 2 (Reserved)
        # ---- Metrics Enum = 2 (Link Margin)
        pkts.filter_wpan_src64(SSED_1) \
            .filter_wpan_dst64(LEADER) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_REQUEST) \
            .filter(lambda p: p.mle.tlv.link_sub_tlv == consts.LM_ENHANCED_ACK_CONFIGURATION_SUB_TLV) \
            .filter(lambda p: p.mle.tlv.link_enh_ack_flags == consts.LINK_METRICS_ENH_ACK_PROBING_REGISTER) \
            .filter(lambda p: p.mle.tlv.link_requested_type_id_flags == '12') \
            .must_next()

        # Step 19 - Leader automatically responds to the invalid query from SSED_1 with a failure
        # The DUT MUST send Link Metrics Management Response to SSED_1containing the following:
        # - MLE Link Metrics Management TLV
        # -- Link Metrics Status Sub-TLV = 254 (Failure)
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(SSED_1) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_OTHER_ERROR) \
            .must_next()


if __name__ == '__main__':
    unittest.main()
