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
from pktverify.consts import MLE_PARENT_REQUEST, MLE_DATA_RESPONSE, MLE_DATA_REQUEST, MGMT_PENDING_SET_URI, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ACTIVE_OPERATION_DATASET_TLV, ACTIVE_TIMESTAMP_TLV, PENDING_TIMESTAMP_TLV, TLV_REQUEST_TLV, NETWORK_DATA_TLV, NM_BORDER_AGENT_LOCATOR_TLV, NM_COMMISSIONER_SESSION_ID_TLV, NM_DELAY_TIMER_TLV, PENDING_OPERATION_DATASET_TLV, NWD_COMMISSIONING_DATA_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField

KEY1 = '00112233445566778899aabbccddeeff'
KEY2 = 'ffeeddccbbaa99887766554433221100'

CHANNEL_INIT = 19
PANID_INIT = 0xface

COMMISSIONER = 1
LEADER = 2
ROUTER1 = 3
ED1 = 4
SED1 = 5

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to confirm the DUT correctly applies
# DELAY_TIMER_DEFAULT when the network key is changed.
# The Commissioner first tries to set a network key update to happen too
# soon (delay of 60s vs DELAY_TIMER_DEFAULT of 300s); the DUT is expected
# to override the short value and communicate an appropriately longer delay
# to the Router.
# The Commissioner then sets a delay time longer than default; the DUT is
# validated to not artificially clamp the longer time back to the
# DELAY_TIMER_DEFAULT value.
#
# Test Topology:
# -------------
#  Commissioner
#       |
#     Leader
#       |
#     Router
#    /     \
#  ED      SED
#
# DUT Types:
# ----------
# Leader


class Cert_9_2_11_NetworkKey(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        COMMISSIONER: {
            'name': 'COMMISSIONER',
            'active_dataset': {
                'timestamp': 10,
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
                'timestamp': 10,
                'panid': PANID_INIT,
                'channel': CHANNEL_INIT,
                'network_key': KEY1
            },
            'mode': 'rdn',
            'allowlist': [COMMISSIONER, ROUTER1]
        },
        ROUTER1: {
            'name': 'ROUTER',
            'active_dataset': {
                'timestamp': 10,
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

        self.collect_rlocs()
        self.collect_ipaddrs()

        self.nodes[COMMISSIONER].send_mgmt_pending_set(
            pending_timestamp=10,
            active_timestamp=70,
            delay_timer=60000,
            network_key=KEY2,
        )
        self.simulator.go(310)

        self.assertEqual(self.nodes[COMMISSIONER].get_networkkey(), KEY2)
        self.assertEqual(self.nodes[LEADER].get_networkkey(), KEY2)
        self.assertEqual(self.nodes[ROUTER1].get_networkkey(), KEY2)
        self.assertEqual(self.nodes[ED1].get_networkkey(), KEY2)
        self.assertEqual(self.nodes[SED1].get_networkkey(), KEY2)

        ipaddr = self.nodes[LEADER].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        self.assertTrue(self.nodes[ROUTER1].ping(ipaddr))

        self.nodes[COMMISSIONER].send_mgmt_pending_set(
            pending_timestamp=20,
            active_timestamp=30,
            delay_timer=500000,
            network_key=KEY1,
        )
        self.simulator.go(510)

        self.assertEqual(self.nodes[COMMISSIONER].get_networkkey(), KEY1)
        self.assertEqual(self.nodes[LEADER].get_networkkey(), KEY1)
        self.assertEqual(self.nodes[ROUTER1].get_networkkey(), KEY1)
        self.assertEqual(self.nodes[ED1].get_networkkey(), KEY1)
        self.assertEqual(self.nodes[SED1].get_networkkey(), KEY1)

        ipaddr = self.nodes[LEADER].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        self.assertTrue(self.nodes[ROUTER1].ping(ipaddr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_MLEID = pv.vars['LEADER_MLEID']
        COMMISSIONER = pv.vars['COMMISSIONER']
        COMMISSIONER_RLOC = pv.vars['COMMISSIONER_RLOC']
        ROUTER = pv.vars['ROUTER']
        ROUTER_MLEID = pv.vars['ROUTER_MLEID']
        ED = pv.vars['ED']
        SED = pv.vars['SED']

        # Step 1: Ensure the topology is formed correctly
        for node in ('COMMISSIONER', 'ROUTER'):
            pv.verify_attached(node, 'LEADER')
        for node in ('ED', 'SED'):
            pv.verify_attached(node, 'ROUTER', 'MTD')
        _pkt = pkts.last()

        # Step 3: Leader sends MGMT_PENDING_SET.rsq to the Commissioner:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             - State TLV (value = Accept)
        #
        #         Leader MUST multicast MLE Data Response with the new network data,
        #         including the following TLVs:
        #             - Leader Data TLV:
        #                 Data Version field incremented
        #                 Stable Version field incremented
        #             - Network Data TLV:
        #                 - Commissioner Data TLV:
        #                     Stable flag set to 0
        #                     Border Agent Locator TLV
        #                     Commissioner Session ID TLV
        #             - Active Timestamp TLV: 10s
        #             - Pending Timestamp TLV: 20s
        pkts.filter_coap_ack(MGMT_PENDING_SET_URI).\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(COMMISSIONER_RLOC).\
            must_next().\
            must_verify(lambda p: p.thread_meshcop.tlv.state == 1)
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter(lambda p: p.mle.tlv.active_tstamp == 10 and\
                   p.mle.tlv.pending_tstamp == 10 and\
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

        # Step 5: Leader sends a MLE Data Response to Router including the following TLVs:
        #             - Source Address TLV
        #             - Leader Data TLV
        #             - Network Data TLV
        #                 - Commissioner Data TLV:
        #                     Stable flag set to 0
        #                     Border Agent Locator TLV
        #                     Commissioner Session ID TLV
        #             - Active Timestamp TLV
        #             - Pending Timestamp TLV
        #             - Pending Operational Dataset TLV
        #                 - Delay Timer TLV <greater than 200s>
        #                 - Network Key TLV: New Network Key
        #                 - Active Timestamp TLV <70s>
        _dr_pkt = pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
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
                   p.thread_meshcop.tlv.delay_timer > 200000 and\
                   p.thread_meshcop.tlv.master_key == KEY2 and\
                   p.thread_meshcop.tlv.active_tstamp == 70
                   ).\
            must_next()

        # Step 8: Verify all devices now use New Network key.
        #  checked in test()

        # Step 9: Verify new MAC key is generated and used when sending ICMPv6 Echo Reply
        #         is received.
        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(ROUTER).\
            filter_ipv6_dst(LEADER_MLEID).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(ROUTER_MLEID).\
            must_next()

        # Step 11: Leader sends MGMT_PENDING_SET.rsq to the Commissioner:
        #          CoAP Response Code
        #              2.04 Changed
        #          CoAP Payload
        #              - State TLV (value = Accept)
        #
        #          Leader MUST multicast MLE Data Response with the new network data,
        #          including the following TLVs:
        #              - Leader Data TLV:
        #                  Data Version field incremented
        #                  Stable Version field incremented
        #              - Network Data TLV:
        #                  - Commissioner Data TLV:
        #                      Stable flag set to 0
        #                      Border Agent Locator TLV
        #                      Commissioner Session ID TLV
        #              - Active Timestamp TLV: 70s
        #              - Pending Timestamp TLV: 20s
        pkts.filter_coap_ack(MGMT_PENDING_SET_URI).\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(COMMISSIONER_RLOC).\
            must_next().\
            must_verify(lambda p: p.thread_meshcop.tlv.state == 1)
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter(lambda p: p.mle.tlv.active_tstamp == 70 and\
                   p.mle.tlv.pending_tstamp == 20 and\
                   (p.mle.tlv.leader_data.data_version -
                   _dr_pkt.mle.tlv.leader_data.data_version) % 256 <= 127 and\
                   (p.mle.tlv.leader_data.stable_data_version -
                   _dr_pkt.mle.tlv.leader_data.stable_data_version) % 256 <= 127 and\
                   p.thread_nwd.tlv.stable == [0] and\
                   NWD_COMMISSIONING_DATA_TLV in p.thread_nwd.tlv.type and\
                   NM_COMMISSIONER_SESSION_ID_TLV in p.thread_meshcop.tlv.type and\
                   NM_BORDER_AGENT_LOCATOR_TLV in p.thread_meshcop.tlv.type
                   ).\
            must_next()

        # Step 13: Leader sends a MLE Data Response to Router including the following TLVs:
        #             - Source Address TLV
        #             - Leader Data TLV
        #             - Network Data TLV
        #                 - Commissioner Data TLV:
        #                     Stable flag set to 0
        #                     Border Agent Locator TLV
        #                     Commissioner Session ID TLV
        #             - Active Timestamp TLV <70s>
        #             - Pending Timestamp TLV <20s>
        #             - Pending Operational Dataset TLV
        #                 - Active Timestamp TLV <30s>
        #                 - Delay Timer TLV <greater than 300s>
        #                 - Network Key TLV: New Network Key
        pkts.filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
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
                   p.mle.tlv.active_tstamp == 70 and\
                   p.mle.tlv.pending_tstamp == 20 and\
                   p.thread_meshcop.tlv.delay_timer > 300000 and\
                   p.thread_meshcop.tlv.master_key == KEY1 and\
                   p.thread_meshcop.tlv.active_tstamp == 30
                   ).\
            must_next()

        # Step 17: The DUT MUST send an ICMPv6 Echo Reply using the new Network key
        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(ROUTER).\
            filter_ipv6_dst(LEADER_MLEID).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(LEADER).\
            filter_ipv6_dst(ROUTER_MLEID).\
            must_next()


if __name__ == '__main__':
    unittest.main()
