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
from pktverify.consts import MLE_DATA_RESPONSE, LEAD_PET_URI, LEAD_KA_URI, MGMT_COMMISSIONER_SET_URI, NM_COMMISSIONER_ID_TLV, NM_COMMISSIONER_SESSION_ID_TLV, NM_STATE_TLV, NM_STEERING_DATA_TLV, NM_BORDER_AGENT_LOCATOR_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, SOURCE_ADDRESS_TLV, NWD_COMMISSIONING_DATA_TLV, MESHCOP_ACCEPT, MESHCOP_REJECT
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
COMMISSIONER = 2
FAKE_JOINER = '166e0a0000000999'

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify that a Commissioner Candidate is able
# to register itself to the network as a Commissioner, send periodic Commissioner
# keep-alive messages, update steering data and unregister itself as a Commissioner
# Leader accepts the Commissioner Candidate as a Commissioner in the network, responds
# to periodic Commissioner keep-alive messages, propagates Thread Network Data correctly
# in the network and unregisters the Commissioner on its request
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


class Cert_8_3_01_CommissionerPetition(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'networkkey': '00112233445566778899aabbccddeeff',
            'mode': 'rdn',
        },
        COMMISSIONER: {
            'name': 'COMMISSIONER',
            'networkkey': '00112233445566778899aabbccddeeff',
            'mode': 'rdn',
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')
        self.nodes[COMMISSIONER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[COMMISSIONER].get_state(), 'router')

        self.collect_rlocs()
        self.collect_rloc16s()
        self.collect_leader_aloc(LEADER)

        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(60)

        self.nodes[COMMISSIONER].commissioner_add_joiner(FAKE_JOINER, 'PSKD01')
        self.simulator.go(5)

        self.nodes[COMMISSIONER].commissioner_stop()
        self.simulator.go(5)
        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(5)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_ALOC = pv.vars['LEADER_ALOC']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        LEADER_RLOC16 = pv.vars['LEADER_RLOC16']
        COMMISSIONER = pv.vars['COMMISSIONER']
        COMMISSIONER_RLOC = pv.vars['COMMISSIONER_RLOC']

        # Step 2: Commissioner sends a petition request (LEAD_PET.req) to Leader:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/lp
        #         CoAP Payload
        #             Commissioner ID TLV
        pv.verify_attached('COMMISSIONER', 'LEADER')
        _pkt = pkts.last()
        _leader_pet_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_wpan_dst16(LEADER_RLOC16).\
            filter_coap_request(LEAD_PET_URI).\
            filter(lambda p: {
                              NM_COMMISSIONER_ID_TLV
                              } == set(p.coap.tlv.type)\
                   ).\
           must_next()

        # Step 3: Leader accepts Commissioner to the network, sends a Leader Petition
        #         Response (LEAD_PET.rsp) to Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             State TLV (value = Accept)
        #             Commissioner ID TLV
        #             Commissioner Session ID TLV
        #
        #         Leader sends a MLE Data Response to the network with the
        #         following TLVs:
        #             - Active Timestamp TLV
        #             - Leader Data TLV
        #             - Network Data TLV
        #             - Source Address TLV
        pkts.filter_ipv6_src_dst(_leader_pet_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(LEAD_PET_URI).\
            filter(lambda p: {
                              NM_STATE_TLV,
                              NM_COMMISSIONER_ID_TLV,
                              NM_COMMISSIONER_SESSION_ID_TLV
                              } <= set(p.coap.tlv.type) and\
                   p.thread_meshcop.tlv.state == MESHCOP_ACCEPT
                   ).\
           must_next()
        _dr_pkt = pkts.filter_wpan_src64(LEADER).\
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
                   p.mle.tlv.leader_data.data_version ==
                   (_pkt.mle.tlv.leader_data.data_version + 1) % 256 and\
                   p.thread_nwd.tlv.stable == [0]
                   ).\
            must_next()

        # Step 4: Commissioner sends a commissioner keep-alive request (LEAD_KA.req) to Leader:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/la
        #         CoAP Payload
        #             State TLV (value = Accept)
        #             Commissioner Session ID TLV
        _leader_ka_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(LEAD_KA_URI).\
            filter(lambda p: {
                              NM_STATE_TLV,
                              NM_COMMISSIONER_SESSION_ID_TLV
                              } == set(p.coap.tlv.type) and\
                   p.thread_meshcop.tlv.state == MESHCOP_ACCEPT
                   ).\
           must_next()

        # Step 5: Leader sends a commissioner keep-alive response (LEAD_KA.rsp) to Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             State TLV (value = Accept)
        pkts.filter_ipv6_src_dst(_leader_ka_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(LEAD_KA_URI).\
            filter(lambda p: {
                              NM_STATE_TLV
                              } <= set(p.coap.tlv.type) and\
                   p.thread_meshcop.tlv.state == MESHCOP_ACCEPT
                   ).\
           must_next()

        # Step 6: Commissioner sends a Set Commissioner Dataset Request (MGMT_COMMISSIONER_SET.req)
        #         to Leader ALOC 0xFC00 or Leader RLOC:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/cs
        #         CoAP Payload
        #             Commissioner Session ID TLV
        #             Steering Data TLV
        _mgmt_set_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(MGMT_COMMISSIONER_SET_URI).\
            filter(lambda p: {
                              NM_STEERING_DATA_TLV,
                              NM_COMMISSIONER_SESSION_ID_TLV
                              } == set(p.coap.tlv.type)
                   ).\
           must_next()

        # Step 7: Leader sends a Set Commissioner Dataset Response (MGMT_COMMISSIONER_SET.rsp) to
        #         Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             State TLV (value = Accept)
        #
        #         Leader sends a MLE Data Response to the network with the
        #         following TLVs:
        #             - Active Timestamp TLV
        #             - Leader Data TLV
        #             - Network Data TLV
        #             - Source Address TLV
        pkts.filter_ipv6_src_dst(_mgmt_set_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(MGMT_COMMISSIONER_SET_URI).\
            filter(lambda p: {
                              NM_STATE_TLV
                              } <= set(p.coap.tlv.type) and\
                   p.thread_meshcop.tlv.state == MESHCOP_ACCEPT
                   ).\
           must_next()

        _dr_pkt2 = pkts.filter_wpan_src64(LEADER).\
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
                   p.mle.tlv.leader_data.data_version ==
                   (_dr_pkt.mle.tlv.leader_data.data_version + 1) % 256 and\
                   p.thread_nwd.tlv.stable == [0]
                   ).\
            must_next()

        # Step 8: Commissioner sends a resign request via commissioner keep-alive request
        #         (LEAD_KA.req) to Leader:
        #         CoAP Request URI
        #             CON POST coap://<L>:MM/c/la
        #         CoAP Payload
        #             State TLV (value = Reject)
        #             Commissioner Session ID TLV
        _leader_ka_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(LEAD_KA_URI).\
            filter(lambda p: {
                              NM_STATE_TLV,
                              NM_COMMISSIONER_SESSION_ID_TLV
                              } == set(p.coap.tlv.type) and\
                   p.thread_meshcop.tlv.state == MESHCOP_REJECT
                   ).\
           must_next()

        # Step 9: Leader accepts the resignation by responding with ‘Reject’ in a commissioner keep-alive
        #         response (LEAD_KA.rsp) to Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             State TLV (value = Reject)
        #
        #         Leader sends a MLE Data Response to the network with the
        #         following TLVs:
        #             - Active Timestamp TLV
        #             - Leader Data TLV
        #             - Network Data TLV
        #             - Source Address TLV
        pkts.filter_ipv6_src_dst(_leader_ka_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(LEAD_KA_URI).\
            filter(lambda p: {
                              NM_STATE_TLV
                              } == set(p.coap.tlv.type) and\
                   p.thread_meshcop.tlv.state == MESHCOP_REJECT
                   ).\
           must_next()
        _dr_pkt3 = pkts.filter_wpan_src64(LEADER).\
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
                              NM_COMMISSIONER_SESSION_ID_TLV
                             } == set(p.thread_meshcop.tlv.type) and\
                   (p.mle.tlv.leader_data.data_version -
                   _dr_pkt2.mle.tlv.leader_data.data_version) % 256 <= 127 and\
                   p.thread_nwd.tlv.stable == [0]
                   ).\
            must_next()

        # Step 10: Commissioner sends a petition request (LEAD_PET.req) to Leader:
        #          CoAP Request URI
        #              CON POST coap://<L>:MM/c/lp
        #          CoAP Payload
        #              Commissioner ID TLV
        _leader_pet_pkt = pkts.filter_wpan_src64(COMMISSIONER).\
            filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
            filter_coap_request(LEAD_PET_URI).\
            filter(lambda p: {
                              NM_COMMISSIONER_ID_TLV
                              } == set(p.coap.tlv.type)\
                   ).\
           must_next()

        # Step 11: Leader accepts Commissioner to the network, sends a Leader Petition
        #          Response (LEAD_PET.rsp) to Commissioner:
        #          CoAP Response Code
        #              2.04 Changed
        #          CoAP Payload
        #              State TLV (value = Accept)
        #              Commissioner ID TLV
        #              Commissioner Session ID TLV (contains higher Session ID number
        #              than in Step 9)
        pkts.filter_ipv6_src_dst(_leader_pet_pkt.ipv6.dst, COMMISSIONER_RLOC).\
            filter_coap_ack(LEAD_PET_URI).\
            filter(lambda p: {
                              NM_STATE_TLV,
                              NM_COMMISSIONER_ID_TLV,
                              NM_COMMISSIONER_SESSION_ID_TLV
                              } <= set(p.coap.tlv.type) and\
                   p.thread_meshcop.tlv.state == MESHCOP_ACCEPT and\
                   p.thread_meshcop.tlv.commissioner_sess_id ==
                   (_dr_pkt3.thread_meshcop.tlv.commissioner_sess_id + 1) % 65536
                   ).\
           must_next()


if __name__ == '__main__':
    unittest.main()
