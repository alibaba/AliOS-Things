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

LEADER = 1
ROUTER1 = 2
DUT_ROUTER2 = 3
ROUTER3 = 4
SED1 = 5
PREFIX_1 = '2001::/64'
GUA_1_START = '2001'
PREFIX_2 = '2002::/64'

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate that the DUT is able to generate
# Address Query and Address Notification messages properly.
# The Leader is configured as a Border Router with DHCPv6 server for prefixes
# 2001:: & 2002::
#
# Test Topology:
# -------------
# Router_1 - Leader
#           /     \
#     Router_3    Router_2(DUT)
#                    |
#                   SED
#
# DUT Types:
# ----------
#  Router


class Cert_5_3_09_AddressQuery(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1, DUT_ROUTER2, ROUTER3]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        DUT_ROUTER2: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER, SED1]
        },
        ROUTER3: {
            'name': 'ROUTER_3',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        SED1: {
            'name': 'SED',
            'is_mtd': True,
            'mode': '-',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [DUT_ROUTER2]
        },
    }

    # override wireshark preferences with case needed parameters
    CASE_WIRESHARK_PREFS = copy.deepcopy(WIRESHARK_OVERRIDE_PREFS)
    CASE_WIRESHARK_PREFS['6lowpan.context1'] = PREFIX_1
    CASE_WIRESHARK_PREFS['6lowpan.context2'] = PREFIX_2

    def test(self):
        # 1 & 2 ALL: Build and verify the topology
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        # Configure the LEADER to be a DHCPv6 Border Router for prefixes
        self.nodes[LEADER].add_prefix(PREFIX_1, 'pdros')
        self.nodes[LEADER].add_prefix(PREFIX_2, 'pdro')
        self.nodes[LEADER].register_netdata()

        self.nodes[ROUTER1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER1].get_state(), 'router')

        self.nodes[DUT_ROUTER2].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT_ROUTER2].get_state(), 'router')

        self.nodes[ROUTER3].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER3].get_state(), 'router')

        self.nodes[SED1].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED1].get_state(), 'child')

        self.collect_rlocs()
        self.collect_rloc16s()
        self.collect_ipaddrs()

        # 3 SED1: The SED1 sends an ICMPv6 Echo Request to ROUTER3 using GUA
        # PREFIX_1 address
        router3_addr = self.nodes[ROUTER3].get_addr(PREFIX_1)
        self.assertTrue(router3_addr is not None)
        self.assertTrue(self.nodes[SED1].ping(router3_addr))

        # 4 ROUTER1: ROUTER1 sends an ICMPv6 Echo Request to the SED1 using GUA
        # PREFIX_1 address
        sed1_addr = self.nodes[SED1].get_addr(PREFIX_1)
        self.assertTrue(sed1_addr is not None)
        self.assertTrue(self.nodes[ROUTER1].ping(sed1_addr))
        self.simulator.go(1)

        # 5 SED1: SED1 sends an ICMPv6 Echo Request to the ROUTER3 using GUA
        # PREFIX_1 address
        self.assertTrue(self.nodes[SED1].ping(router3_addr))
        self.simulator.go(1)

        # 6 DUT_ROUTER2: Power off ROUTER3 and wait 580s to alow LEADER to
        # expire its Router ID
        self.nodes[ROUTER3].stop()
        self.simulator.go(580)

        # The SED1 sends an ICMPv6 Echo Request to ROUTER3 GUA PREFIX_1 address
        self.assertFalse(self.nodes[SED1].ping(router3_addr))
        self.simulator.go(1)

        # 7 SED1: Power off SED1 and wait to allow DUT_ROUTER2 to timeout the
        # child
        self.nodes[SED1].stop()
        self.simulator.go(3)
        self.simulator.go(config.DEFAULT_CHILD_TIMEOUT)

        # ROUTER1 sends two ICMPv6 Echo Requests to SED1 GUA PREFIX_1 address
        self.assertFalse(self.nodes[ROUTER1].ping(sed1_addr))
        self.assertFalse(self.nodes[ROUTER1].ping(sed1_addr))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER_1 = pv.vars['ROUTER_1']
        ROUTER_1_RLOC = pv.vars['ROUTER_1_RLOC']
        ROUTER_2 = pv.vars['ROUTER_2']
        ROUTER_2_RLOC = pv.vars['ROUTER_2_RLOC']
        ROUTER_2_RLOC16 = pv.vars['ROUTER_2_RLOC16']
        ROUTER_3 = pv.vars['ROUTER_3']
        SED = pv.vars['SED']
        SED_RLOC16 = pv.vars['SED_RLOC16']
        MM = pv.vars['MM_PORT']
        GUA1 = {}

        for node in ('ROUTER_1', 'ROUTER_3', 'SED'):
            for addr in pv.vars['%s_IPADDRS' % node]:
                if addr.startswith(Bytes(GUA_1_START)):
                    GUA1[node] = addr

        # Step 1: Build the topology as described

        for i in range(1, 4):
            pv.verify_attached('ROUTER_%d' % i, 'LEADER')
        pv.verify_attached('SED', 'ROUTER_2', 'MTD')

        # Step 2: SED sends an ICMPv6 Echo Request to Router_3 using GUA 2001::
        #         address
        #         The DUT MUST generate an Address Query Request on SED’s behalf
        #         to find each node’s RLOC.
        #         The Address Query Request MUST be sent to the Realm-Local
        #         All-Routers address (FF03::2)
        #             CoAP URI-Path
        #                 - NON POST coap://<FF03::2>
        #             CoAP Payload
        #                 - Target EID TLV
        #         The DUT MUST receive and process the incoming Address Query
        #         Response and forward the ICMPv6 Echo Reply packet to SED

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(SED).\
            filter_ipv6_dst(GUA1['ROUTER_3']).\
            must_next()
        pkts.filter_wpan_src64(ROUTER_2).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            filter(lambda p: p.thread_address.tlv.target_eid == GUA1['ROUTER_3']).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_3).\
            filter_ipv6_dst(GUA1['SED']).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_2).\
            filter_wpan_dst16(SED_RLOC16).\
            must_next()

        # Step 3: Router_1 sends an ICMPv6 Echo Request to SED using GUA 2001::
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

        pkts.filter_wpan_src64(ROUTER_1).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            filter(lambda p: p.thread_address.tlv.target_eid == GUA1['SED']).\
            must_next()
        pkts.filter_ipv6_src_dst(ROUTER_2_RLOC, ROUTER_1_RLOC).\
            filter_coap_request(ADDR_NTF_URI, port=MM).\
            filter(lambda p: {
                                 NL_ML_EID_TLV,
                                 NL_RLOC16_TLV,
                                 NL_TARGET_EID_TLV
                             } <= set(p.coap.tlv.type) and\
                             p.thread_address.tlv.target_eid == GUA1['SED'] and\
                             p.thread_address.tlv.rloc16 == ROUTER_2_RLOC16
                   ).\
            must_next()
        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(ROUTER_1).\
            filter_ipv6_dst(GUA1['SED']).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(SED).\
            filter_ipv6_dst(GUA1['ROUTER_1']).\
            must_next()

        # Step 5: SED sends an ICMPv6 Echo Request to Router_3 using GUA 2001::
        #         address
        #         The DUT MUST not send an Address Query as Router_3 address should
        #         be cached.
        #         The DUT MUST forward the ICMPv6 Echo Reply to SED

        _pkt = pkts.filter_ping_request().\
            filter_wpan_src64(SED).\
            filter_ipv6_dst(GUA1['ROUTER_3']).\
            must_next()
        lstart = pkts.index
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_3).\
            filter_ipv6_dst(GUA1['SED']).\
            must_next()
        pkts.filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            filter_wpan_src64(ROUTER_2).\
            filter_wpan_dst16(SED_RLOC16).\
            must_next()
        lend = pkts.index
        pkts.range(lstart, lend).filter_wpan_src64(ROUTER_2).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            must_not_next()

        # Step 6: SED sends an ICMPv6 Echo Request to Router_3 using GUA 2001::
        #         address
        #         The DUT MUST update its address cache and remove all entries
        #         based on Router_3’s Router ID.
        #         The DUT MUST send an Address Query to discover Router_3’s RLOC address.
        pkts.filter_ping_request().\
            filter_wpan_src64(SED).\
            filter_ipv6_dst(GUA1['ROUTER_3']).\
            must_next()
        pkts.filter_wpan_src64(ROUTER_2).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            filter(lambda p: p.thread_address.tlv.target_eid == GUA1['ROUTER_3']).\
            must_next()

        # Step 7: Router_1 sends two ICMPv6 Echo Requests to SED using GUA 2001::
        #         address
        #         The DUT MUST NOT respond with an Address Notification message

        pkts.filter_wpan_src64(ROUTER_2).\
            filter_ipv6_dst(ROUTER_1_RLOC).\
            filter_coap_request(ADDR_NTF_URI, port=MM).\
            must_not_next()
        pkts.filter_ping_request().\
            filter_wpan_src64(ROUTER_1).\
            filter_ipv6_dst(GUA1['SED']).\
            must_next()


if __name__ == '__main__':
    unittest.main()
