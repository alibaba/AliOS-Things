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
from pktverify.consts import MLE_DATA_RESPONSE, MGMT_ACTIVE_SET_URI, NETWORK_DATA_TLV, SOURCE_ADDRESS_TLV, LEADER_DATA_TLV, ACTIVE_TIMESTAMP_TLV, ACTIVE_OPERATION_DATASET_TLV, NM_CHANNEL_TLV, NM_CHANNEL_MASK_TLV, NM_EXTENDED_PAN_ID_TLV, NM_NETWORK_MESH_LOCAL_PREFIX_TLV, NM_NETWORK_KEY_TLV, NM_NETWORK_NAME_TLV, NM_PAN_ID_TLV, NM_PSKC_TLV, NM_SECURITY_POLICY_TLV
from pktverify.packet_verifier import PacketVerifier

ROUTER = 1
LEADER = 2

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify the DUT’s behavior when receiving
# MGMT_ACTIVE_SET.req from an active Thread node.
#
# Test Topology:
# -------------
#  Router
#    |
#  Leader
#
# DUT Types:
# ----------
#  Leader


class Cert_9_2_05_ActiveDataset(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        ROUTER: {
            'name': 'ROUTER',
            'channel': 11,
            'network_key': '00112233445566778899aabbccddeeff',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        LEADER: {
            'name': 'LEADER',
            'channel': 11,
            'network_key': '00112233445566778899aabbccddeeff',
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
    }

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[ROUTER].commissioner_start()
        self.simulator.go(5)

        self.collect_rlocs()
        # Step 2: new, valid Timestamp TLV
        #         all valid Active Operational Dataset parameters,
        #         with new values in the TLVs that don’t affect connectivity
        #         binary = new pskc and security policy [3600, 0b11100000]
        self.nodes[ROUTER].send_mgmt_active_set(
            active_timestamp=100,
            channel_mask=0x3fff800,
            extended_panid='000db80000000001',
            mesh_local='fd00:0db8::',
            network_name='TEST_1',
            network_key='00112233445566778899aabbccddeeff',
            panid=0xface,
            channel=11,
            binary='0410d2aa9cd8dff7919122d77d37ec3c1b5f0c030e10e0',
        )
        self.simulator.go(5)

        # Step 7: old, invalid Active Timestamp TLV
        #         all valid Active Operational Dataset parameters, with
        #         new values in the TLVs that don’t affect connectivity
        #         binary = new pskc and security policy [3600, 0b11110000]
        self.nodes[ROUTER].send_mgmt_active_set(
            active_timestamp=100,
            channel_mask=0x1fff800,
            extended_panid='000db80000000002',
            mesh_local='fd00:0db8::',
            network_name='TEST_2',
            network_key='00112233445566778899aabbccddeeff',
            panid=0xface,
            channel=11,
            binary='041017d672be32b0c24a2f8385f2fbaf1d970c030e10f0',
        )
        self.simulator.go(5)

        # Step 9: new, valid Active Timestamp TLV
        #         all of valid Commissioner Dataset parameters plus one bogus TLV, and
        #         new values in the TLVs that don’t affect connectivity
        #         binary = new pskc and security policy [3600, 0b11111000] and BogusTLV=0x400
        self.nodes[ROUTER].send_mgmt_active_set(
            active_timestamp=101,
            channel_mask=0xfff800,
            extended_panid='000db80000000003',
            mesh_local='fd00:0db8::',
            network_name='TEST_3',
            network_key='00112233445566778899aabbccddeeff',
            panid=0xface,
            channel=11,
            binary='041008f4e9531e8efa8e852d5f4fb951b13e0c030e10f88202aa55',
        )
        self.simulator.go(5)

        # Step 14: new, valid Active Timestamp TLV
        #          attempt to set Channel TLV to an unsupported channel + all of other TLVs
        #          binary = pskc and security policy step 9
        self.nodes[ROUTER].send_mgmt_active_set(
            active_timestamp=102,
            channel_mask=0x1fff800,
            extended_panid='000db80000000003',
            mesh_local='fd00:0db8::',
            network_name='TEST_3',
            network_key='00112233445566778899aabbccddeeff',
            panid=0xface,
            channel=63,
            binary='041008f4e9531e8efa8e852d5f4fb951b13e0c030e10f8',
        )
        self.simulator.go(5)

        ipaddr = self.nodes[LEADER].get_rloc()
        self.assertTrue(self.nodes[ROUTER].ping(ipaddr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        ROUTER = pv.vars['ROUTER']
        ROUTER_RLOC = pv.vars['ROUTER_RLOC']

        # Step 1: Ensure the topology is formed correctly
        pv.verify_attached('ROUTER', 'LEADER')
        _pkt = pkts.last()

        # Step 3: Leader MUST send MGMT_ACTIVE_SET.rsp to the Router with
        #         with the following format:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             State TLV (value = Accept (01))
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(ROUTER_RLOC).\
            filter_coap_ack(MGMT_ACTIVE_SET_URI).\
            filter(lambda p: p.thread_meshcop.tlv.state == 1).\
            must_next()

        # Step 4: Leader MUST send a multicast MLE Data Response, including
        #         the following TLVs:
        #             - Source Address TLV
        #             - Leader Data TLV
        #                 Data version field [incremented]
        #                 Stable Version field [incremented]
        #             - Network Data TLV
        #             - Active Timestamp TLV [new value set in Step 9]
        _dr_pkt = pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV,
                              ACTIVE_TIMESTAMP_TLV
                             } <= set(p.mle.tlv.type) and\
                   p.mle.tlv.active_tstamp == 100 and\
                   (p.mle.tlv.leader_data.data_version -
                   _pkt.mle.tlv.leader_data.data_version) % 256 <= 127 and\
                   (p.mle.tlv.leader_data.stable_data_version -
                   _pkt.mle.tlv.leader_data.stable_data_version) % 256 <= 127
                   ).\
            must_next()

        # Step 6: Leader MUST send a unicast MLE Data Response to Router, including
        #         the following TLVs:
        #             - Source Address TLV
        #             - Leader Data TLV
        #             - Active Operational Dataset TLV
        #                 - Channel TLV
        #                 - Channel Mask TLV [new value set in Step 2]
        #                 - Extended PAN ID TLV [new value set in Step 2]
        #                 - Network Mesh-Local Prefix TLV
        #                 - Network Key TLV
        #                 - Network Name TLV [new value set in Step 2]
        #                 - PAN ID TLV
        #                 - PSKc TLV [new value set in Step 2]
        #                 - Security Policy TLV [new value set in Step 2]
        #             - Active Timestamp TLV [new value set in Step 2]
        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV,
                              ACTIVE_OPERATION_DATASET_TLV,
                              ACTIVE_TIMESTAMP_TLV
                             } <= set(p.mle.tlv.type) and\
                             {
                              NM_CHANNEL_TLV,
                              NM_CHANNEL_MASK_TLV,
                              NM_EXTENDED_PAN_ID_TLV,
                              NM_NETWORK_MESH_LOCAL_PREFIX_TLV,
                              NM_NETWORK_KEY_TLV,
                              NM_NETWORK_NAME_TLV,
                              NM_PAN_ID_TLV,
                              NM_PSKC_TLV,
                              NM_SECURITY_POLICY_TLV
                             } <= set(p.thread_meshcop.tlv.type) and\
                   p.mle.tlv.active_tstamp == 100 and\
                   p.thread_meshcop.tlv.chan_mask_mask == '001fffc0' and\
                   p.thread_meshcop.tlv.xpan_id == '000db80000000001' and\
                   p.thread_meshcop.tlv.net_name == ['TEST_1'] and\
                   p.thread_meshcop.tlv.pskc == 'd2aa9cd8dff7919122d77d37ec3c1b5f'
                   ).\
            must_next()

        # Step 8: Leader MUST send MGMT_ACTIVE_SET.rsp to the Router with
        #         with the following format:
        #         CoAP Response Code
        #             2.04 Changed
        #         CoAP Payload
        #             State TLV (value = Reject (ff))
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(ROUTER_RLOC).\
            filter_coap_ack(MGMT_ACTIVE_SET_URI).\
            filter(lambda p: p.thread_meshcop.tlv.state == -1).\
            must_next()

        # Step 10: Leader MUST send MGMT_ACTIVE_SET.rsp to the Router with
        #          with the following format:
        #          CoAP Response Code
        #              2.04 Changed
        #          CoAP Payload
        #              State TLV (value = Accept (01))
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(ROUTER_RLOC).\
            filter_coap_ack(MGMT_ACTIVE_SET_URI).\
            filter(lambda p: p.thread_meshcop.tlv.state == 1).\
            must_next()

        # Step 11: Leader MUST send a multicast MLE Data Response, including
        #          the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV
        #                  Data version field [incremented]
        #                  Stable Version field [incremented]
        #              - Network Data TLV
        #              - Active Timestamp TLV [new value set in Step 9]
        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV,
                              ACTIVE_TIMESTAMP_TLV
                             } <= set(p.mle.tlv.type) and\
                   p.mle.tlv.active_tstamp == 101 and\
                   (p.mle.tlv.leader_data.data_version -
                   _dr_pkt.mle.tlv.leader_data.data_version) % 256 <= 127 and\
                   (p.mle.tlv.leader_data.stable_data_version -
                   _dr_pkt.mle.tlv.leader_data.stable_data_version) % 256 <= 127
                   ).\
            must_next()

        # Step 13: Leader MUST send a unicast MLE Data Response to Router, including
        #          the following TLVs:
        #              - Source Address TLV
        #              - Leader Data TLV
        #              - Active Operational Dataset TLV
        #                  - Channel TLV
        #                  - Channel Mask TLV [new value set in Step 9]
        #                  - Extended PAN ID TLV [new value set in Step 9]
        #                  - Network Mesh-Local Prefix TLV
        #                  - Network Key TLV
        #                  - Network Name TLV [new value set in Step 9]
        #                  - PAN ID TLV
        #                  - PSKc TLV [new value set in Step 9]
        #                  - Security Policy TLV [new value set in Step 9]
        #              - Active Timestamp TLV [new value set in Step 9]
        pkts.filter_wpan_src64(LEADER).\
            filter_wpan_dst64(ROUTER).\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              NETWORK_DATA_TLV,
                              SOURCE_ADDRESS_TLV,
                              LEADER_DATA_TLV,
                              ACTIVE_OPERATION_DATASET_TLV,
                              ACTIVE_TIMESTAMP_TLV
                             } <= set(p.mle.tlv.type) and\
                             {
                              NM_CHANNEL_TLV,
                              NM_CHANNEL_MASK_TLV,
                              NM_EXTENDED_PAN_ID_TLV,
                              NM_NETWORK_MESH_LOCAL_PREFIX_TLV,
                              NM_NETWORK_KEY_TLV,
                              NM_NETWORK_NAME_TLV,
                              NM_PAN_ID_TLV,
                              NM_PSKC_TLV,
                              NM_SECURITY_POLICY_TLV
                             } <= set(p.thread_meshcop.tlv.type) and\
                   p.mle.tlv.active_tstamp == 101 and\
                   p.thread_meshcop.tlv.chan_mask_mask == '001fff00' and\
                   p.thread_meshcop.tlv.xpan_id == '000db80000000003' and\
                   p.thread_meshcop.tlv.net_name == ['TEST_3'] and\
                   p.thread_meshcop.tlv.pskc == '08f4e9531e8efa8e852d5f4fb951b13e'
                   ).\
            must_next()

        # Step 15: Leader MUST send MGMT_ACTIVE_SET.rsp to the Router with
        #          with the following format:
        #          CoAP Response Code
        #              2.04 Changed
        #          CoAP Payload
        #              State TLV (value = Reject (ff)))
        pkts.filter_wpan_src64(LEADER).\
            filter_ipv6_dst(ROUTER_RLOC).\
            filter_coap_ack(MGMT_ACTIVE_SET_URI).\
            filter(lambda p: p.thread_meshcop.tlv.state == -1).\
            must_next()

        # Step 16: The DUT must respond with an ICMPv6 Echo Reply
        _pkt = pkts.filter_ping_request().\
            filter_ipv6_src_dst(ROUTER_RLOC, LEADER_RLOC).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_ipv6_src_dst(LEADER_RLOC, ROUTER_RLOC).\
            must_next()


if __name__ == '__main__':
    unittest.main()
