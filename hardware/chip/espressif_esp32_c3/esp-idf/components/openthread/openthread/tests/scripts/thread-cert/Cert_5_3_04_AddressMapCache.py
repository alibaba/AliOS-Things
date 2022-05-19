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
import thread_cert
from pktverify.consts import MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, ADDR_QRY_URI, ADDR_NTF_URI, NL_ML_EID_TLV, NL_RLOC16_TLV, NL_TARGET_EID_TLV, COAP_CODE_POST
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
DUT_ROUTER1 = 2
SED1 = 3
MED1 = 4
MED2 = 5
MED3 = 6
MED4 = 7

MTDS = [SED1, MED1, MED2, MED3, MED4]

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate that the DUT is able to
# maintain an EID-to-RLOC Map Cache for a Sleepy End Device child attached to
# it. Each EID-to-RLOC Set MUST support at least four non-link-local unicast
# IPv6 addresses.
#
# Test Topology:
# -------------
# Router_1 - Leader
#    |      /      \
#   SED   MED_1 .. MED_4
#
# DUT Types:
# ----------
#  Router


class Cert_5_3_4_AddressMapCache(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [DUT_ROUTER1, MED1, MED2, MED3, MED4]
        },
        DUT_ROUTER1: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER, SED1]
        },
        SED1: {
            'name': 'SED',
            'is_mtd': True,
            'mode': '-',
            'timeout': 5,
            'allowlist': [DUT_ROUTER1]
        },
        MED1: {
            'name': 'MED_1',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [LEADER]
        },
        MED2: {
            'name': 'MED_2',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [LEADER]
        },
        MED3: {
            'name': 'MED_3',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [LEADER]
        },
        MED4: {
            'name': 'MED_4',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [LEADER]
        },
    }

    def test(self):
        # 1
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[DUT_ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT_ROUTER1].get_state(), 'router')

        for i in MTDS:
            self.nodes[i].start()
        self.simulator.go(5)

        for i in MTDS:
            self.assertEqual(self.nodes[i].get_state(), 'child')

        self.collect_ipaddrs()
        self.collect_rlocs()
        self.collect_rloc16s()

        # 2
        for MED in [MED1, MED2, MED3, MED4]:
            ed_mleid = self.nodes[MED].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
            self.assertTrue(self.nodes[SED1].ping(ed_mleid))
            self.simulator.go(5)

        # 3 & 4
        for MED in [MED1, MED2, MED3, MED4]:
            ed_mleid = self.nodes[MED].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
            self.assertTrue(self.nodes[SED1].ping(ed_mleid))
            self.simulator.go(5)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC16 = pv.vars['LEADER_RLOC16']
        ROUTER = pv.vars['ROUTER']
        ROUTER_RLOC = pv.vars['ROUTER_RLOC']
        SED = pv.vars['SED']
        SED_RLOC = pv.vars['SED_RLOC']
        SED_RLOC16 = pv.vars['SED_RLOC16']
        SED_MLEID = pv.vars['SED_MLEID']
        MM = pv.vars['MM_PORT']

        # Step 1: Build the topology as described
        pv.verify_attached('ROUTER')
        for i in range(1, 5):
            with pkts.save_index():
                pkts.filter_wpan_src64(pv.vars['MED_%d' %i]).\
                  filter_wpan_dst64(LEADER).\
                  filter_mle_cmd(MLE_CHILD_ID_REQUEST).\
                  must_next()
                pkts.filter_wpan_src64(LEADER).\
                    filter_wpan_dst64(pv.vars['MED_%d' %i]).\
                    filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
                    must_next()
        pkts.filter_wpan_src64(pv.vars['SED']).\
          filter_wpan_dst64(ROUTER).\
          filter_mle_cmd(MLE_CHILD_ID_REQUEST).\
          must_next()
        pkts.filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(pv.vars['SED']).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            must_next()

        # Step 2: SED sends an ICMPv6 Echo Request to MED_1, MED_2, MED_3, MED_4 ML-EID
        #         The DUT MUST generate an Address Query Request on SED’s behalf
        #         to find each node’s RLOC.
        #         The Address Query Request MUST be sent to the Realm-Local
        #         All-Routers address (FF03::2)
        #             CoAP URI-Path
        #                 - NON POST coap://<FF03::2>
        #             CoAP Payload
        #                 - Target EID TLV

        # Step 3: Leader sends Address Notification Messages with RLOC of MED_1, MED_2, MED_3, MED_4
        for i in range(1, 5):
            _pkt = pkts.filter_ping_request().\
                filter_wpan_src64(SED).\
                filter_ipv6_dst(pv.vars['MED_%d_MLEID' %i]).\
                must_next()
            pkts.filter_wpan_src64(ROUTER).\
                filter_RLARMA().\
                filter_coap_request(ADDR_QRY_URI, port=MM).\
                filter(lambda p: p.thread_address.tlv.target_eid == pv.vars['MED_%d_MLEID' %i]).\
                must_next()
            pkts.filter_wpan_src64(LEADER).\
                filter_ipv6_dst(ROUTER_RLOC).\
                filter_coap_request(ADDR_NTF_URI, port=MM).\
                filter(lambda p: {
                                  NL_ML_EID_TLV,
                                  NL_RLOC16_TLV,
                                  NL_TARGET_EID_TLV
                                  } <= set(p.coap.tlv.type) and\
                       p.thread_address.tlv.target_eid == pv.vars['MED_%d_MLEID' %i] and\
                       p.thread_address.tlv.rloc16 == LEADER_RLOC16 and\
                       p.coap.code == COAP_CODE_POST
                       ).\
               must_next()
            pkts.filter_ping_request(identifier=_pkt.icmpv6.echo.identifier).\
                filter_wpan_src64(ROUTER).\
                filter_ipv6_dst(pv.vars['MED_%d_MLEID' %i]).\
                must_next()
            pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
                filter_wpan_src64(pv.vars['MED_%d' %i]).\
                filter_ipv6_dst(SED_MLEID).\
                must_next()
            pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
                filter_wpan_src64(ROUTER).\
                filter_wpan_dst16(SED_RLOC16).\
                must_next()

        # Step 4: SED sends another ICMPv6 Echo Request to MED_1, MED_2, MED_3, MED_4 ML-EID
        #         The DUT MUST not send an Address Query during this step; If an address query
        #         message is sent, the test fails
        #         An ICMPv6 Echo Reply MUST be sent for each ICMPv6 Echo Request from SED
        for i in range(1, 5):
            _pkt = pkts.filter_ping_request().\
                filter_wpan_src64(SED).\
                filter_ipv6_dst(pv.vars['MED_%d_MLEID' %i]).\
                must_next()
            pkts.filter_wpan_src64(ROUTER).\
                filter_RLARMA().\
                filter_coap_request(ADDR_QRY_URI, port=MM).\
                must_not_next()
            pkts.filter_ping_request(identifier=_pkt.icmpv6.echo.identifier).\
                filter_wpan_src64(ROUTER).\
                filter_ipv6_dst(pv.vars['MED_%d_MLEID' %i]).\
                must_next()
            pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
                filter_wpan_src64(pv.vars['MED_%d' %i]).\
                filter_ipv6_dst(SED_MLEID).\
                must_next()
            pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
                filter_wpan_src64(ROUTER).\
                filter_wpan_dst16(SED_RLOC16).\
                must_next()


if __name__ == '__main__':
    unittest.main()
