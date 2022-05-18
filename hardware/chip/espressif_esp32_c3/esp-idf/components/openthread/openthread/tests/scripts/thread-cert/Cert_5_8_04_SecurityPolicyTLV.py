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

from mesh_cop import TlvType
import thread_cert
from pktverify.consts import MLE_DATA_RESPONSE, MGMT_ACTIVE_SET_URI, MGMT_ACTIVE_GET_URI, LEADER_ALOC, NM_COMMISSIONER_SESSION_ID_TLV, NM_ACTIVE_TIMESTAMP_TLV, NM_SECURITY_POLICY_TLV, NM_NETWORK_KEY_TLV, MLE_DISCOVERY_RESPONSE
from pktverify.packet_verifier import PacketVerifier
from pktverify.layer_fields import nullField
from pktverify.bytes import Bytes

LEADER = 1
COMMISSIONER_1 = 2
COMMISSIONER_2 = 3
THREAD_NODE = 4

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify network behavior when Security
# Policy TLV “O”,”N”,”R”,”B” bits are disabled. “C” bit is not tested as it
# requires an External Commissioner which is currently not part of Thread
# Certification.
#
# Notes: Due to the packet parsing compatiable issue for supporting Thread 1.2
#        and 1.1, the security policy values can be fetched only in the unknown
#        field.
#
# Test Topology:
# -------------
#              Leader  ---- Commissioner_2
# Thread_Node    |
#             Commissioner_1
#
# Notes: Commissioner_2 is introduced in Step 10 and powoff
#        Thread_Node sends scan beacon
#
# DUT Types:
# ----------
#  Leader


class Cert_5_8_04_SecurityPolicyTLV(thread_cert.TestCase):
    SUPPORT_NCP = False
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'active_dataset': {
                'timestamp': 1,
                'channel': 19,
                'network_key': '00112233445566778899aabbccddeeff',
                'security_policy': [3600, 'onrcb']
            },
            'mode': 'rdn',
        },
        COMMISSIONER_1: {
            'name': 'COMMISSIONER_1',
            'active_dataset': {
                'timestamp': 1,
                'channel': 19,
                'network_key': '00112233445566778899aabbccddeeff',
                'security_policy': [3600, 'onrcb']
            },
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        COMMISSIONER_2: {
            'name': 'COMMISSIONER_2',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        THREAD_NODE: {
            'name': 'THREAD_NODE',
            'active_dataset': {
                'timestamp': 1,
                'channel': 19,
                'network_key': '00112233445566778899aabbccddeeff',
                'security_policy': [3600, 'onrcb']
            },
            'mode': 'rdn',
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[COMMISSIONER_1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[COMMISSIONER_1].get_state(), 'router')

        self.nodes[COMMISSIONER_1].commissioner_start()
        self.simulator.go(5)
        self.nodes[COMMISSIONER_1].commissioner_add_joiner('*', 'PSKD01')

        self.collect_rlocs()
        leader_rloc = self.nodes[LEADER].get_rloc()

        # Step 2
        self.nodes[COMMISSIONER_1].send_mgmt_active_get()
        self.simulator.go(5)

        # Step 5
        # Disabling O-Bit security_policy = [3600, 0b01111000]
        self.nodes[COMMISSIONER_1].send_mgmt_active_set(
            active_timestamp=15,
            security_policy=[3600, 'nrcb'],
        )
        self.simulator.go(5)

        # Step 7
        # Get NetworkKey
        self.nodes[COMMISSIONER_1].send_mgmt_active_get(leader_rloc, [TlvType.NETWORK_KEY])
        self.simulator.go(5)

        # Step 9
        # Disabling N-Bit security_policy = [3600, 0b10111000]
        self.nodes[COMMISSIONER_1].send_mgmt_active_set(
            active_timestamp=20,
            security_policy=[3600, 'orcb'],
        )
        self.simulator.go(5)

        # Step 10
        # Add Native Commissioner
        self.nodes[COMMISSIONER_2].interface_up()
        self.nodes[COMMISSIONER_2].joiner_start('10DKSP')
        self.simulator.go(5)
        self.nodes[COMMISSIONER_2].reset()

        # Step 13
        # Disabling B-Bit security_policy = [3600, 0b11110000]
        self.nodes[COMMISSIONER_1].send_mgmt_active_set(
            active_timestamp=25,
            security_policy=[3600, 'onrc'],
        )
        self.simulator.go(5)

        # Step 15
        # send beacons
        self.nodes[THREAD_NODE].start()
        self.simulator.go(2)
        self.nodes[THREAD_NODE].scan(result=0)
        self.simulator.go(20)

        # Step 17
        # Disabling R-Bit security_policy = [3600, 0b11011000]
        self.nodes[COMMISSIONER_1].send_mgmt_active_set(
            active_timestamp=30,
            security_policy=[3600, 'oncb'],
        )
        self.simulator.go(5)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        COMMISSIONER_1 = pv.vars['COMMISSIONER_1']
        COMMISSIONER_1_RLOC = pv.vars['COMMISSIONER_1_RLOC']
        COMMISSIONER_2 = pv.vars['COMMISSIONER_2']

        # Step 1: Ensure the topology is formed correctly
        pv.verify_attached('COMMISSIONER_1', 'LEADER')

        # Step 2: Commissioner_1 sends MGMT_ACTIVE_GET.req to Leader
        #         CoAP Request URI
        #             coap://[<L>]:MM/c/ag
        #         CoAP Payload
        #             <empty>
        pkts.filter_wpan_src64(COMMISSIONER_1).\
            filter_ipv6_2dsts(LEADER_RLOC, LEADER_ALOC).\
            filter_coap_request(MGMT_ACTIVE_GET_URI).\
            filter(lambda p: p.thread_meshcop.tlv.type is nullField).\
            must_next()

        # Step 3: Leader MUST send MGMT_ACTIVE_GET.rsp to the Commissioner_1
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             Security Policy TLV
        #                 Bits “O”,”N”,”R”,”C”,”B” should be set to 1
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(COMMISSIONER_1_RLOC).\
            filter_coap_ack(MGMT_ACTIVE_GET_URI).\
            filter(lambda p: (p.thread_meshcop.tlv.sec_policy_rot == 3600 and
                             p.thread_meshcop.tlv.sec_policy_o == 1 and
                             p.thread_meshcop.tlv.sec_policy_n == 1 and
                             p.thread_meshcop.tlv.sec_policy_r == 1 and
                             p.thread_meshcop.tlv.sec_policy_c == 1 and
                             p.thread_meshcop.tlv.sec_policy_b == 1) or
                            (p.thread_meshcop.tlv.unknown == '0e10ff')).\
            must_next()

        # Step 5: Commissioner_1 sends MGMT_ACTIVE_SET.req to Leader
        #         CoAP Request URI
        #             coap://[<L>]:MM/c/as
        #         CoAP Payload
        #             Commissioner Session ID TLV
        #             Active Timestamp TLV > stored value in step 4
        #             Security Policy TLV with “O” bit disabled
        pkts.filter_wpan_src64(COMMISSIONER_1). \
            filter_ipv6_2dsts(LEADER_RLOC, LEADER_ALOC). \
            filter_coap_request(MGMT_ACTIVE_SET_URI). \
            filter(lambda p: {
                              NM_COMMISSIONER_SESSION_ID_TLV,
                              NM_ACTIVE_TIMESTAMP_TLV,
                              NM_SECURITY_POLICY_TLV
                             }  == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.active_tstamp == 15 and\
                   (p.thread_meshcop.tlv.sec_policy_o == 0 or
                    p.thread_meshcop.tlv.unknown == '0e107f')).\
            must_next()

        # Step 6: Leader MUST send MGMT_ACTIVE_SET.rsp to the Commissioner_1
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             State TLV (value = Accept (0x01))
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(COMMISSIONER_1_RLOC).\
            filter_coap_ack(MGMT_ACTIVE_SET_URI).\
            filter(lambda p: p.thread_meshcop.tlv.state == 1).\
            must_next()

        # Step 7: Commissioner_1 sends MGMT_ACTIVE_GET.req to Leader
        #         CoAP Request URI
        #             coap://[<L>]:MM/c/ag
        #         CoAP Payload
        #             Network Key TLV
        pkts.filter_wpan_src64(COMMISSIONER_1).\
            filter_ipv6_2dsts(LEADER_RLOC, LEADER_ALOC).\
            filter_coap_request(MGMT_ACTIVE_GET_URI).\
            filter(lambda p: NM_NETWORK_KEY_TLV in p.thread_meshcop.tlv.type).\
            must_next()

        # Step 8: Leader MUST send MGMT_ACTIVE_GET.rsp to the Commissioner_1
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             Network Key TLV MUST NOT be included
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(COMMISSIONER_1_RLOC).\
            filter_coap_ack(MGMT_ACTIVE_GET_URI).\
            filter(lambda p: p.thread_meshcop.tlv.type is nullField).\
            must_next()

        # Step 9: Commissioner_1 sends MGMT_ACTIVE_SET.req to Leader
        #         CoAP Request URI
        #             coap://[<L>]:MM/c/as
        #         CoAP Payload
        #             Commissioner Session ID TLV
        #             Active Timestamp TLV > stored value in step 5
        #             Security Policy TLV with “N” bit disabled
        pkts.filter_wpan_src64(COMMISSIONER_1). \
            filter_ipv6_2dsts(LEADER_RLOC, LEADER_ALOC). \
            filter_coap_request(MGMT_ACTIVE_SET_URI). \
            filter(lambda p: {
                              NM_COMMISSIONER_SESSION_ID_TLV,
                              NM_ACTIVE_TIMESTAMP_TLV,
                              NM_SECURITY_POLICY_TLV
                             }  == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.active_tstamp == 20 and\
                   (p.thread_meshcop.tlv.sec_policy_n == 0 or
                    p.thread_meshcop.tlv.unknown == '0e10bf')).\
            must_next()

        # Step 10: Leader MUST send MGMT_ACTIVE_SET.rsp to the Commissioner_1
        #          CoAP Response Code
        #              2.04 Changed
        #          CoAP Payload
        #              State TLV (value = Accept (0x01))
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(COMMISSIONER_1_RLOC).\
            filter_coap_ack(MGMT_ACTIVE_SET_URI).\
            filter(lambda p: p.thread_meshcop.tlv.state == 1).\
            must_next()

        # Step 12: Leader MUST send a Discovery Response with Native Commissioning
        #          bit set to “Not Allowed”
        pkts.filter_wpan_src64(LEADER).\
            filter_mle_cmd(MLE_DISCOVERY_RESPONSE).\
            filter(lambda p: p.thread_meshcop.tlv.discovery_rsp_n == 0).\
            must_next()

        # Step 13: Commissioner_1 sends MGMT_ACTIVE_SET.req to Leader
        #          CoAP Request URI
        #              coap://[<L>]:MM/c/as
        #          CoAP Payload
        #              Commissioner Session ID TLV
        #              Active Timestamp TLV > stored value in step 9
        #              Security Policy TLV with “B” bit disabled
        pkts.filter_wpan_src64(COMMISSIONER_1). \
            filter_ipv6_2dsts(LEADER_RLOC, LEADER_ALOC). \
            filter_coap_request(MGMT_ACTIVE_SET_URI). \
            filter(lambda p: {
                              NM_COMMISSIONER_SESSION_ID_TLV,
                              NM_ACTIVE_TIMESTAMP_TLV,
                              NM_SECURITY_POLICY_TLV
                             }  == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.active_tstamp == 25 and\
                   (p.thread_meshcop.tlv.sec_policy_b == 0 or
                    p.thread_meshcop.tlv.unknown == '0e10f7')).\
            must_next()

        # Step 14: Leader MUST send MGMT_ACTIVE_SET.rsp to the Commissioner_1
        #          CoAP Response Code
        #              2.04 Changed
        #          CoAP Payload
        #              State TLV (value = Accept (0x01))
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(COMMISSIONER_1_RLOC).\
            filter_coap_ack(MGMT_ACTIVE_SET_URI).\
            filter(lambda p: p.thread_meshcop.tlv.state == 1).\
            must_next()

        # Step 16: The DUT MUST send beacon response frames.The beacon payload MUST
        #          either be empty OR the payload format MUST be different from the
        #          Thread Beacon payload The Protocol ID and Version field values
        #          MUST be different from the values specified for the Thread beacon
        #          (Protocol ID= 3, Version = 2)
        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_beacon().\
            filter(lambda p:
                   (p.thread_bcn.protocol is nullField or\
                    p.thread_bcn.protocol != 3) and\
                   (p.thread_bcn.version is nullField or\
                    p.thread_bcn.version != 2)
                   ).\
            must_next()

        # Step 17: Commissioner_1 sends MGMT_ACTIVE_SET.req to Leader
        #          CoAP Request URI
        #              coap://[<L>]:MM/c/as
        #          CoAP Payload
        #              Commissioner Session ID TLV
        #              Active Timestamp TLV > stored value in step 9
        #              Security Policy TLV with “R” bit disabled
        pkts.filter_wpan_src64(COMMISSIONER_1). \
            filter_ipv6_2dsts(LEADER_RLOC, LEADER_ALOC). \
            filter_coap_request(MGMT_ACTIVE_SET_URI). \
            filter(lambda p: {
                              NM_COMMISSIONER_SESSION_ID_TLV,
                              NM_ACTIVE_TIMESTAMP_TLV,
                              NM_SECURITY_POLICY_TLV
                             }  == set(p.thread_meshcop.tlv.type) and\
                   p.thread_meshcop.tlv.active_tstamp == 30 and\
                   (p.thread_meshcop.tlv.sec_policy_r == 0 or
                    p.thread_meshcop.tlv.unknown == '0e10df')).\
            must_next()

        # Step 18: Leader MUST send MGMT_ACTIVE_SET.rsp to the Commissioner_1
        #          CoAP Response Code
        #              2.04 Changed
        #          CoAP Payload
        #              State TLV (value = Accept (0x01))
        #          Leader MUST multicast MLE Data Response to the Link-Local All Nodes
        #          multicast address (FF02::1) with active timestamp value as set in
        #          Step 17.
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(COMMISSIONER_1_RLOC).\
            filter_coap_ack(MGMT_ACTIVE_SET_URI).\
            filter(lambda p: p.thread_meshcop.tlv.state == 1).\
            must_next()
        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p: p.mle.tlv.active_tstamp == 30).\
            must_next()

        # Step 20: The DUT MUST send a unicast MLE Data Response to Commissioner_1.
        #          The Active Operational Set MUST contain a Security Policy TLV with
        #          R bit set to 0.
        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(COMMISSIONER_1). \
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p:
                   p.mle.tlv.active_tstamp == 30 and\
                   (p.thread_meshcop.tlv.sec_policy_r == 0 or
                    p.thread_meshcop.tlv.unknown == '0e10df')).\
            must_next()


if __name__ == '__main__':
    unittest.main()
