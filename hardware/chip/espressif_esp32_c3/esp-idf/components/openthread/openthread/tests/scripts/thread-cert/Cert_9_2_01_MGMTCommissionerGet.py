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

import thread_cert
from pktverify.consts import MLE_DATA_RESPONSE, MGMT_COMMISSIONER_GET_URI, NM_CHANNEL_TLV, NM_COMMISSIONER_ID_TLV, NM_COMMISSIONER_SESSION_ID_TLV, NM_STEERING_DATA_TLV, NM_BORDER_AGENT_LOCATOR_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, SOURCE_ADDRESS_TLV, NWD_COMMISSIONING_DATA_TLV, NM_PAN_ID_TLV, NM_NETWORK_NAME_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

COMMISSIONER = 1
LEADER = 2

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify Leader's and active Commissioner's behavior via
# MGMT_COMMISSIONER_GET request and response
#
# Test Topology:
# -------------
# Commissioner
#    |
#  Leader
#
# DUT Types:
# ----------
#  Leader
#  Commissioner


class Cert_9_2_01_MGMTCommissionerGet(thread_cert.TestCase):
    SUPPORT_NCP = False
    USE_MESSAGE_FACTORY = True

    TOPOLOGY = {
        COMMISSIONER: {
            'name': 'COMMISSIONER',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [COMMISSIONER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[COMMISSIONER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[COMMISSIONER].get_state(), 'router')

        self.collect_leader_aloc(LEADER)
        self.collect_rlocs()
        self.collect_rloc16s()

        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(3)

        self.nodes[COMMISSIONER].commissioner_mgmtget()
        self.simulator.go(5)

        self.nodes[COMMISSIONER].commissioner_mgmtget('0b08')
        self.simulator.go(5)

        self.nodes[COMMISSIONER].commissioner_mgmtget('0b01')
        self.simulator.go(5)

        self.nodes[COMMISSIONER].commissioner_mgmtget('0903')
        self.simulator.go(5)

        leader_rloc = self.nodes[LEADER].get_rloc()
        commissioner_rloc = self.nodes[COMMISSIONER].get_rloc()
        self.assertTrue(self.nodes[COMMISSIONER].ping(leader_rloc))
        self.simulator.go(1)
        self.assertTrue(self.nodes[LEADER].ping(commissioner_rloc))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_ALOC = pv.vars['LEADER_ALOC']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        COMMISSIONER = pv.vars['COMMISSIONER']
        COMMISSIONER_RLOC = pv.vars['COMMISSIONER_RLOC']

        # Step 1: Ensure topology is formed correctly
        pv.verify_attached('COMMISSIONER', 'LEADER')

        # Step 2: Commissioner sends a MGMT_COMMISSIONER_GET.req to Leader Anycast
        #         or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/cg
        #         CoAP Payload
        #             <empty> - get all Commissioner Dataset parameters
        _pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_COMMISSIONER_GET_URI).\
            filter(lambda p: p.coap.tlv.type is nullField).\
            must_next()

        # Step 3: Leader sends a MGMT_COMMISSIONER_GET.rsp to Commissioner with
        #         the following format:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             (entire Commissioner Dataset)
        #             Border Agent Locator TLV
        #             Commissioner Session ID TLV
        #             Steering Data TLV
        pkts.filter_ipv6_src_dst(_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_COMMISSIONER_GET_URI).\
            filter(lambda p: {
                              NM_COMMISSIONER_SESSION_ID_TLV,
                              NM_BORDER_AGENT_LOCATOR_TLV,
                              NM_STEERING_DATA_TLV
                             } == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.ba_locator is not nullField and\
                   p.thread_meshcop.tlv.commissioner_sess_id is not nullField and\
                   p.thread_meshcop.tlv.steering_data is not nullField
                   ).\
           must_next()

        # Step 4: Commissioner sends a MGMT_COMMISSIONER_GET.req to Leader Anycast
        #         or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/cg
        #         CoAP Payload
        #             Commissioner Session ID TLV
        #             Steering Data TLV
        _mgmt_get_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_COMMISSIONER_GET_URI).\
            filter(lambda p: {
                              NM_COMMISSIONER_SESSION_ID_TLV,
                              NM_STEERING_DATA_TLV
                             } <= set(p.thread_meshcop.tlv.type)
                   ).\
           must_next()

        # Step 5: Leader sends a MGMT_COMMISSIONER_GET.rsp to Commissioner with
        #         the following format:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #         Encoded values for the requested Commissioner Dataset parameters
        #             Commissioner Session ID TLV
        #             Steering Data TLV
        pkts.filter_ipv6_src_dst(_mgmt_get_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_COMMISSIONER_GET_URI).\
            filter(lambda p: {
                              NM_COMMISSIONER_SESSION_ID_TLV,
                              NM_STEERING_DATA_TLV
                             } == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.commissioner_sess_id is not nullField and\
                   p.thread_meshcop.tlv.steering_data is not nullField
                   ).\
           must_next()

        # Step 6: Commissioner sends a MGMT_COMMISSIONER_GET.req to Leader Anycast
        #         or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/cg
        #         CoAP Payload
        #             Commissioner Session ID TLV
        #             PAN ID TLV
        _mgmt_get_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_COMMISSIONER_GET_URI).\
            filter(lambda p: {
                              NM_COMMISSIONER_SESSION_ID_TLV,
                              NM_PAN_ID_TLV
                             } <= set(p.thread_meshcop.tlv.type)
                   ).\
           must_next()

        # Step 7: Leader sends a MGMT_COMMISSIONER_GET.rsp to Commissioner with
        #         the following format:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #         Encoded values for the requested Commissioner Dataset parameters
        #             Commissioner Session ID TLV
        #             (PAN ID TLV in Get TLV is ignored)
        pkts.filter_ipv6_src_dst(_mgmt_get_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_COMMISSIONER_GET_URI).\
            filter(lambda p: {
                              NM_COMMISSIONER_SESSION_ID_TLV
                             } == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.commissioner_sess_id is not nullField and\
                   p.thread_meshcop.tlv.pan_id is nullField
                   ).\
           must_next()

        # Step 8: Commissioner sends a MGMT_COMMISSIONER_GET.req to Leader Anycast
        #         or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/cg
        #         CoAP Payload
        #             Border Agent Locator TLV
        #             Network Name TLV
        _mgmt_get_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_COMMISSIONER_GET_URI).\
            filter(lambda p: {
                              NM_BORDER_AGENT_LOCATOR_TLV,
                              NM_NETWORK_NAME_TLV
                             } <= set(p.thread_meshcop.tlv.type)
                   ).\
           must_next()

        # Step 9: Leader sends a MGMT_COMMISSIONER_GET.rsp to Commissioner with
        #         the following format:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #         Encoded values for the requested Commissioner Dataset parameters
        #             Border Agent Locator TLV
        #             (Network Name TLV in Get TLV is ignored)
        pkts.filter_ipv6_src_dst(_mgmt_get_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_COMMISSIONER_GET_URI).\
            filter(lambda p: {
                              NM_BORDER_AGENT_LOCATOR_TLV
                             } == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.ba_locator is not nullField and\
                   p.thread_meshcop.tlv.net_name is nullField
                   ).\
           must_next()

        # Step 10: Verify connectivity by sending an ICMPv6 Echo Request to the DUT mesh local address
        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(COMMISSIONER_RLOC, LEADER_RLOC).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(LEADER_RLOC, COMMISSIONER_RLOC).\
            must_next()

        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(LEADER_RLOC, COMMISSIONER_RLOC).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(COMMISSIONER_RLOC, LEADER_RLOC).\
            must_next()


if __name__ == '__main__':
    unittest.main()
