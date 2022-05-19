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

import copy
import unittest

import command
import config
import copy
import ipv6
import thread_cert
from pktverify.consts import WIRESHARK_OVERRIDE_PREFS, ADDR_QRY_URI, ADDR_NTF_URI, NL_ML_EID_TLV, NL_RLOC16_TLV, NL_TARGET_EID_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.bytes import Bytes
from pktverify.addrs import Ipv6Addr

LEADER = 1
BR = 2
ROUTER1 = 3
DUT_ROUTER2 = 4
MED1 = 5

PREFIX_1 = '2003::/64'
GUA_1_START = '2003'
PREFIX_2 = '2004::/64'

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate that the DUT is able to generate
# Address Query and Address Notification messages
# The Border Router is configured as a SLAAC server for prefixes 2003:: & 2004::
#
# Test Topology:
# -------------
# BorderRouter - Leader
#               /     \
#         Router_1 - Router_2(DUT)
#                       |
#                      MED
#
# DUT Types:
# ----------
#  Router


class Cert_5_3_10_AddressQuery(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False
    SUPPORT_NCP = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [BR, ROUTER1, DUT_ROUTER2]
        },
        BR: {
            'name': 'BR',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER, DUT_ROUTER2]
        },
        DUT_ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER, ROUTER1, MED1]
        },
        MED1: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [DUT_ROUTER2]
        },
    }
    # override wireshark preferences with case needed parameters
    CASE_WIRESHARK_PREFS = copy.deepcopy(WIRESHARK_OVERRIDE_PREFS)
    CASE_WIRESHARK_PREFS['6lowpan.context1'] = PREFIX_1
    CASE_WIRESHARK_PREFS['6lowpan.context2'] = PREFIX_2

    def test(self):
        # 1 & 2
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[BR].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[BR].get_state(), 'router')

        # Configure two On-Mesh Prefixes on the BR
        self.nodes[BR].add_prefix(PREFIX_1, 'paros')
        self.nodes[BR].add_prefix(PREFIX_2, 'paros')
        self.nodes[BR].register_netdata()

        self.nodes[DUT_ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT_ROUTER2].get_state(), 'router')

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[MED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MED1].get_state(), 'child')

        self.collect_rlocs()
        self.collect_rloc16s()
        self.collect_ipaddrs()

        # 3 MED1: MED1 sends an ICMPv6 Echo Request to Router1 using GUA
        # PREFIX_1 address
        router1_addr = self.nodes[ROUTER1].get_addr(PREFIX_1)
        self.assertTrue(router1_addr is not None)
        self.assertTrue(self.nodes[MED1].ping(router1_addr))
        self.simulator.go(1)

        # 4 BR: BR sends an ICMPv6 Echo Request to MED1 using GUA PREFIX_1
        # address
        med1_addr = self.nodes[MED1].get_addr(PREFIX_1)
        self.assertTrue(med1_addr is not None)
        self.assertTrue(self.nodes[BR].ping(med1_addr))
        self.simulator.go(1)

        # 5 MED1: MED1 sends an ICMPv6 Echo Request to ROUTER1 using GUA PREFIX_1
        # address
        self.assertTrue(self.nodes[MED1].ping(router1_addr))
        self.simulator.go(1)

        # 6 DUT_ROUTER2: Power off ROUTER1 and wait 580 seconds to allow the
        # LEADER to expire its Router ID
        router1_id = self.nodes[ROUTER1].get_router_id()
        self.nodes[ROUTER1].stop()
        self.simulator.go(580)

        # Send an ICMPv6 Echo Request from MED1 to ROUTER1 GUA PREFIX_1 address
        self.assertFalse(self.nodes[MED1].ping(router1_addr))
        self.simulator.go(1)

        # 7 MED1: Power off MED1 and wait to allow DUT_ROUTER2 to timeout the
        # child
        self.nodes[MED1].stop()
        self.simulator.go(config.MLE_END_DEVICE_TIMEOUT)

        # BR sends two ICMPv6 Echo Requests to MED1 GUA PREFIX_1 address
        self.assertFalse(self.nodes[BR].ping(med1_addr))
        self.assertFalse(self.nodes[BR].ping(med1_addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_2 = pv.vars['ROUTER_2']
        ROUTER_2_RLOC = pv.vars['ROUTER_2_RLOC']
        ROUTER_2_RLOC16 = pv.vars['ROUTER_2_RLOC16']
        BR = pv.vars['BR']
        BR_RLOC = pv.vars['BR_RLOC']
        MED = pv.vars['MED']
        MED_RLOC16 = pv.vars['MED_RLOC16']
        MM = pv.vars['MM_PORT']
        GUA1 = {}

        for node in ('ROUTER_1', 'BR', 'MED'):
            for addr in pv.vars['%s_IPADDRS' % node]:
                if addr.startswith(Bytes(GUA_1_START)):
                    GUA1[node] = addr

        # Step 2: Build the topology as described

        pv.verify_attached('BR', 'LEADER')
        for i in (2, 1):
            pv.verify_attached('ROUTER_%d' % i, 'LEADER')
        pv.verify_attached('MED', 'ROUTER_2', 'MTD')

        # Step 3: MED sends an ICMPv6 Echo Request to Router_1 using GUA 2003::
        #         address
        #         The DUT MUST generate an Address Query Request on MED’s behalf
        #         to find each node’s RLOC.
        #         The Address Query Request MUST be sent to the Realm-Local
        #         All-Routers address (FF03::2)
        #             CoAP URI-Path
        #                 - NON POST coap://<FF03::2>
        #             CoAP Payload
        #                 - Target EID TLV
        #         The DUT MUST receive and process the incoming Address Query
        #         Response and forward the ICMPv6 Echo Request packet to Router_1

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(MED).\
            filter_ipv6_dst(GUA1['ROUTER_1']).\
            must_next()
        pkts.filter_wpan_src64(ROUTER_2).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            filter(lambda p: p.thread_address.tlv.target_eid == GUA1['ROUTER_1']).\
            must_next()
        pkts.filter_ping_request(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_2).\
            filter_ipv6_dst(GUA1['ROUTER_1']).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_1).\
            filter_ipv6_dst(GUA1['MED']).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_2).\
            filter_wpan_dst16(MED_RLOC16).\
            must_next()

        # Step 4: Border Router sends an ICMPv6 Echo Request to MED using GUA 2003::
        #         addresss
        #         The DUT MUST respond to the Address Query Request with a properly
        #         formatted Address Notification Message:
        #             CoAP URI-Path
        #                 - CON POST coap://[<Address Query Source>]:MM/a/an
        #             CoAP Payload
        #                 - ML-EID TLV
        #                 - RLOC16 TLV
        #                 - Target EID TLV
        #         The IPv6 Source address MUST be the RLOC of the originator
        #         The IPv6 Destination address MUST be the RLOC of the destination

        pkts.filter_wpan_src64(BR).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            filter(lambda p: p.thread_address.tlv.target_eid == GUA1['MED']).\
            must_next()
        pkts.filter_ipv6_src_dst(ROUTER_2_RLOC, BR_RLOC).\
            filter_coap_request(ADDR_NTF_URI, port=MM).\
            filter(lambda p: {
                                 NL_ML_EID_TLV,
                                 NL_RLOC16_TLV,
                                 NL_TARGET_EID_TLV
                             } <= set(p.coap.tlv.type) and\
                             p.thread_address.tlv.target_eid == GUA1['MED'] and\
                             p.thread_address.tlv.rloc16 == ROUTER_2_RLOC16
                   ).\
            must_next()
        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(BR).\
            filter_ipv6_dst(GUA1['MED']).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(MED).\
            filter_ipv6_dst(GUA1['BR']).\
            must_next()

        # Step 5: MED sends an ICMPv6 Echo Request to Router_1 using GUA 2003::
        #         address
        #         The DUT MUST not send an Address Query as Router_1 address should
        #         be cached.
        #         The DUT MUST forward the ICMPv6 Echo Reply to MED

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(MED).\
            filter_ipv6_dst(GUA1['ROUTER_1']).\
            must_next()
        lstart = pkts.index
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_1).\
            filter_ipv6_dst(GUA1['MED']).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_2).\
            filter_wpan_dst16(MED_RLOC16).\
            must_next()
        lend = pkts.index
        pkts.range(lstart, lend).filter_wpan_src64(ROUTER_2).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            must_not_next()

        # Step 6: MED sends an ICMPv6 Echo Request to Router_1 using GUA 2003::
        #         address
        #         The DUT MUST update its address cache and remove all entries
        #         based on Router_1’s Router ID.
        #         The DUT MUST send an Address Query to discover Router_1’s RLOC address.

        pkts.filter_ping_request().\
            filter_wpan_src64(MED).\
            filter_ipv6_dst(GUA1['ROUTER_1']).\
            must_next()
        pkts.filter_wpan_src64(ROUTER_2).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            filter(lambda p: p.thread_address.tlv.target_eid == GUA1['ROUTER_1']).\
            must_next()

        # Step 7: Border Router sends two ICMPv6 Echo Requests to MED using GUA 2003::
        #         address
        #         The DUT MUST NOT respond with an Address Notification message

        pkts.filter_wpan_src64(ROUTER_2).\
            filter_ipv6_dst(BR_RLOC).\
            filter_coap_request(ADDR_NTF_URI, port=MM).\
            must_not_next()
        pkts.filter_ping_request().\
            filter_wpan_src64(BR).\
            filter_ipv6_dst(GUA1['MED']).\
            must_next()


if __name__ == '__main__':
    unittest.main()
