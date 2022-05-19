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

from config import ADDRESS_TYPE
from mle import LinkMetricsSubTlvType, TlvType
from pktverify import consts
from pktverify.null_field import nullField
from pktverify.packet_verifier import PacketVerifier

import thread_cert

LEADER = 1
SED_1 = 2
SSED_1 = 3

SERIES_ID = 1
SERIES_ID_2 = 2
POLL_PERIOD = 2000  # 2s


class LowPower_7_2_01_ForwardTrackingSeries(thread_cert.TestCase):
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
        self.assertTrue(self.nodes[SED_1].ping(leader_addr, timeout=POLL_PERIOD / 1000))
        self.assertTrue(self.nodes[SSED_1].ping(leader_addr))
        self.simulator.go(5)

        # Step 4 - SED_1 requests a Forward Tracking Series by sending a Link Metrics Management Request
        # Forward Series Flags = 0x04:
        # - Bit 2 = 1, MAC Data Request
        # - Bits 0,1 & 3-7 = 0
        # Concatenation of Link Metric Type ID Flags = 0x00:
        # - Item1: (0)(1)(000)(000) = 0x40
        # -- E = 0
        # -- L = 1
        # -- Type/Average Enum = 0 (count)
        # -- Metrics Enum = 0 (count)
        self.nodes[SED_1].link_metrics_mgmt_req_forward_tracking_series(leader_addr, SERIES_ID, 'r', 'p')

        self.simulator.go(5)

        # Step 6 - Configures SED_1 to send MAC Data Request every 2 seconds and wait for 30 seconds
        self.simulator.go(30)

        # Step 7 - SED_1 sends an MLE Data Request to retrieve aggregated Forward Series Results
        self.nodes[SED_1].link_metrics_query_forward_tracking_series(leader_addr, SERIES_ID)

        self.simulator.go(5)

        # Step 9 - SED_1 clears the Forward Tracking Series
        # Forward Series Flags = 0x00:
        # - Bits 0-7 = 0
        # Concatenation of Link Metric Type ID Flags = NULL:
        self.nodes[SED_1].link_metrics_mgmt_req_forward_tracking_series(leader_addr, SERIES_ID, 'X', '')

        self.simulator.go(5)

        # Step 16 - SSED_1 requests a Forward Tracking Series by sending a Link Metrics Management Request
        # Forward Series Flags = 0x02:
        # - Bit 1 = 1, (all Link Layer Data Frames)
        # - Bits 0, 2-7 = 0
        # Concatenation of Link Metric Type ID Flags = 0x0a:
        # - Item1: (0)(0)(001)(010) = 0x0a
        # -- E = 0
        # -- L = 0
        # -- Type/Average Enum = 1 (Exponential Moving Average)
        # -- Metrics Enum = 2 (Link Margin)
        self.nodes[SSED_1].link_metrics_mgmt_req_forward_tracking_series(leader_addr, SERIES_ID_2, 'd', 'm')

        self.simulator.go(5)

        # Step 18 - SSED_1 sends MAC Data packet every 1 second for 30 seconds
        # Note: As the api to send MAC Data frame hasn't been implemented, we send MLE Link Probe message here
        for i in range(30):
            self.nodes[SSED_1].link_metrics_send_link_probe(leader_addr, SERIES_ID_2, 1)
            self.simulator.go(1)

        # Step 19 - SSED_1 sends an MLE Data Request to retrieve aggregated Forward Series Results
        self.nodes[SSED_1].link_metrics_query_forward_tracking_series(leader_addr, SERIES_ID_2)

        self.simulator.go(5)

        # Step 21 - SSED_1 clears the Forward Series Link Metrics
        # Forward Series Flags = 0x00:
        # - Bits 0-7 = 0
        # Concatenation of Link Metric Type ID Flags = NULL
        self.nodes[SSED_1].link_metrics_mgmt_req_forward_tracking_series(leader_addr, SERIES_ID_2, 'X', '')

        self.simulator.go(5)

        # Step 23 - SSED_1 sends an MLE Data Request to retrieve aggregated Forward Series Results
        self.nodes[SSED_1].link_metrics_query_forward_tracking_series(leader_addr, SERIES_ID_2)

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

        # Step 4 - SED_1 requests a Forward Series Link Metrics by sending a Link Metrics Management Request
        # Forward Series Flags = 0x04:
        # - Bit 2 = 1, MAC Data Request
        # - Bits 0,1 & 3-7 = 0
        # Concatenation of Link Metric Type ID Flags = 0x40:
        # - Item1: (0)(1)(000)(000) = 0x40
        # -- E = 0
        # -- L = 1
        # -- Type/Average Enum = 0 (count)
        # -- Metrics Enum = 0 (count)
        pkts.filter_wpan_src64(SED_1) \
            .filter_wpan_dst64(LEADER) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_REQUEST) \
            .filter(lambda p: p.mle.tlv.link_sub_tlv == LinkMetricsSubTlvType.FORWARD_PROBING_REGISTRATION) \
            .filter(lambda p: 4 in p.mle.tlv.link_forward_series)  \
            .filter(lambda p: consts.LINK_METRICS_TYPE_AVERAGE_ENUM_COUNT in p.mle.tlv.metric_type_id_flags.type) \
            .filter(lambda p: consts.LINK_METRICS_METRIC_TYPE_ENUM_PDU_COUNT in p.mle.tlv.metric_type_id_flags.metric) \
            .filter(lambda p: 1 in p.mle.tlv.metric_type_id_flags.l) \
            .must_next()

        # Step 5 - Leader responds with a Link Metrics Management Response
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(SED_1) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_SUCCESS) \
            .must_next()

        # Step 8 - Leader responds with an MLE Data Response
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(SED_1) \
            .filter_mle_cmd(consts.MLE_DATA_RESPONSE) \
            .filter(lambda p: TlvType.LINK_METRICS_REPORT in p.mle.tlv.type) \
            .filter(lambda p: consts.LINK_METRICS_TYPE_AVERAGE_ENUM_COUNT in p.mle.tlv.metric_type_id_flags.type) \
            .filter(lambda p: consts.LINK_METRICS_METRIC_TYPE_ENUM_PDU_COUNT in p.mle.tlv.metric_type_id_flags.metric) \
            .must_next()

        # Step 9 - SED_1 clears the Forward Series Link Metrics
        # Forward Series Flags = 0x00:
        # - Bits 0-7 = 0
        # Concatenation of Link Metric Type ID Flags = NULL:
        pkts.filter_wpan_src64(SED_1) \
            .filter_wpan_dst64(LEADER) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_REQUEST) \
            .filter(lambda p: p.mle.tlv.link_sub_tlv == LinkMetricsSubTlvType.FORWARD_PROBING_REGISTRATION) \
            .filter(lambda p: 0 in p.mle.tlv.link_forward_series) \
            .filter(lambda p: p.mle.tlv.metric_type_id_flags.type is nullField) \
            .filter(lambda p: p.mle.tlv.metric_type_id_flags.metric is nullField) \
            .must_next()

        # Step 10 - Leader responds with a Link Metrics Management Response
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(SED_1) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_SUCCESS) \
            .must_next()

        # Step 16 - SSED_1 requests a Forward Series Link Metrics by sending a Link Metrics Management Request
        # Forward Series Flags = 0x02:
        # - Bit 1 = 1, (all Link Layer Data Frames)
        # - Bits 0, 2-7 = 0
        # Concatenation of Link Metric Type ID Flags = 0x0a:
        # - Item1: (0)(0)(001)(010) = 0x0a
        # -- E = 0
        # -- L = 0
        # -- Type/Average Enum = 1 (Exponential Moving Average)
        # -- Metrics Enum = 2 (Link Margin)
        pkts.filter_wpan_src64(SSED_1) \
            .filter_wpan_dst64(LEADER) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_REQUEST) \
            .filter(lambda p: p.mle.tlv.link_sub_tlv == LinkMetricsSubTlvType.FORWARD_PROBING_REGISTRATION) \
            .filter(lambda p: 2 in p.mle.tlv.link_forward_series) \
            .filter(lambda p: consts.LINK_METRICS_TYPE_AVERAGE_ENUM_EXPONENTIAL in p.mle.tlv.metric_type_id_flags.type) \
            .filter(lambda p: consts.LINK_METRICS_METRIC_TYPE_ENUM_LINK_MARGIN in p.mle.tlv.metric_type_id_flags.metric) \
            .must_next()

        # Step 17 - Leader responds with a Link Metrics Management Response
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(SSED_1) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_SUCCESS) \
            .must_next()

        # Step 19 - SSED_1 sends an MLE Data Request to retrieve aggregated Forward Series results
        # - TLV Request TLV(Link Metrics Report TLV specified
        # - Link Metrics Query TLV
        # -- Link Metrics Query ID Sub-TLV
        # --- Query ID: (ID set in step 16)
        # - Verify that the Link Metrics Query Options Sub-TLV is NOT present
        pkts.filter_wpan_src64(SSED_1) \
            .filter_wpan_dst64(LEADER) \
            .filter_mle_cmd(consts.MLE_DATA_REQUEST) \
            .filter(lambda p: TlvType.LINK_METRICS_QUERY in p.mle.tlv.type) \
            .filter(lambda p: p.mle.tlv.query_id == SERIES_ID_2) \
            .must_next()

        # Step 20 - Leader MUST reply to SSED_1 with an MLE Data Response with the following:
        # - Link Metrics Report TLV
        # -- Link Metrics Report Sub-TLV
        # --- Metric Type ID Flags
        # ---- Type / Average Enum = 1 (Exponential Moving Average)
        # ---- Metric Enum = 2 (Link Margin)
        # --- Value (1 bytes)
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(SSED_1) \
            .filter_mle_cmd(consts.MLE_DATA_RESPONSE) \
            .filter_mle_has_tlv(TlvType.LINK_METRICS_REPORT) \
            .filter(lambda p: p.mle.tlv.link_sub_tlv == LinkMetricsSubTlvType.LINK_METRICS_REPORT) \
            .filter(lambda p: consts.LINK_METRICS_TYPE_AVERAGE_ENUM_EXPONENTIAL in p.mle.tlv.metric_type_id_flags.type) \
            .filter(lambda p: consts.LINK_METRICS_METRIC_TYPE_ENUM_LINK_MARGIN in p.mle.tlv.metric_type_id_flags.metric) \
            .must_next()

        # Step 21 - SSED_1 clears the Forward Series Link Metrics
        # Forward Series Flags = 0x00:
        # - Bits 0-7 = 0
        # Concatenation of Link Metric Type ID Flags = NULL
        pkts.filter_wpan_src64(SSED_1) \
            .filter_wpan_dst64(LEADER) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_REQUEST) \
            .filter(lambda p: p.mle.tlv.link_sub_tlv == LinkMetricsSubTlvType.FORWARD_PROBING_REGISTRATION) \
            .filter(lambda p: 0 in p.mle.tlv.link_forward_series) \
            .filter(lambda p: p.mle.tlv.metric_type_id_flags.type is nullField) \
            .filter(lambda p: p.mle.tlv.metric_type_id_flags.metric is nullField) \
            .must_next()

        # Step 22 - Leader responds with a Link Metrics Management Response
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(SSED_1) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_SUCCESS) \
            .must_next()

        # Step 23 - SSED_1 sends an MLE Data Request to retrieve aggregated Forward Series results
        # - TLV Request TLV(Link Metrics Report TLV specified
        # - Link Metrics Query TLV
        # -- Link Metrics Query ID Sub-TLV
        # --- Query ID: (ID set in step 16)
        # This is a negative step
        pkts.filter_wpan_src64(SSED_1) \
            .filter_wpan_dst64(LEADER) \
            .filter_mle_cmd(consts.MLE_DATA_REQUEST) \
            .filter(lambda p: TlvType.LINK_METRICS_QUERY in p.mle.tlv.type) \
            .filter(lambda p: p.mle.tlv.query_id == SERIES_ID_2) \
            .must_next()

        # Step 24 - Leader MUST reply to SSED_1 with an MLE Data Response with the following:
        # - Link Metrics Report TLV
        # -- Link Metrics Status Sub-TLV
        # --- Status = 3 (Series ID not recognized)
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(SSED_1) \
            .filter_mle_cmd(consts.MLE_DATA_RESPONSE) \
            .filter_mle_has_tlv(TlvType.LINK_METRICS_REPORT) \
            .filter(lambda p: p.mle.tlv.link_sub_tlv == LinkMetricsSubTlvType.LINK_METRICS_STATUS) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_SERIES_ID_NOT_RECOGNIZED) \
            .must_next()


if __name__ == '__main__':
    unittest.main()
