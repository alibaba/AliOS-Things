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

import thread_cert
from pktverify.consts import MLE_CHILD_ID_RESPONSE, MLE_DATA_RESPONSE, MGMT_PENDING_SET_URI, MGMT_ACTIVE_SET_URI, MGMT_DATASET_CHANGED_URI, COAP_CODE_ACK, ACTIVE_OPERATION_DATASET_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV, NM_CHANNEL_TLV, NM_CHANNEL_MASK_TLV, NM_EXTENDED_PAN_ID_TLV, NM_NETWORK_KEY_TLV, NM_NETWORK_MESH_LOCAL_PREFIX_TLV, NM_NETWORK_NAME_TLV, NM_PAN_ID_TLV, NM_PSKC_TLV, NM_SECURITY_POLICY_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ACTIVE_TIMESTAMP_TLV, NETWORK_DATA_TLV, NM_BORDER_AGENT_LOCATOR_TLV, NM_COMMISSIONER_SESSION_ID_TLV, NM_DELAY_TIMER_TLV, PENDING_OPERATION_DATASET_TLV
from pktverify.packet_verifier import PacketVerifier

PANID_INIT = 0xface

COMMISSIONER = 1
LEADER = 2
ROUTER = 3

LEADER_ACTIVE_TIMESTAMP = 10
ROUTER_ACTIVE_TIMESTAMP = 20
ROUTER_PENDING_TIMESTAMP = 30
ROUTER_PENDING_ACTIVE_TIMESTAMP = 25
ROUTER_DELAY_TIMER = 3600000

COMMISSIONER_PENDING_TIMESTAMP = 40
COMMISSIONER_PENDING_ACTIVE_TIMESTAMP = 80
COMMISSIONER_DELAY_TIMER = 60000
COMMISSIONER_PENDING_CHANNEL = 20
COMMISSIONER_PENDING_PANID = 0xafce


class Cert_9_2_7_DelayTimer(thread_cert.TestCase):
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
            'partition_id': 0xffffffff,
            'allowlist': [COMMISSIONER]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'partition_id': 1,
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[COMMISSIONER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[COMMISSIONER].get_state(), 'router')
        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(3)

        self.nodes[COMMISSIONER].send_mgmt_active_set(active_timestamp=LEADER_ACTIVE_TIMESTAMP,)
        self.simulator.go(5)

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'leader')

        self.nodes[LEADER].add_allowlist(self.nodes[ROUTER].get_addr64())
        self.nodes[ROUTER].add_allowlist(self.nodes[LEADER].get_addr64())
        self.simulator.go(20)
        self.assertEqual(self.nodes[COMMISSIONER].get_state(), 'router')
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[ROUTER].commissioner_start()
        self.simulator.go(3)
        self.nodes[ROUTER].send_mgmt_active_set(active_timestamp=ROUTER_ACTIVE_TIMESTAMP,)
        self.simulator.go(30)

        self.nodes[ROUTER].send_mgmt_pending_set(
            pending_timestamp=ROUTER_PENDING_TIMESTAMP,
            active_timestamp=ROUTER_PENDING_ACTIVE_TIMESTAMP,
            delay_timer=ROUTER_DELAY_TIMER,
        )
        self.simulator.go(60)

        self.nodes[COMMISSIONER].send_mgmt_pending_set(
            pending_timestamp=COMMISSIONER_PENDING_TIMESTAMP,
            active_timestamp=COMMISSIONER_PENDING_ACTIVE_TIMESTAMP,
            delay_timer=COMMISSIONER_DELAY_TIMER,
            channel=COMMISSIONER_PENDING_CHANNEL,
            panid=COMMISSIONER_PENDING_PANID,
        )
        self.simulator.go(120)

        self.assertEqual(self.nodes[LEADER].get_panid(), COMMISSIONER_PENDING_PANID)
        self.assertEqual(self.nodes[COMMISSIONER].get_panid(), COMMISSIONER_PENDING_PANID)
        self.assertEqual(self.nodes[ROUTER].get_panid(), COMMISSIONER_PENDING_PANID)

        self.assertEqual(self.nodes[LEADER].get_channel(), COMMISSIONER_PENDING_CHANNEL)
        self.assertEqual(
            self.nodes[COMMISSIONER].get_channel(),
            COMMISSIONER_PENDING_CHANNEL,
        )
        self.assertEqual(self.nodes[ROUTER].get_channel(), COMMISSIONER_PENDING_CHANNEL)

        self.collect_rloc16s()
        self.collect_rlocs()
        ipaddrs = self.nodes[ROUTER].get_addrs()
        for ipaddr in ipaddrs:
            if ipaddr[0:4] != 'fe80':
                break
        self.assertTrue(self.nodes[LEADER].ping(ipaddr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC16 = pv.vars['LEADER_RLOC16']
        COMMISSIONER = pv.vars['COMMISSIONER']
        COMMISSIONER_RLOC = pv.vars['COMMISSIONER_RLOC']
        ROUTER = pv.vars['ROUTER']
        ROUTER_RLOC = pv.vars['ROUTER_RLOC']
        ROUTER_RLOC16 = pv.vars['ROUTER_RLOC16']
        _lpkts = pkts.filter_wpan_src64(LEADER)

        # Step 1: Ensure the topology is formed correctly
        _lpkts.filter_wpan_dst64(COMMISSIONER).filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next()
        _lpkts_coap = _lpkts.copy()

        # Step 4: Leader MUST send a unicast MLE Child ID Response to the Router
        _lpkts.filter_wpan_dst64(ROUTER).filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next(
        ).must_verify(lambda p: {ACTIVE_OPERATION_DATASET_TLV, ACTIVE_TIMESTAMP_TLV} < set(p.mle.tlv.type) and {
            NM_CHANNEL_TLV, NM_CHANNEL_MASK_TLV, NM_EXTENDED_PAN_ID_TLV, NM_NETWORK_KEY_TLV,
            NM_NETWORK_MESH_LOCAL_PREFIX_TLV, NM_NETWORK_NAME_TLV, NM_PAN_ID_TLV, NM_PSKC_TLV, NM_SECURITY_POLICY_TLV
        } <= set(p.thread_meshcop.tlv.type) and p.mle.tlv.active_tstamp == LEADER_ACTIVE_TIMESTAMP)

        # Step 6: Leader automatically sends a MGMT_ACTIVE_SET.rsp to the Router
        _lpkts.filter_ipv6_dst(ROUTER_RLOC).filter_coap_ack(MGMT_ACTIVE_SET_URI).must_next().must_verify(
            lambda p: p.coap.code == COAP_CODE_ACK and p.thread_meshcop.tlv.state == 1)

        # Step 7: Leader multicasts a MLE Data Response with the new information
        _lpkts.filter_LLANMA().filter_mle_cmd(MLE_DATA_RESPONSE).must_next().must_verify(
            lambda p: {SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ACTIVE_TIMESTAMP_TLV, NETWORK_DATA_TLV} <= set(
                p.mle.tlv.type) and {NM_BORDER_AGENT_LOCATOR_TLV, NM_COMMISSIONER_SESSION_ID_TLV} <= set(
                    p.thread_meshcop.tlv.type) and p.thread_nwd.tlv.stable == [0] and p.mle.tlv.active_tstamp ==
            ROUTER_ACTIVE_TIMESTAMP)

        # Step 10: Leader MUST send a unicast MLE Data Response to the Router
        _lpkts.filter_wpan_dst64(ROUTER).filter_mle_cmd(MLE_DATA_RESPONSE).must_next(
        ).must_verify(lambda p: {ACTIVE_OPERATION_DATASET_TLV, ACTIVE_TIMESTAMP_TLV} < set(p.mle.tlv.type) and {
            NM_CHANNEL_TLV, NM_CHANNEL_MASK_TLV, NM_EXTENDED_PAN_ID_TLV, NM_NETWORK_KEY_TLV,
            NM_NETWORK_MESH_LOCAL_PREFIX_TLV, NM_NETWORK_NAME_TLV, NM_PAN_ID_TLV, NM_PSKC_TLV, NM_SECURITY_POLICY_TLV
        } <= set(p.thread_meshcop.tlv.type) and p.mle.tlv.active_tstamp == ROUTER_ACTIVE_TIMESTAMP)

        # Step 12: Leader sends a MGMT_PENDING_SET.rsp to the Router with Status = Accept
        _lpkts_coap.filter_ipv6_dst(ROUTER_RLOC).filter_coap_ack(MGMT_PENDING_SET_URI).must_next().must_verify(
            lambda p: p.coap.code == COAP_CODE_ACK and p.thread_meshcop.tlv.state == 1)

        # Step 13: Leader sends a multicast MLE Data Response
        _lpkts.filter_LLANMA().filter_mle_cmd(MLE_DATA_RESPONSE).must_next().must_verify(
            lambda p: {
                SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV, NETWORK_DATA_TLV
            } <= set(p.mle.tlv.type) and p.thread_nwd.tlv.stable == [0] and p.mle.tlv.active_tstamp ==
            ROUTER_ACTIVE_TIMESTAMP and p.mle.tlv.pending_tstamp == ROUTER_PENDING_TIMESTAMP)

        # Step 14: The DUT MUST send MGMT_DATASET_CHANGED.ntf to the Router
        _lpkts_coap.filter_wpan_dst16(ROUTER_RLOC16).filter_coap_request(MGMT_DATASET_CHANGED_URI).must_next()

        # Step 16: Leader MUST send a unicast MLE Data Response to the Router
        _lpkts.filter_wpan_dst64(ROUTER).filter_mle_cmd(MLE_DATA_RESPONSE).must_next().must_verify(
            lambda p: {ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV} < set(p.mle.tlv.type) and p.mle.tlv.active_tstamp
            == ROUTER_ACTIVE_TIMESTAMP and p.mle.tlv.pending_tstamp == ROUTER_PENDING_TIMESTAMP)

        # Step 18: The DUT MUST send MGMT_PENDING_SET.rsp to the Commissioner
        _lpkts_coap.filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_ack(MGMT_PENDING_SET_URI).must_next().must_verify(
            lambda p: p.coap.code == COAP_CODE_ACK and p.thread_meshcop.tlv.state == 1)

        # Step 19: Leader MUST send a unicast MLE Data Response to the Router
        _lpkts.filter_LLANMA().filter_mle_cmd(MLE_DATA_RESPONSE).must_next().must_verify(
            lambda p: {
                SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV
            } <= set(p.mle.tlv.type) and p.thread_nwd.tlv.stable == [0] and p.mle.tlv.active_tstamp ==
            ROUTER_ACTIVE_TIMESTAMP and p.mle.tlv.pending_tstamp == COMMISSIONER_PENDING_TIMESTAMP)

        # Step 20: Leader MUST send a unicast MLE Data Response to the Router
        _lpkts.filter_wpan_dst64(ROUTER).filter_mle_cmd(MLE_DATA_RESPONSE).must_next(
        ).must_verify(lambda p: {ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV, PENDING_OPERATION_DATASET_TLV} < set(
            p.mle.tlv.type) and {NM_CHANNEL_TLV, NM_COMMISSIONER_SESSION_ID_TLV, NM_PAN_ID_TLV, NM_DELAY_TIMER_TLV} <=
                      set(p.thread_meshcop.tlv.type) and p.mle.tlv.active_tstamp == ROUTER_ACTIVE_TIMESTAMP and p.mle.
                      tlv.pending_tstamp == COMMISSIONER_PENDING_TIMESTAMP and p.thread_meshcop.tlv.pan_id ==
                      [COMMISSIONER_PENDING_PANID] and p.thread_meshcop.tlv.channel == [COMMISSIONER_PENDING_CHANNEL])

        # Step 21: Router MUST respond with an ICMPv6 Echo Reply
        pkts.filter_wpan_src16_dst16(ROUTER_RLOC16, LEADER_RLOC16).filter_ping_reply().must_next()


if __name__ == '__main__':
    unittest.main()
