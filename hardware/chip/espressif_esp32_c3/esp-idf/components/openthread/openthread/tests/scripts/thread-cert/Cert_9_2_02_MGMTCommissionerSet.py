#!/usr/bin/env python3
#
#  Copyright (c) 2019, The OpenThread Authors.
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
import mesh_cop
import thread_cert
from pktverify.consts import MLE_DATA_RESPONSE, LEAD_PET_URI, LEAD_KA_URI, MGMT_COMMISSIONER_SET_URI, NM_CHANNEL_TLV, NM_COMMISSIONER_ID_TLV, NM_COMMISSIONER_SESSION_ID_TLV, NM_STATE_TLV, NM_STEERING_DATA_TLV, NM_BORDER_AGENT_LOCATOR_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, SOURCE_ADDRESS_TLV, NWD_COMMISSIONING_DATA_TLV, MESHCOP_ACCEPT, MESHCOP_REJECT, LEADER_ALOC
from pktverify.packet_verifier import PacketVerifier
from pktverify.bytes import Bytes

COMMISSIONER = 1
LEADER = 2

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify Leader's and active Commissioner's behavior via
# MGMT_COMMISSIONER_SET request and response
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


class Cert_9_2_02_MGMTCommissionerSet(thread_cert.TestCase):
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

        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(3)

        leader_messages = self.simulator.get_messages_sent_by(LEADER)
        msg = leader_messages.next_coap_message('2.04', assert_enabled=True)
        commissioner_session_id_tlv = command.get_sub_tlv(msg.coap.payload, mesh_cop.CommissionerSessionId)

        steering_data_tlv = mesh_cop.SteeringData(bytes([0xff]))
        self.nodes[COMMISSIONER].commissioner_mgmtset_with_tlvs([steering_data_tlv])
        self.simulator.go(5)

        self.nodes[COMMISSIONER].commissioner_mgmtset_with_tlvs([steering_data_tlv, commissioner_session_id_tlv])
        self.simulator.go(5)

        border_agent_locator_tlv = mesh_cop.BorderAgentLocator(0x0400)
        self.nodes[COMMISSIONER].commissioner_mgmtset_with_tlvs(
            [commissioner_session_id_tlv, border_agent_locator_tlv])
        self.simulator.go(5)

        self.nodes[COMMISSIONER].commissioner_mgmtset_with_tlvs([
            steering_data_tlv,
            commissioner_session_id_tlv,
            border_agent_locator_tlv,
        ])
        self.simulator.go(5)

        self.nodes[COMMISSIONER].commissioner_mgmtset_with_tlvs(
            [mesh_cop.CommissionerSessionId(0xffff), steering_data_tlv])
        self.simulator.go(5)

        self.nodes[COMMISSIONER].commissioner_mgmtset_with_tlvs([
            commissioner_session_id_tlv,
            steering_data_tlv,
            mesh_cop.Channel(0x0, 0x0),
        ])
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
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        LEADER_RLOC16 = pv.vars['LEADER_RLOC16']
        COMMISSIONER = pv.vars['COMMISSIONER']
        COMMISSIONER_RLOC = pv.vars['COMMISSIONER_RLOC']

        # Step 1: Ensure topology is formed correctly
        pv.verify_attached('COMMISSIONER', 'LEADER')

        # Step 2: Commissioner sends a Set Commissioner Dataset Request (MGMT_COMMISSIONER_SET.req)
        #         to Leader Anycast or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/cs
        #         CoAP Payload
        #             (missing Commissioner Session ID TLV)
        #             Steering Data TLV (0xFF)
        _mgmt_set_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_COMMISSIONER_SET_URI).\
            filter(lambda p:
                   [NM_STEERING_DATA_TLV] == p.coap.tlv.type and\
                   p.thread_meshcop.tlv.steering_data == Bytes('ff')
                   ).\
           must_next()

        # Step 3: Leader sends a Set Commissioner Dataset Response (MGMT_COMMISSIONER_SET.rsp) to
        #         Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             State TLV (value = Reject)
        pkts.filter_ipv6_src_dst(_mgmt_set_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_COMMISSIONER_SET_URI).\
            filter(lambda p:
                   [NM_STATE_TLV] == p.coap.tlv.type and\
                   p.thread_meshcop.tlv.state == MESHCOP_REJECT
                   ).\
           must_next()

        # Step 4: Commissioner sends a Set Commissioner Dataset Request (MGMT_COMMISSIONER_SET.req)
        #         to Leader Anycast or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/cs
        #         CoAP Payload
        #             Commissioner Session ID TLV
        #             Steering Data TLV (0xFF)
        _mgmt_set_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_COMMISSIONER_SET_URI).\
            filter(lambda p: {
                              NM_COMMISSIONER_SESSION_ID_TLV,
                              NM_STEERING_DATA_TLV
                             } == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.steering_data == Bytes('ff')
                   ).\
           must_next()

        # Step 5: Leader sends a Set Commissioner Dataset Response (MGMT_COMMISSIONER_SET.rsp) to
        #         Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             State TLV (value = Accept)
        pkts.filter_ipv6_src_dst(_mgmt_set_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_COMMISSIONER_SET_URI).\
            filter(lambda p:
                   [NM_STATE_TLV] == p.coap.tlv.type and\
                   p.thread_meshcop.tlv.state == MESHCOP_ACCEPT
                   ).\
           must_next()

        # Step 6: Leader sends a MLE Data Response to the network with the
        #         following TLVs:
        #             - Active Timestamp TLV
        #             - Leader Data TLV
        #             - Network Data TLV
        #             - Source Address TLV
        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              ACTIVE_TIMESTAMP_TLV,
                              LEADER_DATA_TLV
                             } == set(p.mle.tlv.type) and\
                             {
                              NWD_COMMISSIONING_DATA_TLV
                             } == set(p.thread_nwd.tlv.type) and\
                             {
                              NM_BORDER_AGENT_LOCATOR_TLV,
                              NM_COMMISSIONER_SESSION_ID_TLV,
                              NM_STEERING_DATA_TLV
                             } == set(p.thread_meshcop.tlv.type) and\
                   p.thread_nwd.tlv.stable == [0]
                   ).\
            must_next()

        # Step 7: Commissioner sends a Set Commissioner Dataset Request (MGMT_COMMISSIONER_SET.req)
        #         to Leader Anycast or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/cs
        #         CoAP Payload
        #             Commissioner Session ID TLV
        #             Border Agent Locator TLV (0x0400) (not allowed TLV)
        _mgmt_set_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_COMMISSIONER_SET_URI).\
            filter(lambda p: {
                              NM_COMMISSIONER_SESSION_ID_TLV,
                              NM_BORDER_AGENT_LOCATOR_TLV
                             } == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.ba_locator == 0x0400
                   ).\
           must_next()

        # Step 8: Leader sends a Set Commissioner Dataset Response (MGMT_COMMISSIONER_SET.rsp) to
        #         Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             State TLV (value = Reject)
        pkts.filter_ipv6_src_dst(_mgmt_set_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_COMMISSIONER_SET_URI).\
            filter(lambda p:
                   [NM_STATE_TLV] == p.coap.tlv.type and\
                   p.thread_meshcop.tlv.state == MESHCOP_REJECT
                   ).\
           must_next()

        # Step 9: Commissioner sends a Set Commissioner Dataset Request (MGMT_COMMISSIONER_SET.req)
        #         to Leader Anycast or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/cs
        #         CoAP Payload
        #             Commissioner Session ID TLV
        #             Steering Data TLV (0xFF)
        #             Border Agent Locator TLV (0x0400) (not allowed TLV)
        _mgmt_set_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_COMMISSIONER_SET_URI).\
            filter(lambda p: {
                              NM_COMMISSIONER_SESSION_ID_TLV,
                              NM_STEERING_DATA_TLV,
                              NM_BORDER_AGENT_LOCATOR_TLV
                             } == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.ba_locator == 0x0400 and\
                   p.thread_meshcop.tlv.steering_data == Bytes('ff')
                   ).\
           must_next()

        # Step 10: Leader sends a Set Commissioner Dataset Response (MGMT_COMMISSIONER_SET.rsp) to
        #         Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             State TLV (value = Reject)
        pkts.filter_ipv6_src_dst(_mgmt_set_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_COMMISSIONER_SET_URI).\
            filter(lambda p:
                   [NM_STATE_TLV] == p.coap.tlv.type and\
                   p.thread_meshcop.tlv.state == MESHCOP_REJECT
                   ).\
           must_next()

        # Step 11: Commissioner sends a Set Commissioner Dataset Request (MGMT_COMMISSIONER_SET.req)
        #         to Leader Anycast or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/cs
        #         CoAP Payload
        #             Commissioner Session ID TLV (0xFFFF) (invalid value)
        #             Steering Data TLV (0xFF)
        _mgmt_set_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_COMMISSIONER_SET_URI).\
            filter(lambda p: {
                              NM_COMMISSIONER_SESSION_ID_TLV,
                              NM_STEERING_DATA_TLV
                             } == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.commissioner_sess_id == 0xFFFF and\
                   p.thread_meshcop.tlv.steering_data == Bytes('ff')
                   ).\
           must_next()

        # Step 12: Leader sends a Set Commissioner Dataset Response (MGMT_COMMISSIONER_SET.rsp) to
        #         Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             State TLV (value = Reject)
        pkts.filter_ipv6_src_dst(_mgmt_set_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_COMMISSIONER_SET_URI).\
            filter(lambda p:
                   [NM_STATE_TLV] == p.coap.tlv.type and\
                   p.thread_meshcop.tlv.state == MESHCOP_REJECT
                   ).\
           must_next()

        # Step 13: Commissioner sends a Set Commissioner Dataset Request (MGMT_COMMISSIONER_SET.req)
        #         to Leader Anycast or Routing Locator:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/cs
        #         CoAP Payload
        #             Commissioner Session ID TLV
        #             Steering Data TLV (0xFF)
        #             Channel TLV (not allowed TLV)
        _mgmt_set_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_COMMISSIONER_SET_URI).\
            filter(lambda p: {
                              NM_COMMISSIONER_SESSION_ID_TLV,
                              NM_STEERING_DATA_TLV,
                              NM_CHANNEL_TLV
                             } == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.steering_data == Bytes('ff')
                   ).\
           must_next()

        # Step 14: Leader sends a Set Commissioner Dataset Response (MGMT_COMMISSIONER_SET.rsp) to
        #         Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             State TLV (value = Accept)
        pkts.filter_ipv6_src_dst(_mgmt_set_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_COMMISSIONER_SET_URI).\
            filter(lambda p:
                   [NM_STATE_TLV] == p.coap.tlv.type and\
                   p.thread_meshcop.tlv.state == MESHCOP_ACCEPT
                   ).\
           must_next()

        # Step 15: Verify connectivity by sending an ICMPv6 Echo Request to the DUT mesh local address
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
