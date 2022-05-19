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
import thread_cert
from pktverify.consts import WIRESHARK_OVERRIDE_PREFS, MLE_PARENT_REQUEST, MLE_PARENT_RESPONSE, MLE_CHILD_ID_REQUEST, MLE_CHILD_ID_RESPONSE, MLE_LINK_REQUEST, ADDR_SOL_URI, ADDR_NTF_URI, SOURCE_ADDRESS_TLV, MODE_TLV, TIMEOUT_TLV, CHALLENGE_TLV, RESPONSE_TLV, LINK_LAYER_FRAME_COUNTER_TLV, MLE_FRAME_COUNTER_TLV, ROUTE64_TLV, ADDRESS16_TLV, LEADER_DATA_TLV, NETWORK_DATA_TLV, TLV_REQUEST_TLV, SCAN_MASK_TLV, CONNECTIVITY_TLV, LINK_MARGIN_TLV, VERSION_TLV, ADDRESS_REGISTRATION_TLV, NL_MAC_EXTENDED_ADDRESS_TLV, NL_RLOC16_TLV, NL_STATUS_TLV, NL_TARGET_EID_TLV, NL_ML_EID_TLV, COAP_CODE_ACK
from pktverify.packet_verifier import PacketVerifier
from pktverify.null_field import nullField
from pktverify.bytes import Bytes

LEADER = 1
ROUTER1 = 2
BR = 3
MED = 17
DUT_REED = 18
ROUTER_SELECTION_JITTER = 1

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to validate that the DUT is able to generate
# Address Notification messages in response to Address Query messages.
#
#  - Build a topology that has a total of 16 active routers, including the Leader,
#    with no communication constraints and
#        - MED only allows Leader
#        - DUT only allows Router1
#        - DUT allows BR later as required in step 5.
#  - The Leader is configured as a DHCPv6 server for prefix 2001::
#  - The Border Router is configured as a SLAAC server for prefix 2002::
#
# Test Topology:
# -------------
#               MED
#                |
#  Router_15 - Leader
#      ...    /     \
#         Router_2  Router_1
#           [BR]       |
#                   REED(DUT)
#
# DUT Types:
# ----------
#  REED


class Cert_5_2_5_AddressQuery(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER1, BR, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, MED]
        },
        ROUTER1: {
            'name': 'ROUTER_1',
            'mode': 'rdn',
            'allowlist': [LEADER, DUT_REED]
        },
        BR: {
            'name': 'ROUTER_2',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        4: {
            'name': 'ROUTER_3',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        5: {
            'name': 'ROUTER_4',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        6: {
            'name': 'ROUTER_5',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        7: {
            'name': 'ROUTER_6',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        8: {
            'name': 'ROUTER_7',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        9: {
            'name': 'ROUTER_8',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        10: {
            'name': 'ROUTER_9',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        11: {
            'name': 'ROUTER_10',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        12: {
            'name': 'ROUTER_11',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        13: {
            'name': 'ROUTER_12',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        14: {
            'name': 'ROUTER_13',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        15: {
            'name': 'ROUTER_14',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        16: {
            'name': 'ROUTER_15',
            'mode': 'rdn',
            'allowlist': [LEADER]
        },
        MED: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'allowlist': [LEADER]
        },
        DUT_REED: {
            'name': 'REED',
            'mode': 'rdn',
            'allowlist': [ROUTER1]
        },
    }

    # override wireshark preferences with case needed parameters
    CASE_WIRESHARK_PREFS = WIRESHARK_OVERRIDE_PREFS
    CASE_WIRESHARK_PREFS['6lowpan.context1'] = '2001::/64'
    CASE_WIRESHARK_PREFS['6lowpan.context2'] = '2002::/64'

    def test(self):
        # 1. LEADER: DHCPv6 Server for prefix 2001::/64.
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')
        self.nodes[LEADER].add_prefix('2001::/64', 'pdros')
        self.nodes[LEADER].register_netdata()

        # 2. BR: SLAAC Server for prefix 2002::/64.
        self.nodes[BR].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[BR].get_state(), 'router')
        self.nodes[BR].add_prefix('2002::/64', 'paros')
        self.nodes[BR].register_netdata()

        # 3. Bring up remaining devices except DUT_REED.
        for i in range(2, 17):
            if i == BR:
                continue
            self.nodes[i].start()
            self.simulator.go(5)
            self.assertEqual(self.nodes[i].get_state(), 'router')

        self.nodes[MED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MED].get_state(), 'child')

        # 4. Bring up DUT_REED.
        self.nodes[DUT_REED].start()
        self.simulator.go(5)
        self.simulator.go(ROUTER_SELECTION_JITTER)

        # 5. Enable a link between the DUT and BR to create a one-way link.
        self.nodes[DUT_REED].add_allowlist(self.nodes[BR].get_addr64())
        self.nodes[BR].add_allowlist(self.nodes[DUT_REED].get_addr64())

        self.collect_ipaddrs()
        self.collect_rlocs()

        # 6. Verify DUT_REED would send Address Notification when ping to its
        # ML-EID.
        mleid = self.nodes[DUT_REED].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        self.assertTrue(self.nodes[MED].ping(mleid))

        # Wait for sniffer collecting packets
        self.simulator.go(1)

        # 7 & 8. Verify DUT_REED would send Address Notification when ping to
        # its 2001::EID and 2002::EID.
        flag2001 = 0
        flag2002 = 0
        for global_address in self.nodes[DUT_REED].get_ip6_address(config.ADDRESS_TYPE.GLOBAL):
            if global_address[0:4] == '2001':
                flag2001 += 1
            elif global_address[0:4] == '2002':
                flag2002 += 1
            else:
                raise "Error: Address is unexpected."
            self.assertTrue(self.nodes[MED].ping(global_address))

            # Wait for sniffer collecting packets
            self.simulator.go(1)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER_RLOC = pv.vars['LEADER_RLOC']
        LEADER_MLEID = pv.vars['LEADER_MLEID']
        ROUTER_1 = pv.vars['ROUTER_1']
        REED = pv.vars['REED']
        REED_MLEID = pv.vars['REED_MLEID']
        REED_RLOC = pv.vars['REED_RLOC']
        MED = pv.vars['MED']
        MED_MLEID = pv.vars['MED_MLEID']
        MM = pv.vars['MM_PORT']
        REED2001 = ''
        REED2002 = ''
        MED2001 = ''
        MED2002 = ''

        for addr in pv.vars['REED_IPADDRS']:
            if addr.startswith(Bytes('2001')):
                REED2001 = addr
            if addr.startswith(Bytes('2002')):
                REED2002 = addr

        for addr in pv.vars['MED_IPADDRS']:
            if addr.startswith(Bytes('2001')):
                MED2001 = addr
            if addr.startswith(Bytes('2002')):
                MED2002 = addr

        # Step 3: Verify topology is formed correctly except REED.

        for i in range(1, 16):
            with pkts.save_index():
                pv.verify_attached('ROUTER_%d' % i)

        # Step 4: REED attaches to Router_1 and MUST NOT attempt to become
        #         an active router by sending an Address Solicit Request

        pv.verify_attached('REED', 'ROUTER_1')
        pkts.filter_wpan_src64(REED).\
            filter_coap_request(ADDR_SOL_URI).\
            must_not_next()

        # Step 6: MED sends an ICMPv6 Echo Request from MED to REED using ML-EID.
        #         The DUT MUST send a properly formatted Address Notification message:
        #         CoAP Request URI-PATH
        #             CON POST coap://[<Address Query Source>]:MM/a/an
        #         CoAP Payload
        #             - Target EID TLV
        #             - RLOC16 TLV
        #             - ML-EID TLV
        #         The IPv6 Source address MUST be the RLOC of the originator
        #         The IPv6 Destination address MUST be the RLOC of the destination
        #         The DUT MUST send an ICMPv6 Echo Reply

        _pkt = pkts.filter_ipv6_src_dst(MED_MLEID, REED_MLEID).\
                    filter_ping_request().\
                    must_next()
        pkts.filter_ipv6_src_dst(REED_RLOC, LEADER_RLOC).\
            filter_coap_request(ADDR_NTF_URI, port=MM).\
            filter(lambda p: {
                              NL_TARGET_EID_TLV,
                              NL_RLOC16_TLV,
                              NL_ML_EID_TLV
                             } == set(p.thread_address.tlv.type)
                   ).\
            must_next()
        pkts.filter_ipv6_src_dst(REED_MLEID, MED_MLEID).\
            filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            must_next()

        # Step 7: MED sends an ICMPv6 Echo Request from MED to REED using 2001::EID.
        #         The DUT MUST send a properly formatted Address Notification message:
        #         CoAP Request URI-PATH
        #             CON POST coap://[<Address Query Source>]:MM/a/an
        #         CoAP Payload
        #             - Target EID TLV
        #             - RLOC16 TLV
        #             - ML-EID TLV
        #         The IPv6 Source address MUST be the RLOC of the originator
        #         The IPv6 Destination address MUST be the RLOC of the destination
        #         The DUT MUST send an ICMPv6 Echo Reply

        _pkt = pkts.filter_ipv6_src_dst(MED2001, REED2001).\
                    filter_ping_request().\
                    must_next()
        pkts.filter_ipv6_src_dst(REED_RLOC, LEADER_RLOC).\
            filter_coap_request(ADDR_NTF_URI, port=MM).\
            filter(lambda p: {
                              NL_TARGET_EID_TLV,
                              NL_RLOC16_TLV,
                              NL_ML_EID_TLV
                             } == set(p.thread_address.tlv.type)
                   ).\
            must_next()
        pkts.filter_ipv6_src_dst(REED2001, MED2001).\
            filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            must_next()

        # Step 8: MED sends an ICMPv6 Echo Request from MED to REED using 2002::EID.
        #         The DUT MUST send a properly formatted Address Notification message:
        #         CoAP Request URI-PATH
        #             CON POST coap://[<Address Query Source>]:MM/a/an
        #         CoAP Payload
        #             - Target EID TLV
        #             - RLOC16 TLV
        #             - ML-EID TLV
        #         The IPv6 Source address MUST be the RLOC of the originator
        #         The IPv6 Destination address MUST be the RLOC of the destination
        #         The DUT MUST send an ICMPv6 Echo Reply

        _pkt = pkts.filter_ipv6_src_dst(MED2002, REED2002).\
                    filter_ping_request().\
                    must_next()
        pkts.filter_ipv6_src_dst(REED_RLOC, LEADER_RLOC).\
            filter_coap_request(ADDR_NTF_URI, port=MM).\
            filter(lambda p: {
                              NL_TARGET_EID_TLV,
                              NL_RLOC16_TLV,
                              NL_ML_EID_TLV
                             } == set(p.thread_address.tlv.type)
                   ).\
            must_next()
        pkts.filter_ipv6_src_dst(REED2002, MED2002).\
            filter_ping_reply(identifier=_pkt.icmpv6.echo.identifier).\
            must_next()


if __name__ == '__main__':
    unittest.main()
