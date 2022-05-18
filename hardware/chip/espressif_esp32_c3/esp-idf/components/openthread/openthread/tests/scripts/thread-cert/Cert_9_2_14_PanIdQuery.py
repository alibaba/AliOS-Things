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
from pktverify.consts import MLE_CHILD_ID_REQUEST, MGMT_PANID_QUERY, MGMT_PANID_CONFLICT, MGMT_ED_REPORT, NM_COMMISSIONER_SESSION_ID_TLV, NM_CHANNEL_MASK_TLV, NM_PAN_ID_TLV, REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS
from pktverify.packet_verifier import PacketVerifier

COMMISSIONER = 1
LEADER1 = 2
ROUTER1 = 3
LEADER2 = 4


class Cert_9_2_14_PanIdQuery(thread_cert.TestCase):
    SUPPORT_NCP = False

    TOPOLOGY = {
        COMMISSIONER: {
            'name': 'COMMISSIONER',
            'mode': 'rdn',
            'allowlist': [LEADER1]
        },
        LEADER1: {
            'name': 'LEADER_1',
            'mode': 'rdn',
            'allowlist': [COMMISSIONER, ROUTER1]
        },
        ROUTER1: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER1, LEADER2]
        },
        LEADER2: {
            'name': 'LEADER_2',
            'mode': 'rdn',
            'panid': 0xdead,
            'allowlist': [ROUTER1]
        },
    }

    def test(self):
        self.nodes[LEADER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER1].get_state(), 'leader')

        self.nodes[COMMISSIONER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[COMMISSIONER].get_state(), 'router')
        self.nodes[COMMISSIONER].commissioner_start()
        self.simulator.go(3)

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[LEADER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER2].get_state(), 'leader')

        self.collect_rlocs()
        ipaddrs = self.nodes[ROUTER1].get_addrs()
        for ipaddr in ipaddrs:
            if ipaddr[0:4] != 'fe80':
                break

        self.nodes[COMMISSIONER].panid_query(0xdead, 0xffffffff, ipaddr)

        self.nodes[COMMISSIONER].panid_query(0xdead, 0xffffffff, 'ff33:0040:fd00:db8:0:0:0:1')

        self.assertTrue(self.nodes[COMMISSIONER].ping(ipaddr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        ROUTER = pv.vars['ROUTER']
        COMMISSIONER = pv.vars['COMMISSIONER']
        ROUTER_RLOC = pv.vars['ROUTER_RLOC']
        COMMISSIONER_RLOC = pv.vars['COMMISSIONER_RLOC']
        _rpkts = pkts.filter_wpan_src64(ROUTER)
        _cpkts = pkts.filter_wpan_src64(COMMISSIONER)

        # Step 1: Ensure the topology is formed correctly
        _rpkts.filter_mle_cmd(MLE_CHILD_ID_REQUEST).must_next()

        # Step 2: Commissioner MUST send a unicast MGMT_PANID_QUERY.qry unicast to Router_1
        _cpkts.filter_ipv6_dst(ROUTER_RLOC).filter_coap_request(MGMT_PANID_QUERY).must_next().must_verify(
            lambda p: {NM_COMMISSIONER_SESSION_ID_TLV, NM_CHANNEL_MASK_TLV, NM_PAN_ID_TLV} <= set(p.thread_meshcop.tlv.
                                                                                                  type))

        # Step 3: Router MUST send MGMT_ED_REPORT.ans to the Commissioner
        _rpkts.range(
            _cpkts.index).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_request(MGMT_PANID_CONFLICT).must_next(
            ).must_verify(lambda p: {NM_CHANNEL_MASK_TLV, NM_PAN_ID_TLV} <= set(p.thread_meshcop.tlv.type))

        # Step 4: Commissioner MUST send a multicast MGMT_PANID_QUERY.qry
        _cpkts.filter_ipv6_dst(REALM_LOCAL_All_THREAD_NODES_MULTICAST_ADDRESS).filter_coap_request(
            MGMT_PANID_QUERY).must_next().must_verify(
                lambda p: {NM_COMMISSIONER_SESSION_ID_TLV, NM_CHANNEL_MASK_TLV, NM_PAN_ID_TLV} <= set(p.thread_meshcop.
                                                                                                      tlv.type))

        # Step 5: Router MUST send MGMT_PANID_CONFLICT.ans to the Commissioner
        _rpkts.range(
            _cpkts.index).filter_ipv6_dst(COMMISSIONER_RLOC).filter_coap_request(MGMT_PANID_CONFLICT).must_next(
            ).must_verify(lambda p: {NM_CHANNEL_MASK_TLV, NM_PAN_ID_TLV} <= set(p.thread_meshcop.tlv.type))

        # Step 6: Router MUST respond with an ICMPv6 Echo Reply
        _rpkts.filter_ipv6_dst(COMMISSIONER_RLOC).filter_ping_reply().must_next()


if __name__ == '__main__':
    unittest.main()
