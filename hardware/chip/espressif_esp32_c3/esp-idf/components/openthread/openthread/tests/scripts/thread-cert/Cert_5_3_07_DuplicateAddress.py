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

import command
import config
import copy
import mle
import thread_cert
from pktverify.consts import WIRESHARK_OVERRIDE_PREFS, ADDR_ERR_URI, ADDR_QRY_URI, ADDR_NTF_URI, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, REALM_LOCAL_ALL_ROUTERS_ADDRESS, NL_TARGET_EID_TLV, NL_RLOC16_TLV, NL_ML_EID_TLV
from pktverify.packet_verifier import PacketVerifier

DUT_LEADER = 1
ROUTER1 = 2
ROUTER2 = 3
MED1 = 4
SED1 = 5
MED2 = 6

MTDS = [MED1, SED1, MED2]
ON_MESH_PREFIX = '2001::/64'
IPV6_ADDR = '2001::1234'

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate the DUT’s ability to perform
# duplicate address detection.
#
# Test Topology:
# -------------
# MED_2 - Leader(DUT)
#         /     \
#  Router_1   Router_2
#      |          |
#    MED_1       SED
#
# DUT Types:
# ----------
#  Leader


class Cert_5_3_7_DuplicateAddress(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        DUT_LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1, ROUTER2, MED2]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [DUT_LEADER, MED1]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [DUT_LEADER, SED1]
        },
        MED1: {
            'name': 'MED_1',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [ROUTER1]
        },
        SED1: {
            'name': 'SED',
            'is_mtd': True,
            'mode': '-',
            'allowlist': [ROUTER2]
        },
        MED2: {
            'name': 'MED_2',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [DUT_LEADER]
        },
    }

    # override wireshark preferences with case needed parameters
    CASE_WIRESHARK_PREFS = copy.deepcopy(WIRESHARK_OVERRIDE_PREFS)
    CASE_WIRESHARK_PREFS['6lowpan.context1'] = ON_MESH_PREFIX

    def test(self):
        self.nodes[DUT_LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT_LEADER].get_state(), 'leader')

        for i in range(ROUTER1, MED2 + 1):
            self.nodes[i].start()
            self.simulator.go(5)

        for i in [ROUTER1, ROUTER2]:
            self.assertEqual(self.nodes[i].get_state(), 'router')

        for i in MTDS:
            self.assertEqual(self.nodes[i].get_state(), 'child')

        self.collect_ipaddrs()
        self.collect_rlocs()

        self.nodes[ROUTER2].add_prefix(ON_MESH_PREFIX, 'paros')
        self.nodes[ROUTER2].register_netdata()

        self.nodes[MED1].add_ipaddr(IPV6_ADDR)
        self.nodes[SED1].add_ipaddr(IPV6_ADDR)
        self.simulator.go(5)

        self.nodes[MED2].ping(IPV6_ADDR)
        self.simulator.go(5)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_2 = pv.vars['ROUTER_2']
        MED_1 = pv.vars['MED_1']
        MED_2 = pv.vars['MED_2']
        SED = pv.vars['SED']
        MM = pv.vars['MM_PORT']

        # Step 1: Ensure topology is formed correctly
        for i in (1, 2):
            pv.verify_attached('ROUTER_%d' % i, 'LEADER')
        pv.verify_attached('MED_1', 'ROUTER_1', 'MTD')
        pv.verify_attached('SED', 'ROUTER_2', 'MTD')
        pv.verify_attached('MED_2', 'LEADER', 'MTD')

        # Step 5: MED_2 sends ICMPv6 Echo Request to address configured on MED_1
        #         and SED_1 with Prefix 2001::
        #         The DUT MUST multicast an Address Query message to the Realm-Local
        #         All Routers address (FF03::2):
        #             CoAP URI-Path
        #                 - NON POST coap://<FF03::2>
        #             CoAP Payload
        #                 - Target EID TLV

        pkts.filter_ping_request().\
            filter_wpan_src64(MED_2). \
            filter_ipv6_dst(IPV6_ADDR). \
            must_next()
        pkts.filter_wpan_src64(LEADER).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            filter(lambda p: p.thread_address.tlv.target_eid == IPV6_ADDR).\
            must_next()

        # Step 6: Router_1 & Router_2 respond with Address Notification message
        #         with matching Target TLVs

        for i in (1, 2):
            with pkts.save_index():
                pkts.filter_wpan_src64(pv.vars['ROUTER_%d' %i]).\
                    filter_ipv6_dst(LEADER_RLOC).\
                    filter_coap_request(ADDR_NTF_URI, port=MM).\
                    filter(lambda p: {
                                      NL_ML_EID_TLV,
                                      NL_RLOC16_TLV,
                                      NL_TARGET_EID_TLV
                                      } <= set(p.coap.tlv.type) and\
                           p.thread_address.tlv.target_eid == IPV6_ADDR
                           ).\
                   must_next()

        # Step 7: The DUT MUST multicast an Address Error Notification to the Realm-Local
        #         All Routers address (FF03::2):
        #             CoAP URI-Path
        #                 - NON POST coap://[<peer address>]:MM/a/ae
        #             CoAP Payload
        #                 - Target EID TLV
        #                 - ML-EID TLV
        #
        #         The IPv6 Source address MUST be the RLOC of the originator

        pkts.filter_ipv6_src_dst(LEADER_RLOC, REALM_LOCAL_ALL_ROUTERS_ADDRESS).\
            filter_coap_request(ADDR_ERR_URI, port=MM).\
            filter(lambda p: {
                              NL_ML_EID_TLV,
                              NL_TARGET_EID_TLV
                              } == set(p.coap.tlv.type) and\
                   p.thread_address.tlv.target_eid == IPV6_ADDR
                   ).\
            must_next()


if __name__ == '__main__':
    unittest.main()
