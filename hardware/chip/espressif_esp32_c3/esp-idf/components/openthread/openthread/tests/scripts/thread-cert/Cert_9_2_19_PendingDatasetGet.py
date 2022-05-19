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

import mesh_cop
import thread_cert
from pktverify.consts import MGMT_PENDING_GET_URI, MGMT_PENDING_SET_URI, NM_CHANNEL_TLV, NM_PAN_ID_TLV, NM_NETWORK_NAME_TLV, NM_NETWORK_MESH_LOCAL_PREFIX_TLV, NM_PSKC_TLV, NM_ACTIVE_TIMESTAMP_TLV, NM_CHANNEL_MASK_TLV, NM_EXTENDED_PAN_ID_TLV, NM_NETWORK_KEY_TLV, NM_SECURITY_POLICY_TLV, NM_PENDING_TIMESTAMP_TLV, NM_DELAY_TIMER_TLV, LEADER_ALOC
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

COMMISSIONER = 1
LEADER = 2

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify Leader's and active Commissioner's behavior via
# MGMT_PENDING_GET request and response
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


class Cert_9_2_19_PendingDatasetGet(thread_cert.TestCase):
    SUPPORT_NCP = False

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
        self.simulator.get_messages_sent_by(LEADER)

        self.collect_rlocs()
        self.collect_rloc16s()

        leader_rloc = self.nodes[LEADER].get_rloc()

        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(3)

        self.nodes[COMMISSIONER].send_mgmt_pending_get()
        self.simulator.go(2)

        self.nodes[COMMISSIONER].send_mgmt_pending_set(
            active_timestamp=60,
            delay_timer=60000,
            panid=0xAFCE,
            pending_timestamp=30,
        )
        self.simulator.go(2)

        self.nodes[COMMISSIONER].send_mgmt_pending_get()
        self.simulator.go(2)

        self.nodes[COMMISSIONER].send_mgmt_pending_get(leader_rloc, [mesh_cop.TlvType.PAN_ID])
        self.simulator.go(92)

        self.nodes[COMMISSIONER].send_mgmt_pending_get()
        self.simulator.go(2)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        COMMISSIONER = pv.vars['COMMISSIONER']
        COMMISSIONER_RLOC = pv.vars['COMMISSIONER_RLOC']

        # Step 1: Ensure topology is formed correctly
        pv.verify_attached('COMMISSIONER', 'LEADER')

        # Step 2: Commissioner sends a MGMT_PENDING_GET.req to Leader Anycast
        #         or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/pg
        #         CoAP Payload
        #             <empty> - get all Active Operational Dataset parameters
        _mgmt_get_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_PENDING_GET_URI).\
            filter(lambda p: p.coap.tlv.type is nullField).\
            must_next()

        # Step 3: Leader sends a MGMT_PENDING_GET.rsp to Commissioner with
        #         the following format:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             <empty> - (no Pending Operational Dataset)
        pkts.filter_ipv6_src_dst(_mgmt_get_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_PENDING_GET_URI).\
            filter(lambda p: p.coap.tlv.type is nullField).\
            must_next()

        # Step 4: Commissioner sends a MGMT_PENDING_SET.req to Leader Anycast
        #         or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/ps
        #         CoAP Payload
        #             Active Timestamp TLV: 60s
        #             Commissioner Session ID TLV (valid)
        #             Delay Timer TLV: 1 minute
        #             Pending Timestamp TLV: 30s
        #             PAN ID TLV: 0xAFCE (new value)
        _mgmt_pending_set_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_PENDING_SET_URI).\
            filter(lambda p: {
                              NM_ACTIVE_TIMESTAMP_TLV,
                              NM_PENDING_TIMESTAMP_TLV,
                              NM_DELAY_TIMER_TLV,
                              NM_PAN_ID_TLV
                             } <= set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.active_tstamp == 60 and\
                   p.thread_meshcop.tlv.pan_id == [0xafce] and\
                   p.thread_meshcop.tlv.delay_timer == 60000
                   ).\
           must_next()

        # Step 5: Leader sends a MGMT_PENDING_SET.rsp to Commissioner with
        #         the following format:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             State TLV (value = Accept (01))
        pkts.filter_ipv6_src_dst(_mgmt_pending_set_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_PENDING_SET_URI).\
            filter(lambda p:
                   p.thread_meshcop.tlv.state == 1
                   ).\
           must_next()

        # Step 6: Commissioner sends a MGMT_PENDING_GET.req to Leader Anycast
        #         or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/pg
        #         CoAP Payload
        #             <empty> - get all Active Operational Dataset parameters
        _mgmt_pending_get_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_PENDING_GET_URI).\
            filter(lambda p: p.coap.tlv.type is nullField).\
            must_next()

        # Step 7: Leader sends a MGMT_PENDING_GET.rsp to Commissioner with
        #         the following format:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             (entire Active Operational Dataset)
        #             Active Timestamp TLV
        #             Channel TLV
        #             Channel Mask TLV
        #             Delay Timer TLV
        #             Extended PAN ID TLV
        #             Network Mesh-Local Prefix TLV
        #             Network Key TLV
        #             Network Name TLV
        #             PAN ID TLV
        #             Pending Timestamp TLV
        #             PSKc TLV
        #             Security Policy TLV
        pkts.filter_ipv6_src_dst(_mgmt_pending_get_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_PENDING_GET_URI).\
            filter(lambda p: {
                              NM_ACTIVE_TIMESTAMP_TLV,
                              NM_CHANNEL_TLV,
                              NM_CHANNEL_MASK_TLV,
                              NM_DELAY_TIMER_TLV,
                              NM_EXTENDED_PAN_ID_TLV,
                              NM_NETWORK_MESH_LOCAL_PREFIX_TLV,
                              NM_NETWORK_KEY_TLV,
                              NM_NETWORK_NAME_TLV,
                              NM_PAN_ID_TLV,
                              NM_PENDING_TIMESTAMP_TLV,
                              NM_PSKC_TLV,
                              NM_SECURITY_POLICY_TLV
                             } == set(p.thread_meshcop.tlv.type)
                   ).\
           must_next()

        # Step 8: Commissioner sends a MGMT_PENDING_GET.req to Leader Anycast
        #         or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/pg
        #         CoAP Payload
        #             PAN ID TLV
        _mgmt_pending_get_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_PENDING_GET_URI).\
            filter(lambda p: {
                              NM_PAN_ID_TLV
                             } <= set(p.thread_meshcop.tlv.type)
                   ).\
           must_next()

        # Step 9: Leader sends a MGMT_PENDING_GET.rsp to Commissioner with
        #         the following format:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             PAN ID TLV
        #             Delay Timer TLV
        pkts.filter_ipv6_src_dst(_mgmt_pending_get_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_PENDING_GET_URI).\
            filter(lambda p: {
                              NM_DELAY_TIMER_TLV,
                              NM_PAN_ID_TLV
                             } == set(p.coap.tlv.type) and\
                   p.thread_meshcop.tlv.pan_id == [0xafce] and\
                   p.thread_meshcop.tlv.delay_timer < 60000
                   ).\
           must_next()

        # Step 11: Commissioner sends a MGMT_PENDING_GET.req to Leader Anycast
        #         or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/pg
        #         CoAP Payload
        #             <empty> - get all Active Operational Dataset parameters
        _mgmt_pending_get_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_PENDING_GET_URI).\
            filter(lambda p: p.coap.tlv.type is nullField).\
            must_next()

        # Step 12: Leader sends a MGMT_PENDING_GET.rsp to Commissioner with
        #         the following format:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             <empty> - (no Pending Operational Dataset)
        pkts.filter_ipv6_src_dst(_mgmt_pending_get_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_PENDING_GET_URI).\
            filter(lambda p: p.thread_meshcop.tlv.type is nullField).\
           must_next()


if __name__ == '__main__':
    unittest.main()
