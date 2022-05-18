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
CHILD = 2

SERIES_ID_1 = 1
SERIES_ID_2 = 2


class LowPower_test_ForwardTrackingSeries(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'version': '1.2',
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [CHILD],
        },
        CHILD: {
            'version': '1.2',
            'name': 'CHILD',
            'mode': 'rn',
            'allowlist': [LEADER],
        }
    }
    """All nodes are created with default configurations"""

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[CHILD].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[CHILD].get_state(), 'child')

        leader_addr = self.nodes[LEADER].get_ip6_address(ADDRESS_TYPE.LINK_LOCAL)

        # 1. Child configures a Forward Tracking Series successfully.
        # The Series tracks the count of MAC Data Request.
        # Child should get a response with status 0 (SUCCESS).
        self.nodes[CHILD].link_metrics_mgmt_req_forward_tracking_series(leader_addr, SERIES_ID_1, 'r', 'p')
        self.simulator.go(1)

        # 2. Child configures the same Forward Tracking Series again.
        # Child should get a response with status 2 (SERIES_ID_ALREADY_REGISTERED).
        self.nodes[CHILD].link_metrics_mgmt_req_forward_tracking_series(leader_addr, SERIES_ID_1, 'r', 'p')
        self.simulator.go(1)

        # 3. Child queries a Series that doesn't exist (using a wrong Series ID).
        # Child should get a report with status 3 (SERIES_ID_NOT_RECOGNIZED).
        self.nodes[CHILD].link_metrics_query_forward_tracking_series(leader_addr, SERIES_ID_2)
        self.simulator.go(1)

        # 4. Child queries a Series that don't have matched frames yet.
        # Child should get a report with status 4 (NO_MATCHING_FRAMES_RECEIVED).
        self.nodes[CHILD].link_metrics_query_forward_tracking_series(leader_addr, SERIES_ID_1)
        self.simulator.go(1)

        # 5. Child clears a Forward Tracking Series that doesn't exist.
        # Child should get a response with status 3 (SERIES_ID_NOT_RECOGNIZED).
        self.nodes[CHILD].link_metrics_mgmt_req_forward_tracking_series(leader_addr, SERIES_ID_2, 'X', '')
        self.simulator.go(1)

        # 6. Child clears a Forward Tracking Series successfully.
        # Child should get a response with status 0 (SUCCESS).
        self.nodes[CHILD].link_metrics_mgmt_req_forward_tracking_series(leader_addr, SERIES_ID_1, 'X', '')
        self.simulator.go(1)

        # 7. Child configures a new Forward Tracking Series successfully.
        # The Series tracks the count of all MAC Data frames.
        # Child should get a response with status 0 (SUCCESS).
        self.nodes[CHILD].link_metrics_mgmt_req_forward_tracking_series(leader_addr, SERIES_ID_2, 'd', 'pqmr')
        self.simulator.go(1)

        # 8. Child sends an MLE Link Probe message to the Subject for the newly configured Series.
        self.nodes[CHILD].link_metrics_send_link_probe(leader_addr, SERIES_ID_2, 1)

        # 9. Child queries the newly configured Series successfully.
        # Child should get a report with valid values.
        self.nodes[CHILD].link_metrics_query_forward_tracking_series(leader_addr, SERIES_ID_2)
        self.simulator.go(1)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()
        LEADER = pv.vars['LEADER']
        CHILD = pv.vars['CHILD']

        # 1. Child should get a response with status 0 (SUCCESS).
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(CHILD) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_SUCCESS) \
            .must_next()

        # 2. Child should get a response with status 2 (SERIES_ID_ALREADY_REGISTERED).
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(CHILD) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_SERIES_ID_ALREADY_REGISTERED) \
            .must_next()

        # 3. Child should get a report with status 3 (SERIES_ID_NOT_RECOGNIZED).
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(CHILD) \
            .filter_mle_cmd(consts.MLE_DATA_RESPONSE) \
            .filter_mle_has_tlv(TlvType.LINK_METRICS_REPORT) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_SERIES_ID_NOT_RECOGNIZED) \
            .must_next()

        # 4. Child should get a report with status 4 (NO_MATCHING_FRAMES_RECEIVED).
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(CHILD) \
            .filter_mle_cmd(consts.MLE_DATA_RESPONSE) \
            .filter_mle_has_tlv(TlvType.LINK_METRICS_REPORT) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_NO_MATCHING_FRAMES_RECEIVED) \
            .must_next()

        # 5. Child should get a response with status 3 (SERIES_ID_NOT_RECOGNIZED).
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(CHILD) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_SERIES_ID_NOT_RECOGNIZED) \
            .must_next()

        # 6. Child should get a response with status 0 (SUCCESS).
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(CHILD) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_SUCCESS) \
            .must_next()

        # 7. Child should get a response with status 0 (SUCCESS).
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(CHILD) \
            .filter_mle_cmd(consts.MLE_LINK_METRICS_MANAGEMENT_RESPONSE) \
            .filter(lambda p: p.mle.tlv.link_status_sub_tlv == consts.LINK_METRICS_STATUS_SUCCESS) \
            .must_next()

        # 8. Child sends an MLE Link Probe message to the Subject for the newly configured Series.
        pkts.filter_wpan_src64(CHILD) \
            .filter_wpan_dst64(LEADER) \
            .filter_mle_cmd(consts.MLE_LINK_PROBE) \
            .must_next()

        # 9. Child should get a report with valid values.
        pkts.filter_wpan_src64(LEADER) \
            .filter_wpan_dst64(CHILD) \
            .filter_mle_cmd(consts.MLE_DATA_RESPONSE) \
            .filter(lambda p: TlvType.LINK_METRICS_REPORT in p.mle.tlv.type) \
            .must_next()


if __name__ == '__main__':
    unittest.main()
