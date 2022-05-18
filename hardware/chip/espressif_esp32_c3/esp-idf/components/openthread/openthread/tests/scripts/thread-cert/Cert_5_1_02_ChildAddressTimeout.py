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
import mle
import thread_cert
from pktverify.consts import MLE_CHILD_ID_RESPONSE, ADDR_QRY_URI, ADDR_NTF_URI, NL_TARGET_EID_TLV
from pktverify.packet_verifier import PacketVerifier

LEADER = 1
ROUTER = 2
MED = 3
SED = 4

MTDS = [MED, SED]

# Test Purpose and Description:
# -----------------------------
# The purpose of the test case is to verify that when the timer reaches
# the value of the Timeout TLV sent by the Child, the Parent stops
# responding to Address Query on the Child's behalf
#
# Test Topology:
# --------------
# Leader
#    |
# Router
#   / \
# MED SED
#
# DUT Types:
# ----------
#  Router


class Cert_5_1_02_ChildAddressTimeout(thread_cert.TestCase):
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
            'allowlist': [LEADER, MED, SED]
        },
        MED: {
            'name': 'MED',
            'is_mtd': True,
            'mode': 'rn',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
            'allowlist': [ROUTER]
        },
        SED: {
            'name': 'SED',
            'is_mtd': True,
            'mode': 'n',
            'timeout': config.DEFAULT_CHILD_TIMEOUT,
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

        self.nodes[MED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[MED].get_state(), 'child')

        self.nodes[SED].start()
        self.simulator.go(5)
        self.assertEqual(self.nodes[SED].get_state(), 'child')

        self.collect_ipaddrs()

        med_mleid = self.nodes[MED].get_ip6_address(config.ADDRESS_TYPE.ML_EID)
        sed_mleid = self.nodes[SED].get_ip6_address(config.ADDRESS_TYPE.ML_EID)

        self.nodes[MED].stop()
        self.nodes[SED].stop()
        self.simulator.go(config.DEFAULT_CHILD_TIMEOUT + 5)
        self.assertFalse(self.nodes[LEADER].ping(med_mleid))
        self.assertFalse(self.nodes[LEADER].ping(sed_mleid))

    def verify(self, pv):
        pkts = pv.pkts
        pv.summary.show()

        LEADER = pv.vars['LEADER']
        ROUTER = pv.vars['ROUTER']
        MED = pv.vars['MED']
        MED_MLEID = pv.vars['MED_MLEID']
        SED = pv.vars['SED']
        SED_MLEID = pv.vars['SED_MLEID']
        MM = pv.vars['MM_PORT']

        # Step 1: Verify topology is formed correctly

        pv.verify_attached('ROUTER')

        pkts.filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(MED).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            must_next()

        pkts.filter_wpan_src64(ROUTER).\
            filter_wpan_dst64(SED).\
            filter_mle_cmd(MLE_CHILD_ID_RESPONSE).\
            must_next()

        # Step 2: Power off both devices and allow for the keep-alive timeout to expire
        # Step 3: The Leader sends an ICMPv6 Echo Request to MED and attempts to perform
        #         address resolution by sending an Address Query Request

        pkts.filter_wpan_src64(LEADER).\
            filter_RLARMA().\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            filter(lambda p: p.thread_address.tlv.type == [NL_TARGET_EID_TLV] and\
                   p.thread_address.tlv.target_eid == MED_MLEID).\
            must_next()

        # Step 4: Router MUST NOT respond with an Address Notification Message

        pkts.filter_wpan_src64(ROUTER).\
            filter_coap_request(ADDR_NTF_URI).\
            must_not_next()

        # Step 5: The Leader sends an ICMPv6 Echo Request to SED and attempts to perform
        #         address resolution by sending an Address Query Request

        pkts.filter_wpan_src64(LEADER).\
            filter_RLARMA().\
            filter(lambda p: p.thread_address.tlv.type == [NL_TARGET_EID_TLV] and\
                   p.thread_address.tlv.target_eid == SED_MLEID).\
            filter_coap_request(ADDR_QRY_URI, port=MM).\
            must_next()

        # Step 6: Router MUST NOT respond with an Address Notification Message

        pkts.filter_wpan_src64(ROUTER).\
            filter_coap_request(ADDR_NTF_URI).\
            must_not_next()


if __name__ == '__main__':
    unittest.main()
