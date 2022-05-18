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
from pktverify.consts import WIRESHARK_OVERRIDE_PREFS, MLE_CHILD_UPDATE_REQUEST, MLE_CHILD_UPDATE_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MODE_TLV, TIMEOUT_TLV, VERSION_TLV, TLV_REQUEST_TLV, ADDRESS16_TLV, NETWORK_DATA_TLV, ROUTE64_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ADDRESS_REGISTRATION_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.addrs import Ipv6Addr

LEADER = 1
ROUTER = 2
SED1 = 3
MED1 = 4

MTDS = [SED1, MED1]
PREFIX_2001 = '2001::/64'
PREFIX_2002 = '2002::/64'

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify that the DUT, as a Border Router,
# acts properly as a Leader device in a Thread network, correctly sets the
# Network Data (stable/non-stable) and successfully propagates the Network Data
# to the devices that attach to it.
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


class Cert_7_1_1_BorderRouterAsLeader(thread_cert.TestCase):
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

        self.nodes[LEADER].add_prefix(PREFIX_2001, 'paros')
        self.nodes[LEADER].add_prefix(PREFIX_2002, 'paro')
        self.nodes[LEADER].register_netdata()

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[SED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED1].get_state(), 'child')

        self.nodes[MED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MED1].get_state(), 'child')

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']
        MED = pv.vars['MED']
        SED = pv.vars['SED']

        # Step 1: The DUT forms the network properly sends MLE Advertisements
        pkts.filter_wpan_src64(LEADER).\
            filter_mle_advertisement('Leader').\
            must_next()

        # Step 3: Router attaches to the Leader (DUT) and requests complete
        #         network data
        pkts.filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(LEADER).\
            filter_mle_cmd(MLE_CHILD_ID_REQUEST).\
            filter(lambda p: {
                              RESPONSE_TLV,
                              LINK_LAYER_FRAME_COUNTER_TLV,
                              MODE_TLV,
                              TIMEOUT_TLV,
                              VERSION_TLV,
                              TLV_REQUEST_TLV,
                              ADDRESS16_TLV,
                              NETWORK_DATA_TLV,
                              ROUTE64_TLV
                             } < set(p.mle.tlv.type) and\
                   p.mle.tlv.mode.network_data == 1
                   ).\
            must_next()

        # Step 4: The DUT MUST send a MLE Child ID Response to Router,
        #         including the following TLVs:
        #             - Network Data TLV
        #               At least two Prefix TLVs (Prefix 1 and Prefix 2),
        #               each including:
        #                   - 6LoWPAN ID sub-TLV
        #                   - Border Router sub-TLV
        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
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

        # Step 5: SED attaches to the Leader (DUT) and requests only stable
        #         network data
        pkts.filter_wpan_src64(SED).\
            filter_wpan_dst64(LEADER).\
            filter_mle_cmd(MLE_CHILD_ID_REQUEST).\
            filter(lambda p: {
                              RESPONSE_TLV,
                              LINK_LAYER_FRAME_COUNTER_TLV,
                              MODE_TLV,
                              TIMEOUT_TLV,
                              VERSION_TLV,
                              TLV_REQUEST_TLV,
                              ADDRESS16_TLV,
                              NETWORK_DATA_TLV,
                              ADDRESS_REGISTRATION_TLV
                             } <= set(p.mle.tlv.type) and\
                   p.mle.tlv.mode.network_data == 0
                   ).\
            must_next()

        # Step 6: The DUT MUST send a MLE Child ID Response to SED,
        #         including the following TLVs:
        #             - Network Data TLV
        #               At least one Prefix TLVs (Prefix 1),including:
        #                   - 6LoWPAN ID sub-TLV
        #                   - Border Router sub-TLV
        #                       - P_border_router_16 <0xFFFE>
        #               Prefix 2 TLV MUST NOT be included
        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(SED).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            filter(lambda p: {
                              MODE_TLV,
                              TIMEOUT_TLV,
                              CHALLENGE_TLV
                             } == set(p.thread_nwd.tlv.type) and\
                   [Ipv6Addr(PREFIX_2001[:-3])] == p.thread_nwd.tlv.prefix and\
                   p.thread_nwd.tlv.border_router.flag.p == [1] and\
                   p.thread_nwd.tlv.border_router.flag.s == [1] and\
                   p.thread_nwd.tlv.border_router.flag.r == [1] and\
                   p.thread_nwd.tlv.border_router.flag.o == [1] and\
                   p.thread_nwd.tlv.stable == [1, 1, 1]
                   ).\
            must_next()

        lstart = pkts.index

        # Step 7: MED attaches to the Leader (DUT) and requests complete
        #         network data
        pkts.filter_wpan_src64(MED).\
            filter_wpan_dst64(LEADER).\
            filter_mle_cmd(MLE_CHILD_ID_REQUEST).\
            filter(lambda p: {
                              RESPONSE_TLV,
                              LINK_LAYER_FRAME_COUNTER_TLV,
                              MODE_TLV,
                              TIMEOUT_TLV,
                              VERSION_TLV,
                              TLV_REQUEST_TLV,
                              ADDRESS16_TLV,
                              NETWORK_DATA_TLV,
                              ADDRESS_REGISTRATION_TLV
                             } < set(p.mle.tlv.type) and\
                   p.mle.tlv.mode.network_data == 1
                   ).\
            must_next()

        # Step 8: The DUT MUST send a MLE Child ID Response to MED,
        #         including the following TLVs:
        #             - Network Data TLV
        #               At least two Prefix TLVs (Prefix 1 and Prefix 2),
        #               each including:
        #                   - 6LoWPAN ID sub-TLV
        #                   - Border Router sub-TLV
        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(MED).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
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

        # Step 9: After attaching, each Child automatically sends its global address
        #         configured to the Leader, in the Address Registration TLV from the
        #         Child Update request command

        # Step 10: The DUT MUST send a MLE Child Update Response, each, to MED & SED
        #          The following TLVs MUST be present in the Child Update Response:
        #              - Source Address TLV
        #              - Address Registration TLV
        #                  - Echoes back addresses configured in step 9
        #              - Mode TLV
        for child in (SED, MED):
            _pkt = pkts.range(lstart).\
                filter_wpan_src64(child).\
                filter_wpan_dst64(LEADER).\
                filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).\
                must_next()
            pkts.range(lstart).\
                filter_wpan_src64(LEADER).\
                filter_wpan_dst64(child).\
                filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).\
                filter(lambda p: {
                                  SOURCE_ADDRESS_TLV,
                                  MODE_TLV,
                                  ADDRESS_REGISTRATION_TLV
                                 } < set(p.mle.tlv.type) and\
                       set(p.mle.tlv.addr_reg_iid) < set(_pkt.mle.tlv.addr_reg_iid)
                       ).\
                must_next()


if __name__ == '__main__':
    unittest.main()
