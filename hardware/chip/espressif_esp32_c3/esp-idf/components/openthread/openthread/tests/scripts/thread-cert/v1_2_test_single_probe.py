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

import mle
import thread_cert

LEADER = 1
SSED_1 = 2

CSL_PERIOD = 500 * 6.25  # 500ms
CSL_TIMEOUT = 30  # 30s


class SSED_SingleProbe(thread_cert.TestCase):
    TOPOLOGY = {
        LEADER: {
            'version': '1.2',
        },
        SSED_1: {
            'version': '1.2',
            'mode': '-',
        },
    }
    """All nodes are created with default configurations"""

    def test(self):

        self.nodes[SSED_1].set_csl_period(CSL_PERIOD)
        self.nodes[SSED_1].set_csl_timeout(CSL_TIMEOUT)

        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[SSED_1].start()
        self.simulator.go(7)
        self.assertEqual(self.nodes[SSED_1].get_state(), 'child')

        leader_addr = self.nodes[LEADER].get_linklocal()

        leader_messages = self.simulator.get_messages_sent_by(LEADER)

        # SSED_1 sends a Single Probe Link Metrics for L2 PDU count using MLE Data Request
        self.nodes[SSED_1].link_metrics_query_single_probe(leader_addr, 'p')
        self.simulator.go(5)

        leader_messages = self.simulator.get_messages_sent_by(LEADER)
        msg = leader_messages.next_mle_message(mle.CommandType.DATA_RESPONSE)
        msg.assertMleMessageContainsTlv(mle.LinkMetricsReport)

        # SSED_1 sends a Single Probe Link Metrics for L2 LQI using MLE Data Request
        self.nodes[SSED_1].link_metrics_query_single_probe(leader_addr, 'q')
        self.simulator.go(5)

        leader_messages = self.simulator.get_messages_sent_by(LEADER)
        msg = leader_messages.next_mle_message(mle.CommandType.DATA_RESPONSE)
        msg.assertMleMessageContainsTlv(mle.LinkMetricsReport)

        # SSED_1 sends a Single Probe Link Metrics for Link Margin using MLE Data Request
        self.nodes[SSED_1].link_metrics_query_single_probe(leader_addr, 'm')
        self.simulator.go(5)

        leader_messages = self.simulator.get_messages_sent_by(LEADER)
        msg = leader_messages.next_mle_message(mle.CommandType.DATA_RESPONSE)
        msg.assertMleMessageContainsTlv(mle.LinkMetricsReport)

        # SSED_1 sends a Single Probe Link Metrics for Link Margin using MLE Data Request
        self.nodes[SSED_1].link_metrics_query_single_probe(leader_addr, 'r')
        self.simulator.go(5)

        leader_messages = self.simulator.get_messages_sent_by(LEADER)
        msg = leader_messages.next_mle_message(mle.CommandType.DATA_RESPONSE)
        msg.assertMleMessageContainsTlv(mle.LinkMetricsReport)

        # SSED_1 sends a Single Probe Link Metrics for all metrics using MLE Data Request
        self.nodes[SSED_1].link_metrics_query_single_probe(leader_addr, 'pqmr')
        self.simulator.go(5)

        leader_messages = self.simulator.get_messages_sent_by(LEADER)
        msg = leader_messages.next_mle_message(mle.CommandType.DATA_RESPONSE)
        msg.assertMleMessageContainsTlv(mle.LinkMetricsReport)

        self.assertTrue(self.nodes[LEADER].ping(self.nodes[SSED_1].get_rloc()))


if __name__ == '__main__':
    unittest.main()
