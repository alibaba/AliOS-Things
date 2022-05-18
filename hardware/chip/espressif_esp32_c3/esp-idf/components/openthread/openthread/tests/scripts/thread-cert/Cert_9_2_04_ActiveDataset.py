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
from pktverify.consts import MLE_CHILD_ID_RESPONSE, MGMT_ACTIVE_SET_URI, MGMT_ACTIVE_GET_URI
from pktverify.packet_verifier import PacketVerifier
from pktverify.bytes import Bytes

COMMISSIONER = 1
LEADER = 2


class Cert_9_2_04_ActiveDataset(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        COMMISSIONER: {
            'name': 'COMMISSIONER',
            'active_dataset': {
                'timestamp': 10,
                'network_key': '00112233445566778899aabbccddeeff'
            },
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        LEADER: {
            'name': 'LEADER',
            'active_dataset': {
                'timestamp': 10,
                'network_key': '00112233445566778899aabbccddeeff'
            },
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

        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(5)

        self.collect_rlocs()
        self.collect_leader_aloc(LEADER)
        # Step 2
        self.nodes[COMMISSIONER].send_mgmt_active_set(
            active_timestamp=101,
            channel_mask=0x7fff800,
            extended_panid='000db70000000000',
            network_name='GRL',
        )
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_network_name(), 'GRL')
        self.nodes[COMMISSIONER].send_mgmt_active_get()
        self.simulator.go(5)

        # Step 6
        # Attempt to set Channel TLV
        self.nodes[COMMISSIONER].send_mgmt_active_set(
            active_timestamp=102,
            channel=18,
            channel_mask=0x7fff800,
            extended_panid='000db70000000001',
            network_name='threadcert',
        )
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_network_name(), 'GRL')

        # Step 8
        # Attempt to set Mesh Local Prefix TLV
        self.nodes[COMMISSIONER].send_mgmt_active_set(
            active_timestamp=103,
            channel_mask=0x7fff800,
            extended_panid='000db70000000000',
            mesh_local='fd00:0db7::',
            network_name='UL',
        )
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_network_name(), 'GRL')

        # Step 10
        # Attempt to set Network Key TLV
        self.nodes[COMMISSIONER].send_mgmt_active_set(
            active_timestamp=104,
            channel_mask=0x7fff800,
            extended_panid='000db70000000000',
            network_key='ffeeddccbbaa99887766554433221100',
            mesh_local='fd00:0db7::',
            network_name='GRL',
        )
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_network_name(), 'GRL')

        # Step 12
        # Attempt to set PAN ID TLV
        self.nodes[COMMISSIONER].send_mgmt_active_set(
            active_timestamp=105,
            channel_mask=0x7fff800,
            extended_panid='000db70000000000',
            network_key='00112233445566778899aabbccddeeff',
            mesh_local='fd00:0db7::',
            network_name='UL',
            panid=0xafce,
        )
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_network_name(), 'GRL')

        # Step 14
        # Invalid Commissioner Session ID
        self.nodes[COMMISSIONER].send_mgmt_active_set(
            active_timestamp=106,
            channel_mask=0x7fff800,
            extended_panid='000db70000000000',
            network_name='UL',
            binary='0b02abcd',
        )
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_network_name(), 'GRL')

        # Step 16
        # Old Active Timestamp
        self.nodes[COMMISSIONER].send_mgmt_active_set(
            active_timestamp=101,
            channel_mask=0x01fff800,
            extended_panid='000db70000000000',
            network_name='UL',
        )
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_network_name(), 'GRL')

        # Step 18
        # Unexpected Steering Data TLV
        self.nodes[COMMISSIONER].send_mgmt_active_set(
            active_timestamp=107,
            channel_mask=0x7fff800,
            extended_panid='000db70000000000',
            network_name='UL',
            binary='0806113320440000',
        )
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_network_name(), 'UL')

        # Step 20
        # Undefined TLV
        self.nodes[COMMISSIONER].send_mgmt_active_set(
            active_timestamp=108,
            channel_mask=0x7fff800,
            extended_panid='000db70000000000',
            network_name='GRL',
            binary='8202aa55',
        )
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_network_name(), 'GRL')

        ipaddrs = self.nodes[COMMISSIONER].get_addrs()
        for ipaddr in ipaddrs:
            self.assertTrue(self.nodes[LEADER].ping(ipaddr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        LEADER_ALOC = pv.vars['LEADER_ALOC']
        COMMISSIONER = pv.vars['COMMISSIONER']
        COMMISSIONER_RLOC = pv.vars['COMMISSIONER_RLOC']

        # Step 1: Ensure the topology is formed correctly
        pkts.filter_wpan_src64(LEADER).filter_wpan_dst64(COMMISSIONER).filter_mle_cmd(
            MLE_CHILD_ID_RESPONSE).must_next()

        # Step 2: Commissioner sends MGMT_ACTIVE_SET.req to Leader RLOC or Anycast Locator
        pkts.filter_wpan_src64(COMMISSIONER).filter_ipv6_2dsts(LEADER_RLOC, LEADER_ALOC).filter_coap_request(
            MGMT_ACTIVE_SET_URI).filter(lambda p: p.thread_meshcop.tlv.xpan_id == '000db70000000000' and p.
                                        thread_meshcop.tlv.net_name == ['GRL'] and p.thread_meshcop.tlv.chan_mask_mask
                                        == '001fffe0' and p.thread_meshcop.tlv.active_tstamp == 101).must_next()

        # Step 3: Leader MUST send MGMT_ACTIVE_SET.rsp to the Commissioner
        pkts.filter_wpan_src64(LEADER).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_ack(MGMT_ACTIVE_SET_URI).filter(
            lambda p: p.thread_meshcop.tlv.state == 1).must_next()

        # Step 4: Commissioner sends MGMT_ACTIVE_GET.req to Leader
        pkts.filter_wpan_src64(COMMISSIONER).filter_ipv6_2dsts(
            LEADER_RLOC, LEADER_ALOC).filter_coap_request(MGMT_ACTIVE_GET_URI).must_next()

        # Step 5: The Leader MUST send MGMT_ACTIVE_GET.rsp to the Commissioner
        pkts.filter_wpan_src64(LEADER).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_ack(
            MGMT_ACTIVE_GET_URI).filter(lambda p: p.thread_meshcop.tlv.active_tstamp == 101 and p.thread_meshcop.tlv.
                                        xpan_id == '000db70000000000' and p.thread_meshcop.tlv.net_name == ['GRL'] and
                                        p.thread_meshcop.tlv.chan_mask_mask == '001fffe0').must_next()

        # Step 6: Commissioner sends MGMT_ACTIVE_SET.req to Leader RLOC or Anycast Locator
        pkts.filter_wpan_src64(COMMISSIONER).filter_ipv6_2dsts(
            LEADER_RLOC, LEADER_ALOC).filter_coap_request(MGMT_ACTIVE_SET_URI).filter(
                lambda p: p.thread_meshcop.tlv.active_tstamp == 102 and p.thread_meshcop.tlv.xpan_id ==
                '000db70000000001' and p.thread_meshcop.tlv.net_name == ['threadcert'] and p.thread_meshcop.tlv.
                chan_mask_mask == '001fffe0' and p.thread_meshcop.tlv.channel == [18]).must_next()

        # Step 7: Leader MUST send MGMT_ACTIVE_SET.rsp to the Commissioner
        pkts.filter_wpan_src64(LEADER).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_ack(MGMT_ACTIVE_SET_URI).filter(
            lambda p: p.thread_meshcop.tlv.state == -1).must_next()

        # Step 8: Commissioner sends MGMT_ACTIVE_SET.req to Leader RLOC or Leader Anycast Locator
        pkts.filter_wpan_src64(COMMISSIONER).filter_ipv6_2dsts(
            LEADER_RLOC, LEADER_ALOC).filter_coap_request(MGMT_ACTIVE_SET_URI).filter(
                lambda p: p.thread_meshcop.tlv.active_tstamp == 103 and p.thread_meshcop.tlv.xpan_id ==
                '000db70000000000' and p.thread_meshcop.tlv.net_name == ['UL'] and p.thread_meshcop.tlv.chan_mask_mask
                == '001fffe0' and p.thread_meshcop.tlv.ml_prefix == 'fd000db700000000').must_next()

        # Step 9: Leader MUST send MGMT_ACTIVE_SET.rsp to the Commissioner
        pkts.filter_wpan_src64(LEADER).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_ack(MGMT_ACTIVE_SET_URI).filter(
            lambda p: p.thread_meshcop.tlv.state == -1).must_next()

        # Step 10: Commissioner sends MGMT_ACTIVE_SET.req to Leader RLOC or Leader Anycast Locator
        pkts.filter_wpan_src64(COMMISSIONER).filter_ipv6_2dsts(
            LEADER_RLOC, LEADER_ALOC).filter_coap_request(MGMT_ACTIVE_SET_URI).filter(
                lambda p: p.thread_meshcop.tlv.active_tstamp == 104 and p.thread_meshcop.tlv.xpan_id ==
                '000db70000000000' and p.thread_meshcop.tlv.net_name == ['GRL'] and p.thread_meshcop.tlv.master_key ==
                'ffeeddccbbaa99887766554433221100' and p.thread_meshcop.tlv.chan_mask_mask == '001fffe0' and p.
                thread_meshcop.tlv.ml_prefix == 'fd000db700000000').must_next()

        # Step 11: Leader MUST send MGMT_ACTIVE_SET.rsp to the Commissioner
        pkts.filter_wpan_src64(LEADER).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_ack(MGMT_ACTIVE_SET_URI).filter(
            lambda p: p.thread_meshcop.tlv.state == -1).must_next()

        # Step 12: Commissioner sends MGMT_ACTIVE_SET.req to Leader RLOC or Leader Anycast Locator
        pkts.filter_wpan_src64(COMMISSIONER).filter_ipv6_2dsts(
            LEADER_RLOC, LEADER_ALOC).filter_coap_request(MGMT_ACTIVE_SET_URI).filter(
                lambda p: p.thread_meshcop.tlv.active_tstamp == 105 and p.thread_meshcop.tlv.xpan_id ==
                '000db70000000000' and p.thread_meshcop.tlv.net_name == ['UL'] and p.thread_meshcop.tlv.master_key ==
                '00112233445566778899aabbccddeeff' and p.thread_meshcop.tlv.pan_id == [0xafce] and p.thread_meshcop.tlv
                .chan_mask_mask == '001fffe0' and p.thread_meshcop.tlv.ml_prefix == 'fd000db700000000').must_next()

        # Step 13: Leader MUST send MGMT_ACTIVE_SET.rsp to the Commissioner
        pkts.filter_wpan_src64(LEADER).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_ack(MGMT_ACTIVE_SET_URI).filter(
            lambda p: p.thread_meshcop.tlv.state == -1).must_next()

        # Step 14: Commissioner sends MGMT_ACTIVE_SET.req to Leader RLOC or Leader Anycast Locator
        pkts.filter_wpan_src64(COMMISSIONER).filter_ipv6_2dsts(
            LEADER_RLOC, LEADER_ALOC).filter_coap_request(MGMT_ACTIVE_SET_URI).filter(
                lambda p: p.thread_meshcop.tlv.active_tstamp == 106 and p.thread_meshcop.tlv.xpan_id ==
                '000db70000000000' and p.thread_meshcop.tlv.net_name == ['UL'] and p.thread_meshcop.tlv.
                commissioner_sess_id == 0xabcd and p.thread_meshcop.tlv.chan_mask_mask == '001fffe0').must_next()

        # Step 15: Leader MUST send MGMT_ACTIVE_SET.rsp to the Commissioner
        pkts.filter_wpan_src64(LEADER).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_ack(MGMT_ACTIVE_SET_URI).filter(
            lambda p: p.thread_meshcop.tlv.state == -1).must_next()

        # Step 16: Commissioner sends MGMT_ACTIVE_SET.req to Leader RLOC or Leader Anycast Locator
        pkts.filter_wpan_src64(COMMISSIONER).filter_ipv6_2dsts(LEADER_RLOC, LEADER_ALOC).filter_coap_request(
            MGMT_ACTIVE_SET_URI).filter(lambda p: p.thread_meshcop.tlv.active_tstamp == 101 and p.thread_meshcop.tlv.
                                        xpan_id == '000db70000000000' and p.thread_meshcop.tlv.net_name == ['UL'] and p
                                        .thread_meshcop.tlv.chan_mask_mask == '001fff80').must_next()

        # Step 17: Leader MUST send MGMT_ACTIVE_SET.rsp to the Commissioner
        pkts.filter_wpan_src64(LEADER).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_ack(MGMT_ACTIVE_SET_URI).filter(
            lambda p: p.thread_meshcop.tlv.state == -1).must_next()

        # Step 18: Commissioner sends MGMT_ACTIVE_SET.req to Leader RLOC or Leader Anycast Locator
        pkts.filter_wpan_src64(COMMISSIONER).filter_ipv6_2dsts(
            LEADER_RLOC, LEADER_ALOC).filter_coap_request(MGMT_ACTIVE_SET_URI).filter(
                lambda p: p.thread_meshcop.tlv.active_tstamp == 107 and p.thread_meshcop.tlv.xpan_id ==
                '000db70000000000' and p.thread_meshcop.tlv.net_name == ['UL'] and p.thread_meshcop.tlv.steering_data
                == Bytes('113320440000') and p.thread_meshcop.tlv.chan_mask_mask == '001fffe0').must_next()

        # Step 19: Leader MUST send MGMT_ACTIVE_SET.rsp to the Commissioner
        pkts.filter_wpan_src64(LEADER).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_ack(MGMT_ACTIVE_SET_URI).filter(
            lambda p: p.thread_meshcop.tlv.state == 1).must_next()

        # Step 20: Commissioner sends MGMT_ACTIVE_SET.req to Leader RLOC or Leader Anycast Locator
        pkts.filter_wpan_src64(COMMISSIONER).filter_ipv6_2dsts(
            LEADER_RLOC, LEADER_ALOC).filter_coap_request(MGMT_ACTIVE_SET_URI).filter(
                lambda p: p.thread_meshcop.tlv.active_tstamp == 108 and p.thread_meshcop.tlv.xpan_id ==
                '000db70000000000' and p.thread_meshcop.tlv.net_name == ['GRL'] and p.thread_meshcop.tlv.unknown ==
                'aa55' and p.thread_meshcop.tlv.chan_mask_mask == '001fffe0').must_next()

        # Step 21: Leader MUST send MGMT_ACTIVE_SET.rsp to the Commissioner
        pkts.filter_wpan_src64(LEADER).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_ack(MGMT_ACTIVE_SET_URI).filter(
            lambda p: p.thread_meshcop.tlv.state == 1).must_next()


if __name__ == '__main__':
    unittest.main()
