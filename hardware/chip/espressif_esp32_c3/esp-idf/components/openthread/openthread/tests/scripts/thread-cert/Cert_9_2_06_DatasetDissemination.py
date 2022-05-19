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

import command
import config
import mesh_cop
import thread_cert
from pktverify.consts import MLE_CHILD_ID_RESPONSE, MLE_CHILD_UPDATE_REQUEST, MLE_DATA_RESPONSE, MLE_DATA_REQUEST, MGMT_COMMISSIONER_SET_URI, MGMT_ACTIVE_SET_URI, MGMT_PENDING_SET_URI, TLV_REQUEST_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV, ACTIVE_OPERATION_DATASET_TLV, PENDING_OPERATION_DATASET_TLV, NM_COMMISSIONER_SESSION_ID_TLV, NM_BORDER_AGENT_LOCATOR_TLV, NM_ACTIVE_TIMESTAMP_TLV, NM_NETWORK_NAME_TLV, NM_NETWORK_KEY_TLV, NM_CHANNEL_TLV, NM_CHANNEL_MASK_TLV, NM_EXTENDED_PAN_ID_TLV, NM_NETWORK_MESH_LOCAL_PREFIX_TLV, NM_PAN_ID_TLV, NM_PSKC_TLV, NM_SECURITY_POLICY_TLV, NM_DELAY_TIMER_TLV, NM_STEERING_DATA_TLV, NWD_COMMISSIONING_DATA_TLV, LEADER_ALOC
from pktverify.packet_verifier import PacketVerifier
from pktverify.layer_fields import nullField

CHANNEL_INIT = 19
PANID_INIT = 0xface
TIMESTAMP_INIT = 10
CHANNEL_SECOND = 21

COMM_ACTIVE_TIMESTAMP = 15
COMM_ACTIVE_NET_NAME = 'Thread'
COMM_ACTIVE_PSKC = '10b95765596ab9d0b86cebdd0fa24da3'

COMM_PENDING_TIMESTAMP = 30
COMM_PENDING_ACTIVE_TIMESTAMP = 75
COMM_DELAY_TIMER = 60000

COMMISSIONER = 1
LEADER = 2
ROUTER = 3
MED = 4
SED = 5

# Test Purpose and Description:
# -----------------------------
# DUT as Leader:
# The purpose of this test case is to verify that the Leader device properly
# collects and disseminates Operational Datasets through a Thread network.
# DUT as Router:
# The purpose of this test case is to show that the Router device correctly
# sets the Commissioning information propagated by the Leader device and sends
# it properly to devices already attached to it.
# DUT as MED/SED:
# MED - requires full network data
# SED - requires only stable network data
# Set on Leader: Active TimeStamp = 10s
#
# Test Topology:
# -------------
#  Commissioner
#       |
#     Leader
#       |
#     Router
#      /  \
#   MED   SED
#
# DUT Types:
# ----------
#  Leader
#  Router
#  MED
#  SED


class Cert_9_2_06_DatasetDissemination(thread_cert.TestCase):
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
            'allowlist': [COMMISSIONER, ROUTER]
        },
        ROUTER: {
            'name': 'ROUTER',
            'active_dataset': {
                'timestamp': TIMESTAMP_INIT,
                'panid': PANID_INIT,
                'channel': CHANNEL_INIT
            },
            'mode': 'rdn',
            'allowlist': [LEADER, MED, SED]
        },
        MED: {
            'name': 'MED',
            'channel': CHANNEL_INIT,
            'is_mtd': True,
            'mode': 'rn',
            'panid': PANID_INIT,
            'allowlist': [ROUTER]
        },
        SED: {
            'name': 'SED',
            'channel': CHANNEL_INIT,
            'is_mtd': True,
            'mode': '-',
            'panid': PANID_INIT,
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
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
        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(3)

        leader_messages = self.simulator.get_messages_sent_by(LEADER)
        msg = leader_messages.next_coap_message('2.04', assert_enabled=True)
        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[MED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MED].get_state(), 'child')

        self.nodes[SED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED].get_state(), 'child')

        commissioner_session_id_tlv = command.get_sub_tlv(msg.coap.payload, mesh_cop.CommissionerSessionId)
        steering_data_tlv = mesh_cop.SteeringData(bytes([0xff]))
        # Step 2
        self.nodes[COMMISSIONER].commissioner_mgmtset_with_tlvs([steering_data_tlv, commissioner_session_id_tlv])
        self.simulator.go(10)

        # Step 7
        self.nodes[COMMISSIONER].send_mgmt_active_set(
            active_timestamp=COMM_ACTIVE_TIMESTAMP,
            network_name=COMM_ACTIVE_NET_NAME,
            binary='0410' + COMM_ACTIVE_PSKC,
        )
        self.simulator.go(10)

        # Step 18
        self.nodes[COMMISSIONER].send_mgmt_pending_set(
            pending_timestamp=COMM_PENDING_TIMESTAMP,
            active_timestamp=COMM_PENDING_ACTIVE_TIMESTAMP,
            delay_timer=COMM_DELAY_TIMER,
            channel=CHANNEL_SECOND,
        )
        self.simulator.go(120)

        self.collect_rlocs()
        ed_rloc = self.nodes[MED].get_rloc()
        sed_rloc = self.nodes[SED].get_rloc()
        leader_rloc = self.nodes[LEADER].get_rloc()
        router_rloc = self.nodes[ROUTER].get_rloc()
        for rloc in (leader_rloc, router_rloc, ed_rloc, sed_rloc):
            self.assertTrue(self.nodes[COMMISSIONER].ping(rloc))
            self.simulator.go(10)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        COMMISSIONER = pv.vars['COMMISSIONER']
        ROUTER = pv.vars['ROUTER']
        MED = pv.vars['MED']
        SED = pv.vars['SED']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        COMMISSIONER_RLOC = pv.vars['COMMISSIONER_RLOC']
        ROUTER_RLOC = pv.vars['ROUTER_RLOC']
        MED_RLOC = pv.vars['MED_RLOC']
        SED_RLOC = pv.vars['SED_RLOC']

        # Step 1: Ensure the topology is formed correctly
        for node in ('COMMISSIONER', 'ROUTER'):
            pv.verify_attached(node, 'LEADER')
        for node in ('MED', 'SED'):
            pv.verify_attached(node, 'ROUTER', 'MTD')
        _pkt = pkts.last()

        # Step 3: Leader sends MGMT_COMMISSIONER_SET.rsp to the Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             - State TLV (value = Accept)
        pkts.filter_coap_ack(MGMT_COMMISSIONER_SET_URI).\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(COMMISSIONER_RLOC).\
            must_next().\
            must_verify(lambda p: p.thread_meshcop.tlv.state == 1)

        # Step 4: Leader MUST multicast MLE Data Response to the Link-Local
        #         All Nodes multicast address (FF02::1) with the new information
        #         including the following TLVs:
        #             - Source Address TLV
        #             - Leader Data TLV:
        #                 Data Version field incremented
        #                 Stable Version field NOT incremented
        #             - Network Data TLV:
        #                 - Commissioner Data TLV:
        #                     Stable flag set to 0
        #                     Border Agent Locator TLV
        #                     Commissioner Session ID TLV
        #                     Steering Data TLV
        #             - Active Timestamp TLV
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE). \
            filter_wpan_src64(LEADER). \
            filter_LLANMA(). \
            filter(lambda p: p.mle.tlv.active_tstamp == TIMESTAMP_INIT and \
                             (p.mle.tlv.leader_data.data_version -
                              _pkt.mle.tlv.leader_data.data_version) % 256 <= 127 and \
                             p.mle.tlv.leader_data.stable_data_version ==
                             _pkt.mle.tlv.leader_data.stable_data_version and \
                             p.thread_nwd.tlv.stable == [0] and \
                             NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and \
                             NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and \
                             NM_STEERING_DATA_TLV in p.thread_meshcop.tlv.type and \
                             NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type
                   ). \
            must_next()
        idx_start = pkts.index

        # Step 5: Router MUST multicast MLE Data Response to the Link-Local
        #         All Nodes multicast address (FF02::1) with the new information
        #         including the following TLVs:
        #             - Source Address TLV
        #             - Leader Data TLV:
        #                 Data Version field incremented
        #                 Stable Version field NOT incremented
        #             - Network Data TLV:
        #                 - Commissioner Data TLV:
        #                     Stable flag set to 0
        #                     Border Agent Locator TLV
        #                     Commissioner Session ID TLV
        #                     Steering Data TLV
        #             - Active Timestamp TLV
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE). \
            filter_wpan_src64(ROUTER). \
            filter_LLANMA(). \
            filter(lambda p: p.mle.tlv.active_tstamp == TIMESTAMP_INIT and \
                             (p.mle.tlv.leader_data.data_version -
                              _pkt.mle.tlv.leader_data.data_version) % 256 <= 127 and \
                             p.mle.tlv.leader_data.stable_data_version ==
                             _pkt.mle.tlv.leader_data.stable_data_version and \
                             p.thread_nwd.tlv.stable == [0] and \
                             NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and \
                             NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and \
                             NM_STEERING_DATA_TLV in p.thread_meshcop.tlv.type and \
                             NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type
                   ). \
            must_next()

        # Step 8: Leader sends MGMT_ACTIVE_SET.rsp to the Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             - State TLV (value = Accept)
        pkts.filter_coap_ack(MGMT_ACTIVE_SET_URI). \
            filter_wpan_src64(LEADER). \
            filter_ipv6_dst(COMMISSIONER_RLOC). \
            must_next(). \
            must_verify(lambda p: p.thread_meshcop.tlv.state == 1)
        idx_end = pkts.index

        # Step 6: Router MUST NOT send a unicast MLE Data Response or MLE Child
        #         Update Request to SED_1
        pkts.range(idx_start, idx_end).\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(SED).\
            must_not_next()

        # Step 9: Leader MUST multicast MLE Data Response to the Link-Local
        #         All Nodes multicast address (FF02::1) with the new information
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
        #                     Steering Data TLV
        #             - Active Timestamp TLV: 15s
        _pkt9 = pkts.filter_mle_cmd(MLE_DATA_RESPONSE). \
            filter_wpan_src64(LEADER). \
            filter_LLANMA(). \
            filter(lambda p: p.mle.tlv.active_tstamp == COMM_ACTIVE_TIMESTAMP and \
                             (p.mle.tlv.leader_data.data_version -
                              _pkt.mle.tlv.leader_data.data_version) % 256 <= 127 and \
                             (p.mle.tlv.leader_data.stable_data_version -
                              _pkt.mle.tlv.leader_data.stable_data_version) % 256 <= 127 and \
                             p.thread_nwd.tlv.stable == [0] and \
                             NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and \
                             NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and \
                             NM_STEERING_DATA_TLV in p.thread_meshcop.tlv.type and \
                             NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type
                   ). \
            must_next()

        # Step 10: Router MUST send a unicast MLE Data Request to the Leader, including the
        #          following TLVs:
        #              - TLV Request TLV:
        #                 - Network Data TLV
        #              - Active Timestamp TLV
        pkts.filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(LEADER).\
            filter_mle_cmd(MLE_DATA_REQUEST).\
            filter(lambda p: {
                              TLV_REQUEST_TLV,
                              NETWORK_DATA_TLV,
                              ACTIVE_TIMESTAMP_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.thread_meshcop.tlv.type is nullField
                   ).\
            must_next()

        # Step 11: Leader sends a MLE Data Response to Router_1 including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV
        #                  Data version numbers should be
        #                  the same as the ones sent in the
        #                  multicast data response in step 9
        #              - Network Data TLV
        #                  - Commissioner Data TLV:
        #                      Stable flag set to 0
        #                      Border Agent Locator TLV
        #                      Commissioner Session ID TLV
        #                      Steering Data TLV
        #              - Active Timestamp TLV: <new value>
        #              - Active Operational Dataset TLV
        #                  - Network Name TLV <new value>
        #                  - Channel TLV
        #                  - PAN ID TLV
        #                  - PSKc TLV
        #                  MUST NOT contain the Active Timestamp TLV
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE). \
            filter_wpan_src64(LEADER). \
            filter_wpan_dst64(ROUTER). \
            filter(lambda p: {
                                 SOURCE_ADDRESS_TLV,
                                 LEADER_DATA_TLV,
                                 ACTIVE_TIMESTAMP_TLV,
                                 ACTIVE_OPERATION_DATASET_TLV
                             } <= set(p.mle.tlv.type) and \
                             {
                                 NM_COMMISSIONER_SESSION_ID_TLV,
                                 NM_BORDER_AGENT_LOCATOR_TLV,
                                 NM_STEERING_DATA_TLV
                             } <= set(p.thread_meshcop.tlv.type) and \
                             p.mle.tlv.leader_data.data_version ==
                             _pkt9.mle.tlv.leader_data.data_version and \
                             p.mle.tlv.leader_data.stable_data_version ==
                             _pkt9.mle.tlv.leader_data.stable_data_version and \
                             NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and \
                             NM_ACTIVE_TIMESTAMP_TLV not in p.thread_meshcop.tlv.type and \
                             p.thread_nwd.tlv.stable == [0] and \
                             p.mle.tlv.active_tstamp == COMM_ACTIVE_TIMESTAMP and \
                             p.thread_meshcop.tlv.channel == [CHANNEL_INIT] and \
                             p.thread_meshcop.tlv.net_name == [COMM_ACTIVE_NET_NAME] and \
                             p.thread_meshcop.tlv.pskc == COMM_ACTIVE_PSKC and \
                             p.thread_meshcop.tlv.pan_id == [PANID_INIT]
                   ). \
            must_next()

        # Step 12: Router MUST multicast MLE Data Response to the Link-Local
        #          All Nodes multicast address (FF02::1) with the new information
        #          including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV:
        #                  Data version numbers should be
        #                  the same as the ones sent in the
        #                  multicast data response in step 9
        #              - Network Data TLV:
        #                  - Commissioner Data TLV:
        #                      Stable flag set to 0
        #                      Border Agent Locator TLV
        #                      Commissioner Session ID TLV
        #                      Steering Data TLV
        #              - Active Timestamp TLV: 15s
        with pkts.save_index():
            pkts.filter_mle_cmd(MLE_DATA_RESPONSE). \
                filter_wpan_src64(ROUTER). \
                filter_LLANMA(). \
                filter(lambda p: p.mle.tlv.active_tstamp == COMM_ACTIVE_TIMESTAMP and \
                                 p.mle.tlv.leader_data.data_version ==
                                 _pkt9.mle.tlv.leader_data.data_version and \
                                 p.mle.tlv.leader_data.stable_data_version ==
                                 _pkt9.mle.tlv.leader_data.stable_data_version and \
                                 p.thread_nwd.tlv.stable == [0] and \
                                 NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and \
                                 NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and \
                                 NM_STEERING_DATA_TLV in p.thread_meshcop.tlv.type and \
                                 NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type
                       ). \
                must_next()

        # Step 13: MED MUST send a unicast MLE Data Request to the Router, including the
        #          following TLVs:
        #              - TLV Request TLV:
        #                 - Network Data TLV
        #              - Active Timestamp TLV

        # Step 14: Router sends a MLE Data Response to MED including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV
        #                  Data version numbers should be
        #                  the same as the ones sent in the
        #                  multicast data response in step 9
        #              - Network Data TLV
        #                  - Commissioner Data TLV:
        #                      Stable flag set to 0
        #                      Border Agent Locator TLV
        #                      Commissioner Session ID TLV
        #                      Steering Data TLV
        #              - Active Timestamp TLV: <new value>
        #              - Active Operational Dataset TLV
        #                  - Network Name TLV <new value>
        #                  - Channel TLV
        #                  - PAN ID TLV
        #                  MUST NOT contain the Active Timestamp TLV
        with pkts.save_index():
            pkts.filter_wpan_src64(MED). \
                filter_wpan_dst64(ROUTER). \
                filter_mle_cmd(MLE_DATA_REQUEST). \
                filter(lambda p: {
                                     TLV_REQUEST_TLV,
                                     NETWORK_DATA_TLV,
                                     ACTIVE_TIMESTAMP_TLV
                                 } <= set(p.mle.tlv.type) and \
                                 p.thread_meshcop.tlv.type is nullField
                       ). \
                must_next()

            pkts.filter_mle_cmd(MLE_DATA_RESPONSE). \
                filter_wpan_src64(ROUTER). \
                filter_wpan_dst64(MED). \
                filter(lambda p: {
                                     SOURCE_ADDRESS_TLV,
                                     LEADER_DATA_TLV,
                                     ACTIVE_TIMESTAMP_TLV,
                                     ACTIVE_OPERATION_DATASET_TLV
                                 } <= set(p.mle.tlv.type) and \
                                 {
                                     NM_COMMISSIONER_SESSION_ID_TLV,
                                     NM_BORDER_AGENT_LOCATOR_TLV,
                                     NM_STEERING_DATA_TLV
                                 } <= set(p.thread_meshcop.tlv.type) and \
                                 p.mle.tlv.leader_data.data_version ==
                                 _pkt9.mle.tlv.leader_data.data_version and \
                                 p.mle.tlv.leader_data.stable_data_version ==
                                 _pkt9.mle.tlv.leader_data.stable_data_version and \
                                 NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and \
                                 NM_ACTIVE_TIMESTAMP_TLV not in p.thread_meshcop.tlv.type and \
                                 p.thread_nwd.tlv.stable == [0] and \
                                 p.mle.tlv.active_tstamp == COMM_ACTIVE_TIMESTAMP and \
                                 p.thread_meshcop.tlv.channel == [CHANNEL_INIT] and \
                                 p.thread_meshcop.tlv.net_name == [COMM_ACTIVE_NET_NAME] and \
                                 p.thread_meshcop.tlv.pskc == COMM_ACTIVE_PSKC and \
                                 p.thread_meshcop.tlv.pan_id == [PANID_INIT]
                       ). \
                must_next()

        # Step 15: Router MUST send MLE Child Update Request or MLE Data Response
        #          to SED, including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV:
        #                  Data version numbers should be
        #                  the same as the ones sent in the
        #                  multicast data response in step 9
        #              - Network Data TLV
        #              - Active Timestamp TLV: 15s
        pkts.filter_wpan_src64(ROUTER). \
            filter_wpan_dst64(SED). \
            filter_mle_cmd2(MLE_CHILD_UPDATE_REQUEST, MLE_DATA_RESPONSE). \
            filter(lambda p: p.mle.tlv.active_tstamp == COMM_ACTIVE_TIMESTAMP and \
                             p.mle.tlv.leader_data.data_version ==
                             _pkt9.mle.tlv.leader_data.data_version and \
                             p.mle.tlv.leader_data.stable_data_version ==
                             _pkt9.mle.tlv.leader_data.stable_data_version and \
                             NETWORK_DATA_TLV in p.mle.tlv.type and\
                             SOURCE_ADDRESS_TLV in p.mle.tlv.type
                   ). \
            must_next()

        # Step 16: SED MUST send a unicast MLE Data Request to the Router, including the
        #          following TLVs:
        #              - TLV Request TLV:
        #                 - Network Data TLV
        #              - Active Timestamp TLV
        pkts.filter_wpan_src64(SED). \
            filter_wpan_dst64(ROUTER). \
            filter_mle_cmd(MLE_DATA_REQUEST). \
            filter(lambda p: {
                                 TLV_REQUEST_TLV,
                                 NETWORK_DATA_TLV,
                                 ACTIVE_TIMESTAMP_TLV
                             } <= set(p.mle.tlv.type) and \
                             p.thread_meshcop.tlv.type is nullField
                   ). \
            must_next()

        # Step 17: Router sends a MLE Data Response to SED including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV
        #                  Data version numbers should be
        #                  the same as the ones sent in the
        #                  multicast data response in step 9
        #              - Network Data TLV
        #              - Active Timestamp TLV: <15s>
        #              - Active Operational Dataset TLV
        #                  - Network Name TLV <new value>
        #                  - Channel TLV
        #                  - PAN ID TLV
        #                  MUST NOT contain the Active Timestamp TLV
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE). \
            filter_wpan_src64(ROUTER). \
            filter_wpan_dst64(SED). \
            filter(lambda p: {
                                 SOURCE_ADDRESS_TLV,
                                 LEADER_DATA_TLV,
                                 ACTIVE_TIMESTAMP_TLV,
                                 ACTIVE_OPERATION_DATASET_TLV
                             } <= set(p.mle.tlv.type) and \
                             p.mle.tlv.leader_data.data_version ==
                             _pkt9.mle.tlv.leader_data.data_version and \
                             p.mle.tlv.leader_data.stable_data_version ==
                             _pkt9.mle.tlv.leader_data.stable_data_version and \
                             NM_ACTIVE_TIMESTAMP_TLV not in p.thread_meshcop.tlv.type and \
                             p.mle.tlv.active_tstamp == COMM_ACTIVE_TIMESTAMP and \
                             p.thread_meshcop.tlv.channel == [CHANNEL_INIT] and \
                             p.thread_meshcop.tlv.net_name == [COMM_ACTIVE_NET_NAME] and \
                             p.thread_meshcop.tlv.pskc == COMM_ACTIVE_PSKC and \
                             p.thread_meshcop.tlv.pan_id == [PANID_INIT]
                   ). \
            must_next()

        # Step 19: Leader sends MGMT_PENDING_SET.rsp to the Commissioner:
        #          CoAP Response Code
        #              2.04 Changed
        #          CoAP Payload
        #              - State TLV (value = Accept)
        pkts.filter_coap_ack(MGMT_PENDING_SET_URI). \
            filter_wpan_src64(LEADER). \
            filter_ipv6_dst(COMMISSIONER_RLOC). \
            must_next(). \
            must_verify(lambda p: p.thread_meshcop.tlv.state == 1)

        # Step 20: Leader MUST multicast MLE Data Response to the Link-Local
        #          All Nodes multicast address (FF02::1) with the new information
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
        #                      Steering Data TLV
        #              - Active Timestamp TLV
        #              - Pending Timestamp TLV
        _pkt20 = pkts.filter_mle_cmd(MLE_DATA_RESPONSE). \
            filter_wpan_src64(LEADER). \
            filter_LLANMA(). \
            filter(lambda p: {
                                 SOURCE_ADDRESS_TLV,
                                 LEADER_DATA_TLV,
                                 ACTIVE_TIMESTAMP_TLV,
                                 PENDING_TIMESTAMP_TLV
                             } <= set(p.mle.tlv.type) and \
                             {
                                 NM_COMMISSIONER_SESSION_ID_TLV,
                                 NM_BORDER_AGENT_LOCATOR_TLV,
                                 NM_STEERING_DATA_TLV
                             } <= set(p.thread_meshcop.tlv.type) and \
                             (p.mle.tlv.leader_data.data_version -
                              _pkt9.mle.tlv.leader_data.data_version) % 256 <= 127 and \
                             (p.mle.tlv.leader_data.stable_data_version -
                              _pkt9.mle.tlv.leader_data.stable_data_version) % 256 <= 127 and \
                             p.thread_nwd.tlv.stable == [0] and \
                             NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type
                   ). \
            must_next()

        # Step 21: Router MUST send a unicast MLE Data Request to the Leader, including the
        #          following TLVs:
        #              - TLV Request TLV:
        #                 - Network Data TLV
        #              - Active Timestamp TLV
        pkts.filter_wpan_src64(ROUTER). \
            filter_wpan_dst64(LEADER). \
            filter_mle_cmd(MLE_DATA_REQUEST). \
            filter(lambda p: {
                                 TLV_REQUEST_TLV,
                                 NETWORK_DATA_TLV,
                                 ACTIVE_TIMESTAMP_TLV
                             } <= set(p.mle.tlv.type) and \
                             p.thread_meshcop.tlv.type is nullField
                   ). \
            must_next()

        # Step 22: Leader sends a MLE Data Response to Router including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV
        #              - Network Data TLV
        #                  - Commissioner Data TLV:
        #                      Stable flag set to 0
        #                      Border Agent Locator TLV
        #                      Commissioner Session ID TLV
        #                      Steering Data TLV
        #              - Active Timestamp TLV
        #              - Pending Timestamp TLV
        #              - Pending Operational Dataset TLV

        pkts.filter_mle_cmd(MLE_DATA_RESPONSE). \
            filter_wpan_src64(LEADER). \
            filter_wpan_dst64(ROUTER). \
            filter(lambda p: {
                                 SOURCE_ADDRESS_TLV,
                                 LEADER_DATA_TLV,
                                 ACTIVE_TIMESTAMP_TLV,
                                 PENDING_TIMESTAMP_TLV,
                                 PENDING_OPERATION_DATASET_TLV
                             } <= set(p.mle.tlv.type) and \
                             {
                                 NM_COMMISSIONER_SESSION_ID_TLV,
                                 NM_BORDER_AGENT_LOCATOR_TLV,
                                 NM_STEERING_DATA_TLV
                             } <= set(p.thread_meshcop.tlv.type) and \
                             p.thread_nwd.tlv.stable == [0] and \
                             NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and \
                             p.mle.tlv.active_tstamp == COMM_ACTIVE_TIMESTAMP and \
                             p.mle.tlv.pending_tstamp == COMM_PENDING_TIMESTAMP and \
                             p.thread_meshcop.tlv.delay_timer < COMM_DELAY_TIMER and \
                             p.thread_meshcop.tlv.active_tstamp == COMM_PENDING_ACTIVE_TIMESTAMP and \
                             p.thread_meshcop.tlv.channel == [CHANNEL_SECOND] and \
                             p.thread_meshcop.tlv.pan_id == [PANID_INIT]
                   ). \
            must_next()

        # Step 23: Router MUST multicast MLE Data Response to the Link-Local
        #          All Nodes multicast address (FF02::1) with the new information
        #          including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV:
        #                  Data version numbers should be
        #                  the same as the ones sent in the
        #                  multicast data response in step 20
        #              - Network Data TLV:
        #                  - Commissioner Data TLV:
        #                      Stable flag set to 0
        #                      Border Agent Locator TLV
        #                      Commissioner Session ID TLV
        #                      Steering Data TLV
        #              - Active Timestamp TLV: 15s
        #              - Pending Timestamp TLV: 30s
        with pkts.save_index():
            pkts.filter_mle_cmd(MLE_DATA_RESPONSE). \
                filter_wpan_src64(ROUTER). \
                filter_LLANMA(). \
                filter(lambda p: {
                                     NM_COMMISSIONER_SESSION_ID_TLV,
                                     NM_BORDER_AGENT_LOCATOR_TLV,
                                     NM_STEERING_DATA_TLV
                                 } <= set(p.thread_meshcop.tlv.type) and \
                                 p.mle.tlv.active_tstamp == COMM_ACTIVE_TIMESTAMP and \
                                 p.mle.tlv.pending_tstamp == COMM_PENDING_TIMESTAMP and \
                                 p.mle.tlv.leader_data.data_version ==
                                 _pkt20.mle.tlv.leader_data.data_version and \
                                 p.mle.tlv.leader_data.stable_data_version ==
                                 _pkt20.mle.tlv.leader_data.stable_data_version and \
                                 p.thread_nwd.tlv.stable == [0] and \
                                 NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and \
                                 NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and \
                                 NM_STEERING_DATA_TLV in p.thread_meshcop.tlv.type and \
                                 NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type
                       ). \
                must_next()

        # Step 24: MED MUST send a unicast MLE Data Request to the Router, including the
        #          following TLVs:
        #              - TLV Request TLV:
        #                 - Network Data TLV
        #              - Active Timestamp TLV

        # Step 25: Router sends a MLE Data Response to MED including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV
        #                  Data version numbers should be
        #                  the same as the ones sent in the
        #                  multicast data response in step 20
        #              - Network Data TLV
        #                  - Commissioner Data TLV:
        #                      Stable flag set to 0
        #                      Border Agent Locator TLV
        #                      Commissioner Session ID TLV
        #                      Steering Data TLV
        #              - Active Timestamp TLV
        #              - Pending Timestamp TLV
        #              - Pending Operational Dataset TLV
        #                  - Network Name TLV
        #                  - Channel TLV
        #                  - PAN ID TLV
        with pkts.save_index():
            pkts.filter_wpan_src64(MED). \
                filter_wpan_dst64(ROUTER). \
                filter_mle_cmd(MLE_DATA_REQUEST). \
                filter(lambda p: {
                                     TLV_REQUEST_TLV,
                                     NETWORK_DATA_TLV,
                                     ACTIVE_TIMESTAMP_TLV
                                 } <= set(p.mle.tlv.type) and \
                                 p.thread_meshcop.tlv.type is nullField
                       ). \
                must_next()

            pkts.filter_mle_cmd(MLE_DATA_RESPONSE). \
                filter_wpan_src64(ROUTER). \
                filter_wpan_dst64(MED). \
                filter(lambda p: {
                                     SOURCE_ADDRESS_TLV,
                                     LEADER_DATA_TLV,
                                     ACTIVE_TIMESTAMP_TLV,
                                     PENDING_TIMESTAMP_TLV,
                                     PENDING_OPERATION_DATASET_TLV
                                 } <= set(p.mle.tlv.type) and \
                                 {
                                     NM_COMMISSIONER_SESSION_ID_TLV,
                                     NM_BORDER_AGENT_LOCATOR_TLV,
                                     NM_STEERING_DATA_TLV
                                 } <= set(p.thread_meshcop.tlv.type) and \
                                 p.mle.tlv.leader_data.data_version ==
                                 _pkt20.mle.tlv.leader_data.data_version and \
                                 p.mle.tlv.leader_data.stable_data_version ==
                                 _pkt20.mle.tlv.leader_data.stable_data_version and \
                                 p.thread_nwd.tlv.stable == [0]
                       ). \
                must_next()

        # Step 26: Router MUST send MLE Child Update Request or MLE Data Response
        #          to SED, including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV:
        #                  Data version numbers should be
        #                  the same as the ones sent in the
        #                  multicast data response in step 20
        #              - Network Data TLV
        #              - Active Timestamp TLV: 15s
        #              - Pending Timestamp TLV: 30s
        pkts.filter_wpan_src64(ROUTER). \
            filter_wpan_dst64(SED). \
            filter_mle_cmd2(MLE_CHILD_UPDATE_REQUEST, MLE_DATA_RESPONSE). \
            filter(lambda p: p.mle.tlv.active_tstamp ==
                             COMM_ACTIVE_TIMESTAMP and \
                             p.mle.tlv.pending_tstamp ==
                             COMM_PENDING_TIMESTAMP and \
                             p.mle.tlv.leader_data.data_version ==
                             _pkt20.mle.tlv.leader_data.data_version and \
                             p.mle.tlv.leader_data.stable_data_version ==
                             _pkt20.mle.tlv.leader_data.stable_data_version and \
                             NETWORK_DATA_TLV in p.mle.tlv.type
                   ). \
            must_next()

        # Step 27: SED MUST send a unicast MLE Data Request to the Router, including the
        #          following TLVs:
        #              - TLV Request TLV:
        #                 - Network Data TLV
        #              - Active Timestamp TLV
        pkts.filter_wpan_src64(SED). \
            filter_wpan_dst64(ROUTER). \
            filter_mle_cmd(MLE_DATA_REQUEST). \
            filter(lambda p: {
                                 TLV_REQUEST_TLV,
                                 NETWORK_DATA_TLV,
                                 ACTIVE_TIMESTAMP_TLV
                             } <= set(p.mle.tlv.type) and \
                             p.thread_meshcop.tlv.type is nullField
                   ). \
            must_next()

        # Step 28: Router sends a MLE Data Response to SED including the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV
        #                  Data version numbers should be
        #                  the same as the ones sent in the
        #                  multicast data response in step 20
        #              - Network Data TLV
        #              - Active Timestamp TLV: 15s
        #              - Pending Timestamp TLV: 30s
        #              - Pending Operational Dataset TLV
        #                  - Network Name TLV
        #                  - Channel TLV
        #                  - PAN ID TLV
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE). \
            filter_wpan_src64(ROUTER). \
            filter_wpan_dst64(SED). \
            filter(lambda p: {
                                 SOURCE_ADDRESS_TLV,
                                 LEADER_DATA_TLV,
                                 ACTIVE_TIMESTAMP_TLV,
                                 PENDING_TIMESTAMP_TLV,
                                 PENDING_OPERATION_DATASET_TLV
                             } <= set(p.mle.tlv.type) and \
                             p.mle.tlv.leader_data.data_version ==
                             _pkt20.mle.tlv.leader_data.data_version and \
                             p.mle.tlv.leader_data.stable_data_version ==
                             _pkt20.mle.tlv.leader_data.stable_data_version and \
                             p.mle.tlv.active_tstamp == COMM_ACTIVE_TIMESTAMP and \
                             p.mle.tlv.pending_tstamp == COMM_PENDING_TIMESTAMP
                   ). \
            must_next()

        # Step 30: The DUT MUST respond with an ICMPv6 Echo Reply
        for DUT_RLOC in (LEADER_RLOC, ROUTER_RLOC, MED_RLOC, SED_RLOC):
            _pkt = pkts.filter_ping_request().\
                filter_ipv6_src_dst(COMMISSIONER_RLOC, DUT_RLOC).\
                must_next()
            pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
                filter_ipv6_src_dst(DUT_RLOC, COMMISSIONER_RLOC).\
                must_next()


if __name__ == '__main__':
    unittest.main()
