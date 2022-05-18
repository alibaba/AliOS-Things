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

import command
import config
import copy
import ipv6
import mle
import thread_cert
from pktverify.consts import WIRESHARK_OVERRIDE_PREFS, MLE_ADVERTISEMENT, ADDR_QRY_URI, SOURCE_ADDRESS_TLV, ROUTE64_TLV, LEADER_DATA_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.bytes import Bytes

DUT_LEADER = 1
BR = 2
MED1 = 3
MED2 = 4

MTDS = [MED1, MED2]

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate that the DUT MTD Child Address Set
# can hold at least 4 IPv6 non-link-local addresses.
#
# Test Topology:
# -------------
#           MED_1
#            |
#  BR - Leader(DUT) - MED_2
#
# DUT Types:
# ----------
#  Leader


class Cert_5_3_8_ChildAddressSet(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        DUT_LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [BR, MED1, MED2]
        },
        BR: {
            'mode': 'rdn',
            'allowlist': [DUT_LEADER]
        },
        MED1: {
            'name': 'MED_1',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [DUT_LEADER]
        },
        MED2: {
            'name': 'MED_2',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [DUT_LEADER]
        },
    }
    # override wireshark preferences with case needed parameters
    CASE_WIRESHARK_PREFS = copy.deepcopy(WIRESHARK_OVERRIDE_PREFS)
    CASE_WIRESHARK_PREFS['6lowpan.context1'] = '2001::/64'
    CASE_WIRESHARK_PREFS['6lowpan.context2'] = '2002::/64'
    CASE_WIRESHARK_PREFS['6lowpan.context3'] = '2003::/64'

    def test(self):
        self.nodes[DUT_LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[DUT_LEADER].get_state(), 'leader')

        self.nodes[BR].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[BR].get_state(), 'router')

        # 1 BR: Configure BR to be a DHCPv6 server
        self.nodes[BR].add_prefix('2001::/64', 'pdros')
        self.nodes[BR].add_prefix('2002::/64', 'pdros')
        self.nodes[BR].add_prefix('2003::/64', 'pdros')
        self.nodes[BR].register_netdata()

        # 3 MED1, MED2: MED1 and MED2 attach to DUT_LEADER
        for i in MTDS:
            self.nodes[i].start()
            self.simulator.go(5)
            self.assertEqual(self.nodes[i].get_state(), 'child')
        self.collect_ipaddrs()

        # 4 MED1: MED1 send an ICMPv6 Echo Request to the MED2 ML-EID
        med2_ml_eid = self.nodes[MED2].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        self.assertTrue(med2_ml_eid is not None)
        self.assertTrue(self.nodes[MED1].ping(med2_ml_eid))

        # Wait for sniffer got packets
        self.simulator.go(1)

        # 5 MED1: MED1 send an ICMPv6 Echo Request to the MED2 2001::GUA
        addr = self.nodes[MED2].get_addr("2001::/64")
        self.assertTrue(addr is not None)
        self.assertTrue(self.nodes[MED1].ping(addr))
        # Wait for sniffer got packets
        self.simulator.go(1)

        # 6 MED1: MED1 send an ICMPv6 Echo Request to the MED2 2002::GUA
        addr = self.nodes[MED2].get_addr("2002::/64")
        self.assertTrue(addr is not None)
        self.assertTrue(self.nodes[MED1].ping(addr))

        # Wait for sniffer got packets
        self.simulator.go(1)

        # 7 MED1: MED1 send an ICMPv6 Echo Request to the MED2 2003::GUA
        addr = self.nodes[MED2].get_addr("2003::/64")
        self.assertTrue(addr is not None)
        self.assertTrue(self.nodes[MED1].ping(addr))

        # Wait for sniffer got packets
        self.simulator.go(1)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        MED_1 = pv.vars['MED_1']
        MED_1_MLEID = pv.vars['MED_1_MLEID']
        MED_2 = pv.vars['MED_2']
        MED_2_MLEID = pv.vars['MED_2_MLEID']
        MED_1_GUA = list()
        MED_2_GUA = list()
        MM = pv.vars['MM_PORT']
        names = locals()

        for i in (1, 2):
            for addr in pv.vars['MED_%d_IPADDRS' % i]:
                for j in range(1, 4):
                    if addr.startswith(Bytes('200%d' % j)):
                        names['MED_' + str(i) + '_GUA'].append(addr)

        # Step 2: Leader is sending properly formatted MLE Advertisements.
        #         Advertisements MUST be sent with an IP hop limit of 255 to
        #         the Link-Local All Nodes multicast address (FF02::1).
        #         The following TLVs MUST be present in the MLE Advertisements:
        #             - Leader Data TLV
        #             - Route64 TLV
        #             - Source Address TLV

        pkts.filter_wpan_src64(LEADER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_ADVERTISEMENT).\
            filter(lambda p: {
                              LEADER_DATA_TLV,
                              ROUTE64_TLV,
                              SOURCE_ADDRESS_TLV
                              } <= set(p.mle.tlv.type) and\
                   p.ipv6.hlim == 255).\
            must_next()

        # Step 3: Send a ICMPv6 Echo Request to the MED_2 ML-EID
        #         The DUT MUST NOT send an Address Query Request
        #         MED_2 MUST respond with an ICMPv6 Echo Reply

        _pkt = pkts.filter_ipv6_src_dst(MED_1_MLEID, MED_2_MLEID).\
                    filter_ping_request().\
                    must_next()
        pkts.filter_wpan_src64(LEADER).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            must_not_next()
        pkts.filter_ipv6_src_dst(MED_2_MLEID, MED_1_MLEID).\
            filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            must_next()

        # Step 4-6: Send a ICMPv6 Echo Request to the MED_2 each GUA
        #           The DUT MUST NOT send an Address Query Request
        #           MED_2 MUST respond with an ICMPv6 Echo Reply

        for med_1_addr, med_2_addr in zip(MED_1_GUA, MED_2_GUA):
            _pkt = pkts.filter_ipv6_src_dst(med_1_addr, med_2_addr).\
                        filter_ping_request().\
                        must_next()
            pkts.filter_wpan_src64(LEADER).\
                filter_RLARMA().\
                filter_coap_request(ADDR_QRY_URI, port=MM).\
                must_not_next()
            pkts.filter_ipv6_src_dst(med_2_addr, med_1_addr).\
                filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
                must_next()


if __name__ == '__main__':
    unittest.main()
