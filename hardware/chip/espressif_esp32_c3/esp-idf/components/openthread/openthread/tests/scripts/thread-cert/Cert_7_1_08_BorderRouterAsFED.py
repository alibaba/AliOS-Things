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
import copy

import config
import thread_cert
from pktverify.consts import WIRESHARK_OVERRIDE_PREFS, MLE_DATA_RESPONSE, SVR_DATA_URI, NWD_6LOWPAN_ID_TLV, NL_RLOC16_TLV
from pktverify.packet_verifier import PacketVerifier
from pktverify.addrs import Ipv6Addr
from pktverify.null_field import nullField

LEADER = 1
ROUTER = 2
FED = 3

PREFIX_2001 = '2001::/64'
PREFIX_2002 = '2002::/64'

# Test Purpose and Description:
# -----------------------------
# The purpose of this test case is to verify that when global prefix information
# is set on the FED, the DUT properly disseminates the associated network data.
# It also verifies that the DUT sends revised server data information to the
# Leader when the FED is removed.
#
# Test Topology:
# -------------
#             FED
#              |
#           Router(DUT)
#              |
#           Leader
#
# DUT Types:
# ----------
# Router


class Cert_7_1_8_BorderRouterAsFED(thread_cert.TestCase):
    USE_MESSAGE_FACTORY = False

    TOPOLOGY = {
        LEADER: {
            'name': 'LEADER',
            'mode': 'rdn',
            'allowlist': [ROUTER]
        },
        ROUTER: {
            'name': 'ROUTER',
            'mode': 'rdn',
            'allowlist': [LEADER, FED]
        },
        FED: {
            'name': 'FED',
            'mode': 'rdn',
            'router_upgrade_threshold': 0,
            'allowlist': [ROUTER]
        },
    }
    # override wireshark preferences with case needed parameters
    CASE_WIRESHARK_PREFS = copy.deepcopy(WIRESHARK_OVERRIDE_PREFS)
    CASE_WIRESHARK_PREFS['6lowpan.context1'] = PREFIX_2001
    CASE_WIRESHARK_PREFS['6lowpan.context2'] = PREFIX_2002

    def test(self):
        self.nodes[LEADER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[LEADER].get_state(), 'leader')

        self.nodes[ROUTER].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[ROUTER].get_state(), 'router')

        self.nodes[FED].start()
        self.simulator.go(3)
        self.assertEqual(self.nodes[FED].get_state(), 'child')

        self.collect_rlocs()
        self.collect_rloc16s()
        self.collect_leader_aloc(LEADER)

        self.nodes[FED].add_prefix(PREFIX_2001, 'paros')
        self.nodes[FED].add_prefix(PREFIX_2002, 'paro')
        self.nodes[FED].register_netdata()
        self.simulator.go(10)

        self.nodes[FED].stop()
        self.simulator.go(250)

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        LEADER_RLOC = pv.vars['LEADER_RLOC']
        LEADER_ALOC = pv.vars['LEADER_ALOC']
        ROUTER = pv.vars['ROUTER']
        ROUTER_RLOC = pv.vars['ROUTER_RLOC']
        ROUTER_RLOC16 = pv.vars['ROUTER_RLOC16']
        FED = pv.vars['FED']
        FED_RLOC = pv.vars['FED_RLOC']
        FED_RLOC16 = pv.vars['FED_RLOC16']

        # Step 1: Ensure topology is formed correctly
        pv.verify_attached('ROUTER', 'LEADER')
        pv.verify_attached('FED', 'ROUTER', 'MTD')

        # Step 2: FED automatically sends a CoAP Server Data Notification
        #         message with the server’s information (Prefix, Border Router)
        #         to the Leader.
        pkts.filter_wpan_src64(FED).\
            filter_ipv6_dst(LEADER_ALOC).\
            filter_coap_request(SVR_DATA_URI).\
            filter(lambda p: {
                              Ipv6Addr(PREFIX_2001[:-3]),
                              Ipv6Addr(PREFIX_2002[:-3])
                             } == set(p.thread_nwd.tlv.prefix) and\
                   p.thread_nwd.tlv.border_router_16 == [FED_RLOC16, FED_RLOC16]
                   ).\
            must_next()

        # Step 3: Leader transmits a 2.04 Changed CoAP response to the DUT.
        #         And transmits multicast MLE Data Response with the new information
        #         collected, adding also 6LoWPAN ID TLV for the prefix set on FED.
        with pkts.save_index():
            pkts.filter_wpan_src64(LEADER).\
                filter_ipv6_dst(FED_RLOC).\
                filter_coap_ack(SVR_DATA_URI).\
                must_next()
        with pkts.save_index():
            pkts.filter_wpan_src64(LEADER).\
                filter_LLANMA().\
                filter_mle_cmd(MLE_DATA_RESPONSE).\
                filter(lambda p: {
                                  Ipv6Addr(PREFIX_2001[:-3]),
                                  Ipv6Addr(PREFIX_2002[:-3])
                                 } == set(p.thread_nwd.tlv.prefix) and\
                       NWD_6LOWPAN_ID_TLV in p.thread_nwd.tlv.type and\
                       p.thread_nwd.tlv.border_router.flag.p == [1, 1] and\
                       p.thread_nwd.tlv.border_router.flag.s == [1, 1] and\
                       p.thread_nwd.tlv.border_router.flag.r == [1, 1] and\
                       p.thread_nwd.tlv.border_router.flag.o == [1, 1] and\
                       p.thread_nwd.tlv.stable == [0, 1, 1, 1, 0, 0, 0]
                       ).\
                must_next()

        # Step 4: The DUT transmits multicast MLE Data Response with the new information
        #         collected, adding also 6LoWPAN ID TLV for the prefix set on FED.
        pkts.filter_wpan_src64(ROUTER).\
            filter_LLANMA().\
            filter_mle_cmd(MLE_DATA_RESPONSE).\
            filter(lambda p: {
                              Ipv6Addr(PREFIX_2001[:-3]),
                              Ipv6Addr(PREFIX_2002[:-3])
                             } == set(p.thread_nwd.tlv.prefix) and\
                   NWD_6LOWPAN_ID_TLV in p.thread_nwd.tlv.type and\
                   p.thread_nwd.tlv.border_router.flag.p == [1, 1] and\
                   p.thread_nwd.tlv.border_router.flag.s == [1, 1] and\
                   p.thread_nwd.tlv.border_router.flag.r == [1, 1] and\
                   p.thread_nwd.tlv.border_router.flag.o == [1, 1] and\
                   p.thread_nwd.tlv.stable == [0, 1, 1, 1, 0, 0, 0]
                   ).\
            must_next()

        # Step 6: The DUT notifies Leader of removed server’s (FED’s) RLOC16.
        #         The DUT MUST send a CoAP Server Data Notification frame
        #         to the Leader containing only the removed server’s RLOC16:
        #             CoAP Request URI
        #                 coap://[<Leader address>]:MM/a/sd
        #             CoAP Payload
        #                 RLOC16 TLV
        pkts.filter_wpan_src64(ROUTER).\
            filter_coap_request(SVR_DATA_URI).\
            filter(lambda p:
                   p.thread_address.tlv.type == [NL_RLOC16_TLV] and\
                   p.thread_address.tlv.rloc16 == FED_RLOC16
                  ).\
            must_next()


if __name__ == '__main__':
    unittest.main()
