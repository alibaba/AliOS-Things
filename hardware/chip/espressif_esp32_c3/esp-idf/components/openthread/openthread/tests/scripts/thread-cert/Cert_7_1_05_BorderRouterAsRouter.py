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

import config
import thread_cert
from pktverify.consts import WIRESHARK_OVERRIDE_PREFS, MLE_CHILD_UPDATE_REQUEST, MLE_CHILD_UPDATE_RESPONSE, MLE_DATA_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, SVR_DATA_URI, ACTIVE_TIMESTAMP_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MODE_TLV, TIMEOUT_TLV, VERSION_TLV, TLV_REQUEST_TLV, ADDRESS16_TLV, NETWORK_DATA_TLV, ROUTE64_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ADDRESS_REGISTRATION_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.addrs import Ipv6Addr

LEADER = 1
ROUTER = 2
MED = 3
SED = 4

MTDS = [MED, SED]
PREFIX_2001 = '2001::/64'
PREFIX_2002 = '2002::/64'
PREFIX_2003 = '2003::/64'

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify that the DUT sends properly formatted
# Server Data Notification CoAP frame when a third global prefix information is
# set on the DUT. The DUT must also correctly set Network Data aggregated and
# disseminated by the Leader and transmit it properly to all child devices already
# attached to it.
#
# Test Topology:
# -------------
#             SED
#              |
#           Router(DUT) - MED
#              |
#           Leader
#
# DUT Types:
# ----------
# Router


class Cert_7_1_5_BorderRouterAsRouter(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER, MED, SED]
        },
        MED: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [ROUTER]
        },
        SED: {
            'name': 'SED',
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[MED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MED].get_state(), 'child')

        self.nodes[SED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED].get_state(), 'child')

        self.nodes[ROUTER].add_prefix(PREFIX_2001, 'paros')
        self.nodes[ROUTER].add_prefix(PREFIX_2002, 'paro')
        self.nodes[ROUTER].add_prefix(PREFIX_2003, 'paos')
        self.nodes[ROUTER].register_netdata()
        self.simulator.go(10)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']
        SED = pv.vars['SED']
        MED = pv.vars['MED']

        # Step 1: Ensure topology is formed correctly
        pv.verify_attached('ROUTER', 'LEADER')
        pv.verify_attached('MED', 'ROUTER', 'MTD')
        pv.verify_attached('SED', 'ROUTER', 'MTD')

        # Step 3: The DUT MUST send a CoAP Server Data Notification frame
        #         to the Leader including the server’s information(Prefix,
        #         Border Router) for all three prefixes (Prefix 1, 2 and 3):
        #             CoAP Request URI
        #                 coap://[<Leader address>]:MM/a/sd
        #             CoAP Payload
        #                 Thread Network Data TLV

        pkts.filter_wpan_src64(ROUTER).\
            filter_coap_request(SVR_DATA_URI).\
            filter(lambda p: {
                              Ipv6Addr(PREFIX_2001[:-3]),
                              Ipv6Addr(PREFIX_2002[:-3]),
                              Ipv6Addr(PREFIX_2003[:-3])
                             } == set(p.thread_nwd.tlv.prefix)
                   ).\
            must_next()
        _index = pkts.index

        # Step 5: The DUT MUST send a multicast MLE Data Response,
        #         including at least three Prefix TLVs (Prefix 1, Prefix2,
        #         and Prefix 3).
        with pkts.save_index():
            _dv_pkt = pkts.filter_wpan_src64(ROUTER).\
                filter_LLANMA().\
                filter_mle_cmd(MLE_DATA_RESPONSE).\
                filter(lambda p: {
                                  Ipv6Addr(PREFIX_2001[:-3]),
                                  Ipv6Addr(PREFIX_2002[:-3]),
                                  Ipv6Addr(PREFIX_2003[:-3])
                                 } <= set(p.thread_nwd.tlv.prefix)
                       ).\
                must_next()

        # Step 6: MED automatically sends MLE Child Update Request to its parent
        #         (DUT), reporting its configured global addresses in the Address
        #         Registration TLV

        # Step 7: The DUT MUST send a MLE Child Update Response to MED
        #          The following TLVs MUST be present in the Child Update Response:
        #              - Source Address TLV
        #              - Address Registration TLV
        #                  - Echoes back addresses configured in step 4
        #              - Mode TLV
        _pkt = pkts.filter_wpan_src64(MED).\
            filter_wpan_dst64(ROUTER).\
            filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).\
            filter(lambda p: len(p.mle.tlv.addr_reg_iid) >= 4).\
            must_next()
        pkts.filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(MED).\
            filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).\
            filter(lambda p: {
                              SOURCE_ADDRESS_TLV,
                              MODE_TLV,
                              ADDRESS_REGISTRATION_TLV
                             } < set(p.mle.tlv.type) and\
                   len(p.mle.tlv.addr_reg_iid) >= 3 and\
                   set(p.mle.tlv.addr_reg_iid) < set(_pkt.mle.tlv.addr_reg_iid)
                   ).\
            must_next()

        # Step 8: The DUT MUST send a MLE Child Update Request or MLE Data
        #         Response to SED, including the following TLVs:
        #             - Network Data TLV
        #                 At least two Prefix TLVs (Prefix 1 and Prefix3)
        #                     - Border Router TLV
        #                         - P_border_router_16<0xFFFE>
        #                 Prefix 2 TLV MUST NOT be included
        #             - Source Address TLV
        #             - Leader Data TLV
        #                 Data version numbers should be the same as the ones
        #                 sent in the multicast data response in step 5.
        #             - Active Timestamp TLV
        pkts.range(_index).filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(SED).\
            filter_mle_cmd2(MLE_CHILD_UPDATE_REQUEST, MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV,
                              ACTIVE_TIMESTAMP_TLV
                             } == set(p.mle.tlv.type) and\
                             {
                              Ipv6Addr(PREFIX_2001[:-3]),
                              Ipv6Addr(PREFIX_2003[:-3])
                             } == set(p.thread_nwd.tlv.prefix) and\
                   p.mle.tlv.leader_data.data_version  ==
                   _dv_pkt.mle.tlv.leader_data.data_version and\
                   [0xFFFE, 0xFFFE] == p.thread_nwd.tlv.border_router_16
                   ).\
            must_next()

        # Step 9: SED automatically sends its global address configured to the Leader,
        #         in the Address Registration TLV from the Child Update request command

        # Step 10: The DUT MUST send a MLE Child Update Response, each, to SED
        #          The following TLVs MUST be present in the Child Update Response:
        #              - Source Address TLV
        #              - Address Registration TLV
        #                  - Echoes back addresses configured in step 9
        #              - Mode TLV
        _pkt = pkts.filter_wpan_src64(SED).\
            filter_wpan_dst64(ROUTER).\
            filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).\
            filter(lambda p: len(p.mle.tlv.addr_reg_iid) >= 3).\
            must_next()
        pkts.filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(SED).\
            filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).\
            filter(lambda p: {
                              SOURCE_ADDRESS_TLV,
                              MODE_TLV,
                              ADDRESS_REGISTRATION_TLV
                             } < set(p.mle.tlv.type) and\
                   len(p.mle.tlv.addr_reg_iid) >= 2 and\
                   set(p.mle.tlv.addr_reg_iid) < set(_pkt.mle.tlv.addr_reg_iid)
                   ).\
            must_next()


if __name__ == '__main__':
    unittest.main()
