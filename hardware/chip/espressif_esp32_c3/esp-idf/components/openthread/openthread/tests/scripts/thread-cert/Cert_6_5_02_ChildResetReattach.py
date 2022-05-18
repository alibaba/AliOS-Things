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

import thread_cert
import config
from pktverify.consts import WPAN_DATA_REQUEST, MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_UPDATE_REQUEST, MLE_CHILD_UPDATE_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, ADDR_SOL_URI, SOURCE_ADDRESS_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MLE_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV, NL_MAC_EXTENDED_ADDRESS_TLV, NL_RLOC16_TLV, NL_STATUS_TLV, NL_ROUTER_MASK_TLV, COAP_CODE_ACK
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

LEADER = 1
ROUTER = 2
MTD = 3
POLL_PERIOD = 3  # seconds

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate that after the DUT resets and receives
# no response from its parent, it will reattach to the network through a different parent.
#
# Test Topology:
# -------------
#  Leader
#    |
#  Router
#    |
#   DUT
#
# DUT Types:
# ----------
#  ED
#  SED


class Cert_6_5_2_ChildResetReattach_Base(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER, MTD]
        },
        MTD: {
            'name': 'DUT',
            'is_mtd': True,
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
    }

    def _setUpMTD(self):
        self.nodes[MTD].add_allowlist(self.nodes[LEADER].get_addr64())
        self.nodes[MTD].enable_allowlist()
        if self.TOPOLOGY[MTD]['mode'] == '-':
            self.nodes[MTD].set_pollperiod(POLL_PERIOD * 1000)

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[MTD].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MTD].get_state(), 'child')
        self.collect_rloc16s()

        self.nodes[LEADER].clear_allowlist()
        self.nodes[MTD].clear_allowlist()
        self.nodes[ROUTER].stop()

        self.nodes[MTD].reset()
        self._setUpMTD()
        self.simulator.go(5)
        self.nodes[LEADER].add_allowlist(self.nodes[MTD].get_addr64())
        self.simulator.go(5)
        self.nodes[MTD].start()
        self.simulator.go(10)
        self.assertEqual(self.nodes[MTD].get_state(), 'child')
        self.simulator.go(360)

        self.collect_ipaddrs()
        dut_addr = self.nodes[MTD].get_ip6_address(config.ADDRESS_TYPE.LINK_LOCAL)
        self.nodes[LEADER].ping(dut_addr)
        self.simulator.go(15)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_LLA = pv.vars['LEADER_LLA']
        ROUTER = pv.vars['ROUTER']
        ROUTER_RLOC16 = pv.vars['ROUTER_RLOC16']
        DUT = pv.vars['DUT']
        DUT_LLA = pv.vars['DUT_LLA']

        # Step 1: Ensure topology is formed correctly

        pv.verify_attached('ROUTER', 'LEADER')
        pv.verify_attached('DUT', 'ROUTER', 'MTD')

        # Step 4: DUT sends an MLE Child Update Request.
        #         The following TLVs MUST be included in the Child Update
        #         Request:
        #             - Mode TLV
        #             - Address Registration TLV (optional)
        #         If the DUT is a SED, it MUST resume polling after sending
        #         MLE Child Update.
        pkts.filter_wpan_src64(DUT).\
            filter_wpan_dst64(ROUTER).\
            filter_mle_cmd(MLE_CHILD_UPDATE_REQUEST).\
            filter(lambda p: {
                              MODE_TLV
                             } < set(p.mle.tlv.type)
                   ).\
            must_next()
        if self.TOPOLOGY[MTD]['mode'] == '-':
            _pkt = pkts.filter_wpan_src64(DUT).\
                filter_wpan_dst16(ROUTER_RLOC16).\
                filter_wpan_cmd(WPAN_DATA_REQUEST).\
                must_next()
            pkts.filter_wpan_src64(DUT).\
                filter_wpan_dst16(ROUTER_RLOC16).\
                filter_wpan_cmd(WPAN_DATA_REQUEST).\
                filter(lambda p:
                       p.sniff_timestamp - _pkt.sniff_timestamp <=
                       POLL_PERIOD + 0.1
                      ).\
                must_next()
        lstart = pkts.index

        # Step 6: The DUT MUST attach to the Leader
        pkts.filter_wpan_src64(DUT).\
            filter_LLARMA().\
            filter_mle_cmd(MLE_PARENT_REQUEST).\
            filter(lambda p: {
                              CHALLENGE_TLV,
                              MODE_TLV,
                              SCAN_MASK_TLV,
                              VERSION_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.ipv6.hlim == 255 and\
                   p.mle.tlv.scan_mask.r == 1 and\
                   p.mle.tlv.scan_mask.e == 0 and\
                   p.wpan.aux_sec.key_id_mode == 0x2
                   ).\
            must_next()
        lend = pkts.index

        # Step 5: No response from Router.
        pkts.range(lstart, lend).\
            filter_wpan_dst64(ROUTER).\
            filter_mle_cmd(MLE_CHILD_UPDATE_RESPONSE).\
            must_not_next()

        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(DUT).\
            filter_mle_cmd(MLE_PARENT_RESPONSE).\
            filter(lambda p: {
                              CHALLENGE_TLV,
                              CONNECTIVITY_TLV,
                              LEADER_DATA_TLV,
                              LINK_LAYER_FRAME_COUNTER_TLV,
                              LINK_MARGIN_TLV,
                              RESPONSE_TLV,
                              SOURCE_ADDRESS_TLV,
                              VERSION_TLV
                               } <= set(p.mle.tlv.type)).\
                   must_next()

        pkts.filter_wpan_src64(DUT).\
            filter_wpan_dst64(LEADER).\
            filter_mle_cmd(MLE_CHILD_ID_REQUEST).\
            filter(lambda p: {
                              ADDRESS_REGISTRATION_TLV,
                              LINK_LAYER_FRAME_COUNTER_TLV,
                              MODE_TLV,
                              RESPONSE_TLV,
                              TIMEOUT_TLV,
                              TLV_REQUEST_TLV,
                              VERSION_TLV
                             } <= set(p.mle.tlv.type) and\
                   p.wpan.aux_sec.key_id_mode == 0x2
                   ).\
             must_next()

        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(DUT).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            filter(lambda p: {
                              ADDRESS16_TLV,
                              LEADER_DATA_TLV,
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV
                              } <= set(p.mle.tlv.type)
                   ).\
            must_next()

        # Step 7: Leader Verifies connectivity by sending an ICMPv6 Echo Request
        #          to the DUT link local address
        #          The End Device MUST respond with ICMPv6 Echo Reply
        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(LEADER_LLA, DUT_LLA).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(DUT_LLA, LEADER_LLA).\
              must_next()


class Cert_6_5_2_ChildResetReattach_ED(Cert_6_5_2_ChildResetReattach_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_5_2_ChildResetReattach_Base.TOPOLOGY)
    TOPOLOGY[MTD]['mode'] = 'rn'


class Cert_6_5_2_ChildResetReattach_SED(Cert_6_5_2_ChildResetReattach_Base):
    TOPOLOGY = copy.deepcopy(Cert_6_5_2_ChildResetReattach_Base.TOPOLOGY)
    TOPOLOGY[MTD]['mode'] = '-'


del (Cert_6_5_2_ChildResetReattach_Base)

if __name__ == '__main__':
    unittest.main()
