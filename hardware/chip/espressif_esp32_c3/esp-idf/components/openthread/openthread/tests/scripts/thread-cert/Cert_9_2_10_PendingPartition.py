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
from pktverify.consts import MLE_ADVERTISEMENT, MLE_PARENT_REQUEST, MLE_DATA_REQUEST, MLE_DATA_RESPONSE, MLE_CHILD_UPDATE_REQUEST, MLE_CHILD_UPDATE_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, ADDR_SOL_URI, VERSION_TLV, TLV_REQUEST_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, CHALLENGE_TLV, LINK_MARGIN_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV, ACTIVE_OPERATION_DATASET_TLV, PENDING_OPERATION_DATASET_TLV, LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS, LINK_LOCAL_ALL_ROUTERS_MULTICAST_ADDRESS, NM_COMMISSIONER_SESSION_ID_TLV, NM_BORDER_AGENT_LOCATOR_TLV, NM_CHANNEL_TLV, NM_NETWORK_MESH_LOCAL_PREFIX_TLV, NM_PAN_ID_TLV, NM_DELAY_TIMER_TLV, NM_ACTIVE_TIMESTAMP_TLV
from pktverify.packet_verifier import PacketVerifier

CHANNEL_INIT = 19
PANID_INIT = 0xface

CHANNEL_FINAL = 16
PANID_FINAL = 0xafce

COMMISSIONER = 1
LEADER = 2
ROUTER1 = 3
ED1 = 4
SED1 = 5

MTDS = [ED1, SED1]


class Cert_9_2_10_PendingPartition(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        COMMISSIONER: {
            'name': 'COMMISSIONER',
            'active_dataset': {
                'timestamp': 15,
                'channel': 19
            },
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        LEADER: {
            'name': 'LEADER',
            'active_dataset': {
                'timestamp': 15,
                'channel': 19
            },
            'mode': 'rdn',
            'partition_id': 0xffffffff,
            'allowlist': [COMMISSIONER, ROUTER1]
        },
        ROUTER1: {
            'name': 'ROUTER',
            'active_dataset': {
                'timestamp': 15,
                'channel': 19
            },
            'mode': 'rdn',
            'allowlist': [LEADER, ED1, SED1]
        },
        ED1: {
            'name': 'MED',
            'channel': 19,
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [ROUTER1]
        },
        SED1: {
            'name': 'SED',
            'channel': 19,
            'is_mtd': True,
            'mode': '-',
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

        self.nodes[COMMISSIONER].send_mgmt_pending_set(
            pending_timestamp=30,
            active_timestamp=165,
            delay_timer=250,
            channel=CHANNEL_FINAL,
            panid=PANID_FINAL,
        )
        self.simulator.go(260)

        self.nodes[LEADER].remove_allowlist(self.nodes[ROUTER1].get_addr64())
        self.nodes[ROUTER1].remove_allowlist(self.nodes[LEADER].get_addr64())
        self.simulator.go(300)

        self.assertEqual(self.nodes[ROUTER1].get_state(), 'leader')
        self.assertEqual(self.nodes[ED1].get_state(), 'child')
        self.assertEqual(self.nodes[SED1].get_state(), 'child')

        self.assertEqual(self.nodes[ROUTER1].get_panid(), PANID_FINAL)
        self.assertEqual(self.nodes[ED1].get_panid(), PANID_FINAL)
        self.assertEqual(self.nodes[SED1].get_panid(), PANID_FINAL)

        self.assertEqual(self.nodes[ROUTER1].get_channel(), CHANNEL_FINAL)
        self.assertEqual(self.nodes[ED1].get_channel(), CHANNEL_FINAL)
        self.assertEqual(self.nodes[SED1].get_channel(), CHANNEL_FINAL)

        self.nodes[LEADER].add_allowlist(self.nodes[ROUTER1].get_addr64())
        self.nodes[ROUTER1].add_allowlist(self.nodes[LEADER].get_addr64())
        self.simulator.go(60)

        self.assertEqual(self.nodes[COMMISSIONER].get_state(), 'router')
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')
        self.assertEqual(self.nodes[ED1].get_state(), 'child')
        self.assertEqual(self.nodes[SED1].get_state(), 'child')

        ipaddrs = self.nodes[ED1].get_addrs()
        for ipaddr in ipaddrs:
            if ipaddr[0:4] != 'fe80':
                break

        self.assertTrue(self.nodes[LEADER].ping(ipaddr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']
        MED = pv.vars['MED']
        SED = pv.vars['SED']
        COMMISSIONER = pv.vars['COMMISSIONER']
        _rpkts = pkts.filter_wpan_src64(ROUTER, cascade=False)

        # Step 1: Ensure the topology is formed correctly
        _rpkts.filter_wpan_dst64(SED).filter_mle_cmd(MLE_CHILD_ID_RESPONSE).must_next()

        # Step 5: Router MUST send a unicast MLE Data Request to the Leader
        _rpkts.filter_wpan_dst64(LEADER).filter_mle_cmd(MLE_DATA_REQUEST).must_next().must_verify(
            lambda p: {TLV_REQUEST_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV} <= set(p.mle.tlv.type))
        _rpkts_med = _rpkts.copy()

        # Step 7: Router MUST multicast a MLE Data Response
        _rpkts.filter_ipv6_dst(LINK_LOCAL_ALL_NODES_MULTICAST_ADDRESS).filter_mle_cmd(
            MLE_DATA_RESPONSE).must_next().must_verify(lambda p: {
                SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV
            } <= set(p.mle.tlv.type) and {NM_COMMISSIONER_SESSION_ID_TLV, NM_BORDER_AGENT_LOCATOR_TLV} <= set(
                p.thread_meshcop.tlv.type) and p.thread_nwd.tlv.stable == [0])

        # Step 8: MED MUST send a unicast MLE Data Request to Router_1,
        with pkts.save_index():
            pkts.filter_wpan_src64(MED).filter_wpan_dst64(ROUTER).filter_mle_cmd(MLE_DATA_REQUEST).must_next(
            ).must_verify(lambda p: {TLV_REQUEST_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV} <= set(p.mle.tlv.type))

        # Step 9: Router MUST send a unicast MLE Data Response to MED_1
        _rpkts_med.filter_wpan_dst64(MED).filter_mle_cmd(MLE_DATA_RESPONSE).must_next().must_verify(lambda p: {
            SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_OPERATION_DATASET_TLV
        } <= set(p.mle.tlv.type) and {
            NM_CHANNEL_TLV, NM_NETWORK_MESH_LOCAL_PREFIX_TLV, NM_PAN_ID_TLV, NM_DELAY_TIMER_TLV,
            NM_ACTIVE_TIMESTAMP_TLV
        } <= set(p.thread_meshcop.tlv.type) and p.thread_nwd.tlv.stable == [0])

        # Step 10: Router MUST send MLE Child Update Request to SED_1
        _rpkts.range(pkts.index).filter_wpan_dst64(SED).filter_mle_cmd(
            MLE_CHILD_UPDATE_REQUEST).must_next().must_verify(lambda p: {
                SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV
            } <= set(p.mle.tlv.type))

        # Step 11: SED MUST send a unicast MLE Data Request to Router_1
        pkts.filter_wpan_src64(SED).filter_wpan_dst64(ROUTER).filter_mle_cmd(MLE_DATA_REQUEST).must_next().must_verify(
            lambda p: {TLV_REQUEST_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV} <= set(p.mle.tlv.type))

        # Step 12: Router MUST send a unicast MLE Data Response to SED_1
        _pkt = _rpkts.filter_wpan_dst64(SED).filter_mle_cmd(MLE_DATA_RESPONSE).must_next()
        _pkt.must_verify(lambda p: {
            SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_OPERATION_DATASET_TLV
        } <= set(p.mle.tlv.type) and {
            NM_CHANNEL_TLV, NM_NETWORK_MESH_LOCAL_PREFIX_TLV, NM_PAN_ID_TLV, NM_DELAY_TIMER_TLV,
            NM_ACTIVE_TIMESTAMP_TLV
        } <= set(p.thread_meshcop.tlv.type))

        # Step 14: After NETWORK_ID_TIMEOUT, Router MUST start a new partition
        _rpkts.filter_ipv6_dst(LINK_LOCAL_ALL_ROUTERS_MULTICAST_ADDRESS).filter_mle_cmd(
            MLE_PARENT_REQUEST).must_next().must_verify(lambda p: p.sniff_timestamp - _pkt.sniff_timestamp > 300)
        _rpkts.filter_mle_cmd(MLE_DATA_RESPONSE).filter(lambda p: p.wpan.dst_pan == PANID_FINAL).must_next()

        # Step 16: After the Delay Timer expires, Router MUST move to the Secondary channel
        _rpkts.filter_mle_cmd(MLE_ADVERTISEMENT).filter(lambda p: p.wpan.dst_pan == PANID_FINAL).must_next()

        # Step 19: Router MUST reattach to the Leader and the partitions MUST merge
        pkts.filter_wpan_src64(LEADER).filter_wpan_dst64(ROUTER).filter_mle_cmd(
            MLE_CHILD_ID_RESPONSE).must_next().must_verify(lambda p: p.mle.tlv.leader_data.partition_id == 0xffffffff)

        # Step 20: MED MUST respond with an ICMPv6 Echo Reply
        p = pkts.filter_ping_request().filter_wpan_src64(LEADER).must_next()
        pkts.filter_ping_reply(identifier=p.icmpv6.echo.identifier).filter_wpan_src64(MED).must_next()


if __name__ == '__main__':
    unittest.main()
