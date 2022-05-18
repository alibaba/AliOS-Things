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
from pktverify.consts import MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, MLE_DATA_RESPONSE, MLE_DATA_REQUEST, MGMT_PENDING_SET_URI, MGMT_ACTIVE_SET_URI, MGMT_DATASET_CHANGED_URI, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ACTIVE_OPERATION_DATASET_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV, TLV_REQUEST_TLV, NETWORK_DATA_TLV, NM_BORDER_AGENT_LOCATOR_TLV, NM_COMMISSIONER_SESSION_ID_TLV, NM_DELAY_TIMER_TLV, PENDING_OPERATION_DATASET_TLV, NWD_COMMISSIONING_DATA_TLV, LEADER_ALOC, NM_ACTIVE_TIMESTAMP_TLV, NM_CHANNEL_TLV, NM_CHANNEL_MASK_TLV, NM_EXTENDED_PAN_ID_TLV, NM_NETWORK_MESH_LOCAL_PREFIX_TLV, NM_NETWORK_KEY_TLV, NM_NETWORK_NAME_TLV, NM_PAN_ID_TLV, NM_PSKC_TLV, NM_SECURITY_POLICY_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

CHANNEL_INIT = 19
PANID_INIT = 0xface
TIMESTAMP_INIT = 10
CHANNEL_SECOND = 20

CHANNEL_FINAL = 19
PANID_FINAL = 0xabcd

ROUTER2_ACTIVE_TIMESTAMP = 15
ROUTER2_PENDING_ACTIVE_TIMESTAMP = 410
ROUTER2_PENDING_TIMESTAMP = 50
ROUTER2_DELAY_TIMER = 200000
ROUTER2_NET_NAME = 'TEST'

COMM_PENDING_ACTIVE_TIMESTAMP = 210
COMM_PENDING_TIMESTAMP = 30
COMM_DELAY_TIMER = 1000000

COMMISSIONER = 1
LEADER = 2
ROUTER1 = 3
ROUTER2 = 4

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify how Pending Operational Datasets
# are synchronized when two partitions merge.
#
# Test Topology:
# -------------
#   Commissioner
#       |
#     Leader
#       |
#     Router_1
#       |
#     Router_2
#
# Note: Router_1 and Router_2 will be in&out RF shield box
#
# DUT Types:
# ----------
# Leader
# Router


class Cert_9_2_09_PendingPartition(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        COMMISSIONER: {
            'name': 'COMMISSIONER',
            'active_dataset': {
                'timestamp': TIMESTAMP_INIT,
                'panid': PANID_INIT,
                'channel': CHANNEL_INIT
            },
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        LEADER: {
            'name': 'LEADER',
            'active_dataset': {
                'timestamp': TIMESTAMP_INIT,
                'panid': PANID_INIT,
                'channel': CHANNEL_INIT
            },
            'mode': 'rdn',
            'partition_id': 0xffffffff,
            'allowlist': [COMMISSIONER, ROUTER1]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'active_dataset': {
                'timestamp': TIMESTAMP_INIT,
                'panid': PANID_INIT,
                'channel': CHANNEL_INIT
            },
            'mode': 'rdn',
            'allowlist': [LEADER, ROUTER2]
        },
        ROUTER2: {
            'name': 'ROUTER_2',
            'active_dataset': {
                'timestamp': TIMESTAMP_INIT,
                'panid': PANID_INIT,
                'channel': CHANNEL_INIT
            },
            'mode': 'rdn',
            'network_id_timeout': 70,
            'allowlist': [ROUTER1]
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

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'router')

        self.nodes[COMMISSIONER].send_mgmt_pending_set(
            pending_timestamp=COMM_PENDING_TIMESTAMP,
            active_timestamp=COMM_PENDING_ACTIVE_TIMESTAMP,
            delay_timer=COMM_DELAY_TIMER,
            channel=CHANNEL_SECOND,
            panid=PANID_INIT,
        )
        self.simulator.go(5)

        self.nodes[LEADER].remove_allowlist(self.nodes[ROUTER1].get_addr64())
        self.nodes[ROUTER1].remove_allowlist(self.nodes[LEADER].get_addr64())
        self.nodes[ROUTER2].set_preferred_partition_id(1)
        self.simulator.go(250)

        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'leader')

        # Keeping network id timeout at 70 can result in ROUTER2
        # occasionally creating its own partition. Reset back to 120
        # here to avoid occasional test failures.
        self.nodes[ROUTER2].set_network_id_timeout(120)

        self.nodes[ROUTER2].commissioner_start()
        self.simulator.go(3)
        self.nodes[ROUTER2].send_mgmt_active_set(
            active_timestamp=ROUTER2_ACTIVE_TIMESTAMP,
            network_name=ROUTER2_NET_NAME,
        )
        self.simulator.go(5)

        self.nodes[ROUTER2].send_mgmt_pending_set(
            pending_timestamp=ROUTER2_PENDING_TIMESTAMP,
            active_timestamp=ROUTER2_PENDING_ACTIVE_TIMESTAMP,
            delay_timer=ROUTER2_DELAY_TIMER,
            channel=CHANNEL_FINAL,
            panid=PANID_FINAL,
        )
        self.simulator.go(5)

        self.nodes[LEADER].add_allowlist(self.nodes[ROUTER1].get_addr64())
        self.nodes[ROUTER1].add_allowlist(self.nodes[LEADER].get_addr64())
        self.simulator.go(260)

        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')
        self.assertEqual(self.nodes[ROUTER2].get_state(), 'router')

        self.collect_rlocs()
        self.collect_rloc16s()
        self.collect_ipaddrs()
        self.assertEqual(self.nodes[COMMISSIONER].get_panid(), PANID_FINAL)
        self.assertEqual(self.nodes[LEADER].get_panid(), PANID_FINAL)
        self.assertEqual(self.nodes[ROUTER1].get_panid(), PANID_FINAL)
        self.assertEqual(self.nodes[ROUTER2].get_panid(), PANID_FINAL)

        self.assertEqual(self.nodes[COMMISSIONER].get_channel(), CHANNEL_FINAL)
        self.assertEqual(self.nodes[LEADER].get_channel(), CHANNEL_FINAL)
        self.assertEqual(self.nodes[ROUTER1].get_channel(), CHANNEL_FINAL)
        self.assertEqual(self.nodes[ROUTER2].get_channel(), CHANNEL_FINAL)

        leader_addr = self.nodes[LEADER].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        router1_addr = self.nodes[ROUTER1].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        self.assertTrue(self.nodes[ROUTER2].ping(leader_addr, timeout=10))
        self.assertTrue(self.nodes[COMMISSIONER].ping(router1_addr, timeout=10))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        LEADER_MLEID = pv.vars['LEADER_MLEID']
        COMMISSIONER = pv.vars['COMMISSIONER']
        COMMISSIONER_MLEID = pv.vars['COMMISSIONER_MLEID']
        COMMISSIONER_RLOC = pv.vars['COMMISSIONER_RLOC']
        COMMISSIONER_RLOC16 = pv.vars['COMMISSIONER_RLOC16']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_1_RLOC = pv.vars['ROUTER_1_RLOC']
        ROUTER_1_MLEID = pv.vars['ROUTER_1_MLEID']
        ROUTER_2 = pv.vars['ROUTER_2']
        ROUTER_2_RLOC = pv.vars['ROUTER_2_RLOC']
        ROUTER_2_MLEID = pv.vars['ROUTER_2_MLEID']

        # Step 1: Ensure the topology is formed correctly
        for node in ('COMMISSIONER', 'ROUTER_1'):
            pv.verify_attached(node, 'LEADER')
        pv.verify_attached('ROUTER_2', 'ROUTER_1')
        _pkt = pkts.last()

        # Step 3: Leader sends MGMT_PENDING_SET.rsq to the Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             - State TLV (value = Accept)
        pkts.filter_coap_ack(MGMT_PENDING_SET_URI).\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(COMMISSIONER_RLOC).\
            must_next().\
            must_verify(lambda p: p.thread_meshcop.tlv.state == 1)

        # Step 4: Leader MUST multicast MLE Data Response with the new network data,
        #         including the following TLVs:
        #             - Source Address TLV
        #             - Leader Data TLV:
        #                 Data Version field incremented
        #                 Stable Version field incremented
        #             - Network Data TLV:
        #                 - Commissioner Data TLV:
        #                     Stable flag set to 0
        #                     Border Agent Locator TLV
        #                     Commissioner Session ID TLV
        #             - Active Timestamp TLV: 10s
        #             - Pending Timestamp TLV: 30s
        #
        #         Router_1 MUST send a unicast MLE Data Request to the Leader, including the
        #         following TLVs:
        #             - TLV Request TLV:
        #                - Network Data TLV
        #             - Active Timestamp TLV (10s)
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter(lambda p: p.mle.tlv.active_tstamp == TIMESTAMP_INIT and\
                   p.mle.tlv.pending_tstamp == COMM_PENDING_TIMESTAMP and\
                   (p.mle.tlv.leader_data.data_version -
                   _pkt.mle.tlv.leader_data.data_version) % 256 <= 127 and\
                   (p.mle.tlv.leader_data.stable_data_version -
                   _pkt.mle.tlv.leader_data.stable_data_version) % 256 <= 127 and\
                   p.thread_nwd.tlv.stable == [0] and\
                   NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and\
                   NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and\
                   NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type
                   ).\
            must_next()

        pkts.filter_wpan_src64(ROUTER_1).\
            filter_wpan_dst64(LEADER).\
            filter_mle_cmd(MLE_DATA_REQUEST).\
            filter(lambda p: {
                              TLV_REQUEST_TLV,
                              NETWORK_DATA_TLV,
                              ACTIVE_TIMESTAMP_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.mle.tlv.active_tstamp == TIMESTAMP_INIT and\
                   p.thread_meshcop.tlv.type is nullField
                   ).\
            must_next()

        # Step 5: Leader sends a MLE Data Response to Router_1 including the following TLVs:
        #             - Source Address TLV
        #             - Leader Data TLV
        #             - Network Data TLV
        #                 - Commissioner Data TLV:
        #                     Stable flag set to 0
        #                     Border Agent Locator TLV
        #                     Commissioner Session ID TLV
        #             - Active Timestamp TLV: 10s
        #             - Pending Timestamp TLV: 30s
        #             - Pending Operational Dataset TLV
        #                 - Active Timestamp TLV <210s>
        #                 - Delay Timer TLV <~ 1000s>
        #                 - Channel TLV : ‘Secondary’
        #                 - PAN ID TLV : 0xAFCE
        _dr_pkt = pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER_1).\
            filter(lambda p: {
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV,
                              ACTIVE_TIMESTAMP_TLV,
                              PENDING_TIMESTAMP_TLV,
                              PENDING_OPERATION_DATASET_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.thread_nwd.tlv.stable == [0] and\
                   NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and\
                   NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and\
                   NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type and\
                   p.mle.tlv.active_tstamp == TIMESTAMP_INIT and\
                   p.mle.tlv.pending_tstamp == COMM_PENDING_TIMESTAMP and\
                   p.thread_meshcop.tlv.delay_timer < COMM_DELAY_TIMER and\
                   p.thread_meshcop.tlv.active_tstamp == COMM_PENDING_ACTIVE_TIMESTAMP and\
                   p.thread_meshcop.tlv.channel == [CHANNEL_SECOND] and\
                   p.thread_meshcop.tlv.pan_id == [PANID_INIT]
                   ).\
            must_next()

        # Step 6: Router_1 MUST multicast MLE Data Response with the new network data,
        #         including the following TLVs:
        #             - Source Address TLV
        #             - Leader Data TLV:
        #                 Data Version field incremented
        #                 Stable Version field incremented
        #             - Network Data TLV:
        #                 - Commissioner Data TLV:
        #                     Stable flag set to 0
        #                     Border Agent Locator TLV
        #                     Commissioner Session ID TLV
        #             - Active Timestamp TLV: 10s
        #             - Pending Timestamp TLV: 30s
        with pkts.save_index():
            pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
                filter_wpan_src64(ROUTER_1).\
                filter_LLANMA().\
                filter(lambda p: p.mle.tlv.active_tstamp == TIMESTAMP_INIT and\
                       p.mle.tlv.pending_tstamp == COMM_PENDING_TIMESTAMP and\
                       (p.mle.tlv.leader_data.data_version -
                       _pkt.mle.tlv.leader_data.data_version) % 256 <= 127 and\
                       (p.mle.tlv.leader_data.stable_data_version -
                       _pkt.mle.tlv.leader_data.stable_data_version) % 256 <= 127 and\
                       p.thread_nwd.tlv.stable == [0] and\
                       NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and\
                       NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and\
                       NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type
                       ).\
                must_next()

        # Step 8: Router_1 sends a MLE Data Response to Router_2 including the following TLVs:
        #             - Source Address TLV
        #             - Leader Data TLV
        #             - Network Data TLV
        #                 - Commissioner Data TLV:
        #                     Stable flag set to 0
        #                     Border Agent Locator TLV
        #                     Commissioner Session ID TLV
        #             - Active Timestamp TLV: 10s
        #             - Pending Timestamp TLV: 30s
        #             - Pending Operational Dataset TLV
        #                 - Active Timestamp TLV <210s>
        #                 - Delay Timer TLV <~ 1000s>
        #                 - Channel TLV : ‘Secondary’
        #                 - PAN ID TLV : 0xAFCE
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter_wpan_src64(ROUTER_1).\
            filter_wpan_dst64(ROUTER_2).\
            filter(lambda p: {
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV,
                              ACTIVE_TIMESTAMP_TLV,
                              PENDING_TIMESTAMP_TLV,
                              PENDING_OPERATION_DATASET_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.thread_nwd.tlv.stable == [0] and\
                   NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and\
                   NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and\
                   NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type and\
                   p.mle.tlv.active_tstamp == TIMESTAMP_INIT and\
                   p.mle.tlv.pending_tstamp == COMM_PENDING_TIMESTAMP and\
                   p.thread_meshcop.tlv.delay_timer < COMM_DELAY_TIMER and\
                   p.thread_meshcop.tlv.active_tstamp == COMM_PENDING_ACTIVE_TIMESTAMP and\
                   p.thread_meshcop.tlv.channel == [CHANNEL_SECOND] and\
                   p.thread_meshcop.tlv.pan_id == [PANID_INIT]
                   ).\
            must_next()

        # Step 10: Router_1 MUST attach to the new partition formed by Router_2
        pv.verify_attached('ROUTER_1', 'ROUTER_2')
        _pkt = pkts.last()

        # Step 12: Router_1 MUST send a unicast MLE Data Request to the Router_2, including the
        #         following TLVs:
        #             - TLV Request TLV:
        #                - Network Data TLV
        #             - Active Timestamp TLV (10s)
        #             - Pending Timestamp TLV (30s)
        with pkts.save_index():
            pkts.filter_wpan_src64(ROUTER_1).\
                filter_wpan_dst64(LEADER).\
                filter_mle_cmd(MLE_DATA_REQUEST).\
                filter(lambda p: {
                                  TLV_REQUEST_TLV,
                                  NETWORK_DATA_TLV,
                                  ACTIVE_TIMESTAMP_TLV
                                  } <= set(p.mle.tlv.type) and\
                       p.mle.tlv.active_tstamp == TIMESTAMP_INIT and\
                       p.mle.tlv.pending_tstamp == COMM_PENDING_TIMESTAMP and\
                       p.thread_meshcop.tlv.type is nullField
                       ).\
                must_next()

        # Step 14: Router_1 MUST multicast MLE Data Response with the new network data,
        #          including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV:
        #                  Data Version field incremented
        #                  Stable Version field incremented
        #              - Network Data TLV:
        #                  - Commissioner Data TLV:
        #                      Stable flag set to 0
        #                      Border Agent Locator TLV
        #                      Commissioner Session ID TLV
        #              - Active Timestamp TLV: 15s
        #              - Pending Timestamp TLV: 30s
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter_wpan_src64(ROUTER_1).\
            filter_LLANMA().\
            filter(lambda p: p.mle.tlv.active_tstamp == ROUTER2_ACTIVE_TIMESTAMP and\
                   p.mle.tlv.pending_tstamp == COMM_PENDING_TIMESTAMP and\
                   (p.mle.tlv.leader_data.data_version -
                   _pkt.mle.tlv.leader_data.data_version) % 256 <= 127 and\
                   (p.mle.tlv.leader_data.stable_data_version -
                   _pkt.mle.tlv.leader_data.stable_data_version) % 256 <= 127 and\
                   p.thread_nwd.tlv.stable == [0] and\
                   NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and\
                   NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and\
                   NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type
                   ).\
            must_next()

        # Step 17: Router_1 MUST send a unicast MLE Data Request to the Router_2, including the
        #          following TLVs:
        #             - TLV Request TLV:
        #                - Network Data TLV
        #             - Active Timestamp TLV (15s)
        #             - Pending Timestamp TLV (30s)
        pkts.filter_wpan_src64(ROUTER_1).\
            filter_wpan_dst64(ROUTER_2).\
            filter_mle_cmd(MLE_DATA_REQUEST).\
            filter(lambda p: {
                              TLV_REQUEST_TLV,
                              NETWORK_DATA_TLV,
                              ACTIVE_TIMESTAMP_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.mle.tlv.active_tstamp == ROUTER2_ACTIVE_TIMESTAMP and\
                   p.mle.tlv.pending_tstamp == COMM_PENDING_TIMESTAMP and\
                   p.thread_meshcop.tlv.type is nullField
                   ).\
            must_next()

        # Step 19: Router_1 MUST multicast MLE Data Response with the new network data,
        #          including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV:
        #                  Data Version field incremented
        #                  Stable Version field incremented
        #              - Network Data TLV:
        #                  - Commissioner Data TLV:
        #                      Stable flag set to 0
        #                      Border Agent Locator TLV
        #                      Commissioner Session ID TLV
        #              - Active Timestamp TLV: 15s
        #              - Pending Timestamp TLV: 50s
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter_wpan_src64(ROUTER_1).\
            filter_LLANMA().\
            filter(lambda p: p.mle.tlv.active_tstamp == ROUTER2_ACTIVE_TIMESTAMP and\
                   p.mle.tlv.pending_tstamp == ROUTER2_PENDING_TIMESTAMP and\
                   (p.mle.tlv.leader_data.data_version -
                   _pkt.mle.tlv.leader_data.data_version) % 256 <= 127 and\
                   (p.mle.tlv.leader_data.stable_data_version -
                   _pkt.mle.tlv.leader_data.stable_data_version) % 256 <= 127 and\
                   p.thread_nwd.tlv.stable == [0] and\
                   NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and\
                   NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and\
                   NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type
                   ).\
            must_next()

        # Step 21: Router_1 MUST go through the attachment process and send MLE Child ID
        #          Request to the Leader, including the following TLV:
        #              - Active Timestamp TLV: 15s
        pkts.filter_mle_cmd(MLE_CHILD_ID_REQUEST).\
            filter_wpan_src64(ROUTER_1).\
            filter_wpan_dst64(LEADER).\
            filter(lambda p: p.mle.tlv.active_tstamp == ROUTER2_ACTIVE_TIMESTAMP).\
            must_next()

        # Step 22: Leader MUST send MLE Child ID Response to Router_1, including its current
        #          active timestamp and active configuration set:
        #              - Active Timestamp TLV: 10s
        #              - Active Operational Dataset TLV:
        #              - Pending Timestamp TLV: 30s
        #              - Pending Operational Dataset TLV:
        #                      - Active Timestamp TLV:210s
        _pkt = pkts.filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER_1).\
            filter(lambda p:
                   p.mle.tlv.active_tstamp == TIMESTAMP_INIT and\
                   p.mle.tlv.pending_tstamp == COMM_PENDING_TIMESTAMP and\
                   p.thread_meshcop.tlv.active_tstamp == COMM_PENDING_ACTIVE_TIMESTAMP
                   ).\
            must_next()

        # Step 23: Router_1 MUST send MGMT_ACTIVE_SET.req to the Leader RLOC or Anycast Locator:
        #          CoAP Request URI
        #              coap://[Leader]:MM/c/as
        #          CoAP Payload
        #                  - Active Timestamp TLV: 15s
        #                  - Network Name TLV: “TEST”
        #                  - PAN ID TLV
        #                  - Channel TLV
        with pkts.save_index():
            pkts.filter_wpan_src64(ROUTER_1).\
                filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
                filter_coap_request(MGMT_ACTIVE_SET_URI) .\
                filter(lambda p: {
                                  NM_ACTIVE_TIMESTAMP_TLV,
                                  NM_CHANNEL_TLV,
                                  NM_NETWORK_NAME_TLV,
                                  NM_PAN_ID_TLV,
                                 } <= set(p.thread_meshcop.tlv.type) and\
                       p.thread_meshcop.tlv.active_tstamp == ROUTER2_ACTIVE_TIMESTAMP and\
                       p.thread_meshcop.tlv.net_name == [ROUTER2_NET_NAME]
                       ).\
                must_next()

        # Step 24: Leader sends MGMT_ACTIVE_SET.rsp to the Router_1:
        #          CoAP Response Code
        #              2.04 Changed
        #          CoAP Payload
        #              - State TLV (value = Accept)
        #          TODO: this ack can not be parsed by pktverify

        # Step 25: Leader MUST send MGMT_DATASET_CHANGED.ntf to Commissioner:
        #          CoAP Request URI
        #              coap://[ Commissioner]:MM/c/dc
        #          CoAP Payload
        #              <empty>
        with pkts.save_index():
            pkts.filter_wpan_src64(LEADER).\
                filter_wpan_dst16(COMMISSIONER_RLOC16).\
                filter_coap_request(MGMT_DATASET_CHANGED_URI) .\
                filter(lambda p: p.thread_meshcop.tlv.type is nullField).\
                must_next()

        # Step 27: Router_1 MUST send MGMT_PENDING_SET.req to the Leader RLOC or Anycast Locator:
        #          CoAP Request URI
        #              coap://[Leader]:MM/c/ps
        #          CoAP Payload
        #               - Delay Timer TLV: ~200s
        #               - Channel TLV : ‘Primary’
        #               - PAN ID TLV : 0xABCD
        #               - Network Name TLV: ‘TEST’
        #               - Active Timestamp TLV: 410s
        #               - Pending Timestamp TLV: 50s
        with pkts.save_index():
            pkts.filter_wpan_src64(ROUTER_1).\
                filter_ipv6_2dsts(LEADER_ALOC, LEADER_RLOC).\
                filter_coap_request(MGMT_PENDING_SET_URI) .\
                filter(lambda p:
                       p.thread_meshcop.tlv.delay_timer < ROUTER2_DELAY_TIMER and\
                       p.thread_meshcop.tlv.channel == [CHANNEL_FINAL] and\
                       p.thread_meshcop.tlv.pan_id == [PANID_FINAL] and\
                       p.thread_meshcop.tlv.active_tstamp == ROUTER2_PENDING_ACTIVE_TIMESTAMP and\
                       p.thread_meshcop.tlv.pending_tstamp == ROUTER2_PENDING_TIMESTAMP and\
                       p.thread_meshcop.tlv.net_name == [ROUTER2_NET_NAME]
                       ).\
                must_next()

        # Step 28: Leader sends MGMT_PENDING_SET.rsq to the Router_1:
        #          CoAP Response Code
        #              2.04 Changed
        #          CoAP Payload
        #              - State TLV (value = Accept)
        #          TODO: this ack can not be parsed by pktverify

        # Step 29: Leader MUST send MGMT_DATASET_CHANGED.ntf to Commissioner:
        #          CoAP Request URI
        #              coap://[ Commissioner]:MM/c/dc
        #          CoAP Payload
        #              <empty>
        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst16(COMMISSIONER_RLOC16).\
            filter_coap_request(MGMT_DATASET_CHANGED_URI) .\
            filter(lambda p: p.thread_meshcop.tlv.type is nullField).\
            must_next()

        # Step 30: Leader MUST multicast MLE Data Response with the new network data,
        #          including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV:
        #                  Data Version field incremented
        #                  Stable Version field incremented
        #              - Network Data TLV:
        #                  - Commissioner Data TLV:
        #                      Stable flag set to 0
        #                      Border Agent Locator TLV
        #                      Commissioner Session ID TLV
        #              - Active Timestamp TLV: 15s
        #              - Pending Timestamp TLV: 50s
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter(lambda p: p.mle.tlv.active_tstamp == ROUTER2_ACTIVE_TIMESTAMP and\
                   p.mle.tlv.pending_tstamp == ROUTER2_PENDING_TIMESTAMP and\
                   (p.mle.tlv.leader_data.data_version -
                   _pkt.mle.tlv.leader_data.data_version) % 256 <= 127 and\
                   (p.mle.tlv.leader_data.stable_data_version -
                   _pkt.mle.tlv.leader_data.stable_data_version) % 256 <= 127 and\
                   p.thread_nwd.tlv.stable == [0] and\
                   NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and\
                   NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and\
                   NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type
                   ).\
            must_next()

        # Step 32: Leader sends a MLE Data Response to Commissioner including the following TLVs:
        #             - Source Address TLV
        #             - Leader Data TLV
        #             - Active Timestamp TLV: 15s
        #             - Active Operational Dataset TLV:
        #                 - Network Name TLV : ‘TEST’
        #             - Pending Timestamp TLV: 50s
        #             - Pending Operational Dataset TLV
        #                 - Active Timestamp TLV <410s>
        #                 - Delay Timer TLV <~ 200s>
        #                 - Channel TLV : ‘Primary’
        #                 - PAN ID TLV : 0xABCD
        #                 - Network Name TLV : 'TEST'
        with pkts.save_index():
            pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
                filter_wpan_src64(LEADER).\
                filter_wpan_dst64(COMMISSIONER).\
                filter(lambda p: {
                                  SOURCE_ADDRESS_TLV,
                                  LEADER_DATA_TLV,
                                  ACTIVE_TIMESTAMP_TLV,
                                  PENDING_TIMESTAMP_TLV,
                                  PENDING_OPERATION_DATASET_TLV
                                  } <= set(p.mle.tlv.type) and\
                       p.thread_nwd.tlv.stable == [0] and\
                       NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and\
                       NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and\
                       NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type and\
                       p.mle.tlv.active_tstamp == ROUTER2_ACTIVE_TIMESTAMP and\
                       p.mle.tlv.pending_tstamp == ROUTER2_PENDING_TIMESTAMP and\
                       p.thread_meshcop.tlv.net_name == [ROUTER2_NET_NAME, ROUTER2_NET_NAME] and\
                       p.thread_meshcop.tlv.delay_timer < ROUTER2_DELAY_TIMER and\
                       p.thread_meshcop.tlv.active_tstamp == ROUTER2_PENDING_ACTIVE_TIMESTAMP and\
                       p.thread_meshcop.tlv.channel == [CHANNEL_INIT, CHANNEL_FINAL] and\
                       p.thread_meshcop.tlv.pan_id == [PANID_INIT, PANID_FINAL]
                       ).\
                must_next()

        # Step 33: Router_1 MUST send a unicast MLE Data Request to the Leader, including the
        #          following TLVs:
        #             - TLV Request TLV:
        #                - Network Data TLV
        #             - Active Timestamp TLV (10s)
        #             - Pending Timestamp TLV (30s)
        pkts.filter_wpan_src64(ROUTER_1).\
            filter_wpan_dst64(LEADER).\
            filter_mle_cmd(MLE_DATA_REQUEST).\
            filter(lambda p: {
                              TLV_REQUEST_TLV,
                              NETWORK_DATA_TLV,
                              ACTIVE_TIMESTAMP_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.mle.tlv.active_tstamp == TIMESTAMP_INIT and\
                   p.mle.tlv.pending_tstamp == COMM_PENDING_TIMESTAMP and\
                   p.thread_meshcop.tlv.type is nullField
                   ).\
            must_next()

        # Step 34: Leader sends a MLE Data Response to Router_1 including the following TLVs:
        #             - Source Address TLV
        #             - Leader Data TLV
        #             - Active Timestamp TLV: 15s
        #             - Active Operational Dataset TLV:
        #                 - Network Name TLV : ‘TEST’
        #             - Pending Timestamp TLV: 50s
        #             - Pending Operational Dataset TLV
        #                 - Active Timestamp TLV <410s>
        #                 - Delay Timer TLV <~ 200s>
        #                 - Channel TLV : ‘Primary’
        #                 - PAN ID TLV : 0xABCD
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER_1).\
            filter(lambda p: {
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV,
                              ACTIVE_TIMESTAMP_TLV,
                              PENDING_TIMESTAMP_TLV,
                              PENDING_OPERATION_DATASET_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.mle.tlv.active_tstamp == ROUTER2_ACTIVE_TIMESTAMP and\
                   p.mle.tlv.pending_tstamp == ROUTER2_PENDING_TIMESTAMP and\
                   p.thread_meshcop.tlv.delay_timer < ROUTER2_DELAY_TIMER and\
                   p.thread_meshcop.tlv.active_tstamp == ROUTER2_PENDING_ACTIVE_TIMESTAMP and\
                   p.thread_meshcop.tlv.channel == [CHANNEL_INIT, CHANNEL_FINAL] and\
                   p.thread_meshcop.tlv.pan_id == [PANID_INIT, PANID_FINAL]
                   ).\
            must_next()

        # Step 36: The DUT MUST respond with an ICMPv6 Echo Reply
        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(ROUTER_2_MLEID, LEADER_MLEID).\
            filter_ipv6_dst(LEADER_MLEID).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(LEADER_MLEID, ROUTER_2_MLEID).\
            must_next()

        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(COMMISSIONER_MLEID, ROUTER_1_MLEID).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(ROUTER_1_MLEID, COMMISSIONER_MLEID).\
            must_next()


if __name__ == '__main__':
    unittest.main()
