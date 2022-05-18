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
from pktverify.consts import MLE_CHILD_ID_RESPONSE, MLE_CHILD_UPDATE_REQUEST, MLE_DATA_RESPONSE, MLE_DATA_REQUEST, MGMT_ACTIVE_SET_URI, MGMT_PENDING_SET_URI, LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS, TLV_REQUEST_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV, PENDING_OPERATION_DATASET_TLV, NM_COMMISSIONER_SESSION_ID_TLV, NM_BORDER_AGENT_LOCATOR_TLV, NM_ACTIVE_TIMESTAMP_TLV, NM_NETWORK_NAME_TLV, NM_NETWORK_KEY_TLV, NM_CHANNEL_TLV, NM_CHANNEL_MASK_TLV, NM_EXTENDED_PAN_ID_TLV, NM_NETWORK_MESH_LOCAL_PREFIX_TLV, NM_PAN_ID_TLV, NM_PSKC_TLV, NM_SECURITY_POLICY_TLV, NM_DELAY_TIMER_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.addrs import Ipv6Addr

KEY1 = '00112233445566778899aabbccddeeff'
KEY2 = 'ffeeddccbbaa99887766554433221100'

CHANNEL_INIT = 19
PANID_INIT = 0xface

COMMISSIONER = 1
LEADER = 2
ROUTER1 = 3
ED1 = 4
SED1 = 5

MTDS = [ED1, SED1]


class Cert_9_2_18_RollBackActiveTimestamp(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        COMMISSIONER: {
            'name': 'COMMISSIONER',
            'active_dataset': {
                'timestamp': 1,
                'panid': PANID_INIT,
                'channel': CHANNEL_INIT,
                'network_key': KEY1
            },
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        LEADER: {
            'name': 'LEADER',
            'active_dataset': {
                'timestamp': 1,
                'panid': PANID_INIT,
                'channel': CHANNEL_INIT,
                'network_key': KEY1
            },
            'mode': 'rdn',
            'partition_id': 0xffffffff,
            'allowlist': [COMMISSIONER, ROUTER1]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'active_dataset': {
                'timestamp': 1,
                'panid': PANID_INIT,
                'channel': CHANNEL_INIT,
                'network_key': KEY1
            },
            'mode': 'rdn',
            'allowlist': [LEADER, ED1, SED1]
        },
        ED1: {
            'name': 'ED',
            'channel': CHANNEL_INIT,
            'is_mtd': True,
            'networkkey': KEY1,
            'mode': 'rn',
            'panid': PANID_INIT,
            'allowlist': [ROUTER1]
        },
        SED1: {
            'name': 'SED',
            'channel': CHANNEL_INIT,
            'is_mtd': True,
            'networkkey': KEY1,
            'mode': '-',
            'panid': PANID_INIT,
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
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

        self.nodes[ED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ED1].get_state(), 'child')

        self.nodes[SED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED1].get_state(), 'child')

        self.nodes[COMMISSIONER].send_mgmt_active_set(active_timestamp=20000, network_name='GRL')
        self.simulator.go(5)

        self.nodes[COMMISSIONER].send_mgmt_pending_set(
            pending_timestamp=20,
            active_timestamp=20,
            delay_timer=20,
            network_name='Shouldnotbe',
        )
        self.simulator.go(30)

        self.nodes[COMMISSIONER].send_mgmt_pending_set(
            pending_timestamp=20,
            active_timestamp=20,
            delay_timer=300,
            network_name='MyHouse',
            network_key=KEY2,
        )
        self.simulator.go(310)

        self.assertEqual(self.nodes[COMMISSIONER].get_networkkey(), KEY2)
        self.assertEqual(self.nodes[LEADER].get_networkkey(), KEY2)
        self.assertEqual(self.nodes[ROUTER1].get_networkkey(), KEY2)
        self.assertEqual(self.nodes[ED1].get_networkkey(), KEY2)
        self.assertEqual(self.nodes[SED1].get_networkkey(), KEY2)

        self.collect_rlocs()
        ed_rloc = self.nodes[ED1].get_rloc()
        sed_rloc = self.nodes[SED1].get_rloc()
        self.assertTrue(self.nodes[COMMISSIONER].ping(ed_rloc))
        self.assertTrue(self.nodes[COMMISSIONER].ping(sed_rloc))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        COMMISSIONER = pv.vars['COMMISSIONER']
        ROUTER_1 = pv.vars['ROUTER_1']
        SED = pv.vars['SED']
        COMMISSIONER_RLOC = pv.vars['COMMISSIONER_RLOC']
        ED_RLOC = pv.vars['ED_RLOC']
        SED_RLOC = pv.vars['SED_RLOC']

        # Step 1: Ensure the topology is formed correctly
        pkts.filter_wpan_src64(ROUTER_1).filter_wpan_dst64(SED).filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next()

        # Step 3: Leader MUST send MGMT_ACTIVE_SET.rsp (Accept) to the Commissioner
        pkts.filter_wpan_src64(LEADER).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_ack(
            MGMT_ACTIVE_SET_URI).must_next().must_verify(lambda p: p.thread_meshcop.tlv.state == 1)

        # Step 5: Leader MUST send MGMT_PENDING_SET.rsp (Reject) to the Commissioner
        pkts.filter_wpan_src64(LEADER).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_ack(
            MGMT_PENDING_SET_URI).must_next().must_verify(lambda p: p.thread_meshcop.tlv.state == -1)

        # Step 7: Leader MUST send MGMT_PENDING_SET.rsp (Accept) to Commissioner
        pkts.filter_wpan_src64(LEADER).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_ack(
            MGMT_PENDING_SET_URI).must_next().must_verify(lambda p: p.thread_meshcop.tlv.state == 1)

        # Step 8: Leader MUST multicast a MLE Data Response to the Link-Local All Nodes multicast address
        _pkt = pkts.filter_wpan_src64(LEADER).filter_ipv6_dst(LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS).filter_mle_cmd(
            MLE_DATA_RESPONSE).must_next()
        _pkt.must_verify(lambda p: {
            SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV
        } == set(p.mle.tlv.type) and {NM_COMMISSIONER_SESSION_ID_TLV, NM_BORDER_AGENT_LOCATOR_TLV} <= set(
            p.thread_meshcop.tlv.type) and p.thread_nwd.tlv.stable == [0])

        # Step 9: Router MUST send a unicast MLE Data Request to the Leader
        pkts.filter_wpan_src64(ROUTER_1).filter_wpan_dst64(LEADER).filter_mle_cmd(MLE_DATA_REQUEST).must_next(
        ).must_verify(lambda p: {TLV_REQUEST_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV} <= set(p.mle.tlv.type))

        # Step 10: Leader MUST send a unicast MLE Data Response to Router_1
        pkts.filter_wpan_src64(LEADER).filter_wpan_dst64(ROUTER_1).filter_mle_cmd(
            MLE_DATA_RESPONSE).must_next().must_verify(
                lambda p: {
                    SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV,
                    PENDING_OPERATION_DATASET_TLV
                } == set(p.mle.tlv.type) and {
                    NM_COMMISSIONER_SESSION_ID_TLV, NM_BORDER_AGENT_LOCATOR_TLV, NM_ACTIVE_TIMESTAMP_TLV,
                    NM_NETWORK_NAME_TLV, NM_NETWORK_KEY_TLV
                } <= set(p.thread_meshcop.tlv.type) and p.thread_nwd.tlv.stable == [0])

        # Copy a pv.pkts here to filter SED related packets for potential sequence packets disorder
        _pkts_sed = pkts.copy()

        # Step 11: Router MUST multicast a MLE Data Response with the new information
        pkts.filter_wpan_src64(ROUTER_1).filter_ipv6_dst(LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS).filter_mle_cmd(
            MLE_DATA_RESPONSE).must_next().must_verify(
                lambda p: {
                    SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV
                } == set(p.mle.tlv.type) and p.mle.tlv.leader_data.data_version == _pkt.mle.tlv.leader_data.
                data_version and p.mle.tlv.leader_data.stable_data_version == _pkt.mle.tlv.leader_data.
                stable_data_version and {NM_COMMISSIONER_SESSION_ID_TLV, NM_BORDER_AGENT_LOCATOR_TLV} <= set(
                    p.thread_meshcop.tlv.type) and p.thread_nwd.tlv.stable == [0])

        # Step 12: Router MUST send MLE Child Update Request to SED_1
        pkts.filter_wpan_src64(ROUTER_1).filter_wpan_dst64(SED).filter_mle_cmd(
            MLE_CHILD_UPDATE_REQUEST).must_next().must_verify(lambda p: {
                SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV
            } == set(p.mle.tlv.type) and p.mle.tlv.leader_data.data_version == _pkt.mle.tlv.leader_data.data_version)

        # Step 13: SED MUST send a unicast MLE Data Request to Router_1
        _pkts_sed.filter_wpan_src64(SED).filter_wpan_dst64(ROUTER_1).filter_mle_cmd(MLE_DATA_REQUEST).must_next(
        ).must_verify(lambda p: {TLV_REQUEST_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV} <= set(p.mle.tlv.type))

        # Step 14: Router MUST send a unicast MLE Data Response to SED_1
        _pkts_sed.filter_wpan_src64(ROUTER_1).filter_wpan_dst64(SED).filter_mle_cmd(
            MLE_DATA_RESPONSE).must_next().must_verify(
                lambda p: {
                    SOURCE_ADDRESS_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV,
                    PENDING_OPERATION_DATASET_TLV
                } <= set(p.mle.tlv.type) and {
                    NM_CHANNEL_TLV, NM_NETWORK_MESH_LOCAL_PREFIX_TLV, NM_PAN_ID_TLV, NM_DELAY_TIMER_TLV,
                    NM_ACTIVE_TIMESTAMP_TLV, NM_NETWORK_NAME_TLV, NM_NETWORK_KEY_TLV
                } <= set(p.thread_meshcop.tlv.type) and p.thread_meshcop.tlv.net_name == ["MyHouse"] and p.
                thread_meshcop.tlv.master_key == KEY2)

        # Step 17: MED and SED MUST respond with an ICMPv6 Echo Reply
        pkts.filter_ipv6_src_dst(ED_RLOC, COMMISSIONER_RLOC).filter_ping_reply().must_next()
        pkts.filter_ipv6_src_dst(SED_RLOC, COMMISSIONER_RLOC).filter_ping_reply().must_next()


if __name__ == '__main__':
    unittest.main()
