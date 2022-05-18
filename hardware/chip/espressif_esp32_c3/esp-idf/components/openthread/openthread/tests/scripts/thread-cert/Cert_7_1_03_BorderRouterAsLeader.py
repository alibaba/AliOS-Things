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
import copy

import config
import thread_cert
from pktverify.consts import WIRESHARK_OVERRIDE_PREFS, MLE_CHILD_UPDATE_REQUEST, MLE_CHILD_UPDATE_RESPONSE, MLE_DATA_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, ACTIVE_TIMESTAMP_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MODE_TLV, TIMEOUT_TLV, VERSION_TLV, TLV_REQUEST_TLV, ADDRESS16_TLV, NETWORK_DATA_TLV, ROUTE64_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ADDRESS_REGISTRATION_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.addrs import Ipv6Addr
from pktverify.null_field import nullField

LEADER = 1
ROUTER = 2
SED1 = 3
MED1 = 4

MTDS = [SED1, MED1]
PREFIX_2001 = '2001::/64'
PREFIX_2002 = '2002::/64'

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify that global prefix information can
# be set on the DUT, which is acting as a Leader in the Thread network. The DUT
# must also demonstrate that it correctly sets the Network Data (stable/non-stable)
# and propagates it properly in an already formed network.
#
# Test Topology:
# -------------
#             SED
#              |
#  ROUTER - Leader(DUT) - MED
#
# DUT Types:
# ----------
#  Leader


class Cert_7_1_3_BorderRouterAsLeader(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER, SED1, MED1]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        SED1: {
            'name': 'SED',
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [LEADER]
        },
        MED1: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [LEADER]
        },
    }
    # override wireshark preferences with case needed parameters
    CASE_WIRESHARK_PREFS = copy.deepcopy(WIRESHARK_OVERRIDE_PREFS)
    CASE_WIRESHARK_PREFS['6lowpan.context1'] = PREFIX_2001
    CASE_WIRESHARK_PREFS['6lowpan.context2'] = PREFIX_2002

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[SED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED1].get_state(), 'child')

        self.nodes[MED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MED1].get_state(), 'child')

        self.nodes[LEADER].add_prefix(PREFIX_2001, 'paros')
        self.nodes[LEADER].add_prefix(PREFIX_2002, 'paro')
        self.nodes[LEADER].register_netdata()
        self.simulator.go(5)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']
        SED = pv.vars['SED']
        MED = pv.vars['MED']

        # Step 1: Ensure topology is formed correctly
        pv.verify_attached('ROUTER', 'LEADER')
        pv.verify_attached('SED', 'LEADER', 'MTD')
        pv.verify_attached('MED', 'LEADER', 'MTD')

        # Step 3: The DUT MUST send a multicast MLE Data Response,
        #         including the following TLVs:
        #             - Network Data TLV
        #               At least two Prefix TLVs (Prefix 1 and Prefix 2),
        #               each including:
        #                   - 6LoWPAN ID sub-TLV
        #                   - Border Router sub-TLV
        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              Ipv6Addr(PREFIX_2001[:-3]),
                              Ipv6Addr(PREFIX_2002[:-3])
                             } == set(p.thread_nwd.tlv.prefix) and\
                   p.thread_nwd.tlv.border_router.flag.p == [1, 1] and\
                   p.thread_nwd.tlv.border_router.flag.s == [1, 1] and\
                   p.thread_nwd.tlv.border_router.flag.r == [1, 1] and\
                   p.thread_nwd.tlv.border_router.flag.o == [1, 1] and\
                   p.thread_nwd.tlv.stable == [0, 1, 1, 1, 0, 0, 0]
                   ).\
            must_next()

        # Step 4: MED automatically sends the global address configured to its parent
        #         (the DUT), via the Address Registration TLV included in its Child
        #         Update Request keep-alive message.

        # Step 5: The DUT MUST send a MLE Child Update Response to MED
        #          The following TLVs MUST be present in the Child Update Response:
        #              - Source Address TLV
        #              - Address Registration TLV
        #                  - Echoes back addresses configured in step 4
        #              - Mode TLV
        with pkts.save_index():
            _pkt = pkts.filter_wpan_src64(MED).\
                filter_wpan_dst64(LEADER).\
                filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).\
                must_next()
            pkts.filter_wpan_src64(LEADER).\
                filter_wpan_dst64(MED).\
                filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).\
                filter(lambda p: {
                                  SOURCE_ADDRESS_TLV,
                                  MODE_TLV,
                                  ADDRESS_REGISTRATION_TLV
                                 } < set(p.mle.tlv.type) and\
                       p.mle.tlv.addr_reg_iid is not nullField and\
                       set(_pkt.mle.tlv.addr_reg_iid) > set(p.mle.tlv.addr_reg_iid)
                       ).\
                must_next()

        # Step 6: The DUT MUST send a MLE Child Update Request or MLE Data
        #         Response to SED, including the following TLVs:
        #             - Network Data TLV
        #             - Source Address TLV
        #             - Leader Data TLV
        #             - Active Timestamp TLV
        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(SED).\
            filter_mle_cmd2(MLE_CHILD_UPDATE_REQUEST, MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV,
                              ACTIVE_TIMESTAMP_TLV
                             } == set(p.mle.tlv.type) and\
                   [Ipv6Addr(PREFIX_2001[:-3])] == p.thread_nwd.tlv.prefix and\
                   p.thread_nwd.tlv.border_router.flag.p == [1] and\
                   p.thread_nwd.tlv.border_router.flag.s == [1] and\
                   p.thread_nwd.tlv.border_router.flag.r == [1] and\
                   p.thread_nwd.tlv.border_router.flag.o == [1] and\
                   p.thread_nwd.tlv.stable == [1, 1, 1]
                   ).\
            must_next()

        # Step 9: After receiving the MLE Data Response or MLE Child Update Request,
        #         SED automatically sends its global address configured to the Leader,
        #         in the Address Registration TLV from the Child Update request command

        # Step 10: The DUT MUST send a MLE Child Update Response, each, to SED
        #          The following TLVs MUST be present in the Child Update Response:
        #              - Source Address TLV
        #              - Address Registration TLV
        #                  - Echoes back addresses configured in step 9
        #              - Mode TLV
        _pkt = pkts.filter_wpan_src64(SED).\
            filter_wpan_dst64(LEADER).\
            filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).\
            must_next()
        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(SED).\
            filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).\
            filter(lambda p: {
                              SOURCE_ADDRESS_TLV,
                              MODE_TLV,
                              ADDRESS_REGISTRATION_TLV
                             } < set(p.mle.tlv.type) and\
                   p.mle.tlv.addr_reg_iid is not nullField and\
                   set(_pkt.mle.tlv.addr_reg_iid) > set(p.mle.tlv.addr_reg_iid)
                   ).\
            must_next()


if __name__ == '__main__':
    unittest.main()
